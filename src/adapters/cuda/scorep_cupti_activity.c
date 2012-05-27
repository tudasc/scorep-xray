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
#include "SCOREP_Config.h"
#include "SCOREP_Definitions.h"
#include "SCOREP_Memory.h"
#include "SCOREP_Events.h"
#include "SCOREP_Mutex.h"

#include <scorep_utility/SCOREP_Debug.h>
#include <scorep_utility/SCOREP_Error.h>

#include "SCOREP_Location.h"
#include <SCOREP_Timing.h>

#include "scorep_cuda.h"
#include "scorep_cupti_activity.h"

#if defined( HAVE_DEMANGLE )
#include <demangle.h>
#define sp_cuptiact_demangleKernel( mangled ) \
    cplus_demangle( mangled, 0 )
#else
#define sp_cuptiact_demangleKernel( mangled ) \
    mangled
#endif

/* reduce buffer size for alignment, if necessary */
#define SP_CUPTIACT_ALIGN_BUFFER( buffer, align ) \
    ( ( ( uintptr_t )( buffer ) & ( ( align ) - 1 ) ) ? \
      ( ( buffer ) - ( ( uintptr_t )( buffer ) & ( ( align ) - 1 ) ) ) : ( buffer ) )

/* the default buffer size for the CUPTI activity buffer */
#define SP_CUPTIACT_DEFAULT_BSIZE 65536

/* the default size for the CUDA kernel name hash table */
#define SP_CUPTIACT_HASHTABLE_SIZE 1024

/*
 * The key of the hash node is a string, the value the corresponding region handle.
 * It is used to store region names with its corresponding region handles.
 */
typedef struct sp_cuptiact_hnString_st
{
    char*                           sname;        /**< name of the symbol */
    SCOREP_RegionHandle             regionHandle; /**< associated region handle */
    struct sp_cuptiact_hnString_st* next;         /**< bucket for collision */
} sp_cuptiact_hn_string_t;

/*
 * ScoreP CUPTI activity synchronization structure
 */
typedef struct
{
    uint64_t hostStart; /**< host measurement interval start timestamp */
    uint64_t hostStop;  /**< host measurement interval stop timestamp */
    uint64_t gpuStart;  /**< gpu measurement interval start timestamp */
    double   factor;    /**< synchronization factor for time interval */
}sp_cuptiact_sync_t;

/*
 * The ScoreP CUPTI activity stream stores the CUPTI stream ID and the
 * corresponding SCOREP location. The timestamp is used to ensure writing of
 * events with increasing timestamps.
 */
typedef struct spcuptiactstrm_st
{
    uint32_t                  strmID;     /**< the CUDA stream */
    SCOREP_Location*          spLoc;      /**< ScoreP location for this stream (unique) */
    uint64_t                  spLastTime; /**< last written ScoreP timestamp */
    struct spcuptiactstrm_st* next;
}sp_cuptiact_strm_t;

/*
 * The ScoreP CUPTI activity context bundles everything, which is correlated to
 * a CUDA context, together. The are stored in a global linked list. This is
 * efficient enough, as typical CUDA applications are working with one to four
 * CUDA contexts per (MPI) process, depending on the number of available CUDA
 * devices per node.
 */
typedef struct spcuptiactctx_st
{
    uint32_t                 ctxID;         /**< context ID */
    CUcontext                cuCtx;         /**< CUDA context handle */
    uint32_t                 devID;         /**< device ID */
    CUdevice                 cuDev;         /**< CUDA device handle */
    SCOREP_Location*         sphostLoc;     /**< ScoreP context host location */
    sp_cuptiact_strm_t*      strmList;      /**< list of streams */
    uint32_t                 defaultStrmID; /**< CUPTI stream ID of default stream */
    sp_cuptiact_sync_t       sync;          /**< store synchronization information */
    uint8_t*                 buffer;        /**< CUPTI activity buffer pointer */
    uint64_t                 spLastGPUTime; /**< last written ScoreP timestamp in this context */
    uint8_t                  gpuIdleOn;     /**< has idle region enter been written last */
    struct spcuptiactctx_st* next;
}sp_cuptiact_ctx_t;


/* initialization and finalization flags */
static bool sp_cuptiact_initialized = false;
static bool sp_cuptiact_finalized   = false;

/* thread (un)locking macros */
static SCOREP_Mutex cuptiactMutex = NULL;
# define SP_CUPTIACT_LOCK() SCOREP_MutexLock( cuptiactMutex )
# define SP_CUPTIACT_UNLOCK() SCOREP_MutexUnlock( cuptiactMutex )

/* size of the activity buffer */
static size_t sp_cuptiact_bufSize = SP_CUPTIACT_DEFAULT_BSIZE;

/* hash table for CUDA kernels */
static sp_cuptiact_hn_string_t* sp_cuptiact_string_htab[ SP_CUPTIACT_HASHTABLE_SIZE ];

/* head of the Score-P CUPTI activity context list */
static sp_cuptiact_ctx_t* sp_cuptiact_ctxList = NULL;

/* handle for kernel regions */
static SCOREP_SourceFileHandle sp_kernelFileHandle = SCOREP_INVALID_SOURCE_FILE;

/* handle CUDA idle regions */
SCOREP_RegionHandle sp_cudaIdleRegionHandle = SCOREP_INVALID_REGION;

/* handle for ScoreP flush CUPTI buffer regions */
SCOREP_RegionHandle sp_flushCuptiActBufRegionHandle = SCOREP_INVALID_REGION;

/***************** internal function declarations *****************************/

static uint8_t*
sp_cuptiact_queueNewBuffer( CUcontext cuCtx );

static sp_cuptiact_ctx_t*
sp_cuptiact_createContext( uint32_t  ctxID,
                           CUcontext cuCtx,
                           uint32_t  devID );

static void
sp_cuptiact_destroyContext( sp_cuptiact_ctx_t* vtCtx );

static sp_cuptiact_ctx_t*
sp_cuptiact_getCtx( CUcontext cuCtx );

static void
sp_cuptiact_writeRecord( CUpti_Activity*    record,
                         sp_cuptiact_ctx_t* vtCtx );

static void
sp_cuptiact_writeMemcpyRecord( CUpti_ActivityMemcpy* mcpy,
                               sp_cuptiact_ctx_t*    vtCtx );

static void
sp_cuptiact_writeKernelRecord( CUpti_ActivityKernel* kernel,
                               sp_cuptiact_ctx_t*    vtCtx );

static void*
sp_cuptiact_stringHashPut( const char*         n,
                           SCOREP_RegionHandle rh );

static void*
sp_cuptiact_stringHashGet( const char* n );

static void
sp_cuptiact_stringhashClear( void );

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

/*
 * SDBM hash function. (better than DJB2 for table size 2^10)
 *
 * @param s pointer to a string to be hashed
 */
static unsigned int
hashStringSDBM( const char* s )
{
    unsigned int hash = 0;
    int          c;

    while ( ( c = *s++ ) )
    {
        hash = c + ( hash << 6 ) + ( hash << 16 ) - hash;
    }

    return hash;
}

void
scorep_cupti_activity_init()
{
    if ( !sp_cuptiact_initialized )
    {
        /* TODO: is it possible to lock this mutex creation */
        SCOREP_MutexCreate( &cuptiactMutex );

        SP_CUPTIACT_LOCK();
        if ( !sp_cuptiact_initialized )
        {
            SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                     "[CUPTI Activity] Initializing ... \n" );

            /*** enable the activities ***/
            /* enable kernel tracing */
            if ( scorep_cuda_record_kernels > 0 )
            {
                SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_KERNEL ) );

                /* create a region handle for CUDA idle */
                sp_kernelFileHandle = SCOREP_DefineSourceFile( "CUDA_KERNEL" );

                /* create a region handle for CUDA idle */
                if ( scorep_cuda_record_idle )
                {
                    SCOREP_SourceFileHandle cudaIdleFileHandle =
                        SCOREP_DefineSourceFile( "CUDA_IDLE" );

                    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                             "[CUPTI Activity] Idle enabled ... \n" );

                    sp_cudaIdleRegionHandle = SCOREP_DefineRegion(
                        "compute_idle", cudaIdleFileHandle,
                        0, 0, SCOREP_ADAPTER_CUDA, SCOREP_REGION_FUNCTION );
                }
            }


            /* enable memory copy tracing */
            if ( scorep_cuda_record_memcpy )
            {
                SCOREP_CUPTI_CALL( cuptiActivityEnable( CUPTI_ACTIVITY_KIND_MEMCPY ) );
            }

            /* create the CUPTI activity buffer flush region handle */
            {
                SCOREP_SourceFileHandle cuptiBufFlushFileHandle =
                    SCOREP_DefineSourceFile( "SCOREP_CUDA" );

                sp_flushCuptiActBufRegionHandle = SCOREP_DefineRegion(
                    "flush_cupti_activity_buffer", cuptiBufFlushFileHandle,
                    0, 0, SCOREP_ADAPTER_CUDA, SCOREP_REGION_FUNCTION );
            }

            sp_cuptiact_initialized = true;
            SP_CUPTIACT_UNLOCK();
        }
    }
}

void
scorep_cupti_activity_finalize()
{
    if ( !sp_cuptiact_finalized && sp_cuptiact_initialized )
    {
        SP_CUPTIACT_LOCK();
        if ( !sp_cuptiact_finalized && sp_cuptiact_initialized )
        {
            SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                                     "[CUPTI Activity] Finalizing ... \n" );

            sp_cuptiact_finalized = true;
            SP_CUPTIACT_UNLOCK();

            while ( sp_cuptiact_ctxList != NULL )
            {
                sp_cuptiact_ctx_t* spCtx = sp_cuptiact_ctxList;

                /* write buffered activities, which have not been dumped yet */
                scorep_cuptiact_flushCtxActivities( spCtx->cuCtx );

                /* set pointer to next context before freeing current one */
                sp_cuptiact_ctxList = sp_cuptiact_ctxList->next;

                /* free the context */
                sp_cuptiact_destroyContext( spCtx );
            }

            sp_cuptiact_stringhashClear();
            SCOREP_MutexDestroy( &cuptiactMutex );
        }
    }
}

void
scorep_cuptiact_addContext( CUcontext cuCtx, CUdevice cuDev )
{
    sp_cuptiact_ctx_t* spCtx = NULL;

    if ( sp_cuptiact_getCtx( cuCtx ) != NULL )
    {
        return;
    }

    spCtx = sp_cuptiact_createContext( ( uint32_t )-1, cuCtx, ( uint32_t )cuDev );

    /* prepend context */
    SP_CUPTIACT_LOCK();
    spCtx->next         = sp_cuptiact_ctxList;
    sp_cuptiact_ctxList = spCtx;
    SP_CUPTIACT_UNLOCK();

    /* queue new buffer to context to record activities*/
    spCtx->buffer = sp_cuptiact_queueNewBuffer( cuCtx );
}

void
scorep_cuptiact_flushCtxActivities( CUcontext cuCtx )
{
    CUptiResult        status;
    uint8_t*           buffer = NULL;
    size_t             bufSize;
    CUpti_Activity*    record = NULL;
    sp_cuptiact_ctx_t* spCtx  = NULL;
    uint64_t           hostStop, gpuStop;

    /* check if the buffer contains records */
    status = cuptiActivityQueryBuffer( cuCtx, 0, &bufSize );
    if ( status != CUPTI_SUCCESS )
    {
        if ( CUPTI_ERROR_QUEUE_EMPTY == status ||
             CUPTI_ERROR_MAX_LIMIT_REACHED != status )
        {
            return;
        }
    }

    SCOREP_EnterRegion( sp_flushCuptiActBufRegionHandle );

    /* get the corresponding Score-P CUPTI activity context */
    spCtx = sp_cuptiact_getCtx( cuCtx );
    if ( spCtx == NULL )
    {
        SCOREP_ERROR( SCOREP_WARNING, "[CUPTI Activity] Context not found!\n" );

        SCOREP_ExitRegion( sp_flushCuptiActBufRegionHandle );

        return;
    }

    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                             "[CUPTI Activity] Handle context %d activities\n", cuCtx );

    /* lock the whole buffer flush */
    SP_CUPTIACT_LOCK();

    /* dump the contents of the global queue */
    SCOREP_CUPTI_CALL( cuptiActivityDequeueBuffer( cuCtx, 0, &buffer, &bufSize ) );

    /*
     * Get time synchronization factor between host and GPU time for measured
     * period
     */
    {
        SCOREP_CUPTI_CALL( cuptiGetTimestamp( &gpuStop ) );
        hostStop             = SCOREP_GetClockTicks();
        spCtx->sync.hostStop = hostStop;

        spCtx->sync.factor = ( double )( hostStop - spCtx->sync.hostStart )
                             / ( double )( gpuStop - spCtx->sync.gpuStart );
    }

    /*vt_cntl_msg(1, "hostStop: %llu , gpuStop: %llu", hostStopTS, gpuStopTS);
       vt_cntl_msg(1, "factor: %lf", syncFactor);*/

    do
    {
        status = cuptiActivityGetNextRecord( buffer, bufSize, &record );
        if ( status == CUPTI_SUCCESS )
        {
            sp_cuptiact_writeRecord( record, spCtx );
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

        SCOREP_CUPTI_CALL( cuptiActivityGetNumDroppedRecords( cuCtx, 0, &dropped ) );
        if ( dropped != 0 )
        {
            SCOREP_ERROR( SCOREP_WARNING,
                          "[CUPTI Activity] Dropped %u records. Current buffer size: %llu \n"
                          "To avoid dropping of records increase the buffer size!\n"
                          "Proposed minimum SCOREP_CUDA_BUFFER=%llu",
                          ( unsigned int )dropped, sp_cuptiact_bufSize,
                          sp_cuptiact_bufSize + dropped / 2 *
                          ( sizeof( CUpti_ActivityKernel ) + sizeof( CUpti_ActivityMemcpy ) ) );
        }
    }

    /* enter GPU idle region after last kernel, if exited before */
    if ( spCtx->gpuIdleOn == 0 )
    {
        SCOREP_Location_EnterRegion( spCtx->strmList->spLoc,
                                     spCtx->spLastGPUTime,
                                     sp_cudaIdleRegionHandle );
        spCtx->gpuIdleOn = 1;
    }

    /* enqueue buffer again */
    SCOREP_CUPTI_CALL( cuptiActivityEnqueueBuffer( cuCtx, 0, buffer,
                                                   sp_cuptiact_bufSize ) );


    /* set new synchronization point */
    spCtx->sync.hostStart = hostStop;
    spCtx->sync.gpuStart  = gpuStop;

    SP_CUPTIACT_UNLOCK();

    SCOREP_ExitRegion( sp_flushCuptiActBufRegionHandle );
}

/******************************************************************************/
/***************** internal function implementations **************************/

/*
 * Allocate a new buffer and add it to the queue specified by a CUDA context.
 *
 * @param cuCtx the CUDA context, specifying the queue
 */
static uint8_t*
sp_cuptiact_queueNewBuffer( CUcontext cuCtx )
{
    uint8_t* buffer = ( uint8_t* )malloc( sp_cuptiact_bufSize );

    SCOREP_CUPTI_CALL( cuptiActivityEnqueueBuffer( cuCtx, 0, SP_CUPTIACT_ALIGN_BUFFER( buffer, 8 ),
                                                   sp_cuptiact_bufSize ) );

    return buffer;
}

/*
 * Create a ScoreP CUPTI Activity stream.
 *
 * @param spCtx pointer to the ScoreP CUPTI activity context, the stream
 * is created in
 * @param strmID ID of the CUDA stream
 *
 * @return pointer to created ScoreP CUPTI Activity stream
 */
static sp_cuptiact_strm_t*
sp_cuptiact_createStream( sp_cuptiact_ctx_t* spCtx,
                          uint32_t           strmID )
{
    sp_cuptiact_strm_t* spStrm = NULL;

    spStrm = ( sp_cuptiact_strm_t* )SCOREP_Memory_AllocForMisc( sizeof( sp_cuptiact_strm_t ) );
    if ( spStrm == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                      "[CUPTI Activity] Could not allocate memory for stream!" );
    }
    spStrm->strmID     = strmID;
    spStrm->spLastTime = scorep_cuda_init_timestamp;
    spStrm->next       = NULL;

    /* if no valid stream ID is given, create default stream 0 */
    if ( ( uint32_t )-1 == strmID )
    {
        strmID = 0;
    }

    {
        char thread_name[ 16 ];

        /* create GPU location */
        if ( spCtx->devID == SCOREP_CUDA_NO_ID )
        {
            if ( -1 == snprintf( thread_name, 15, "CUDA[?:%d]", strmID ) )
            {
                SCOREP_ERROR( SCOREP_WARNING,
                              "Could not create thread name for CUDA thread!" );
            }
        }
        else
        {
            if ( -1 == snprintf( thread_name, 15, "CUDA[%d:%d]", spCtx->devID, strmID ) )
            {
                SCOREP_ERROR( SCOREP_WARNING,
                              "Could not create thread name for CUDA thread!" );
            }
        }

        spStrm->spLoc = SCOREP_Location_CreateNonCPULocation( spCtx->sphostLoc,
                                                              SCOREP_LOCATION_TYPE_GPU, thread_name );
        //SCOREP_Thread_SetLastTimestamp( spStrm->spLoc, scorep_cuda_init_timestamp );
    }

    /* if first stream created for this device, make it the default stream */
    if ( spCtx->strmList == NULL )
    {
        /* write enter event for GPU_IDLE on first stream */
        if ( scorep_cuda_record_idle )
        {
            SCOREP_Location_EnterRegion( spStrm->spLoc,
                                         scorep_cuda_init_timestamp,
                                         sp_cudaIdleRegionHandle );
            spCtx->gpuIdleOn = 1;
        }
    }

    return spStrm;
}

/*
 * Check for a ScoreP activity stream by stream ID. If it does not exist,
 * create it.
 *
 * @param spCtx pointer to the ScoreP CUPTI activity context
 * @param strmID the CUDA stream ID provided by CUPTI callback API
 *
 * @return the ScoreP CUDA stream
 */
static sp_cuptiact_strm_t*
sp_cuptiact_checkStream( sp_cuptiact_ctx_t* spCtx,
                         uint32_t           strmID )
{
    sp_cuptiact_strm_t* currStrm = NULL;
    sp_cuptiact_strm_t* lastStrm = NULL;

    if ( spCtx == NULL )
    {
        SCOREP_ERROR( SCOREP_WARNING, "[CUPTI Activity] No context given!" );
        return NULL;
    }

    /* lookup stream */
    currStrm = spCtx->strmList;
    lastStrm = spCtx->strmList;
    while ( currStrm != NULL )
    {
        if ( currStrm->strmID == strmID )
        {
            return currStrm;
        }
        lastStrm = currStrm;
        currStrm = currStrm->next;
    }

    /*
     * If stream list is empty, the stream to be created is not the default
     * stream and GPU idle and memory copy tracing is enabled, then create
     * a default stream.
     */
    if ( spCtx->strmList == NULL && strmID != spCtx->defaultStrmID &&
         scorep_cuda_record_idle && scorep_cuda_record_memcpy )
    {
        spCtx->strmList = sp_cuptiact_createStream( spCtx, spCtx->defaultStrmID );
        lastStrm        = spCtx->strmList;
    }

    currStrm = sp_cuptiact_createStream( spCtx, strmID );

    /* append */
    if ( NULL != lastStrm )
    {
        lastStrm->next = currStrm;
    }
    else
    {
        spCtx->strmList = currStrm;
    }

    return currStrm;
}

/*
 * Create a ScoreP CUPTI Activity context.
 *
 * @param ctxID ID of the CUDA context
 * @param cuCtx the CUDA context
 * @param devID ID of the CUDA device
 *
 * @return pointer to created ScoreP CUPTI Activity context
 */
static sp_cuptiact_ctx_t*
sp_cuptiact_createContext( uint32_t  ctxID,
                           CUcontext cuCtx,
                           uint32_t  devID )
{
    sp_cuptiact_ctx_t* spCtx = NULL;

    /* create new context, as it is not listed */
    spCtx = ( sp_cuptiact_ctx_t* )malloc( sizeof( sp_cuptiact_ctx_t ) );
    if ( spCtx == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                      "[CUPTI Activity] Could not allocate memory for context!" );
    }
    spCtx->ctxID         = ctxID;
    spCtx->next          = NULL;
    spCtx->strmList      = NULL;
    spCtx->buffer        = NULL;
    spCtx->spLastGPUTime = scorep_cuda_init_timestamp;
    spCtx->gpuIdleOn     = 1;

    /*
     * Get time synchronization factor between host and GPU time for measurement
     * interval
     */
    SCOREP_CUPTI_CALL( cuptiGetTimestamp( &( spCtx->sync.gpuStart ) ) );
    spCtx->sync.hostStart = SCOREP_GetClockTicks();

    spCtx->sphostLoc = SCOREP_Location_GetCurrentCPULocation();

    if ( cuCtx == NULL )
    {
        SCOREP_CUDRV_CALL( cuCtxGetCurrent( &cuCtx ) );
    }
    spCtx->cuCtx = cuCtx;

    /* set default CUPTI stream ID (needed for memory usage and idle tracing) */
    SCOREP_CUPTI_CALL( cuptiGetStreamId( spCtx->cuCtx, NULL, &( spCtx->defaultStrmID ) ) );

    if ( devID == ( uint32_t )-1 )
    {
        CUdevice cuDev;

        /* driver API prog: correct cuDev, but result is 201 (invalid context) */
        if ( CUDA_SUCCESS != cuCtxGetDevice( &cuDev ) )
        {
            devID = SCOREP_CUDA_NO_ID;
        }
        else
        {
            devID = ( uint32_t )cuDev;
        }
    }

    spCtx->devID = devID;
    spCtx->cuDev = devID;

    return spCtx;
}

/*
 * Destroy a Score-P CUPTI Activity context.
 *
 * @param spCtx pointer to the ScoreP CUPTI Activity context
 */
static void
sp_cuptiact_destroyContext( sp_cuptiact_ctx_t* spCtx )
{
    /* write exit event for GPU idle time */
    if ( scorep_cuda_record_idle && spCtx->gpuIdleOn == 1 )
    {
        uint64_t idle_end = SCOREP_GetClockTicks();
        SCOREP_Location_ExitRegion( spCtx->strmList->spLoc, idle_end,
                                    sp_cudaIdleRegionHandle );
    }

    /* cleanup stream list (not needed due to SCOREP_Memory_AllocForMisc())
       while ( spCtx->strmList != NULL )
       {
        sp_cuptiact_strm_t* spStrm = spCtx->strmList;

        spCtx->strmList = spCtx->strmList->next;

        free( spStrm );
        spStrm = NULL;
       }*/

    /* free activity buffer */
    if ( spCtx->buffer != NULL )
    {
        free( spCtx->buffer );
    }

    /* free the memory allocated for the Score-P CUPTI activity context */
    free( spCtx );
}

/*
 * Get a ScoreP CUPTI Activity context by CUDA context
 *
 * @param cuCtx the CUDA context
 *
 * @return ScoreP CUPTI Activity context
 */
static sp_cuptiact_ctx_t*
sp_cuptiact_getCtx( CUcontext cuCtx )
{
    sp_cuptiact_ctx_t* spCtx = NULL;

    /* lookup context */
    SP_CUPTIACT_LOCK();
    spCtx = sp_cuptiact_ctxList;
    while ( spCtx != NULL )
    {
        if ( spCtx->cuCtx == cuCtx )
        {
            SP_CUPTIACT_UNLOCK();
            return spCtx;
        }
        spCtx = spCtx->next;
    }
    SP_CUPTIACT_UNLOCK();

    return NULL;
}

/*
 * Select record type and call respective function.
 *
 * @param record pointer to the basic CUPTI activity record
 * @param spCtx the Score-P CUPTI activity context
 */
static void
sp_cuptiact_writeRecord( CUpti_Activity*    record,
                         sp_cuptiact_ctx_t* spCtx )
{
    switch ( record->kind )
    {
        case CUPTI_ACTIVITY_KIND_KERNEL:
        {
            sp_cuptiact_writeKernelRecord( ( CUpti_ActivityKernel* )record, spCtx );
            break;
        }

        case CUPTI_ACTIVITY_KIND_MEMCPY:
        {
            sp_cuptiact_writeMemcpyRecord( ( CUpti_ActivityMemcpy* )record, spCtx );
            break;
        }
        default:
        {
            break;
        }
    }
}

/*
 * Use the CUPTI activity kernel record to write the corresponding ScoreP
 * events.
 *
 * @param kernel the CUPTI activity kernel record
 * @param spCtx the ScoreP CUPTI activity context
 */
static void
sp_cuptiact_writeKernelRecord( CUpti_ActivityKernel* kernel,
                               sp_cuptiact_ctx_t*    spCtx )
{
    /* get Score-P thread ID for the kernel's stream */
    sp_cuptiact_strm_t* spStrm         = sp_cuptiact_checkStream( spCtx, kernel->streamId );
    SCOREP_Location*    strmLoc        = spStrm->spLoc;
    SCOREP_RegionHandle knRegionHandle = SCOREP_INVALID_REGION;

    /* get the ScoreP region handle for the kernel */
    sp_cuptiact_hn_string_t* knHn = sp_cuptiact_stringHashGet( kernel->name );
    if ( knHn != NULL )
    {
        knRegionHandle = knHn->regionHandle;

        /*SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI Activity] "
                                 "Get kernel: '%s' \n", knHn->sname );*/
    }
    else
    {
        char* knName =  sp_cuptiact_demangleKernel( kernel->name );

        if ( knName == NULL )
        {
            SCOREP_ERROR( SCOREP_WARNING, "[CUPTI Activity] Kernel name missing!" );
        }

        /*SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI Activity] "
                                 "Define kernel: '%s' \n", knName );*/

        knRegionHandle = SCOREP_DefineRegion( knName, sp_kernelFileHandle,
                                              0, 0, SCOREP_ADAPTER_CUDA, SCOREP_REGION_FUNCTION );

        knHn = sp_cuptiact_stringHashPut( kernel->name, knRegionHandle );
    }

    /* write events */
    {
        uint64_t start = spCtx->sync.hostStart
                         + ( kernel->start - spCtx->sync.gpuStart ) * spCtx->sync.factor;
        uint64_t stop = start + ( kernel->end - kernel->start ) * spCtx->sync.factor;

        /* if current activity's start time is before last written timestamp */
        if ( start < spStrm->spLastTime )
        {
            SCOREP_WARN_ONCE( "[CUPTI Activity] "
                              "Kernel: start time < last written timestamp!" );
            return;
        }

        /* check if time between start and stop is increasing */
        if ( stop < start )
        {
            SCOREP_WARN_ONCE( "[CUPTI Activity] "
                              "Kernel: start time > stop time!" );
            return;
        }

        /* check if synchronization stop time is before kernel stop time */
        if ( spCtx->sync.hostStop < stop )
        {
            SCOREP_WARN_ONCE( "[CUPTI Activity] "
                              "Kernel: sync stop time < stop time!" );
            return;
        }

        /* set the last Score-P timestamp, written in this stream */
        spStrm->spLastTime = stop;

        /* GPU idle time will be written to first CUDA stream in list */
        if ( scorep_cuda_record_idle )
        {
            if ( spCtx->gpuIdleOn )
            {
                SCOREP_Location_ExitRegion( spCtx->strmList->spLoc, start,
                                            sp_cudaIdleRegionHandle );
                spCtx->gpuIdleOn = 0;
            }
            else if ( start > spCtx->spLastGPUTime )
            {
                /* idle is off and kernels are consecutive */
                SCOREP_Location_EnterRegion( spCtx->strmList->spLoc,
                                             spCtx->spLastGPUTime,
                                             sp_cudaIdleRegionHandle );
                SCOREP_Location_ExitRegion( spCtx->strmList->spLoc, start, sp_cudaIdleRegionHandle );
            }
        }

        SCOREP_Location_EnterRegion( strmLoc, start, knRegionHandle );
        SCOREP_Location_ExitRegion( strmLoc, stop, knRegionHandle );

        if ( spCtx->spLastGPUTime < stop )
        {
            spCtx->spLastGPUTime = stop;
        }
    }

    /*SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI Activity] "
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

/*
 * Use the CUPTI activity memory copy record to write the corresponding
 * Score-P events.
 *
 * @param mcpy the CUPTI activity memory copy record
 * @param spCtx the Score-P CUPTI activity context
 */
static void
sp_cuptiact_writeMemcpyRecord( CUpti_ActivityMemcpy* mcpy,
                               sp_cuptiact_ctx_t*    spCtx )
{
    /*vt_gpu_copy_kind_t kind = VT_GPU_COPYDIRECTION_UNKNOWN;*/

    SCOREP_Location*    strmLoc = NULL;
    uint64_t            start, stop;
    sp_cuptiact_strm_t* spStrm = NULL;

    if ( mcpy->copyKind == CUPTI_ACTIVITY_MEMCPY_KIND_DTOD )
    {
        return;
    }

    start = spCtx->sync.hostStart
            + ( mcpy->start - spCtx->sync.gpuStart ) * spCtx->sync.factor;
    stop = start + ( mcpy->end - mcpy->start ) * spCtx->sync.factor;

    /* get ScoreP location for the kernel's stream */
    spStrm  = sp_cuptiact_checkStream( spCtx, mcpy->streamId );
    strmLoc = spStrm->spLoc;

    /* if current activity's start time is before last written timestamp */
    if ( start < spStrm->spLastTime )
    {
        SCOREP_WARN_ONCE( "[CUPTI Activity] Memcpy: start time < last written timestamp!" );
        return;
    }

    /* check if time between start and stop is increasing */
    if ( stop < start )
    {
        SCOREP_WARN_ONCE( "[CUPTI Activity] Memcpy: start time > stop time!" );
        return;
    }

    /* check if synchronization stop time is before kernel stop time */
    if ( spCtx->sync.hostStop < stop )
    {
        SCOREP_WARN_ONCE( "[CUPTI Activity] Memcpy: sync stop time < stop time!" );
        return;
    }

    /* set the last Score-P timestamp, written in this stream */
    spStrm->spLastTime = stop;

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

    if ( spCtx->gpuIdleOn == 0 && mcpy->streamId == spCtx->defaultStrmID )
    {
        SCOREP_Location_EnterRegion( spCtx->strmList->spLoc,
                                     spCtx->spLastGPUTime,
                                     sp_cudaIdleRegionHandle );
        spCtx->gpuIdleOn = 1;
    }

    /* TODO: write the ScoreP communication events
       SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI Activity] "
                             "MEMCPY %llu -> %llu[%llu ns] device %u, context %u, stream %u, "
                             "correlation %u/r%u \n",
                             mcpy->start, mcpy->end,
                             ( unsigned long long )( mcpy->end - mcpy->start ),
                             mcpy->deviceId, mcpy->contextId, mcpy->streamId,
                             mcpy->correlationId, mcpy->runtimeCorrelationId );*/
}

/*
 * Puts a string into the CUPTI activity hash table
 *
 * @param n pointer to a string to be stored in the hash table
 *
 * @return pointer to the created hash node
 */
static void*
sp_cuptiact_stringHashPut( const char* n, SCOREP_RegionHandle rh )
{
    uint32_t id = ( uint32_t )hashStringSDBM( n ) % SP_CUPTIACT_HASHTABLE_SIZE;

    sp_cuptiact_hn_string_t* add =
        ( sp_cuptiact_hn_string_t* )SCOREP_Memory_AllocForMisc( sizeof( sp_cuptiact_hn_string_t ) );

    add->sname                    = SCOREP_CStr_dup( n ); /* does an implicit malloc */
    add->regionHandle             = rh;
    add->next                     = sp_cuptiact_string_htab[ id ];
    sp_cuptiact_string_htab[ id ] = add;

    return add;
}

/*
 * Get a string from the CUPTI Activity hash table
 *
 * @param n pointer to a string to be retrieved from the hash table
 *
 * @return pointer to the retrieved hash node
 */
static void*
sp_cuptiact_stringHashGet( const char* n )
{
    uint32_t id = ( uint32_t )hashStringSDBM( n )  % SP_CUPTIACT_HASHTABLE_SIZE;

    sp_cuptiact_hn_string_t* curr = sp_cuptiact_string_htab[ id ];

    while ( curr )
    {
        if ( strcmp( curr->sname, n ) == 0 )
        {
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}

/*
 * Clear the CUPTI Activity hash table (free allocated memory).
 */
static void
sp_cuptiact_stringhashClear()
{
    int                      i;
    sp_cuptiact_hn_string_t* tmp_node;

    for ( i = 0; i < SP_CUPTIACT_HASHTABLE_SIZE; i++ )
    {
        while ( sp_cuptiact_string_htab[ i ] )
        {
            tmp_node = sp_cuptiact_string_htab[ i ]->next;
            free( sp_cuptiact_string_htab[ i ]->sname );
            //free( sp_cuptiact_string_htab[ i ] );
            sp_cuptiact_string_htab[ i ] = tmp_node;
        }
    }
}

/******************************************************************************/
