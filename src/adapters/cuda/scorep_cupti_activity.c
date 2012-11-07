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
 *  @file       scorep_cupti_activity.c
 *  @maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>
 *
 *  Implementation of GPU activity (kernel execution and memory copies)
 *  capturing with CUPTI activities.
 */

#include <config.h>
#include <SCOREP_RuntimeManagement.h>
#include "SCOREP_Config.h"
#include "SCOREP_Definitions.h"
#include "SCOREP_Memory.h"
#include "SCOREP_Events.h"
#include "SCOREP_Mutex.h"

#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <UTILS_CStr.h>

#include "SCOREP_Location.h"
#include <SCOREP_Timing.h>

#include "scorep_cuda.h"
#include "scorep_cupti.h"
#include "scorep_cupti_activity.h"

/*#if defined( HAVE_DEMANGLE )
   #include <demangle.h>
   #define SCOREP_DEMANGLE_CUDA_KERNEL( mangled ) \
    cplus_demangle( mangled, 0 )
 #else*/
#define SCOREP_DEMANGLE_CUDA_KERNEL( mangled ) \
    mangled
/*#endif*/

/* reduce buffer size for alignment, if necessary */
#define SCOREP_CUPTI_ACTIVITY_ALIGN_BUFFER( buffer, align ) \
    ( ( ( uintptr_t )( buffer ) & ( ( align ) - 1 ) ) ? \
      ( ( buffer ) - ( ( uintptr_t )( buffer ) & ( ( align ) - 1 ) ) ) : ( buffer ) )

/* the default buffer size for the CUPTI activity buffer */
#define SCOREP_CUPTI_ACTIVITY_DEFAULT_BUFFER_SIZE 65536

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

/*
 * Score-P CUPTI activity synchronization structure
 */
typedef struct scorep_cupti_activity_sync_struct
{
    uint64_t host_start; /**< host measurement interval start timestamp */
    uint64_t host_stop;  /**< host measurement interval stop timestamp */
    uint64_t gpu_start;  /**< gpu measurement interval start timestamp */
    double   factor;     /**< synchronization factor for time interval */
}scorep_cupti_activity_sync;

/*
 * The Score-P CUPTI activity stream stores the CUPTI stream ID and the
 * corresponding SCOREP location. The timestamp is used to ensure writing of
 * events with increasing timestamps.
 */
typedef struct scorep_cupti_activity_stream_struct
{
    uint32_t                                    stream_id;             /**< the CUDA stream */
    SCOREP_Location*                            scorep_location;       /**< Score-P location for this stream (unique) */
    uint64_t                                    scorep_last_timestamp; /**< last written Score-P timestamp */
    struct scorep_cupti_activity_stream_struct* next;
}scorep_cupti_activity_stream;

/*
 * The Score-P CUPTI activity context bundles everything, which is correlated to
 * a CUDA context, together. The are stored in a global linked list. This is
 * efficient enough, as typical CUDA applications are working with one to four
 * CUDA contexts per (MPI) process, depending on the number of available CUDA
 * devices per node.
 */
typedef struct scorep_cupti_activity_context_struct
{
    uint32_t                                     context_id;                /**< context ID */
    CUcontext                                    cuda_context;              /**< CUDA context handle */
    uint32_t                                     device_id;                 /**< device ID */
    CUdevice                                     cuda_device;               /**< CUDA device handle */
    SCOREP_Location*                             scorep_host_location;      /**< Score-P context host location */
    scorep_cupti_activity_stream*                stream_list;               /**< list of streams */
    uint32_t                                     default_stream_id;         /**< CUPTI stream ID of default stream */
    scorep_cupti_activity_sync                   sync;                      /**< store synchronization information */
    uint8_t*                                     buffer;                    /**< CUPTI activity buffer pointer */
    uint64_t                                     scorep_last_gpu_timestamp; /**< last written Score-P timestamp in this context */
    uint8_t                                      gpu_idle_on;               /**< has idle region enter been written last */
    struct scorep_cupti_activity_context_struct* next;
}scorep_cupti_activity_context;


/* initialization and finalization flags */
static bool scorep_cupti_activity_initialized = false;
static bool scorep_cupti_activity_finalized   = false;

/* thread (un)locking macros */
static SCOREP_Mutex cupti_activity_mutex = NULL;
# define SCOREP_CUPTI_ACTIVITY_LOCK() SCOREP_MutexLock( cupti_activity_mutex )
# define SCOREP_CUPTI_ACTIVITY_UNLOCK() SCOREP_MutexUnlock( cupti_activity_mutex )

/* size of the activity buffer */
static size_t scorep_cupti_activity_buffer_size = SCOREP_CUPTI_ACTIVITY_DEFAULT_BUFFER_SIZE;

/* hash table for CUDA kernels */
static scorep_cupti_activity_hash_node_string* scorep_cupti_activity_string_hashtab[ SCOREP_CUPTI_ACTIVITY_HASHTABLE_SIZE ];

/* head of the Score-P CUPTI activity context list */
static scorep_cupti_activity_context* scorep_cupti_activity_context_list = NULL;

/* handle for kernel regions */
static SCOREP_SourceFileHandle scorep_kernel_file_handle = SCOREP_INVALID_SOURCE_FILE;

/* handle CUDA idle regions */
SCOREP_RegionHandle scorep_cuda_idle_region_handle = SCOREP_INVALID_REGION;

/* handle for Score-P flush CUPTI buffer regions */
SCOREP_RegionHandle scorep_flush_cupti_activity_buffer_region_handle = SCOREP_INVALID_REGION;

/***************** internal function declarations *****************************/

static uint8_t*
scorep_cupti_activity_queue_new_buffer( CUcontext cudaContext );

static scorep_cupti_activity_context*
scorep_cupti_activity_create_context( uint32_t  contextId,
                                      CUcontext cudaContext,
                                      uint32_t  deviceId );

static void
scorep_cupti_activity_destroy_context( scorep_cupti_activity_context* context );

static scorep_cupti_activity_context*
scorep_cupti_activity_get_context( CUcontext cudaContext );

static void
scorep_cupti_activity_write_record( CUpti_Activity*                record,
                                    scorep_cupti_activity_context* context );

static void
scorep_cupti_activity_write_memcpy_record( CUpti_ActivityMemcpy*          mcpy,
                                           scorep_cupti_activity_context* context );

static void
scorep_cupti_activity_write_kernel_record( CUpti_ActivityKernel*          kernel,
                                           scorep_cupti_activity_context* context );

static void*
scorep_cupti_activity_string_hash_put( const char*         name,
                                       SCOREP_RegionHandle region );

static void*
scorep_cupti_activity_string_hash_get( const char* name );

static void
scorep_cupti_activity_string_hash_clear( void );

/******************************************************************************/


/*
 * DJB2 hash function.
 *
 * @param s pointer to a string to be hashed

   static unsigned int
   hashStringDJB2( const char* s )
   {
    unsigned int hash = 5381;
    int          c;

    while ( ( c = *s++ ) )
    {
        hash = ( ( hash << 5 ) + hash ) ^ c;
    }

    return hash;
   }*/

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

void
scorep_cupti_activity_init( void )
{
    if ( !scorep_cupti_activity_initialized )
    {
        /* TODO: is it possible to lock this mutex creation */
        SCOREP_MutexCreate( &cupti_activity_mutex );

        SCOREP_CUPTI_ACTIVITY_LOCK();
        if ( !scorep_cupti_activity_initialized )
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                    "[CUPTI Activity] Initializing ... \n" );

            /*** enable the activities ***/
            /* enable kernel tracing */
            if ( scorep_cuda_record_kernels > 0 )
            {
                SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_KERNEL ) );

                /* create a region handle for CUDA idle */
                scorep_kernel_file_handle = SCOREP_DefineSourceFile( "CUDA_KERNEL" );

                /* create a region handle for CUDA idle */
                if ( scorep_cuda_record_idle )
                {
                    SCOREP_SourceFileHandle cuda_idle_file_handle =
                        SCOREP_DefineSourceFile( "CUDA_IDLE" );

                    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                            "[CUPTI Activity] Idle enabled ... \n" );

                    scorep_cuda_idle_region_handle =
                        SCOREP_DefineRegion( "compute_idle",
                                             NULL,
                                             cuda_idle_file_handle,
                                             0, 0, SCOREP_ADAPTER_CUDA,
                                             SCOREP_REGION_FUNCTION );
                }
            }


            /* enable memory copy tracing */
            if ( scorep_cuda_record_memcpy )
            {
                SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_MEMCPY ) );
            }

            /* create the CUPTI activity buffer flush region handle */
            {
                SCOREP_SourceFileHandle cupti_buffer_flush_file_handle =
                    SCOREP_DefineSourceFile( "SCOREP_CUDA" );

                scorep_flush_cupti_activity_buffer_region_handle =
                    SCOREP_DefineRegion( "flush_cupti_activity_buffer",
                                         NULL,
                                         cupti_buffer_flush_file_handle,
                                         0, 0, SCOREP_ADAPTER_CUDA,
                                         SCOREP_REGION_FUNCTION );
            }

            scorep_cupti_activity_initialized = true;
            SCOREP_CUPTI_ACTIVITY_UNLOCK();
        }
    }
}

void
scorep_cupti_activity_finalize( void )
{
    if ( !scorep_cupti_activity_finalized && scorep_cupti_activity_initialized )
    {
        SCOREP_CUPTI_ACTIVITY_LOCK();
        if ( !scorep_cupti_activity_finalized && scorep_cupti_activity_initialized )
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                    "[CUPTI Activity] Finalizing ... \n" );

            scorep_cupti_activity_finalized = true;
            SCOREP_CUPTI_ACTIVITY_UNLOCK();

            while ( scorep_cupti_activity_context_list != NULL )
            {
                scorep_cupti_activity_context* context = scorep_cupti_activity_context_list;

                /* write buffered activities, which have not been dumped yet */
                scorep_cupti_activity_flush_context_activities( context->cuda_context );

                /* set pointer to next context before freeing current one */
                scorep_cupti_activity_context_list = scorep_cupti_activity_context_list->next;

                /* free the context */
                scorep_cupti_activity_destroy_context( context );
            }

            scorep_cupti_activity_string_hash_clear();
            SCOREP_MutexDestroy( &cupti_activity_mutex );
        }
    }
}

void
scorep_cupti_activity_add_context( CUcontext cudaContext,
                                   CUdevice  cudaDevice )
{
    scorep_cupti_activity_context* context = NULL;

    if ( scorep_cupti_activity_get_context( cudaContext ) != NULL )
    {
        return;
    }

    context = scorep_cupti_activity_create_context( ( uint32_t )-1, cudaContext, ( uint32_t )cudaDevice );

    /* prepend context */
    SCOREP_CUPTI_ACTIVITY_LOCK();
    context->next                      = scorep_cupti_activity_context_list;
    scorep_cupti_activity_context_list = context;
    SCOREP_CUPTI_ACTIVITY_UNLOCK();

    /* queue new buffer to context to record activities*/
    context->buffer = scorep_cupti_activity_queue_new_buffer( cudaContext );
}

void
scorep_cupti_activity_flush_context_activities( CUcontext cudaContext )
{
    CUptiResult                    status;
    uint8_t*                       buffer = NULL;
    size_t                         buffer_size;
    CUpti_Activity*                record  = NULL;
    scorep_cupti_activity_context* context = NULL;
    uint64_t                       host_stop, gpu_stop;

    /* check if the buffer contains records */
    status = cuptiActivityQueryBuffer( cudaContext, 0, &buffer_size );
    if ( status != CUPTI_SUCCESS )
    {
        if ( CUPTI_ERROR_QUEUE_EMPTY == status ||
             CUPTI_ERROR_MAX_LIMIT_REACHED != status )
        {
            return;
        }
    }

    SCOREP_EnterRegion( scorep_flush_cupti_activity_buffer_region_handle );

    /* get the corresponding Score-P CUPTI activity context */
    context = scorep_cupti_activity_get_context( cudaContext );
    if ( context == NULL )
    {
        UTILS_WARNING( "[CUPTI Activity] Context not found!" );

        SCOREP_ExitRegion( scorep_flush_cupti_activity_buffer_region_handle );

        return;
    }

    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                            "[CUPTI Activity] Handle context %d activities\n", cudaContext );

    /* lock the whole buffer flush */
    SCOREP_CUPTI_ACTIVITY_LOCK();

    /* dump the contents of the global queue */
    SCOREP_CUPTI_CALL( cuptiActivityDequeueBuffer( cudaContext, 0, &buffer, &buffer_size ) );

    /*
     * Get time synchronization factor between host and GPU time for measured
     * period
     */
    {
        SCOREP_CUPTI_CALL( cuptiGetTimestamp( &gpu_stop ) );
        host_stop               = SCOREP_GetClockTicks();
        context->sync.host_stop = host_stop;

        context->sync.factor = ( double )( host_stop - context->sync.host_start )
                               / ( double )( gpu_stop - context->sync.gpu_start );
    }

    do
    {
        status = cuptiActivityGetNextRecord( buffer, buffer_size, &record );
        if ( status == CUPTI_SUCCESS )
        {
            scorep_cupti_activity_write_record( record, context );
        }
        else if ( status == CUPTI_ERROR_MAX_LIMIT_REACHED )
        {
            break;
        }
        else
        {
            SCOREP_CUPTI_CALL( status );
        }
    }
    while ( 1 );

    /* report any records dropped from the global queue */
    {
        size_t dropped;

        SCOREP_CUPTI_CALL( cuptiActivityGetNumDroppedRecords( cudaContext, 0, &dropped ) );
        if ( dropped != 0 )
        {
            UTILS_WARNING( "[CUPTI Activity] Dropped %u records. Current buffer size: %llu \n"
                           "To avoid dropping of records increase the buffer size!\n"
                           "Proposed minimum SCOREP_CUDA_BUFFER=%llu",
                           ( unsigned int )dropped, scorep_cupti_activity_buffer_size,
                           scorep_cupti_activity_buffer_size + dropped / 2 *
                           ( sizeof( CUpti_ActivityKernel ) + sizeof( CUpti_ActivityMemcpy ) ) );
        }
    }

    /* enter GPU idle region after last kernel, if exited before */
    if ( scorep_cuda_record_idle && context->gpu_idle_on == 0 )
    {
        SCOREP_Location_EnterRegion( context->stream_list->scorep_location,
                                     context->scorep_last_gpu_timestamp,
                                     scorep_cuda_idle_region_handle );
        context->gpu_idle_on = 1;
    }

    /* enqueue buffer again */
    SCOREP_CUPTI_CALL( cuptiActivityEnqueueBuffer( cudaContext, 0, buffer,
                                                   scorep_cupti_activity_buffer_size ) );


    /* set new synchronization point */
    context->sync.host_start = host_stop;
    context->sync.gpu_start  = gpu_stop;

    SCOREP_CUPTI_ACTIVITY_UNLOCK();

    SCOREP_ExitRegion( scorep_flush_cupti_activity_buffer_region_handle );
}

/******************************************************************************/
/***************** internal function implementations **************************/

/** @brief
 * Allocate a new buffer and add it to the queue specified by a CUDA context.
 *
 * @param cudaContext       CUDA context specifying the queue.
 *
 * @return Return pointer to buffer.
 */
static uint8_t*
scorep_cupti_activity_queue_new_buffer( CUcontext cudaContext )
{
    uint8_t* buffer = ( uint8_t* )malloc( scorep_cupti_activity_buffer_size );

    SCOREP_CUPTI_CALL( cuptiActivityEnqueueBuffer( cudaContext, 0, SCOREP_CUPTI_ACTIVITY_ALIGN_BUFFER( buffer, 8 ),
                                                   scorep_cupti_activity_buffer_size ) );

    return buffer;
}

/** @brief
 * Create a Score-P CUPTI Activity stream.
 *
 * @param context           Pointer to the Score-P CUPTI activity context,
 *                          the stream is created in.
 * @param streamId          ID of the CUDA stream.
 *
 * @return Return pointer to created Score-P CUPTI Activity stream.
 */
static scorep_cupti_activity_stream*
scorep_cupti_activity_create_stream( scorep_cupti_activity_context* context,
                                     uint32_t                       streamId )
{
    scorep_cupti_activity_stream* stream = NULL;

    stream = ( scorep_cupti_activity_stream* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cupti_activity_stream ) );
    if ( stream == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                     "[CUPTI Activity] Could not allocate memory for stream!" );
    }
    stream->stream_id             = streamId;
    stream->scorep_last_timestamp = SCOREP_GetBeginEpoch();
    stream->next                  = NULL;

    /* if no valid stream ID is given, create default stream 0 */
    if ( ( uint32_t )-1 == streamId )
    {
        streamId = 0;
    }

    {
        char thread_name[ 16 ];

        /* create GPU location */
        if ( context->device_id == SCOREP_CUDA_NO_ID )
        {
            if ( -1 == snprintf( thread_name, 15, "CUDA[?:%d]", streamId ) )
            {
                UTILS_WARNING( "Could not create thread name for CUDA thread!" );
            }
        }
        else
        {
            if ( -1 == snprintf( thread_name, 15, "CUDA[%d:%d]", context->device_id, streamId ) )
            {
                UTILS_WARNING( "Could not create thread name for CUDA thread!" );
            }
        }

        stream->scorep_location = SCOREP_Location_CreateNonCPULocation( context->scorep_host_location,
                                                                        SCOREP_LOCATION_TYPE_GPU, thread_name );
        //SCOREP_Thread_SetLastTimestamp( stream->scorep_location, scorep_cuda_init_timestamp );
    }

    /* if first stream created for this device, make it the default stream */
    if ( context->stream_list == NULL )
    {
        /* write enter event for GPU_IDLE on first stream */
        if ( scorep_cuda_record_idle )
        {
            SCOREP_Location_EnterRegion( stream->scorep_location,
                                         stream->scorep_last_timestamp,
                                         scorep_cuda_idle_region_handle );
            context->gpu_idle_on = 1;
        }
    }

    return stream;
}

/** @brief
 * Check for a Score-P activity stream by stream ID. If it does not exist,
 * create it.
 *
 * @param context           Pointer to the Score-P CUPTI activity context.
 * @param streamId          CUDA stream ID provided by CUPTI callback API.
 *
 * @return Return the Score-P CUDA stream.
 */
static scorep_cupti_activity_stream*
scorep_cupti_activity_check_stream( scorep_cupti_activity_context* context,
                                    uint32_t                       streamId )
{
    scorep_cupti_activity_stream* current_stream = NULL;
    scorep_cupti_activity_stream* last_stream    = NULL;

    if ( context == NULL )
    {
        UTILS_WARNING( "[CUPTI Activity] No context given!" );
        return NULL;
    }

    /* lookup stream */
    current_stream = context->stream_list;
    last_stream    = context->stream_list;
    while ( current_stream != NULL )
    {
        if ( current_stream->stream_id == streamId )
        {
            return current_stream;
        }
        last_stream    = current_stream;
        current_stream = current_stream->next;
    }

    /*
     * If stream list is empty, the stream to be created is not the default
     * stream and GPU idle and memory copy tracing is enabled, then create
     * a default stream.
     */
    if ( context->stream_list == NULL && streamId != context->default_stream_id &&
         scorep_cuda_record_idle && scorep_cuda_record_memcpy )
    {
        context->stream_list = scorep_cupti_activity_create_stream( context, context->default_stream_id );
        last_stream          = context->stream_list;
    }

    current_stream = scorep_cupti_activity_create_stream( context, streamId );

    /* append */
    if ( NULL != last_stream )
    {
        last_stream->next = current_stream;
    }
    else
    {
        context->stream_list = current_stream;
    }

    return current_stream;
}

/** @brief
 * Create a Score-P CUPTI Activity context.
 *
 * @param contextId         ID of the CUDA context.
 * @param cudaContext       CUDA context.
 * @param deviceId          ID of the CUDA device.
 *
 * @return Return pointer to created Score-P CUPTI Activity context.
 */
static scorep_cupti_activity_context*
scorep_cupti_activity_create_context( uint32_t  contextId,
                                      CUcontext cudaContext,
                                      uint32_t  deviceId )
{
    scorep_cupti_activity_context* context = NULL;

    /* create new context, as it is not listed */
    context = ( scorep_cupti_activity_context* )malloc( sizeof( scorep_cupti_activity_context ) );
    if ( context == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                     "[CUPTI Activity] Could not allocate memory for context!" );
    }
    context->context_id                = contextId;
    context->next                      = NULL;
    context->stream_list               = NULL;
    context->buffer                    = NULL;
    context->scorep_last_gpu_timestamp = SCOREP_GetBeginEpoch();
    context->gpu_idle_on               = 1;

    /*
     * Get time synchronization factor between host and GPU time for measurement
     * interval
     */
    SCOREP_CUPTI_CALL( cuptiGetTimestamp( &( context->sync.gpu_start ) ) );
    context->sync.host_start = SCOREP_GetClockTicks();

    context->scorep_host_location = SCOREP_Location_GetCurrentCPULocation();

    if ( cudaContext == NULL )
    {
        SCOREP_CUDA_DRIVER_CALL( cuCtxGetCurrent( &cudaContext ) );
    }
    context->cuda_context = cudaContext;

    /* set default CUPTI stream ID (needed for memory usage and idle tracing) */
    SCOREP_CUPTI_CALL( cuptiGetStreamId( context->cuda_context, NULL, &( context->default_stream_id ) ) );

    if ( deviceId == ( uint32_t )-1 )
    {
        CUdevice cudaDevice;

        /* driver API prog: correct cudaDevice, but result is 201 (invalid context) */
        if ( CUDA_SUCCESS != cuCtxGetDevice( &cudaDevice ) )
        {
            deviceId = SCOREP_CUDA_NO_ID;
        }
        else
        {
            deviceId = ( uint32_t )cudaDevice;
        }
    }

    context->device_id   = deviceId;
    context->cuda_device = deviceId;

    return context;
}

/** @brief
 * Destroy a Score-P CUPTI Activity context.
 *
 * @param context           Pointer to the Score-P CUPTI Activity context.
 */
static void
scorep_cupti_activity_destroy_context( scorep_cupti_activity_context* context )
{
    /* write exit event for GPU idle time */
    if ( scorep_cuda_record_idle && context->gpu_idle_on == 1 )
    {
        uint64_t idle_end = SCOREP_GetClockTicks();
        SCOREP_Location_ExitRegion( context->stream_list->scorep_location, idle_end,
                                    scorep_cuda_idle_region_handle );
    }

    /* cleanup stream list (not needed due to SCOREP_Memory_AllocForMisc())
       while ( context->stream_list != NULL )
       {
        scorep_cupti_activity_stream* stream = context->stream_list;

        context->stream_list = context->stream_list->next;

        free( stream );
        stream = NULL;
       }*/

    /* free activity buffer */
    if ( context->buffer != NULL )
    {
        free( context->buffer );
    }

    /* free the memory allocated for the Score-P CUPTI activity context */
    free( context );
}

/** @brief
 * Get a Score-P CUPTI Activity context by CUDA context.
 *
 * @param cudaContext       CUDA context.
 *
 * @return Return Score-P CUPTI Activity context.
 */
static scorep_cupti_activity_context*
scorep_cupti_activity_get_context( CUcontext cudaContext )
{
    scorep_cupti_activity_context* context = NULL;

    /* lookup context */
    SCOREP_CUPTI_ACTIVITY_LOCK();
    context = scorep_cupti_activity_context_list;
    while ( context != NULL )
    {
        if ( context->cuda_context == cudaContext )
        {
            SCOREP_CUPTI_ACTIVITY_UNLOCK();
            return context;
        }
        context = context->next;
    }
    SCOREP_CUPTI_ACTIVITY_UNLOCK();

    return NULL;
}

/** @brief
 * Select record type and call respective function.
 *
 * @param record            Pointer to the basic CUPTI activity record.
 * @param context           Score-P CUPTI activity context.
 */
static void
scorep_cupti_activity_write_record( CUpti_Activity*                record,
                                    scorep_cupti_activity_context* context )
{
    switch ( record->kind )
    {
        case CUPTI_ACTIVITY_KIND_KERNEL:
        {
            scorep_cupti_activity_write_kernel_record( ( CUpti_ActivityKernel* )record, context );
            break;
        }

        case CUPTI_ACTIVITY_KIND_MEMCPY:
        {
            scorep_cupti_activity_write_memcpy_record( ( CUpti_ActivityMemcpy* )record, context );
            break;
        }
        default:
        {
            break;
        }
    }
}

/** @brief
 * Use the CUPTI activity kernel record to write the corresponding Score-P
 * events.
 *
 * @param kernel            CUPTI activity kernel record.
 * @param context           Score-P CUPTI activity context.
 */
static void
scorep_cupti_activity_write_kernel_record( CUpti_ActivityKernel*          kernel,
                                           scorep_cupti_activity_context* context )
{
    /* get Score-P thread ID for the kernel's stream */
    scorep_cupti_activity_stream* stream          = scorep_cupti_activity_check_stream( context, kernel->streamId );
    SCOREP_Location*              stream_location = stream->scorep_location;
    SCOREP_RegionHandle           kernel_region   = SCOREP_INVALID_REGION;

    /* get the Score-P region handle for the kernel */
    scorep_cupti_activity_hash_node_string* kernel_hash_node = scorep_cupti_activity_string_hash_get( kernel->name );
    if ( kernel_hash_node != NULL )
    {
        kernel_region = kernel_hash_node->region;

        /*UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI Activity] "
                                 "Get kernel: '%s' \n", kernel_hash_node->name );*/
    }
    else
    {
        const char* kernel_name =  SCOREP_DEMANGLE_CUDA_KERNEL( kernel->name );

        if ( kernel_name == NULL )
        {
            UTILS_WARNING( "[CUPTI Activity] Kernel name missing!" );
        }

        /*UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI Activity] "
                                 "Define kernel: '%s' \n", kernel_name );*/

        kernel_region = SCOREP_DefineRegion( kernel_name, NULL, scorep_kernel_file_handle,
                                             0, 0, SCOREP_ADAPTER_CUDA, SCOREP_REGION_FUNCTION );

        kernel_hash_node = scorep_cupti_activity_string_hash_put( kernel->name, kernel_region );
    }

    /* write events */
    {
        uint64_t start = context->sync.host_start
                         + ( kernel->start - context->sync.gpu_start ) * context->sync.factor;
        uint64_t stop = start + ( kernel->end - kernel->start ) * context->sync.factor;

        /* if current activity's start time is before last written timestamp */
        if ( start < stream->scorep_last_timestamp )
        {
            UTILS_WARN_ONCE( "[CUPTI Activity] "
                             "Kernel: start time < last written timestamp!" );
            return;
        }

        /* check if time between start and stop is increasing */
        if ( stop < start )
        {
            UTILS_WARN_ONCE( "[CUPTI Activity] "
                             "Kernel: start time > stop time!" );
            return;
        }

        /* check if synchronization stop time is before kernel stop time */
        if ( context->sync.host_stop < stop )
        {
            UTILS_WARN_ONCE( "[CUPTI Activity] "
                             "Kernel: sync stop time < stop time!" );
            return;
        }

        /* set the last Score-P timestamp, written in this stream */
        stream->scorep_last_timestamp = stop;

        /* GPU idle time will be written to first CUDA stream in list */
        if ( scorep_cuda_record_idle )
        {
            if ( context->gpu_idle_on == 1 )
            {
                SCOREP_Location_ExitRegion( context->stream_list->scorep_location, start,
                                            scorep_cuda_idle_region_handle );
                context->gpu_idle_on = 0;
            }
            else if ( start > context->scorep_last_gpu_timestamp )
            {
                /* idle is off and kernels are consecutive */
                SCOREP_Location_EnterRegion( context->stream_list->scorep_location,
                                             context->scorep_last_gpu_timestamp,
                                             scorep_cuda_idle_region_handle );
                SCOREP_Location_ExitRegion( context->stream_list->scorep_location,
                                            start,
                                            scorep_cuda_idle_region_handle );
            }
        }

        SCOREP_Location_EnterRegion( stream_location, start, kernel_region );
        SCOREP_Location_ExitRegion( stream_location, stop, kernel_region );

        if ( context->scorep_last_gpu_timestamp < stop )
        {
            context->scorep_last_gpu_timestamp = stop;
        }
    }

    /*UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI Activity] "
                             "KERNEL '%s' [%llu ns] device %u, context %u, stream %u, "
                             "correlation %u/r%u\n"
                             "\t grid [%u,%u,%u], block [%u,%u,%u], "
                             "shared memory (static %u, dynamic %u)\n",
                             kernel->name, ( unsigned long long )( kernel->end - kernel->start ),
                             kernel->deviceId, kernel->contextId, kernel->streamId,
                             kernel->correlationId, kernel->runtimeCorrelationId,
                             kernel->gridX, kernel->gridY, kernel->gridZ,
                             kernel->blockX, kernel->blockY, kernel->blockZ,
                             kernel->staticSharedMemory, kernel->dynamicSharedMemory );*/
}

/** @brief
 * Use the CUPTI activity memory copy record to write the corresponding
 * Score-P events.
 *
 * @param mcpy              CUPTI activity memory copy record.
 * @param context           Score-P CUPTI activity context.
 */
static void
scorep_cupti_activity_write_memcpy_record( CUpti_ActivityMemcpy*          mcpy,
                                           scorep_cupti_activity_context* context )
{
    /*vt_gpu_copy_kind_t kind = VT_GPU_COPYDIRECTION_UNKNOWN;*/

    SCOREP_Location*              stream_location = NULL;
    uint64_t                      start, stop;
    scorep_cupti_activity_stream* stream = NULL;

    if ( mcpy->copyKind == CUPTI_ACTIVITY_MEMCPY_KIND_DTOD )
    {
        return;
    }

    start = context->sync.host_start
            + ( mcpy->start - context->sync.gpu_start ) * context->sync.factor;
    stop = start + ( mcpy->end - mcpy->start ) * context->sync.factor;

    /* get Score-P location for the kernel's stream */
    stream          = scorep_cupti_activity_check_stream( context, mcpy->streamId );
    stream_location = stream->scorep_location;

    /* if current activity's start time is before last written timestamp */
    if ( start < stream->scorep_last_timestamp )
    {
        UTILS_WARN_ONCE( "[CUPTI Activity] Memcpy: start time < last written timestamp!" );
        return;
    }

    /* check if time between start and stop is increasing */
    if ( stop < start )
    {
        UTILS_WARN_ONCE( "[CUPTI Activity] Memcpy: start time > stop time!" );
        return;
    }

    /* check if synchronization stop time is before kernel stop time */
    if ( context->sync.host_stop < stop )
    {
        UTILS_WARN_ONCE( "[CUPTI Activity] Memcpy: sync stop time < stop time!" );
        return;
    }

    /* set the last Score-P timestamp, written in this stream */
    stream->scorep_last_timestamp = stop;

    /* check copy direction */
    if ( mcpy->srcKind == CUPTI_ACTIVITY_MEMORY_KIND_DEVICE )
    {
        if ( mcpy->dstKind == CUPTI_ACTIVITY_MEMORY_KIND_DEVICE )
        {
            /*kind = VT_GPU_DEV2DEV;*/
        }
        else
        {
            /*kind = VT_GPU_DEV2HOST;*/
        }
    }
    else
    {
        if ( mcpy->dstKind == CUPTI_ACTIVITY_MEMORY_KIND_DEVICE )
        {
            /*kind = VT_GPU_HOST2DEV;*/
        }
        else
        {
            /*kind = VT_GPU_HOST2HOST;*/
        }
    }

    if ( scorep_cuda_record_idle
         && context->gpu_idle_on == 0
         && mcpy->streamId == context->default_stream_id )
    {
        SCOREP_Location_EnterRegion( context->stream_list->scorep_location,
                                     context->scorep_last_gpu_timestamp,
                                     scorep_cuda_idle_region_handle );
        context->gpu_idle_on = 1;
    }

    /* TODO: write the Score-P communication events
       UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI Activity] "
                             "MEMCPY %llu -> %llu[%llu ns] device %u, context %u, stream %u, "
                             "correlation %u/r%u \n",
                             mcpy->start, mcpy->end,
                             ( unsigned long long )( mcpy->end - mcpy->start ),
                             mcpy->deviceId, mcpy->contextId, mcpy->streamId,
                             mcpy->correlationId, mcpy->runtimeCorrelationId );*/
}

/** @brief
 * Puts a string into the CUPTI activity hash table
 *
 * @param name              Pointer to a string to be stored in the hash table.
 * @param region            Region handle.
 *
 * @return Return pointer to the created hash node.
 */
static void*
scorep_cupti_activity_string_hash_put( const char*         name,
                                       SCOREP_RegionHandle region )
{
    uint32_t id = ( uint32_t )hash_string( name ) % SCOREP_CUPTI_ACTIVITY_HASHTABLE_SIZE;

    scorep_cupti_activity_hash_node_string* add =
        ( scorep_cupti_activity_hash_node_string* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cupti_activity_hash_node_string ) );

    add->name                                  = UTILS_CStr_dup( name ); /* does an implicit malloc */
    add->region                                = region;
    add->next                                  = scorep_cupti_activity_string_hashtab[ id ];
    scorep_cupti_activity_string_hashtab[ id ] = add;

    return add;
}

/** @brief
 * Get a string from the CUPTI Activity hash table
 *
 * @param name              Pointer to a string to be retrieved from the hash table.
 *
 * @return Return pointer to the retrieved hash node.
 */
static void*
scorep_cupti_activity_string_hash_get( const char* name )
{
    uint32_t id = ( uint32_t )hash_string( name )  % SCOREP_CUPTI_ACTIVITY_HASHTABLE_SIZE;

    scorep_cupti_activity_hash_node_string* curr = scorep_cupti_activity_string_hashtab[ id ];

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

/** @brief
 * Clear the CUPTI Activity hash table (free allocated memory).
 */
static void
scorep_cupti_activity_string_hash_clear( void )
{
    int                                     i;
    scorep_cupti_activity_hash_node_string* tmp_node;

    for ( i = 0; i < SCOREP_CUPTI_ACTIVITY_HASHTABLE_SIZE; i++ )
    {
        while ( scorep_cupti_activity_string_hashtab[ i ] )
        {
            tmp_node = scorep_cupti_activity_string_hashtab[ i ]->next;
            free( scorep_cupti_activity_string_hashtab[ i ]->name );
            //free( scorep_cupti_activity_string_hashtab[ i ] );
            scorep_cupti_activity_string_hashtab[ i ] = tmp_node;
        }
    }
}

/******************************************************************************/
