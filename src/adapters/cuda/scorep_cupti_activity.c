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
 *  Implementation of asynchronous CUDA activity recording.
 */

#include <config.h>

#include <SCOREP_Config.h>
#include <SCOREP_Filtering.h>
#include <SCOREP_Timer_Ticks.h>

#include <UTILS_CStr.h>

#include "scorep_cuda.h"
#include "scorep_cupti_activity.h"
#include "scorep_cupti_activity_internal.h"
#include "scorep_cupti_callbacks.h"

#include <inttypes.h>

/* the default size for the CUDA kernel name hash table */
#define SCOREP_CUPTI_ACTIVITY_HASHTABLE_SIZE 1024

/*
 * The key of the hash node is a string, the value the corresponding region handle.
 * It is used to store region names with its corresponding region handles.
 */
typedef struct scorep_cupti_activity_hash_node_string_struct
{
    char*                                                 name;   /**< name of the symbol */
    SCOREP_RegionHandle                                   region; /**< associated region handle */
    struct scorep_cupti_activity_hash_node_string_struct* next;   /**< bucket for collision */
} scorep_cupti_activity_hash_node_string;

/* device/host communication directions */
typedef enum
{
    SCOREP_CUPTI_DEV2HOST              = 0x00, /* device to host copy */
    SCOREP_CUPTI_HOST2DEV              = 0x01, /* host to device copy */
    SCOREP_CUPTI_DEV2DEV               = 0x02, /* device to device copy */
    SCOREP_CUPTI_HOST2HOST             = 0x04, /* host to host copy */
    SCOREP_CUPTI_COPYDIRECTION_UNKNOWN = 0x08  /* unknown */
} scorep_cupti_activity_memcpy_kind;

/* initialization and finalization flags */
static bool scorep_cupti_activity_initialized               = false;
static bool scorep_cupti_activity_finalized                 = false;

/* enable state of individual CUPTI activity types (zero is disabled) */
uint8_t scorep_cupti_activity_state                         = 0;

/* global region IDs for wrapper internal recording */
SCOREP_RegionHandle scorep_cupti_buffer_flush_region_handle = SCOREP_INVALID_REGION;

#if ( defined( CUDA_VERSION ) && ( CUDA_VERSION >= 6000 ) )
static void
replace_context( uint32_t               newContextId,
                 scorep_cupti_context** context );

#endif

/*
 * Initialize the Score-P CUPTI Activity implementation.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_cupti_activity_init( void )
{
    if ( !scorep_cupti_activity_initialized )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                            "[CUPTI Activity] Initializing ... " );

        /* no buffer size < 1024 bytes allowed (see CUPTI documentation) */
        if ( scorep_cupti_activity_buffer_size < 1024 )
        {
            if ( scorep_cupti_activity_buffer_size > 0 )
            {
                UTILS_WARNING( "[CUPTI Activity] Buffer size has to be at least 1024 "
                               "bytes! It has been set to %zu. Continuing with "
                               "buffer size of 1M",
                               scorep_cupti_activity_buffer_size );
            }

            /* set it to the default buffer size (see scorep_cuda_confvars.inc.c) */
            scorep_cupti_activity_buffer_size = 1024 * 1024;
        }

        scorep_cupti_activity_check_chunk_size();

        if ( scorep_cuda_record_kernels == SCOREP_CUDA_KERNEL_AND_COUNTER )
        {
            /* define kernel counters */
            {
                SCOREP_MetricHandle metric_handle =
                    SCOREP_Definitions_NewMetric( "static_shared_mem",
                                                  "static shared memory",
                                                  SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                                  SCOREP_METRIC_MODE_ABSOLUTE_NEXT,
                                                  SCOREP_METRIC_VALUE_UINT64,
                                                  SCOREP_METRIC_BASE_DECIMAL,
                                                  0,
                                                  "Byte",
                                                  SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                                  SCOREP_INVALID_METRIC );

                scorep_cupti_sampling_set_static_shared_mem =
                    SCOREP_Definitions_NewSamplingSet( 1, &metric_handle,
                                                       SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS, SCOREP_SAMPLING_SET_GPU );
            }

            {
                SCOREP_MetricHandle metric_handle =
                    SCOREP_Definitions_NewMetric( "dynamic_shared_mem",
                                                  "dynamic shared memory",
                                                  SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                                  SCOREP_METRIC_MODE_ABSOLUTE_NEXT,
                                                  SCOREP_METRIC_VALUE_UINT64,
                                                  SCOREP_METRIC_BASE_DECIMAL,
                                                  0,
                                                  "Byte",
                                                  SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                                  SCOREP_INVALID_METRIC );

                scorep_cupti_sampling_set_dynamic_shared_mem =
                    SCOREP_Definitions_NewSamplingSet( 1, &metric_handle,
                                                       SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS, SCOREP_SAMPLING_SET_GPU );
            }

            {
                SCOREP_MetricHandle metric_handle =
                    SCOREP_Definitions_NewMetric( "local_mem_total",
                                                  "total local memory",
                                                  SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                                  SCOREP_METRIC_MODE_ABSOLUTE_NEXT,
                                                  SCOREP_METRIC_VALUE_UINT64,
                                                  SCOREP_METRIC_BASE_DECIMAL,
                                                  0,
                                                  "Byte",
                                                  SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                                  SCOREP_INVALID_METRIC );

                scorep_cupti_sampling_set_local_mem_total =
                    SCOREP_Definitions_NewSamplingSet( 1, &metric_handle,
                                                       SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS, SCOREP_SAMPLING_SET_GPU );
            }

            {
                SCOREP_MetricHandle metric_handle =
                    SCOREP_Definitions_NewMetric( "registers_per_thread",
                                                  "registers per thread",
                                                  SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                                  SCOREP_METRIC_MODE_ABSOLUTE_NEXT,
                                                  SCOREP_METRIC_VALUE_UINT64,
                                                  SCOREP_METRIC_BASE_DECIMAL,
                                                  0,
                                                  "#",
                                                  SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                                  SCOREP_INVALID_METRIC );

                scorep_cupti_sampling_set_registers_per_thread =
                    SCOREP_Definitions_NewSamplingSet( 1, &metric_handle,
                                                       SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS, SCOREP_SAMPLING_SET_GPU );
            }
        }

        /* define region for GPU activity flush */
        /* create the CUPTI activity buffer flush region handle */
        {
            SCOREP_SourceFileHandle cupti_buffer_flush_file_handle =
                SCOREP_Definitions_NewSourceFile( "CUDA_FLUSH" );

            scorep_cupti_buffer_flush_region_handle =
                SCOREP_Definitions_NewRegion( "BUFFER FLUSH",
                                              NULL,
                                              cupti_buffer_flush_file_handle,
                                              0, 0, SCOREP_PARADIGM_CUDA,
                                              SCOREP_REGION_ARTIFICIAL );
        }

        scorep_cupti_activity_register_callbacks();

        /*** enable the activities ***/
        scorep_cupti_activity_enable( true );

        scorep_cupti_activity_initialized = true;
    }
}

void
scorep_cupti_activity_finalize( void )
{
    if ( !scorep_cupti_activity_finalized && scorep_cupti_activity_initialized )
    {
        scorep_cupti_context* context = scorep_cupti_context_list;

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                            "[CUPTI Activity] Finalizing ... " );

        while ( context != NULL )
        {
            /* finalize the CUPTI activity context */
            scorep_cupti_activity_context_finalize( context );
            context->activity = NULL;

            /* set pointer to next context */
            context = context->next;
        }

        scorep_cupti_activity_finalized = true;
    }
}

scorep_cupti_activity*
scorep_cupti_activity_context_create( CUcontext cudaContext )
{
    scorep_cupti_activity* context_activity = NULL;

    /* create new context, as it is not listed */
    context_activity = ( scorep_cupti_activity* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cupti_activity ) );

#if HAVE( CUPTI_ASYNC_SUPPORT )
    context_activity->buffers                  = NULL;
    context_activity->max_buffer_size_exceeded = false;
#else
    context_activity->buffer = NULL;
#endif
    context_activity->scorep_last_gpu_time = SCOREP_GetBeginEpoch();
    context_activity->gpu_idle             = true;

    /*
     * Get time synchronization factor between host and GPU time for measurement
     * interval
     */
    {
        /* get pseudo timestamp as warmup for CUPTI */
        SCOREP_CUPTI_CALL( cuptiGetTimestamp( &( context_activity->sync.gpu_start ) ) );

        scorep_cupti_set_synchronization_point( &( context_activity->sync.gpu_start ),
                                                &( context_activity->sync.host_start ) );
    }

    /* set default CUPTI stream ID (needed for memory usage and idle tracing) */
    SCOREP_CUPTI_CALL( cuptiGetStreamId( cudaContext, NULL, &( context_activity->default_strm_id ) ) );

    return context_activity;
}

#if ( defined( CUDA_VERSION ) && ( CUDA_VERSION >= 6000 ) )
/*
 * Set the given Score-P context (currently flushing records) to the context
 * corresponding to the given context id (record's context).
 * The synchronization data of the new context (record's context) are overwritten
 * with the Score-P (flush) context synchronization data.
 *
 * @param newContextId context ID of the context replacing the current one
 * @param context Score-P context (that is currently flushed)
 */
static void
replace_context( uint32_t               newContextId,
                 scorep_cupti_context** context )
{
    if ( NULL == context || NULL == *context )
    {
        return;
    }

    //flush context is the same as record context
    if ( ( *context )->context_id == newContextId )
    {
        return;
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                        "[CUPTI Activity] Replace flush context (ID: %d) with "
                        "record context (ID: %d)",
                        ( *context )->context_id, newContextId );

    /* get CUDA context for each individual record as records are mixed in buffer */
    /* update sync data of record's context with that of actually sync'd context */
    scorep_cupti_sync current_sync_data = ( *context )->activity->sync;

    // get the record's context
    scorep_cupti_context* sync_context = scorep_cupti_context_get_by_id( newContextId );

    if ( sync_context )
    {
        //set record's context for write kernel/memcpy routine
        *context = sync_context;

        //overwrite synchronization data of the records context
        ( *context )->activity->sync           = current_sync_data;
        ( *context )->activity->sync.host_stop = current_sync_data.host_stop;
        ( *context )->activity->sync.gpu_stop  = current_sync_data.gpu_stop;
        ( *context )->activity->sync.factor    = current_sync_data.factor;
    }
}
#endif


void
scorep_cupti_activity_write_kernel( CUpti_ActivityKernelType* kernel,
                                    scorep_cupti_context*     context )
{
#if ( defined( CUDA_VERSION ) && ( CUDA_VERSION >= 6000 ) )
    replace_context( kernel->contextId, &context );
#endif

    //context and context->activity cannot be NULL (caller of this function checks both)

    scorep_cupti_activity*        contextActivity = context->activity;
    scorep_cupti_stream*          stream          = NULL;
    SCOREP_Location*              stream_location = NULL;
    SCOREP_RegionHandle           regionHandle    = SCOREP_INVALID_REGION;
    scorep_cuda_kernel_hash_node* hashNode        = NULL;

    /* get Score-P thread ID for the kernel's stream */
    stream          = scorep_cupti_stream_get_create( context, kernel->streamId );
    stream_location = stream->scorep_location;

    /* get the Score-P region ID for the kernel */
    hashNode = scorep_cupti_kernel_hash_get( kernel->name );
    if ( hashNode )
    {
        regionHandle = hashNode->region;
    }
    else
    {
        char* knName = SCOREP_DEMANGLE_CUDA_KERNEL( kernel->name );

        if ( knName == NULL || *knName == '\0' )
        {
            // kernel->name is probably already demangled
            knName = ( char* )kernel->name;

            if ( knName == NULL )
            {
                knName = "unknownKernel";
            }
        }

        // create a new region definition for this kernel
        regionHandle = SCOREP_Definitions_NewRegion( knName, kernel->name,
                                                     scorep_cupti_kernel_file_handle, 0, 0,
                                                     SCOREP_PARADIGM_CUDA, SCOREP_REGION_FUNCTION );

        hashNode = scorep_cupti_kernel_hash_put( kernel->name, regionHandle );
    }

    /* write events */
    {
        uint64_t start = contextActivity->sync.host_start
                         + ( kernel->start - contextActivity->sync.gpu_start )
                         * contextActivity->sync.factor;
        uint64_t stop = start + ( kernel->end - kernel->start )
                        * contextActivity->sync.factor;

        /* if current activity's start time is before last written timestamp */
        if ( start < stream->scorep_last_timestamp )
        {
            UTILS_WARN_ONCE( "[CUPTI Activity] Kernel: start time (%" PRIu64 ") "
                             "< (%" PRIu64 ") last written timestamp!",
                             start, stream->scorep_last_timestamp );
            UTILS_WARN_ONCE( "[CUPTI Activity] Kernel: '%s', CUdevice: %d, "
                             "CUDA stream ID: %d",
                             hashNode->name, context->cuda_device, stream->stream_id );

            if ( stream->scorep_last_timestamp < stop )
            {
                UTILS_WARN_ONCE( "[CUPTI Activity] Set kernel start time to sync-point time"
                                 " (truncate %.4lf%%)",
                                 ( double )( stream->scorep_last_timestamp - start ) / ( double )( stop - start ) );
                start = stream->scorep_last_timestamp;
            }
            else
            {
                UTILS_WARN_ONCE( "[CUPTI Activity] Skipping ..." );
                return;
            }
        }

        /* check if time between start and stop is increasing */
        if ( stop < start )
        {
            UTILS_WARN_ONCE( "[CUPTI Activity] Kernel: start time > stop time!" );
            UTILS_WARN_ONCE( "[CUPTI Activity] Skipping '%s' on CUDA device:stream [%d:%d],",
                             hashNode->name, context->cuda_device, stream->stream_id );
            return;
        }

        /* check if synchronization stop time is before kernel stop time */
        if ( contextActivity->sync.host_stop < stop )
        {
            UTILS_WARN_ONCE( "[CUPTI Activity] Kernel: sync-point time "
                             "(%" PRIu64 ") < (%" PRIu64 ") kernel stop time",
                             contextActivity->sync.host_stop, stop );
            UTILS_WARN_ONCE( "[CUPTI Activity] Kernel: '%s', CUdevice: %d, "
                             "CUDA stream ID: %d",
                             hashNode->name, context->cuda_device, stream->stream_id );

            /* Write kernel with sync.hostStop stop time stamp, if possible */
            if ( contextActivity->sync.host_stop > start )
            {
                UTILS_WARN_ONCE( "[CUPTI Activity] Set kernel-stop-time to sync-point-time "
                                 "(truncate %.4lf%%)",
                                 ( double )( stop - contextActivity->sync.host_stop ) / ( double )( stop - start ) );

                stop = contextActivity->sync.host_stop;
            }
            else
            {
                UTILS_WARN_ONCE( "[CUPTI Activity] Skipping ..." );
                return;
            }
        }

        /* set the last Score-P timestamp, written in this stream */
        stream->scorep_last_timestamp = stop;

        /* GPU idle time will be written to first CUDA stream in list */
        if ( scorep_cuda_record_idle )
        {
            if ( contextActivity->gpu_idle )
            {
                SCOREP_Location_ExitRegion( context->streams->scorep_location, start, scorep_cupti_idle_region_handle );
                contextActivity->gpu_idle = false;
            }
            else if ( start > contextActivity->scorep_last_gpu_time )
            {
                /* idle is off and kernels are consecutive */
                SCOREP_Location_EnterRegion( context->streams->scorep_location, ( contextActivity->scorep_last_gpu_time ), scorep_cupti_idle_region_handle );
                SCOREP_Location_ExitRegion( context->streams->scorep_location, start, scorep_cupti_idle_region_handle );
            }
        }

        if ( !SCOREP_Filtering_MatchFunction( kernel->name, NULL ) )
        {
            SCOREP_Location_EnterRegion( stream_location, start, regionHandle );

            /* use counter to provide additional information for kernels */
            if ( scorep_cuda_record_kernels == SCOREP_CUDA_KERNEL_AND_COUNTER )
            {
                /* grid and block size counter (start) */
                {
                    uint32_t threadsPerBlock = kernel->blockX * kernel->blockY * kernel->blockZ;
                    uint32_t blocksPerGrid   = kernel->gridX * kernel->gridY * kernel->gridZ;

                    SCOREP_Location_TriggerCounterUint64( stream_location, start,
                                                          scorep_cupti_sampling_set_blocks_per_grid, blocksPerGrid );
                    SCOREP_Location_TriggerCounterUint64( stream_location, start,
                                                          scorep_cupti_sampling_set_threads_per_block, threadsPerBlock );
                    SCOREP_Location_TriggerCounterUint64( stream_location, start,
                                                          scorep_cupti_sampling_set_threads_per_kernel, threadsPerBlock * blocksPerGrid );
                }

                /* memory counter (start) */
                SCOREP_Location_TriggerCounterUint64( stream_location, start,
                                                      scorep_cupti_sampling_set_static_shared_mem, kernel->staticSharedMemory );
                SCOREP_Location_TriggerCounterUint64( stream_location, start,
                                                      scorep_cupti_sampling_set_dynamic_shared_mem, kernel->dynamicSharedMemory );
                SCOREP_Location_TriggerCounterUint64( stream_location, start,
                                                      scorep_cupti_sampling_set_local_mem_total, kernel->localMemoryTotal );
                SCOREP_Location_TriggerCounterUint64( stream_location, start,
                                                      scorep_cupti_sampling_set_registers_per_thread, kernel->registersPerThread );

                /* memory counter (stop) */
                SCOREP_Location_TriggerCounterUint64( stream_location, stop,
                                                      scorep_cupti_sampling_set_static_shared_mem, 0 );
                SCOREP_Location_TriggerCounterUint64( stream_location, stop,
                                                      scorep_cupti_sampling_set_dynamic_shared_mem, 0 );
                SCOREP_Location_TriggerCounterUint64( stream_location, stop,
                                                      scorep_cupti_sampling_set_local_mem_total, 0 );
                SCOREP_Location_TriggerCounterUint64( stream_location, stop,
                                                      scorep_cupti_sampling_set_registers_per_thread, 0 );

                /* grid and block size counter (stop) */
                SCOREP_Location_TriggerCounterUint64( stream_location, stop,
                                                      scorep_cupti_sampling_set_blocks_per_grid, 0 );
                SCOREP_Location_TriggerCounterUint64( stream_location, stop,
                                                      scorep_cupti_sampling_set_threads_per_block, 0 );
                SCOREP_Location_TriggerCounterUint64( stream_location, stop,
                                                      scorep_cupti_sampling_set_threads_per_kernel, 0 );
            }

            SCOREP_Location_ExitRegion( stream_location, stop, regionHandle );
        }

        if ( contextActivity->scorep_last_gpu_time < stop )
        {
            contextActivity->scorep_last_gpu_time = stop;
        }
    }
}

void
scorep_cupti_activity_write_memcpy( CUpti_ActivityMemcpy* memcpy,
                                    scorep_cupti_context* context )
{
#if ( defined( CUDA_VERSION ) && ( CUDA_VERSION >= 6000 ) )
    replace_context( memcpy->contextId, &context );
#endif

    scorep_cupti_activity*            contextActivity = context->activity;
    scorep_cupti_activity_memcpy_kind kind            = SCOREP_CUPTI_COPYDIRECTION_UNKNOWN;

    SCOREP_Location*     stream_location = NULL;
    uint64_t             start, stop;
    scorep_cupti_stream* stream = NULL;

    if ( memcpy->copyKind == CUPTI_ACTIVITY_MEMCPY_KIND_DTOD )
    {
        return;
    }

    start = contextActivity->sync.host_start
            + ( memcpy->start - contextActivity->sync.gpu_start )
            * contextActivity->sync.factor;
    stop = start + ( memcpy->end - memcpy->start ) * contextActivity->sync.factor;

    /* get Score-P thread ID for the kernel's stream */
    stream          = scorep_cupti_stream_get_create( context, memcpy->streamId );
    stream_location = stream->scorep_location;

    /* if current activity's start time is before last written timestamp */
    if ( start < stream->scorep_last_timestamp )
    {
        UTILS_WARN_ONCE( "[CUPTI Activity] Memcpy: start time < last written "
                         "timestamp! (CUDA device:stream [%d:%d])",
                         context->cuda_device, stream->stream_id );


        if ( stream->scorep_last_timestamp < stop )
        {
            UTILS_WARN_ONCE( "[CUPTI Activity] Set memcpy start time to "
                             "sync-point time (truncate %.4lf%%)",
                             ( double )( stream->scorep_last_timestamp - start ) / ( double )( stop - start ) );
            start = stream->scorep_last_timestamp;
        }
        else
        {
            UTILS_WARN_ONCE( "[CUPTI Activity] Skipping ..." );
            return;
        }
    }

    /* check if time between start and stop is increasing */
    if ( stop < start )
    {
        UTILS_WARN_ONCE( "[CUPTI Activity] Skipping memcpy (start time > stop time) "
                         "on CUdevice:Stream %d:%d",
                         context->cuda_device, stream->stream_id );
        return;
    }

    /* check if synchronization stop time is before kernel stop time */
    if ( contextActivity->sync.host_stop < stop )
    {
        UTILS_WARN_ONCE( "[CUPTI Activity] Memcpy: sync stop time < stop time! "
                         "(CUDA device:stream [%d:%d])",
                         context->cuda_device, stream->stream_id );

        /* Write memcpy with sync.hostStop stop time stamp, if possible */
        if ( contextActivity->sync.host_stop > start )
        {
            UTILS_WARN_ONCE( "[CUPTI Activity] Set memcpy-stop-time to "
                             "sync-point-time (truncate %.4lf%%)",
                             ( double )( stop - contextActivity->sync.host_stop ) /
                             ( double )( stop - start ) );

            stop = contextActivity->sync.host_stop;
        }
        else
        {
            UTILS_WARN_ONCE( "[CUPTI Activity] Skipping ..." );
            return;
        }
    }

    /* set the last Score-P timestamp, written in this stream */
    stream->scorep_last_timestamp = stop;

    /* check copy direction */
    if ( memcpy->srcKind == CUPTI_ACTIVITY_MEMORY_KIND_DEVICE )
    {
        if ( memcpy->dstKind == CUPTI_ACTIVITY_MEMORY_KIND_DEVICE )
        {
            kind = SCOREP_CUPTI_DEV2DEV;
        }
        else
        {
            kind = SCOREP_CUPTI_DEV2HOST;
        }
    }
    else
    {
        if ( memcpy->dstKind == CUPTI_ACTIVITY_MEMORY_KIND_DEVICE )
        {
            kind = SCOREP_CUPTI_HOST2DEV;
        }
        else
        {
            kind = SCOREP_CUPTI_HOST2HOST;
        }
    }

    /* GPU idle time will be written to first CUDA stream in list */
    if ( scorep_cuda_record_idle == SCOREP_CUDA_PURE_IDLE )
    {
        if ( contextActivity->gpu_idle )
        {
            SCOREP_Location_ExitRegion( context->streams->scorep_location,
                                        start, scorep_cupti_idle_region_handle );
            contextActivity->gpu_idle = false;
        }
        else if ( start > contextActivity->scorep_last_gpu_time )
        {
            SCOREP_Location_EnterRegion( context->streams->scorep_location,
                                         contextActivity->scorep_last_gpu_time,
                                         scorep_cupti_idle_region_handle );
            SCOREP_Location_ExitRegion( context->streams->scorep_location,
                                        start, scorep_cupti_idle_region_handle );
        }
        if ( contextActivity->scorep_last_gpu_time < stop )
        {
            contextActivity->scorep_last_gpu_time = stop;
        }
    }
    else if ( contextActivity->gpu_idle == false &&
              memcpy->streamId == contextActivity->default_strm_id )
    {
        SCOREP_Location_EnterRegion( context->streams->scorep_location, ( contextActivity->scorep_last_gpu_time ),
                                     scorep_cupti_idle_region_handle );
        contextActivity->gpu_idle = true;
    }

    /* remember this CUDA stream is doing CUDA communication */
    if ( kind != SCOREP_CUPTI_DEV2DEV &&
         context->location_id == SCOREP_CUPTI_NO_ID )
    {
        context->location_id = scorep_cupti_location_counter++;
    }

    if ( SCOREP_CUPTI_NO_ID == stream->location_id )
    {
        stream->location_id = scorep_cupti_location_counter++;

        /* create window on every location, where it is used */
        SCOREP_Location_RmaWinCreate( stream->scorep_location,
                                      start,
                                      scorep_cuda_window_handle );
    }

    if ( kind == SCOREP_CUPTI_HOST2DEV )
    {
        SCOREP_Location_RmaGet( stream_location, start,
                                scorep_cuda_window_handle,
                                context->location_id, memcpy->bytes, 42 );
    }
    else if ( kind == SCOREP_CUPTI_DEV2HOST )
    {
        SCOREP_Location_RmaPut( stream_location, start,
                                scorep_cuda_window_handle,
                                context->location_id, memcpy->bytes, 42 );
    }
    else if ( kind == SCOREP_CUPTI_DEV2DEV )
    {
        SCOREP_Location_RmaGet( stream_location, start,
                                scorep_cuda_window_handle,
                                stream->location_id, memcpy->bytes, 42 );
    }

    if ( kind != SCOREP_CUPTI_HOST2HOST )
    {
        SCOREP_Location_RmaOpCompleteBlocking( stream_location, stop,
                                               scorep_cuda_window_handle, 42 );
    }
}

void
scorep_cupti_set_synchronization_point( uint64_t* gpu,
                                        uint64_t* host )
{
    uint64_t t1 = 0, t2 = 0;

    t1 = SCOREP_Timer_GetClockTicks();

    SCOREP_CUPTI_CALL( cuptiGetTimestamp( gpu ) );

    t2 = SCOREP_Timer_GetClockTicks();

    *host = t1 + ( t2 - t1 ) / 2;
}

static void
synchronize_context_list( void )
{
    CUcontext             old_context = NULL;
    scorep_cupti_context* context     = scorep_cupti_context_list;

    if ( context == NULL )
    {
        return;
    }

    /* save the current CUDA context */
    SCOREP_SUSPEND_CUDRV_CALLBACKS();
    SCOREP_CUDA_DRIVER_CALL( cuCtxGetCurrent( &old_context ) );
    while ( NULL != context )
    {
        /* set the context to be synchronized */
        if ( context->cuda_context != old_context )
        {
            SCOREP_CUDA_DRIVER_CALL( cuCtxPushCurrent( context->cuda_context ) );
        }

        SCOREP_CUPTI_UNLOCK();
        SCOREP_CUDA_DRIVER_CALL( cuCtxSynchronize() );
        SCOREP_CUPTI_LOCK();

        /* pop the context from context stack */
        if ( context->cuda_context != old_context )
        {
            SCOREP_CUDA_DRIVER_CALL( cuCtxPopCurrent( &( context->cuda_context ) ) );
        }

        scorep_cupti_set_synchronization_point( &( context->activity->sync.gpu_start ),
                                                &( context->activity->sync.host_start ) );

        context = context->next;
    }
    SCOREP_RESUME_CUDRV_CALLBACKS();
}

/*
 * Enable/Disable recording of CUPTI activities. Use CUPTI mutex to lock this
 * function.
 *
 * @param enable 1 to enable recording of activities, 0 to disable
 */
void
scorep_cupti_activity_enable( bool enable )
{
    if ( enable ) /* enable activities */
    {
        if ( !scorep_cupti_activity_state )
        {
            /* enable kernel recording */
            if ( scorep_cuda_record_kernels )
            {
  #if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
                if ( !( scorep_cuda_features & SCOREP_CUDA_FEATURE_KERNEL_SERIAL ) )
                {
                    SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL ) );
                    scorep_cupti_activity_state |= SCOREP_CUPTI_ACTIVITY_STATE_CONCURRENT_KERNEL;
                }
                else
  #endif
                {
                    SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_KERNEL ) );
                    scorep_cupti_activity_state |= SCOREP_CUPTI_ACTIVITY_STATE_KERNEL;
                }
            }

            /* enable memory copy tracing */
            if ( scorep_cuda_record_memcpy && scorep_cuda_sync_level == 0 )
            {
                SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_MEMCPY ) );
                scorep_cupti_activity_state |= SCOREP_CUPTI_ACTIVITY_STATE_MEMCPY;
            }

            /* create new synchronization points */
            if ( scorep_cupti_activity_state )
            {
                synchronize_context_list();
            }
        }
    }
    else if ( scorep_cupti_activity_state ) /* disable activities */
    {
        /* disable kernel recording */
        if ( scorep_cuda_record_kernels )
        {
#if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
            if ( !( scorep_cuda_features & SCOREP_CUDA_FEATURE_KERNEL_SERIAL ) )
            {
                SCOREP_CUPTI_CALL( cuptiActivityDisable( CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL ) );
                scorep_cupti_activity_state &= ~SCOREP_CUPTI_ACTIVITY_STATE_CONCURRENT_KERNEL;
            }
            else
#endif
            {
                SCOREP_CUPTI_CALL( cuptiActivityDisable( CUPTI_ACTIVITY_KIND_KERNEL ) );
                scorep_cupti_activity_state &= ~SCOREP_CUPTI_ACTIVITY_STATE_KERNEL;
            }
        }

        /* disable memory copy recording */
        if ( scorep_cuda_record_memcpy )
        {
            SCOREP_CUPTI_CALL( cuptiActivityDisable( CUPTI_ACTIVITY_KIND_MEMCPY ) );
            scorep_cupti_activity_state &= ~SCOREP_CUPTI_ACTIVITY_STATE_MEMCPY;
        }

        /* flush activities */
#if HAVE( CUPTI_ASYNC_SUPPORT )
        SCOREP_CUPTI_CALL( cuptiActivityFlushAll( 0 ) );
#else
        synchronize_context_list();
#endif
    }
}
