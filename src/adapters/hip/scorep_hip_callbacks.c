/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
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
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_Filtering.h>
#include <SCOREP_AcceleratorManagement.h>
#include <SCOREP_FastHashtab.h>
#include <SCOREP_Demangle.h>
#include <SCOREP_AllocMetric.h>

#include <scorep_system_tree.h>

#include <UTILS_Atomic.h>
#define SCOREP_DEBUG_MODULE_NAME HIP
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <roctracer_hip.h>

#if HAVE( ROCM_SMI_SUPPORT )
#include <rocm_smi/rocm_smi.h>
#endif

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

static uint32_t local_rank_counter;

/* will be defined when the first device will be created */
SCOREP_RmaWindowHandle           scorep_hip_window_handle;
SCOREP_InterimCommunicatorHandle scorep_hip_interim_communicator_handle;

uint64_t  scorep_hip_global_location_count = 0;
uint64_t* scorep_hip_global_location_ids   = NULL;

/************************** Forward declarations ******************************/

/* Assign the current host location a rank inside the HIP specific RMA window/communicator */
static void
activate_host_location( void );

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

#if HAVE( ROCM_SMI_SUPPORT )

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

#endif


static void
enumerate_smi_devices( void )
{
#if HAVE( ROCM_SMI_SUPPORT )

// Macro to check ROCm-SMI calls status
// Note that this applies only to calls returning `rsmi_status_t`!
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

#endif
}

static int
get_smi_device( int       deviceId,
                uint64_t* uuidOut )
{
    UTILS_ASSERT( uuidOut );

#if HAVE( ROCM_SMI_SUPPORT )

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

#endif

    return deviceId;
}

/************************** HIP devices ***************************************/

typedef struct
{
    int                         device_id;
    int                         smi_device_id;
    SCOREP_SystemTreeNodeHandle system_tree_node;
    SCOREP_LocationGroupHandle  location_group;
    uint32_t                    stream_counter;
    SCOREP_AllocMetric*         alloc_metric;
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

    device->device_id = *key;

    uint64_t uuid = 0;
    device->smi_device_id = get_smi_device( *key, &uuid );

    char buffer[ 80 ];
    snprintf( buffer, sizeof( buffer ), "%d", device->smi_device_id );
    device->system_tree_node = SCOREP_Definitions_NewSystemTreeNode(
        SCOREP_GetSystemTreeNodeHandleForSharedMemory(),
        SCOREP_SYSTEM_TREE_DOMAIN_ACCELERATOR_DEVICE,
        "ROCm Device",
        buffer );

    if ( uuid != 0 )
    {
        /* ROCm uses "GPU-" as prefix for the UUID
         * https://github.com/RadeonOpenCompute/ROCR-Runtime/blob/master/src/core/runtime/amd_gpu_agent.cpp#L993 */
        char uuid_buffer[ 22 ];
        snprintf( uuid_buffer, sizeof( uuid_buffer ), "GPU-%016" PRIx64, uuid );
        SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node, "UUID", uuid_buffer );
    }

    hipDeviceProp_t device_props;
    hipGetDeviceProperties( &device_props, *key );

    if ( strnlen( device_props.name, sizeof( device_props.name ) ) < sizeof( device_props.name ) )
    {
        SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                                 "Device name",
                                                 device_props.name );
    }

    if ( strnlen( device_props.gcnArchName, sizeof( device_props.gcnArchName ) ) < sizeof( device_props.gcnArchName ) )
    {
        SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                                 "AMD GCN architecture name",
                                                 device_props.gcnArchName );
    }

    snprintf( buffer, sizeof( buffer ), "%zu", device_props.totalGlobalMem );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Size of global memory region (in bytes)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%zu", device_props.sharedMemPerBlock );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Size of shared memory region (in bytes)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%zu", device_props.totalConstMem );
    /* Documentation looks wrong,
     * see https://github.com/ROCm-Developer-Tools/HIP/issues/3035 */
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.regsPerBlock );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Registers per block",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.warpSize );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Warp size",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.maxThreadsPerBlock );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Maximum work items per work group or workgroup max size",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d, %d, %d",
              device_props.maxThreadsDim[ 0 ],
              device_props.maxThreadsDim[ 1 ],
              device_props.maxThreadsDim[ 2 ] );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Maximum number of threads in each dimension (XYZ) of a block",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d, %d, %d",
              device_props.maxGridSize[ 0 ],
              device_props.maxGridSize[ 1 ],
              device_props.maxGridSize[ 2 ] );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Maximum grid dimensions (XYZ)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.clockRate );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Maximum clock frequency of the multi-processors (in khz)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.memoryClockRate );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Max global memory clock frequency (in khz)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.clockInstructionRate );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Timer clock frequency (in khz)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.memoryBusWidth );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Global memory bus width (in bits)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.isMultiGpuBoard );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Is multi-GPU board (1 if yes, 0 if not)",
                                             buffer );


    snprintf( buffer, sizeof( buffer ), "%d", device_props.canMapHostMemory );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Device can map host memory (1 if yes, 0 if not)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.concurrentKernels );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Device can possibly execute multiple kernels concurrently (1 if yes, 0 if not)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.multiProcessorCount );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Number of multi-processors (compute units)",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.l2CacheSize );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "L2 cache size",
                                             buffer );

    snprintf( buffer, sizeof( buffer ), "%d", device_props.maxThreadsPerMultiProcessor );
    SCOREP_SystemTreeNodeHandle_AddProperty( device->system_tree_node,
                                             "Maximum resident threads per multi-processor",
                                             buffer );

    SCOREP_SystemTreeNode_AddPCIProperties( device->system_tree_node,
                                            device_props.pciDomainID,
                                            device_props.pciBusID,
                                            device_props.pciDeviceID,
                                            UINT8_MAX );

    /* ROCm/HIP does not have a "context", we add one per device. The "ID"
     * is irrelevant regarding unification. But as this is already the
     * software layer, we use the HIP device ID, not the ROCm SMI device index.
     */
    snprintf( buffer, sizeof( buffer ), "HIP Context %d", *key );
    device->location_group = SCOREP_AcceleratorMgmt_CreateContext(
        device->system_tree_node,
        buffer );

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_MALLOC )
    {
        snprintf( buffer, 80, "HIP Context %d Memory", *key );
        SCOREP_AllocMetric_NewScoped( buffer, device->location_group, &device->alloc_metric );
    }

    if ( scorep_hip_interim_communicator_handle == SCOREP_INVALID_INTERIM_COMMUNICATOR )
    {
        // @todo: only with memcpy enabled
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

    return device;
}

/* nPairsPerChunk: 4+8 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( device_table,
                             10,
                             hashsize( DEVICE_TABLE_HASH_EXPONENT ) );

static scorep_hip_device*
get_device( device_table_key_t deviceId )
{
    device_table_value_t device = NULL;
    device_table_get_and_insert( deviceId, NULL, &device );
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

typedef struct
{
    int      device_id;
    uint64_t stream_id;
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
    return key1.device_id == key2.device_id && key1.stream_id == key2.stream_id;
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

static inline stream_table_value_t
stream_table_value_ctor( stream_table_key_t* key,
                         void*               ctorData )
{
    scorep_hip_device* device = NULL;
    device_table_get_and_insert( key->device_id, NULL, &device );
    uint32_t stream_seq = 0;
    if ( key->stream_id != 0 )
    {
        /* Ensure that stream_seq == 0 is always given to the NULL-stream */
        stream_seq = UTILS_Atomic_AddFetch_uint32( &device->stream_counter, 1,
                                                   UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    }

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
    stream_table_key_t key;
    hipGetDevice( &key.device_id );
    key.stream_id = streamId;

    stream_table_value_t ignored = NULL;
    if ( !stream_table_get_and_insert( key, NULL, &ignored ) )
    {
        UTILS_WARNING( "Duplicate stream %p", ( void* )streamId );
    }
}

static scorep_hip_stream*
get_stream( uint64_t streamId )
{
    stream_table_key_t key;
    hipGetDevice( &key.device_id );
    key.stream_id = streamId;

    scorep_hip_stream* stream = NULL;
    if ( !stream_table_get( key, &stream ) && streamId == 0 )
    {
        /* the NULL-stream is created implicitly at hipInit, which itself
         * is called implicitly without triggering the callback. */
        stream_table_get_and_insert( key, NULL, &stream );
    }
    UTILS_BUG_ON( stream == NULL, "Unknown stream %p", ( void* )streamId );

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
                                                                              SCOREP_REGION_FUNCTION );

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
        } launch;
        struct
        {
            SCOREP_AllocMetric* metric;
            void*               allocation;
        } free;
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
        api_region_enter( cid, SCOREP_REGION_WRAPPER, "HIP_API", false );
    }

    // Only store the correlation record on exit
    if ( data->phase == ACTIVITY_API_PHASE_EXIT )
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
        correlation_entry* e = create_correlation_entry( data->correlation_id, cid );
        e->payload.launch.stream        = get_stream( stream );
        e->payload.launch.kernel_region = get_kernel_region_by_name( kernel_name );

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

// Init tracing routine
void
scorep_hip_callbacks_init( void )
{
    UTILS_DEBUG( "######################### Init ROC tracer" );
    UTILS_DEBUG( "    Enabled features: %" PRIx64, scorep_hip_features );

    enumerate_smi_devices();

    hip_file_handle = SCOREP_Definitions_NewSourceFile( "HIP" );

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_MALLOC )
    {
        const char* host_name = "HIP Host Memory";
        SCOREP_AllocMetric_New( host_name, &host_alloc_metric );

        attribute_allocation_size   = SCOREP_AllocMetric_GetAllocationSizeAttribute();
        attribute_deallocation_size = SCOREP_AllocMetric_GetDeallocationSizeAttribute();
    }
}

void
scorep_hip_callbacks_finalize( void )
{
    UTILS_DEBUG( "######################### Finalize ROC tracer" );
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

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_API )
    {
        /* These are all allowed to be filtered, thus setting callback arg to !NULL */
        SCOREP_ROCTRACER_CALL( roctracer_enable_domain_callback( ACTIVITY_DOMAIN_HIP_API, api_cb, ( void* )1 ) );
    }

    bool need_stream_api_tracing = false;

    /* Kernel launches. */
    if ( scorep_hip_features & SCOREP_HIP_FEATURE_KERNELS )
    {
        need_stream_api_tracing = true;

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
    SCOREP_AllocMetric_ReportLeaked( v->alloc_metric );
}

// Stop tracing routine
void
scorep_hip_callbacks_disable( void )
{
    SCOREP_ROCTRACER_CALL( roctracer_disable_domain_callback( ACTIVITY_DOMAIN_HIP_API ) );
    UTILS_DEBUG( "############################## Stop HIP tracing" );
#if HAVE( UTILS_DEBUG )
    correlation_table_iterate_key_value_pairs( &print_orphan, NULL );
#endif

    if ( scorep_hip_features & SCOREP_HIP_FEATURE_MALLOC )
    {
        SCOREP_AllocMetric_ReportLeaked( host_alloc_metric );
        device_table_iterate_key_value_pairs( &report_leaked, NULL );
    }
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
