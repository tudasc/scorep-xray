/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 *  @status     alpha
 *  @file       scorep_cupti_callbacks.c
 *  @maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>
 *
 *  Implementation of the CUDA runtime and driver API event logging via
 *  CUPTI callbacks.
 */

#include <config.h>
#include "SCOREP_Config.h"
#include "SCOREP_Definitions.h"
#include "SCOREP_Events.h"
#include "SCOREP_Types.h"

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include "scorep_cuda.h"
#include "scorep_cupti.h"
#include "scorep_cupti_callbacks.h"

#if defined( SCOREP_CUPTI_ACTIVITY )
#include "scorep_cupti_activity.h" /* Support for CUPTI activity */
#endif

/*
 * Enable a CUPTI callback domain.
 *
 * @param _domain CUPTI callbacks domain
 */
#define SCOREP_CUPTI_ENABLE_CALLBACK_DOMAIN( _domain )                           \
    {                                                                            \
        SCOREP_CUPTI_CALL( cuptiEnableDomain( 1, scorep_cupti_callbacks_subscriber, _domain ) );                  \
    }

/*
 * Disable a CUPTI callback domain.
 *
 * @param _domain CUPTI callbacks domain
 */
#define SCOREP_CUPTI_DISABLE_CALLBACK_DOMAIN( _domain )                          \
    {                                                                            \
        SCOREP_CUPTI_CALL( cuptiEnableDomain( 0, scorep_cupti_callbacks_subscriber, _domain ) );                 \
    }

/* Enable CUDA runtime API callbacks, if recording is enabled */
#define ENABLE_CUDART_CALLBACKS() \
    if ( record_runtime_api ) \
        SCOREP_CUPTI_ENABLE_CALLBACK_DOMAIN( CUPTI_CB_DOMAIN_RUNTIME_API )

/* Disable CUDA runtime API callbacks, if recording is enabled */
#define DISABLE_CUDART_CALLBACKS() \
    if ( record_runtime_api ) \
        SCOREP_CUPTI_DISABLE_CALLBACK_DOMAIN( CUPTI_CB_DOMAIN_RUNTIME_API )

/* Enable CUDA driver API callbacks, if recording is enabled */
#define ENABLE_CUDRV_CALLBACKS() \
    if ( record_driver_api ) \
        SCOREP_CUPTI_ENABLE_CALLBACK_DOMAIN( CUPTI_CB_DOMAIN_DRIVER_API )

/* Disable CUDA driver API callbacks, if recording is enabled */
#define DISABLE_CUDRV_CALLBACKS() \
    if ( record_driver_api ) \
        SCOREP_CUPTI_DISABLE_CALLBACK_DOMAIN( CUPTI_CB_DOMAIN_DRIVER_API )

/*
 * Handle CUDA runtime memory copy calls.
 *
 * @param _cbInfo information about the callback
 * @param _kind the direction of the transfer
 * @param _src source memory pointer
 * @param _dst destination memory pointer
 * @param _bytes the number of transfered bytes
 * @param _time execution time stamp
 */
#define SCOREP_CUPTICB_MEMCPY_CUDART( _cbInfo, _kind, _src, _dst, _bytes, _time ) \
    { \
        if ( _kind == cudaMemcpyDefault ) { \
            handle_cuda_memcpy_default( _cbInfo, ( CUdeviceptr )_src, \
                                        ( CUdeviceptr )_dst, _bytes, _time ); \
        }else{ \
            handle_cuda_memcpy( _cbInfo, _kind, _bytes, _time ); } \
    }

/* initialization and finalization flags */
static bool scorep_cupti_callbacks_initialized = false;
static bool scorep_cupti_callbacks_finalized   = false;

/* flag: Are CUPTI callbacks enabled? */
static bool scorep_cupti_callbacks_enabled = false;

/* flag: Are CUPTI callbacks driver API domain enabled? */
static bool is_driver_domain_enabled = false;

/* flag: tracing of CUDA runtime API enabled? */
static bool record_runtime_api = false;

/* flag: tracing of CUDA driver API enabled? */
static bool record_driver_api = false;

/* global subscriber handle */
static CUpti_SubscriberHandle scorep_cupti_callbacks_subscriber;

/* CUDA runtime and driver API source file handle (function group description) */
static SCOREP_SourceFileHandle cuda_runtime_file_handle = SCOREP_INVALID_SOURCE_FILE;
static SCOREP_SourceFileHandle cuda_driver_file_handle  = SCOREP_INVALID_SOURCE_FILE;

static SCOREP_RegionHandle cuda_sync_region_handle = SCOREP_INVALID_REGION;


/**************** The callback functions to be registered *********************/

void CUPTIAPI
scorep_cupti_callbacks_all( void*,
                            CUpti_CallbackDomain,
                            CUpti_CallbackId,
                            const void* );

void ( * scorep_cupti_callbacks_all_ptr )( void*,
                                           CUpti_CallbackDomain,
                                           CUpti_CallbackId,
                                           const void* )
    = scorep_cupti_callbacks_all;

void CUPTIAPI
scorep_cupti_callbacks_runtime_api( CUpti_CallbackId,
                                    const CUpti_CallbackData* );

void
scorep_cupti_callbacks_driver_api( CUpti_CallbackId,
                                   const CUpti_CallbackData* );

void
scorep_cupti_callbacks_resource( CUpti_CallbackId,
                                 const CUpti_ResourceData* );

void
scorep_cupti_callbacks_sync( CUpti_CallbackId,
                             const CUpti_SynchronizeData* );

/******************************************************************************/

/*********************** Internal function declarations ***********************/
static enum cudaMemcpyKind
get_cuda_memcpy_kind( CUmemorytype,
                      CUmemorytype );
static void
handle_cuda_memcpy( const CUpti_CallbackData*,
                    enum  cudaMemcpyKind,
                    uint64_t,
                    uint64_t );
static void
handle_cuda_memcpy_p2p( const CUpti_CallbackData* cbInfo,
                        CUcontext                 cuSrcCtx,
                        CUcontext                 cuDstCtx,
                        uint64_t                  bytes,
                        uint64_t                  time );
static void
handle_cuda_memcpy_default( const CUpti_CallbackData* cbInfo,
                            CUdeviceptr               cuSrcDevPtr,
                            CUdeviceptr               cuDstDevPtr,
                            uint64_t                  bytes,
                            uint64_t                  time );
static void
handle_cuda_runtime_memcpy_async( const CUpti_CallbackData* cbInfo,
                                  enum cudaMemcpyKind       kind,
                                  uint64_t                  bytes,
                                  cudaStream_t              cuStrm );

static void
handle_cudart_knconf( const CUpti_CallbackData* );
static void
handle_cuda_kernel( const    CUpti_CallbackData*,
                    CUstream,
                    uint64_t blocks );

static void
handle_cuda_malloc( CUcontext,
                    uint64_t,
                    size_t );
static void
handle_cuda_free( CUcontext,
                  uint64_t );

/******************************************************************************/

/************************** CUDA function table *******************************/
#define CUPTI_CALLBACKS_CUDA_API_FUNC_MAX 1024
static SCOREP_RegionHandle scorep_cupti_callbacks_cuda_function_table[ CUPTI_CALLBACKS_CUDA_API_FUNC_MAX ];

/**
 * This is a pseudo hash function for CUPTI callbacks. No real hash is needed,
 * as the callback IDs are 3-digit integer values, which can be stored directly
 * in an array.
 *
 * @param domain            CUPTI callback domain.
 * @param callbackId        CUPTI callback ID.
 *
 * @return Return the position in the hash table (index).
 */
static uint32_t
cuda_api_function_hash( CUpti_CallbackDomain domain,
                        CUpti_CallbackId     callbackId )
{
    uint32_t index = 0;

    /* Use an offset for the driver API functions, if CUDA runtime and driver
       API recording is enabled (uncommon case) */
    if ( record_runtime_api && record_driver_api )
    {
        uint16_t offset = 0;

        if ( domain == CUPTI_CB_DOMAIN_DRIVER_API )
        {
            offset = CUPTI_CALLBACKS_CUDA_API_FUNC_MAX / 2;
        }

        index = offset + ( uint32_t )callbackId;

        if ( ( domain == CUPTI_CB_DOMAIN_RUNTIME_API ) &&
             ( index >= ( uint32_t )( CUPTI_CALLBACKS_CUDA_API_FUNC_MAX - offset ) ) )
        {
            index = 0;

            UTILS_WARNING( "[CUPTI Callbacks] Hash table for CUDA runtime API "
                           "function %d is to small!", callbackId );
        }
    }
    else
    {
        index = ( uint32_t )callbackId;
    }

    if ( index >= CUPTI_CALLBACKS_CUDA_API_FUNC_MAX )
    {
        index = 0;

        UTILS_WARNING( "[CUPTI Callbacks] Hash table for CUDA API "
                       "function %d is to small!", callbackId );
    }

    return ( uint32_t )index;
}

/**
 * Store a CUPTI callback together with a Score-P region handle.
 *
 * @param domain            CUPTI callback domain.
 * @param callbackId        CUPTI callback ID.
 * @param region          Score-P region handle.
 */
static void
cuda_api_function_put( CUpti_CallbackDomain domain,
                       CUpti_CallbackId     callbackId,
                       SCOREP_RegionHandle  region )
{
    scorep_cupti_callbacks_cuda_function_table[ cuda_api_function_hash( domain, callbackId ) ] = region;
}

/**
 * Retrieve the Score-P region handle of a CUPTI callback.
 *
 * @param domain            CUPTI callback domain.
 * @param callbackId        CUPTI callback ID.
 *
 * @return Return corresponding Score-P region handle.
 */
static SCOREP_RegionHandle
cuda_api_function_get( CUpti_CallbackDomain domain,
                       CUpti_CallbackId     callbackId )
{
    return scorep_cupti_callbacks_cuda_function_table[ cuda_api_function_hash( domain, callbackId ) ];
}
/******************************************************************************/


/*
 * Set a subscriber and a callback function for CUPTI callbacks.
 *
 * @param callback the callback function
 */
static void
scorep_cupti_set_callback( CUpti_CallbackFunc callback )
{
    CUptiResult cuptiErr;
    static bool initflag = true;

    if ( initflag )
    {
        initflag = false;

        SCOREP_CUDA_DRIVER_CALL( cuInit( 0 ) );

        /* only one subscriber allowed at a time */
        cuptiErr = cuptiSubscribe( &scorep_cupti_callbacks_subscriber, callback, NULL );
        SCOREP_CUPTI_CALL( cuptiErr );
    }
}

/*
 * Enable CUPTI callback domains depending on the requested GPU features.
 *
 * @param enable 'true' to enable CUPTI callbacks, 'false' to disable callbacks
 */
void
scorep_cupti_callbacks_enable( bool enable )
{
    SCOREP_CUPTI_LOCK();

    scorep_cupti_activity_enable( enable );

    if ( enable )
    {
        if ( !scorep_cupti_callbacks_enabled )
        {
            /* set callback for CUDA API functions */
            if ( record_runtime_api )
            {
                SCOREP_CUPTI_CALL( cuptiEnableDomain( 1, scorep_cupti_callbacks_subscriber,
                                                      CUPTI_CB_DOMAIN_RUNTIME_API ) );

                scorep_cupti_callbacks_enabled = true;
            }

            if ( record_driver_api ||
                 ( !record_runtime_api && scorep_cuda_record_gpumemusage ) ||
#if defined( SCOREP_CUPTI_EVENTS )
                 ( !record_runtime_api && scorep_cupti_events_enabled ) ||
#endif
                 ( !record_runtime_api && scorep_cuda_record_memcpy &&
                   scorep_cuda_sync_level == SCOREP_CUDA_RECORD_SYNC_FULL ) )
            {
                SCOREP_CUPTI_CALL( cuptiEnableDomain( 1, scorep_cupti_callbacks_subscriber,
                                                      CUPTI_CB_DOMAIN_DRIVER_API ) );

                is_driver_domain_enabled       = true;
                scorep_cupti_callbacks_enabled = true;
            }

#if defined( SCOREP_CUPTI_ACTIVITY )
            if ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy ||
                 scorep_cuda_record_gpumemusage )
            {
                if ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy )
                {
                    SCOREP_CUPTI_CALL( cuptiEnableDomain( 1, scorep_cupti_callbacks_subscriber,
                                                          CUPTI_CB_DOMAIN_SYNCHRONIZE ) );
                }

                SCOREP_CUPTI_CALL( cuptiEnableDomain( 1, scorep_cupti_callbacks_subscriber,
                                                      CUPTI_CB_DOMAIN_RESOURCE ) );

                scorep_cupti_callbacks_enabled = true;
            }
#endif
        }
    }
    else if ( scorep_cupti_callbacks_enabled )
    {
        SCOREP_CUPTI_CALL( cuptiEnableAllDomains( 0, scorep_cupti_callbacks_subscriber ) );

        is_driver_domain_enabled       = false;
        scorep_cupti_callbacks_enabled = false;
    }

    SCOREP_CUPTI_UNLOCK();
}

/*
 * Creates a Score-P CUPTI callbacks context.
 *
 * @param context the Score-P CUPTI context
 * @param cuStrm the CUDA stream
 *
 * @return the Score-P CUPTI callbacks context
 */
static scorep_cupti_callbacks_t*
scorep_cupticb_create_callbacks_context(
    scorep_cupti_context_t* context
#if !defined( SCOREP_CUPTI_ACTIVITY )
    , CUstream cuStrm
#endif
    )
{
    scorep_cupti_callbacks_t* context_callbacks = NULL;

    if ( context == NULL )
    {
        return NULL;
    }

    /* create new context, as it is not listed */
    context_callbacks = ( scorep_cupti_callbacks_t* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cupti_callbacks_t ) );

    context_callbacks->kernel_data = NULL;

#if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
    if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_CONCURRENT_KERNEL )
         == SCOREP_CUDA_FEATURE_CONCURRENT_KERNEL )
    {
        context_callbacks->concurrent_kernels = true;
    }
    else
    {
        int tmp_ck;

        /* check for concurrent kernel support */
        SCOREP_CUDA_DRIVER_CALL( cuDeviceGetAttribute( &tmp_ck,
                                                       CU_DEVICE_ATTRIBUTE_CONCURRENT_KERNELS, context->cuda_device ) );

        context_callbacks->concurrent_kernels = ( bool )tmp_ck;
    }

    context_callbacks->streams_created = 0;
#endif

#if !defined( SCOREP_CUPTI_ACTIVITY )
    /* enable handling of callbacks */
    context_callbacks->callbacks_enabled = 1;

    /* create first empty CUDA stream */
    context_callbacks->streams_created = 2;
    context_callbacks->strmList        = NULL;
    context_callbacks->strmList        = scorep_cupticb_createStream( cuStrm, context );
#endif

    /* set the callback context */
    context->callbacks = context_callbacks;

    return context_callbacks;
}

/*
 * Finalize the Score-P CUPTI Callbacks context.
 *
 * @param context pointer to Score-P CUPTI context
 */
static void
scorep_cupti_callbacks_finalize_context( scorep_cupti_context_t* context )
{
    if ( context == NULL || context->callbacks == NULL ||
         context->callbacks->kernel_data == NULL )
    {
        return;
    }

    if ( context->callbacks->kernel_data->down != NULL )
    {
        UTILS_WARNING( "[CUPTI Callbacks] Not all configured kernels have been executed!" );
    }

    context->callbacks->kernel_data = NULL;
}

/*
 * This CUPTI callback function chooses the CUPTI domain.
 *
 * @param userdata pointer to the user data
 * @param domain the callback domain (runtime or driver API)
 * @param cbid the ID of the callback function in the given domain
 * @param cbInfo information about the callback
 */
void CUPTIAPI
scorep_cupti_callbacks_all( void*                userdata,
                            CUpti_CallbackDomain domain,
                            CUpti_CallbackId     cbid,
                            const void*          cbInfo )
{
    if ( CUPTI_CB_DOMAIN_RUNTIME_API == domain )
    {
        scorep_cupti_callbacks_runtime_api( cbid, ( const CUpti_CallbackData* )cbInfo );
    }

    if ( CUPTI_CB_DOMAIN_DRIVER_API == domain )
    {
        scorep_cupti_callbacks_driver_api( cbid, ( const CUpti_CallbackData* )cbInfo );
    }

    if ( CUPTI_CB_DOMAIN_RESOURCE == domain )
    {
        scorep_cupti_callbacks_resource( cbid, ( const CUpti_ResourceData* )cbInfo );
    }

    if ( CUPTI_CB_DOMAIN_SYNCHRONIZE == domain )
    {
        scorep_cupti_callbacks_sync( cbid, ( const CUpti_SynchronizeData* )cbInfo );
    }
}

/*
 * This callback function is used to trace the CUDA runtime API.
 *
 * @param cbid the ID of the callback function in the given domain
 * @param cbInfo information about the callback
 *
 */
void CUPTIAPI
scorep_cupti_callbacks_runtime_api( CUpti_CallbackId          callbackId,
                                    const CUpti_CallbackData* cbInfo )
{
    SCOREP_Location*    location = SCOREP_Location_GetCurrentCPULocation();
    uint64_t            time;
    SCOREP_RegionHandle region_handle        = SCOREP_INVALID_REGION;
    SCOREP_RegionHandle region_handle_stored = SCOREP_INVALID_REGION;

    if ( callbackId == CUPTI_RUNTIME_TRACE_CBID_INVALID )
    {
        return;
    }

    /* record cuCtxSynchronize in an extra function group */
    if ( callbackId == CUPTI_RUNTIME_TRACE_CBID_cudaDeviceSynchronize_v3020 )
    {
        if ( scorep_cuda_sync_level > SCOREP_CUDA_RECORD_SYNC )
        {
            if ( cbInfo->callbackSite == CUPTI_API_ENTER )
            {
                SCOREP_EnterRegion( cuda_sync_region_handle );
            }
            else if ( cbInfo->callbackSite == CUPTI_API_EXIT )
            {
                SCOREP_ExitRegion( cuda_sync_region_handle );
            }

            return;
        }
    }

    /* get the region handle for the API function */
    region_handle_stored =
        cuda_api_function_get( CUPTI_CB_DOMAIN_RUNTIME_API, callbackId );
    if ( region_handle_stored != SCOREP_INVALID_REGION )
    {
        region_handle = region_handle_stored;
    }
    else
    {
        region_handle = SCOREP_Definitions_NewRegion( cbInfo->functionName, NULL, cuda_runtime_file_handle,
                                                      0, 0, SCOREP_ADAPTER_CUDA, SCOREP_REGION_WRAPPER );

        cuda_api_function_put( CUPTI_CB_DOMAIN_RUNTIME_API, callbackId, region_handle );
    }

    /* time stamp for all the following potential events */
    time = SCOREP_GetClockTicks();

    /*********** write enter and exit records for CUDA runtime API **************/
    if ( cbInfo->callbackSite == CUPTI_API_ENTER )
    {
        SCOREP_Location_EnterRegion( location, time, region_handle );
    }
    else if ( cbInfo->callbackSite == CUPTI_API_EXIT )
    {
        SCOREP_Location_ExitRegion( location, time, region_handle );
    }

    /*
     ************** Semantic function instrumentation *********************
     */

    /* Memory allocation and deallocation tracing */
    if ( scorep_cuda_record_gpumemusage && !record_driver_api )
    {
        switch ( callbackId )
        {
            /********************** CUDA memory allocation ******************************/
            case CUPTI_RUNTIME_TRACE_CBID_cudaMalloc_v3020:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cudaMalloc_v3020_params* params =
                        ( cudaMalloc_v3020_params* )cbInfo->functionParams;

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->devPtr ),
                                        params->size );
                }

                return;
            }

            case CUPTI_RUNTIME_TRACE_CBID_cudaMallocPitch_v3020:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cudaMallocPitch_v3020_params* params =
                        ( cudaMallocPitch_v3020_params* )cbInfo->functionParams;

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->devPtr ),
                                        params->height * ( *( params->pitch ) ) );
                }

                return;
            }

            case CUPTI_RUNTIME_TRACE_CBID_cudaMallocArray_v3020:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cudaMallocArray_v3020_params* params =
                        ( cudaMallocArray_v3020_params* )cbInfo->functionParams;

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->array ),
                                        params->height * params->width );
                }

                return;
            }

            case CUPTI_RUNTIME_TRACE_CBID_cudaMalloc3D_v3020:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cudaMalloc3D_v3020_params* params =
                        ( cudaMalloc3D_v3020_params* )cbInfo->functionParams;

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )params->pitchedDevPtr->ptr,
                                        params->pitchedDevPtr->pitch * params->extent.height * params->extent.depth );
                }

                return;
            }

            case CUPTI_RUNTIME_TRACE_CBID_cudaMalloc3DArray_v3020:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cudaMalloc3DArray_v3020_params* params =
                        ( cudaMalloc3DArray_v3020_params* )cbInfo->functionParams;

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->array ),
                                        params->extent.width * params->extent.height * params->extent.depth );
                }

                return;
            }

            case CUPTI_RUNTIME_TRACE_CBID_cudaFree_v3020:
            {
                if ( cbInfo->callbackSite == CUPTI_API_ENTER )
                {
                    handle_cuda_free( cbInfo->context,
                                      ( uint64_t )( ( cudaFree_v3020_params* )cbInfo->functionParams )->devPtr );
                }

                return;
            }

            case CUPTI_RUNTIME_TRACE_CBID_cudaFreeArray_v3020:
            {
                if ( cbInfo->callbackSite == CUPTI_API_ENTER )
                {
                    handle_cuda_free( cbInfo->context,
                                      ( uint64_t )( ( cudaFreeArray_v3020_params* )cbInfo->functionParams )->array );
                }

                return;
            }

            default:
                break;
        }
    }

    if ( scorep_cuda_record_memcpy && !record_driver_api )
    {
#if defined( SCOREP_CUPTI_ACTIVITY )
        if (
#if defined( SCOREP_CUPTI_EVENTS )
            scorep_cupti_events_enabled ||
#endif
            ( scorep_cuda_sync_level == SCOREP_CUDA_RECORD_SYNC_FULL ) )
#endif
        {
            switch ( callbackId )
            {
                /****************** synchronous CUDA memory copies **************************/
                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy_v3020:
                {
                    cudaMemcpy_v3020_params* params =
                        ( cudaMemcpy_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  params->count, time );

                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy2D_v3020:
                {
                    cudaMemcpy2D_v3020_params* params =
                        ( cudaMemcpy2D_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )( params->height * params->width ),
                                                  time );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyToArray_v3020:
                {
                    cudaMemcpyToArray_v3020_params* params =
                        ( cudaMemcpyToArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )params->count, time );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy2DToArray_v3020:
                {
                    cudaMemcpy2DToArray_v3020_params* params =
                        ( cudaMemcpy2DToArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )( params->height * params->width ),
                                                  time );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyFromArray_v3020:
                {
                    cudaMemcpyFromArray_v3020_params* params =
                        ( cudaMemcpyFromArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )params->count, time );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy2DFromArray_v3020:
                {
                    cudaMemcpy2DFromArray_v3020_params* params =
                        ( cudaMemcpy2DFromArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )( params->height * params->width ),
                                                  time );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyArrayToArray_v3020:
                {
                    cudaMemcpyArrayToArray_v3020_params* params =
                        ( cudaMemcpyArrayToArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )params->count, time );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy2DArrayToArray_v3020:
                {
                    cudaMemcpy2DArrayToArray_v3020_params* params =
                        ( cudaMemcpy2DArrayToArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )( params->height * params->width ),
                                                  time );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyToSymbol_v3020:
                {
                    cudaMemcpyToSymbol_v3020_params* params =
                        ( cudaMemcpyToSymbol_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->symbol,
                                                  ( uint64_t )params->count, time );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyFromSymbol_v3020:
                {
                    cudaMemcpyFromSymbol_v3020_params* params =
                        ( cudaMemcpyFromSymbol_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->symbol, params->dst,
                                                  ( uint64_t )params->count, time );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy3D_v3020:
                {
                    cudaMemcpy3D_v3020_params* params =
                        ( cudaMemcpy3D_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->p->kind,
                                                  params->p->srcArray, params->p->dstArray,
                                                  ( uint64_t )( params->p->extent.height * params->p->extent.width *
                                                                params->p->extent.depth ),
                                                  time );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyPeer_v4000:
                {
                    cudaMemcpyPeer_v4000_params* params =
                        ( cudaMemcpyPeer_v4000_params* )cbInfo->functionParams;

                    CUcontext cuSrcCtx;
                    CUcontext cuDstCtx;
                    int       cudaCurrDev = 0;

                    DISABLE_CUDART_CALLBACKS();
                    DISABLE_CUDRV_CALLBACKS();

                    /* get the currently active device */
                    cudaGetDevice( &cudaCurrDev );

                    /* get the source and destination context */
                    if ( params->srcDevice == cudaCurrDev )
                    {
                        cuSrcCtx = cbInfo->context;
                        if ( cudaSuccess != cudaSetDevice( params->dstDevice ) )
                        {
                            UTILS_WARNING( "[CUPTI Callbacks] Could not set device in P2P mcpy!" );
                            return;
                        }
                        SCOREP_CUDA_DRIVER_CALL( cuCtxGetCurrent( &cuDstCtx ) );
                    }
                    else if ( params->dstDevice == cudaCurrDev )
                    {
                        cuDstCtx = cbInfo->context;
                        if ( cudaSuccess != cudaSetDevice( params->srcDevice ) )
                        {
                            UTILS_WARNING( "[CUPTI Callbacks] Could not set device in P2P mcpy!" );
                            return;
                        }
                        SCOREP_CUDA_DRIVER_CALL( cuCtxGetCurrent( &cuSrcCtx ) );
                    }
                    else
                    {
                        UTILS_WARNING( "[CUPTI Callbacks] Could not identify P2P memcpy!" );
                        return;
                    }

                    /* reset the original active device */
                    if ( cudaSuccess != cudaSetDevice( cudaCurrDev ) )
                    {
                        UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                                     "[CUPTI Callbacks] Could not reset device in P2P mcpy!" );
                    }

                    ENABLE_CUDART_CALLBACKS();
                    ENABLE_CUDRV_CALLBACKS();

                    handle_cuda_memcpy_p2p( cbInfo, cuSrcCtx, cuDstCtx,
                                            ( uint64_t )params->count, time );

                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy3DPeer_v4000:
                {
                    cudaMemcpy3DPeer_v4000_params* params =
                        ( cudaMemcpy3DPeer_v4000_params* )cbInfo->functionParams;

                    CUcontext cuSrcCtx;
                    CUcontext cuDstCtx;
                    int       cudaCurrDev = 0;

                    DISABLE_CUDART_CALLBACKS();
                    DISABLE_CUDRV_CALLBACKS();

                    /* get the currently active device */
                    cudaGetDevice( &cudaCurrDev );

                    /* get the source and destination context */
                    if ( params->p->srcDevice == cudaCurrDev )
                    {
                        cuSrcCtx = cbInfo->context;
                        if ( cudaSuccess != cudaSetDevice( params->p->dstDevice ) )
                        {
                            UTILS_WARNING( "[CUPTI Callbacks] Could not set device in P2P mcyp!" );
                            return;
                        }
                        SCOREP_CUDA_DRIVER_CALL( cuCtxGetCurrent( &cuDstCtx ) );
                    }
                    else if ( params->p->dstDevice == cudaCurrDev )
                    {
                        cuDstCtx = cbInfo->context;
                        if ( cudaSuccess != cudaSetDevice( params->p->srcDevice ) )
                        {
                            UTILS_WARNING( "[CUPTI Callbacks] Could not set device in P2P mcyp!" );
                            return;
                        }
                        SCOREP_CUDA_DRIVER_CALL( cuCtxGetCurrent( &cuSrcCtx ) );
                    }
                    else
                    {
                        UTILS_WARNING( "[CUPTI Callbacks] Could not identify P2P memcpy 3D!" );
                        return;
                    }

                    /* reset the original active device */
                    if ( cudaSuccess != cudaSetDevice( cudaCurrDev ) )
                    {
                        UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                                     "[CUPTI Callbacks] Could not reset device in P2P mcyp!" );
                    }

                    ENABLE_CUDART_CALLBACKS();
                    ENABLE_CUDRV_CALLBACKS();

                    handle_cuda_memcpy_p2p( cbInfo, cuSrcCtx, cuDstCtx,
                                            ( uint64_t )( params->p->extent.height * params->p->extent.width *
                                                          params->p->extent.depth ),
                                            time );
                    return;
                }

                default:
                    break;
            } /* switch(cbid) */
        }     /* synchronization recording enabled */
    }         /* if(scorep_gpu_trace_memcpy) */

#if defined( SCOREP_CUPTI_EVENTS )
#if defined( SCOREP_CUPTI_ACTIVITY )
    if ( scorep_cupti_events_enabled )
#endif
    {
        if ( scorep_cuda_record_kernels )
        {
            switch ( callbackId )
            {
                /************* the CUDA runtime kernel configure call ************/
                case CUPTI_RUNTIME_TRACE_CBID_cudaConfigureCall_v3020:
                {
                    if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                    {
                        handle_cudart_knconf( cbInfo );
                    }

                    return;
                }

                /***** the CUDA runtime kernel launch ******/
                case CUPTI_RUNTIME_TRACE_CBID_cudaLaunch_v3020:
                {
                    handle_cuda_kernel( cbInfo, NULL, 0 );

                    return;
                }

                default:
                    break;
            }
        }
        /****************************************************************************/

        if ( scorep_cuda_record_memcpy )
        {
            switch ( callbackId )
            {
                /******************** asynchronous memory copies **************************/
                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyAsync_v3020:
                {
                    cudaMemcpyAsync_v3020_params* params =
                        ( cudaMemcpyAsync_v3020_params* )cbInfo->functionParams;

                    handle_cuda_runtime_memcpy_async( cbInfo, params->kind,
                                                      ( uint64_t )params->count,
                                                      params->stream );
                    break;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyToArrayAsync_v3020:
                {
                    cudaMemcpyToArrayAsync_v3020_params* params =
                        ( cudaMemcpyToArrayAsync_v3020_params* )cbInfo->functionParams;

                    handle_cuda_runtime_memcpy_async( cbInfo, params->kind,
                                                      ( uint64_t )params->count,
                                                      params->stream );
                    break;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyFromArrayAsync_v3020:
                {
                    cudaMemcpyFromArrayAsync_v3020_params* params =
                        ( cudaMemcpyFromArrayAsync_v3020_params* )cbInfo->functionParams;

                    handle_cuda_runtime_memcpy_async( cbInfo, params->kind,
                                                      ( uint64_t )params->count,
                                                      params->stream );
                    break;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy2DAsync_v3020:
                {
                    cudaMemcpy2DAsync_v3020_params* params =
                        ( cudaMemcpy2DAsync_v3020_params* )cbInfo->functionParams;

                    handle_cuda_runtime_memcpy_async( cbInfo, params->kind,
                                                      ( uint64_t )( params->height * params->width ),
                                                      params->stream );
                    break;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy2DToArrayAsync_v3020:
                {
                    cudaMemcpy2DToArrayAsync_v3020_params* params =
                        ( cudaMemcpy2DToArrayAsync_v3020_params* )cbInfo->functionParams;

                    handle_cuda_runtime_memcpy_async( cbInfo, params->kind,
                                                      ( uint64_t )( params->height * params->width ),
                                                      params->stream );
                    break;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy2DFromArrayAsync_v3020:
                {
                    cudaMemcpy2DFromArrayAsync_v3020_params* params =
                        ( cudaMemcpy2DFromArrayAsync_v3020_params* )cbInfo->functionParams;

                    handle_cuda_runtime_memcpy_async( cbInfo, params->kind,
                                                      ( uint64_t )( params->height * params->width ),
                                                      params->stream );
                    break;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyToSymbolAsync_v3020:
                {
                    cudaMemcpyToSymbolAsync_v3020_params* params =
                        ( cudaMemcpyToSymbolAsync_v3020_params* )cbInfo->functionParams;

                    handle_cuda_runtime_memcpy_async( cbInfo, params->kind,
                                                      ( uint64_t )params->count,
                                                      params->stream );
                    break;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyFromSymbolAsync_v3020:
                {
                    cudaMemcpyFromSymbolAsync_v3020_params* params =
                        ( cudaMemcpyFromSymbolAsync_v3020_params* )cbInfo->functionParams;

                    handle_cuda_runtime_memcpy_async( cbInfo, params->kind,
                                                      ( uint64_t )params->count,
                                                      params->stream );
                    break;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy3DAsync_v3020:
                {
                    cudaMemcpy3DAsync_v3020_params* params =
                        ( cudaMemcpy3DAsync_v3020_params* )cbInfo->functionParams;

                    handle_cuda_runtime_memcpy_async( cbInfo, params->p->kind,
                                                      ( uint64_t )( params->p->extent.height * params->p->extent.width *
                                                                    params->p->extent.depth ),
                                                      params->stream );
                    break;
                }
                /**************************************************************************/

                default:
                    break;
            } /* switch(cbid) */
        }     /* if(scorep_gpu_trace_memcpy) */
    }
#endif
    /****************************************************************************/
}

/*
 * This function handles the CUDA driver API callbacks.
 *
 * @param cbid the ID of the callback function in the given domain
 * @param cbInfo information about the callback
 */
void CUPTIAPI
scorep_cupti_callbacks_driver_api( CUpti_CallbackId          callbackId,
                                   const CUpti_CallbackData* cbInfo )
{
    SCOREP_Location*    location = SCOREP_Location_GetCurrentCPULocation();
    uint64_t            time;
    SCOREP_RegionHandle region_handle        = SCOREP_INVALID_REGION;
    SCOREP_RegionHandle region_handle_stored = SCOREP_INVALID_REGION;

    if ( callbackId == CUPTI_DRIVER_TRACE_CBID_INVALID )
    {
        return;
    }

    /* record cuCtxSynchronize in an extra function group */
    if ( callbackId == CUPTI_DRIVER_TRACE_CBID_cuCtxSynchronize )
    {
        if ( scorep_cuda_sync_level > SCOREP_CUDA_RECORD_SYNC )
        {
            if ( cbInfo->callbackSite == CUPTI_API_ENTER )
            {
                SCOREP_EnterRegion( cuda_sync_region_handle );
            }
            else if ( cbInfo->callbackSite == CUPTI_API_EXIT )
            {
                SCOREP_ExitRegion( cuda_sync_region_handle );
            }

            return;
        }
    }

    if ( record_driver_api )
    {
        /* get the Score-P region handle for the API function */
        region_handle_stored =
            cuda_api_function_get( CUPTI_CB_DOMAIN_DRIVER_API, callbackId );
        if ( region_handle_stored != SCOREP_INVALID_REGION )
        {
            region_handle = region_handle_stored;
        }
        else
        {
            region_handle = SCOREP_Definitions_NewRegion( cbInfo->functionName, NULL, cuda_driver_file_handle,
                                                          0, 0, SCOREP_ADAPTER_CUDA, SCOREP_REGION_WRAPPER );

            cuda_api_function_put( CUPTI_CB_DOMAIN_DRIVER_API, callbackId, region_handle );
        }

        time = SCOREP_GetClockTicks();
        /*********** write enter and exit records for CUDA runtime API **************/
        if ( cbInfo->callbackSite == CUPTI_API_ENTER )
        {
            SCOREP_Location_EnterRegion( location, time, region_handle );
        }
        else if ( cbInfo->callbackSite == CUPTI_API_EXIT )
        {
            SCOREP_Location_ExitRegion( location, time, region_handle );
        }
    }

    if ( scorep_cuda_record_memcpy )
    {
#if defined( SCOREP_CUPTI_ACTIVITY )
        if (
#if defined( SCOREP_CUPTI_EVENTS )
            scorep_cupti_events_enabled ||
#endif
            ( scorep_cuda_sync_level == SCOREP_CUDA_RECORD_SYNC_FULL ) )
#endif
        {
            if ( !record_driver_api )
            {
                time = SCOREP_GetClockTicks();
            }

            /****************** synchronous CUDA memory copies **********************/
            switch ( callbackId )
            {
                case CUPTI_DRIVER_TRACE_CBID_cuMemcpy:
                {
                    cuMemcpy_params* params =
                        ( cuMemcpy_params* )cbInfo->functionParams;

                    handle_cuda_memcpy_default( cbInfo, params->src, params->dst,
                                                params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpy2D_v2:
                {
                    cuMemcpy2D_v2_params* params =
                        ( cuMemcpy2D_v2_params* )cbInfo->functionParams;

                    size_t bytes = params->pCopy->WidthInBytes * params->pCopy->Height;

                    handle_cuda_memcpy_default( cbInfo, params->pCopy->srcDevice,
                                                params->pCopy->dstDevice, time, bytes );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpy3D_v2:
                {
                    cuMemcpy3D_v2_params* params =
                        ( cuMemcpy3D_v2_params* )cbInfo->functionParams;

                    size_t bytes = params->pCopy->WidthInBytes * params->pCopy->Height
                                   * params->pCopy->Depth;

                    handle_cuda_memcpy_default( cbInfo, params->pCopy->srcDevice,
                                                params->pCopy->dstDevice, time, bytes );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyPeer:
                {
                    cuMemcpyPeer_params* params =
                        ( cuMemcpyPeer_params* )cbInfo->functionParams;

                    size_t bytes = params->ByteCount;

                    handle_cuda_memcpy_p2p( cbInfo,
                                            params->srcContext, params->dstContext,
                                            bytes, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpy3DPeer:
                {
                    cuMemcpy3DPeer_params* params =
                        ( cuMemcpy3DPeer_params* )cbInfo->functionParams;

                    size_t bytes = params->pCopy->WidthInBytes
                                   * params->pCopy->Depth * params->pCopy->Height;

                    handle_cuda_memcpy_p2p( cbInfo, params->pCopy->srcContext,
                                            params->pCopy->dstContext,
                                            bytes, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyHtoD:
                {
                    cuMemcpyHtoD_params* params =
                        ( cuMemcpyHtoD_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyHostToDevice,
                                        params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyHtoD_v2:
                {
                    cuMemcpyHtoD_v2_params* params =
                        ( cuMemcpyHtoD_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyHostToDevice,
                                        params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyDtoH:
                {
                    cuMemcpyDtoH_params* params =
                        ( cuMemcpyDtoH_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToHost,
                                        params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyDtoH_v2:
                {
                    cuMemcpyDtoH_v2_params* params =
                        ( cuMemcpyDtoH_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToHost,
                                        params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyDtoD:
                {
                    cuMemcpyDtoD_params* params =
                        ( cuMemcpyDtoD_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToDevice,
                                        params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyDtoD_v2:
                {
                    cuMemcpyDtoD_v2_params* params =
                        ( cuMemcpyDtoD_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToDevice,
                                        params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyHtoA:
                {
                    cuMemcpyHtoA_params* params =
                        ( cuMemcpyHtoA_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyHostToDevice,
                                        params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyHtoA_v2:
                {
                    cuMemcpyHtoA_v2_params* params =
                        ( cuMemcpyHtoA_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyHostToDevice,
                                        params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyAtoH:
                {
                    cuMemcpyAtoH_params* params =
                        ( cuMemcpyAtoH_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToHost,
                                        params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyAtoH_v2:
                {
                    cuMemcpyAtoH_v2_params* params =
                        ( cuMemcpyAtoH_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToHost,
                                        params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyAtoA:
                {
                    cuMemcpyAtoA_params* params =
                        ( cuMemcpyAtoA_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToDevice,
                                        params->ByteCount, time );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyAtoA_v2:
                {
                    cuMemcpyAtoA_v2_params* params =
                        ( cuMemcpyAtoA_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToDevice,
                                        params->ByteCount, time );

                    return;
                }

                default:
                    break;
            } /* switch(cbid) */
        }
    }         /* if(scorep_cuda_record_memcpy)*/

    /************* CUDA kernel launches *************/
#if defined( SCOREP_CUPTI_EVENTS )
#if defined( SCOREP_CUPTI_ACTIVITY )
    if ( scorep_cupti_events_enabled )
#endif
    {
        if ( scorep_cuda_record_kernels )
        {
            switch ( callbackId )
            {
                case CUPTI_DRIVER_TRACE_CBID_cuLaunchGrid:
                {
                    cuLaunchGrid_params* params =
                        ( cuLaunchGrid_params* )cbInfo->functionParams;

                    uint64_t blocks = params->grid_width;

                    if ( params->grid_height != 0 )
                    {
                        blocks *= params->grid_height;
                    }

                    handle_cuda_kernel( cbInfo, NULL, blocks );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuLaunchGridAsync:
                {
                    cuLaunchGridAsync_params* params =
                        ( cuLaunchGridAsync_params* )cbInfo->functionParams;

                    uint64_t blocks = params->grid_width;

                    if ( params->grid_height != 0 )
                    {
                        blocks *= params->grid_height;
                    }

                    handle_cuda_kernel( cbInfo, params->hStream, blocks );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuLaunchKernel:
                {
                    cuLaunchKernel_params* params =
                        ( cuLaunchKernel_params* )cbInfo->functionParams;

                    uint64_t blocks = params->gridDimX;

                    if ( params->gridDimY != 0 )
                    {
                        blocks *= params->gridDimY;
                    }

                    if ( params->gridDimZ != 0 )
                    {
                        blocks *= params->gridDimZ;
                    }

                    handle_cuda_kernel( cbInfo, params->hStream, blocks );

                    return;
                }

                default:
                    break;
            }
        } /* scorep_cuda_record_kernels */
    }
#endif /* SCOREP_CUPTI_EVENTS */

    if ( scorep_cuda_record_gpumemusage )
    {
        switch ( callbackId )
        {
            /********************** CUDA memory allocation ****************************/
            case CUPTI_DRIVER_TRACE_CBID_cuMemAlloc:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cuMemAlloc_params* params =
                        ( cuMemAlloc_params* )cbInfo->functionParams;

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->dptr ),
                                        ( size_t )params->bytesize );
                }

                break;
            }

            case CUPTI_DRIVER_TRACE_CBID_cuMemAlloc_v2:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cuMemAlloc_v2_params* params =
                        ( cuMemAlloc_v2_params* )cbInfo->functionParams;

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->dptr ),
                                        ( size_t )params->bytesize );
                }

                break;
            }

            case CUPTI_DRIVER_TRACE_CBID_cuMemAllocPitch:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cuMemAllocPitch_params* params =
                        ( cuMemAllocPitch_params* )cbInfo->functionParams;

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->dptr ),
                                        ( size_t )( params->Height * ( *( params->pPitch ) ) ) );
                }

                break;
            }

            case CUPTI_DRIVER_TRACE_CBID_cuMemAllocPitch_v2:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cuMemAllocPitch_v2_params* params =
                        ( cuMemAllocPitch_v2_params* )cbInfo->functionParams;

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->dptr ),
                                        ( size_t )( params->Height * ( *( params->pPitch ) ) ) );
                }

                break;
            }

            case CUPTI_DRIVER_TRACE_CBID_cuArrayCreate:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cuArrayCreate_params* params =
                        ( cuArrayCreate_params* )cbInfo->functionParams;
                    size_t sizeInBytes = ( size_t )( params->pAllocateArray->dummy );

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->pHandle ),
                                        sizeInBytes );
                }

                break;
            }

            case CUPTI_DRIVER_TRACE_CBID_cuArrayCreate_v2:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cuArrayCreate_v2_params* params =
                        ( cuArrayCreate_v2_params* )cbInfo->functionParams;
                    size_t sizeInBytes = ( size_t )( params->pAllocateArray->Width *
                                                     params->pAllocateArray->NumChannels );

                    if ( params->pAllocateArray->Height != 0 )
                    {
                        sizeInBytes *= params->pAllocateArray->Height;
                    }

                    if ( params->pAllocateArray->Format == CU_AD_FORMAT_UNSIGNED_INT16 ||
                         params->pAllocateArray->Format == CU_AD_FORMAT_SIGNED_INT16 ||
                         params->pAllocateArray->Format == CU_AD_FORMAT_HALF )
                    {
                        sizeInBytes *= 2;
                    }

                    if ( params->pAllocateArray->Format == CU_AD_FORMAT_UNSIGNED_INT32 ||
                         params->pAllocateArray->Format == CU_AD_FORMAT_SIGNED_INT32 ||
                         params->pAllocateArray->Format == CU_AD_FORMAT_FLOAT )
                    {
                        sizeInBytes *= 4;
                    }

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->pHandle ),
                                        sizeInBytes );
                }

                break;
            }

            case CUPTI_DRIVER_TRACE_CBID_cuArray3DCreate:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cuArray3DCreate_params* params =
                        ( cuArray3DCreate_params* )cbInfo->functionParams;
                    size_t sizeInBytes = ( size_t )( params->pAllocateArray->dummy );

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->pHandle ),
                                        sizeInBytes );
                }

                break;
            }

            case CUPTI_DRIVER_TRACE_CBID_cuArray3DCreate_v2:
            {
                if ( cbInfo->callbackSite == CUPTI_API_EXIT )
                {
                    cuArray3DCreate_v2_params* params =
                        ( cuArray3DCreate_v2_params* )cbInfo->functionParams;
                    size_t sizeInBytes = ( size_t )( params->pAllocateArray->Width *
                                                     params->pAllocateArray->NumChannels );

                    if ( params->pAllocateArray->Height != 0 )
                    {
                        sizeInBytes *= params->pAllocateArray->Height;
                    }

                    if ( params->pAllocateArray->Depth != 0 )
                    {
                        sizeInBytes *= params->pAllocateArray->Depth;
                    }

                    if ( params->pAllocateArray->Format == CU_AD_FORMAT_UNSIGNED_INT16 ||
                         params->pAllocateArray->Format == CU_AD_FORMAT_SIGNED_INT16 ||
                         params->pAllocateArray->Format == CU_AD_FORMAT_HALF )
                    {
                        sizeInBytes *= 2;
                    }

                    if ( params->pAllocateArray->Format == CU_AD_FORMAT_UNSIGNED_INT32 ||
                         params->pAllocateArray->Format == CU_AD_FORMAT_SIGNED_INT32 ||
                         params->pAllocateArray->Format == CU_AD_FORMAT_FLOAT )
                    {
                        sizeInBytes *= 4;
                    }

                    handle_cuda_malloc( cbInfo->context,
                                        ( uint64_t )*( params->pHandle ),
                                        sizeInBytes );
                }

                break;
            }

            case CUPTI_DRIVER_TRACE_CBID_cuMemFree:
            {
                if ( cbInfo->callbackSite == CUPTI_API_ENTER )
                {
                    cuMemFree_params* params =
                        ( cuMemFree_params* )cbInfo->functionParams;

                    handle_cuda_free( cbInfo->context, ( uint64_t )params->dptr );
                }

                break;
            }

            case CUPTI_DRIVER_TRACE_CBID_cuMemFree_v2:
            {
                if ( cbInfo->callbackSite == CUPTI_API_ENTER )
                {
                    cuMemFree_v2_params* params =
                        ( cuMemFree_v2_params* )cbInfo->functionParams;

                    handle_cuda_free( cbInfo->context, ( uint64_t )params->dptr );
                }

                break;
            }

            case CUPTI_DRIVER_TRACE_CBID_cuArrayDestroy:
            {
                if ( cbInfo->callbackSite == CUPTI_API_ENTER )
                {
                    cuArrayDestroy_params* params =
                        ( cuArrayDestroy_params* )cbInfo->functionParams;

                    handle_cuda_free( cbInfo->context, ( uint64_t )params->hArray );
                }

                break;
            }

            default:
                break;
        }
    }
}

#if defined( SCOREP_CUPTI_ACTIVITY )

/*
 * This callback function is used to handle synchronization calls.
 *
 * @param cbid the ID of the callback function in the given domain
 * @param syncData synchronization data (CUDA context, CUDA stream)
 */
void
scorep_cupti_callbacks_sync( CUpti_CallbackId             cbid,
                             const CUpti_SynchronizeData* syncData )
{
    if ( CUPTI_CBID_SYNCHRONIZE_CONTEXT_SYNCHRONIZED == cbid )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                            "[CUPTI Callbacks] Synchronize called for CUDA context %d",
                            syncData->context );

#if defined( SCOREP_CUPTI_ACTIVITY )
#if defined( SCOREP_CUPTI_EVENTS )
        if ( !scorep_cupti_events_enabled )
#endif
        {
            SCOREP_CUPTI_LOCK();
            scorep_cupti_activity_context_flush(
                scorep_cupti_context_get_nolock( syncData->context ) );
            SCOREP_CUPTI_UNLOCK();
        }
#endif
    }
}

/*
 * This callback function is used to handle resource usage.
 *
 * @param callbackId the ID of the callback function in the given domain
 * @param resourceData resource information (CUDA context, CUDA stream)
 */
void
scorep_cupti_callbacks_resource( CUpti_CallbackId          callbackId,
                                 const CUpti_ResourceData* resourceData )
{
    switch ( callbackId )
    {
        /********************** CUDA memory allocation ******************************/
        case CUPTI_CBID_RESOURCE_CONTEXT_CREATED:
        {
            scorep_cupti_context_t* context     = NULL;
            CUcontext               cudaContext = resourceData->context;

            context = scorep_cupti_context_get_create( cudaContext );

            /*
             * Create the Score-P CUPTI callbacks context for CUPTI >3
             * (needed for concurrent kernel tracing)
             */
#if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
            if ( context->callbacks == NULL )
            {
                context->callbacks = scorep_cupticb_create_callbacks_context( context );
            }
#endif
#if defined( SCOREP_CUPTI_EVENTS )
            if ( !scorep_cupti_events_enabled )
#endif
            {
                /* add the context without tracing CUDA driver API calls, if enabled */
                DISABLE_CUDRV_CALLBACKS();
                scorep_cupti_activity_context_setup( context );
                ENABLE_CUDRV_CALLBACKS();
            }

            break;
        }

        case CUPTI_CBID_RESOURCE_CONTEXT_DESTROY_STARTING:
        {
            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                                "[CUPTI Callbacks] Destroying context %d ...",
                                resourceData->context );

#if defined( SCOREP_CUPTI_EVENTS )
            if ( !scorep_cupti_events_enabled )
#endif
            {
                scorep_cupti_context_t* context = NULL;
                /* Only flush the activities of the context. The user code has to ensure,
                   that the context is synchronized */
                SCOREP_CUPTI_LOCK();
                context = scorep_cupti_context_get_nolock( resourceData->context );

                scorep_cupti_activity_context_flush( context );

                /* If device reuse is set, mark the context as destroyed. Memory is not
                   deallocated, but buffer dequeued. */
                if ( scorep_cuda_device_reuse )
                {
                    scorep_cupti_context_set_destroyed( context );
                }

                SCOREP_CUPTI_UNLOCK();
            }

            break;
        }

#if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
        case CUPTI_CBID_RESOURCE_STREAM_CREATED:
        {
            if ( scorep_cuda_record_kernels &&
                 ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_CONCURRENT_KERNEL )
                   != SCOREP_CUDA_FEATURE_CONCURRENT_KERNEL ) )
            {
                scorep_cupti_context_t* context = scorep_cupti_context_get_create( resourceData->context );

                /* if the device is capable of concurrent kernels */
                if ( context->callbacks->concurrent_kernels )
                {
                    context->callbacks->streams_created++;

                    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                                        "[CUPTI Callbacks] Creating stream %d (context %d)",
                                        context->callbacks->streams_created, resourceData->context );

                    if (
#if defined( SCOREP_CUPTI_EVENTS )
                        !scorep_cupti_events_enabled &&
#endif
                        context->callbacks->streams_created > 1 )
                    {
                        scorep_cupti_activity_enable_concurrent_kernel( context );
                    }
                }
            }


            break;
        }
#endif  /* CUPTI_API_VERSION >= 3 */

        case CUPTI_CBID_RESOURCE_STREAM_DESTROY_STARTING:
        {
            if ( scorep_cuda_stream_reuse )
            {
                uint32_t strmID;

#if ( defined( CUDA_VERSION ) && ( CUDA_VERSION < 5000 ) )
                /* implicitly flush context activities via cuCtxSynchronize() */
                {
                    SCOREP_EnterRegion( cuda_sync_region_handle );


                    DISABLE_CUDRV_CALLBACKS();
                    SCOREP_CUDA_DRIVER_CALL( cuCtxSynchronize() );
                    ENABLE_CUDRV_CALLBACKS();

                    SCOREP_ExitRegion( cuda_sync_region_handle );
                }
#else
                /* TODO: NVIDIA bug??? */
                /* cuCtxSynchronize() runs into a lock here, therefore just flush */
#if defined( SCOREP_CUPTI_EVENTS )
                if ( !scorep_cupti_events_enabled )
#endif
                {
                    SCOREP_CUPTI_LOCK();
                    scorep_cupti_activity_context_flush(
                        scorep_cupti_context_get_nolock( resourceData->context ) );
                    SCOREP_CUPTI_UNLOCK();
                }
#endif
#if defined( SCOREP_CUPTI_EVENTS )
                if ( !scorep_cupti_events_enabled )
#endif
                {
                    /* get the stream id from stream type */
                    SCOREP_CUPTI_CALL( cuptiGetStreamId( resourceData->context,
                                                         resourceData->resourceHandle.stream,
                                                         &strmID ) );

                    /* mark the stream as destroyed to be available for reuse */
                    scorep_cupti_stream_set_destroyed( resourceData->context, strmID );
                }

                UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                                    "[CUPTI Callbacks] Destroying stream %d (context %d)",
                                    strmID, resourceData->context );
            }

            break;
        }

        default:
            break;
    }
}
#endif /* SCOREP_CUPTI_ACTIVITY */

/*
 * Synchronize the current CUDA context and record the synchronization as
 * necessary.
 *
 * @param location Score-P process/thread location
 *
 * @return Score-P time stamp after synchronization
 */
static uint64_t
scorep_cupticb_synchronize_context( SCOREP_Location* location )
{
    uint64_t time;

    if ( is_driver_domain_enabled )
    {
        SCOREP_CUDA_DRIVER_CALL( cuCtxSynchronize() );
        time = SCOREP_GetClockTicks();
    }
    else
    {
        if ( scorep_cuda_sync_level > SCOREP_CUDA_RECORD_SYNC )
        {
            time = SCOREP_GetClockTicks();
            SCOREP_Location_EnterRegion( location, time, cuda_sync_region_handle );
        }

        SCOREP_CUDA_DRIVER_CALL( cuCtxSynchronize() );
        time = SCOREP_GetClockTicks();

        if ( scorep_cuda_sync_level > SCOREP_CUDA_RECORD_SYNC )
        {
            SCOREP_Location_ExitRegion( location, time, cuda_sync_region_handle );
        }
    }
    return time;
}

/*
 * This function handles the cudaConfigureCall callback.
 * Kernel configuration data are written on the kernel configure stack.
 *
 * @param cbInfo information about the callback
 */
static void
handle_cudart_knconf( const CUpti_CallbackData* cbInfo )
{
    /* configure call parameter have to be saved for kernel launch on a per
     * thread basis. */
    scorep_cupti_kernel_t*          kernel_params = NULL;
    cudaConfigureCall_v3020_params* params        =
        ( cudaConfigureCall_v3020_params* )cbInfo->functionParams;
    scorep_cupti_context_t*   context           = scorep_cupti_context_get_create( cbInfo->context );
    scorep_cupti_callbacks_t* context_callbacks =
#if defined( SCOREP_CUPTI_ACTIVITY )
        scorep_cupticb_create_callbacks_context( context );
#else
        scorep_cupticb_create_callbacks_context( context, ( CUstream )( params->stream ) );
#endif

    /* Is their already a kernel configured? */
    if ( NULL == context_callbacks->kernel_data ) /* NO */
    {                                             /* allocate parameter memory for first kernel, if not yet done */
        kernel_params = ( scorep_cupti_kernel_t* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cupti_kernel_t ) );

        kernel_params->up   = NULL;
        kernel_params->down = NULL;

        context_callbacks->kernel_data = kernel_params;
    }
    else /* YES */
    { /* for the bottom element, which has been invalidated (reuse it) */
        if ( context_callbacks->kernel_data->threads_per_block == SCOREP_CUPTI_NO_ID )
        {
            kernel_params = context_callbacks->kernel_data;
        }
        else
        {
            /* there may have been some kernels configured (allocated) */
            if ( NULL == context_callbacks->kernel_data->up )
            {
                kernel_params = ( scorep_cupti_kernel_t* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cupti_kernel_t ) );

                kernel_params->up   = NULL;
                kernel_params->down = context_callbacks->kernel_data;
            }
            else
            {
                /* just use the already allocated kernel element */
                kernel_params = context_callbacks->kernel_data->up;
            }

            /* set the current kernel */
            context_callbacks->kernel_data = kernel_params;
        }
    }

    kernel_params->blocks_per_grid = params->gridDim.x * params->gridDim.y
                                     * params->gridDim.z;
    kernel_params->threads_per_block = params->blockDim.x * params->blockDim.y
                                       * params->blockDim.z;

    kernel_params->cuda_stream = params->stream;
}

/*
 * This function can be called at the beginning and end of a CUDA kernel launch.
 * Time stamps will be written to the corresponding CUDA stream.
 * !!! The kernel has to be configured (cudaConfigureCall) !!!

 * @param cbInfo information about the callback
 * @param cuStrm the CUDA stream
 * @param blocks number of blocks executed with this kernel
 */
static void
handle_cuda_kernel( const CUpti_CallbackData* cbInfo,
                    CUstream cudaStream, uint64_t blocks )
{
    uint64_t time;

    if ( cbInfo->callbackSite == CUPTI_API_ENTER )
    {
        SCOREP_RegionHandle           kernel_region     = SCOREP_INVALID_REGION;
        const char*                   symName           = cbInfo->symbolName;
        scorep_cupti_stream_t*        stream            = NULL;
        scorep_cuda_kernel_hash_node* hn                = NULL;
        scorep_cupti_context_t*       context           = NULL;
        scorep_cupti_callbacks_t*     context_callbacks = NULL;

        if ( cbInfo->symbolName == NULL )
        {
            symName = "_Z7noSymbolName";
        }

        /* get the Score-P region ID for the kernel */
        hn = scorep_cupti_kernel_hash_get( symName );

        if ( hn )
        {
            kernel_region = hn->region;
        }
        else
        {
            char* knName = NULL;

            knName = SCOREP_DEMANGLE_CUDA_KERNEL( symName );

            if ( knName == NULL || *knName == '\0' )
            {
                knName = ( char* )symName;

                if ( knName == NULL )
                {
                    knName = "unknownKernel";
                }
            }

            SCOREP_CUPTI_LOCK();
            kernel_region = SCOREP_Definitions_NewRegion( knName, NULL,
                                                          scorep_cupti_kernel_file_handle, 0, 0, SCOREP_ADAPTER_CUDA,
                                                          SCOREP_REGION_FUNCTION );
            SCOREP_CUPTI_UNLOCK();

            hn = scorep_cupti_kernel_hash_put( symName, kernel_region );
        }

        /* get the Score-P CUPTI context the kernel is running on */
        if ( blocks != 0 ) /* if called from driver API launch */
        {
            context = scorep_cupti_context_get_create( cbInfo->context );
        }
        else
        {
            context = scorep_cupti_context_get( cbInfo->context );
        }

        if ( NULL == context )
        {
            UTILS_WARNING( "[CUPTI Callbacks] No context available!" );
            return;
        }

        /* check if current host thread is the same as the context host thread */
        if ( context->scorep_host_location != SCOREP_Location_GetCurrentCPULocation() )
        {
            UTILS_WARNING( "[CUPTI Callbacks] Host thread of context changed!" );
            return;
        }

        context_callbacks = context->callbacks;

        /* if called from driver API launch, the callback context may not be created */
        if ( blocks != 0 && NULL == context_callbacks )
        {
            context_callbacks =
#if defined( SCOREP_CUPTI_ACTIVITY )
                scorep_cupticb_create_callbacks_context( context );
#else
                scorep_cupticb_create_callbacks_context( context, cudaStream );
#endif
        }

        if ( NULL == context_callbacks )
        {
            UTILS_WARNING( "[CUPTI Callbacks] No callbacks context available!" );
            return;
        }

        /* called from CUDA runtime API */
        if ( NULL == context_callbacks->kernel_data && blocks == 0 )
        {
            UTILS_WARNING( "[CUPTI Callbacks] No kernel parameter set! "
                           "cudaConfigureCall() failed?" );
            return;
        }

        {
            uint32_t cuStrmID = SCOREP_CUPTI_NO_STREAM_ID;

            if ( blocks == 0 )
            {
                cudaStream = context_callbacks->kernel_data->cuda_stream;
            }

#if defined( SCOREP_CUPTI_ACTIVITY )
            SCOREP_CUPTI_CALL( cuptiGetStreamId( cbInfo->context, cudaStream, &cuStrmID ) );
#endif

            stream = scorep_cupti_stream_get_create( context, cudaStream, cuStrmID );
        }

        /* save address into 64 Bit correlation value for exit callback */
        *cbInfo->correlationData = ( uint64_t )stream;

#if defined( SCOREP_CUPTI_EVENTS )
        if ( scorep_cupti_events_enabled && !scorep_cupti_activity_is_buffer_empty( context->cuda_context ) )
        {
            /* write the event records */

            time = scorep_cupticb_synchronize_context( context->scorep_host_location );
        }
        else
#endif

        time = SCOREP_GetClockTicks();

        /* write Score-P kernel start events */
        if ( scorep_cuda_record_idle )
        {
            SCOREP_Location_ExitRegion( context->streams->scorep_location, time, kernel_region );
        }

        SCOREP_Location_EnterRegion( stream->scorep_location, time, kernel_region );

        if ( scorep_cuda_record_kernels == SCOREP_CUDA_KERNEL_AND_COUNTER )
        {
            if ( blocks == 0 )
            {
                SCOREP_Location_TriggerCounterUint64( stream->scorep_location, time,
                                                      scorep_cupti_sampling_set_blocks_per_grid, context_callbacks->kernel_data->blocks_per_grid );
                SCOREP_Location_TriggerCounterUint64( stream->scorep_location, time,
                                                      scorep_cupti_sampling_set_blocks_per_grid, context_callbacks->kernel_data->threads_per_block );
                SCOREP_Location_TriggerCounterUint64( stream->scorep_location, time,
                                                      scorep_cupti_sampling_set_blocks_per_grid, context_callbacks->kernel_data->threads_per_block *
                                                      context_callbacks->kernel_data->blocks_per_grid );
            }
            else
            {
                SCOREP_Location_TriggerCounterUint64( stream->scorep_location, time,
                                                      scorep_cupti_sampling_set_blocks_per_grid, context_callbacks->kernel_data->blocks_per_grid );
            }
        }

#if defined( SCOREP_CUPTI_EVENTS )
        if ( scorep_cupti_events_enabled )
        {
            // TODO: reset PAPI CUDA counters
        }
#endif

        /* Only for CUDA runtime API */
        if ( blocks == 0 )
        {
            /* take the configure parameters from stack or invalidate it */
            if ( NULL != context_callbacks->kernel_data->down )
            {
                context_callbacks->kernel_data = context_callbacks->kernel_data->down;
            }
            else
            {
                /* use this parameter for invalidation */
                context_callbacks->kernel_data->threads_per_block = SCOREP_CUPTI_NO_ID;
            }
        }
    }

    if ( cbInfo->callbackSite == CUPTI_API_EXIT )
    {
        scorep_cupti_stream_t*  stream          = ( scorep_cupti_stream_t* )( *cbInfo->correlationData );
        SCOREP_Location*        stream_location = stream->scorep_location;
        scorep_cupti_context_t* context         = scorep_cupti_context_get( cbInfo->context );

        if ( NULL == context )
        {
            UTILS_WARNING( "[CUPTI Callbacks] No CUPTI context available!" );
            return;
        }

        /* check if current host thread is the same as the context host thread */
        if ( context->scorep_host_location != SCOREP_Location_GetCurrentCPULocation() )
        {
            UTILS_WARNING( "[CUPTI Callbacks] Host thread of context changed!" );
            return;
        }

#if defined( SCOREP_CUPTI_EVENTS )
        if ( scorep_cupti_events_enabled )
        {
            if ( NULL == context->events )
            {
                UTILS_WARNING( "[CUPTI Callbacks] No events context available!" );
            }
            else
            {
                time = scorep_get_timestamp( location );
                SCOREP_Location_EnterRegion( context->scorep_host_location, &time, scorep_gpu_rid_sync );

                if ( scorep_cupti_events_sampling )
                {
                    CUresult ret = CUDA_SUCCESS;

                    DISABLE_CUDRV_CALLBACKS();
                    /* sampling of CUPTI counter values */
                    do
                    {
                        // TODO: get and write PAPI CUDA counter value
                        ret = cuStreamQuery( stream->cuda_stream );
                    }
                    while ( ret != CUDA_SUCCESS );
                    ENABLE_CUDRV_CALLBACKS();
                }
                else
                {
                    /* synchronize context before
                       (assume that the given context is the current one) */
                    time = scorep_cupticb_synchronize_context( context->scorep_host_location );
                }

                // TODO: set counter to zero
                SCOREP_Location_ExitRegion( context->scorep_host_location, &time, scorep_gpu_rid_sync );
            } /* NULL != context->events */
        }
        else
#endif  /* SCOREP_CUPTI_EVENTS */
        {
            if ( scorep_cuda_sync_level )
            {
                time = scorep_cupticb_synchronize_context( context->scorep_host_location );
            }
        }

        /* write Score-P kernel stop events */
        if ( scorep_cuda_record_kernels == SCOREP_CUDA_KERNEL_AND_COUNTER )
        {
            SCOREP_Location_TriggerCounterUint64( stream_location, time,
                                                  scorep_cupti_sampling_set_blocks_per_grid, 0 );

            if ( blocks == 0 )
            {
                SCOREP_Location_TriggerCounterUint64( stream_location, time,
                                                      scorep_cupti_sampling_set_threads_per_block, 0 );
                SCOREP_Location_TriggerCounterUint64( stream_location, time,
                                                      scorep_cupti_sampling_set_threads_per_kernel, 0 );
            }
        }

        SCOREP_Location_ExitRegion( stream_location, time, ( ( scorep_cuda_kernel_hash_node* )scorep_cupti_kernel_hash_get( cbInfo->symbolName ) )->region );

        if ( scorep_cuda_record_idle )
        {
            SCOREP_Location_EnterRegion( context->streams->scorep_location, time,
                                         scorep_cupti_idle_region_handle );
        }
    }
}

/*
 * Create and add the default stream to the given Score-P CUPTI context
 *
 * @param context pointer to the Score-P CUPTI context
 */
static void
scorep_cupticb_create_default_stream( scorep_cupti_context_t* context )
{
    uint32_t cuStrmID = SCOREP_CUPTI_NO_STREAM_ID;

#if defined( SCOREP_CUPTI_ACTIVITY )
    /* create a Score-P CUPTI stream */
    if ( context->activity == NULL )
    {
        SCOREP_CUPTI_CALL( cuptiGetStreamId( context->cuda_context, NULL, &cuStrmID ) );
    }
    else
    {
        cuStrmID = context->activity->default_strm_id;
    }
#endif /* SCOREP_CUPTI_ACTIVITY */

    /* this will create a valid Score-P stream object if !defined(SCOREP_CUPTI_ACTIVITY) */
    context->streams = scorep_cupti_stream_create( context, SCOREP_CUPTI_NO_STREAM,
                                                   cuStrmID );
}

/*
 * Increases the "Allocated CUDA memory" counter.
 *
 * @param cudaContext CUDA context
 * @param address pointer to the allocated memory (needed for handle_cuda_free())
 * @param size the number of bytes allocated
 */
static void
handle_cuda_malloc( CUcontext cudaContext, uint64_t address, size_t size )
{
    scorep_cupti_gpumem_t*  gpu_memory = NULL;
    scorep_cupti_context_t* context    = NULL;

    if ( address == ( uint64_t )NULL )
    {
        return;
    }

    gpu_memory = ( scorep_cupti_gpumem_t* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cupti_gpumem_t ) );


    /* set address and size of the allocated GPU memory */
    gpu_memory->address = address;
    gpu_memory->size    = size;

    /* lock the work on the context */
    SCOREP_CUPTI_LOCK();

    /* get the context without additional locks or malloc tracing checks */
    context = scorep_cupti_context_get_nolock( cudaContext );
    if ( context == NULL )
    {
        context = scorep_cupti_context_create( cudaContext, SCOREP_CUPTI_NO_DEVICE,
                                               SCOREP_CUPTI_NO_CONTEXT_ID,
                                               SCOREP_CUPTI_NO_DEVICE_ID );
        scorep_cupti_context_prepend( context );
    }

    /* add malloc entry to list */
    gpu_memory->next      = context->cuda_mallocs;
    context->cuda_mallocs = gpu_memory;

    /* increase the context global allocated memory counter */
    context->gpu_memory_allocated += size;

    /* check if first CUDA stream is available */
    if ( context->streams == NULL )
    {
        scorep_cupticb_create_default_stream( context );

        SCOREP_Location_TriggerCounterUint64( context->streams->scorep_location,
                                              context->streams->scorep_last_timestamp, scorep_cupti_sampling_set_gpumemusage, 0 );
    }

    SCOREP_CUPTI_UNLOCK();

#if defined( SCOREP_CUPTI_ACTIVITY )
# if defined( SCOREP_CUPTI_EVENTS )
    if ( !scorep_cupti_events_enabled )
# endif
    {
        /* synchronize context before (implicit activity buffer flush)
           (assume that the given context is the current one) */
        if ( !( scorep_cuda_record_kernels && scorep_cuda_record_memcpy ) ||
             ( ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy ) &&
               !scorep_cupti_activity_is_buffer_empty( context->cuda_context ) ) )
        {
            scorep_cupticb_synchronize_context( context->scorep_host_location );
        }
    }
#endif /* SCOREP_CUPTI_ACTIVITY */

    /* write counter value */
    {
        uint64_t timestamp = SCOREP_GetClockTicks();

        SCOREP_Location_TriggerCounterUint64( context->streams->scorep_location,
                                              timestamp, scorep_cupti_sampling_set_gpumemusage, ( uint64_t )( context->gpu_memory_allocated ) );
    }
}

/*
 * Decreases the "Allocated CUDA memory" counter.
 *
 * @param cudaContext CUDA context
 * @param devicePtr pointer to the allocated memory
 */
static void
handle_cuda_free( CUcontext cudaContext, uint64_t devicePtr )
{
    scorep_cupti_context_t* context        = NULL;
    scorep_cupti_gpumem_t*  current_gpumem = NULL;
    scorep_cupti_gpumem_t*  last_gpumem    = NULL;

    if ( devicePtr == ( uint64_t )NULL )
    {
        return;
    }

    /* lock the work on the context */
    SCOREP_CUPTI_LOCK();

    /* get the context without additional locks or malloc tracing checks */
    context = scorep_cupti_context_get_nolock( cudaContext );
    if ( context == NULL )
    {
        context = scorep_cupti_context_create( cudaContext, SCOREP_CUPTI_NO_DEVICE,
                                               SCOREP_CUPTI_NO_CONTEXT_ID, SCOREP_CUPTI_NO_DEVICE_ID );
        scorep_cupti_context_prepend( context );
    }

    SCOREP_CUPTI_UNLOCK();

#if defined( SCOREP_CUPTI_ACTIVITY )
# if defined( SCOREP_CUPTI_EVENTS )
    if ( !scorep_cupti_events_enabled )
# endif
    {
        /* synchronize context before
           (assume that the given context is the current one) */
        if ( !( scorep_cuda_record_kernels && scorep_cuda_record_memcpy ) ||
             ( ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy ) &&
               !scorep_cupti_activity_is_buffer_empty( context->cuda_context ) ) )
        {
            scorep_cupticb_synchronize_context( context->scorep_host_location );
        }
    }
#endif /* SCOREP_CUPTI_ACTIVITY */

    SCOREP_CUPTI_LOCK();
    current_gpumem = context->cuda_mallocs;
    last_gpumem    = context->cuda_mallocs;
    while ( current_gpumem != NULL )
    {
        if ( devicePtr == current_gpumem->address )
        {
            /* decrease allocated counter value and write it */
            uint64_t timestamp = SCOREP_GetClockTicks();
            context->gpu_memory_allocated -= current_gpumem->size;
            SCOREP_Location_TriggerCounterUint64( context->streams->scorep_location,
                                                  timestamp, scorep_cupti_sampling_set_gpumemusage, ( uint64_t )( context->gpu_memory_allocated ) );

            /* set pointer over current element to next one */
            last_gpumem->next = current_gpumem->next;

            /* if current element is the first list entry, set the list entry */
            if ( current_gpumem == context->cuda_mallocs )
            {
                context->cuda_mallocs = current_gpumem->next;
            }

            /* free Score-P memory of CUDA malloc */
            current_gpumem->next = NULL;
            /* free is implicitly done by Score-P memory management */
            /*free(curMalloc);*/
            current_gpumem = NULL;

            /* set mallocList to NULL, if last element freed */
            if ( context->gpu_memory_allocated == 0 )
            {
                context->cuda_mallocs = NULL;
            }

            SCOREP_CUPTI_UNLOCK();
            return;
        }

        last_gpumem    = current_gpumem;
        current_gpumem = current_gpumem->next;
    }

    SCOREP_CUPTI_UNLOCK();

    UTILS_WARNING( "[CUPTI Callbacks] Free CUDA memory, which has not been allocated!" );
}

/*
 * Retrieve the direction of CUDA memory copies, based on the source and
 * destination memory type.
 * (source: host, destination: device -> cudaMemcpyHostToDevice)
 *
 * @param srcMemType memory type of source
 * @param dstMemType memory type of destination
 *
 * @return kind the cudaMemcpyKind
 */
static enum cudaMemcpyKind
get_cuda_memcpy_kind( CUmemorytype srcMemType,
                      CUmemorytype dstMemType )
{
    if ( CU_MEMORYTYPE_HOST == srcMemType )
    {
        if ( CU_MEMORYTYPE_DEVICE == dstMemType ||
             CU_MEMORYTYPE_ARRAY == dstMemType )
        {
            return cudaMemcpyHostToDevice;
        }
        else if ( CU_MEMORYTYPE_HOST == dstMemType )
        {
            return cudaMemcpyHostToHost;
        }
    }
    else
    {
        if ( CU_MEMORYTYPE_DEVICE == srcMemType ||
             CU_MEMORYTYPE_ARRAY == srcMemType )
        {
            if ( CU_MEMORYTYPE_DEVICE == dstMemType ||
                 CU_MEMORYTYPE_ARRAY == dstMemType )
            {
                return cudaMemcpyDeviceToDevice;
            }
            else
            {
                if ( CU_MEMORYTYPE_HOST == dstMemType )
                {
                    return cudaMemcpyDeviceToHost;
                }
            }
        }
    }

    return cudaMemcpyDefault;
}

/*
 * Handle synchronous CUDA memory copy calls.
 *
 * @param cbInfo information about the callback
 * @param kind direction of the data transfer
 * @param bytes number of bytes transfered
 * @param time the start/stop time of the synchronous transfer
 */
static void
handle_cuda_memcpy( const CUpti_CallbackData* cbInfo,
                    enum cudaMemcpyKind kind,
                    uint64_t bytes, uint64_t time )
{
    SCOREP_Location* stream_location;
    SCOREP_Location* host_location = SCOREP_Location_GetCurrentCPULocation();

    if ( cbInfo->callbackSite == CUPTI_API_ENTER )
    {
        scorep_cupti_context_t* context = NULL;
        scorep_cupti_stream_t*  stream  = NULL;

#if defined( SCOREP_CUPTI_ACTIVITY )
        /* disable activity API for memory copy recording */
# if defined( SCOREP_CUPTI_EVENTS )
        if ( !scorep_cupti_events_enabled )
#endif
        {
            SCOREP_CUPTI_CALL( cuptiActivityDisable( CUPTI_ACTIVITY_KIND_MEMCPY ) );
        }
#endif  /* SCOREP_CUPTI_ACTIVITY */

        /* get the Score-P thread ID the kernel is running on */
        {
            context = scorep_cupti_context_get_create( cbInfo->context );

            if ( context->streams == NULL )
            {
                scorep_cupticb_create_default_stream( context );
            }

            stream          = context->streams;
            stream_location = stream->scorep_location;

            /* save address into 64 Bit correlation value for exit callback */
            *cbInfo->correlationData = ( uint64_t )stream;

            /* check if current host thread is the same as the context host thread */
            if ( context->scorep_host_location != host_location )
            {
                UTILS_WARNING( "[CUPTI Callbacks] Host thread of context changed! "
                               "Skipping memory copy!" );
                return;
            }

            /* synchronize to get host waiting time */
            if ( scorep_cuda_sync_level )
            {
                if ( !scorep_cuda_record_kernels ||
                     !scorep_cupti_activity_is_buffer_empty( context->cuda_context ) )
                {
                    time = scorep_cupticb_synchronize_context( context->scorep_host_location );
                }
            }

            /* pure idle time */
            if ( scorep_cuda_record_idle == SCOREP_CUDA_PURE_IDLE )
            {
#if defined( SCOREP_CUPTI_ACTIVITY )
                if ( NULL != context->activity )
                {
                    if ( context->activity->gpu_idle )
                    {
                        SCOREP_Location_ExitRegion( context->streams->scorep_location,
                                                    time, scorep_cupti_idle_region_handle );
                        context->activity->gpu_idle = false;
                    }
                }
                else
#endif          /* SCOREP_CUPTI_ACTIVITY */
                {
                    SCOREP_Location_ExitRegion( context->streams->scorep_location,
                                                time, scorep_cupti_idle_region_handle );
                }
            }

            SCOREP_CUPTI_LOCK();

            if ( kind != cudaMemcpyDeviceToDevice &&
                 context->location_id == SCOREP_CUPTI_NO_ID )
            {
                context->location_id = scorep_cupti_location_counter++;

                /* create window on every location, where it is used
                   SCOREP_RmaWinCreate( scorep_cuda_interim_window_handle );*/
            }

            if ( SCOREP_CUPTI_NO_ID == stream->location_id )
            {
                stream->location_id = scorep_cupti_location_counter++;

                /* create window on every location, where it is used */
                SCOREP_Location_RmaWinCreate( stream->scorep_location, time,
                                              scorep_cuda_interim_window_handle );
            }

            SCOREP_CUPTI_UNLOCK();
        }

        if ( kind == cudaMemcpyHostToDevice )
        {
            SCOREP_Location_RmaGet( stream_location, time,
                                    scorep_cuda_interim_window_handle,
                                    context->location_id, bytes, 42 );
        }
        else if ( kind == cudaMemcpyDeviceToHost )
        {
            SCOREP_Location_RmaPut( stream_location, time,
                                    scorep_cuda_interim_window_handle,
                                    context->location_id, bytes, 42 );
        }
        else if ( kind == cudaMemcpyDeviceToDevice )
        {
            SCOREP_Location_RmaGet( stream_location, time,
                                    scorep_cuda_interim_window_handle,
                                    stream->location_id, bytes, 42 );
        }
    }

    if ( cbInfo->callbackSite == CUPTI_API_EXIT )
    {
        stream_location = ( ( scorep_cupti_stream_t* )( *cbInfo->correlationData ) )->scorep_location;

        if ( kind == cudaMemcpyDeviceToDevice )
        {
            SCOREP_Location_RmaOpCompleteBlocking( stream_location, time,
                                                   scorep_cuda_interim_window_handle, 42 );
        }
        else if ( kind != cudaMemcpyHostToHost )
        {
            SCOREP_Location_RmaOpCompleteBlocking( stream_location, time,
                                                   scorep_cuda_interim_window_handle, 42 );
        }

        /* pure idle time */
        if ( scorep_cuda_record_idle == SCOREP_CUDA_PURE_IDLE )
        {
            scorep_cupti_context_t* context = scorep_cupti_context_get_create( cbInfo->context );

            if ( context->streams != NULL )
            {
#if defined( SCOREP_CUPTI_ACTIVITY )
                if ( NULL != context->activity )
                {
                    if ( !context->activity->gpu_idle )
                    {
                        SCOREP_Location_EnterRegion( context->streams->scorep_location,
                                                     time, scorep_cupti_idle_region_handle );
                        context->activity->gpu_idle = true;
                    }
                }
                else
  #endif        /* SCOREP_CUPTI_ACTIVITY */
                {
                    SCOREP_Location_EnterRegion( context->streams->scorep_location,
                                                 time, scorep_cupti_idle_region_handle );
                }
            }
        }

#if defined( SCOREP_CUPTI_ACTIVITY )
        /* enable activity API for memory copy recording */
# if defined( SCOREP_CUPTI_EVENTS )
        if ( !scorep_cupti_events_enabled )
#endif
        {
            SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_MEMCPY ) );
        }
#endif
    }
}

/*
 * Handle synchronous Peer-toPeer CUDA memory copy calls.
 *
 * @param cbInfo information about the callback
 * @param cuSrcCtx CUDA context of source memory
 * @param cuDstCtx CUDA context of destination memory
 * @param bytes number of bytes transfered
 * @param time the start/stop time of the synchronous transfer
 */
static void
handle_cuda_memcpy_p2p( const CUpti_CallbackData* cbInfo,
                        CUcontext cuSrcCtx,
                        CUcontext cuDstCtx,
                        uint64_t bytes, uint64_t time )
{
    if ( cbInfo->callbackSite == CUPTI_API_ENTER )
    {
        scorep_cupti_context_t* source_context      = scorep_cupti_context_get_create( cuSrcCtx );
        scorep_cupti_context_t* destination_context = scorep_cupti_context_get_create( cuDstCtx );

#if defined( SCOREP_CUPTI_ACTIVITY )
        /* disable activity API for memory copy recording */
# if defined( SCOREP_CUPTI_EVENTS )
        if ( !scorep_cupti_events_enabled )
#endif
        {
            SCOREP_CUPTI_CALL( cuptiActivityDisable( CUPTI_ACTIVITY_KIND_MEMCPY ) );
        }
#endif  /* SCOREP_CUPTI_ACTIVITY */

        if ( source_context->streams == NULL )
        {
            scorep_cupticb_create_default_stream( source_context );
        }

        if ( destination_context->streams == NULL )
        {
            scorep_cupticb_create_default_stream( destination_context );
        }

        *( cbInfo->correlationData ) = SCOREP_CUPTI_NO_ID;

        if ( cbInfo->context == cuSrcCtx )
        {
            *( cbInfo->correlationData ) = ( uint64_t )( source_context->streams->scorep_location );

            if ( !scorep_cupti_activity_is_buffer_empty( cuSrcCtx ) )
            {
                SCOREP_CUDA_DRIVER_CALL( cuCtxSynchronize() );
            }

            DISABLE_CUDRV_CALLBACKS();
            SCOREP_CUDA_DRIVER_CALL( cuCtxSetCurrent( cuDstCtx ) );
            ENABLE_CUDRV_CALLBACKS();

            if ( !scorep_cupti_activity_is_buffer_empty( cuDstCtx ) )
            {
                SCOREP_CUDA_DRIVER_CALL( cuCtxSynchronize() );
            }

            DISABLE_CUDRV_CALLBACKS();
            SCOREP_CUDA_DRIVER_CALL( cuCtxSetCurrent( cuSrcCtx ) );
            ENABLE_CUDRV_CALLBACKS();

            time = SCOREP_GetClockTicks();

            SCOREP_Location_RmaPut( source_context->streams->scorep_location, time,
                                    scorep_cuda_interim_window_handle,
                                    destination_context->streams->location_id, bytes, 42 );
        }
        else if ( cbInfo->context == cuDstCtx )
        {
            *( cbInfo->correlationData ) = ( uint64_t )( destination_context->streams->scorep_location );

            if ( !scorep_cupti_activity_is_buffer_empty( cuDstCtx ) )
            {
                SCOREP_CUDA_DRIVER_CALL( cuCtxSynchronize() );
            }

            DISABLE_CUDRV_CALLBACKS();
            SCOREP_CUDA_DRIVER_CALL( cuCtxSetCurrent( cuSrcCtx ) );
            ENABLE_CUDRV_CALLBACKS();

            if ( !scorep_cupti_activity_is_buffer_empty( cuSrcCtx ) )
            {
                SCOREP_CUDA_DRIVER_CALL( cuCtxSynchronize() );
            }

            DISABLE_CUDRV_CALLBACKS();
            SCOREP_CUDA_DRIVER_CALL( cuCtxSetCurrent( cuDstCtx ) );
            ENABLE_CUDRV_CALLBACKS();

            time = SCOREP_GetClockTicks();

            SCOREP_Location_RmaGet( destination_context->streams->scorep_location, time,
                                    scorep_cuda_interim_window_handle,
                                    source_context->streams->location_id, bytes, 42 );
        }
        else
        {
            return;
        }

        /* pure idle time */
        if ( scorep_cuda_record_idle == SCOREP_CUDA_PURE_IDLE )
        {
#if defined( SCOREP_CUPTI_ACTIVITY )
            if ( NULL != source_context->activity )
            {
                if ( source_context->activity->gpu_idle )
                {
                    SCOREP_Location_ExitRegion( source_context->streams->scorep_location,
                                                time, scorep_cupti_idle_region_handle );
                    source_context->activity->gpu_idle = false;
                }
            }
            else
#endif      /* SCOREP_CUPTI_ACTIVITY */
            {
                SCOREP_Location_ExitRegion( source_context->streams->scorep_location,
                                            time, scorep_cupti_idle_region_handle );
            }
#if defined( SCOREP_CUPTI_ACTIVITY )
            if ( NULL != destination_context->activity )
            {
                if ( destination_context->activity->gpu_idle )
                {
                    SCOREP_Location_ExitRegion( destination_context->streams->scorep_location,
                                                time, scorep_cupti_idle_region_handle );
                    destination_context->activity->gpu_idle = false;
                }
            }
            else
#endif      /* SCOREP_CUPTI_ACTIVITY */
            {
                SCOREP_Location_ExitRegion( destination_context->streams->scorep_location,
                                            time, scorep_cupti_idle_region_handle );
            }
        }
    }
    else if ( cbInfo->callbackSite == CUPTI_API_EXIT )
    {
        /* if communication put/get has been written, then write the
           rma_end on current context */
        if ( *( cbInfo->correlationData ) != SCOREP_CUPTI_NO_ID )
        {
            SCOREP_Location_RmaOpCompleteBlocking( ( SCOREP_Location* )*( cbInfo->correlationData ), time,
                                                   scorep_cuda_interim_window_handle, 42 );


            /* pure idle time */
            if ( scorep_cuda_record_idle == SCOREP_CUDA_PURE_IDLE )
            {
                scorep_cupti_context_t* src_context = scorep_cupti_context_get_create( cuSrcCtx );
                scorep_cupti_context_t* dst_context = scorep_cupti_context_get_create( cuDstCtx );

                /* source context */
#if defined( SCOREP_CUPTI_ACTIVITY )
                if ( src_context->streams != NULL )
                {
                    if ( NULL != src_context->activity )
                    {
                        if ( !src_context->activity->gpu_idle )
                        {
                            SCOREP_Location_EnterRegion( src_context->streams->scorep_location,
                                                         time, scorep_cupti_idle_region_handle );
                            src_context->activity->gpu_idle = true;
                        }
                    }
                    else
#endif          /* SCOREP_CUPTI_ACTIVITY */
                {
                    SCOREP_Location_EnterRegion( src_context->streams->scorep_location,
                                                 time, scorep_cupti_idle_region_handle );
                }
            }

            /* destination context */
#if defined( SCOREP_CUPTI_ACTIVITY )
            if ( dst_context->streams != NULL )
            {
                if ( NULL != dst_context->activity )
                {
                    if ( !dst_context->activity->gpu_idle )
                    {
                        SCOREP_Location_EnterRegion( dst_context->streams->scorep_location,
                                                     time, scorep_cupti_idle_region_handle );
                        dst_context->activity->gpu_idle = true;
                    }
                }
                else
#endif      /* SCOREP_CUPTI_ACTIVITY */
            {
                SCOREP_Location_EnterRegion( dst_context->streams->scorep_location,
                                             time, scorep_cupti_idle_region_handle );
            }
        }
    }
}

#if defined( SCOREP_CUPTI_ACTIVITY )
/* enable activity API for memory copy recording */
# if defined( SCOREP_CUPTI_EVENTS )
if ( !scorep_cupti_events_enabled )
#endif
{
    SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_MEMCPY ) );
}
#endif
}
}

/*
 * Handle CUDA memory copies with the flag cudaMemcpyDefault
 *
 * @param cbInfo information about the callback
 * @param cuSrcDevPtr CUDA source device pointer
 * @param cuDstDevPtr CUDA destination device pointer
 * @param bytes number of bytes to be transfered
 * @param time the enter or exit timestamp of the memory copy API function
 */
static void
handle_cuda_memcpy_default( const CUpti_CallbackData* cbInfo,
                            CUdeviceptr cuSrcDevPtr,
                            CUdeviceptr cuDstDevPtr,
                            uint64_t bytes, uint64_t time )
{
    CUcontext cuSrcCtx;
    CUcontext cuDstCtx;
    CUmemorytype srcMemType;
    CUmemorytype dstMemType;
    enum cudaMemcpyKind kind = cudaMemcpyDefault;

    /* do not trace these CUDA driver API function calls */
    DISABLE_CUDRV_CALLBACKS();

    cuPointerGetAttribute( &cuSrcCtx, CU_POINTER_ATTRIBUTE_CONTEXT,
                           cuSrcDevPtr );
    cuPointerGetAttribute( &cuDstCtx, CU_POINTER_ATTRIBUTE_CONTEXT,
                           cuDstDevPtr );

    cuPointerGetAttribute( &srcMemType, CU_POINTER_ATTRIBUTE_MEMORY_TYPE,
                           cuSrcDevPtr );
    cuPointerGetAttribute( &dstMemType, CU_POINTER_ATTRIBUTE_MEMORY_TYPE,
                           cuDstDevPtr );

    ENABLE_CUDRV_CALLBACKS();

    /* get memory copy direction */
    kind = get_cuda_memcpy_kind( srcMemType, dstMemType );

    if ( kind == cudaMemcpyDefault )
    {
        UTILS_WARNING( "[CUPTI Callbacks] Could not determine memory copy kind! "
                       "Skipping this memory copy!" );
        return;
    }

    /* if device<->host the context should be the current one */

    /* check for peer-to-peer memory copy */
    if ( cuSrcCtx != cuDstCtx )
    {
        if ( kind == cudaMemcpyDeviceToDevice )
        {
            handle_cuda_memcpy_p2p( cbInfo, cuSrcCtx, cuDstCtx,
                                    bytes, time );
        }
        else
        {
            /* device <-> host memory copies can be written on the current context,
             * even if the device memory in a different context, than the host memory */
            handle_cuda_memcpy( cbInfo, kind, bytes, time );
        }
    }
    else
    {
        if ( cbInfo->context == cuSrcCtx )
        {
            handle_cuda_memcpy( cbInfo, kind, bytes, time );
        }
        else if ( cbInfo->context == cuDstCtx )
        {
            /* switch memory copy direction*/
            if ( kind == cudaMemcpyDeviceToHost )
            {
                kind = cudaMemcpyHostToDevice;
            }
            else if ( kind == cudaMemcpyHostToDevice )
            {
                kind = cudaMemcpyDeviceToHost;
            }

            handle_cuda_memcpy( cbInfo, kind, bytes, time );
        }
        else
        {
            UTILS_WARNING( "[CUPTI Callbacks] Memory copy within context skipped! "
                           "(kind=%d)", kind );
        }
    }
}

/*
 * Handle asynchronous CUDA runtime memory copy calls.
 *
 * @param cbInfo information about the callback
 * @param kind the direction of the transfer
 * @param bytes the number of transfered bytes
 * @param cudaStream the CUDA stream
 */
static void
handle_cuda_runtime_memcpy_async( const CUpti_CallbackData* cbInfo,
                                  enum cudaMemcpyKind       kind,
                                  uint64_t                  bytes,
                                  cudaStream_t              cudaStream )
{
    SCOREP_Location* stream_location;
    uint64_t time;

    if ( cbInfo->callbackSite == CUPTI_API_ENTER )
    {
        /* get the Score-P thread ID the kernel is running on */
        scorep_cupti_context_t* context = scorep_cupti_context_get_create( cbInfo->context );
        scorep_cupti_stream_t* stream   = NULL;
        uint32_t cuda_stream_id         = SCOREP_CUPTI_NO_STREAM_ID;

#if defined( SCOREP_CUPTI_ACTIVITY )
        if ( context->activity == NULL )
        {
            SCOREP_CUPTI_CALL( cuptiGetStreamId( cbInfo->context, cudaStream, &cuda_stream_id ) );
        }
        else
        {
            cuda_stream_id = context->activity->default_strm_id;
        }
#endif  /* SCOREP_CUPTI_ACTIVITY */

        stream = scorep_cupti_stream_get_create( context, cudaStream, cuda_stream_id );

        stream_location = stream->scorep_location;

        /* save address into 64 Bit correlation value for exit callback */
        *cbInfo->correlationData = ( uint64_t )stream;

        SCOREP_CUPTI_LOCK();

        if ( kind != cudaMemcpyDeviceToDevice &&
             context->location_id == SCOREP_CUPTI_NO_ID )
        {
            context->location_id = scorep_cupti_location_counter++;

            /* create window on every location, where it is used
               SCOREP_RmaWinCreate( scorep_cuda_interim_window_handle );*/
        }

        time = SCOREP_GetClockTicks();
        if ( SCOREP_CUPTI_NO_ID == stream->location_id )
        {
            stream->location_id = scorep_cupti_location_counter++;

            /* create window on every location, where it is used */
            SCOREP_Location_RmaWinCreate( stream->scorep_location, time,
                                          scorep_cuda_interim_window_handle );
        }

        SCOREP_CUPTI_UNLOCK();


        if ( kind == cudaMemcpyHostToDevice )
        {
            SCOREP_Location_RmaGet( stream_location, time,
                                    scorep_cuda_interim_window_handle,
                                    context->location_id, bytes, 42 );
        }
        else if ( kind == cudaMemcpyDeviceToHost )
        {
            SCOREP_Location_RmaPut( stream_location, time,
                                    scorep_cuda_interim_window_handle,
                                    context->location_id, bytes, 42 );
        }
        else if ( kind == cudaMemcpyDeviceToDevice )
        {
            SCOREP_Location_RmaGet( stream_location, time,
                                    scorep_cuda_interim_window_handle,
                                    stream->location_id, bytes, 42 );
        }
    }

    if ( cbInfo->callbackSite == CUPTI_API_EXIT )
    {
        SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
        stream_location = ( ( scorep_cupti_stream_t* )( *cbInfo->correlationData ) )->scorep_location;

        /* synchronize to get host waiting time */
        if ( scorep_cuda_sync_level )
        {
            time = scorep_cupticb_synchronize_context( location );
        }
        else
        {
            time = SCOREP_GetClockTicks();
        }

        if ( kind == cudaMemcpyDeviceToDevice )
        {
            SCOREP_Location_RmaOpCompleteBlocking( stream_location, time,
                                                   scorep_cuda_interim_window_handle, 42 );
        }
        else if ( kind != cudaMemcpyHostToHost )
        {
            SCOREP_Location_RmaOpCompleteBlocking( stream_location, time,
                                                   scorep_cuda_interim_window_handle, 42 );
        }
    }
}

/* -------------START: Implementation of public functions ------------------ */
/* ------------------------------------------------------------------------- */

/**
 * Initialize the Score-P CUPTI callback implementation.
 */
void
scorep_cupti_callbacks_init()
{
    if ( !scorep_cupti_callbacks_initialized )
    {
        scorep_cuda_set_features();
        scorep_cupti_init();
        SCOREP_CUPTI_LOCK();
        if ( !scorep_cupti_callbacks_initialized )
        {
            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                                "[CUPTI Callbacks] Initializing ... " );

            /* check the CUDA APIs to be traced */
            record_driver_api  = false;
            record_runtime_api = false;

            /* check for CUDA runtime API */
            if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_RUNTIME_API ) == SCOREP_CUDA_FEATURE_RUNTIME_API )
            {
                record_runtime_api       = true;
                cuda_runtime_file_handle = SCOREP_Definitions_NewSourceFile( "CUDART" );
            }

            /* check for CUDA driver API */
            if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_DRIVER_API ) == SCOREP_CUDA_FEATURE_DRIVER_API )
            {
                record_driver_api       = true;
                cuda_driver_file_handle = SCOREP_Definitions_NewSourceFile( "CUDRV" );
            }

#if defined( SCOREP_CUPTI_EVENTS )
            /* check for PAPI CUDA events */
            if ( scorep_cuda_record_kernels  && scorep_metrics_cuda_available )
            {
                scorep_cupti_events_enabled = true;
            }
#endif

            scorep_cupti_set_callback( scorep_cupti_callbacks_all_ptr );

            /* reset the hash table for CUDA API functions */
            memset( scorep_cupti_callbacks_cuda_function_table, SCOREP_INVALID_REGION,
                    CUPTI_CALLBACKS_CUDA_API_FUNC_MAX * sizeof( uint32_t ) );

            if ( scorep_cuda_record_kernels )
            {
                /* create a region handle for CUDA idle */
                scorep_cupti_kernel_file_handle = SCOREP_Definitions_NewSourceFile( "CUDA_KERNEL" );
            }

            /* if GPU streams are necessary */
            if ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy || scorep_cuda_record_gpumemusage )
            {
                if ( scorep_cuda_record_memcpy )
                {
                    /* create interim communicator once for a process */
                    scorep_cuda_interim_communicator_handle =
                        SCOREP_Definitions_NewInterimCommunicator(
                            SCOREP_INVALID_INTERIM_COMMUNICATOR,
                            SCOREP_ADAPTER_CUDA,
                            0,
                            NULL );

                    scorep_cuda_interim_window_handle =
                        SCOREP_Definitions_NewInterimRmaWindow(
                            "CUDA_WINDOW",
                            scorep_cuda_interim_communicator_handle );
                }

                /* get global counter group IDs */
                if ( scorep_cuda_record_kernels == SCOREP_CUDA_KERNEL_AND_COUNTER )
                {
                    {
                        SCOREP_MetricHandle metric_handle_bpg =
                            SCOREP_Definitions_NewMetric( "blocks_per_grid",
                                                          "blocks per grid",
                                                          SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                                          SCOREP_METRIC_MODE_ABSOLUTE_NEXT,
                                                          SCOREP_METRIC_VALUE_UINT64,
                                                          SCOREP_METRIC_BASE_DECIMAL,
                                                          1,
                                                          "#",
                                                          SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );

                        scorep_cupti_sampling_set_blocks_per_grid =
                            SCOREP_Definitions_NewSamplingSet( 1, &metric_handle_bpg,
                                                               SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS, SCOREP_SAMPLING_SET_GPU );
                    }

                    {
                        SCOREP_MetricHandle metric_handle_tpb =
                            SCOREP_Definitions_NewMetric( "threads_per_block",
                                                          "threads per block",
                                                          SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                                          SCOREP_METRIC_MODE_ABSOLUTE_NEXT,
                                                          SCOREP_METRIC_VALUE_UINT64,
                                                          SCOREP_METRIC_BASE_DECIMAL,
                                                          1,
                                                          "#",
                                                          SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );

                        scorep_cupti_sampling_set_threads_per_block =
                            SCOREP_Definitions_NewSamplingSet( 1, &metric_handle_tpb,
                                                               SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS, SCOREP_SAMPLING_SET_GPU );
                    }

                    {
                        SCOREP_MetricHandle metric_handle_tpk =
                            SCOREP_Definitions_NewMetric( "threads_per_kernel",
                                                          "threads per kernel",
                                                          SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                                          SCOREP_METRIC_MODE_ABSOLUTE_NEXT,
                                                          SCOREP_METRIC_VALUE_UINT64,
                                                          SCOREP_METRIC_BASE_DECIMAL,
                                                          1,
                                                          "#",
                                                          SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );

                        scorep_cupti_sampling_set_threads_per_kernel =
                            SCOREP_Definitions_NewSamplingSet( 1, &metric_handle_tpk,
                                                               SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS, SCOREP_SAMPLING_SET_GPU );
                    }
                }

                {
                    SCOREP_SourceFileHandle scorep_cuda_sync_file_handle =
                        SCOREP_Definitions_NewSourceFile( "CUDA_SYNC" );
                    cuda_sync_region_handle = SCOREP_Definitions_NewRegion(
                        "cudaSynchronize", NULL, scorep_cuda_sync_file_handle,
                        0, 0, SCOREP_ADAPTER_CUDA, SCOREP_REGION_IMPLICIT_BARRIER );
                }


#if defined( SCOREP_CUPTI_ACTIVITY )
#if defined( SCOREP_CUPTI_EVENTS )
                if ( !scorep_cupti_events_enabled )
#endif
                {
                    scorep_cupti_activity_init();
                }
#endif          /* SCOREP_CUPTI_ACTIVITY */
            } /* scorep_cuda_record_kernels || scorep_cuda_record_memcpy || scorep_cuda_record_gpumemusage */
              /*}  scorep_gpu_get_config() != 0 */

            /* register the finalize function of Score-P CUPTI to be called before
             * the program exits */
            atexit( scorep_cupti_callbacks_finalize );

            scorep_cupti_callbacks_initialized = true;
        } /* !scorep_cupticb_initialized */

        SCOREP_CUPTI_UNLOCK();

        scorep_cupti_callbacks_enable( true );
    } /* !scorep_cupticb_initialized */
}

/*
 * Finalize the Score-P CUPTI callback implementation.
 */
void
scorep_cupti_callbacks_finalize()
{
    if ( !scorep_cupti_callbacks_finalized && scorep_cupti_callbacks_initialized )
    {
        SCOREP_CUPTI_LOCK();
        if ( !scorep_cupti_callbacks_finalized && scorep_cupti_callbacks_initialized )
        {
            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                                "[CUPTI Callbacks] Finalizing ... " );

            /* create the global CUDA communication group before the structures
               are destroyed */
            if ( scorep_cuda_record_memcpy )
            {
                scorep_cuda_global_location_number =
                    scorep_cupti_create_cuda_comm_group( &scorep_cuda_global_location_ids );
            }

            if ( record_runtime_api || record_driver_api ||
                 scorep_cuda_record_kernels || scorep_cuda_record_memcpy ||
                 scorep_cuda_record_gpumemusage )
            {
                SCOREP_CUPTI_CALL( cuptiUnsubscribe( scorep_cupti_callbacks_subscriber ) );
            }

            /* clean up the Score-P CUPTI context list */
            while ( scorep_cupti_context_list != NULL )
            {
                scorep_cupti_context_t* context = scorep_cupti_context_list;

                scorep_cupti_context_list = scorep_cupti_context_list->next;

#if defined( SCOREP_CUPTI_ACTIVITY )
                if ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy ||
                     scorep_cuda_record_gpumemusage )
                {
                    scorep_cupti_activity_context_finalize( context );
                }
#endif

                scorep_cupti_callbacks_finalize_context( context );

                /* this will free the allocated memory of the context as well */
                scorep_cupti_context_finalize( context );

                context = NULL;
            }

            scorep_cupti_callbacks_finalized = true;
            SCOREP_CUPTI_UNLOCK();

#if defined( SCOREP_CUPTI_ACTIVITY )
            if ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy ||
                 scorep_cuda_record_gpumemusage )
            {
                scorep_cupti_activity_finalize();
            }
#endif

            scorep_cupti_finalize();
        }
    }
}
