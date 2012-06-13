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
static bool scorep_cupti_callbacks_initialized = false;
static bool scorep_cupti_callbacks_finalized   = false;

/* flag: Recording of CUDA runtime API functions enabled? */
static bool scorep_cupti_callbacks_record_cudart = false;

/* flag: Recording of CUDA driver API functions enabled? */
static bool scorep_cupti_callbacks_record_cuda_driver = false;

/* global subscriber handle */
static CUpti_SubscriberHandle scorep_cupti_callbacks_subscriber;

/* CUDA runtime and driver API source file handle (function group description) */
static SCOREP_SourceFileHandle cuda_runtime_file_handle = SCOREP_INVALID_SOURCE_FILE;
static SCOREP_SourceFileHandle cuda_driver_file_handle  = SCOREP_INVALID_SOURCE_FILE;

/****************** Some internal function declarations ***********************/
static void
scorep_cupti_set_callback( CUpti_CallbackFunc   callback,
                           CUpti_CallbackDomain domain,
                           CUpti_CallbackId     callbackId );

void CUPTIAPI
scorep_cupti_callback_all( void*                userdata,
                           CUpti_CallbackDomain domain,
                           CUpti_CallbackId     callbackId,
                           const void*          callbackInfo );

void ( * scorep_cupti_callback_all_ptr )( void*                userdata,
                                          CUpti_CallbackDomain domain,
                                          CUpti_CallbackId     callbackId,
                                          const void*          callbackInfo )
    = scorep_cupti_callback_all;

void CUPTIAPI
scorep_cupti_callback_cuda_runtime( CUpti_CallbackId          callbackId,
                                    const CUpti_CallbackData* callbackInfo );

void
scorep_cupti_callback_driver_api( CUpti_CallbackId          callbackId,
                                  const CUpti_CallbackData* callbackInfo );

void
scorep_cupti_callback_resource( CUpti_CallbackId          callbackId,
                                const CUpti_ResourceData* callbackInfo );

void
scorep_cupti_callback_sync( CUpti_CallbackId             callbackId,
                            const CUpti_SynchronizeData* callbackInfo );

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
    if ( scorep_cupti_callbacks_record_cuda_driver && scorep_cupti_callbacks_record_cudart )
    {
        uint16_t offset = 0;

        if ( domain == CUPTI_CB_DOMAIN_DRIVER_API )
        {
            offset = CUPTI_CALLBACKS_CUDA_API_FUNC_MAX / 2;
        }

        index = offset + ( uint32_t )callbackId;

        if ( ( domain == CUPTI_CB_DOMAIN_RUNTIME_API ) &&
             ( index >= CUPTI_CALLBACKS_CUDA_API_FUNC_MAX - offset ) )
        {
            index = 0;

            SCOREP_ERROR( SCOREP_WARNING,
                          "[CUPTI Callbacks] Hash table for CUDA runtime API "
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

        SCOREP_ERROR( SCOREP_WARNING,
                      "[CUPTI Callbacks] Hash table for CUDA API "
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

void
scorep_cupti_callbacks_init()
{
    if ( !scorep_cupti_callbacks_initialized )
    {
        SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                 "[CUPTI Callbacks] Initializing ... \n" );

        /* check for CUDA runtime API tracing */
        if ( ( scorep_cuda_features & SCOREP_CUDA_RECORD_RUNTIME_API )
             == SCOREP_CUDA_RECORD_RUNTIME_API )
        {
            scorep_cupti_callbacks_record_cudart = true;
        }

        /* check for CUDA runtime API tracing */
        if ( ( scorep_cuda_features & SCOREP_CUDA_RECORD_DRIVER_API )
             == SCOREP_CUDA_RECORD_DRIVER_API )
        {
            scorep_cupti_callbacks_record_cuda_driver = true;
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
            scorep_cupti_set_callback( scorep_cupti_callback_all_ptr,
                                       CUPTI_CB_DOMAIN_RESOURCE,
                                       CUPTI_RUNTIME_TRACE_CBID_INVALID );

            scorep_cupti_set_callback( scorep_cupti_callback_all_ptr,
                                       CUPTI_CB_DOMAIN_SYNCHRONIZE,
                                       CUPTI_RUNTIME_TRACE_CBID_INVALID );

            scorep_cupti_activity_init();
        }

        if ( scorep_cupti_callbacks_record_cudart )
        {
            cuda_runtime_file_handle = SCOREP_DefineSourceFile( "CUDART" );

            scorep_cupti_set_callback( scorep_cupti_callback_all_ptr,
                                       CUPTI_CB_DOMAIN_RUNTIME_API,
                                       CUPTI_RUNTIME_TRACE_CBID_INVALID );
        }

        if ( scorep_cupti_callbacks_record_cuda_driver )
        {
            cuda_driver_file_handle = SCOREP_DefineSourceFile( "CUDRV" );

            scorep_cupti_set_callback( scorep_cupti_callback_all_ptr,
                                       CUPTI_CB_DOMAIN_DRIVER_API,
                                       CUPTI_DRIVER_TRACE_CBID_INVALID );
        }

        /* reset the hash table for CUDA API functions */
        memset( scorep_cupti_callbacks_cuda_function_table, SCOREP_INVALID_REGION,
                CUPTI_CALLBACKS_CUDA_API_FUNC_MAX * sizeof( uint32_t ) );

        atexit( scorep_cupti_callbacks_finalize );

        scorep_cupti_callbacks_initialized = true;
    }
}

void
scorep_cupti_callbacks_finalize()
{
    if ( !scorep_cupti_callbacks_finalized && scorep_cupti_callbacks_initialized )
    {
        SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                 "[CUPTI Callbacks] Finalizing ... \n" );

        if ( scorep_cuda_record_kernels > 0 || scorep_cuda_record_memcpy )
        {
            scorep_cupti_activity_finalize();
        }

        scorep_cupti_callbacks_finalized = true;
    }
}

/**
 * Set a CUPTI callback function for a specific CUDA runtime or driver function
 * or for a whole domain (runtime or driver API)
 *
 * @param callback          Callback function.
 * @param domain            Domain of the callback.
 * @param callbackId        ID of the API function associated with this
 *                          callback. If it is not valid, the whole
 *                          domain will be enabled.
 */
static void
scorep_cupti_set_callback( CUpti_CallbackFunc   callback,
                           CUpti_CallbackDomain domain,
                           CUpti_CallbackId     callbackId )
{
    CUptiResult    cupti_error;
    static uint8_t initflag = 1;

    if ( initflag )
    {
        initflag = 0;

        SCOREP_CUDRV_CALL( cuInit( 0 ) );

        /* only one subscriber allowed at a time */
        cupti_error = cuptiSubscribe( &scorep_cupti_callbacks_subscriber, callback, NULL );
        SCOREP_CUPTI_CALL( cupti_error );
    }

    if ( CUPTI_CB_DOMAIN_INVALID == domain )
    {
        cuptiEnableAllDomains( 1, scorep_cupti_callbacks_subscriber );
    }
    else
    {
        if ( ( callbackId == CUPTI_RUNTIME_TRACE_CBID_INVALID ) ||
             ( callbackId == CUPTI_DRIVER_TRACE_CBID_INVALID ) )
        {
            cupti_error = cuptiEnableDomain( 1, scorep_cupti_callbacks_subscriber, domain );
            SCOREP_CUPTI_CALL( cupti_error );
        }
        else
        {
            cupti_error = cuptiEnableCallback( 1, scorep_cupti_callbacks_subscriber, domain, callbackId );
            SCOREP_CUPTI_CALL( cupti_error );
        }
    }
}

/**
 * This function chooses the callback handling depending on the CUPTI domain.
 *
 * @param userdata          Not needed/used.
 * @param domain            Callback domain (runtime or driver API).
 * @param callbackId        ID of the callback function in the given domain
 * @param callbackInfo      Information about the callback.
 */
void CUPTIAPI
scorep_cupti_callback_all( void*                userdata,
                           CUpti_CallbackDomain domain,
                           CUpti_CallbackId     callbackId,
                           const void*          callbackInfo )
{
    if ( CUPTI_CB_DOMAIN_RUNTIME_API == domain )
    {
        scorep_cupti_callback_cuda_runtime( callbackId, ( CUpti_CallbackData* )callbackInfo );
    }

    if ( CUPTI_CB_DOMAIN_DRIVER_API == domain )
    {
        scorep_cupti_callback_driver_api( callbackId, ( CUpti_CallbackData* )callbackInfo );
    }

    if ( CUPTI_CB_DOMAIN_RESOURCE == domain )
    {
        scorep_cupti_callback_resource( callbackId, ( CUpti_ResourceData* )callbackInfo );
    }

    if ( CUPTI_CB_DOMAIN_SYNCHRONIZE == domain )
    {
        scorep_cupti_callback_sync( callbackId, ( CUpti_SynchronizeData* )callbackInfo );
    }
}

/**
 * This callback function is used to record the CUDA runtime API.
 *
 * @param callbackId        ID of the callback function in the given domain.
 * @param callbackInfo      Information about the callback.
 */
void CUPTIAPI
scorep_cupti_callback_cuda_runtime( CUpti_CallbackId          callbackId,
                                    const CUpti_CallbackData* callbackInfo )
{
    SCOREP_RegionHandle region_handle        = SCOREP_INVALID_REGION;
    SCOREP_RegionHandle region_handle_stored = SCOREP_INVALID_REGION;

    if ( callbackId == CUPTI_RUNTIME_TRACE_CBID_INVALID )
    {
        return;
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
        /* replace SCOREP_REGION_FUNCTION with "CUDART_API" */
        region_handle = SCOREP_DefineRegion( callbackInfo->functionName, cuda_runtime_file_handle,
                                             0, 0, SCOREP_ADAPTER_CUDA, SCOREP_REGION_FUNCTION );

        cuda_api_function_put( CUPTI_CB_DOMAIN_RUNTIME_API, callbackId, region_handle );
    }

    /*********** write enter and exit records for CUDA runtime API **************/
    if ( callbackInfo->callbackSite == CUPTI_API_ENTER )
    {
        SCOREP_EnterRegion( region_handle );
    }

    if ( callbackInfo->callbackSite == CUPTI_API_EXIT )
    {
        SCOREP_ExitRegion( region_handle );
    }
}

/**
 * This callback function is used to record the CUDA driver API.
 *
 * @param callbackId        ID of the callback function in the given domain.
 * @param callbackInfo      Information about the callback.
 */
void CUPTIAPI
scorep_cupti_callback_driver_api( CUpti_CallbackId          callbackId,
                                  const CUpti_CallbackData* callbackInfo )
{
    SCOREP_RegionHandle region_handle        = SCOREP_INVALID_REGION;
    SCOREP_RegionHandle region_handle_stored = SCOREP_INVALID_REGION;

    if ( callbackId == CUPTI_DRIVER_TRACE_CBID_INVALID )
    {
        return;
    }

    if ( !scorep_cupti_callbacks_record_cuda_driver )
    {
        return;
    }

    /* get the Score-P region handle for the API function */
    region_handle_stored =
        cuda_api_function_get( CUPTI_CB_DOMAIN_DRIVER_API, callbackId );
    if ( region_handle_stored != SCOREP_INVALID_REGION )
    {
        region_handle = region_handle_stored;
    }
    else
    {
        /* replace SCOREP_REGION_FUNCTION with "CUDRV_API" */
        region_handle = SCOREP_DefineRegion( callbackInfo->functionName, cuda_driver_file_handle,
                                             0, 0, SCOREP_ADAPTER_CUDA, SCOREP_REGION_FUNCTION );

        cuda_api_function_put( CUPTI_CB_DOMAIN_DRIVER_API, callbackId, region_handle );
    }

    /*********** write enter and exit records for CUDA runtime API **************/
    if ( callbackInfo->callbackSite == CUPTI_API_ENTER )
    {
        SCOREP_EnterRegion( region_handle );
    }

    if ( callbackInfo->callbackSite == CUPTI_API_EXIT )
    {
        SCOREP_ExitRegion( region_handle );
    }
}

/**
 * This callback function is used to handle synchronization calls.
 *
 * @param callbackId the ID of the callback function in the given domain
 * @param syncData synchronization data (CUDA context, CUDA stream)
 */
void
scorep_cupti_callback_sync( CUpti_CallbackId             callbackId,
                            const CUpti_SynchronizeData* syncData )
{
    if ( CUPTI_CBID_SYNCHRONIZE_CONTEXT_SYNCHRONIZED == callbackId )
    {
        SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                 "[CUPTI Callbacks] Synchronize called" );

        scorep_cupti_activity_flush_context_activities( syncData->context );
    }
}

/**
 * This callback function is used to handle resource usage.
 *
 * @param callbackId        ID of the callback function in the given domain.
 * @param resourceData      Resource information (CUDA context, CUDA stream).
 */
void
scorep_cupti_callback_resource( CUpti_CallbackId          callbackId,
                                const CUpti_ResourceData* resourceData )
{
    switch ( callbackId )
    {
        /********************** CUDA memory allocation ******************************/
        case CUPTI_CBID_RESOURCE_CONTEXT_CREATED:
        {
            SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                     "[CUPTI Callbacks] Creating context %d \n",
                                     resourceData->context );
            if ( scorep_cupti_callbacks_record_cuda_driver )
            {
                cuptiEnableDomain( 0, scorep_cupti_callbacks_subscriber, CUPTI_CB_DOMAIN_DRIVER_API );
            }

            scorep_cupti_activity_add_context( resourceData->context, ( CUdevice ) - 1 );

            if ( scorep_cupti_callbacks_record_cuda_driver )
            {
                cuptiEnableDomain( 1, scorep_cupti_callbacks_subscriber, CUPTI_CB_DOMAIN_DRIVER_API );
            }

            break;
        }

        case CUPTI_CBID_RESOURCE_CONTEXT_DESTROY_STARTING:
        {
            SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                     "[CUPTI Callbacks] Destroying context" );
            scorep_cupti_activity_flush_context_activities( resourceData->context );

            break;
        }
        default:
            break;
    }
}
