/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 *  @file
 *
 *  Implementation of GPU activity (kernel execution and memory copies)
 *  capturing with CUPTI activities.
 */

#include <config.h>
#include "SCOREP_Config.h"

#include <UTILS_CStr.h>

#include "scorep_cuda.h"
#include "scorep_cupti_activity.h"

/* CUPTI 4 uses CUpti_ActivityKernel2 instead of CUpti_ActivityKernel */
#if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 4 ) )
typedef CUpti_ActivityKernel2 CUpti_ActivityKernelType;
#else
typedef CUpti_ActivityKernel  CUpti_ActivityKernelType;
#endif

/* reduce buffer size for alignment, if necessary */
#define SCOREP_CUPTI_ACTIVITY_ALIGN_BUFFER( buffer, align ) \
    ( ( ( uintptr_t )( buffer ) & ( ( align ) - 1 ) ) ? \
      ( ( buffer ) - ( ( uintptr_t )( buffer ) & ( ( align ) - 1 ) ) ) : ( buffer ) )

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
} scorep_cupti_activity_memcpy_kind_t;

/* initialization and finalization flags */
static bool scorep_cupti_activity_initialized               = false;
static bool scorep_cupti_activity_finalized                 = false;

static bool scorep_cupti_activity_enabled                   = false;

/* global region IDs for wrapper internal recording */
static SCOREP_RegionHandle cupti_buffer_flush_region_handle = SCOREP_INVALID_REGION;

/*********************** function declarations ***************************/
static scorep_cupti_activity_t*
scorep_cupti_activity_context_create( CUcontext cuCtx );

static void
scorep_cupti_activity_write_record( CUpti_Activity*         record,
                                    scorep_cupti_context_t* context );

static void
scorep_cupti_activity_write_memcpy( CUpti_ActivityMemcpy*   memcpy,
                                    scorep_cupti_context_t* context );

static void
scorep_cupti_activity_write_kernel( CUpti_ActivityKernelType* kernel,
                                    scorep_cupti_context_t*   context );

/******************************************************************************/

/*
 * Initialize the Score-P CUPTI Activity implementation.
 * !!! Has to be locked with Score-P CUPTI lock !!!
 */
void
scorep_cupti_activity_init()
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
                               "bytes! It has been set to %d.", scorep_cupti_activity_buffer_size );
            }
            scorep_cupti_activity_buffer_size = 1024 * 1024;
        }

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
                                                  1,
                                                  "Byte",
                                                  SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );

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
                                                  1,
                                                  "Byte",
                                                  SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );

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
                                                  1,
                                                  "Byte",
                                                  SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );

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
                                                  1,
                                                  "#",
                                                  SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );

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

            cupti_buffer_flush_region_handle =
                SCOREP_Definitions_NewRegion( "flush_cupti_activity_buffer",
                                              NULL,
                                              cupti_buffer_flush_file_handle,
                                              0, 0, SCOREP_PARADIGM_CUDA,
                                              SCOREP_REGION_ARTIFICIAL );
        }

        /*** enable the activities ***/
        scorep_cupti_activity_enable( true );

        scorep_cupti_activity_initialized = true;
    }
}

void
scorep_cupti_activity_finalize()
{
    if ( !scorep_cupti_activity_finalized && scorep_cupti_activity_initialized )
    {
        SCOREP_CUPTI_LOCK();
        if ( !scorep_cupti_activity_finalized && scorep_cupti_activity_initialized )
        {
            scorep_cupti_context_t* context = scorep_cupti_context_list;

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
            SCOREP_CUPTI_UNLOCK();
        }
    }
}

/*
 * Allocate a new buffer and add it to the queue specified by a CUDA context.
 *
 * @param cuCtx the CUDA context, specifying the queue
 *
 * @return pointer to the created buffer
 */
static uint8_t*
scorep_cupti_activity_queue_new_buffer( CUcontext cudaContext )
{
    uint8_t* buffer = ( uint8_t* )malloc( scorep_cupti_activity_buffer_size );

    SCOREP_CUPTI_CALL( cuptiActivityEnqueueBuffer( cudaContext, 0,
                                                   SCOREP_CUPTI_ACTIVITY_ALIGN_BUFFER( buffer, 8 ),
                                                   scorep_cupti_activity_buffer_size ) );

    return buffer;
}

uint8_t
scorep_cupti_activity_is_buffer_empty( CUcontext cudaContext )
{
    CUptiResult status = CUPTI_SUCCESS;
    size_t      bsize  = 0;

    status = cuptiActivityQueryBuffer( cudaContext, 0, &bsize );
    if ( ( status == CUPTI_SUCCESS && bsize > 0 ) ||
         status == CUPTI_ERROR_MAX_LIMIT_REACHED )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void
scorep_cupti_activity_context_setup( scorep_cupti_context_t* context )
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

    /* queue new buffer to context to record activities */
    if ( context->activity->buffer == NULL )
    {
        context->activity->buffer = scorep_cupti_activity_queue_new_buffer( context->cuda_context );
    }
    else
    {
        SCOREP_CUPTI_CALL( cuptiActivityEnqueueBuffer( context->cuda_context, 0,
                                                       SCOREP_CUPTI_ACTIVITY_ALIGN_BUFFER( context->activity->buffer, 8 ),
                                                       scorep_cupti_activity_buffer_size ) );
    }
}

/*
 * Create a Score-P CUPTI activity context.
 *
 * @param cudaContext the CUDA context
 *
 * @return pointer to created Score-P CUPTI Activity context
 */
static scorep_cupti_activity_t*
scorep_cupti_activity_context_create( CUcontext cudaContext )
{
    scorep_cupti_activity_t* context_activity = NULL;

    /* create new context, as it is not listed */
    context_activity = ( scorep_cupti_activity_t* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cupti_activity_t ) );

    context_activity->buffer               = NULL;
    context_activity->scorep_last_gpu_time = SCOREP_GetBeginEpoch();
    context_activity->gpu_idle             = true;

    /*
     * Get time synchronization factor between host and GPU time for measurement
     * interval
     */
    {
        SCOREP_CUPTI_CALL( cuptiGetTimestamp( &( context_activity->sync.gpu_start ) ) );
        context_activity->sync.host_start = SCOREP_GetClockTicks();
    }

    /* set default CUPTI stream ID (needed for memory usage and idle tracing) */
    SCOREP_CUPTI_CALL( cuptiGetStreamId( cudaContext, NULL, &( context_activity->default_strm_id ) ) );

    return context_activity;
}

/*
 * Finalize a Score-P CUPTI Activity context.
 *
 * @param context Score-P CUPTI context
 */
void
scorep_cupti_activity_context_finalize( scorep_cupti_context_t* context )
{
    scorep_cupti_activity_t* context_activity = NULL;

    if ( context == NULL )
    {
        return;
    }

    context_activity = context->activity;

    if ( context_activity == NULL )
    {
        return;
    }

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
 * Handle activities buffered by CUPTI.
 * !!! Has to be locked with Score-P CUPTI lock !!!
 *
 * @param context the Score-P CUPTI context
 */
void
scorep_cupti_activity_context_flush( scorep_cupti_context_t* context )
{
    CUptiResult              status = CUPTI_SUCCESS;
    uint8_t*                 buffer = NULL;
    size_t                   bufSize;
    CUpti_Activity*          record = NULL;
    uint64_t                 hostStop, gpuStop;
    scorep_cupti_activity_t* context_activity = NULL;

    /* check for Score-P CUPTI context */
    if ( context == NULL || context->activity == NULL )
    {
        UTILS_WARNING( "[CUPTI Activity] Context not found! Cannot flush buffer ..." );
        return;
    }
    context_activity = context->activity;

    /* check if the buffer contains records */
    if ( scorep_cupti_activity_is_buffer_empty( context->cuda_context ) )
    {
        return;
    }

    /* expose Score-P CUPTI activity flush as measurement overhead */
    SCOREP_EnterRegion( cupti_buffer_flush_region_handle );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI Activity] Handle context %d activities", context->cuda_context );

    /* dump the contents of the global queue */
    SCOREP_CUPTI_CALL( cuptiActivityDequeueBuffer( context->cuda_context, 0, &buffer,
                                                   &bufSize ) );

    /*
     * Get time synchronization factor between host and GPU time for measured
     * period
     */
    {
        SCOREP_CUPTI_CALL( cuptiGetTimestamp( &gpuStop ) );
        hostStop                         = SCOREP_GetClockTicks();
        context_activity->sync.host_stop = hostStop;

        context_activity->sync.factor = ( double )( hostStop - context_activity->sync.host_start )
                                        / ( double )( gpuStop - context_activity->sync.gpu_start );
    }

    do
    {
        status = cuptiActivityGetNextRecord( buffer, bufSize, &record );
        if ( status == CUPTI_SUCCESS )
        {
            scorep_cupti_activity_write_record( record, context );
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

    /* enqueue buffer again */
    SCOREP_CUPTI_CALL( cuptiActivityEnqueueBuffer( context->cuda_context, 0, buffer,
                                                   scorep_cupti_activity_buffer_size ) );


    /* set new synchronization point */
    context_activity->sync.host_start = hostStop;
    context_activity->sync.gpu_start  = gpuStop;

    /* use local variable hostStop to write exit event for activity flush */
    SCOREP_ExitRegion( cupti_buffer_flush_region_handle );
}

/*
 * Select record type and call respective function.
 *
 * @param record the basic CUPTI activity record
 * @param context the Score-P CUPTI activity context
 */
static void
scorep_cupti_activity_write_record( CUpti_Activity*         record,
                                    scorep_cupti_context_t* context )
{
    switch ( record->kind )
    {
        case CUPTI_ACTIVITY_KIND_KERNEL:
#if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
        case CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL:
#endif
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
 * Use the CUPTI activity kernel record to write the corresponding Score-P
 * events.
 *
 * @param kernel the CUPTI activity kernel record
 * @param context the Score-P CUPTI activity context
 */
static void
scorep_cupti_activity_write_kernel( CUpti_ActivityKernelType* kernel,
                                    scorep_cupti_context_t*   context )
{
    scorep_cupti_activity_t*      contextActivity = context->activity;
    scorep_cupti_stream_t*        stream          = NULL;
    SCOREP_Location*              stream_location = NULL;
    SCOREP_RegionHandle           regionHandle    = SCOREP_INVALID_REGION;
    scorep_cuda_kernel_hash_node* hashNode        = NULL;

    /* get Score-P thread ID for the kernel's stream */
    stream = scorep_cupti_stream_get_create( context,
                                             SCOREP_CUPTI_NO_STREAM, kernel->streamId );
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
            knName = ( char* )kernel->name;

            if ( knName == NULL )
            {
                knName = "unknownKernel";
            }
        }

        regionHandle = SCOREP_Definitions_NewRegion( knName, NULL,
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
            UTILS_WARN_ONCE( "[CUPTI Activity] Kernel: start time < last written timestamp!" );
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
            UTILS_WARN_ONCE( "[CUPTI Activity] Kernel: sync-point time < kernel stop time" );
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

        if ( contextActivity->scorep_last_gpu_time < stop )
        {
            contextActivity->scorep_last_gpu_time = stop;
        }
    }
}

/*
 * Use the CUPTI activity memory copy record to write the corresponding
 * Score-P events.
 *
 * @param memcpy the CUPTI activity memory copy record
 * @param context the Score-P CUPTI activity context
 */
static void
scorep_cupti_activity_write_memcpy( CUpti_ActivityMemcpy*   memcpy,
                                    scorep_cupti_context_t* context )
{
    scorep_cupti_activity_t*            contextActivity = context->activity;
    scorep_cupti_activity_memcpy_kind_t kind            = SCOREP_CUPTI_COPYDIRECTION_UNKNOWN;

    SCOREP_Location*       stream_location = NULL;
    uint64_t               start, stop;
    scorep_cupti_stream_t* stream = NULL;

    if ( memcpy->copyKind == CUPTI_ACTIVITY_MEMCPY_KIND_DTOD )
    {
        return;
    }

    start = contextActivity->sync.host_start
            + ( memcpy->start - contextActivity->sync.gpu_start )
            * contextActivity->sync.factor;
    stop = start + ( memcpy->end - memcpy->start ) * contextActivity->sync.factor;

    /* get Score-P thread ID for the kernel's stream */
    stream = scorep_cupti_stream_get_create( context,
                                             SCOREP_CUPTI_NO_STREAM, memcpy->streamId );
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
                                      scorep_cuda_interim_window_handle );
    }

    if ( kind == SCOREP_CUPTI_HOST2DEV )
    {
        SCOREP_Location_RmaGet( stream_location, start,
                                scorep_cuda_interim_window_handle,
                                context->location_id, memcpy->bytes, 42 );
    }
    else if ( kind == SCOREP_CUPTI_DEV2HOST )
    {
        SCOREP_Location_RmaPut( stream_location, start,
                                scorep_cuda_interim_window_handle,
                                context->location_id, memcpy->bytes, 42 );
    }
    else if ( kind == SCOREP_CUPTI_DEV2DEV )
    {
        SCOREP_Location_RmaGet( stream_location, start,
                                scorep_cuda_interim_window_handle,
                                stream->location_id, memcpy->bytes, 42 );
    }

    if ( kind != SCOREP_CUPTI_HOST2HOST )
    {
        SCOREP_Location_RmaOpCompleteBlocking( stream_location, stop,
                                               scorep_cuda_interim_window_handle, 42 );
    }
}

static void
synchronize_context_list( void )
{
    CUcontext               old_context = NULL;
    scorep_cupti_context_t* context     = scorep_cupti_context_list;

    if ( context == NULL )
    {
        return;
    }

    /* save the current CUDA context */
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

        SCOREP_CUPTI_CALL( cuptiGetTimestamp( &( context->activity->sync.gpu_start ) ) );
        context->activity->sync.host_start = SCOREP_GetClockTicks();

        context = context->next;
    }
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
        if ( !scorep_cupti_activity_enabled )
        {
            /* enable kernel recording */
            if ( scorep_cuda_record_kernels )
            {
  #if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
                if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_CONCURRENT_KERNEL )
                     != SCOREP_CUDA_FEATURE_CONCURRENT_KERNEL )
                {
                    SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL ) );
                }
                else
  #endif
                {
                    SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_KERNEL ) );
                }

                scorep_cupti_activity_enabled = true;
            }

            /* enable memory copy tracing */
            if ( scorep_cuda_record_memcpy )
            {
                SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_MEMCPY ) );

                scorep_cupti_activity_enabled = true;
            }

            /* create new synchronization points */
            if ( scorep_cupti_activity_enabled )
            {
                synchronize_context_list();
            }
        }
    }
    else if ( scorep_cupti_activity_enabled ) /* disable activities */
    {
        /* disable kernel recording */
        if ( scorep_cuda_record_kernels )
        {
#if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
            if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_CONCURRENT_KERNEL )
                 != SCOREP_CUDA_FEATURE_CONCURRENT_KERNEL )
            {
                SCOREP_CUPTI_CALL( cuptiActivityDisable( CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL ) );
            }
            else
#endif
            {
                SCOREP_CUPTI_CALL( cuptiActivityDisable( CUPTI_ACTIVITY_KIND_KERNEL ) );
            }

            scorep_cupti_activity_enabled = false;
        }

        /* disable memory copy recording */
        if ( scorep_cuda_record_memcpy )
        {
            SCOREP_CUPTI_CALL( cuptiActivityDisable( CUPTI_ACTIVITY_KIND_MEMCPY ) );

            scorep_cupti_activity_enabled = false;
        }

        /* flush activities */
        if ( !scorep_cupti_activity_enabled )
        {
            synchronize_context_list();
        }
    }
}

#if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
void
scorep_cupti_activity_enable_concurrent_kernel( scorep_cupti_context_t* context )
{
    /*
     * Disable collection of kernels for the given CUDA context.
     * !!! does not work yet !!!

       SCOREP_CUPTI_CALL(cuptiActivityDisableContext(cuCtx, CUPTI_ACTIVITY_KIND_KERNEL));*

     * flush the already buffered activities for this CUDA context *
       scorep_cuptiact_flushCtxActivities(cuCtx);

     * Enable collection of kernels for the given CUDA context
       SCOREP_CUPTI_CALL(cuptiActivityEnableContext(cuCtx, CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL));*/

    /*if((scorep_cuda_features & SCOREP_CUDA_RECORD_CONCURRENT_KERNEL)
           != SCOREP_CUDA_RECORD_CONCURRENT_KERNEL){*/

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                        "[CUPTI Activity] Enable concurrent kernel tracing." );

    /*
     * Disable normal (lower overhead) kernel tracing.
     */
    SCOREP_CUPTI_CALL( cuptiActivityDisable( CUPTI_ACTIVITY_KIND_KERNEL ) );

    SCOREP_CUDA_DRIVER_CALL( cuCtxSynchronize() );

    /*
     * Enable concurrent kernel tracing (higher overhead).
     */
    SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL ) );

    scorep_cuda_features |= SCOREP_CUDA_FEATURE_CONCURRENT_KERNEL;
    /*}*/
}
#endif
