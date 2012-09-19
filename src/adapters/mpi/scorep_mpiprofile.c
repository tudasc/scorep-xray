/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @status      alpha
 * @file        scorep_mpiprofile.c
 * @maintainer  Yury Olenyik <oleynik@in.tum.de>
 *
 * @brief   Declaration of MPI profiling functions
 *
 *
 *
 */

#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include <SCOREP_Config.h>
#include <SCOREP_Types.h>
#include <SCOREP_Mpi.h>

#define SCOREP_USER_ENABLE
#include <scorep/SCOREP_User.h>

#include "scorep_mpiprofile.h"


static int64_t lateThreshold;
int            myrank;
static int     numprocs;
static int     mpiprofiling_initialized = 0;
static int     metrics_initialized      = 0;

static void* mpi_profiling_remote_time_packs;
static void* mpi_profiling_local_time_pack;
static void* mpi_profiling_remote_time_pack;
static int   remote_time_packs_in_use = 0;
static int   local_time_pack_in_use   = 0;
static int   remote_time_pack_in_use  = 0;

static void**       send_timepack_pool = 0;
static MPI_Request* timepack_requests  = 0;
static int          timepack_pool_size = 0;

#define POOL_INITIAL_SIZE       5
#define POOL_SIZE_INCREMENT             2


scorep_mpiprofile_world_comm_dup world_comm_dup;

SCOREP_USER_METRIC_LOCAL( lateSend )
SCOREP_USER_METRIC_LOCAL( lateRecv )


#define _WITH_PREALLOCATION_OF_TIME_PACKS


/**
 * Initializes MPI profiling module
 */
void
scorep_mpiprofile_init
(
)
{
    if ( mpiprofiling_initialized )
    {
        return;
    }
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "Initialization of mpiprofiling\n" );
    /* -- duplicate MPI_COMM_WORLD to be used for mpi profiling messages -- */
    if ( PMPI_Comm_dup( MPI_COMM_WORLD, &( world_comm_dup.comm ) ) != MPI_SUCCESS )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "Could not duplicate MPI_COMM_WORLD\n" );
        return;
    }
    if ( PMPI_Comm_group( world_comm_dup.comm, &( world_comm_dup.group ) ) != MPI_SUCCESS )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "Could not get the group of the MPI_COMM_WORLD duplicate\n" );
        return;
    }

    PMPI_Comm_size( world_comm_dup.comm, &numprocs );
    PMPI_Comm_rank( world_comm_dup.comm, &myrank );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "INIT: myrank = %d, numprocs = %d", myrank, numprocs );

    /* -- allocate timepack buffers -- */
        #ifdef _WITH_PREALLOCATION_OF_TIME_PACKS
    mpi_profiling_local_time_pack   = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    mpi_profiling_remote_time_pack  = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    mpi_profiling_remote_time_packs = malloc( numprocs * MPIPROFILER_TIMEPACK_BUFSIZE );
    if ( mpi_profiling_remote_time_packs == NULL
         || mpi_profiling_local_time_pack == NULL
         || mpi_profiling_remote_time_pack == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED );
        abort();
    }
    #endif

    mpiprofiling_initialized = 1;
}

void
scorep_mpiprofile_init_metrics
(
)
{
    if ( metrics_initialized )
    {
        return;
    }
    /* -- initialize late metrics -- */
    lateThreshold = 0.001;
    SCOREP_USER_METRIC_INIT( lateSend, "late_send", "s", SCOREP_USER_METRIC_TYPE_INT64,
                             SCOREP_USER_METRIC_CONTEXT_CALLPATH );
    SCOREP_USER_METRIC_INIT( lateRecv, "late_receive", "s", SCOREP_USER_METRIC_TYPE_INT64,
                             SCOREP_USER_METRIC_CONTEXT_CALLPATH );
    metrics_initialized = 1;
}

void
scorep_mpiprofile_reinit_metrics
(
)
{
    metrics_initialized = 0;
    scorep_mpiprofile_init_metrics();
}

void
scorep_mpiprofile_finalize
(
)
{
    if ( !mpiprofiling_initialized )
    {
        return;
    }
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "Finalization of mpiprofiling\n" );
    mpiprofiling_initialized = 0;
    if ( remote_time_packs_in_use )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s: remote_time_packs_in_use is still in use\n", __func__ );
    }
    if ( local_time_pack_in_use )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s: local_time_pack_in_use is still in use\n", __func__ );
    }
    if ( remote_time_pack_in_use )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s: remote_time_pack_in_use is still in use\n", __func__ );
    }
    MPI_Status statuses[ timepack_pool_size ];
    int        flag = 0;
    PMPI_Testall( timepack_pool_size, timepack_requests, &flag, statuses );
    if ( !flag )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s: at least one timepack buffer in the pool is busy\n", __func__ );
    }
    scorep_mpiprofile_free_timepack_pool();
    free( mpi_profiling_local_time_pack );
    free( mpi_profiling_remote_time_pack );
    free( mpi_profiling_remote_time_packs );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "Finalization of mpiprofiling completed\n" );
}

/**
 * This function searches for the available time_pack buffer in the pool. It returns a pointer to this buffer and the position
 * in of this buffer in the pool. After the non-blocking send or receive using the returned buffer is initated the generated
 * request has to be stored under the same index using ...
 *
 * @param free_buffer a pointer to the pointer of the found available timepack buffer
 * @param index a pointer to the index of the available buffer
 * @return 0 if successful, 1 if MPI_Testany failed, 2 if memory allocation failed
 */
int
scorep_mpiprofile_get_timepack_from_pool( void** free_buffer, int* index )
{
    int insert_position;
    if ( !metrics_initialized )
    {
        scorep_mpiprofile_init_metrics();
    }
    if ( timepack_pool_size == 0 )
    {
        /* -- never used: initialize -- */
        send_timepack_pool = malloc( POOL_INITIAL_SIZE * sizeof( void* ) );
        timepack_requests  = malloc( POOL_INITIAL_SIZE * sizeof( MPI_Request ) );
        if ( send_timepack_pool == NULL || timepack_requests == NULL )
        {
            UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED );
            abort();
        }
        timepack_pool_size = POOL_INITIAL_SIZE;
        int i;
        for ( i = 0; i < timepack_pool_size; i++ )
        {
            timepack_requests[ i ]  = MPI_REQUEST_NULL;
            send_timepack_pool[ i ] = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
            if ( send_timepack_pool[ i ] == NULL )
            {
                UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED );
                abort();
            }
        }
        insert_position = 0;
    }
    else
    {
        /* -- check for the completed timepack send requests -- */
        int        flag;
        MPI_Status tp_status;
        if ( PMPI_Testany( timepack_pool_size, timepack_requests, &insert_position, &flag, &tp_status ) != MPI_SUCCESS )
        {
            return 1;
        }
        if ( flag )
        {
            /* -- one of the previous timepack sends was completed (or there are no pending sends) -- */
            if ( insert_position == MPI_UNDEFINED )
            {
                UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, MPI_UNDEFINED: buffer %d is available\n", __func__, insert_position );
                insert_position = 0;
            }
            else
            {
                UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, buffer %d tested available\n", __func__, insert_position );
            }
        }
        else
        {
            /* -- all the slots for timepack send buffers are busy, need to increase the pool -- */
            int old_size = timepack_pool_size;
            timepack_pool_size += POOL_SIZE_INCREMENT;
            send_timepack_pool  = realloc( send_timepack_pool, timepack_pool_size * sizeof( void* ) );
            timepack_requests   = realloc( timepack_requests, timepack_pool_size * sizeof( MPI_Request ) );
            if ( send_timepack_pool == NULL || timepack_requests == NULL )
            {
                UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED );
                abort();
            }
            int i;
            for ( i = old_size; i < timepack_pool_size; i++ )
            {
                timepack_requests[ i ]  = MPI_REQUEST_NULL;
                send_timepack_pool[ i ] = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
                if ( send_timepack_pool[ i ] == NULL )
                {
                    UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED );
                    abort();
                }
            }
            insert_position = old_size;
            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s, buffers are not available, have to increase pool size to %d\n", __func__, timepack_pool_size );
        }
    }
    ( *index )       = insert_position;
    ( *free_buffer ) = send_timepack_pool[ insert_position ];
    return 0;
}
void
scorep_mpiprofile_store_timepack_request_in_pool( MPI_Request request, int position )
{
    if ( position >= timepack_pool_size )
    {
        return;
    }
    timepack_requests[ position ] = request;
}
void
scorep_mpiprofile_free_timepack_pool()
{
    int i;
    for ( i = 0; i < timepack_pool_size; i++ )
    {
        free( send_timepack_pool[ i ] );
    }
    free( send_timepack_pool );
    free( timepack_requests );
}
void
scorep_mpiprofile_init_timepack( void* buf, uint64_t time )
{
    int pos = 0;
    if ( !metrics_initialized )
    {
        scorep_mpiprofile_init_metrics();
    }
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s: timestamp %llu\n", __func__, time );
    PMPI_Pack(      &time,
                    1,
                    MPI_LONG_LONG_INT,
                    buf,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    &pos,
                    MPI_COMM_WORLD );
    PMPI_Pack(      &myrank,
                    1,
                    MPI_INT,
                    buf,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    &pos,
                    MPI_COMM_WORLD );
}

int
scorep_mpiprofiling_rank_to_pe( int      rank,
                                MPI_Comm comm,
                                int*     global_rank )
{
    MPI_Group group;
    int32_t   inter;
    int       ret_value;

    if ( comm == MPI_COMM_WORLD )
    {
        ( *global_rank ) = rank;
        return 0;
    }
    /* inter-communicators need different call than intra-communicators */
    PMPI_Comm_test_inter( comm, &inter );
    if ( inter )
    {
        ret_value = PMPI_Comm_remote_group( comm, &group );
    }
    else
    {
        ret_value = PMPI_Comm_group( comm, &group );
    }

    if ( ret_value == MPI_ERR_COMM )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s: WARNING: the communicator is not valid\n", __func__ );
        return 2;
    }

    /* translate rank with respect to \a MPI_COMM_WORLD */
    PMPI_Group_translate_ranks( group, 1, &rank, world_comm_dup.group, global_rank );
    /* free internal group of input communicator */
    PMPI_Group_free( &group );

    if ( *global_rank == MPI_UNDEFINED )
    {
        return 1;
    }

    return 0;
}

/**
 * Returns a time pack buffer containing rank and time stamp
 *
 * @param time time stamp to be packed
 * @return a pointer to the buffer containing packed time stamp and a rank
 */
void*
scorep_mpiprofile_get_time_pack
(
    uint64_t time
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "mpiprofile : myrank = %d,%s", myrank, __func__ );

    if ( !metrics_initialized )
    {
        scorep_mpiprofile_init_metrics();
    }

    #ifdef _WITH_PREALLOCATION_OF_TIME_PACKS
    if ( local_time_pack_in_use == 1 )
    {
        fprintf( stderr, "1 Warning attempt of multiple use of time packs pool. MPI_Profiling will be disabled.\n" );
        return malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    }
    local_time_pack_in_use = 1;
    void* buf = mpi_profiling_local_time_pack;
    #else
    void* buf = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    #endif

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s: timestamp %llu\n", __func__, time );

    int pos = 0;
    PMPI_Pack(      &time,
                    1,
                    MPI_LONG_LONG_INT,
                    buf,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    &pos,
                    MPI_COMM_WORLD );
    PMPI_Pack(      &myrank,
                    1,
                    MPI_INT,
                    buf,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    &pos,
                    MPI_COMM_WORLD );


    return buf;
}

/**
 * Gives a pointer to the buffer for receiving remote timepacks
 *
 */
void*
scorep_mpiprofile_get_remote_time_packs
(
    int size
)
{
    if ( !metrics_initialized )
    {
        scorep_mpiprofile_init_metrics();
    }

    #ifdef _WITH_PREALLOCATION_OF_TIME_PACKS
    if ( remote_time_packs_in_use == 1 )
    {
        fprintf( stderr, "2 Warning attempt of multiple use of time packs pool. MPI_Profiling will be disabled.\n" );
        return malloc( size * MPIPROFILER_TIMEPACK_BUFSIZE );
    }
    remote_time_packs_in_use = 1;
    return mpi_profiling_remote_time_packs;
        #else
    return malloc( size * MPIPROFILER_TIMEPACK_BUFSIZE );
        #endif
}

/**
 * Gives a pointer to the buffer for receiving remote timepack
 *
 */
void*
scorep_mpiprofile_get_remote_time_pack
(
)
{
    if ( !metrics_initialized )
    {
        scorep_mpiprofile_init_metrics();
    }

    #ifdef _WITH_PREALLOCATION_OF_TIME_PACKS
    if ( remote_time_pack_in_use == 1 )
    {
        fprintf( stderr, "3 Warning attempt of multiple use of time packs pool. MPI_Profiling will be disabled.\n" );
        return malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    }
    remote_time_pack_in_use = 1;
    return mpi_profiling_remote_time_pack;
        #else
    return malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
        #endif
}

void
scorep_mpiprofile_release_local_time_pack
(
    void* local_time_pack
)
{
    if ( !metrics_initialized )
    {
        scorep_mpiprofile_init_metrics();
    }

    #ifdef _WITH_PREALLOCATION_OF_TIME_PACKS
    local_time_pack_in_use = 0;
        #else
    free( local_time_pack );
        #endif
}
void
scorep_mpiprofile_release_remote_time_pack
(
    void* remote_time_pack
)
{
    #ifdef _WITH_PREALLOCATION_OF_TIME_PACKS
    remote_time_pack_in_use = 0;
        #else
    free( remote_time_pack );
        #endif
}
void
scorep_mpiprofile_release_remote_time_packs
(
    void* remote_time_packs
)
{
    #ifdef _WITH_PREALLOCATION_OF_TIME_PACKS
    remote_time_packs_in_use = 0;
        #else
    free( remote_time_packs );
        #endif
}

/**
 * Evaluates two time packs for p2p communications
 *
 */
void
scorep_mpiprofile_eval_1x1_time_packs
(
    void* srcTimePack,
    void* dstTimePack
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "mpiprofile : myrank = %d,%s", myrank, __func__ );
    int      src;
    int      dst;
    uint64_t sendTime;
    uint64_t recvTime;
    int      pos = 0;
    PMPI_Unpack(    srcTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    &pos,
                    &sendTime,
                    1,
                    MPI_LONG_LONG_INT,
                    MPI_COMM_WORLD );

    PMPI_Unpack(    srcTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    &pos,
                    &src,
                    1,
                    MPI_INT,
                    MPI_COMM_WORLD );
    pos = 0;
    PMPI_Unpack(    dstTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    &pos,
                    &recvTime,
                    1,
                    MPI_LONG_LONG_INT,
                    MPI_COMM_WORLD );

    PMPI_Unpack(    dstTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    &pos,
                    &dst,
                    1,
                    MPI_INT,
                    MPI_COMM_WORLD );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "%s: timestamps: (send %llu) (recv %llu)\n", __func__, sendTime, recvTime );
    scorep_mpiprofile_eval_time_stamps(       src,
                                              dst,
                                              sendTime,
                                              recvTime );
}

/**
 * Evaluates multiple time packs for Nx1 communications
 *
 */
void
scorep_mpiprofile_eval_nx1_time_packs
(
    void* timePacks,
    int   size
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "mpiprofile : myrank = %d,%s", myrank, __func__ );
    int      src;
    int      dst;
    uint64_t sendTime;
    uint64_t recvTime;
    void*    srcTimePack, * dstTimePack;
    int      pos, last, source, dest;
    uint64_t lastTime = 0;

    last = -1;
    for ( source = 0; source < size; source++ )
    {
        srcTimePack = ( void* )( ( long )timePacks + source * MPIPROFILER_TIMEPACK_BUFSIZE );
        pos         = 0;
        PMPI_Unpack(    srcTimePack,
                        MPIPROFILER_TIMEPACK_BUFSIZE,
                        &pos,
                        &sendTime,
                        1,
                        MPI_LONG_LONG_INT,
                        MPI_COMM_WORLD );

        PMPI_Unpack(    srcTimePack,
                        MPIPROFILER_TIMEPACK_BUFSIZE,
                        &pos,
                        &src,
                        1,
                        MPI_INT,
                        MPI_COMM_WORLD );
        if (    last == -1 ||
                sendTime > lastTime )
        {
            lastTime = sendTime;
            last     = source;
        }
    }

    srcTimePack = ( void* )( ( long )timePacks + last * MPIPROFILER_TIMEPACK_BUFSIZE );
    dstTimePack = ( void* )( ( long )timePacks + myrank * MPIPROFILER_TIMEPACK_BUFSIZE );
    scorep_mpiprofile_eval_1x1_time_packs(    dstTimePack,
                                              srcTimePack );
}

/**
 * Evaluates multiple time packs for NxN communications
 *
 */
void
scorep_mpiprofile_eval_multi_time_packs
(
    void* srcTimePacks,
    void* dstTimePack,
    int   size
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "mpiprofile : myrank = %d,%s", myrank, __func__ );
    int      src;
    int      dst;
    uint64_t sendTime;
    uint64_t recvTime;
    void*    srcTimePack;
    int      pos, last, source;
    uint64_t lastTime;

    last = -1;
    for ( source = 0; source < size; source++ )
    {
        srcTimePack = ( void* )( ( long )srcTimePacks + source * MPIPROFILER_TIMEPACK_BUFSIZE );
        pos         = 0;
        PMPI_Unpack(    srcTimePack,
                        MPIPROFILER_TIMEPACK_BUFSIZE,
                        &pos,
                        &sendTime,
                        1,
                        MPI_LONG_LONG_INT,
                        MPI_COMM_WORLD );
        PMPI_Unpack(    srcTimePack,
                        MPIPROFILER_TIMEPACK_BUFSIZE,
                        &pos,
                        &src,
                        1,
                        MPI_INT,
                        MPI_COMM_WORLD );
        if (    last == -1 ||
                sendTime > lastTime )
        {
            lastTime = sendTime;
            last     = source;
        }
    }

    srcTimePack = ( void* )( ( long )srcTimePacks + last * MPIPROFILER_TIMEPACK_BUFSIZE );
    scorep_mpiprofile_eval_1x1_time_packs(    srcTimePack,
                                              dstTimePack );
}

/**
 * Evaluates two time stamps of the communicating processes to determine waiting states
 *
 */
void
scorep_mpiprofile_eval_time_stamps
(
    int      src,
    int      dst,
    uint64_t sendTime,
    uint64_t recvTime
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "mpiprofile : myrank = %d,%s", myrank, __func__ );
    if ( src == dst )
    {
        return;
    }

    int64_t delta = recvTime - sendTime;

    if ( delta > mpiprofiling_get_late_threshold() )
    {
        //UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "LATE RECEIVE: myrank=%d, src/dst = (%d/%d) Delta = %ld = %ld-%ld", myrank, src, dst, delta, recvTime, sendTime );
        //SCOREP_USER_METRIC_INT64( lateRecv, delta );
        ///receive process is late: store EARLY_SEND/LATE_RECEIVE=delta value for the remote side, currently not supported
        ///trigger user metric here
    }
    else if ( delta < -mpiprofiling_get_late_threshold() )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "LATE SENDER: myrank=%d, src/dst = (%d/%d) Delta = %ld = %ld-%ld", myrank, src, dst, delta, recvTime, sendTime );
        SCOREP_USER_METRIC_INT64( lateSend, -delta );
        ///sending process is late: store LATE_SEND/EARLY_RECEIVE=-delta value on the current process
        ///trigger user metric here
    }
    else
    {
        delta = abs( delta );
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "IN TIME: myrank=%d, src/dst = (%d/%d) Delta = %ld = %ld-%ld\n", myrank, src, dst, delta, recvTime, sendTime );
        ///no late state
        ///trigger user metric here
    }
}

int64_t
mpiprofiling_get_late_threshold
(
)
{
    return lateThreshold;
}

void
mpiprofiling_set_late_threshold
(
    int64_t newThreshold
)
{
    lateThreshold = newThreshold;
}
