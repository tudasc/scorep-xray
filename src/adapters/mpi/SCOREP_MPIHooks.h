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


#ifndef SCOREP_MPIHOOKS_H
#define SCOREP_MPIHOOKS_H


/**
 * @file        SCOREP_MPIHooks.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief   Declaration of MPI pre and post communication hooks
 *
 * @status ALPHA
 */

#include <mpi.h>

#include "SCOREP_Types.h"
#include "SCOREP_Mpi_Request.h"

void
scorep_mpiprofile_init
(
);

void
scorep_mpiprofile_finalize
(
);


/*----------------------------------------------
 * 1x1 pre- and post- communication hooks
   -----------------------------------------------*/

/**
 * Pre-communication hook for MPI_Send
 *
 * @param buf Buffer to be sent.
 *
 * @param count Number of elements to be sent.
 *
 * @param datatype Data type of the elements to be sent.
 *
 * @param dest Rank of the destination process.
 *
 * @param tag Tag of the communication.
 *
 * @param comm Communicator.
 *
 * @param start_time_stamp time stamp recorded at region enter event of the MPI_Send operation
 */
void
SCOREP_Hooks_Pre_MPI_Send
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp
);

/**
 * Post-communication hook for MPI_Send
 *
 * @param buf Buffer to be sent.
 *
 * @param count Number of elements to be sent.
 *
 * @param datatype Data type of the elements to be sent.
 *
 * @param dest Rank of the destination process.
 *
 * @param tag Tag of the communication.
 *
 * @param comm Communicator.
 *
 * @param start_time_stamp time stamp recorded at region enter event of the MPI_Send operation
 *
 * @param return_val return value of MPI_Send
 */
void
SCOREP_Hooks_Post_MPI_Send
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
);

/**
 * Post-communication hook for MPI_Bsend
 *
 * @param buf Buffer to be sent.
 *
 * @param count Number of elements to be sent.
 *
 * @param datatype Data type of the elements to be sent.
 *
 * @param dest Rank of the destination process.
 *
 * @param tag Tag of the communication.
 *
 * @param comm Communicator.
 *
 * @param start_time_stamp time stamp recorded at region enter event of the MPI_Bsend operation
 *
 * @param return_val return value of MPI_Bsend
 */
void
SCOREP_Hooks_Post_MPI_Bsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
);

/**
 * Post-communication hook for MPI_Rsend
 *
 * @param buf Buffer to be sent.
 *
 * @param count Number of elements to be sent.
 *
 * @param datatype Data type of the elements to be sent.
 *
 * @param dest Rank of the destination process.
 *
 * @param tag Tag of the communication.
 *
 * @param comm Communicator.
 *
 * @param start_time_stamp time stamp recorded at region enter event of the MPI_Rsend operation
 *
 * @param return_val return value of MPI_Rsend
 */
void
SCOREP_Hooks_Post_MPI_Rsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
);

/**
 * Post-communication hook for MPI_Ssend
 *
 * @param buf Buffer to be sent.
 *
 * @param count Number of elements to be sent.
 *
 * @param datatype Data type of the elements to be sent.
 *
 * @param dest Rank of the destination process.
 *
 * @param tag Tag of the communication.
 *
 * @param comm Communicator.
 *
 * @param start_time_stamp time stamp recorded at region enter event of the MPI_Ssend operation
 *
 * @param return_val return value of MPI_Ssend
 */
void
SCOREP_Hooks_Post_MPI_Ssend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
);

/**
 * Pre-communication hook for MPI_Recv
 *
 * @param buf Buffer to be received.
 *
 * @param count Number of elements to be received.
 *
 * @param datatype Data type of the elements to be received.
 *
 * @param dest Rank of the source process.
 *
 * @param tag Tag of the communication.
 *
 * @param comm Communicator.
 *
 * @param start_time_stamp time stamp recorded at region enter event of the MPI_Recv operation
 *
 */
void
SCOREP_Hooks_Pre_MPI_Recv
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Status*  status,
    uint64_t     start_time_stamp
);

/**
 * Post-communication hook for MPI_Recv
 *
 * @param buf Buffer to be received.
 *
 * @param count Number of elements to be received.
 *
 * @param datatype Data type of the elements to be received.
 *
 * @param dest Rank of the source process.
 *
 * @param tag Tag of the communication.
 *
 * @param comm Communicator.
 *
 * @param start_time_stamp time stamp recorded at region enter event of the MPI_Recv operation
 *
 * @param return_val return value of MPI_Recv
 */
void
SCOREP_Hooks_Post_MPI_Recv
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
);

void
SCOREP_Hooks_Post_MPI_Isend
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
);

void
SCOREP_Hooks_Post_MPI_Issend
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
);

void
SCOREP_Hooks_Post_MPI_Ibsend
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
);

void
SCOREP_Hooks_Post_MPI_Irsend
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
);

void
SCOREP_Hooks_Post_MPI_Irecv
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
);

void
SCOREP_Hooks_Post_MPI_Send_init
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
);
void
SCOREP_Hooks_Post_MPI_Ssend_init
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
);
void
SCOREP_Hooks_Post_MPI_Rsend_init
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
);
void
SCOREP_Hooks_Post_MPI_Bsend_init
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
);
void
SCOREP_Hooks_Post_MPI_Recv_init
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
);
void
SCOREP_Hooks_Post_MPI_Start
(
    MPI_Request* request,
    int64_t      start_time_stamp,
    int          return_val
);

void
SCOREP_Hooks_Pre_MPI_Request_free
(
    scorep_mpi_request* scorep_req
);

void
SCOREP_Hooks_Post_MPI_Cancel
(
    scorep_mpi_request* scorep_req
);

void
SCOREP_Hooks_Post_MPI_Asynch_Complete
(
    scorep_mpi_request* orig_req,
    MPI_Status*         status,
    int64_t             start_time_stamp
);

void
SCOREP_Hooks_Post_MPI_Asynch_Complete_Blocking
(
    scorep_mpi_request* orig_req,
    MPI_Status*         status,
    int64_t             start_time_stamp
);

/*----------------------------------------------
 * NxN pre- and post- communication hooks
   -----------------------------------------------*/

void
SCOREP_Hooks_Post_MPI_Alltoall
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
);

void
SCOREP_Hooks_Post_MPI_Alltoallv
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
);

void
SCOREP_Hooks_Post_MPI_Barrier
(
    MPI_Comm comm,
    uint64_t start_time_stamp,
    int      return_val
);

void
SCOREP_Hooks_Post_MPI_Exscan
(
    void*        sendbuf,
    void*        recvbuf,
    int          count,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
);

void
SCOREP_Hooks_Post_MPI_Alltoallw
(
    void* sendbuf,
    int sendcounts[],
    int sdispls[],
    MPI_Datatype sendtypes[],
    void* recvbuf,
    int recvcounts[],
    int rdispls[],
    MPI_Datatype recvtypes[],
    MPI_Comm comm,
    uint64_t start_time_stamp,
    int return_val
);

void
SCOREP_Hooks_Post_MPI_Reduce_scatter
(
    void*        sendbuf,
    void*        recvbuf,
    int*         recvcounts,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
);

void
SCOREP_Hooks_Post_MPI_Reduce_scatter_block
(
    void*        sendbuf,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
);

void
SCOREP_Hooks_Post_MPI_Scan
(
    void*        sendbuf,
    void*        recvbuf,
    int          count,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
);

void
SCOREP_Hooks_Post_MPI_Scatterv
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

);

/*----------------------------------------------
 * Nx1 pre- and post- communication hooks
   -----------------------------------------------*/

/**
 * Pre-communication hook for MPI_Gather
 *
 * @param senbuf Buffer to be sent.
 *
 * @param sendcount Number of elements to be sent.
 *
 * @param senttype Data type of the elements to be sent.
 *
 * @param recvbuf Buffer to be received in.
 *
 * @param recvcount Number of elements to be received.
 *
 * @param recvtype Data type of elements to be received.
 *
 * @param root root process rank;
 *
 * @param comm Communicator.
 *
 * @param start_time_stamp time stamp recorded at region enter event of the MPI_Gather operation
 *
 */
void
SCOREP_Hooks_Pre_MPI_Gather
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp
);

/**
 * Post-communication hook for MPI_Gather
 *
 * @param senbuf Buffer to be sent.
 *
 * @param sendcount Number of elements to be sent.
 *
 * @param senttype Data type of the elements to be sent.
 *
 * @param recvbuf Buffer to be received in.
 *
 * @param recvcount Number of elements to be received.
 *
 * @param recvtype Data type of elements to be received.
 *
 * @param root root process rank;
 *
 * @param comm Communicator.
 *
 * @param start_time_stamp time stamp recorded at region enter event of the MPI_Gather operation
 *
 * @param return_val return value of MPI_Gather
 */
void
SCOREP_Hooks_Post_MPI_Gather
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
);

/**
 * Post-communication hook for MPI_Gatherv
 *
 * @param senbuf Buffer to be sent.
 *
 * @param sendcount Number of elements to be sent.
 *
 * @param senttype Data type of the elements to be sent.
 *
 * @param recvbuf Buffer to be received in.
 *
 * @param recvcount Number of elements to be received.
 *
 * @param displs Displacement.
 *
 * @param recvtype Data type of elements to be received.
 *
 * @param root root process rank;
 *
 * @param comm Communicator.
 *
 * @param start_time_stamp time stamp recorded at region enter event of the MPI_Gatherv operation
 *
 * @param return_val return value of MPI_Gatherv
 */
void
SCOREP_Hooks_Post_MPI_Gatherv
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
);

void
SCOREP_Hooks_Post_MPI_Reduce
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
);

void
SCOREP_Hooks_Post_MPI_Allreduce
(
    void*        sendbuf,
    void*        recvbuf,
    int          count,
    MPI_Datatype datatype,
    MPI_Op       op,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
);

void
SCOREP_Hooks_Post_MPI_Allgather
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
);

void
SCOREP_Hooks_Post_MPI_Allgatherv
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
);

/*----------------------------------------------
 * 1xN pre- and post- communication hooks
   -----------------------------------------------*/

void
SCOREP_Hooks_Post_MPI_Bcast
(
    void*        buffer,
    int          count,
    MPI_Datatype datatype,
    int          root,
    MPI_Comm     comm,
    uint64_t     start_time_stamp,
    int          return_val
);

void
SCOREP_Hooks_Post_MPI_Scatter
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
);



#endif /* SCOREP_MPIHOOKS_H */
