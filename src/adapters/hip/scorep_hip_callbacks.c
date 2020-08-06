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

#include <scorep_system_tree.h>

#include <UTILS_Atomic.h>
#define SCOREP_DEBUG_MODULE_NAME HIP
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <roctracer_hip.h>

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

/************************** HIP devices ***************************************/

typedef struct
{
    int                         device_id;
    SCOREP_SystemTreeNodeHandle system_tree_node;
    SCOREP_LocationGroupHandle  location_group;
    uint32_t                    stream_counter;
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

    char buffer[ 80 ];
    snprintf( buffer, sizeof( buffer ), "%d", *key );
    device->system_tree_node = SCOREP_Definitions_NewSystemTreeNode(
        SCOREP_GetSystemTreeNodeHandleForSharedMemory(),
        SCOREP_SYSTEM_TREE_DOMAIN_ACCELERATOR_DEVICE,
        "ROCm Device",
        buffer );

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

    snprintf( buffer, sizeof( buffer ), "HIP Context %d", *key );
    device->location_group = SCOREP_AcceleratorMgmt_CreateContext(
        device->system_tree_node,
        buffer );

    return device;
}

/* nPairsPerChunk: 4+8 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( device_table,
                             10,
                             hashsize( DEVICE_TABLE_HASH_EXPONENT ) );

/************************** HIP streams ***************************************/

typedef struct
{
    int              device_id;
    uint64_t         stream_id;
    uint32_t         stream_seq;
    SCOREP_Location* device_location;
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
    snprintf( thread_name, sizeof( thread_name ), "HIP[%d:%u]", stream->device_id, stream->stream_seq );

    stream->device_location =
        SCOREP_Location_CreateNonCPULocation( SCOREP_Location_GetCurrentCPULocation(),
                                              SCOREP_LOCATION_TYPE_GPU,
                                              SCOREP_PARADIGM_HIP,
                                              thread_name,
                                              device->location_group );

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

// Init tracing routine
void
scorep_hip_callbacks_init( void )
{
    UTILS_DEBUG( "######################### Init ROC tracer" );
    UTILS_DEBUG( "    Enabled features: %" PRIx64, scorep_hip_features );

    hip_file_handle = SCOREP_Definitions_NewSourceFile( "HIP" );
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

// Stop tracing routine
void
scorep_hip_callbacks_disable( void )
{
    SCOREP_ROCTRACER_CALL( roctracer_disable_domain_callback( ACTIVITY_DOMAIN_HIP_API ) );
    UTILS_DEBUG( "############################## Stop HIP tracing" );
}
