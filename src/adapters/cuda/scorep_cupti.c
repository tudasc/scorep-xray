/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
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
 *  Implementation of common functionality between CUPTI activity, callbacks
 *  and events.
 */

#include <config.h>
#include "scorep_cupti.h"
#include "scorep_cuda.h"     /* CUPTI common structures, functions, etc. */
#include "scorep_cupti_callbacks.h"

#include <SCOREP_Timer_Ticks.h>

#include <UTILS_CStr.h>

#include <stdio.h>

/* String constants for CUDA attribute references */
#define SCOREP_CUPTI_CUDA_STREAMREF_KEY     "CUDA_STREAM_REF"
#define SCOREP_CUPTI_CUDA_EVENTREF_KEY      "CUDA_EVENT_REF"
#define SCOREP_CUPTI_CUDA_CURESULT_KEY      "CUDA_DRV_API_RESULT"

/* String constants for CUDA location properties */
#define SCOREP_CUPTI_LOCATION_NULL_STREAM   "CUDA_NULL_STREAM"

/* hash table for CUDA kernels */
static scorep_cuda_kernel_hash_node* scorep_cuda_kernel_hashtab[ SCOREP_CUDA_KERNEL_HASHTABLE_SIZE ];

/* mutex for locking the CUPTI environment */
SCOREP_Mutex scorep_cupti_mutex = SCOREP_INVALID_MUTEX;

/* attribute handles for CUDA references */
scorep_cupti_attribute_handles scorep_cupti_attributes;

/* set the list of CUPTI contexts to 'empty' */
scorep_cupti_context* scorep_cupti_context_list = NULL;

/* set the location counter to zero */
size_t scorep_cupti_location_counter = 0;

/* handle for kernel regions */
SCOREP_SourceFileHandle scorep_cupti_kernel_file_handle = SCOREP_INVALID_SOURCE_FILE;

/* handle CUDA idle regions */
SCOREP_RegionHandle scorep_cupti_idle_region_handle = SCOREP_INVALID_REGION;

SCOREP_SamplingSetHandle scorep_cupti_sampling_set_gpumemusage =
    SCOREP_INVALID_SAMPLING_SET;

SCOREP_SamplingSetHandle scorep_cupti_sampling_set_threads_per_kernel =
    SCOREP_INVALID_SAMPLING_SET;
SCOREP_SamplingSetHandle scorep_cupti_sampling_set_threads_per_block =
    SCOREP_INVALID_SAMPLING_SET;
SCOREP_SamplingSetHandle scorep_cupti_sampling_set_blocks_per_grid =
    SCOREP_INVALID_SAMPLING_SET;

/* CUPTI activity specific kernel counter IDs */
SCOREP_SamplingSetHandle scorep_cupti_sampling_set_static_shared_mem =
    SCOREP_INVALID_SAMPLING_SET;
SCOREP_SamplingSetHandle scorep_cupti_sampling_set_dynamic_shared_mem =
    SCOREP_INVALID_SAMPLING_SET;
SCOREP_SamplingSetHandle scorep_cupti_sampling_set_local_mem_total =
    SCOREP_INVALID_SAMPLING_SET;
SCOREP_SamplingSetHandle scorep_cupti_sampling_set_registers_per_thread =
    SCOREP_INVALID_SAMPLING_SET;

static bool scorep_cupti_initialized = 0;
static bool scorep_cupti_finalized   = 0;

void
scorep_cupti_init( void )
{
    if ( !scorep_cupti_initialized )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI] Initializing ..." );

        SCOREP_MutexCreate( &scorep_cupti_mutex );

        /* GPU idle time */
        if ( scorep_cuda_record_idle )
        {
            if ( scorep_cuda_record_idle == SCOREP_CUDA_PURE_IDLE &&
                 scorep_cuda_record_memcpy )
            {
                SCOREP_SourceFileHandle file_handle =
                    SCOREP_Definitions_NewSourceFile( "CUDA_IDLE" );
                scorep_cupti_idle_region_handle = SCOREP_Definitions_NewRegion(
                    "GPU IDLE", NULL, file_handle,
                    0, 0, SCOREP_PARADIGM_CUDA, SCOREP_REGION_ARTIFICIAL );
            }
            else if ( scorep_cuda_record_kernels )
            {
                SCOREP_SourceFileHandle file_handle =
                    SCOREP_Definitions_NewSourceFile( "CUDA_IDLE" );
                scorep_cupti_idle_region_handle = SCOREP_Definitions_NewRegion(
                    "COMPUTE IDLE", NULL, file_handle,
                    0, 0, SCOREP_PARADIGM_CUDA, SCOREP_REGION_ARTIFICIAL );
            }
            else
            {
                scorep_cuda_record_idle = SCOREP_CUDA_NO_IDLE;
            }
        }

        /* GPU memory usage */
        if ( scorep_cuda_record_gpumemusage )
        {
            SCOREP_MetricHandle metric_handle =
                SCOREP_Definitions_NewMetric( "gpu_mem_usage",
                                              "GPU memory usage",
                                              SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                              SCOREP_METRIC_MODE_ABSOLUTE_NEXT,
                                              SCOREP_METRIC_VALUE_UINT64,
                                              SCOREP_METRIC_BASE_DECIMAL,
                                              0,
                                              "Byte",
                                              SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                              SCOREP_INVALID_METRIC );

            scorep_cupti_sampling_set_gpumemusage =
                SCOREP_Definitions_NewSamplingSet( 1, &metric_handle,
                                                   SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS, SCOREP_SAMPLING_SET_GPU );
        }

        if ( scorep_cuda_record_references )
        {
            scorep_cupti_attributes.stream_ref = SCOREP_Definitions_NewAttribute(
                SCOREP_CUPTI_CUDA_STREAMREF_KEY,
                "Referenced CUDA stream",
                SCOREP_ATTRIBUTE_TYPE_LOCATION );
            scorep_cupti_attributes.event_ref = SCOREP_Definitions_NewAttribute(
                SCOREP_CUPTI_CUDA_EVENTREF_KEY,
                "ID (address) of referenced CUDA event",
                SCOREP_ATTRIBUTE_TYPE_UINT64 );
            scorep_cupti_attributes.result_ref = SCOREP_Definitions_NewAttribute(
                SCOREP_CUPTI_CUDA_CURESULT_KEY,
                "CUDA driver API function result",
                SCOREP_ATTRIBUTE_TYPE_UINT32 );
        }

        scorep_cupti_initialized = true;
    }
}

/*
 * Finalize the CUPTI common interface.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_cupti_finalize( void )
{
    if ( !scorep_cupti_finalized && scorep_cupti_initialized )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI] Finalizing ..." );

        /* free Score-P CUPTI context structures */
        while ( scorep_cupti_context_list != NULL )
        {
            scorep_cupti_context* tmp =  scorep_cupti_context_list;

            scorep_cupti_context_list = scorep_cupti_context_list->next;

            scorep_cupti_context_finalize( tmp );
            tmp = NULL;
        }

        scorep_cupti_finalized = true;

        SCOREP_MutexDestroy( &scorep_cupti_mutex );
    }
}

/*
 * Create a Score-P CUPTI stream.
 *
 * @param context Score-P CUPTI context
 * @param streamId ID of the CUDA stream
 *
 * @return pointer to created Score-P CUPTI stream
 */
scorep_cupti_stream*
scorep_cupti_stream_create( scorep_cupti_context* context,
                            uint32_t              streamId )
{
    scorep_cupti_stream* stream = NULL;

    if ( context == NULL )
    {
        UTILS_WARNING( "[CUPTI] Cannot create stream without Score-P CUPTI context" );
        return NULL;
    }

    /* create stream by Score-P CUPTI callbacks implementation (CUstream is given) */
    if ( streamId == SCOREP_CUPTI_NO_STREAM_ID )
    {
        UTILS_WARNING( "[CUPTI] No CUDA stream ID given!" );
        return NULL;
    }

    stream = ( scorep_cupti_stream* )SCOREP_Memory_AllocForMisc(
        sizeof( scorep_cupti_stream ) );

    stream->scorep_last_timestamp = SCOREP_GetBeginEpoch();
    stream->stream_id             = streamId;
    stream->next                  = NULL;

    /* create Score-P thread */
    {
        char thread_name[ 16 ] = "CUDA";

        if ( context->device_id == SCOREP_CUPTI_NO_DEVICE_ID )
        {
            if ( -1 == snprintf( thread_name + 4, 12, "[?:%d]", streamId ) )
            {
                UTILS_WARNING( "[CUPTI] Could not create thread name for CUDA thread!" );
            }
        }
        else
        {
            if ( -1 == snprintf( thread_name + 4, 12, "[%d:%d]", context->device_id, streamId ) )
            {
                UTILS_WARNING( "[CUPTI] Could not create thread name for CUDA thread!" );
            }
        }

        stream->scorep_location =
            SCOREP_Location_CreateNonCPULocation( context->scorep_host_location,
                                                  SCOREP_LOCATION_TYPE_GPU, thread_name );

#if defined( SCOREP_CUPTI_ACTIVITY )
        if ( context->activity && ( stream->stream_id == context->activity->default_strm_id ) )
        {
            /* add a location property marking CUDA NULL streams */
            SCOREP_Location_AddLocationProperty( stream->scorep_location,
                                                 SCOREP_CUPTI_LOCATION_NULL_STREAM,
                                                 "yes" );
        }
#endif

        stream->location_id = SCOREP_CUPTI_NO_ID;
    }

    /* for the first stream created for this context */
    if ( context->streams == NULL )
    {
        if ( scorep_cuda_record_idle )
        {
            /* write enter event for GPU_IDLE on first stream */
            SCOREP_Location_EnterRegion( stream->scorep_location,
                                         stream->scorep_last_timestamp,
                                         scorep_cupti_idle_region_handle );

            if ( context->activity != NULL )
            {
                context->activity->gpu_idle = 1;
            }
        }

        /* set the counter value for cudaMalloc to 0 on first stream */
        if ( scorep_cuda_record_gpumemusage )
        {
            SCOREP_Location_TriggerCounterUint64( stream->scorep_location,
                                                  stream->scorep_last_timestamp,
                                                  scorep_cupti_sampling_set_gpumemusage, 0 );
        }
    }

    if ( scorep_cuda_record_kernels == SCOREP_CUDA_KERNEL_AND_COUNTER )
    {
        /* set count values to zero */
        SCOREP_Location_TriggerCounterUint64( stream->scorep_location,
                                              stream->scorep_last_timestamp, scorep_cupti_sampling_set_blocks_per_grid, 0 );
        SCOREP_Location_TriggerCounterUint64( stream->scorep_location,
                                              stream->scorep_last_timestamp, scorep_cupti_sampling_set_threads_per_block, 0 );
        SCOREP_Location_TriggerCounterUint64( stream->scorep_location,
                                              stream->scorep_last_timestamp, scorep_cupti_sampling_set_threads_per_kernel, 0 );

        if ( scorep_cupti_sampling_set_static_shared_mem != SCOREP_INVALID_SAMPLING_SET )
        {
            SCOREP_Location_TriggerCounterUint64( stream->scorep_location,
                                                  stream->scorep_last_timestamp, scorep_cupti_sampling_set_static_shared_mem, 0 );
        }

        if ( scorep_cupti_sampling_set_dynamic_shared_mem != SCOREP_INVALID_SAMPLING_SET )
        {
            SCOREP_Location_TriggerCounterUint64( stream->scorep_location,
                                                  stream->scorep_last_timestamp, scorep_cupti_sampling_set_dynamic_shared_mem, 0 );
        }

        if ( scorep_cupti_sampling_set_local_mem_total != SCOREP_INVALID_SAMPLING_SET )
        {
            SCOREP_Location_TriggerCounterUint64( stream->scorep_location,
                                                  stream->scorep_last_timestamp, scorep_cupti_sampling_set_local_mem_total, 0 );
        }

        if ( scorep_cupti_sampling_set_registers_per_thread != SCOREP_INVALID_SAMPLING_SET )
        {
            SCOREP_Location_TriggerCounterUint64( stream->scorep_location,
                                                  stream->scorep_last_timestamp, scorep_cupti_sampling_set_registers_per_thread, 0 );
        }
    }

    return stream;
}

/*
 * Retrieve a Score-P CUPTI stream object. This function will lookup if
 * the stream is already available or if it has to be
 * created and will return the Score-P CUPTI stream object.
 *
 * Note: This function must be locked with a Score-P CUPTI lock
 *
 * @param context Score-P CUPTI Activity context
 * @param streamId the CUDA stream ID provided by CUPTI callback API
 *
 * @return the Score-P CUPTI stream
 */
scorep_cupti_stream*
scorep_cupti_stream_get_create( scorep_cupti_context* context, uint32_t streamId )
{
    scorep_cupti_stream* stream      = NULL;
    scorep_cupti_stream* last_stream = NULL;

    if ( context == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                     "[CUPTI] Stream creation: No context given!" );
        return NULL;
    }

    if ( streamId == SCOREP_CUPTI_NO_STREAM_ID )
    {
        UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                     "[CUPTI] No stream information given!" );
        return NULL;
    }

    /*** lookup stream ***/
    stream      = context->streams;
    last_stream = context->streams;
    while ( stream != NULL )
    {
        /* check for existing stream */
        if ( streamId != SCOREP_CUPTI_NO_STREAM_ID && stream->stream_id == streamId )
        {
            return stream;
        }

        /* remember last stream to append new created stream later */
        last_stream = stream;

        /* check next stream */
        stream = stream->next;
    }

    /*
     * If stream list is empty, the stream to be created is not the default
     * stream and GPU idle and memory copy tracing is enabled, then create
     * a default stream.
     * This is necessary to preserve increasing event time stamps!
     */
    if ( context->streams == NULL && context->activity != NULL &&
         streamId != context->activity->default_strm_id &&
         scorep_cuda_record_idle && scorep_cuda_record_memcpy )
    {
        context->streams = scorep_cupti_stream_create( context,
                                                       context->activity->default_strm_id );
        last_stream = context->streams;
    }

    /* create the stream, which has not been created yet */
    stream = scorep_cupti_stream_create( context, streamId );

    /* append the newly created stream */
    if ( NULL != last_stream )
    {
        last_stream->next = stream;
    }
    else
    {
        context->streams = stream;
    }

    return stream;
}

/*
 * Get a Score-P CUPTI stream by CUDA stream without locking.
 *
 * @param context pointer to the Score-P CUPTI context, containing the stream
 * @param streamId the CUPTI stream ID
 *
 * @return Score-P CUPTI stream
 */
scorep_cupti_stream*
scorep_cupti_stream_get_by_id( scorep_cupti_context* context,
                               uint32_t              streamId )
{
    scorep_cupti_stream* stream = NULL;

    stream = context->streams;
    while ( stream != NULL )
    {
        if ( stream->stream_id == streamId )
        {
            return stream;
        }
        stream = stream->next;
    }

    return NULL;
}

/*
 * Create a Score-P CUPTI context. If the CUDA context is not given, the
 * current context will be requested and used.
 *
 * @param cudaContext CUDA context
 * @param cudaDevice CUDA device
 * @param contextId ID of the CUDA context
 * @param deviceId ID of the CUDA device
 *
 * @return pointer to created Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_create( CUcontext cudaContext, CUdevice cudaDevice,
                             uint32_t contextId, uint32_t deviceId )
{
    scorep_cupti_context* context = NULL;

    /* create new context */
    context = ( scorep_cupti_context* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cupti_context ) );

    context->context_id           = contextId;
    context->gpu_memory_allocated = 0;
    context->cuda_mallocs         = NULL;
    context->free_cuda_mallocs    = NULL;
    context->streams              = NULL;
    context->next                 = NULL;

    context->scorep_host_location = SCOREP_Location_GetCurrentCPULocation();
    context->location_id          = SCOREP_CUPTI_NO_ID;

    SCOREP_SUSPEND_CUDRV_CALLBACKS();

    /* try to get CUDA device (ID), if they are not given */
    if ( deviceId == SCOREP_CUPTI_NO_DEVICE_ID )
    {
        if ( cudaDevice == SCOREP_CUPTI_NO_DEVICE )
        {
            CUcontext cuCurrCtx;

            if ( cudaContext != NULL )
            {
                SCOREP_CUDA_DRIVER_CALL( cuCtxGetCurrent( &cuCurrCtx ) );

                /* if given context does not match the current one, get the device for
                   the given one */
                if ( cudaContext != cuCurrCtx )
                {
                    SCOREP_CUDA_DRIVER_CALL( cuCtxSetCurrent( cudaContext ) );
                }
            }

            if ( CUDA_SUCCESS == cuCtxGetDevice( &cudaDevice ) )
            {
                deviceId = ( uint32_t )cudaDevice;
            }

            /* reset the active context */
            if ( cudaContext != NULL && cudaContext != cuCurrCtx )
            {
                SCOREP_CUDA_DRIVER_CALL( cuCtxSetCurrent( cuCurrCtx ) );
            }
        }
        else
        {
            /* no device ID, but CUDA device is given */
            deviceId = ( uint32_t )cudaDevice;
        }
    }

    context->device_id   = deviceId;
    context->cuda_device = cudaDevice;

    /* get the current CUDA context, if it is not given */
    if ( cudaContext == NULL )
    {
        SCOREP_CUDA_DRIVER_CALL( cuCtxGetCurrent( &cudaContext ) );
    }

    /* set the CUDA context */
    context->cuda_context = cudaContext;

    // create window on every CPU location, where communication might occur
    if ( scorep_cuda_record_memcpy )
    {
        // ensure to not create the RMA window twice on the same host location
        scorep_cuda_location_data* loc_data =
            SCOREP_Location_GetSubsystemData( SCOREP_Location_GetCurrentCPULocation(),
                                              scorep_cuda_subsystem_id );

        if ( !loc_data->rma_window_active )
        {
            SCOREP_RmaWinCreate( scorep_cuda_window_handle );
            loc_data->rma_window_active = true;
        }
    }

    context->activity = NULL;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                        "[CUPTI] Created context for CUcontext %p, CUdevice %d",
                        cudaContext, cudaDevice );

    SCOREP_RESUME_CUDRV_CALLBACKS();

    return context;
}

/*
 * Get a Score-P CUPTI context by CUDA context
 *
 * @param cudaContext the CUDA context
 *
 * @return Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_get( CUcontext cudaContext )
{
    scorep_cupti_context* context = NULL;

    /* since CUDA 6.0, we also have to test against the CUDA context ID */
#if ( defined( CUDA_VERSION ) && ( CUDA_VERSION >= 6000 ) )
    uint32_t cudaContextId = SCOREP_CUPTI_NO_CONTEXT_ID;
    SCOREP_CUPTI_CALL( cuptiGetContextId( cudaContext, &cudaContextId ) );
#endif


    /* lookup context */
    context = scorep_cupti_context_list;
    while ( context != NULL )
    {
        if ( context->cuda_context == cudaContext )
        {
#if ( defined( CUDA_VERSION ) && ( CUDA_VERSION >= 6000 ) )
            if ( context->context_id == cudaContextId )
#endif
            return context;
        }

        context = context->next;
    }

    return NULL;
}

/*
 * Get a Score-P CUPTI context by CUDA context ID
 *
 * @param contextId the CUDA context ID
 *
 * @return Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_get_by_id( uint32_t contextId )
{
    scorep_cupti_context* context = NULL;

    /* lookup context */
    context = scorep_cupti_context_list;
    while ( context != NULL )
    {
        if ( context->context_id == contextId )
        {
            return context;
        }

        context = context->next;
    }

    return NULL;
}


/*
 * Get or if not available create a Score-P CUPTI context by CUDA context.
 *
 * @param cudaContext the CUDA context
 *
 * @return Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_get_create( CUcontext cudaContext )
{
    /* try to find CUPTI context without locking */
    scorep_cupti_context* context = scorep_cupti_context_get( cudaContext );

    if ( context == NULL )
    {
        /* securely insert context into global context list */
        SCOREP_CUPTI_LOCK();
        context = scorep_cupti_context_get( cudaContext );

        if ( context == NULL )
        {
            uint32_t context_id = SCOREP_CUPTI_NO_CONTEXT_ID;
#if ( defined( CUDA_VERSION ) && ( CUDA_VERSION >= 6000 ) )
            SCOREP_CUPTI_CALL( cuptiGetContextId( cudaContext, &context_id ) );
#endif
            context = scorep_cupti_context_create( cudaContext, SCOREP_CUPTI_NO_DEVICE,
                                                   context_id, SCOREP_CUPTI_NO_DEVICE_ID );

            /* prepend context to global context list */
            context->next             = scorep_cupti_context_list;
            scorep_cupti_context_list = context;
        }
        SCOREP_CUPTI_UNLOCK();
    }

    return context;
}

/*
 * Remove a context from the global context list and return it.
 *
 * @param cudaContext pointer to the CUDA context
 * @return the Score-P CUPTI context, which has been removed
 */
scorep_cupti_context*
scorep_cupti_context_remove( CUcontext cudaContext )
{
    scorep_cupti_context* currCtx = NULL;
    scorep_cupti_context* lastCtx = NULL;

    SCOREP_CUPTI_LOCK();
    currCtx = scorep_cupti_context_list;
    lastCtx = scorep_cupti_context_list;
    while ( currCtx != NULL )
    {
        if ( currCtx->cuda_context == cudaContext )
        {
            /* if first element in list */
            if ( currCtx == scorep_cupti_context_list )
            {
                scorep_cupti_context_list = scorep_cupti_context_list->next;
            }
            else
            {
                lastCtx->next = currCtx->next;
            }
            SCOREP_CUPTI_UNLOCK();
            return currCtx;
        }
        lastCtx = currCtx;
        currCtx = currCtx->next;
    }
    SCOREP_CUPTI_UNLOCK();

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                        "[CUPTI] Could not remove context (CUDA Context not found)!" );

    return NULL;
}

/*
 * Finalize the Score-P CUPTI context and free all memory allocated with it.
 *
 * @param scorepCtx pointer to the Score-P CUPTI context
 */
void
scorep_cupti_context_finalize( scorep_cupti_context* context )
{
    if ( context == NULL )
    {
        return;
    }

    /* write exit event for GPU idle time */
    if ( scorep_cuda_record_idle && context->streams != NULL
         && context->activity != NULL && context->activity->gpu_idle == 1 )
    {
        uint64_t idle_end = SCOREP_Timer_GetClockTicks();

        SCOREP_Location_ExitRegion( context->streams->scorep_location, idle_end,
                                    scorep_cupti_idle_region_handle );
    }

    /* cleanup stream list */
    /* free is implicitly done by Score-P memory management */
    /* currently there are only RMA windows destroyed */
    if ( scorep_cuda_record_memcpy )
    {
        while ( context->streams != NULL )
        {
            scorep_cupti_stream* stream = context->streams;

            if ( SCOREP_CUPTI_NO_ID != stream->location_id )
            {
                /* destroy window on every location, where it is used */
                SCOREP_Location_RmaWinDestroy( stream->scorep_location,
                                               SCOREP_Timer_GetClockTicks(),
                                               scorep_cuda_window_handle );
            }

            context->streams = context->streams->next;

            /*free(scorepStrm);*/
            stream = NULL;
        }
    }
    context->streams = NULL;

    /* free CUDA malloc entries, if user application has memory leaks */
    while ( context->cuda_mallocs != NULL )
    {
        scorep_cupti_gpumem* scorepMem =  context->cuda_mallocs;

        if ( scorep_cuda_record_gpumemusage == SCOREP_CUDA_GPUMEMUSAGE_AND_MISSING_FREES )
        {
            UTILS_WARNING( "[CUPTI] Free of %d bytes GPU memory missing!", scorepMem->size );
        }

        context->cuda_mallocs = scorepMem->next;
        /* free is implicitly done by Score-P memory management */
        /*free(scorepMem);*/
        scorepMem = NULL;
    }

    // destroy the RMA window on the host
    if ( scorep_cuda_record_memcpy )
    {
        // ensure to not destroy the RMA window twice on the same host location
        scorep_cuda_location_data* loc_data =
            SCOREP_Location_GetSubsystemData( SCOREP_Location_GetCurrentCPULocation(),
                                              scorep_cuda_subsystem_id );

        if ( loc_data->rma_window_active )
        {
            SCOREP_RmaWinDestroy( scorep_cuda_window_handle );
            loc_data->rma_window_active = false;
        }
    }

    if ( context->activity != NULL )
    {
        /* free is implicitly done by Score-P memory management */
        /*free(scorepCtx->activity);*/
        context->activity = NULL;
    }

    /* free is implicitly done by Score-P memory management */
    /*free(scorepCtx);*/
    context = NULL;
}


size_t
scorep_cupti_create_cuda_comm_group( uint64_t** globalLocationIds )
{
    size_t                count   = 0;
    scorep_cupti_context* context = NULL;

    /* get the number of CUDA communication partners */
    context = scorep_cupti_context_list;
    while ( context != NULL )
    {
        scorep_cupti_stream* stream = context->streams;

        while ( NULL != stream )
        {
            if ( SCOREP_CUPTI_NO_ID != stream->location_id )
            {
                count++;
            }

            stream = stream->next;
        }

        /* get an array element for the context location */
        if ( scorep_cuda_record_memcpy )
        {
            count++;
        }

        context = context->next;
    }

    if ( count == 0 )
    {
        return count;
    }

    /* allocate the CUDA communication group array */
    *globalLocationIds = ( uint64_t* )SCOREP_Memory_AllocForMisc( count * sizeof( uint64_t ) );

    /* add the communication partners allocated array */
    context = scorep_cupti_context_list;
    while ( context != NULL )
    {
        scorep_cupti_stream* stream = context->streams;

        while ( NULL != stream )
        {
            if ( SCOREP_CUPTI_NO_ID != stream->location_id )
            {
                ( *globalLocationIds )[ stream->location_id ] =
                    SCOREP_Location_GetGlobalId( stream->scorep_location );
            }

            stream = stream->next;
        }

        /* add the context location */
        if ( SCOREP_CUPTI_NO_ID != context->location_id )
        {
            ( *globalLocationIds )[ context->location_id ] =
                SCOREP_Location_GetGlobalId( context->scorep_host_location );
        }

        context = context->next;
    }

    return count;
}


/** @brief
 * SDBM hash function. (better than DJB2 for table size 2^10)
 *
 * @param str               Pointer to a string to be hashed
 *
 * @return Returns hash code of @ str.
 */
static unsigned int
hash_string( const char* str )
{
    unsigned int hash = 0;
    int          c;

    while ( ( c = *str++ ) )
    {
        hash = c + ( hash << 6 ) + ( hash << 16 ) - hash;
    }

    return hash;
}

/** @brief
 * Puts a string into the CUPTI activity hash table
 *
 * @param name              Pointer to a string to be stored in the hash table.
 * @param region            Region handle.
 *
 * @return Return pointer to the created hash node.
 */
void*
scorep_cupti_kernel_hash_put( const char*         name,
                              SCOREP_RegionHandle region )
{
    uint32_t id = ( uint32_t )hash_string( name ) % SCOREP_CUDA_KERNEL_HASHTABLE_SIZE;

    scorep_cuda_kernel_hash_node* add =
        ( scorep_cuda_kernel_hash_node* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cuda_kernel_hash_node ) );

    add->name                        = UTILS_CStr_dup( name );           /* does an implicit malloc */
    add->region                      = region;
    add->next                        = scorep_cuda_kernel_hashtab[ id ];
    scorep_cuda_kernel_hashtab[ id ] = add;

    return add;
}

/** @brief
 * Get a string from the CUPTI Activity hash table
 *
 * @param name              Pointer to a string to be retrieved from the hash table.
 *
 * @return Return pointer to the retrieved hash node.
 */
void*
scorep_cupti_kernel_hash_get( const char* name )
{
    uint32_t id = ( uint32_t )hash_string( name )  % SCOREP_CUDA_KERNEL_HASHTABLE_SIZE;

    scorep_cuda_kernel_hash_node* curr = scorep_cuda_kernel_hashtab[ id ];

    while ( curr )
    {
        if ( strcmp( curr->name, name ) == 0 )
        {
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}
