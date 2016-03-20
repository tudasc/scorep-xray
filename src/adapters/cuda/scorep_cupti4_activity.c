/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
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
 *  Implementation of CUPTI activity asynchronous buffer handling
 *  (CUPTI_VERSION >= 4).
 */

#include <config.h>

#include <UTILS_Error.h>

#include <SCOREP_InMeasurement.h>

#include "scorep_cuda.h"
#include "scorep_cupti_activity.h"
#include "scorep_cupti_activity_internal.h"
#include "scorep_cupti_callbacks.h"

/* static/internal function declarations */
static bool
is_kernel_record_valid( CUpti_ActivityKernelType* kernel );

static void
handle_buffer( uint8_t*              buffer,
               size_t                validSize,
               scorep_cupti_context* context,
               uint32_t              streamId );

static scorep_cupti_buffer*
get_free_buffer( scorep_cupti_context* context );

static scorep_cupti_context*
mark_complete_buffer( uint8_t*  buffer,
                      size_t    validSize,
                      CUcontext cudaContext,
                      uint32_t  streamId );

static void
buffer_requested_callback( uint8_t** buffer,
                           size_t*   size,
                           size_t*   maxNumRecords );

static void
buffer_completed_callback( CUcontext cudaContext,
                           uint32_t  streamId,
                           uint8_t*  buffer,
                           size_t    size,
                           size_t    validSize );

/*****************************************/

void
scorep_cupti_activity_check_chunk_size( void )
{
    if ( scorep_cupti_activity_buffer_chunk_size < 1024 )
    {
        if ( scorep_cupti_activity_buffer_chunk_size > 0 )
        {
            UTILS_WARNING( "[CUPTI Activity] Stream buffer size has to be at least 1024 "
                           "bytes! It has been set to %d bytes.", scorep_cupti_activity_buffer_chunk_size );
        }

        /* set it to the default size 8k (see scorep_cuda_confvars.inc.c) */
        scorep_cupti_activity_buffer_chunk_size = 8 * 1024;
    }

    if ( scorep_cupti_activity_buffer_chunk_size > scorep_cupti_activity_buffer_size )
    {
        UTILS_WARNING( "[CUPTI Activity] Context buffer size cannot be smaller than "
                       "stream buffer size. It has been set to %d bytes.", scorep_cupti_activity_buffer_size );
        scorep_cupti_activity_buffer_size = scorep_cupti_activity_buffer_chunk_size;
    }
}

void
scorep_cupti_activity_context_setup( scorep_cupti_context* context )
{
    /* try to get the global Score-P CUPTI context */
    if ( context == NULL )
    {
        UTILS_WARNING( "[CUPTI Activity] No context given. "
                       "Cannot setup activity context!" );
        return;
    }

    /* create the Score-P CUPTI activity context */
    if ( context->activity == NULL )
    {
        context->activity = scorep_cupti_activity_context_create( context->cuda_context );
    }
}

bool
scorep_cupti_activity_is_buffer_empty( scorep_cupti_context* context )
{
    if ( context->activity && context->activity->buffers &&
         ( context->activity->buffers->committed ||
           context->activity->buffers->pending ) )
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*
 * Handle CUPTI activity buffers, which are completed.
 * !!! Has to be locked with Score-P CUPTI lock !!!
 */
void
scorep_cupti_activity_context_flush( scorep_cupti_context* context )
{
    scorep_cupti_buffer* current           = NULL;
    uint8_t              buffers_completed = 1;

    /* check for Score-P CUPTI context */
    if ( context == NULL || context->activity == NULL )
    {
        UTILS_WARNING( "[CUPTI Activity] Context not found! Cannot flush context ..." );
        return;
    }

#if ( defined( CUDA_VERSION ) && ( CUDA_VERSION >= 6000 ) )
    SCOREP_CUPTI_CALL( cuptiActivityFlushAll( 0 ) );
#else
    SCOREP_CUPTI_CALL( cuptiActivityFlush( context->cuda_context, 0, 0 ) );
#endif

    SCOREP_CUPTI_LOCK();
    {
        uint64_t               hostStop, gpuStop;
        scorep_cupti_activity* context_activity = NULL;

        context_activity = context->activity;

        /*
         * Get time synchronization factor between host and GPU time for measured
         * period
         */
        {
            double gpu_diff = 0;

            scorep_cupti_set_synchronization_point( &( context_activity->sync.gpu_stop ),
                                                    &( context_activity->sync.host_stop ) );

            /* save these as next synchronization point */
            gpuStop  = context_activity->sync.gpu_stop;
            hostStop = context_activity->sync.host_stop;

            gpu_diff = ( double )( gpuStop - context_activity->sync.gpu_start );

            if ( gpu_diff == ( double )0 )
            {
                UTILS_WARNING( "[CUPTI Activity] GPU time difference is 0! Cannot flush." );
                SCOREP_CUPTI_UNLOCK();
                return;
            }

            context_activity->sync.factor =
                ( double )( hostStop - context_activity->sync.host_start ) / gpu_diff;
        }

        /* expose Score-P CUPTI activity flush as measurement overhead */
        SCOREP_EnterRegion( scorep_cupti_buffer_flush_region_handle );

        /* handle all completed buffers for this context */
        current = context_activity->buffers;
        while ( current )
        {
            if ( current->committed )
            {
                buffers_completed = 0;
            }
            else if ( current->pending )
            {
                handle_buffer( current->buffer, current->valid_size,
                               context, current->stream_id );

                /* mark buffer as free */
                current->pending                           = false;
                current->valid_size                        = 0;
                context_activity->max_buffer_size_exceeded = false;
            }

            current = current->next;
        }

        /* report any dropped records */
        {
            size_t dropped = 0;

            SCOREP_CUPTI_CALL( cuptiActivityGetNumDroppedRecords( context->cuda_context, 0, &dropped ) );
            if ( dropped != 0 )
            {
                UTILS_WARNING( "[CUPTI Activity] Dropped %u records. Current buffer size: %llu bytes\n"
                               "To avoid dropping of records increase the buffer size!\n"
                               "Proposed minimum SCOREP_CUDA_BUFFER=%llu",
                               ( unsigned int )dropped, scorep_cupti_activity_buffer_size,
                               scorep_cupti_activity_buffer_size + dropped / 2 *
                               ( sizeof( CUpti_ActivityKernelType ) + sizeof( CUpti_ActivityMemcpy ) ) );
            }
        }

        /* enter GPU idle region after last kernel, if exited before */
        if ( context_activity->gpu_idle == false )
        {
            SCOREP_Location_EnterRegion( context->streams->scorep_location,
                                         ( context_activity->scorep_last_gpu_time ), scorep_cupti_idle_region_handle );
            context_activity->gpu_idle = true;
        }

        /* set new synchronization point, if no buffers are currently controlled by
           CUPTI */
        if ( buffers_completed )
        {
            context_activity->sync.host_start = hostStop;
            context_activity->sync.gpu_start  = gpuStop;
        }

        /* write exit event for activity flush */
        SCOREP_ExitRegion( scorep_cupti_buffer_flush_region_handle );
    }
    SCOREP_CUPTI_UNLOCK();
}

void
scorep_cupti_activity_context_finalize( scorep_cupti_context* context )
{
    if ( !context || ( !context->activity ) || ( !context->activity->buffers ) )
    {
        return;
    }

    {
        scorep_cupti_buffer* bfr = NULL;

        bfr = context->activity->buffers;
        while ( bfr )
        {
            scorep_cupti_buffer* next = bfr->next;

            if ( bfr->committed || bfr->pending )
            {
                UTILS_WARNING( "[CUPTI Activity] Destroying buffer which is currently in use (%d, %u, %u)",
                               context->cuda_context, bfr->committed, bfr->pending );
            }

            free( bfr->buffer );
            free( bfr );

            bfr = next;
        }
        context->activity->buffers = NULL;
    }

    /* do not free the activity context itself, as gpuIdleOn is needed later */
}

void
scorep_cupti_activity_register_callbacks( void )
{
    /* opt-in for CUPTI activity callbacks */
    SCOREP_CUPTI_CALL( cuptiActivityRegisterCallbacks(
                           buffer_requested_callback,
                           buffer_completed_callback ) );
}

/*
 * Select record type and call respective function.
 *
 * @param record the basic CUPTI activity record
 * @param context the Score-P CUPTI activity context
 */
static void
write_cupti_activity_record( CUpti_Activity*       record,
                             scorep_cupti_context* context )
{
    switch ( record->kind )
    {
        case CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL:
            if ( !is_kernel_record_valid( ( CUpti_ActivityKernelType* )record ) )
            {
                break;
            }
        case CUPTI_ACTIVITY_KIND_KERNEL:
            scorep_cupti_activity_write_kernel( ( CUpti_ActivityKernelType* )record, context );
            break;

        case CUPTI_ACTIVITY_KIND_MEMCPY:
        {
            scorep_cupti_activity_write_memcpy( ( CUpti_ActivityMemcpy* )record, context );
            break;
        }
        default:
        {
            break;
        }
    }
}

/*
 * Check for invalid concurrent kernel records. Since CUPTI 4 it is possible
 * that concurrent kernel records have a start or a stop time stamp with a
 * 0 value and are therefore invalid.
 *  see cupti_activity.h (since CUPTI 4):
 *  "Records of kind CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL may contain
 *  invalid (0) timestamps, indicating that no timing information could be
 *  collected for lack of device memory."
 *
 * @param kernel the concurrent kernel record
 *
 * @return true if kernel is valid, otherwise false.
 *
 */
static bool
is_kernel_record_valid( CUpti_ActivityKernelType* kernel )
{
    if ( kernel->start == 0 || kernel->end == 0 )
    {
        UTILS_WARN_ONCE( "[CUPTI Activity] Skipping kernel '%s': "
                         "No timing information could be collected. Try to add option "
                         "'kernel_serial' to SCOREP_CUDA_ENABLE, but notice that this "
                         "will force sequential execution of CUDA kernels.", kernel->name );

        return false;
    }
    else
    {
        return true;
    }
}

static void
buffer_requested_callback( uint8_t** buffer,
                           size_t*   size,
                           size_t*   maxNumRecords )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    scorep_cupti_buffer*  free_buffer  = NULL;
    scorep_cupti_context* context      = NULL;
    CUcontext             cuda_context = NULL;

    SCOREP_SUSPEND_CUDRV_CALLBACKS();
    SCOREP_CUDA_DRIVER_CALL( cuCtxGetCurrent( &cuda_context ) );
    SCOREP_RESUME_CUDRV_CALLBACKS();
    context = scorep_cupti_context_get( cuda_context );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                        "[CUPTI Activity] Buffer for context %p requested", cuda_context );

    if ( context )
    {
        free_buffer = get_free_buffer( context );
    }
    else
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                            "[CUPTI Activity] No Score-P context for CUDA context %p found.",
                            cuda_context );
    }

    if ( free_buffer )
    {
        *buffer = free_buffer->buffer;
        *size   = free_buffer->size;
    }
    else
    {
        *buffer = NULL;
        *size   = 0;
    }
    /* allow as many records as possible */
    *maxNumRecords = 0;

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

static void
buffer_completed_callback( CUcontext cudaContext,
                           uint32_t  streamId,
                           uint8_t*  buffer,
                           size_t    size,
                           size_t    validSize )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    scorep_cupti_context* scorep_ctx = mark_complete_buffer( buffer, validSize, cudaContext, streamId );
    if ( scorep_ctx )
    {
        cudaContext = scorep_ctx->cuda_context;
    }
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                        "[CUPTI Activity] Buffer with %zu bytes for context %p, stream %d completed",
                        validSize, cudaContext, streamId );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

static void
handle_buffer( uint8_t*              buffer,
               size_t                validSize,
               scorep_cupti_context* context,
               uint32_t              streamId )
{
    CUptiResult     status = CUPTI_SUCCESS;
    CUpti_Activity* record = NULL;

    if ( validSize > 0 )
    {
        /* check for Score-P CUPTI context */
        if ( context == NULL || context->activity == NULL )
        {
            UTILS_WARNING( "[CUPTI Activity] Context not found! Cannot handle activity buffer ..." );
            return;
        }

        if ( context->cuda_context == 0 )
        {
            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                                "[CUPTI Activity] Handle activities from global queue" );
        }
        else
        {
            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                                "[CUPTI Activity] Handle activities from context %p, stream %d",
                                context->cuda_context, streamId );
        }

        do
        {
            status = cuptiActivityGetNextRecord( buffer, validSize, &record );
            if ( status == CUPTI_SUCCESS )
            {
                write_cupti_activity_record( record, context );
            }
            else
            {
                if ( status != CUPTI_ERROR_MAX_LIMIT_REACHED )
                {
                    SCOREP_CUPTI_CALL( status );
                }
                break;
            }
        }
        while ( 1 );
    }
}

/*
 * Get Score-P buffer entry by CUPTI buffer pointer. Searches for all
 * buffers in all contexts.
 *
 * @param buffer CUPTI buffer pointer
 * @return Score-P buffer entry
 */
static scorep_cupti_buffer*
get_buffer( uint8_t* buffer, scorep_cupti_context** scorepCtx )
{
    scorep_cupti_context* context = NULL;

    /*
     * TODO: Check why locks run into a deadlock here, assumption:
     * getBuffer() is indirectly called from
     * bufferCompletedCallback().
     * The context destroy callback triggers scorep_cupti_activity_context_flush()
     * within a SCOREP_CUPTI_LOCK.
     * scorep_cupti_activity_context_flush() calls cuptiActivityFlush(), which
     * probably triggers bufferCompletedCallback().
     */
    /*SCOREP_CUPTI_LOCK();*/
    context = scorep_cupti_context_list;
    while ( context != NULL )
    {
        scorep_cupti_buffer* current = context->activity->buffers;

        while ( current )
        {
            if ( current->buffer == buffer )
            {
                if ( scorepCtx )
                {
                    *scorepCtx = context;
                }
                return current;
            }

            current = current->next;
        }

        context = context->next;
    }
    /*SCOREP_CUPTI_UNLOCK();*/

    return NULL;
}

static scorep_cupti_context*
mark_complete_buffer( uint8_t*  buffer,
                      size_t    validSize,
                      CUcontext cudaContext,
                      uint32_t  streamId )
{
    scorep_cupti_buffer*  buffer_entry = NULL;
    scorep_cupti_context* result       = NULL;

    if ( cudaContext ) /* get Score-P buffer for context queue */
    {
        scorep_cupti_context* context = NULL;

        /* check for Score-P CUPTI context */
        context = scorep_cupti_context_get( cudaContext );
        if ( context == NULL || context->activity == NULL )
        {
            UTILS_WARNING( "[CUPTI Activity] Context not found!" );
            return NULL;
        }

        /* try to find entry for buffer in pool */
        {
            scorep_cupti_buffer* current = context->activity->buffers;
            while ( current )
            {
                if ( current->buffer == buffer )
                {
                    buffer_entry = current;
                    result       = context;
                    break;
                }

                current = current->next;
            }
        }
    }
    else /* get Score-P buffer for global queue */
    {
        buffer_entry = get_buffer( buffer, &result );
    }

    if ( !buffer_entry )
    {
        UTILS_WARNING( "[CUPTI Activity] Could not find CUPTI activity buffer entry! " );
        return NULL;
    }

    /* mark entry to contain completed, pending records*/
    buffer_entry->committed = false;
    if ( validSize > 0 )
    {
        buffer_entry->pending = true;
    }
    buffer_entry->valid_size = validSize;
    buffer_entry->stream_id  = streamId;
    return result;
}

static scorep_cupti_buffer*
get_free_buffer( scorep_cupti_context* context )
{
    scorep_cupti_buffer*   free_buffer    = NULL;
    scorep_cupti_activity* activity       = NULL;
    size_t                 total_bfr_size = 0;
    scorep_cupti_buffer*   prev           = NULL;

    /* check for Score-P CUPTI context */
    if ( context == NULL || context->activity == NULL )
    {
        UTILS_WARNING( "[CUPTI Activity] Context not found! " );
        return NULL;
    }

    activity = context->activity;

    /* try to get a free buffer from the pool */
    if ( activity->buffers )
    {
        scorep_cupti_buffer* current = activity->buffers;
        while ( current )
        {
            prev            = current;
            total_bfr_size += current->size;

            if ( ( !current->committed ) && ( !current->pending ) )
            {
                free_buffer = current;
                break;
            }

            current = current->next;
        }
    }

    if ( !free_buffer )
    {
        /* if none free, allocate new buffer */
        if ( total_bfr_size + scorep_cupti_activity_buffer_chunk_size + SCOREP_CUPTI_ACTIVITY_ALIGN_SIZE
             > scorep_cupti_activity_buffer_size )
        {
            if ( !activity->max_buffer_size_exceeded )
            {
                UTILS_WARNING( "[CUPTI Activity] Reached maximum CUDA buffer size for context %p",
                               context->cuda_context );
                activity->max_buffer_size_exceeded = true;
            }
            return NULL;
        }

        free_buffer = ( scorep_cupti_buffer* )malloc( sizeof( scorep_cupti_buffer ) );
        if ( !free_buffer )
        {
            UTILS_WARNING( "[CUPTI Activity] Could not allocate buffer entry" );
            return NULL;
        }

        free_buffer->buffer = ( uint8_t* )malloc( scorep_cupti_activity_buffer_chunk_size +
                                                  SCOREP_CUPTI_ACTIVITY_ALIGN_SIZE );
        if ( !free_buffer->buffer )
        {
            UTILS_WARNING( "[CUPTI Activity] Could not allocate buffer" );
            free( free_buffer );
            return NULL;
        }

        free_buffer->buffer     = SCOREP_CUPTI_ACTIVITY_ALIGN_BUFFER( free_buffer->buffer );
        free_buffer->pending    = false;
        free_buffer->size       = scorep_cupti_activity_buffer_chunk_size;
        free_buffer->valid_size = 0;

        /* enqueue new buffer at end or as new head entry in pool */
        free_buffer->next = NULL;
        if ( prev )
        {
            prev->next = free_buffer;
        }
        else
        {
            activity->buffers = free_buffer;
        }
    }

    UTILS_ASSERT( free_buffer->valid_size == 0 );

    free_buffer->committed = true;
    free_buffer->stream_id = 0;

    return free_buffer;
}
