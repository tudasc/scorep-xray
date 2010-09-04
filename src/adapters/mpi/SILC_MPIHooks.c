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
 * @file        SILC_MPIHooks.c
 * @maintainer  Yury Olenyik <oleynik@in.tum.de>
 *
 * @brief   Declaration of mpi hooks functions to be used by MPI profiling
 *
 * @status alpha
 *
 */

#include <config.h>

#include "SILC_MPIHooks.h"
#include "silc_utility/SILC_Debug.h"
#include "silc_mpiprofile.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*----------------------------------------------
 * 1x1 pre- and post- communication hooks
   -----------------------------------------------*/

int32_t silc_hooks_on = 1;

/**
 * Pre-communication hook for MPI_Send
 */
void
SILC_Hooks_Pre_MPI_Send
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp
)
{
}

/**
 * Post-communication hook for MPI_Send
 */
void
SILC_Hooks_Post_MPI_Send
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_value
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    PMPI_Send(  localTimePack,
                MPIPROFILER_TIMEPACK_BUFSIZE,
                MPI_PACKED,
                dest,
                tag,
                comm );
    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Bsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    PMPI_Send(  localTimePack,
                MPIPROFILER_TIMEPACK_BUFSIZE,
                MPI_PACKED,
                dest,
                tag,
                comm );
    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Ssend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    PMPI_Send(  localTimePack,
                MPIPROFILER_TIMEPACK_BUFSIZE,
                MPI_PACKED,
                dest,
                tag,
                comm );
    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Rsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    PMPI_Send(  localTimePack,
                MPIPROFILER_TIMEPACK_BUFSIZE,
                MPI_PACKED,
                dest,
                tag,
                comm );
    free( localTimePack );
}

/**
 * Pre-communication hook for MPI_Recv
 */
void
SILC_Hooks_Pre_MPI_Recv
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Status*  status,
    uint64_t     start_time_stamp
)
{
}

/**
 * Post-communication hook for MPI_Recv
 */
void
SILC_Hooks_Post_MPI_Recv
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Status*  status,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    source = status->MPI_SOURCE;
    tag    = status->MPI_TAG;
    void*      remoteTimePack = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    MPI_Status s;
    PMPI_Recv(      remoteTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    source,
                    tag,
                    comm,
                    &s );

    silc_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                         localTimePack );
    free( remoteTimePack );
    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Isend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
}

void
SILC_Hooks_Post_MPI_Issend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
}

void
SILC_Hooks_Post_MPI_Ibsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
}

void
SILC_Hooks_Post_MPI_Irsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
)
{
}

void
SILC_Hooks_Post_MPI_Exscan
(
    void*        sendbuf,
    void*        recvbuf,
    int          count,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
}

void
SILC_Hooks_Post_MPI_Alltoallw
(
    void*        sendbuf,
    int          sendcounts[],
    int          sdispls[],
    MPI_Datatype sendtypes[],
    void*        recvbuf,
    int          recvcounts[],
    int          rdispls[],
    MPI_Datatype recvtypes[],
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
}

void
SILC_Hooks_Post_MPI_Reduce_scatter
(
    void*        sendbuf,
    void*        recvbuf,
    int*         recvcounts,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
}

void
SILC_Hooks_Post_MPI_Reduce_scatter_block
(
    void*        sendbuf,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
}

void
SILC_Hooks_Post_MPI_Scan
(
    void*        sendbuf,
    void*        recvbuf,
    int          count,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
}

void
SILC_Hooks_Post_MPI_Scatterv
(
    void*        sendbuf,
    int*         sendcounts,
    int*         displs,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val

)
{
}
/*----------------------------------------------
 * NxN pre- and post- communication hooks
   -----------------------------------------------*/

void
SILC_Hooks_Post_MPI_Alltoall
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    MPI_Comm_size( comm, &commSize );
    remoteTimePacks = malloc( commSize * MPIPROFILER_TIMEPACK_BUFSIZE );
    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE, \
                    MPI_PACKED,
                    comm );
    silc_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                            localTimePack,
                                            commSize );

    free( remoteTimePacks );
    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Alltoallv
(
    void*        sendbuf,
    int*         sendcounts,
    int*         sdispls,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int*         recvcounts,
    int*         rdispls,
    MPI_Datatype recvtype,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    MPI_Comm_size( comm, &commSize );
    remoteTimePacks = malloc( commSize * MPIPROFILER_TIMEPACK_BUFSIZE );
    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE, \
                    MPI_PACKED,
                    comm );
    silc_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                            localTimePack,
                                            commSize );

    free( remoteTimePacks );
    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Barrier
(
    MPI_Comm comm,
    uint64_t start_time_stamp,
    int      return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    MPI_Comm_size( comm, &commSize );
    remoteTimePacks = malloc( commSize * MPIPROFILER_TIMEPACK_BUFSIZE );
    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE, \
                    MPI_PACKED,
                    comm );
    silc_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                            localTimePack,
                                            commSize );
    free( remoteTimePacks );
    free( localTimePack );
}

/*----------------------------------------------
 * Nx1 pre- and post- communication hooks
   -----------------------------------------------*/

/**
 * Post-communication hook for MPI_Recv
 */
void
SILC_Hooks_Post_MPI_Gather
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    if ( myrank == root )
    {
        PMPI_Comm_size( comm, &commSize );
        remoteTimePacks = malloc( commSize * MPIPROFILER_TIMEPACK_BUFSIZE );
    }
    PMPI_Gather(    localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );

    if ( myrank == root )
    {
        silc_mpiprofile_eval_nx1_time_packs( remoteTimePacks,
                                             commSize );
        free( remoteTimePacks );
    }
    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Gatherv
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int*         recvcounts,
    int*         displs,
    MPI_Datatype recvtype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    if ( myrank == root )
    {
        PMPI_Comm_size( comm, &commSize );
        remoteTimePacks = malloc( commSize * MPIPROFILER_TIMEPACK_BUFSIZE );
    }
    PMPI_Gather(    localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );

    if ( myrank == root )
    {
        silc_mpiprofile_eval_nx1_time_packs( remoteTimePacks,
                                             commSize );
        free( remoteTimePacks );
    }
    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Reduce
(
    void*        sendbuf,
    void*        recvbuf,
    int          count,
    MPI_Datatype datatype,
    MPI_Op       op,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    PMPI_Comm_size( comm, &commSize );
    remoteTimePacks = malloc( commSize * MPIPROFILER_TIMEPACK_BUFSIZE );

    PMPI_Gather(    localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );

    if ( myrank == root )
    {
        silc_mpiprofile_eval_nx1_time_packs( remoteTimePacks,
                                             commSize );
        free( remoteTimePacks );
    }
    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Allreduce
(
    void*        sendbuf,
    void*        recvbuf,
    int          count,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    PMPI_Comm_size( comm, &commSize );
    remoteTimePacks = malloc( commSize * MPIPROFILER_TIMEPACK_BUFSIZE );

    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    comm );


    silc_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                            localTimePack,
                                            commSize );
    free( remoteTimePacks );

    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Allgather
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    PMPI_Comm_size( comm, &commSize );
    remoteTimePacks = malloc( commSize * MPIPROFILER_TIMEPACK_BUFSIZE );

    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    comm );


    silc_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                            localTimePack,
                                            commSize );
    free( remoteTimePacks );

    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Allgatherv
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int*         recvcounts,
    int*         displs,
    MPI_Datatype recvtype,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePacks;
    int   commSize;
    PMPI_Comm_size( comm, &commSize );
    remoteTimePacks = malloc( commSize * MPIPROFILER_TIMEPACK_BUFSIZE );

    PMPI_Allgather( localTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    remoteTimePacks,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    comm );


    silc_mpiprofile_eval_multi_time_packs(  remoteTimePacks,
                                            localTimePack,
                                            commSize );
    free( remoteTimePacks );
    free( localTimePack );
}

/*----------------------------------------------
 * 1xN pre- and post- communication hooks
   -----------------------------------------------*/

void
SILC_Hooks_Post_MPI_Bcast
(
    void*        buffer,
    int          count,
    MPI_Datatype datatype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    silc_mpiprofile_init();
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePack;
    remoteTimePack = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    memcpy( remoteTimePack, localTimePack, MPIPROFILER_TIMEPACK_BUFSIZE );
    PMPI_Bcast(     remoteTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );


    if ( myrank != root )
    {
        silc_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                             localTimePack );
    }
    free( remoteTimePack );
    free( localTimePack );
}

void
SILC_Hooks_Post_MPI_Scatter
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPIPROFILING, "HOOK : myrank = %d,%s \n", myrank, __FUNCTION__ );
    void* localTimePack = silc_mpiprofile_get_time_pack( start_time_stamp );
    void* remoteTimePack;
    remoteTimePack = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    memcpy( remoteTimePack, localTimePack, MPIPROFILER_TIMEPACK_BUFSIZE );
    PMPI_Bcast(     remoteTimePack,
                    MPIPROFILER_TIMEPACK_BUFSIZE,
                    MPI_PACKED,
                    root,
                    comm );


    if ( myrank != root )
    {
        silc_mpiprofile_eval_1x1_time_packs( remoteTimePack,
                                             localTimePack );
    }
    free( remoteTimePack );
    free( localTimePack );
}
