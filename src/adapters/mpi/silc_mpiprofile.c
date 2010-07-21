/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file        silc_mpiprofile.c
 * @maintainer  Yury Olenyik <oleynik@in.tum.de>
 *
 * @brief   Declaration of MPI profiling functions
 *
 *
 *
 */


#include <stdlib.h>
#include <stdio.h>

#include <SILC_Debug.h>
#include <mpi.h>

#include "silc_mpiprofile.h"

//#include "silc_runtime_management.h"



static int64_t lateThreshold;
int            myrank;
static int     numprocs;
static int     mpiprofiling_initialized = 0;

/**
 * Initializes MPI profiling module
 */
void
silc_mpiprofile_init
(
)
{
    if ( mpiprofiling_initialized )
    {
        return;
    }

    PMPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    PMPI_Comm_rank( MPI_COMM_WORLD, &myrank );
    //printf("INIT: myrank = %d, numprocs = %d\n",myrank,numprocs);

    lateThreshold = 0.001;

    /// @TODO synchronize clocks here

    mpiprofiling_initialized = 1;
}

/**
 * Creates time pack buffer containing rank and time stamp
 *
 */
void*
silc_mpiprofile_get_time_pack
(
    uint64_t time
)
{
    //printf("mpiprofile : myrank = %d,%s \n",myrank,__FUNCTION__);
    void* buf = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
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
 * Evaluates two time packs for p2p communications
 *
 */
void
silc_mpiprofile_eval_1x1_time_packs
(
    void* srcTimePack,
    void* dstTimePack
)
{
    //printf("mpiprofile : myrank = %d,%s \n",myrank,__FUNCTION__);
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
    silc_mpiprofile_eval_time_stamps(       src,
                                            dst,
                                            sendTime,
                                            recvTime );
}

/**
 * Evaluates multiple time packs for Nx1 communications
 *
 */
void
silc_mpiprofile_eval_nx1_time_packs
(
    void* timePacks,
    int   size
)
{
    //printf("mpiprofile : myrank = %d,%s \n",myrank,__FUNCTION__);
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
    silc_mpiprofile_eval_1x1_time_packs(    dstTimePack,
                                            srcTimePack );
}

/**
 * Evaluates multiple time packs for NxN communications
 *
 */
void
silc_mpiprofile_eval_multi_time_packs
(
    void* srcTimePacks,
    void* dstTimePack,
    int   size
)
{
    //printf("mpiprofile : myrank = %d,%s \n",myrank,__FUNCTION__);
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
    silc_mpiprofile_eval_1x1_time_packs(    srcTimePack,
                                            dstTimePack );
}

/**
 * Evaluates two time stamps of the communicating processes to determine waiting states
 *
 */
void
silc_mpiprofile_eval_time_stamps
(
    int      src,
    int      dst,
    uint64_t sendTime,
    uint64_t recvTime
)
{
    //printf("mpiprofile : myrank = %d,%s \n",myrank,__FUNCTION__);
    if ( src == dst )
    {
        return;
    }

    int64_t delta = recvTime - sendTime;

    if ( delta > mpiprofiling_get_late_threshold() )
    {
        //printf( "LATE RECEIVE: myrank=%d, src/dst = (%d/%d) Delta = %ld = %ld-%ld\n", myrank, src, dst, delta, recvTime, sendTime );
        ///receive process is late: store EARLY_SEND/LATE_RECEIVE=delta value for the remote side, currently not supported
        ///trigger user metric here
    }
    else if ( delta < -mpiprofiling_get_late_threshold() )
    {
        //printf( "LATE SENDER: myrank=%d, src/dst = (%d/%d) Delta = %ld = %ld-%ld\n", myrank, src, dst, delta, recvTime, sendTime );
        ///sending process is late: store LATE_SEND/EARLY_RECEIVE=-delta value on the current process
        ///trigger user metric here
    }
    else
    {
        //printf( "IN TIME: myrank=%d, src/dst = (%d/%d) Delta = %ld = %ld-%ld\n", myrank, src, dst, delta, recvTime, sendTime );
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
