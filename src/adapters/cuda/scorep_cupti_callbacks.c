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

#include "scorep_cuda.h"
#include "scorep_cupti_callbacks.h"
#include "scorep_cupti_activity.h"

/* initialization and finalization flags */
static bool sp_cupticb_initialized = false;
static bool sp_cupticb_finalized   = false;

/* flag: Recording of CUDA runtime API functions enabled? */
static bool sp_cupticb_record_cudart = false;

/* flag: Recording of CUDA driver API functions enabled? */
static bool sp_cupticb_record_cudrv = false;

/* global subscriber handle */
static CUpti_SubscriberHandle sp_cupticb_subscriber;

/* CUDA runtime and driver API source file handle (function group description) */
static SCOREP_SourceFileHandle cudartFileHandle = SCOREP_INVALID_SOURCE_FILE;
static SCOREP_SourceFileHandle cudrvFileHandle  = SCOREP_INVALID_SOURCE_FILE;

/****************** Some internal function declarations ***********************/
static void
sp_cupti_set_callback( CUpti_CallbackFunc   callback,
                       CUpti_CallbackDomain domain,
                       CUpti_CallbackId     cbid );

void CUPTIAPI
sp_cupticb_all( void*,
                CUpti_CallbackDomain,
                CUpti_CallbackId,
                const void* );

void ( * sp_cupticb_all_ptr )( void*,
                               CUpti_CallbackDomain,
                               CUpti_CallbackId,
                               const void* )
    = sp_cupticb_all;

void CUPTIAPI
sp_cupticb_cudart( CUpti_CallbackId,
                   const CUpti_CallbackData* );

void
sp_cupticb_driverAPI( CUpti_CallbackId,
                      const CUpti_CallbackData* );

void
sp_cupticb_resource( CUpti_CallbackId,
                     const CUpti_ResourceData* );

void
sp_cupticb_sync( CUpti_CallbackId,
                 const CUpti_SynchronizeData* );

/******************************************************************************/

/************************** CUDA function table *******************************/
#define SP_CUPTICB_CUDA_API_FUNC_MAX 1024
static SCOREP_RegionHandle sp_cupticb_cudaFuncTab[ SP_CUPTICB_CUDA_API_FUNC_MAX ];

/*
 * This is a pseudo hash function for CUPTI callbacks. No real hash is needed,
 * as the callback IDs are 3-digit integer values, which can be stored directly
 * in an array.
 *
 * @param domain the CUPTI callback domain
 * @param cid the CUPTI callback ID
 *
 * @return the position in the hash table (index)
 */
static uint32_t
sp_cupticb_cudaApiHashFunc( CUpti_CallbackDomain domain,
                            CUpti_CallbackId     cid )
{
    uint32_t idx = 0;

    /* Use an offset for the driver API functions, if CUDA runtime and driver
       API recording is enabled (uncommon case) */
    if ( sp_cupticb_record_cudrv && sp_cupticb_record_cudart )
    {
        uint16_t offset = 0;

        if ( domain == CUPTI_CB_DOMAIN_DRIVER_API )
        {
            offset = SP_CUPTICB_CUDA_API_FUNC_MAX / 2;
        }

        idx = offset + ( uint32_t )cid;

        if ( ( domain == CUPTI_CB_DOMAIN_RUNTIME_API ) &&
             ( idx >= SP_CUPTICB_CUDA_API_FUNC_MAX - offset ) )
        {
            idx = 0;

            SCOREP_ERROR( SCOREP_WARNING,
                          "[CUPTI Callbacks] Hash table for CUDA runtime API "
                          "function %d is to small!", cid );
        }
    }
    else
    {
        idx = ( uint32_t )cid;
    }

    if ( idx >= SP_CUPTICB_CUDA_API_FUNC_MAX )
    {
        idx = 0;

        SCOREP_ERROR( SCOREP_WARNING,
                      "[CUPTI Callbacks] Hash table for CUDA API "
                      "function %d is to small!", cid );
    }

    return ( uint32_t )idx;
}

/*
 * Store a CUPTI callback together with a ScoreP region handle.
 *
 * @param domain the CUPTI callback domain
 * @param cid the CUPTI callback ID
 * @param rid the ScoreP region handle
 */
static void
sp_cupticb_cudaApiFuncPut( CUpti_CallbackDomain domain,
                           CUpti_CallbackId     cid,
                           SCOREP_RegionHandle  rid )
{
    sp_cupticb_cudaFuncTab[ sp_cupticb_cudaApiHashFunc( domain, cid ) ] = rid;
}

/*
 * Retrieve the ScoreP region handle of a CUPTI callback.
 *
 * @param domain the CUPTI callback domain
 * @param cid the CUPTI callback ID
 *
 * @return the corresponding ScoreP region handle
 */
static SCOREP_RegionHandle
sp_cupticb_cudaApiFuncGet(
    CUpti_CallbackDomain domain,
    CUpti_CallbackId     cid )
{
    return sp_cupticb_cudaFuncTab[ sp_cupticb_cudaApiHashFunc( domain, cid ) ];
}
/******************************************************************************/

void
scorep_cupti_callbacks_init()
{
    if ( !sp_cupticb_initialized )
    {
        SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                 "[CUPTI Callbacks] Initializing ... \n" );

        /* check for CUDA runtime API tracing */
        if ( ( scorep_cuda_features & SCOREP_CUDA_RECORD_RUNTIME_API )
             == SCOREP_CUDA_RECORD_RUNTIME_API )
        {
            sp_cupticb_record_cudart = true;
        }

        /* check for CUDA runtime API tracing */
        if ( ( scorep_cuda_features & SCOREP_CUDA_RECORD_DRIVER_API )
             == SCOREP_CUDA_RECORD_DRIVER_API )
        {
            sp_cupticb_record_cudrv = true;
        }

        /* check for CUDA kernel tracing */
        if ( ( scorep_cuda_features & SCOREP_CUDA_RECORD_KERNEL )
             == SCOREP_CUDA_RECORD_KERNEL )
        {
            scorep_cuda_record_kernels = 1;
        }

        /* check for GPU compute idle time tracing */
        if ( ( scorep_cuda_features & SCOREP_CUDA_RECORD_IDLE )
             == SCOREP_CUDA_RECORD_IDLE )
        {
            scorep_cuda_record_idle = true;
        }

        /* TODO: uncomment, if generic DMA records are available */
        /* check for CUDA memory copy tracing
           if ( ( scorep_cuda_features & SCOREP_CUDA_RECORD_MEMCPY )
             == SCOREP_CUDA_RECORD_MEMCPY )
           {
            scorep_cuda_record_memcpy = true;
           }*/

        if ( scorep_cuda_record_kernels > 0 || scorep_cuda_record_memcpy )
        {
            sp_cupti_set_callback( sp_cupticb_all_ptr,
                                   CUPTI_CB_DOMAIN_RESOURCE,
                                   CUPTI_RUNTIME_TRACE_CBID_INVALID );

            sp_cupti_set_callback( sp_cupticb_all_ptr,
                                   CUPTI_CB_DOMAIN_SYNCHRONIZE,
                                   CUPTI_RUNTIME_TRACE_CBID_INVALID );

            scorep_cupti_activity_init();
        }

        if ( sp_cupticb_record_cudart )
        {
            cudartFileHandle = SCOREP_DefineSourceFile( "CUDART" );

            sp_cupti_set_callback( sp_cupticb_all_ptr,
                                   CUPTI_CB_DOMAIN_RUNTIME_API,
                                   CUPTI_RUNTIME_TRACE_CBID_INVALID );
        }

        if ( sp_cupticb_record_cudrv )
        {
            cudrvFileHandle = SCOREP_DefineSourceFile( "CUDRV" );

            sp_cupti_set_callback( sp_cupticb_all_ptr,
                                   CUPTI_CB_DOMAIN_DRIVER_API,
                                   CUPTI_DRIVER_TRACE_CBID_INVALID );
        }

        /* reset the hash table for CUDA API functions */
        memset( sp_cupticb_cudaFuncTab, SCOREP_INVALID_REGION,
                SP_CUPTICB_CUDA_API_FUNC_MAX * sizeof( uint32_t ) );

        atexit( scorep_cupti_callbacks_finalize );

        sp_cupticb_initialized = true;
    }
}

void
scorep_cupti_callbacks_finalize()
{
    if ( !sp_cupticb_finalized && sp_cupticb_initialized )
    {
        SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                 "[CUPTI Callbacks] Finalizing ... \n" );

        if ( scorep_cuda_record_kernels > 0 || scorep_cuda_record_memcpy )
        {
            scorep_cupti_activity_finalize();
        }

        sp_cupticb_finalized = true;
    }
}

/*
 * Set a CUPTI callback function for a specific CUDA runtime or driver function
 * or for a whole domain (runtime or driver API)
 *
 * @param callback the callback function
 * @param domain The domain of the callback
 * @param cbid The ID of the API function associated with this callback, if it
 *             is not valid, the whole domain will be enabled
 */
static void
sp_cupti_set_callback( CUpti_CallbackFunc   callback,
                       CUpti_CallbackDomain domain,
                       CUpti_CallbackId     cbid )
{
    CUptiResult    cuptiErr;
    static uint8_t initflag = 1;

    if ( initflag )
    {
        initflag = 0;

        SCOREP_CUDRV_CALL( cuInit( 0 ) );

        /* only one subscriber allowed at a time */
        cuptiErr = cuptiSubscribe( &sp_cupticb_subscriber, callback, NULL );
        SCOREP_CUPTI_CALL( cuptiErr );
    }

    if ( CUPTI_CB_DOMAIN_INVALID == domain )
    {
        cuptiEnableAllDomains( 1, sp_cupticb_subscriber );
    }
    else
    {
        if ( ( cbid == CUPTI_RUNTIME_TRACE_CBID_INVALID ) ||
             ( cbid == CUPTI_DRIVER_TRACE_CBID_INVALID ) )
        {
            cuptiErr = cuptiEnableDomain( 1, sp_cupticb_subscriber, domain );
            SCOREP_CUPTI_CALL( cuptiErr );
        }
        else
        {
            cuptiErr = cuptiEnableCallback( 1, sp_cupticb_subscriber, domain, cbid );
            SCOREP_CUPTI_CALL( cuptiErr );
        }
    }
}

/*
 * This function chooses the callback handling depending on the CUPTI domain.
 *
 * @param userdata not needed/used
 * @param domain the callback domain (runtime or driver API)
 * @param cbid the ID of the callback function in the given domain
 * @param cbInfo information about the callback
 */
void CUPTIAPI
sp_cupticb_all( void*                userdata,
                CUpti_CallbackDomain domain,
                CUpti_CallbackId     cbid,
                const void*          cbInfo )
{
    if ( CUPTI_CB_DOMAIN_RUNTIME_API == domain )
    {
        sp_cupticb_cudart( cbid, ( CUpti_CallbackData* )cbInfo );
    }

    if ( CUPTI_CB_DOMAIN_DRIVER_API == domain )
    {
        sp_cupticb_driverAPI( cbid, ( CUpti_CallbackData* )cbInfo );
    }

    if ( CUPTI_CB_DOMAIN_RESOURCE == domain )
    {
        sp_cupticb_resource( cbid, ( CUpti_ResourceData* )cbInfo );
    }

    if ( CUPTI_CB_DOMAIN_SYNCHRONIZE == domain )
    {
        sp_cupticb_sync( cbid, ( CUpti_SynchronizeData* )cbInfo );
    }
}

/*
 * This callback function is used to record the CUDA runtime API.
 *
 * @param cbid the ID of the callback function in the given domain
 * @param cbInfo information about the callback
 */
void CUPTIAPI
sp_cupticb_cudart( CUpti_CallbackId          cbid,
                   const CUpti_CallbackData* cbInfo )
{
    SCOREP_RegionHandle sp_region_handle        = SCOREP_INVALID_REGION;
    SCOREP_RegionHandle sp_region_handle_stored = SCOREP_INVALID_REGION;

    if ( cbid == CUPTI_RUNTIME_TRACE_CBID_INVALID )
    {
        return;
    }

    /* get the region handle for the API function */
    sp_region_handle_stored =
        sp_cupticb_cudaApiFuncGet( CUPTI_CB_DOMAIN_RUNTIME_API, cbid );
    if ( sp_region_handle_stored != SCOREP_INVALID_REGION )
    {
        sp_region_handle = sp_region_handle_stored;
    }
    else
    {
        /* replace SCOREP_REGION_FUNCTION with "CUDART_API" */
        sp_region_handle = SCOREP_DefineRegion( cbInfo->functionName, cudartFileHandle,
                                                0, 0, SCOREP_ADAPTER_CUDA, SCOREP_REGION_FUNCTION );

        sp_cupticb_cudaApiFuncPut( CUPTI_CB_DOMAIN_RUNTIME_API, cbid, sp_region_handle );
    }

    /*********** write enter and exit records for CUDA runtime API **************/
    if ( cbInfo->callbackSite == CUPTI_API_ENTER )
    {
        SCOREP_EnterRegion( sp_region_handle );
    }

    if ( cbInfo->callbackSite == CUPTI_API_EXIT )
    {
        SCOREP_ExitRegion( sp_region_handle );
    }
}

/*
 * This callback function is used to record the CUDA driver API.
 *
 * @param cbid the ID of the callback function in the given domain
 * @param cbInfo information about the callback
 */
void CUPTIAPI
sp_cupticb_driverAPI( CUpti_CallbackId          cbid,
                      const CUpti_CallbackData* cbInfo )
{
    SCOREP_RegionHandle sp_region_handle        = SCOREP_INVALID_REGION;
    SCOREP_RegionHandle sp_region_handle_stored = SCOREP_INVALID_REGION;

    if ( cbid == CUPTI_DRIVER_TRACE_CBID_INVALID )
    {
        return;
    }

    if ( !sp_cupticb_record_cudrv )
    {
        return;
    }

    /* get the Score-P region handle for the API function */
    sp_region_handle_stored =
        sp_cupticb_cudaApiFuncGet( CUPTI_CB_DOMAIN_DRIVER_API, cbid );
    if ( sp_region_handle_stored != SCOREP_INVALID_REGION )
    {
        sp_region_handle = sp_region_handle_stored;
    }
    else
    {
        /* replace SCOREP_REGION_FUNCTION with "CUDRV_API" */
        sp_region_handle = SCOREP_DefineRegion( cbInfo->functionName, cudrvFileHandle,
                                                0, 0, SCOREP_ADAPTER_CUDA, SCOREP_REGION_FUNCTION );

        sp_cupticb_cudaApiFuncPut( CUPTI_CB_DOMAIN_DRIVER_API, cbid, sp_region_handle );
    }

    /*********** write enter and exit records for CUDA runtime API **************/
    if ( cbInfo->callbackSite == CUPTI_API_ENTER )
    {
        SCOREP_EnterRegion( sp_region_handle );
    }

    if ( cbInfo->callbackSite == CUPTI_API_EXIT )
    {
        SCOREP_ExitRegion( sp_region_handle );
    }
}

/*
 * This callback function is used to handle synchronization calls.
 *
 * @param cbid the ID of the callback function in the given domain
 * @param syncData synchronization data (CUDA context, CUDA stream)
 */
void
sp_cupticb_sync( CUpti_CallbackId             cbid,
                 const CUpti_SynchronizeData* syncData )
{
    if ( CUPTI_CBID_SYNCHRONIZE_CONTEXT_SYNCHRONIZED == cbid )
    {
        SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                 "[CUPTI Callbacks] Synchronize called" );

        scorep_cuptiact_flushCtxActivities( syncData->context );
    }
}

/*
 * This callback function is used to handle resource usage.
 *
 * @param cbid the ID of the callback function in the given domain
 * @param resData resource information (CUDA context, CUDA stream)
 */
void
sp_cupticb_resource( CUpti_CallbackId          cbid,
                     const CUpti_ResourceData* resData )
{
    switch ( cbid )
    {
        /********************** CUDA memory allocation ******************************/
        case CUPTI_CBID_RESOURCE_CONTEXT_CREATED:
        {
            SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                     "[CUPTI Callbacks] Creating context %d \n",
                                     resData->context );
            if ( sp_cupticb_record_cudrv )
            {
                cuptiEnableDomain( 0, sp_cupticb_subscriber, CUPTI_CB_DOMAIN_DRIVER_API );
            }

            scorep_cuptiact_addContext( resData->context, ( CUdevice ) - 1 );

            if ( sp_cupticb_record_cudrv )
            {
                cuptiEnableDomain( 1, sp_cupticb_subscriber, CUPTI_CB_DOMAIN_DRIVER_API );
            }

            break;
        }

        case CUPTI_CBID_RESOURCE_CONTEXT_DESTROY_STARTING:
        {
            SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                     "[CUPTI Callbacks] Destroying context" );
            scorep_cuptiact_flushCtxActivities( resData->context );

            break;
        }
        default:
            break;
    }
}
