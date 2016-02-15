/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  Implementation of CUPTI activity buffer handling for CUPTI 2 and 3.
 */

#include <config.h>
#include <SCOREP_InMeasurement.h>

#include <SCOREP_Timer_Ticks.h>
#include "scorep_cuda.h"
#include "scorep_cupti_activity.h"
#include "scorep_cupti_activity_internal.h"

static void
write_cupti_activity_record( CUpti_Activity*       record,
                             scorep_cupti_context* context );

/*
 * Functions that are not used in in CUPTI prior to version 4.
 */
void
scorep_cupti_activity_check_chunk_size( void )
{
    return;
}

void
scorep_cupti_activity_register_callbacks( void )
{
    return;
}

void
scorep_cupti_activity_context_setup( scorep_cupti_context* context )
{
    /* try to get the global Score-P CUPTI context */
    if ( context == NULL )
    {
        UTILS_WARNING( "[CUPTI3 Activity] No context given. "
                       "Cannot setup activity context!" );
        return;
    }

    /* create the Score-P CUPTI activity context */
    if ( context->activity == NULL )
    {
        context->activity = scorep_cupti_activity_context_create( context->cuda_context );
    }

    /* queue new buffer to context to record activities */
    if ( context->activity->buffer == NULL )
    {
        context->activity->buffer =
            ( uint8_t* )malloc( scorep_cupti_activity_buffer_size );
    }

    SCOREP_CUPTI_CALL( cuptiActivityEnqueueBuffer( context->cuda_context, 0,
                                                   SCOREP_CUPTI_ACTIVITY_ALIGN_BUFFER( context->activity->buffer ),
                                                   scorep_cupti_activity_buffer_size ) );
}

bool
scorep_cupti_activity_is_buffer_empty( scorep_cupti_context* context )
{
    CUptiResult status = CUPTI_SUCCESS;
    size_t      bsize  = 0;

    status = cuptiActivityQueryBuffer( context->cuda_context, 0, &bsize );
    if ( ( status == CUPTI_SUCCESS && bsize > 0 ) ||
         status == CUPTI_ERROR_MAX_LIMIT_REACHED )
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*
 * Handle activities buffered by CUPTI.
 * !!! Has to be locked with Score-P CUPTI lock !!!
 *
 * @param context the Score-P CUPTI context
 */
void
scorep_cupti_activity_context_flush( scorep_cupti_context* context )
{
    CUptiResult            status = CUPTI_SUCCESS;
    uint8_t*               buffer = NULL;
    size_t                 bufSize;
    CUpti_Activity*        record = NULL;
    uint64_t               hostStop, gpuStop;
    scorep_cupti_activity* context_activity = NULL;

    /* check for Score-P CUPTI context */
    if ( context == NULL || context->activity == NULL )
    {
        UTILS_WARNING( "[CUPTI3 Activity] Context not found! Cannot flush buffer ..." );
        return;
    }
    context_activity = context->activity;

    /* check if the buffer contains records */
    if ( scorep_cupti_activity_is_buffer_empty( context ) )
    {
        return;
    }

    /* expose Score-P CUPTI activity flush as measurement overhead */
    SCOREP_EnterRegion( scorep_cupti_buffer_flush_region_handle );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI3 Activity] Handle context %p activities", context->cuda_context );

    /* dump the contents of the global queue */
    SCOREP_CUPTI_CALL( cuptiActivityDequeueBuffer( context->cuda_context, 0, &buffer,
                                                   &bufSize ) );

    /*
     * Get time synchronization factor between host and GPU time for measured
     * period
     */
    double gpu_diff = 0;

    SCOREP_CUPTI_CALL( cuptiGetTimestamp( &gpuStop ) );
    hostStop                         = SCOREP_Timer_GetClockTicks();
    context_activity->sync.host_stop = hostStop;
    scorep_cupti_set_synchronization_point( &( context_activity->sync.gpu_stop ),
                                            &( context_activity->sync.host_stop ) );

    /* save these as next synchronization point */
    gpuStop  = context_activity->sync.gpu_stop;
    hostStop = context_activity->sync.host_stop;

    gpu_diff = ( double )( gpuStop - context_activity->sync.gpu_start );

    if ( gpu_diff == ( double )0 )
    {
        UTILS_WARNING( "[CUPTI3 Activity] GPU time difference is 0! Cannot flush." );
        return;
    }

    context_activity->sync.factor =
        ( double )( hostStop - context_activity->sync.host_start ) / gpu_diff;

    /* iterate over CUPTI records */
    do
    {
        status = cuptiActivityGetNextRecord( buffer, bufSize, &record );
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

    /* report any records dropped from the global queue */
    {
        size_t dropped;

        SCOREP_CUPTI_CALL( cuptiActivityGetNumDroppedRecords( context->cuda_context, 0, &dropped ) );
        if ( dropped != 0 )
        {
            UTILS_WARNING( "[CUPTI3 Activity] Dropped %u records. Current buffer size: %llu bytes\n"
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

    /* enqueue buffer again */
    SCOREP_CUPTI_CALL( cuptiActivityEnqueueBuffer( context->cuda_context, 0, buffer,
                                                   scorep_cupti_activity_buffer_size ) );


    /* set new synchronization point */
    context_activity->sync.host_start = hostStop;
    context_activity->sync.gpu_start  = gpuStop;

    /* write exit event for activity flush */
    SCOREP_ExitRegion( scorep_cupti_buffer_flush_region_handle );
}

/*
 * Finalize a Score-P CUPTI Activity context.
 *
 * @param context Score-P CUPTI context
 */
void
scorep_cupti_activity_context_finalize( scorep_cupti_context* context )
{
    scorep_cupti_activity* context_activity = NULL;

    if ( !context || ( !context->activity ) )
    {
        return;
    }

    context_activity = context->activity;

    /* CUPTI buffer flush is only allowed in cudaDeviceReset and
       cudaDeviceSynchronize */
    /*scorep_cupti_activity_context_flush( context );*/

    /* free activity buffer */
    if ( context_activity->buffer != NULL )
    {
        free( context_activity->buffer );
        context_activity->buffer = NULL;
    }

    /* do not free the activity context itself, as gpuIdleOn is needed later */
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
        /* Concurrent kernel records are available since CUPTI 3 */
#if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
        case CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL:
#endif
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
