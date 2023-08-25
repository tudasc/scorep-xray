/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022-2023,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  Implementation of the HIP runtime API event logging via HIP callbacks.
 */

#include <config.h>

#include "scorep_hip_callbacks.h"

#include <scorep/SCOREP_PublicTypes.h>

#include <SCOREP_InMeasurement.h>
#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_Filtering.h>
#include <SCOREP_Task.h>
#include <SCOREP_AcceleratorManagement.h>
#include <SCOREP_FastHashtab.h>
#include <SCOREP_Demangle.h>
#include <SCOREP_AllocMetric.h>

#include <scorep_system_tree.h>

#include <UTILS_Atomic.h>
#include <UTILS_Mutex.h>
#define SCOREP_DEBUG_MODULE_NAME HIP
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <roctracer/roctracer_hip.h>
#include <roctracer/roctracer_roctx.h>
#include <rocm_smi/rocm_smi.h>

#include <jenkins_hash.h>

#include "scorep_hip.h"
#include "scorep_hip_confvars.h"

// Macro to check ROC-tracer calls status
// Note that this applies only to calls returning `roctracer_status_t`!
#define SCOREP_ROCTRACER_CALL( call ) \
    do { \
        roctracer_status_t err = call; \
        if ( err != ROCTRACER_STATUS_SUCCESS ) { \
            UTILS_FATAL( "Call '%s' failed with: %s", #call, roctracer_error_string() ); \
        } \
    } while ( 0 )

static SCOREP_SourceFileHandle hip_file_handle;

static SCOREP_AllocMetric*    host_alloc_metric;
static SCOREP_AttributeHandle attribute_allocation_size;
static SCOREP_AttributeHandle attribute_deallocation_size;

static SCOREP_ParameterHandle callsite_id_parameter;

static uint32_t local_rank_counter;

SCOREP_RmaWindowHandle           scorep_hip_window_handle               = SCOREP_INVALID_RMA_WINDOW;
SCOREP_InterimCommunicatorHandle scorep_hip_interim_communicator_handle = SCOREP_INVALID_INTERIM_COMMUNICATOR;

uint64_t  scorep_hip_global_location_count = 0;
uint64_t* scorep_hip_global_location_ids   = NULL;

static uint64_t previous_activity_scorep_time;
static uint64_t previous_activity_hip_time;

/* Our own event phase, whiche goes until subsystem_end to flush the activity buffers */
static volatile sig_atomic_t event_phase;

/************************** Forward declarations ******************************/

/* Assign the current host location a rank inside the HIP specific RMA window/communicator */
static void
activate_host_location( void );

static uint32_t
get_host_rank( SCOREP_Location* hostLocation );

static void
get_synced_timestamps( uint64_t* scorepTime,
                       uint64_t* hipTime );

/************************** HIP API function table ****************************/

typedef uint32_t            api_region_table_key_t;
typedef SCOREP_RegionHandle api_region_table_value_t;

#define API_REGION_TABLE_HASH_EXPONENT 7

static inline uint32_t
api_region_table_bucket_idx( api_region_table_key_t key )
{
    uint32_t hashvalue = jenkins_hash( &key, sizeof( key ), 0 );

    return hashvalue & hashmask( API_REGION_TABLE_HASH_EXPONENT );
}

static inline bool
api_region_table_equals( api_region_table_key_t key1,
                         api_region_table_key_t key2 )
{
    return key1 == key2;
}

static inline void*
api_region_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
api_region_table_free_chunk( void* chunk )
{
}

typedef struct
{
    SCOREP_RegionType region_type;
    const char*       group_name;
    bool              allow_filter;
} api_region_table_infos;

static inline api_region_table_value_t
api_region_table_value_ctor( api_region_table_key_t* key,
                             void*                   ctorData )
{
    api_region_table_infos* infos = ctorData;
    // `roctracer_op_string` returns at least in one case a static string,
    // thus we cannot reliably free the result
    const char* function_name = roctracer_op_string( ACTIVITY_DOMAIN_HIP_API, *key, 0 );

    SCOREP_RegionHandle region = SCOREP_FILTERED_REGION;
    if ( !infos->allow_filter
         || !SCOREP_Filtering_MatchFunction( function_name, NULL ) )
    {
        region = SCOREP_Definitions_NewRegion( function_name, NULL,
                                               hip_file_handle,
                                               0, 0,
                                               SCOREP_PARADIGM_HIP,
                                               infos->region_type );

        SCOREP_RegionHandle_SetGroup( region, infos->group_name );
    }

    return region;
}

/* nPairsPerChunk: 4+4 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( api_region_table,
                             15,
                             hashsize( API_REGION_TABLE_HASH_EXPONENT ) );

/************************** ROCm SMI devices **********************************/

typedef uint64_t smi_device_table_key_t;
typedef int      smi_device_table_value_t;

#define SMI_DEVICE_TABLE_HASH_EXPONENT 3

static inline uint32_t
smi_device_table_bucket_idx( smi_device_table_key_t key )
{
    return jenkins_hash( &key, sizeof( key ), 0 ) & hashmask( SMI_DEVICE_TABLE_HASH_EXPONENT );
}

static inline bool
smi_device_table_equals( smi_device_table_key_t key1,
                         smi_device_table_key_t key2 )
{
    return key1 == key2;
}

static inline void*
smi_device_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
smi_device_table_free_chunk( void* chunk )
{
}

static inline smi_device_table_value_t
smi_device_table_value_ctor( smi_device_table_key_t* key,
                             void*                   ctorData )
{
    return *( int* )ctorData;
}

/* nPairsPerChunk: 8+4 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( smi_device_table,
                             10,
                             hashsize( SMI_DEVICE_TABLE_HASH_EXPONENT ) );

static void
enumerate_smi_devices( void )
{
/* Macro to check ROCm-SMI calls status
 * Note that this applies only to calls returning `rsmi_status_t`! */
#define SCOREP_ROCMSMI_CALL( call ) \
    do { \
        rsmi_status_t err = call; \
        if ( err != RSMI_STATUS_SUCCESS ) { \
            const char* error_string; \
            rsmi_status_string( err, &error_string ); \
            UTILS_WARNING( "Call '%s' failed with: %s", #call, error_string ); \
            return; \
        } \
    } while ( 0 )

    SCOREP_ROCMSMI_CALL( rsmi_init( 0 ) );

    uint32_t num_devices;
    SCOREP_ROCMSMI_CALL( rsmi_num_monitor_devices( &num_devices ) );

    for ( uint32_t i = 0; i < num_devices; ++i )
    {
        uint64_t uuid;
        SCOREP_ROCMSMI_CALL( rsmi_dev_unique_id_get( i, &uuid ) );

        smi_device_table_value_t ignored;
        smi_device_table_get_and_insert( uuid, &i, &ignored );
    }

    SCOREP_ROCMSMI_CALL( rsmi_shut_down() );

#undef SCOREP_ROCMSMI_CALL
}

static int
get_smi_device( int       deviceId,
                uint64_t* uuidOut )
{
    UTILS_ASSERT( uuidOut );

    /* HIP returns only the hex string part of the UUID "GPI-XXXXXXXXXXXXXXXX"
     * but without a terminating 0, hence the union with one more byte.
     * https://github.com/RadeonOpenCompute/ROCR-Runtime/blob/master/src/inc/hsa_ext_amd.h#L291-L300 */
    union
    {
        hipUUID hip_uuid;
        char    terminated_bytes[ sizeof( hipUUID ) + 1 ];
    } uuid_bytes;
    hipDeviceGetUuid( &uuid_bytes.hip_uuid, deviceId );
    uuid_bytes.terminated_bytes[ sizeof( hipUUID ) ] = '\0';
    uint64_t uuid = strtoull( uuid_bytes.terminated_bytes, NULL, 16 );

    int smi_device;
    if ( smi_device_table_get( uuid, &smi_device ) )
    {
        *uuidOut = uuid;
        return smi_device;
    }

    return deviceId;
}

/************************** HIP devices ***************************************/

typedef struct
{
    // protects concurrent creation of definitions for this device
    UTILS_Mutex                 lock;
    int                         device_id;
    int                         smi_device_id;
    SCOREP_SystemTreeNodeHandle system_tree_node;
    SCOREP_LocationGroupHandle  location_group;
    uint32_t                    stream_counter;
    SCOREP_AllocMetric*         alloc_metric;
    hipDeviceProp_t             properties;
    uint64_t                    uuid;
} scorep_hip_device;

typedef int                device_table_key_t;
typedef scorep_hip_device* device_table_value_t;

#define DEVICE_TABLE_HASH_EXPONENT 3

static inline uint32_t
device_table_bucket_idx( device_table_key_t key )
{
    return jenkins_hash( &key, sizeof( key ), 0 ) & hashmask( DEVICE_TABLE_HASH_EXPONENT );
}

static inline bool
device_table_equals( device_table_key_t key1,
                     device_table_key_t key2 )
{
    return key1 == key2;
}

static inline void*
device_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
device_table_free_chunk( void* chunk )
{
}

static inline device_table_value_t
device_table_value_ctor( device_table_key_t* key,
                         void*               ctorData )
{
    scorep_hip_device* device = SCOREP_Memory_AllocForMisc( sizeof( *device ) );
    memset( device, 0, sizeof( *device ) );

    device->device_id     = *key;
    device->smi_device_id = get_smi_device( device->device_id, &device->uuid );

    hipGetDeviceProperties( &device->properties, device->device_id );

    return device;
}

/* nPairsPerChunk: 4+8 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( device_table,
                             10,
                             hashsize( DEVICE_TABLE_HASH_EXPONENT ) );

static void
create_device( device_table_key_t deviceId )
{
    device_table_value_t ignored = NULL;
    device_table_get_and_insert( deviceId, NULL, &ignored );
}

static void
define_device( scorep_hip_device* device )
{
    char buffer[ 80 ];
    snprintf( buffer, sizeof( buffer ), "%d", device->smi_device_id );
    device->system_tree_node = SCOREP_Definitions_NewSystemTreeNode(
        SCOREP_GetSystemTreeNodeHandleForSharedMemory(),
        SCOREP_SYSTEM_TREE_DOMAIN_ACCELERATOR_DEVICE,
        "ROCm Device",
        buffer );

    if ( device->uuid != 0 )
    {
        /* ROCm uses "GPU-" as prefix for the UUID
         * https://github.com/RadeonOpenCompute/ROCR-Runtime/blob/master/src/core/runtime/amd_gpu_agent.cpp#L993 */
        char uuid_buffer[ 22 ];
        snprintf( uuid_buffer, sizeof( uuid_buffer ), "GPU-%016" PRIx64, device->uuid );
        SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node, "UUID", uuid_buffer );
    }

    if ( strnlen( device->properties.name, sizeof( device->properties.name ) ) < sizeof( device->properties.name ) )
    {
        SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                                 "Device name",
                                                 device->properties.name );
    }

    if ( strnlen( device->properties.gcnArchName, sizeof( device->properties.gcnArchName ) ) < sizeof( device->properties.gcnArchName ) )
    {
        SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                                 "AMD GCN architecture name",
                                                 device->properties.gcnArchName );
    }

    snprintf( buffer, sizeof( buffer ), "%zu", device->properties.totalGlobalMem );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Size of global memory region (in bytes)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%zu", device->properties.sharedMemPerBlock );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Size of shared memory region (in bytes)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%zu", device->properties.totalConstMem );
    /* Documentation looks wrong,
     * see https://github.com/ROCm-Developer-Tools/HIP/issues/3035 */
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.regsPerBlock );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Registers per block",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.warpSize );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Warp size",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.maxThreadsPerBlock );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Maximum work items per work group or workgroup max size",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d, %d, %d",
              device->properties.maxThreadsDim[ 0 ],
              device->properties.maxThreadsDim[ 1 ],
              device->properties.maxThreadsDim[ 2 ] );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Maximum number of threads in each dimension (XYZ) of a block",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d, %d, %d",
              device->properties.maxGridSize[ 0 ],
              device->properties.maxGridSize[ 1 ],
              device->properties.maxGridSize[ 2 ] );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Maximum grid dimensions (XYZ)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.clockRate );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Maximum clock frequency of the multi-processors (in khz)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.memoryClockRate );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Max global memory clock frequency (in khz)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.clockInstructionRate );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Timer clock frequency (in khz)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.memoryBusWidth );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Global memory bus width (in bits)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.isMultiGpuBoard );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Is multi-GPU board (1 if yes, 0 if not)",
                                             buffer );


    snprintf( buffer, sizeof( buffer ), "%d", device->properties.canMapHostMemory );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Device can map host memory (1 if yes, 0 if not)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.concurrentKernels );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Device can possibly execute multiple kernels concurrently (1 if yes, 0 if not)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.multiProcessorCount );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Number of multi-processors (compute units)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.l2CacheSize );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "L2 cache size",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device->properties.maxThreadsPerMultiProcessor );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Maximum resident threads per multi-processor",
                                             buffer );

    SCOREP_SystemTreeNode_AddPCIProperties( device->system_tree_node,
                                            device->properties.pciDomainID,
                                            device->properties.pciBusID,
                                            device->properties.pciDeviceID,
                                            UINT8_MAX );

    /* ROCm/HIP does not have a "context", we add one per device. The "ID"
     * is irrelevant regarding unification. But as this is already the
     * software layer, we use the HIP device ID, not the ROCm SMI device index.
     */
    snprintf( buffer, sizeof( buffer ), "HIP Context %d", device->device_id );
    device->location_group = SCOREP_AcceleratorMgmt_CreateContext(
        device->system_tree_node,
        buffer );

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_MALLOC )
    {
        snprintf( buffer, 80, "HIP Context %d Memory", device->device_id );
        SCOREP_AllocMetric_NewScoped( buffer, device->location_group, &device->alloc_metric );
    }
}

static scorep_hip_device*
get_device( device_table_key_t deviceId )
{
    device_table_value_t device = NULL;
    if ( !device_table_get( deviceId, &device ) )
    {
        UTILS_BUG( "Unknown HIP device %d. Not created in init", deviceId );
    }

    if ( device->system_tree_node == SCOREP_INVALID_SYSTEM_TREE_NODE )
    {
        UTILS_MutexLock( &device->lock );
        if ( device->system_tree_node == SCOREP_INVALID_SYSTEM_TREE_NODE )
        {
            define_device( device );
        }
        UTILS_MutexUnlock( &device->lock );
    }

    return device;
}

/************************** HIP streams ***************************************/

typedef struct
{
    int              device_id;
    uint64_t         stream_id;
    uint32_t         stream_seq;
    SCOREP_Location* device_location;
    uint32_t         local_rank;
} scorep_hip_stream;

/**
 * Acts more like a union. If @p stream_id is 0, then only the device is the key.
 * else the device is ignored and must be 0.
 */
typedef struct
{
    uint64_t stream_id;
    int      device_id;
} stream_table_key_t;
typedef scorep_hip_stream* stream_table_value_t;

#define STREAM_TABLE_HASH_EXPONENT 6

static inline uint32_t
stream_table_bucket_idx( stream_table_key_t key )
{
    uint32_t hashvalue = jenkins_hash( &key.device_id, sizeof( key.device_id ), 0 );
    hashvalue = jenkins_hash( &key.stream_id, sizeof( key.stream_id ), hashvalue );

    return hashvalue & hashmask( STREAM_TABLE_HASH_EXPONENT );
}

static inline bool
stream_table_equals( stream_table_key_t key1,
                     stream_table_key_t key2 )
{
    if ( key1.stream_id != key2.stream_id )
    {
        return false;
    }

    /* Same stream, when both are 0 compare the devices, else its the
     * "same" stream */
    if ( key1.stream_id != 0 )
    {
        return true;
    }

    return key1.device_id == key2.device_id;
}

static inline void*
stream_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
stream_table_free_chunk( void* chunk )
{
}

struct stream_table_ctor_data
{
    int device_id;
};

static inline stream_table_value_t
stream_table_value_ctor( stream_table_key_t* key,
                         void*               ctorData )
{
    struct stream_table_ctor_data* data = ctorData;
    UTILS_BUG_ON( !data, "Missing ctor data" );

    /* device ID in the key is only valid for NULL-streams */
    scorep_hip_device* device     = get_device( data->device_id );
    uint32_t           stream_seq = 0;
    if ( key->stream_id != 0 )
    {
        /* Ensure that stream_seq == 0 is always given to the NULL-stream */
        stream_seq = UTILS_Atomic_AddFetch_uint32( &device->stream_counter, 1,
                                                   UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    }

    UTILS_DEBUG( "Creating stream %p/%d -> [%d:%u]", ( void* )key->stream_id, key->device_id, data->device_id, stream_seq );

    scorep_hip_stream* stream = SCOREP_Memory_AllocForMisc( sizeof( *stream ) );
    stream->device_id  = key->device_id;
    stream->stream_id  = key->stream_id;
    stream->stream_seq = stream_seq;

    char thread_name[ 32 ];
    /* This is the software layer, thus we use the HIP device ID, not the
     * ROCm SMI device index. See the comment on "HIP Context".
     */
    snprintf( thread_name, sizeof( thread_name ), "HIP[%d:%u]", stream->device_id, stream->stream_seq );

    stream->device_location =
        SCOREP_Location_CreateNonCPULocation( SCOREP_Location_GetCurrentCPULocation(),
                                              SCOREP_LOCATION_TYPE_GPU,
                                              SCOREP_PARADIGM_HIP,
                                              thread_name,
                                              device->location_group );

    /* streams will have a lower rank than threads, is this ok? */
    stream->local_rank = UTILS_Atomic_FetchAdd_uint32(
        &local_rank_counter, 1,
        UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );

    /* Only valid for non-NULL-streams */
    if ( key->stream_id != 0 )
    {
        unsigned int stream_flags;
        hipStreamGetFlags( ( hipStream_t )key->stream_id, &stream_flags );
        if ( stream_flags & hipStreamNonBlocking )
        {
            SCOREP_Location_AddLocationProperty( stream->device_location,
                                                 "hipStreamNonBlocking",
                                                 0, "1" );
        }

        int stream_priority;
        hipStreamGetPriority( ( hipStream_t )key->stream_id, &stream_priority );
        SCOREP_Location_AddLocationProperty( stream->device_location,
                                             "hipStreamPriority",
                                             16, "%d", stream_priority );
    }
    else
    {
        SCOREP_Location_AddLocationProperty( stream->device_location,
                                             "hipNullStream",
                                             0, "1" );
    }

    return stream;
}

/* nPairsPerChunk: 16+8 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( stream_table,
                             5,
                             hashsize( STREAM_TABLE_HASH_EXPONENT ) )

static void
create_stream( uint64_t streamId )
{
    stream_table_key_t            key  = { .stream_id = streamId, .device_id = 0 };
    struct stream_table_ctor_data data = { 0, };
    hipGetDevice( &data.device_id );

    if ( streamId == 0 )
    {
        // only the NULL-stream depends on the current device
        key.device_id = data.device_id;
    }

    stream_table_value_t ignored = NULL;
    if ( !stream_table_get_and_insert( key, &data, &ignored ) )
    {
        UTILS_WARNING( "Duplicate stream %p/%d", ( void* )key.stream_id, key.device_id );
    }
}

static scorep_hip_stream*
get_stream( uint64_t streamId )
{
    stream_table_key_t key = { .stream_id = streamId, .device_id = 0 };
    if ( streamId == 0 )
    {
        hipGetDevice( &key.device_id );
    }

    scorep_hip_stream* stream = NULL;
    if ( !stream_table_get( key, &stream ) && streamId == 0 )
    {
        struct stream_table_ctor_data data =
        {
            .device_id = key.device_id
        };
        /* the NULL-stream is created implicitly at hipInit, which itself
         * is called implicitly without triggering the callback. */
        stream_table_get_and_insert( key, &data, &stream );
    }
    UTILS_BUG_ON( stream == NULL, "Unknown stream %p/%d", ( void* )key.stream_id, key.device_id );

    return stream;
}

/************************** HIP kernel table **********************************/

typedef struct
{
    uint32_t    hash_value;
    const char* string_value;
} kernel_table_key_t;
typedef SCOREP_RegionHandle kernel_table_value_t;

#define KERNEL_TABLE_HASH_EXPONENT 8

static inline uint32_t
kernel_table_bucket_idx( kernel_table_key_t key )
{
    return key.hash_value & hashmask( KERNEL_TABLE_HASH_EXPONENT );
}

static inline bool
kernel_table_equals( kernel_table_key_t key1,
                     kernel_table_key_t key2 )
{
    return key1.hash_value == key2.hash_value && strcmp( key1.string_value, key2.string_value ) == 0;
}

static inline void*
kernel_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
kernel_table_free_chunk( void* chunk )
{
}

static inline const char*
demangle( char const** mangled )
{
    const char* demangled = SCOREP_Demangle( *mangled, SCOREP_DEMANGLE_DEFAULT );
    if ( !demangled )
    {
        demangled = *mangled;
        *mangled  = NULL;
    }

    return demangled;
}

static inline kernel_table_value_t
kernel_table_value_ctor( kernel_table_key_t* key,
                         void*               ctorData )
{
    const char*         kernel_name_mangled   = key->string_value;
    const char*         kernel_name_demangled = demangle( &kernel_name_mangled );
    SCOREP_RegionHandle new_region            = SCOREP_Definitions_NewRegion( kernel_name_demangled,
                                                                              kernel_name_mangled,
                                                                              hip_file_handle,
                                                                              0, 0,
                                                                              SCOREP_PARADIGM_HIP,
                                                                              SCOREP_REGION_KERNEL );

    SCOREP_RegionHandle_SetGroup( new_region, "HIP_KERNEL" );

    if ( kernel_name_mangled )
    {
        free( ( char* )kernel_name_demangled );
    }

    /* the mangled named is the key */
    key->string_value = SCOREP_RegionHandle_GetCanonicalName( new_region );

    return new_region;
}

/* nPairsPerChunk: 16+4 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( kernel_table,
                             6,
                             hashsize( KERNEL_TABLE_HASH_EXPONENT ) );

static SCOREP_RegionHandle
get_kernel_region_by_name( const char* kernelName )
{
    size_t             kernel_name_length = strlen( kernelName );
    kernel_table_key_t key                = {
        .hash_value   = jenkins_hash( kernelName, kernel_name_length, 0 ),
        .string_value = kernelName
    };

    SCOREP_RegionHandle new_region = SCOREP_INVALID_REGION;
    if ( kernel_table_get_and_insert( key, NULL, &new_region ) )
    {
        UTILS_DEBUG( "Added region for %s", kernelName );
    }

    return new_region;
}

/************************** HIP correlations **********************************/

typedef struct
{
    SCOREP_Location* host_origin_location;
    uint32_t         cid;
    union
    {
        struct
        {
            scorep_hip_stream*  stream;
            SCOREP_RegionHandle kernel_region;
            uint32_t            callsite_hash;
        } launch;
        struct
        {
            SCOREP_AllocMetric* metric;
            void*               allocation;
        } free;
        struct
        {
            scorep_hip_stream* stream;
            uint64_t           size;
            hipMemcpyKind      kind;
        } memcpy;
    } payload;
} correlation_entry;

typedef uint64_t           correlation_table_key_t;
typedef correlation_entry* correlation_table_value_t;

#define CORRELATION_TABLE_HASH_EXPONENT 10

typedef struct free_list_entry
{
    struct free_list_entry* next;
} free_list_entry;
static free_list_entry* correlation_entry_free_list;
static UTILS_Mutex      correlation_entry_free_list_mutex;

static inline uint32_t
correlation_table_bucket_idx( correlation_table_key_t key )
{
    uint32_t hashvalue = jenkins_hash( &key, sizeof( key ), 0 );

    return hashvalue & hashmask( CORRELATION_TABLE_HASH_EXPONENT );
}

static inline bool
correlation_table_equals( correlation_table_key_t key1,
                          correlation_table_key_t key2 )
{
    return key1 == key2;
}

static inline void*
correlation_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
correlation_table_free_chunk( void* chunk )
{
}

static inline correlation_table_value_t
correlation_table_value_ctor( correlation_table_key_t* key,
                              void*                    ctorData )
{
    UTILS_MutexLock( &correlation_entry_free_list_mutex );

    correlation_entry* correlation = ( correlation_entry* )correlation_entry_free_list;
    if ( correlation )
    {
        correlation_entry_free_list = correlation_entry_free_list->next;
    }
    else
    {
        correlation = SCOREP_Memory_AllocForMisc( sizeof( *correlation ) );
    }

    UTILS_MutexUnlock( &correlation_entry_free_list_mutex );

    memset( correlation, 0, sizeof( *correlation ) );

    return correlation;
}

static inline void
correlation_table_value_dtor( correlation_table_key_t   key,
                              correlation_table_value_t value )
{
    free_list_entry* unused_object = ( free_list_entry* )value;

    UTILS_MutexLock( &correlation_entry_free_list_mutex );

    unused_object->next         = correlation_entry_free_list;
    correlation_entry_free_list = unused_object;

    UTILS_MutexUnlock( &correlation_entry_free_list_mutex );
}

/* nPairsPerChunk: 8+8 bytes per pair, 8 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_NON_MONOTONIC( correlation_table,
                                 7,
                                 hashsize( CORRELATION_TABLE_HASH_EXPONENT ) );

static correlation_entry*
get_correlation_entry( uint64_t correlationId )
{
    correlation_entry* correlation;
    if ( !correlation_table_get( correlationId, &correlation ) )
    {
        return NULL;
    }

    return correlation;
}

static correlation_entry*
create_correlation_entry( uint64_t correlationId,
                          uint32_t cid )
{
    correlation_entry* correlation = NULL;
    if ( !correlation_table_get_and_insert( correlationId, NULL, &correlation ) )
    {
        UTILS_WARNING( "Duplicate correlation entry for ID %" PRIu64, correlationId );
    }

    correlation->host_origin_location = SCOREP_Location_GetCurrentCPULocation();
    correlation->cid                  = cid;

    return correlation;
}

/*************************** ROCTX user regions *******************************/

typedef struct
{
    uint32_t    hash_value;
    const char* string_value;
} user_region_table_key_t;
typedef SCOREP_RegionHandle user_region_table_value_t;

#define USER_REGION_TABLE_HASH_EXPONENT 8

static inline uint32_t
user_region_table_bucket_idx( user_region_table_key_t key )
{
    return key.hash_value & hashmask( USER_REGION_TABLE_HASH_EXPONENT );
}

static inline bool
user_region_table_equals( user_region_table_key_t key1,
                          user_region_table_key_t key2 )
{
    return key1.hash_value == key2.hash_value && strcmp( key1.string_value, key2.string_value ) == 0;
}

static inline void*
user_region_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
user_region_table_free_chunk( void* chunk )
{
}

static inline user_region_table_value_t
user_region_table_value_ctor( user_region_table_key_t* key,
                              void*                    ctorData )
{
    SCOREP_RegionHandle new_region;

    if ( SCOREP_Filtering_MatchFunction( key->string_value, NULL ) )
    {
        new_region = SCOREP_FILTERED_REGION;

        SCOREP_StringHandle region_name = SCOREP_Definitions_NewString( key->string_value );
        key->string_value = SCOREP_StringHandle_Get( region_name );
    }
    else
    {
        new_region = SCOREP_Definitions_NewRegion( key->string_value, NULL,
                                                   hip_file_handle,
                                                   0, 0,
                                                   SCOREP_PARADIGM_HIP,
                                                   SCOREP_REGION_USER );

        SCOREP_RegionHandle_SetGroup( new_region, "ROCTX" );

        key->string_value = SCOREP_RegionHandle_GetName( new_region );
    }

    return new_region;
}

/* nPairsPerChunk: 16+4 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( user_region_table,
                             5,
                             hashsize( USER_REGION_TABLE_HASH_EXPONENT ) );

static void
push_user_region( const char* name )
{
    size_t                  name_length = strlen( name );
    user_region_table_key_t key         = {
        .hash_value   = jenkins_hash( name, name_length, 0 ),
        .string_value = name
    };

    SCOREP_RegionHandle region = SCOREP_INVALID_REGION;
    if ( user_region_table_get_and_insert( key, NULL, &region ) )
    {
        UTILS_DEBUG( "Added ROCTX region for %s", name );
    }

    if ( region != SCOREP_FILTERED_REGION )
    {
        SCOREP_EnterRegion( region );
    }
    else
    {
        SCOREP_Task_Enter( SCOREP_Location_GetCurrentCPULocation(), region );
    }
}

static void
pop_user_region( void )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    UTILS_ASSERT( location != NULL );

    scorep_hip_cpu_location_data* location_data = SCOREP_Location_GetSubsystemData(
        location, scorep_hip_subsystem_id );

    SCOREP_RegionHandle region_handle =
        SCOREP_Task_GetTopRegion( SCOREP_Task_GetCurrentTask( location ) );
    UTILS_ASSERT( region_handle != SCOREP_INVALID_REGION );

    if ( region_handle != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( region_handle );
    }
    else
    {
        SCOREP_Task_Exit( location );
    }
}

// Runtime API callback function
static void
api_region_enter( uint32_t          cid,
                  SCOREP_RegionType regionType,
                  const char*       groupName,
                  bool              allowFilter )
{
    api_region_table_infos infos = {
        .region_type  = regionType,
        .group_name   = groupName,
        .allow_filter = allowFilter
    };
    SCOREP_RegionHandle    region_handle = SCOREP_INVALID_REGION;
    api_region_table_get_and_insert( cid, &infos, &region_handle );

    /********** write enter record for HIP driver API **********/
    if ( region_handle != SCOREP_FILTERED_REGION )
    {
        SCOREP_EnterRegion( region_handle );
    }
}

static void
api_region_exit( uint32_t cid )
{
    SCOREP_RegionHandle region_handle = SCOREP_INVALID_REGION;
    bool                known         = api_region_table_get( cid, &region_handle );
    UTILS_BUG_ON( !known, "Exit after Enter for unknown region." );

    /********** write exit record for HIP driver API **********/
    if ( region_handle != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( region_handle );
    }
}

static void
api_cb( uint32_t    domain,
        uint32_t    cid,
        const void* callbackData,
        void*       arg )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_BUG_ON( domain != ACTIVITY_DOMAIN_HIP_API, "Only HIP domain handled." );

    const hip_api_data_t* data = ( const hip_api_data_t* )callbackData;

    if ( data->phase == ACTIVITY_API_PHASE_ENTER )
    {
        api_region_enter( cid, SCOREP_REGION_WRAPPER, "HIP_API", !!arg );
    }

    if ( data->phase == ACTIVITY_API_PHASE_EXIT )
    {
        api_region_exit( cid );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

static void
stream_cb( uint32_t    domain,
           uint32_t    cid,
           const void* callbackData,
           void*       arg )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_BUG_ON( domain != ACTIVITY_DOMAIN_HIP_API, "Only HIP domain handled." );

    const hip_api_data_t* data = ( const hip_api_data_t* )callbackData;

    if ( data->phase == ACTIVITY_API_PHASE_ENTER )
    {
        api_region_enter( cid, SCOREP_REGION_WRAPPER, "HIP_API", false );
    }

    if ( data->phase == ACTIVITY_API_PHASE_EXIT )
    {
        switch ( cid )
        {
            case HIP_API_ID_hipInit:
                create_stream( 0 );
                break;
            case HIP_API_ID_hipStreamCreate:
                if ( data->args.hipStreamCreate.stream )
                {
                    create_stream( ( uint64_t )( *data->args.hipStreamCreate.stream ) );
                }
                break;
            case HIP_API_ID_hipStreamCreateWithFlags:
                if ( data->args.hipStreamCreateWithFlags.stream )
                {
                    create_stream( ( uint64_t )( *data->args.hipStreamCreateWithFlags.stream ) );
                }
                break;
            case HIP_API_ID_hipStreamCreateWithPriority:
                if ( data->args.hipStreamCreateWithPriority.stream )
                {
                    create_stream( ( uint64_t )( *data->args.hipStreamCreateWithPriority.stream ) );
                }
                break;

            case HIP_API_ID_hipExtStreamCreateWithCUMask:
                if ( data->args.hipExtStreamCreateWithCUMask.stream )
                {
                    create_stream( ( uint64_t )( *data->args.hipExtStreamCreateWithCUMask.stream ) );
                }
                break;

            default:
                UTILS_BUG( "Unhandled stream call" );
        }

        api_region_exit( cid );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

static void
kernel_cb( uint32_t    domain,
           uint32_t    cid,
           const void* callbackData,
           void*       arg )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    // Early exit if we're already in measurement
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_BUG_ON( domain != ACTIVITY_DOMAIN_HIP_API, "Only HIP domain handled." );

    const hip_api_data_t* data = ( const hip_api_data_t* )callbackData;

    if ( data->phase == ACTIVITY_API_PHASE_ENTER )
    {
        api_region_enter( cid, SCOREP_REGION_KERNEL_LAUNCH, "HIP_API", false );

        correlation_entry* e = create_correlation_entry( data->correlation_id, cid );
        if ( scorep_hip_features & SCOREP_HIP_FEATURE_KERNEL_CALLSITE )
        {
            e->payload.launch.callsite_hash = SCOREP_Task_GetRegionStackHash( SCOREP_Task_GetCurrentTask( SCOREP_Location_GetCurrentCPULocation() ) );
            // other parameters are only valid in the EXIT phase

            SCOREP_TriggerParameterUint64( callsite_id_parameter, e->payload.launch.callsite_hash );
        }
    }

    // Only store the correlation record on exit
    if ( data->phase == ACTIVITY_API_PHASE_EXIT )
    {
        correlation_entry* e = get_correlation_entry( data->correlation_id );
        if ( e )
        {
            const char* kernel_name = NULL;
            uint64_t    stream      = 0;
            switch ( cid )
            {
                case HIP_API_ID_hipModuleLaunchKernel:
                    kernel_name = hipKernelNameRef( data->args.hipModuleLaunchKernel.f );
                    stream      = ( uint64_t )( data->args.hipModuleLaunchKernel.stream );
                    break;
                case HIP_API_ID_hipExtModuleLaunchKernel:
                    kernel_name = hipKernelNameRef( data->args.hipExtModuleLaunchKernel.f );
                    stream      = ( uint64_t )( data->args.hipExtModuleLaunchKernel.hStream );
                    break;
                case HIP_API_ID_hipHccModuleLaunchKernel:
                    kernel_name = hipKernelNameRef( data->args.hipHccModuleLaunchKernel.f );
                    stream      = ( uint64_t )( data->args.hipHccModuleLaunchKernel.hStream );
                    break;
                case HIP_API_ID_hipLaunchKernel:
                    kernel_name = hipKernelNameRefByPtr( data->args.hipLaunchKernel.function_address,
                                                         data->args.hipLaunchKernel.stream );
                    stream = ( uint64_t )data->args.hipLaunchKernel.stream;
                    break;
                default:
                    UTILS_BUG( "Unhandled kernel call" );
                    break;
            }
            e->payload.launch.stream        = get_stream( stream );
            e->payload.launch.kernel_region = get_kernel_region_by_name( kernel_name );
        }

        api_region_exit( cid );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

static void
handle_alloc( const void* ptr,
              size_t      size )
{
    hipPointerAttribute_t attributes = { 0, };
    hipError_t            err        = hipPointerGetAttributes( &attributes, ptr );
    if ( err != hipSuccess )
    {
        UTILS_WARNING( "Invalid HIP pointer to alloc: %s", hipGetErrorString( err ) );
        return;
    }

    SCOREP_AllocMetric* metric = host_alloc_metric;
    switch ( attributes.memoryType )
    {
        case hipMemoryTypeDevice:
        case hipMemoryTypeArray:
        case hipMemoryTypeUnified:
            metric = get_device( attributes.device )->alloc_metric;
            break;

        case hipMemoryTypeHost:
        default:
            break;
    }

    SCOREP_AllocMetric_HandleAlloc( metric, ( uint64_t )ptr, size );
}

static void
malloc_cb( uint32_t    domain,
           uint32_t    cid,
           const void* callbackData,
           void*       arg )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_BUG_ON( domain != ACTIVITY_DOMAIN_HIP_API, "Only HIP domain handled." );

    const hip_api_data_t* data = ( const hip_api_data_t* )callbackData;

    if ( data->phase == ACTIVITY_API_PHASE_ENTER )
    {
        uint64_t alloc_size;
        switch ( cid )
        {
            case HIP_API_ID_hipMalloc:
                alloc_size = data->args.hipMalloc.size;
                break;

            case HIP_API_ID_hipHostMalloc:
                alloc_size = data->args.hipHostMalloc.size;
                break;

            case HIP_API_ID_hipMallocManaged:
                alloc_size = data->args.hipMallocManaged.size;
                break;

            case HIP_API_ID_hipExtMallocWithFlags:
                alloc_size = data->args.hipExtMallocWithFlags.sizeBytes;
                break;

#if HAVE( DECL_HIP_API_ID_HIPMALLOCHOST )
            case HIP_API_ID_hipMallocHost:
                alloc_size = data->args.hipMallocHost.size;
                break;
#endif

#if HAVE( DECL_HIP_API_ID_HIPHOSTALLOC )
            case HIP_API_ID_hipHostAlloc:
                alloc_size = data->args.hipHostAlloc.size;
                break;
#endif

            default:
                UTILS_BUG( "Unhandled malloc category call '%s'", hip_api_name( cid ) );
        }

        SCOREP_AddAttribute( attribute_allocation_size, &alloc_size );
        api_region_enter( cid, SCOREP_REGION_ALLOCATE, "HIP_MALLOC", false );
    }

    if ( data->phase == ACTIVITY_API_PHASE_EXIT )
    {
        switch ( cid )
        {
            case HIP_API_ID_hipMalloc:
                handle_alloc( *data->args.hipMalloc.ptr,
                              data->args.hipMalloc.size );
                break;

            case HIP_API_ID_hipHostMalloc:
                handle_alloc( *data->args.hipHostMalloc.ptr,
                              data->args.hipHostMalloc.size );
                break;

            case HIP_API_ID_hipMallocManaged:
                handle_alloc( *data->args.hipMallocManaged.dev_ptr,
                              data->args.hipMallocManaged.size );
                break;

            case HIP_API_ID_hipExtMallocWithFlags:
                handle_alloc( *data->args.hipExtMallocWithFlags.ptr,
                              data->args.hipExtMallocWithFlags.sizeBytes );
                break;

#if HAVE( DECL_HIP_API_ID_HIPMALLOCHOST )
            case HIP_API_ID_hipMallocHost:
                handle_alloc( *data->args.hipMallocHost.ptr,
                              data->args.hipMallocHost.size );
                break;
#endif

#if HAVE( DECL_HIP_API_ID_HIPHOSTALLOC )
            case HIP_API_ID_hipHostAlloc:
                handle_alloc( *data->args.hipHostAlloc.ptr,
                              data->args.hipHostAlloc.size );
                break;
#endif

            default:
                UTILS_BUG( "Unhandled malloc category call '%s'", hip_api_name( cid ) );
        }

        api_region_exit( cid );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

static void
handle_free( uint64_t    correlationId,
             uint32_t    cid,
             const void* ptr )
{
    hipPointerAttribute_t attributes = { 0, };
    hipError_t            err        = hipPointerGetAttributes( &attributes, ptr );
    if ( err != hipSuccess )
    {
        UTILS_WARNING( "Invalid HIP pointer to free: %s", hipGetErrorString( err ) );
        return;
    }

    SCOREP_AllocMetric* metric = host_alloc_metric;
    switch ( attributes.memoryType )
    {
        case hipMemoryTypeDevice:
        case hipMemoryTypeArray:
        case hipMemoryTypeUnified:
            metric = get_device( attributes.device )->alloc_metric;
            break;

        case hipMemoryTypeHost:
        default:
            break;
    }

    void* allocation = NULL;
    SCOREP_AllocMetric_AcquireAlloc( metric, ( uint64_t )ptr, &allocation );
    if ( allocation )
    {
        correlation_entry* e = create_correlation_entry( correlationId, cid );
        e->payload.free.metric     = metric;
        e->payload.free.allocation = allocation;
    }
}

static void
free_cb( uint32_t    domain,
         uint32_t    cid,
         const void* callbackData,
         void*       arg )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_BUG_ON( domain != ACTIVITY_DOMAIN_HIP_API, "Only HIP domain handled." );

    const hip_api_data_t* data = ( const hip_api_data_t* )callbackData;

    if ( data->phase == ACTIVITY_API_PHASE_ENTER )
    {
        api_region_enter( cid, SCOREP_REGION_DEALLOCATE, "HIP_MALLOC", false );

        switch ( cid )
        {
            case HIP_API_ID_hipFree:
                handle_free( data->correlation_id, cid, data->args.hipFree.ptr );
                break;

            case HIP_API_ID_hipHostFree:
                handle_free( data->correlation_id, cid, data->args.hipHostFree.ptr );
                break;

#if HAVE( DECL_HIP_API_ID_HIPFREEHOST )
            case HIP_API_ID_hipFreeHost:
                handle_free( data->correlation_id, cid, data->args.hipFreeHost.ptr );
                break;
#endif

            default:
                UTILS_BUG( "Unhandled free category call '%s'", hip_api_name( cid ) );
                break;
        }
    }

    if ( data->phase == ACTIVITY_API_PHASE_EXIT )
    {
        correlation_entry* e = get_correlation_entry( data->correlation_id );
        if ( e )
        {
            uint64_t dealloc_size;
            SCOREP_AllocMetric_HandleFree( e->payload.free.metric,
                                           e->payload.free.allocation,
                                           &dealloc_size );
            correlation_table_remove( data->correlation_id );

            SCOREP_AddAttribute( attribute_deallocation_size, &dealloc_size );
        }

        api_region_exit( cid );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

static void
memcpy_cb( uint32_t    domain,
           uint32_t    cid,
           const void* callbackData,
           void*       arg )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_BUG_ON( domain != ACTIVITY_DOMAIN_HIP_API, "Only HIP domain handled." );

    const hip_api_data_t* data = ( const hip_api_data_t* )callbackData;

    uint64_t      stream_id = 0;
    hipMemcpyKind kind;
    uint64_t      size;
    bool          is_sync;
    switch ( cid )
    {
        case HIP_API_ID_hipMemcpy:
            kind    = data->args.hipMemcpy.kind;
            size    = data->args.hipMemcpy.sizeBytes;
            is_sync = true;
            break;
        case HIP_API_ID_hipMemcpyWithStream:
            stream_id = ( uint64_t )data->args.hipMemcpyWithStream.stream;
            kind      = data->args.hipMemcpyWithStream.kind;
            size      = data->args.hipMemcpyWithStream.sizeBytes;
            is_sync   = true;
            break;
        case HIP_API_ID_hipMemcpyHtoD:
            kind    = hipMemcpyHostToDevice;
            size    = data->args.hipMemcpyHtoD.sizeBytes;
            is_sync = true;
            break;
        case HIP_API_ID_hipMemcpyDtoH:
            kind    = hipMemcpyDeviceToHost;
            size    = data->args.hipMemcpyDtoH.sizeBytes;
            is_sync = true;
            break;
        case HIP_API_ID_hipMemcpyDtoD:
            kind    = hipMemcpyDeviceToDevice;
            size    = data->args.hipMemcpyDtoD.sizeBytes;
            is_sync = true;
            break;
        case HIP_API_ID_hipMemcpyFromSymbol:
            kind    = data->args.hipMemcpyFromSymbol.kind;
            size    = data->args.hipMemcpyFromSymbol.sizeBytes;
            is_sync = true;
            break;
        case HIP_API_ID_hipMemcpyAsync:
            kind    = data->args.hipMemcpyAsync.kind;
            size    = data->args.hipMemcpyAsync.sizeBytes;
            is_sync = false;
            break;
        case HIP_API_ID_hipMemcpyHtoDAsync:
            kind    = hipMemcpyHostToDevice;
            size    = data->args.hipMemcpyHtoDAsync.sizeBytes;
            is_sync = false;
            break;
        case HIP_API_ID_hipMemcpyDtoHAsync:
            kind    = hipMemcpyDeviceToHost;
            size    = data->args.hipMemcpyDtoHAsync.sizeBytes;
            is_sync = false;
            break;
        case HIP_API_ID_hipMemcpyDtoDAsync:
            kind    = hipMemcpyDeviceToDevice;
            size    = data->args.hipMemcpyDtoDAsync.sizeBytes;
            is_sync = false;
            break;
        case HIP_API_ID_hipMemcpyFromSymbolAsync:
            kind    = data->args.hipMemcpyFromSymbolAsync.kind;
            size    = data->args.hipMemcpyFromSymbolAsync.sizeBytes;
            is_sync = false;
            break;

        default:
            UTILS_BUG( "Unhandled memcpy callback" );
            break;
    }

    if ( data->phase == ACTIVITY_API_PHASE_ENTER )
    {
        api_region_enter( cid, SCOREP_REGION_RMA, "HIP_MEMCPY", false );

        correlation_entry* e = create_correlation_entry( data->correlation_id, cid );
        e->payload.memcpy.stream = get_stream( stream_id );
        e->payload.memcpy.kind   = kind;
        e->payload.memcpy.size   = size;

        activate_host_location();
    }

    if ( data->phase == ACTIVITY_API_PHASE_EXIT )
    {
        api_region_exit( cid );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

static void
sync_cb( uint32_t    domain,
         uint32_t    cid,
         const void* callbackData,
         void*       arg )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_BUG_ON( domain != ACTIVITY_DOMAIN_HIP_API, "Only HIP domain handled." );

    const hip_api_data_t* data = ( const hip_api_data_t* )callbackData;

    if ( data->phase == ACTIVITY_API_PHASE_ENTER )
    {
        api_region_enter( cid, SCOREP_REGION_WRAPPER, "HIP_SYNC", false );
    }

    if ( data->phase == ACTIVITY_API_PHASE_EXIT )
    {
        api_region_exit( cid );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

// Activity tracing callback
//
// Called on activity buffer flush. Executes in a dedicated roctracer thread.
// Hence, do not trigger any callpath to SCOREP_Location_GetCurrentCPULocation()
// (i.e., do not trigger host events). But be aware that allocating memory
// could also trigger this.
static void
activity_cb( const char* begin,
             const char* end,
             void*       arg )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !event_phase )
    {
        UTILS_WARN_ONCE( "HIP activity buffer flush past event phase." );

        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    uint64_t current_activity_scorep_time;
    uint64_t current_activity_hip_time;
    get_synced_timestamps( &current_activity_scorep_time, &current_activity_hip_time );
    UTILS_ASSERT( previous_activity_scorep_time < current_activity_scorep_time );
    UTILS_ASSERT( previous_activity_hip_time < current_activity_hip_time );

    double slope =   ( double )( current_activity_scorep_time - previous_activity_scorep_time )
                   / ( double )( current_activity_hip_time    - previous_activity_hip_time );

#define CONVERT_TO_SCOREP_TIMESTAMP( hipTimestamp ) \
    ( uint64_t )( ( ( ( double )( hipTimestamp ) - ( double )previous_activity_hip_time ) * slope ) + ( double )previous_activity_scorep_time )

    for ( const roctracer_record_t* record = ( const roctracer_record_t* )begin;
          record != ( const roctracer_record_t* )end;
          roctracer_next_record( record, &record ) )
    {
        if ( record->domain != ACTIVITY_DOMAIN_HIP_OPS )
        {
            UTILS_WARNING( "Unexpected activity record not in HIP_OPS domain: %s",
                           roctracer_op_string( record->domain, record->op, record->kind ) );
            continue;
        }

        correlation_entry* e = get_correlation_entry( record->correlation_id );
        if ( !e )
        {
            UTILS_WARNING( "No correlation entry for activity record, discarding" );
            continue;
        }

        uint64_t begin_time = CONVERT_TO_SCOREP_TIMESTAMP( record->begin_ns );
        uint64_t end_time   = CONVERT_TO_SCOREP_TIMESTAMP( record->end_ns );
        switch ( e->cid )
        {
            case HIP_API_ID_hipModuleLaunchKernel:
            case HIP_API_ID_hipExtModuleLaunchKernel:
            case HIP_API_ID_hipHccModuleLaunchKernel:
            case HIP_API_ID_hipLaunchKernel:
                UTILS_DEBUG( "Recording kernel %s execution from correlation id %" PRIu64 ", "
                             "translated %" PRIu64 " duration, original %" PRIu64 " duration",
                             SCOREP_RegionHandle_GetName( e->payload.launch.kernel_region ),
                             record->correlation_id,
                             record->end_ns - record->begin_ns,
                             end_time - begin_time );

                SCOREP_Location_EnterRegion( e->payload.launch.stream->device_location,
                                             begin_time,
                                             e->payload.launch.kernel_region );

                if ( scorep_hip_features & SCOREP_HIP_FEATURE_KERNEL_CALLSITE )
                {
                    SCOREP_Location_TriggerParameterUint64( e->payload.launch.stream->device_location,
                                                            begin_time,
                                                            callsite_id_parameter,
                                                            e->payload.launch.callsite_hash );
                }

                SCOREP_Location_ExitRegion( e->payload.launch.stream->device_location,
                                            end_time,
                                            e->payload.launch.kernel_region );
                break;

            case HIP_API_ID_hipMemcpyAsync:
            case HIP_API_ID_hipMemcpyFromSymbolAsync:
            case HIP_API_ID_hipMemcpyHtoDAsync:
            case HIP_API_ID_hipMemcpyDtoHAsync:
            case HIP_API_ID_hipMemcpy:
            case HIP_API_ID_hipMemcpyWithStream:
            case HIP_API_ID_hipMemcpyFromSymbol:
            case HIP_API_ID_hipMemcpyHtoD:
            case HIP_API_ID_hipMemcpyDtoH:
            case HIP_API_ID_hipMemcpyDtoD:
                UTILS_DEBUG( "Recording RMA completion from correlation id %d, %" PRIu64 ":%" PRIu64,
                             record->correlation_id,
                             begin_time, end_time );

                if ( e->payload.memcpy.kind == hipMemcpyHostToDevice )
                {
                    SCOREP_Location_RmaGet( e->payload.memcpy.stream->device_location,
                                            begin_time,
                                            scorep_hip_window_handle,
                                            get_host_rank( e->host_origin_location ),
                                            e->payload.memcpy.size,
                                            record->correlation_id );
                }
                else if ( e->payload.memcpy.kind == hipMemcpyDeviceToHost )
                {
                    SCOREP_Location_RmaPut( e->payload.memcpy.stream->device_location,
                                            begin_time,
                                            scorep_hip_window_handle,
                                            get_host_rank( e->host_origin_location ),
                                            e->payload.memcpy.size,
                                            record->correlation_id );
                }

                if ( e->payload.memcpy.kind == hipMemcpyHostToDevice
                     || e->payload.memcpy.kind == hipMemcpyDeviceToHost )
                {
                    SCOREP_Location_RmaOpCompleteBlocking( e->payload.memcpy.stream->device_location,
                                                           end_time,
                                                           scorep_hip_window_handle,
                                                           record->correlation_id );
                }

                break;

            default:
                break;
        }

        correlation_table_remove( record->correlation_id );
    }

#undef CONVERT_TO_SCOREP_TIMESTAMP

    previous_activity_scorep_time = current_activity_scorep_time;
    previous_activity_hip_time    = current_activity_hip_time;

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

static void
user_cb( uint32_t    domain,
         uint32_t    cid,
         const void* callbackData,
         void*       arg )
{
    ( void )arg;

    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    const roctx_api_data_t* data = ( const roctx_api_data_t* )callbackData;

    UTILS_DEBUG( "CID %d", cid );

    switch ( cid )
    {
        case ROCTX_API_ID_roctxMarkA:
        case ROCTX_API_ID_roctxRangePushA:
            if ( !data->args.message )
            {
                UTILS_WARNING( "missing message for CID %d", cid );
                SCOREP_IN_MEASUREMENT_DECREMENT();
                return;
            }

            push_user_region( data->args.message );
            if ( cid != ROCTX_API_ID_roctxMarkA )
            {
                break;
            }
        /* fall through */

        case ROCTX_API_ID_roctxRangePop:
            pop_user_region();
            break;

        default:
            break;
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

#if ROCTRACER_VERSION_MAJOR > 4 || ( ROCTRACER_VERSION_MAJOR == 4 && ROCTRACER_VERSION_MINOR >= 1 )
static void
activity_alloc_cb( char** ptr, size_t size, void* arg )
{
    // alloc
    if ( *ptr == NULL )
    {
        *ptr = SCOREP_Memory_AllocForMisc( size );
        return;
    }

    // realloc
    if ( *ptr != NULL && size != 0 )
    {
        // leak memory
        *ptr = SCOREP_Memory_AllocForMisc( size );
        return;
    }

    // free
    // leak memory
}
#endif

// Init tracing routine
void
scorep_hip_callbacks_init( void )
{
    UTILS_DEBUG( "######################### Init ROC tracer" );
    UTILS_DEBUG( "    Enabled features: %" PRIx64, scorep_hip_features );

    enumerate_smi_devices();

    /* Already create all HIP devices, so that we do not query the device
     * properties inside a roctracer API callback. We create the corresponding
     * Score-P device on demand though. */
    int number_of_hip_devices;
    hipGetDeviceCount( &number_of_hip_devices );
    for ( int hip_device_id = 0; hip_device_id < number_of_hip_devices; hip_device_id++ )
    {
        /* Creates only the device struct, but not the Score-P definitions. */
        create_device( hip_device_id );
    }

    hip_file_handle = SCOREP_Definitions_NewSourceFile( "HIP" );

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_KERNEL_CALLSITE )
    {
        /* callsites need kernel tracking */
        scorep_hip_features |= SCOREP_HIP_FEATURE_KERNEL;

        callsite_id_parameter = SCOREP_AcceleratorMgmt_GetCallsiteParameter();
    }

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_MALLOC )
    {
        const char* host_name = "HIP Host Memory";
        SCOREP_AllocMetric_New( host_name, &host_alloc_metric );

        attribute_allocation_size   = SCOREP_AllocMetric_GetAllocationSizeAttribute();
        attribute_deallocation_size = SCOREP_AllocMetric_GetDeallocationSizeAttribute();
    }

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_MEMCPY )
    {
        /* create interim communicator once for a process */
        scorep_hip_interim_communicator_handle = SCOREP_Definitions_NewInterimCommunicator(
            SCOREP_INVALID_INTERIM_COMMUNICATOR,
            SCOREP_PARADIGM_HIP,
            0,
            NULL );
        scorep_hip_window_handle = SCOREP_Definitions_NewRmaWindow(
            "HIP_WINDOW",
            scorep_hip_interim_communicator_handle,
            SCOREP_RMA_WINDOW_FLAG_NONE );
    }

    // Must be called at least once on one of HIP_API, HCC_OPS, or its alias
    // HIP_OPS in order to initialize any HIP domain recording properly
    SCOREP_ROCTRACER_CALL( roctracer_set_properties( ACTIVITY_DOMAIN_HIP_API, NULL ) );
    // Second call is redundant given the current state of roctracer
    // but hurts nothing to leave in
    SCOREP_ROCTRACER_CALL( roctracer_set_properties( ACTIVITY_DOMAIN_HIP_OPS, NULL ) );

    // Allocating tracing pool
    // cannot use the Score-P allocatorbecause of:
    // https://github.com/ROCm-Developer-Tools/roctracer/issues/54
    // fixed in 5.3, roctracer 4.1 is included in ROCm 5.3
    roctracer_properties_t properties = { 0 };
#if ROCTRACER_VERSION_MAJOR > 4 || ( ROCTRACER_VERSION_MAJOR == 4 && ROCTRACER_VERSION_MINOR >= 1 )
    properties.alloc_fun = activity_alloc_cb;
#endif
    properties.buffer_size         = scorep_hip_activity_buffer_size;
    properties.buffer_callback_fun = activity_cb;
    SCOREP_ROCTRACER_CALL( roctracer_open_pool( &properties ) );
}

void
scorep_hip_callbacks_finalize( void )
{
    UTILS_DEBUG( "######################### Finalize ROC tracer" );
    SCOREP_ROCTRACER_CALL( roctracer_close_pool() );
}

#define ENABLE_TRACING( fn, callback )      \
    do {                                    \
        SCOREP_ROCTRACER_CALL(              \
            roctracer_enable_op_callback(   \
                ACTIVITY_DOMAIN_HIP_API,    \
                HIP_API_ID_ ## fn,          \
                callback, NULL ) );         \
    } while ( 0 )

// Start tracing routine
void
scorep_hip_callbacks_enable( void )
{
    UTILS_DEBUG( "############################## Start HIP tracing" );

    /* need to call any HIP API to init HSA before calling roctracer_get_timestamp
     * see: https://github.com/ROCm-Developer-Tools/roctracer/issues/65 */
    int dummy_device_id;
    hipGetDevice( &dummy_device_id );

    get_synced_timestamps( &previous_activity_scorep_time, &previous_activity_hip_time );

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_API )
    {
        /* These are all allowed to be filtered, thus setting callback arg to !NULL */
        SCOREP_ROCTRACER_CALL( roctracer_enable_domain_callback( ACTIVITY_DOMAIN_HIP_API, api_cb, ( void* )1 ) );
    }

    bool need_stream_api_tracing = false;
    bool need_activity_tracing   = false;

    /* Kernel launches. */
    if ( scorep_hip_features & SCOREP_HIP_FEATURE_KERNEL )
    {
        need_stream_api_tracing = true;
        need_activity_tracing   = true;

        ENABLE_TRACING( hipModuleLaunchKernel, kernel_cb );
        ENABLE_TRACING( hipExtModuleLaunchKernel, kernel_cb );
        ENABLE_TRACING( hipHccModuleLaunchKernel, kernel_cb );
        ENABLE_TRACING( hipLaunchKernel, kernel_cb );
        /* calls hipModuleLaunchKernel through public API, so we get this traced
         * with actual meaningful args that way */
        /* ENABLE_TRACING( hipLaunchByPtr, kernel_cb ); */
    }

    /* Basic host and device mallocs */
    if ( scorep_hip_features & SCOREP_HIP_FEATURE_MALLOC )
    {
        need_stream_api_tracing = true;

        /* Host and device allocs */
        ENABLE_TRACING( hipMalloc, malloc_cb );
        ENABLE_TRACING( hipHostMalloc, malloc_cb );
        ENABLE_TRACING( hipMallocManaged, malloc_cb );
        ENABLE_TRACING( hipExtMallocWithFlags, malloc_cb );

        /* Host and device frees */
        ENABLE_TRACING( hipFree, free_cb );
        ENABLE_TRACING( hipHostFree, free_cb );

        /* These are deprecated. */
#if HAVE( DECL_HIP_API_ID_HIPMALLOCHOST )
        ENABLE_TRACING( hipMallocHost, malloc_cb );
#endif
#if HAVE( DECL_HIP_API_ID_HIPHOSTALLOC )
        ENABLE_TRACING( hipHostAlloc, malloc_cb );
#endif
#if HAVE( DECL_HIP_API_ID_HIPFREEHOST )
        ENABLE_TRACING( hipFreeHost, free_cb );
#endif

        /* The below are graphics-oriented allocs/frees that don't
         * give an easy way to compute their size and don't show
         * up in HPC codes I've found. */

        /* ENABLE_TRACING( hipMalloc3D, malloc_cb ); */
        /* ENABLE_TRACING( hipMalloc3DArray, malloc_cb ); */
        /* ENABLE_TRACING( hipMallocArray, malloc_cb ); */
        /* ENABLE_TRACING( hipMallocMipmappedArray, malloc_cb ); */
        /* ENABLE_TRACING( hipMallocPitch, malloc_cb ); */
        /* ENABLE_TRACING( hipMemAllocHost, malloc_cb ); */
        /* ENABLE_TRACING( hipMemAllocPitch, malloc_cb ); */

        /* ENABLE_TRACING( hipFreeArray, free_cb ); */
        /* ENABLE_TRACING( hipFreeMipmappedArray, free_cb ); */
        /* ENABLE_TRACING( hipFreeAsync, free_cb ); */
    }

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_MEMCPY )
    {
        need_stream_api_tracing = true;
        need_activity_tracing   = true;

        /* Synchronous memory copies needs both callback and activity handling */
        ENABLE_TRACING( hipMemcpy, memcpy_cb );
        ENABLE_TRACING( hipMemcpyWithStream, memcpy_cb );
        ENABLE_TRACING( hipMemcpyHtoD, memcpy_cb );
        ENABLE_TRACING( hipMemcpyFromSymbol, memcpy_cb );
        ENABLE_TRACING( hipMemcpyDtoH, memcpy_cb );
        ENABLE_TRACING( hipMemcpyDtoD, memcpy_cb ); /* host side only */

        /* Array memcpy has the same issues (and lack of HPC use) as
         * array malloc, so defer these */
        /* ENABLE_TRACING( hipMemcpyHtoA, memcpy_cb ); */
        /* ENABLE_TRACING( hipMemcpyAtoH, memcpy_cb ); */
        /* ENABLE_TRACING( hipMemcpy2DToArray, memcpy_cb ); */
        /* ENABLE_TRACING( hipMemcpy2DFromArray, memcpy_cb ); */
        /* ENABLE_TRACING( hipMemcpy3D, memcpy_cb ); */
        /* ENABLE_TRACING( hipMemcpy2D, memcpy_cb ); */

        /* Async memcpy needs both callback and activity handling */
        ENABLE_TRACING( hipMemcpyAsync, memcpy_cb );
        ENABLE_TRACING( hipMemcpyHtoDAsync, memcpy_cb );
        ENABLE_TRACING( hipMemcpyFromSymbolAsync, memcpy_cb );
        ENABLE_TRACING( hipMemcpyDtoHAsync, memcpy_cb );
        ENABLE_TRACING( hipMemcpyDtoDAsync, memcpy_cb ); /* host side only */

        /* Peer is multi-device, deferred */
        /* ENABLE_TRACING( hipMemcpyPeerAsync, memcpy_cb ); */
        /* And the async array memcpys are also deferred */
        /* ENABLE_TRACING( hipMemcpy3DAsync, memcpy_cb ); */
        /* ENABLE_TRACING( hipMemcpy2DAsync, memcpy_cb ); */
        /* ENABLE_TRACING( hipMemcpyParam2DAsync, memcpy_cb ); */
    }

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_SYNC )
    {
        /* Record time we spend in device or stream synchronizes */
        ENABLE_TRACING( hipDeviceSynchronize, sync_cb );
        ENABLE_TRACING( hipStreamSynchronize, sync_cb );
    }

    if ( need_stream_api_tracing )
    {
        /* Should all our static setup move here? In principle sure but
         * in practice hipInit is not mandatory to call it seems... */
        ENABLE_TRACING( hipInit, stream_cb );
        /* Stream and device functions we want to track one way or another */
        ENABLE_TRACING( hipStreamCreate, stream_cb );
        ENABLE_TRACING( hipStreamCreateWithFlags, stream_cb );
        ENABLE_TRACING( hipStreamCreateWithPriority, stream_cb );
        ENABLE_TRACING( hipExtStreamCreateWithCUMask, stream_cb );

        ENABLE_TRACING( hipStreamDestroy, api_cb );
        ENABLE_TRACING( hipSetDevice, api_cb );
    }

    if ( need_activity_tracing )
    {
        SCOREP_ROCTRACER_CALL( roctracer_enable_domain_activity( ACTIVITY_DOMAIN_HIP_OPS ) );
    }

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_USER )
    {
        /* we do not support RangeStart/Stop */

        SCOREP_ROCTRACER_CALL(
            roctracer_enable_op_callback( ACTIVITY_DOMAIN_ROCTX,
                                          ROCTX_API_ID_roctxMarkA, user_cb, NULL ) );
        SCOREP_ROCTRACER_CALL(
            roctracer_enable_op_callback( ACTIVITY_DOMAIN_ROCTX,
                                          ROCTX_API_ID_roctxRangePushA, user_cb, NULL ) );
        SCOREP_ROCTRACER_CALL(
            roctracer_enable_op_callback( ACTIVITY_DOMAIN_ROCTX,
                                          ROCTX_API_ID_roctxRangePop, user_cb, NULL ) );
    }

    /* Enable adapter internal event phase which lasts till scorep_hip_callbacks_disable,
     * thus extending WITHIN */
    event_phase = 1;
}

#if HAVE( UTILS_DEBUG )
static void
print_orphan( correlation_table_key_t   k,
              correlation_table_value_t v,
              void*                     unused )
{
    UTILS_DEBUG( "correlation id %d", v->cid );
}
#endif

static void
report_leaked( device_table_key_t   k,
               device_table_value_t v,
               void*                unused )
{
    if ( v->alloc_metric )
    {
        SCOREP_AllocMetric_ReportLeaked( v->alloc_metric );
    }
}

// Stop tracing routine
void
scorep_hip_callbacks_disable( void )
{
    SCOREP_ROCTRACER_CALL( roctracer_disable_domain_callback( ACTIVITY_DOMAIN_HIP_API ) );
    SCOREP_ROCTRACER_CALL( roctracer_disable_domain_activity( ACTIVITY_DOMAIN_HIP_OPS ) );
    SCOREP_ROCTRACER_CALL( roctracer_disable_domain_callback( ACTIVITY_DOMAIN_ROCTX ) );
    SCOREP_ROCTRACER_CALL( roctracer_flush_activity() );
    UTILS_DEBUG( "############################## Stop HIP tracing" );
#if HAVE( UTILS_DEBUG )
    correlation_table_iterate_key_value_pairs( &print_orphan, NULL );
#endif

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_MALLOC )
    {
        SCOREP_AllocMetric_ReportLeaked( host_alloc_metric );
        device_table_iterate_key_value_pairs( &report_leaked, NULL );
    }

    /* Now that all buffers are flushed, we can disable the event phase again */
    event_phase = 0;
}

static bool
assign_cpu_locations( SCOREP_Location* location,
                      void*            arg )
{
    if ( SCOREP_Location_GetType( location ) == SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        scorep_hip_cpu_location_data* location_data = SCOREP_Location_GetSubsystemData(
            location, scorep_hip_subsystem_id );
        if ( location_data->local_rank != SCOREP_HIP_NO_RANK )
        {
            UTILS_BUG_ON( location_data->local_rank >= scorep_hip_global_location_count,
                          "HIP rank exceeds total number of assigned ranks." );

            scorep_hip_global_location_ids[ location_data->local_rank ] =
                SCOREP_Location_GetGlobalId( location );
        }
    }

    return false;
}

static void
assign_gpu_locations( stream_table_key_t   key,
                      stream_table_value_t value,
                      void*                arg )
{
    scorep_hip_stream* stream = value;

    UTILS_BUG_ON( stream->local_rank == SCOREP_HIP_NO_RANK,
                  "HIP stream without assigned rank." );

    UTILS_BUG_ON( stream->local_rank >= scorep_hip_global_location_count,
                  "HIP rank exceeds total number of assigned ranks." );

    scorep_hip_global_location_ids[ stream->local_rank ] =
        SCOREP_Location_GetGlobalId( stream->device_location );
}

void
scorep_hip_collect_comm_locations( void )
{
    scorep_hip_global_location_count = UTILS_Atomic_LoadN_uint32(
        &local_rank_counter, UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );

    if ( scorep_hip_global_location_count == 0 )
    {
        return;
    }

    /* allocate the HIP communication group array */
    scorep_hip_global_location_ids = calloc( scorep_hip_global_location_count,
                                             sizeof( *scorep_hip_global_location_ids ) );

    /* Assign CPU locations */
    SCOREP_Location_ForAll( assign_cpu_locations, NULL );

    /* Assign GPU locations */
    stream_table_iterate_key_value_pairs( assign_gpu_locations, NULL );
}

void
activate_host_location( void )
{
    scorep_hip_cpu_location_data* location_data = SCOREP_Location_GetSubsystemData(
        SCOREP_Location_GetCurrentCPULocation(), scorep_hip_subsystem_id );
    if ( location_data->local_rank == SCOREP_HIP_NO_RANK )
    {
        location_data->local_rank = UTILS_Atomic_FetchAdd_uint32(
            &local_rank_counter, 1,
            UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    }
}

uint32_t
get_host_rank( SCOREP_Location* location )
{
    scorep_hip_cpu_location_data* location_data = SCOREP_Location_GetSubsystemData(
        location, scorep_hip_subsystem_id );
    return location_data->local_rank;
}

void
get_synced_timestamps( uint64_t* scorepTime,
                       uint64_t* hipTime )
{
    uint64_t before = SCOREP_Timer_GetClockTicks();

    SCOREP_ROCTRACER_CALL( roctracer_get_timestamp( hipTime ) );

    uint64_t after = SCOREP_Timer_GetClockTicks();

    /* Update the time once and only once
     * We remain careful of overflow, so add the
     * averaged *difference* to the timestamp.
     */
    *scorepTime = before + ( after - before ) / 2;
}
