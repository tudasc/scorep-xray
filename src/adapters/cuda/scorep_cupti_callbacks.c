/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  Implementation of the CUDA runtime and driver API event logging via
 *  CUPTI callbacks.
 */

#include <config.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Config.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_Types.h>
#include <SCOREP_Filtering.h>
#include <SCOREP_Timer_Ticks.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include "scorep_cuda.h"
#include "scorep_cupti_callbacks.h"

#include "scorep_cupti_activity.h" /* Support for CUPTI activity */

/* Defines CUPTI keys to write as attributes */
#define SCOREP_CUPTI_KEY_STREAM    ( 1 << 1 )
#define SCOREP_CUPTI_KEY_EVENT     ( 1 << 2 )
#define SCOREP_CUPTI_KEY_CURESULT  ( 1 << 3 )

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
    if ( scorep_record_runtime_api ) \
        SCOREP_CUPTI_ENABLE_CALLBACK_DOMAIN( CUPTI_CB_DOMAIN_RUNTIME_API )

/* Disable CUDA runtime API callbacks, if recording is enabled */
#define DISABLE_CUDART_CALLBACKS() \
    if ( scorep_record_runtime_api ) \
        SCOREP_CUPTI_DISABLE_CALLBACK_DOMAIN( CUPTI_CB_DOMAIN_RUNTIME_API )

/* Enable CUDA driver API callbacks, if recording is enabled */
#define ENABLE_CUDRV_CALLBACKS() \
    if ( scorep_record_driver_api ) \
        SCOREP_CUPTI_ENABLE_CALLBACK_DOMAIN( CUPTI_CB_DOMAIN_DRIVER_API )

/* Disable CUDA driver API callbacks, if recording is enabled */
#define DISABLE_CUDRV_CALLBACKS() \
    if ( scorep_record_driver_api ) \
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
#define SCOREP_CUPTICB_MEMCPY_CUDART( _cbInfo, _kind, _src, _dst, _bytes, _time, _region ) \
    { \
        if ( _kind == cudaMemcpyDefault ) { \
            handle_cuda_memcpy_default( _cbInfo, ( CUdeviceptr )_src, \
                                        ( CUdeviceptr )_dst, _bytes, _time, _region ); \
        }else if ( _kind != cudaMemcpyHostToHost ) { \
            handle_cuda_memcpy( _cbInfo, _kind, _bytes, _time, _region ); } \
    }

/* initialization and finalization flags */
static bool scorep_cupti_callbacks_initialized = false;
static bool scorep_cupti_callbacks_finalized   = false;

/* flag: Are CUPTI callbacks enabled? */
static bool scorep_cupti_callbacks_enabled = false;

/* flag: Are CUPTI callbacks driver API domain enabled? */
static bool is_driver_domain_enabled = false;

/* flag: tracing of CUDA runtime API enabled? */
bool scorep_record_runtime_api = false;

/* flag: tracing of CUDA driver API enabled? */
bool scorep_record_driver_api = false;

/* flag: callbacks state */
uint8_t scorep_cupti_callbacks_state = 0;

/* global subscriber handle */
static CUpti_SubscriberHandle scorep_cupti_callbacks_subscriber;

/* CUDA runtime and driver API source file handle (function group description) */
static SCOREP_SourceFileHandle cuda_runtime_file_handle = SCOREP_INVALID_SOURCE_FILE;
static SCOREP_SourceFileHandle cuda_driver_file_handle  = SCOREP_INVALID_SOURCE_FILE;

static SCOREP_RegionHandle cuda_sync_region_handle = SCOREP_INVALID_REGION;


/**************** The callback functions to be registered *********************/

static void CUPTIAPI
scorep_cupti_callbacks_all( void*,
                            CUpti_CallbackDomain,
                            CUpti_CallbackId,
                            const void* );

static void CUPTIAPI
scorep_cupti_callbacks_runtime_api( CUpti_CallbackId,
                                    const CUpti_CallbackData* );

static void
scorep_cupti_callbacks_driver_api( CUpti_CallbackId,
                                   const CUpti_CallbackData* );

static void
scorep_cupti_callbacks_resource( CUpti_CallbackId,
                                 const CUpti_ResourceData* );

static void
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
                    uint64_t,
                    SCOREP_RegionHandle );

static void
handle_cuda_memcpy_default( const CUpti_CallbackData* cbInfo,
                            CUdeviceptr               cuSrcDevPtr,
                            CUdeviceptr               cuDstDevPtr,
                            uint64_t                  bytes,
                            uint64_t                  time,
                            SCOREP_RegionHandle       region );

static void
handle_cuda_malloc( CUcontext,
                    uint64_t,
                    size_t );
static void
handle_cuda_free( CUcontext,
                  uint64_t );

static void
create_references_list( SCOREP_Location* location,
                        CUcontext        context,
                        CUstream         stream,
                        CUevent          event,
                        CUresult         result,
                        int              references );

static void
enter_with_refs( uint64_t*           time,
                 SCOREP_RegionHandle region,
                 CUcontext           context,
                 CUstream            stream,
                 CUevent             event,
                 CUresult            result,
                 int                 references );

static void
exit_with_refs( uint64_t*           time,
                SCOREP_RegionHandle region,
                CUcontext           context,
                CUstream            stream,
                CUevent             event,
                CUresult            result,
                int                 references );

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
    if ( scorep_record_runtime_api && scorep_record_driver_api )
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
            if ( scorep_record_runtime_api )
            {
                SCOREP_CUPTI_CALL( cuptiEnableDomain( 1, scorep_cupti_callbacks_subscriber,
                                                      CUPTI_CB_DOMAIN_RUNTIME_API ) );

                scorep_cupti_callbacks_enabled = true;
            }

            if ( scorep_record_driver_api ||
                 ( !scorep_record_runtime_api && scorep_cuda_record_gpumemusage ) ||
                 ( !scorep_record_runtime_api && scorep_cuda_record_memcpy &&
                   scorep_cuda_sync_level == SCOREP_CUDA_RECORD_SYNC_FULL ) )
            {
                SCOREP_CUPTI_CALL( cuptiEnableDomain( 1, scorep_cupti_callbacks_subscriber,
                                                      CUPTI_CB_DOMAIN_DRIVER_API ) );

                is_driver_domain_enabled       = true;
                scorep_cupti_callbacks_enabled = true;
            }

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
 * This CUPTI callback function chooses the CUPTI domain.
 *
 * @param userdata pointer to the user data
 * @param domain the callback domain (runtime or driver API)
 * @param cbid the ID of the callback function in the given domain
 * @param cbInfo information about the callback
 */
static void CUPTIAPI
scorep_cupti_callbacks_all( void*                userdata,
                            CUpti_CallbackDomain domain,
                            CUpti_CallbackId     cbid,
                            const void*          cbInfo )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

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

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * Writes CUPTI reference attributes to an attribute list.
 *
 * @param location Location for attribute.
 * @param context CUDA context
 * @param stream CUDA stream
 * @param event CUDA event (optional)
 * @param result CUDA result (optional)
 * @param references combination of SCOREP_CUPTI_KEY_* values
 */
static void
create_references_list( SCOREP_Location* location,
                        CUcontext        context,
                        CUstream         stream,
                        CUevent          event,
                        CUresult         result,
                        int              references )
{
    if ( references & SCOREP_CUPTI_KEY_CURESULT )
    {
        SCOREP_Location_AddAttribute( location, scorep_cupti_attributes.result_ref, &result );
    }

    if ( references & SCOREP_CUPTI_KEY_EVENT )
    {
        SCOREP_Location_AddAttribute( location, scorep_cupti_attributes.event_ref, &event );
    }

    if ( references & SCOREP_CUPTI_KEY_STREAM )
    {
        scorep_cupti_context* scorep_ctx  = scorep_cupti_context_get_create( context );
        uint32_t              strm_id     = 0;
        scorep_cupti_stream*  scorep_strm = NULL;

        SCOREP_CUPTI_CALL( cuptiGetStreamId( context, stream, &strm_id ) );
        SCOREP_CUPTI_LOCK();
        scorep_strm = scorep_cupti_stream_get_create( scorep_ctx, strm_id );
        SCOREP_CUPTI_UNLOCK();

        SCOREP_LocationHandle location_handle =
            SCOREP_Location_GetLocationHandle( scorep_strm->scorep_location );

        SCOREP_Location_AddAttribute( location, scorep_cupti_attributes.stream_ref, &location_handle );
    }
}

/**
 * Writes an enter region on the current CPU location
 * along with reference attributes.
 *
 * @param time pointer to the timestamp for this region
 * @param region region handle
 * @param context CUDA context
 * @param stream CUDA stream
 * @param event CUDA event (optional)
 * @param result CUresult (optional)
 * @param references combination of SCOREP_CUPTI_KEY_* values
 */
static void
enter_with_refs( uint64_t*           time,
                 SCOREP_RegionHandle region,
                 CUcontext           context,
                 CUstream            stream,
                 CUevent             event,
                 CUresult            result,
                 int                 references )
{
    if ( scorep_cuda_record_references )
    {
        create_references_list( SCOREP_Location_GetCurrentCPULocation(),
                                context, stream, event,
                                result, references );
    }
    if ( time )
    {
        /* With 'location == NULL' SCOREP_Location_EnterRegion will
         * write the event on the current CPU location */
        SCOREP_Location_EnterRegion( NULL, *time, region );
    }
}

/**
 * Writes an exit region on the current CPU location
 * along with reference attributes.
 *
 * @param time pointer to the timestamp for this region
 * @param region region handle
 * @param context CUDA context
 * @param stream CUDA stream
 * @param event CUDA event (optional)
 * @param result CUresult (optional)
 * @param references combination of SCOREP_CUPTI_KEY_* values
 */
static void
exit_with_refs( uint64_t*           time,
                SCOREP_RegionHandle region,
                CUcontext           context,
                CUstream            stream,
                CUevent             event,
                CUresult            result,
                int                 references )
{
    if ( scorep_cuda_record_references )
    {
        create_references_list( SCOREP_Location_GetCurrentCPULocation(),
                                context, stream, event,
                                result, references );
    }
    if ( time )
    {
        /* With 'location == NULL' SCOREP_Location_ExitRegion will
         * write the event on the current CPU location */
        SCOREP_Location_ExitRegion( NULL, *time, region );
    }
}

/*
 * This callback function is used to trace the CUDA runtime API.
 *
 * @param cbid the ID of the callback function in the given domain
 * @param cbInfo information about the callback
 *
 */
static void CUPTIAPI
scorep_cupti_callbacks_runtime_api( CUpti_CallbackId          callbackId,
                                    const CUpti_CallbackData* cbInfo )
{
    SCOREP_Location*    location = SCOREP_Location_GetCurrentCPULocation();
    uint64_t            time;
    SCOREP_RegionHandle region_handle              = SCOREP_INVALID_REGION;
    SCOREP_RegionHandle region_handle_stored       = SCOREP_INVALID_REGION;
    bool                record_driver_api_location =
        SCOREP_IS_CALLBACK_STATE_SET( SCOREP_CUPTI_CALLBACKS_STATE_DRIVER, location );

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
                                                      0, 0, SCOREP_PARADIGM_CUDA, SCOREP_REGION_WRAPPER );

        cuda_api_function_put( CUPTI_CB_DOMAIN_RUNTIME_API, callbackId, region_handle );
    }

    /* time stamp for all the following potential events */
    time = SCOREP_Timer_GetClockTicks();

    if ( scorep_cuda_record_memcpy && !record_driver_api_location )
    {
        if (
            ( scorep_cuda_sync_level == SCOREP_CUDA_RECORD_SYNC_FULL ) )
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
                                                  params->count, time, region_handle );

                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy2D_v3020:
                {
                    cudaMemcpy2D_v3020_params* params =
                        ( cudaMemcpy2D_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )( params->height * params->width ),
                                                  time, region_handle );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyToArray_v3020:
                {
                    cudaMemcpyToArray_v3020_params* params =
                        ( cudaMemcpyToArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )params->count, time,
                                                  region_handle );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy2DToArray_v3020:
                {
                    cudaMemcpy2DToArray_v3020_params* params =
                        ( cudaMemcpy2DToArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )( params->height * params->width ),
                                                  time, region_handle );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyFromArray_v3020:
                {
                    cudaMemcpyFromArray_v3020_params* params =
                        ( cudaMemcpyFromArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )params->count, time,
                                                  region_handle );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy2DFromArray_v3020:
                {
                    cudaMemcpy2DFromArray_v3020_params* params =
                        ( cudaMemcpy2DFromArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )( params->height * params->width ),
                                                  time, region_handle );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyArrayToArray_v3020:
                {
                    cudaMemcpyArrayToArray_v3020_params* params =
                        ( cudaMemcpyArrayToArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )params->count, time,
                                                  region_handle );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpy2DArrayToArray_v3020:
                {
                    cudaMemcpy2DArrayToArray_v3020_params* params =
                        ( cudaMemcpy2DArrayToArray_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->dst,
                                                  ( uint64_t )( params->height * params->width ),
                                                  time, region_handle );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyToSymbol_v3020:
                {
                    cudaMemcpyToSymbol_v3020_params* params =
                        ( cudaMemcpyToSymbol_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->src, params->symbol,
                                                  ( uint64_t )params->count, time,
                                                  region_handle );
                    return;
                }

                case CUPTI_RUNTIME_TRACE_CBID_cudaMemcpyFromSymbol_v3020:
                {
                    cudaMemcpyFromSymbol_v3020_params* params =
                        ( cudaMemcpyFromSymbol_v3020_params* )cbInfo->functionParams;

                    SCOREP_CUPTICB_MEMCPY_CUDART( cbInfo, params->kind,
                                                  params->symbol, params->dst,
                                                  ( uint64_t )params->count, time,
                                                  region_handle );
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
                                                  time, region_handle );
                    return;
                }

                default:
                    break;
            } /* switch(cbid) */
        }     /* synchronization recording enabled */
    }         /* if(scorep_gpu_trace_memcpy) */
    /****************************************************************************/

    /* all synchronous memory copies have been handled here and will not reach
       this point */

    /*********** write enter and exit records for CUDA runtime API **************/
    if ( !SCOREP_Filtering_MatchFunction( cbInfo->functionName, NULL ) )
    {
        if ( cbInfo->callbackSite == CUPTI_API_ENTER )
        {
            /* With 'location == NULL' SCOREP_Location_EnterRegion will
             * write the event on the current CPU location */
            SCOREP_Location_EnterRegion( NULL, time, region_handle );
        }
        else if ( cbInfo->callbackSite == CUPTI_API_EXIT )
        {
            /* With 'location == NULL' SCOREP_Location_ExitRegion will
             * write the event on the current CPU location */
            SCOREP_Location_ExitRegion( NULL, time, region_handle );
        }
    }

    /*
     ************** Semantic function instrumentation *********************
     */

    /* Memory allocation and deallocation tracing */
    if ( scorep_cuda_record_gpumemusage && !record_driver_api_location )
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
}

/*
 * This function handles the CUDA driver API callbacks.
 *
 * @param cbid the ID of the callback function in the given domain
 * @param cbInfo information about the callback
 */
static void CUPTIAPI
scorep_cupti_callbacks_driver_api( CUpti_CallbackId          callbackId,
                                   const CUpti_CallbackData* cbInfo )
{
    SCOREP_Location*    location = SCOREP_Location_GetCurrentCPULocation();
    uint64_t            time;
    SCOREP_RegionHandle region_handle              = SCOREP_INVALID_REGION;
    SCOREP_RegionHandle region_handle_stored       = SCOREP_INVALID_REGION;
    bool                record_driver_api_location =
        SCOREP_IS_CALLBACK_STATE_SET( SCOREP_CUPTI_CALLBACKS_STATE_DRIVER, location );

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

    /* Generate Score-P region handle for the API functions (if enabled) */
    if ( record_driver_api_location )
    {
        region_handle_stored =
            cuda_api_function_get( CUPTI_CB_DOMAIN_DRIVER_API, callbackId );
        if ( region_handle_stored != SCOREP_INVALID_REGION )
        {
            region_handle = region_handle_stored;
        }
        else
        {
            region_handle = SCOREP_Definitions_NewRegion( cbInfo->functionName, NULL, cuda_driver_file_handle,
                                                          0, 0, SCOREP_PARADIGM_CUDA, SCOREP_REGION_WRAPPER );

            cuda_api_function_put( CUPTI_CB_DOMAIN_DRIVER_API, callbackId, region_handle );
        }

        time = SCOREP_Timer_GetClockTicks();
    }

    /* handle memory copies with full sync option */
    if ( scorep_cuda_record_memcpy )
    {
        if ( scorep_cuda_sync_level == SCOREP_CUDA_RECORD_SYNC_FULL )
        {
            if ( !record_driver_api_location )
            {
                time = SCOREP_Timer_GetClockTicks();
            }

            /****************** synchronous CUDA memory copies **********************/
            switch ( callbackId )
            {
                case CUPTI_DRIVER_TRACE_CBID_cuMemcpy:
                {
                    cuMemcpy_params* params =
                        ( cuMemcpy_params* )cbInfo->functionParams;

                    handle_cuda_memcpy_default( cbInfo, params->src, params->dst,
                                                params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpy2D_v2:
                {
                    cuMemcpy2D_v2_params* params =
                        ( cuMemcpy2D_v2_params* )cbInfo->functionParams;

                    size_t bytes = params->pCopy->WidthInBytes * params->pCopy->Height;

                    handle_cuda_memcpy_default( cbInfo, params->pCopy->srcDevice,
                                                params->pCopy->dstDevice, time, bytes,
                                                region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpy3D_v2:
                {
                    cuMemcpy3D_v2_params* params =
                        ( cuMemcpy3D_v2_params* )cbInfo->functionParams;

                    size_t bytes = params->pCopy->WidthInBytes * params->pCopy->Height
                                   * params->pCopy->Depth;

                    handle_cuda_memcpy_default( cbInfo, params->pCopy->srcDevice,
                                                params->pCopy->dstDevice, time, bytes,
                                                region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyHtoD:
                {
                    cuMemcpyHtoD_params* params =
                        ( cuMemcpyHtoD_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyHostToDevice,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyHtoD_v2:
                {
                    cuMemcpyHtoD_v2_params* params =
                        ( cuMemcpyHtoD_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyHostToDevice,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyDtoH:
                {
                    cuMemcpyDtoH_params* params =
                        ( cuMemcpyDtoH_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToHost,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyDtoH_v2:
                {
                    cuMemcpyDtoH_v2_params* params =
                        ( cuMemcpyDtoH_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToHost,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyDtoD:
                {
                    cuMemcpyDtoD_params* params =
                        ( cuMemcpyDtoD_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToDevice,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyDtoD_v2:
                {
                    cuMemcpyDtoD_v2_params* params =
                        ( cuMemcpyDtoD_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToDevice,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyHtoA:
                {
                    cuMemcpyHtoA_params* params =
                        ( cuMemcpyHtoA_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyHostToDevice,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyHtoA_v2:
                {
                    cuMemcpyHtoA_v2_params* params =
                        ( cuMemcpyHtoA_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyHostToDevice,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyAtoH:
                {
                    cuMemcpyAtoH_params* params =
                        ( cuMemcpyAtoH_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToHost,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyAtoH_v2:
                {
                    cuMemcpyAtoH_v2_params* params =
                        ( cuMemcpyAtoH_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToHost,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyAtoA:
                {
                    cuMemcpyAtoA_params* params =
                        ( cuMemcpyAtoA_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToDevice,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                case CUPTI_DRIVER_TRACE_CBID_cuMemcpyAtoA_v2:
                {
                    cuMemcpyAtoA_v2_params* params =
                        ( cuMemcpyAtoA_v2_params* )cbInfo->functionParams;

                    handle_cuda_memcpy( cbInfo, cudaMemcpyDeviceToDevice,
                                        params->ByteCount, time, region_handle );

                    return;
                }

                default:
                    break;
            } /* switch(cbid) */
        }
    }         /* if(scorep_cuda_record_memcpy)*/

    /* all synchronous memory copies have been handled here and will not reach
       this point */

    if ( record_driver_api_location )
    {
        /********** write enter and exit records for CUDA driver API **********/
        if ( !SCOREP_Filtering_MatchFunction( cbInfo->functionName, NULL ) )
        {
            if ( cbInfo->callbackSite == CUPTI_API_ENTER )
            {
                /* handle callbacks which might have to write reference attributes separately */
                switch ( callbackId )
                {
                    case CUPTI_DRIVER_TRACE_CBID_cuLaunchKernel:
                    {
                        cuLaunchKernel_params* params =
                            ( cuLaunchKernel_params* )cbInfo->functionParams;

                        enter_with_refs( &time, region_handle,
                                         cbInfo->context, params->hStream,
                                         NULL, CUDA_SUCCESS, SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuLaunchGrid:
                    {
                        enter_with_refs( &time, region_handle,
                                         cbInfo->context, NULL, NULL,
                                         CUDA_SUCCESS, SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuLaunchGridAsync:
                    {
                        cuLaunchGridAsync_params* params =
                            ( cuLaunchGridAsync_params* )cbInfo->functionParams;

                        enter_with_refs( &time, region_handle,
                                         cbInfo->context, params->hStream,
                                         NULL, CUDA_SUCCESS, SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuLaunch:
                    {
                        enter_with_refs( &time, region_handle,
                                         cbInfo->context, NULL, NULL,
                                         CUDA_SUCCESS, SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }

                    default:
                        /* With 'location == NULL' SCOREP_Location_EnterRegion will
                         * write the event on the current CPU location */
                        SCOREP_Location_EnterRegion( NULL, time, region_handle );
                }
            }
            else if ( cbInfo->callbackSite == CUPTI_API_EXIT )
            {
                /* handle callbacks which might have to write reference attributes separately */
                switch ( callbackId )
                {
                    case CUPTI_DRIVER_TRACE_CBID_cuStreamSynchronize:
                    {
                        cuStreamSynchronize_params* params =
                            ( cuStreamSynchronize_params* )cbInfo->functionParams;
                        exit_with_refs( &time, region_handle, cbInfo->context,
                                        params->hStream, NULL, CUDA_SUCCESS, SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuMemcpyAsync:
                    {
                        cuMemcpyAsync_params* params =
                            ( cuMemcpyAsync_params* )cbInfo->functionParams;

                        exit_with_refs( &time, region_handle, cbInfo->context,
                                        params->hStream, NULL, CUDA_SUCCESS, SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuMemcpy2DAsync:
                    {
                        cuMemcpy2DAsync_params* params =
                            ( cuMemcpy2DAsync_params* )cbInfo->functionParams;

                        exit_with_refs( &time, region_handle, cbInfo->context,
                                        params->hStream, NULL, CUDA_SUCCESS, SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuMemcpy2DAsync_v2:
                    {
                        cuMemcpy2DAsync_v2_params* params =
                            ( cuMemcpy2DAsync_v2_params* )cbInfo->functionParams;

                        exit_with_refs( &time, region_handle, cbInfo->context,
                                        params->hStream, NULL, CUDA_SUCCESS, SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuMemcpy3DAsync:
                    {
                        cuMemcpy3DAsync_params* params =
                            ( cuMemcpy3DAsync_params* )cbInfo->functionParams;

                        exit_with_refs( &time, region_handle, cbInfo->context,
                                        params->hStream, NULL, CUDA_SUCCESS, SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuMemcpy3DAsync_v2:
                    {
                        cuMemcpy3DAsync_v2_params* params =
                            ( cuMemcpy3DAsync_v2_params* )cbInfo->functionParams;

                        exit_with_refs( &time, region_handle, cbInfo->context,
                                        params->hStream, NULL, CUDA_SUCCESS, SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuEventRecord:
                    {
                        cuEventRecord_params* params =
                            ( cuEventRecord_params* )cbInfo->functionParams;

                        CUresult funcResult = *( ( CUresult* )( cbInfo->functionReturnValue ) );
                        exit_with_refs( &time, region_handle, cbInfo->context,
                                        params->hStream, params->hEvent, funcResult,
                                        SCOREP_CUPTI_KEY_EVENT | SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuEventSynchronize:
                    {
                        cuEventSynchronize_params* params =
                            ( cuEventSynchronize_params* )cbInfo->functionParams;

                        CUresult funcResult = *( ( CUresult* )( cbInfo->functionReturnValue ) );
                        exit_with_refs( &time, region_handle, cbInfo->context,
                                        NULL, params->hEvent, funcResult,
                                        SCOREP_CUPTI_KEY_EVENT );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuEventQuery:
                    {
                        cuEventQuery_params* params =
                            ( cuEventQuery_params* )cbInfo->functionParams;

                        CUresult funcResult = *( ( CUresult* )( cbInfo->functionReturnValue ) );
                        exit_with_refs( &time, region_handle, cbInfo->context,
                                        NULL, params->hEvent, funcResult,
                                        SCOREP_CUPTI_KEY_CURESULT | SCOREP_CUPTI_KEY_EVENT );
                        break;
                    }

                    case CUPTI_DRIVER_TRACE_CBID_cuStreamWaitEvent:
                    {
                        cuStreamWaitEvent_params* params =
                            ( cuStreamWaitEvent_params* )cbInfo->functionParams;

                        CUresult funcResult = *( ( CUresult* )( cbInfo->functionReturnValue ) );
                        exit_with_refs( &time, region_handle, cbInfo->context,
                                        params->hStream, params->hEvent, funcResult,
                                        SCOREP_CUPTI_KEY_EVENT | SCOREP_CUPTI_KEY_STREAM );
                        break;
                    }
                    default:
                        /* With 'location == NULL' SCOREP_Location_EnterRegion will
                         * write the event on the current CPU location */
                        SCOREP_Location_ExitRegion( NULL, time, region_handle );
                }
            }
        }
    }

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

/*
 * This callback function is used to handle synchronization calls.
 *
 * @param cbid the ID of the callback function in the given domain
 * @param syncData synchronization data (CUDA context, CUDA stream)
 */
static void
scorep_cupti_callbacks_sync( CUpti_CallbackId             cbid,
                             const CUpti_SynchronizeData* syncData )
{
    if ( CUPTI_CBID_SYNCHRONIZE_CONTEXT_SYNCHRONIZED == cbid )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                            "[CUPTI Callbacks] Synchronize called for CUDA context %p",
                            syncData->context );

        if ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy )
        {
            scorep_cupti_context* context =
                scorep_cupti_context_get( syncData->context );

            if ( NULL == context )
            {
                UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                                    "[CUPTI Callbacks] Synchronize: No Score-P "
                                    "context for CUDA context %p." );
            }
            else if ( !scorep_cupti_activity_is_buffer_empty( context ) )
            {
                scorep_cupti_activity_context_flush( context );
            }
        }
    }
}

/*
 * This callback function is used to handle resource usage.
 *
 * @param callbackId the ID of the callback function in the given domain
 * @param resourceData resource information (CUDA context, CUDA stream)
 */
static void
scorep_cupti_callbacks_resource( CUpti_CallbackId          callbackId,
                                 const CUpti_ResourceData* resourceData )
{
    switch ( callbackId )
    {
        /********************** CUDA memory allocation ******************************/
        case CUPTI_CBID_RESOURCE_CONTEXT_CREATED:
        {
            scorep_cupti_context* context     = NULL;
            CUcontext             cudaContext = resourceData->context;

            context = scorep_cupti_context_get_create( cudaContext );

            /* setup and create activity context, if necessary */
            scorep_cupti_activity_context_setup( context );

            break;
        }

        case CUPTI_CBID_RESOURCE_CONTEXT_DESTROY_STARTING:
        {
            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                                "[CUPTI Callbacks] Destroying context %p ...",
                                resourceData->context );

            {
                scorep_cupti_context* context = NULL;
                /* Only flush the activities of the context. The user code has to ensure,
                   that the context is synchronized */
                context = scorep_cupti_context_get( resourceData->context );

                scorep_cupti_activity_context_flush( context );
            }

            break;
        }

        case CUPTI_CBID_RESOURCE_STREAM_DESTROY_STARTING:
        {
            uint32_t strmID;

            /* get the stream id from stream type */
            SCOREP_CUPTI_CALL( cuptiGetStreamId( resourceData->context,
                                                 resourceData->resourceHandle.stream,
                                                 &strmID ) );

            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                                "[CUPTI Callbacks] Destroying stream %d (context %p)",
                                strmID, resourceData->context );

            break;
        }

        default:
            break;
    }
}

/*
 * Synchronize the current CUDA context and record the synchronization as
 * necessary.
 *
 * @return Score-P time stamp after synchronization
 */
static uint64_t
scorep_cupticb_synchronize_context( void )
{
    uint64_t time;

    SCOREP_SUSPEND_CUDRV_CALLBACKS();

    if ( is_driver_domain_enabled )
    {
        SCOREP_CUDA_DRIVER_CALL( cuCtxSynchronize() );
        time = SCOREP_Timer_GetClockTicks();
    }
    else
    {
        if ( scorep_cuda_sync_level > SCOREP_CUDA_RECORD_SYNC )
        {
            time = SCOREP_Timer_GetClockTicks();
            /* With 'location == NULL' SCOREP_Location_EnterRegion will
             * write the event on the current CPU location */
            SCOREP_Location_EnterRegion( NULL, time, cuda_sync_region_handle );
        }

        SCOREP_CUDA_DRIVER_CALL( cuCtxSynchronize() );
        time = SCOREP_Timer_GetClockTicks();

        if ( scorep_cuda_sync_level > SCOREP_CUDA_RECORD_SYNC )
        {
            /* With 'location == NULL' SCOREP_Location_ExitRegion will
             * write the event on the current CPU location */
            SCOREP_Location_ExitRegion( NULL, time, cuda_sync_region_handle );
        }
    }

    SCOREP_RESUME_CUDRV_CALLBACKS();

    return time;
}

/*
 * Create and add the default stream to the given Score-P CUPTI context
 *
 * @param context pointer to the Score-P CUPTI context
 */
static void
scorep_cupticb_create_default_stream( scorep_cupti_context* context )
{
    uint32_t cuStrmID = SCOREP_CUPTI_NO_STREAM_ID;

    // create a Score-P CUPTI stream
    if ( context->activity == NULL )
    {
        SCOREP_CUPTI_CALL( cuptiGetStreamId( context->cuda_context, NULL, &cuStrmID ) );
    }
    else
    {
        cuStrmID = context->activity->default_strm_id;
    }

    context->streams = scorep_cupti_stream_create( context, cuStrmID );
}

/*
 * Increases the "Allocated CUDA memory" counter.
 *
 * @param cudaContext CUDA context
 * @param address pointer to the allocated memory (needed for handle_cuda_free())
 * @param size the number of bytes allocated
 */
static void
handle_cuda_malloc( CUcontext cudaContext,
                    uint64_t  address,
                    size_t    size )
{
    if ( address == ( uint64_t )NULL )
    {
        return;
    }

    scorep_cupti_context* context = scorep_cupti_context_get_create( cudaContext );

    /* lock the work on the context */
    SCOREP_CUPTI_LOCK();

    scorep_cupti_gpumem* gpu_memory = context->free_cuda_mallocs;
    if ( gpu_memory )
    {
        context->free_cuda_mallocs = gpu_memory->next;
    }
    else
    {
        gpu_memory = SCOREP_Memory_AllocForMisc( sizeof( *gpu_memory ) );
    }

    /* set address and size of the allocated GPU memory */
    gpu_memory->address = address;
    gpu_memory->size    = size;

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

    /* synchronize context before (implicit activity buffer flush)
       (assume that the given context is the current one) */
    if ( !( scorep_cuda_record_kernels && scorep_cuda_record_memcpy ) ||
         ( ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy ) &&
           !scorep_cupti_activity_is_buffer_empty( context ) ) )
    {
        scorep_cupticb_synchronize_context();
    }

    /* write counter value */
    {
        uint64_t time = SCOREP_Timer_GetClockTicks();

        if ( time < context->streams->scorep_last_timestamp )
        {
            UTILS_WARN_ONCE( "[CUPTI Callbacks] cudaMalloc: time stamp < last written "
                             "timestamp! (CUDA device: %d)",
                             context->cuda_device );

            time = context->streams->scorep_last_timestamp;
        }
        else
        {
            /* remember the last written time stamp on the default stream */
            context->streams->scorep_last_timestamp = time;
        }

        SCOREP_Location_TriggerCounterUint64( context->streams->scorep_location,
                                              time,
                                              scorep_cupti_sampling_set_gpumemusage,
                                              ( uint64_t )( context->gpu_memory_allocated ) );
    }
}

/*
 * Decreases the "Allocated CUDA memory" counter.
 *
 * @param cudaContext CUDA context
 * @param devicePtr pointer to the allocated memory
 */
static void
handle_cuda_free( CUcontext cudaContext,
                  uint64_t  devicePtr )
{
    if ( devicePtr == ( uint64_t )NULL )
    {
        return;
    }

    scorep_cupti_context* context = scorep_cupti_context_get_create( cudaContext );

    /* synchronize context before
       (assume that the given context is the current one) */
    if ( !( scorep_cuda_record_kernels && scorep_cuda_record_memcpy ) ||
         ( ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy ) &&
           !scorep_cupti_activity_is_buffer_empty( context ) ) )
    {
        scorep_cupticb_synchronize_context();
    }

    SCOREP_CUPTI_LOCK();
    scorep_cupti_gpumem** gpumem_it = &context->cuda_mallocs;
    while ( *gpumem_it != NULL )
    {
        scorep_cupti_gpumem* current_gpumem = *gpumem_it;
        if ( devicePtr == current_gpumem->address )
        {
            uint64_t time = SCOREP_Timer_GetClockTicks();

            if ( time < context->streams->scorep_last_timestamp )
            {
                UTILS_WARN_ONCE( "[CUPTI Callbacks] cudaFree: time stamp < last written "
                                 "timestamp! (CUDA device: %d)",
                                 context->cuda_device );

                time = context->streams->scorep_last_timestamp;
            }
            else
            {
                /* remember the last written time stamp on the default stream */
                context->streams->scorep_last_timestamp = time;
            }

            SCOREP_Location_TriggerCounterUint64( context->streams->scorep_location,
                                                  time,
                                                  scorep_cupti_sampling_set_gpumemusage,
                                                  ( uint64_t )( context->gpu_memory_allocated ) );

            /* decrease allocated counter value */
            context->gpu_memory_allocated -= current_gpumem->size;

            /* set pointer over current element to next one */
            *gpumem_it = current_gpumem->next;

            /* put to free list */
            current_gpumem->next       = context->free_cuda_mallocs;
            context->free_cuda_mallocs = current_gpumem;

            SCOREP_CUPTI_UNLOCK();
            return;
        }
        gpumem_it = &current_gpumem->next;
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
                    enum cudaMemcpyKind       kind,
                    uint64_t                  bytes,
                    uint64_t                  time,
                    SCOREP_RegionHandle       region )
{
    SCOREP_Location* stream_location;
    SCOREP_Location* host_location = SCOREP_Location_GetCurrentCPULocation();

    if ( cbInfo->callbackSite == CUPTI_API_ENTER )
    {
        scorep_cupti_context* context = NULL;
        scorep_cupti_stream*  stream  = NULL;

        SCOREP_CUPTI_CALL( cuptiActivityDisable( CUPTI_ACTIVITY_KIND_MEMCPY ) );

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
                     !scorep_cupti_activity_is_buffer_empty( context ) )
                {
                    time = scorep_cupticb_synchronize_context();
                }
            }

            if ( time < stream->scorep_last_timestamp )
            {
                UTILS_WARN_ONCE( "[CUPTI Callbacks] memcpy start: "
                                 "time stamp < last written time stamp! "
                                 "(CUDA device: %d) Using the last written time stamp.",
                                 context->cuda_device );

                time = stream->scorep_last_timestamp;
            }
            else
            {
                /* remember the last time stamp to check for a valid stop time */
                stream->scorep_last_timestamp = time;
            }

            /* pure idle time */
            if ( scorep_cuda_record_idle == SCOREP_CUDA_PURE_IDLE )
            {
                if ( NULL != context->activity )
                {
                    if ( context->activity->gpu_idle )
                    {
                        SCOREP_Location_ExitRegion( stream_location,
                                                    time, scorep_cupti_idle_region_handle );
                        context->activity->gpu_idle = false;
                    }
                }
                else
                {
                    SCOREP_Location_ExitRegion( stream_location,
                                                time, scorep_cupti_idle_region_handle );
                }
            }

            SCOREP_CUPTI_LOCK();

            if ( kind != cudaMemcpyDeviceToDevice &&
                 context->location_id == SCOREP_CUPTI_NO_ID )
            {
                context->location_id = scorep_cupti_location_counter++;

                /* create window on every location, where it is used
                   SCOREP_RmaWinCreate( scorep_cuda_window_handle );*/
            }

            if ( SCOREP_CUPTI_NO_ID == stream->location_id )
            {
                stream->location_id = scorep_cupti_location_counter++;

                /* create window on every location, where it is used */
                SCOREP_Location_RmaWinCreate( stream->scorep_location, time,
                                              scorep_cuda_window_handle );
            }

            SCOREP_CUPTI_UNLOCK();
        }

        if ( kind == cudaMemcpyHostToDevice )
        {
            SCOREP_Location_RmaGet( stream_location, time,
                                    scorep_cuda_window_handle,
                                    context->location_id, bytes, 42 );
        }
        else if ( kind == cudaMemcpyDeviceToHost )
        {
            SCOREP_Location_RmaPut( stream_location, time,
                                    scorep_cuda_window_handle,
                                    context->location_id, bytes, 42 );
        }
        else if ( kind == cudaMemcpyDeviceToDevice )
        {
            SCOREP_Location_RmaGet( stream_location, time,
                                    scorep_cuda_window_handle,
                                    stream->location_id, bytes, 42 );
        }

        if ( ( region != SCOREP_INVALID_REGION ) &&
             ( !SCOREP_Filtering_MatchFunction( cbInfo->functionName, NULL ) ) )
        {
            /* With 'location == NULL' SCOREP_Location_EnterRegion will
             * write the event on the current CPU location */
            SCOREP_Location_EnterRegion( NULL, time, region );
        }
    }

    if ( cbInfo->callbackSite == CUPTI_API_EXIT )
    {
        scorep_cupti_stream* stream = ( scorep_cupti_stream* )( *cbInfo->correlationData );
        stream_location = stream->scorep_location;

        if ( time < stream->scorep_last_timestamp )
        {
            UTILS_WARN_ONCE( "[CUPTI Callbacks] memcpy end: "
                             "time stamp < last written time stamp! "
                             "Using the last written time stamp." );

            time = stream->scorep_last_timestamp;
        }
        else
        {
            stream->scorep_last_timestamp = time;
        }

        if ( kind == cudaMemcpyDeviceToDevice )
        {
            SCOREP_Location_RmaOpCompleteBlocking( stream_location, time,
                                                   scorep_cuda_window_handle, 42 );
        }
        else if ( kind != cudaMemcpyHostToHost )
        {
            SCOREP_Location_RmaOpCompleteBlocking( stream_location, time,
                                                   scorep_cuda_window_handle, 42 );
        }

        if ( ( region != SCOREP_INVALID_REGION ) &&
             ( !SCOREP_Filtering_MatchFunction( cbInfo->functionName, NULL ) ) )
        {
            /* With 'location == NULL' SCOREP_Location_ExitRegion will
             * write the event on the current CPU location */
            SCOREP_Location_ExitRegion( NULL, time, region );
        }

        /* pure idle time */
        if ( scorep_cuda_record_idle == SCOREP_CUDA_PURE_IDLE )
        {
            scorep_cupti_context* context = scorep_cupti_context_get_create( cbInfo->context );

            if ( context->streams != NULL )
            {
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
                {
                    SCOREP_Location_EnterRegion( context->streams->scorep_location,
                                                 time, scorep_cupti_idle_region_handle );
                }
            }
        }

        if ( scorep_cupti_activity_state & SCOREP_CUPTI_ACTIVITY_STATE_MEMCPY )
        {
            SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_MEMCPY ) );
        }
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
                            CUdeviceptr               cuSrcDevPtr,
                            CUdeviceptr               cuDstDevPtr,
                            uint64_t                  bytes,
                            uint64_t                  time,
                            SCOREP_RegionHandle       region )
{
    CUcontext           cuSrcCtx;
    CUcontext           cuDstCtx;
    CUmemorytype        srcMemType;
    CUmemorytype        dstMemType;
    enum cudaMemcpyKind kind = cudaMemcpyDefault;

    /* do not trace these CUDA driver API function calls */
    SCOREP_SUSPEND_CUDRV_CALLBACKS();

    cuPointerGetAttribute( &cuSrcCtx, CU_POINTER_ATTRIBUTE_CONTEXT,
                           cuSrcDevPtr );
    cuPointerGetAttribute( &cuDstCtx, CU_POINTER_ATTRIBUTE_CONTEXT,
                           cuDstDevPtr );

    cuPointerGetAttribute( &srcMemType, CU_POINTER_ATTRIBUTE_MEMORY_TYPE,
                           cuSrcDevPtr );
    cuPointerGetAttribute( &dstMemType, CU_POINTER_ATTRIBUTE_MEMORY_TYPE,
                           cuDstDevPtr );

    SCOREP_RESUME_CUDRV_CALLBACKS();

    /* get memory copy direction */
    kind = get_cuda_memcpy_kind( srcMemType, dstMemType );

    if ( kind == cudaMemcpyDefault )
    {
        UTILS_WARNING( "[CUPTI Callbacks] Could not determine memory copy kind! "
                       "Skipping this memory copy!" );
        return;
    }

    /* if device<->host the context should be the current one */

    /* ignore peer-to-peer memory copy */
    if ( cuSrcCtx != cuDstCtx )
    {
        if ( kind != cudaMemcpyDeviceToDevice )
        {
            /* device <-> host memory copies can be written on the current context,
             * even if the device memory in a different context, than the host memory */
            handle_cuda_memcpy( cbInfo, kind, bytes, time, region );
        }
    }
    else
    {
        if ( cbInfo->context == cuSrcCtx )
        {
            handle_cuda_memcpy( cbInfo, kind, bytes, time, region );
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

            handle_cuda_memcpy( cbInfo, kind, bytes, time, region );
        }
        else
        {
            UTILS_WARNING( "[CUPTI Callbacks] Memory copy within context skipped! "
                           "(kind=%d)", kind );
        }
    }
}

/* -------------START: Implementation of public functions ------------------ */
/* ------------------------------------------------------------------------- */

/**
 * Initialize the Score-P CUPTI callback implementation.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_cupti_callbacks_init( void )
{
    if ( !scorep_cupti_callbacks_initialized )
    {
        scorep_cuda_set_features();
        scorep_cupti_init();

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                            "[CUPTI Callbacks] Initializing ... " );

        /* check the CUDA APIs to be traced */
        scorep_record_driver_api     = false;
        scorep_record_runtime_api    = false;
        scorep_cupti_callbacks_state = SCOREP_CUPTI_CALLBACKS_STATE_NONE;

        /* check for CUDA runtime API */
        if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_RUNTIME_API ) == SCOREP_CUDA_FEATURE_RUNTIME_API )
        {
            scorep_record_runtime_api     = true;
            scorep_cupti_callbacks_state |= SCOREP_CUPTI_CALLBACKS_STATE_RUNTIME;
            cuda_runtime_file_handle      = SCOREP_Definitions_NewSourceFile( "CUDART" );
        }

        /* check for CUDA driver API */
        if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_DRIVER_API ) == SCOREP_CUDA_FEATURE_DRIVER_API )
        {
            scorep_record_driver_api      = true;
            scorep_cupti_callbacks_state |= SCOREP_CUPTI_CALLBACKS_STATE_DRIVER;
            cuda_driver_file_handle       = SCOREP_Definitions_NewSourceFile( "CUDRV" );
        }

        scorep_cupti_set_callback( scorep_cupti_callbacks_all );

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
                        SCOREP_PARADIGM_CUDA,
                        0,
                        NULL );

                scorep_cuda_window_handle =
                    SCOREP_Definitions_NewRmaWindow(
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
                                                      0,
                                                      "#",
                                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                                      SCOREP_INVALID_METRIC );

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
                                                      0,
                                                      "#",
                                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                                      SCOREP_INVALID_METRIC );

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
                                                      0,
                                                      "#",
                                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                                      SCOREP_INVALID_METRIC );

                    scorep_cupti_sampling_set_threads_per_kernel =
                        SCOREP_Definitions_NewSamplingSet( 1, &metric_handle_tpk,
                                                           SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS, SCOREP_SAMPLING_SET_GPU );
                }
            }

            {
                SCOREP_SourceFileHandle scorep_cuda_sync_file_handle =
                    SCOREP_Definitions_NewSourceFile( "CUDA_SYNC" );
                cuda_sync_region_handle = SCOREP_Definitions_NewRegion(
                    "DEVICE SYNCHRONIZE", NULL, scorep_cuda_sync_file_handle,
                    0, 0, SCOREP_PARADIGM_CUDA, SCOREP_REGION_IMPLICIT_BARRIER );
            }


            scorep_cupti_activity_init();
        }     /* scorep_cuda_record_kernels || scorep_cuda_record_memcpy || scorep_cuda_record_gpumemusage */
              /*}  scorep_gpu_get_config() != 0 */

        scorep_cupti_callbacks_initialized = true;

        scorep_cupti_callbacks_enable( true );
    } /* !scorep_cupti_callbacks_initialized */
}

/*
 * Finalize the Score-P CUPTI callback implementation.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_cupti_callbacks_finalize( void )
{
    if ( !scorep_cupti_callbacks_finalized && scorep_cupti_callbacks_initialized )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                            "[CUPTI Callbacks] Finalizing ... " );

        if ( scorep_record_runtime_api || scorep_record_driver_api ||
             scorep_cuda_record_kernels || scorep_cuda_record_memcpy ||
             scorep_cuda_record_gpumemusage )
        {
            SCOREP_CUPTI_CALL( cuptiUnsubscribe( scorep_cupti_callbacks_subscriber ) );
        }

        if ( scorep_cuda_features & SCOREP_CUDA_FEATURE_FLUSHATEXIT )
        {
            scorep_cupti_context* context = scorep_cupti_context_list;

            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                                "[CUPTI Callbacks] Force CUPTI activity flush at program exit." );

            while ( NULL != context )
            {
                scorep_cupti_activity_context_flush( context );
                context = context->next;
            }
        }

        /* create the global CUDA communication group before the structures
           are destroyed */
        if ( scorep_cuda_record_memcpy )
        {
            scorep_cuda_global_location_number =
                scorep_cupti_create_cuda_comm_group( &scorep_cuda_global_location_ids );
        }

        /* clean up the Score-P CUPTI context list */
        while ( scorep_cupti_context_list != NULL )
        {
            scorep_cupti_context* context = scorep_cupti_context_list;

            scorep_cupti_context_list = scorep_cupti_context_list->next;

            if ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy ||
                 scorep_cuda_record_gpumemusage )
            {
                scorep_cupti_activity_context_finalize( context );
            }

            /* this will free the allocated memory of the context as well */
            scorep_cupti_context_finalize( context );

            context = NULL;
        }

        scorep_cupti_callbacks_finalized = true;

        if ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy ||
             scorep_cuda_record_gpumemusage )
        {
            scorep_cupti_activity_finalize();
        }

        scorep_cupti_finalize();
    }
}
