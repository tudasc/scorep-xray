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

#include <scorep_utility/SCOREP_Debug.h>
#include <SCOREP_Config.h>
#include <SCOREP_Types.h>
#include <SCOREP_Mpi.h>

#define SCOREP_USER_ENABLE
#include <scorep/SCOREP_User.h>

#include "scorep_mpiprofile.h"

//#include "scorep_runtime_management.h"



static int64_t lateThreshold;
int            myrank;
static int     numprocs;
static int     mpiprofiling_initialized = 0;

static void*   mpi_profiling_remote_time_packs_pool;
static void*   mpi_profiling_local_time_pack;
static void*   mpi_profiling_remote_time_pack;
static int     remote_time_packs_in_use = 0;
static int     local_time_pack_in_use   = 0;
static int     remote_time_pack_in_use  = 0;

SCOREP_USER_METRIC_LOCAL( lateSend )
SCOREP_USER_METRIC_LOCAL( lateRecv )

#define SCOREP_MPI_PROFILED_DEFAULT = 0

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

    PMPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    PMPI_Comm_rank( MPI_COMM_WORLD, &myrank );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "INIT: myrank = %d, numprocs = %d", myrank, numprocs );

        #ifdef _WITH_PREALLOCATION_OF_TIME_PACKS
    mpi_profiling_local_time_pack        = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    mpi_profiling_remote_time_pack       = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    mpi_profiling_remote_time_packs_pool = malloc( numprocs * MPIPROFILER_TIMEPACK_BUFSIZE );
    if ( mpi_profiling_remote_time_packs_pool == NULL
         || mpi_profiling_local_time_pack == NULL
         || mpi_profiling_remote_time_pack == NULL )
    {
        fprintf( stderr, "MPI_Profiling not enough memory. MPI_Profiling will be disabled." );
        SCOREP_MPI_HOOKS_SET( false );
        return;
    }
        #endif

    lateThreshold = 0.001;

    SCOREP_USER_METRIC_INIT( lateSend, "lateSend", "s", SCOREP_USER_METRIC_TYPE_INT64,
                             SCOREP_USER_METRIC_CONTEXT_CALLPATH, SCOREP_USER_METRIC_GROUP_DEFAULT );
    SCOREP_USER_METRIC_INIT( lateRecv, "lateRecv", "s", SCOREP_USER_METRIC_TYPE_INT64,
                             SCOREP_USER_METRIC_CONTEXT_CALLPATH, SCOREP_USER_METRIC_GROUP_DEFAULT );

    mpiprofiling_initialized = 1;
}

/**
 * Creates time pack buffer containing rank and time stamp
 *
 */
void*
scorep_mpiprofile_get_time_pack
(
    uint64_t time
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "mpiprofile : myrank = %d,%s", myrank, __FUNCTION__ );
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
    int   pos = 0;
    PMPI_Pack(      &time,
                    1,
                    MPI_LONG,
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
        #ifdef _WITH_PREALLOCATION_OF_TIME_PACKS
    if ( remote_time_packs_in_use == 1 )
    {
        fprintf( stderr, "2 Warning attempt of multiple use of time packs pool. MPI_Profiling will be disabled.\n" );
        return malloc( size * MPIPROFILER_TIMEPACK_BUFSIZE );
    }
    remote_time_packs_in_use = 1;
    return mpi_profiling_remote_time_packs_pool;
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "mpiprofile : myrank = %d,%s", myrank, __FUNCTION__ );
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
                    MPI_LONG,
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
                    MPI_LONG,
                    MPI_COMM_WORLD );

    PMPI_Unpack(    dstTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    &pos,
                    &dst,
                    1,
                    MPI_INT,
                    MPI_COMM_WORLD );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "mpiprofile : myrank = %d,%s", myrank, __FUNCTION__ );
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
                        MPI_LONG,
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "mpiprofile : myrank = %d,%s", myrank, __FUNCTION__ );
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
                        MPI_LONG,
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "mpiprofile : myrank = %d,%s", myrank, __FUNCTION__ );
    if ( src == dst )
    {
        return;
    }

    int64_t delta = recvTime - sendTime;

    if ( delta > mpiprofiling_get_late_threshold() )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "LATE RECEIVE: myrank=%d, src/dst = (%d/%d) Delta = %ld = %ld-%ld", myrank, src, dst, delta, recvTime, sendTime );
        SCOREP_USER_METRIC_INT64( lateRecv, delta );
        ///receive process is late: store EARLY_SEND/LATE_RECEIVE=delta value for the remote side, currently not supported
        ///trigger user metric here
    }
    else if ( delta < -mpiprofiling_get_late_threshold() )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "LATE SENDER: myrank=%d, src/dst = (%d/%d) Delta = %ld = %ld-%ld", myrank, src, dst, delta, recvTime, sendTime );
        SCOREP_USER_METRIC_INT64( lateSend, -delta );
        ///sending process is late: store LATE_SEND/EARLY_RECEIVE=-delta value on the current process
        ///trigger user metric here
    }
    else
    {
        delta = abs( delta );
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPIPROFILING, "IN TIME: myrank=%d, src/dst = (%d/%d) Delta = %ld = %ld-%ld\n", myrank, src, dst, delta, recvTime, sendTime );
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
