/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2017,
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


#ifndef SCOREP_MPIHOOKS_H
#define SCOREP_MPIHOOKS_H


/**
 * @file
 *
 * @brief   Declaration of MPI pre and post communication hooks
 *
 */

#include <mpi.h>

#include "SCOREP_Mpi.h"

#include <SCOREP_Types.h>
#include "scorep_mpi_request_mgmt.h"

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
SCOREP_Hooks_Pre_MPI_Send( SCOREP_MPI_CONST_DECL void* buf,
                           int                         count,
                           MPI_Datatype                datatype,
                           int                         dest,
                           int                         tag,
                           MPI_Comm                    comm,
                           uint64_t                    start_time_stamp );

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
SCOREP_Hooks_Post_MPI_Send( SCOREP_MPI_CONST_DECL void* buf,
                            int                         count,
                            MPI_Datatype                datatype,
                            int                         dest,
                            int                         tag,
                            MPI_Comm                    comm,
                            uint64_t                    start_time_stamp,
                            int                         return_val );

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
SCOREP_Hooks_Post_MPI_Bsend( SCOREP_MPI_CONST_DECL void* buf,
                             int                         count,
                             MPI_Datatype                datatype,
                             int                         dest,
                             int                         tag,
                             MPI_Comm                    comm,
                             uint64_t                    start_time_stamp,
                             int                         return_val );

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
SCOREP_Hooks_Post_MPI_Rsend( SCOREP_MPI_CONST_DECL void* buf,
                             int                         count,
                             MPI_Datatype                datatype,
                             int                         dest,
                             int                         tag,
                             MPI_Comm                    comm,
                             uint64_t                    start_time_stamp,
                             int                         return_val );

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
SCOREP_Hooks_Post_MPI_Ssend( SCOREP_MPI_CONST_DECL void* buf,
                             int                         count,
                             MPI_Datatype                datatype,
                             int                         dest,
                             int                         tag,
                             MPI_Comm                    comm,
                             uint64_t                    start_time_stamp,
                             int                         return_val );

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
SCOREP_Hooks_Pre_MPI_Recv( void*        buf,
                           int          count,
                           MPI_Datatype datatype,
                           int          source,
                           int          tag,
                           MPI_Comm     comm,
                           MPI_Status*  status,
                           uint64_t     start_time_stamp );

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
SCOREP_Hooks_Post_MPI_Recv( void*        buf,
                            int          count,
                            MPI_Datatype datatype,
                            int          source,
                            int          tag,
                            MPI_Comm     comm,
                            MPI_Status*  status,
                            uint64_t     start_time_stamp,
                            int          return_val );

void
SCOREP_Hooks_Post_MPI_Isend( SCOREP_MPI_CONST_DECL void* buf,
                             int                         count,
                             MPI_Datatype                datatype,
                             int                         dest,
                             int                         tag,
                             MPI_Comm                    comm,
                             MPI_Request*                request,
                             int64_t                     start_time_stamp,
                             int                         return_val );

void
SCOREP_Hooks_Post_MPI_Issend( SCOREP_MPI_CONST_DECL void* buf,
                              int                         count,
                              MPI_Datatype                datatype,
                              int                         dest,
                              int                         tag,
                              MPI_Comm                    comm,
                              MPI_Request*                request,
                              int64_t                     start_time_stamp,
                              int                         return_val );

void
SCOREP_Hooks_Post_MPI_Ibsend( SCOREP_MPI_CONST_DECL void* buf,
                              int                         count,
                              MPI_Datatype                datatype,
                              int                         dest,
                              int                         tag,
                              MPI_Comm                    comm,
                              MPI_Request*                request,
                              int64_t                     start_time_stamp,
                              int                         return_val );

void
SCOREP_Hooks_Post_MPI_Irsend( SCOREP_MPI_CONST_DECL void* buf,
                              int                         count,
                              MPI_Datatype                datatype,
                              int                         dest,
                              int                         tag,
                              MPI_Comm                    comm,
                              MPI_Request*                request,
                              int64_t                     start_time_stamp,
                              int                         return_val );

void
SCOREP_Hooks_Post_MPI_Irecv( void*        buf,
                             int          count,
                             MPI_Datatype datatype,
                             int          source,
                             int          tag,
                             MPI_Comm     comm,
                             MPI_Request* request,
                             int64_t      start_time_stamp,
                             int          return_val );

void
SCOREP_Hooks_Post_MPI_Send_init( SCOREP_MPI_CONST_DECL void* buf,
                                 int                         count,
                                 MPI_Datatype                datatype,
                                 int                         dest,
                                 int                         tag,
                                 MPI_Comm                    comm,
                                 MPI_Request*                request,
                                 int64_t                     start_time_stamp,
                                 int                         return_val );

void
SCOREP_Hooks_Post_MPI_Ssend_init( SCOREP_MPI_CONST_DECL void* buf,
                                  int                         count,
                                  MPI_Datatype                datatype,
                                  int                         dest,
                                  int                         tag,
                                  MPI_Comm                    comm,
                                  MPI_Request*                request,
                                  int64_t                     start_time_stamp,
                                  int                         return_val );

void
SCOREP_Hooks_Post_MPI_Rsend_init( SCOREP_MPI_CONST_DECL void* buf,
                                  int                         count,
                                  MPI_Datatype                datatype,
                                  int                         dest,
                                  int                         tag,
                                  MPI_Comm                    comm,
                                  MPI_Request*                request,
                                  int64_t                     start_time_stamp,
                                  int                         return_val );

void
SCOREP_Hooks_Post_MPI_Bsend_init( SCOREP_MPI_CONST_DECL void* buf,
                                  int                         count,
                                  MPI_Datatype                datatype,
                                  int                         dest,
                                  int                         tag,
                                  MPI_Comm                    comm,
                                  MPI_Request*                request,
                                  int64_t                     start_time_stamp,
                                  int                         return_val );

void
SCOREP_Hooks_Post_MPI_Recv_init( void*        buf,
                                 int          count,
                                 MPI_Datatype datatype,
                                 int          source,
                                 int          tag,
                                 MPI_Comm     comm,
                                 MPI_Request* request,
                                 int64_t      start_time_stamp,
                                 int          return_val );

void
SCOREP_Hooks_Post_MPI_Start( MPI_Request* request,
                             int64_t      start_time_stamp,
                             int          return_val );

void
SCOREP_Hooks_Pre_MPI_Request_free( scorep_mpi_request* scorep_req );

void
SCOREP_Hooks_Post_MPI_Cancel( scorep_mpi_request* scorep_req );

void
SCOREP_Hooks_Post_MPI_Asynch_Complete( scorep_mpi_request* orig_req,
                                       MPI_Status*         status,
                                       int64_t             start_time_stamp );

void
SCOREP_Hooks_Post_MPI_Asynch_Complete_Blocking( scorep_mpi_request* orig_req,
                                                MPI_Status*         status,
                                                int64_t             start_time_stamp );

/*----------------------------------------------
 * NxN pre- and post- communication hooks
   -----------------------------------------------*/

void
SCOREP_Hooks_Post_MPI_Alltoall( SCOREP_MPI_CONST_DECL void* sendbuf,
                                int                         sendcount,
                                MPI_Datatype                sendtype,
                                void*                       recvbuf,
                                int                         recvcount,
                                MPI_Datatype                recvtype,
                                MPI_Comm                    comm,
                                uint64_t                    start_time_stamp,
                                int                         return_val );

void
SCOREP_Hooks_Post_MPI_Alltoallv( SCOREP_MPI_CONST_DECL void* sendbuf,
                                 SCOREP_MPI_CONST_DECL int*  sendcounts,
                                 SCOREP_MPI_CONST_DECL int*  sdispls,
                                 MPI_Datatype                sendtype,
                                 void*                       recvbuf,
                                 SCOREP_MPI_CONST_DECL int*  recvcounts,
                                 SCOREP_MPI_CONST_DECL int*  rdispls,
                                 MPI_Datatype                recvtype,
                                 MPI_Comm                    comm,
                                 uint64_t                    start_time_stamp,
                                 int                         return_val );

void
SCOREP_Hooks_Post_MPI_Barrier( MPI_Comm comm,
                               uint64_t start_time_stamp,
                               int      return_val );

void
SCOREP_Hooks_Post_MPI_Exscan( SCOREP_MPI_CONST_DECL void* sendbuf,
                              void*                       recvbuf,
                              int                         count,
                              MPI_Datatype                datatype,
                              MPI_Op                      op,
                              MPI_Comm                    comm,
                              uint64_t                    start_time_stamp,
                              int                         return_val );

void
SCOREP_Hooks_Post_MPI_Alltoallw( SCOREP_MPI_CONST_DECL void*        sendbuf,
                                 SCOREP_MPI_CONST_DECL int          sendcounts[],
                                 SCOREP_MPI_CONST_DECL int          sdispls[],
                                 SCOREP_MPI_CONST_DECL MPI_Datatype sendtypes[],
                                 void*                              recvbuf,
                                 SCOREP_MPI_CONST_DECL int          recvcounts[],
                                 SCOREP_MPI_CONST_DECL int          rdispls[],
                                 SCOREP_MPI_CONST_DECL MPI_Datatype recvtypes[],
                                 MPI_Comm                           comm,
                                 uint64_t                           start_time_stamp,
                                 int                                return_val );

void
SCOREP_Hooks_Post_MPI_Reduce_scatter( SCOREP_MPI_CONST_DECL void* sendbuf,
                                      void*                       recvbuf,
                                      SCOREP_MPI_CONST_DECL int*  recvcounts,
                                      MPI_Datatype                datatype,
                                      MPI_Op                      op,
                                      MPI_Comm                    comm,
                                      uint64_t                    start_time_stamp,
                                      int                         return_val );

void
SCOREP_Hooks_Post_MPI_Reduce_scatter_block( SCOREP_MPI_CONST_DECL void* sendbuf,
                                            void*                       recvbuf,
                                            int                         recvcount,
                                            MPI_Datatype                datatype,
                                            MPI_Op                      op,
                                            MPI_Comm                    comm,
                                            uint64_t                    start_time_stamp,
                                            int                         return_val );

void
SCOREP_Hooks_Post_MPI_Scan( SCOREP_MPI_CONST_DECL void* sendbuf,
                            void*                       recvbuf,
                            int                         count,
                            MPI_Datatype                datatype,
                            MPI_Op                      op,
                            MPI_Comm                    comm,
                            uint64_t                    start_time_stamp,
                            int                         return_val );

void
SCOREP_Hooks_Post_MPI_Scatterv( SCOREP_MPI_CONST_DECL void* sendbuf,
                                SCOREP_MPI_CONST_DECL int*  sendcounts,
                                SCOREP_MPI_CONST_DECL int*  displs,
                                MPI_Datatype                sendtype,
                                void*                       recvbuf,
                                int                         recvcount,
                                MPI_Datatype                recvtype,
                                int                         root,
                                MPI_Comm                    comm,
                                uint64_t                    start_time_stamp,
                                int                         return_val );

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
SCOREP_Hooks_Pre_MPI_Gather( SCOREP_MPI_CONST_DECL void* sendbuf,
                             int                         sendcount,
                             MPI_Datatype                sendtype,
                             void*                       recvbuf,
                             int                         recvcount,
                             MPI_Datatype                recvtype,
                             int                         root,
                             MPI_Comm                    comm,
                             uint64_t                    start_time_stamp );

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
SCOREP_Hooks_Post_MPI_Gather( SCOREP_MPI_CONST_DECL void* sendbuf,
                              int                         sendcount,
                              MPI_Datatype                sendtype,
                              void*                       recvbuf,
                              int                         recvcount,
                              MPI_Datatype                recvtype,
                              int                         root,
                              MPI_Comm                    comm,
                              uint64_t                    start_time_stamp,
                              int                         return_val );

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
SCOREP_Hooks_Post_MPI_Gatherv( SCOREP_MPI_CONST_DECL void* sendbuf,
                               int                         sendcount,
                               MPI_Datatype                sendtype,
                               void*                       recvbuf,
                               SCOREP_MPI_CONST_DECL int*  recvcounts,
                               SCOREP_MPI_CONST_DECL int*  displs,
                               MPI_Datatype                recvtype,
                               int                         root,
                               MPI_Comm                    comm,
                               uint64_t                    start_time_stamp,
                               int                         return_val );

void
SCOREP_Hooks_Post_MPI_Reduce( SCOREP_MPI_CONST_DECL void* sendbuf,
                              void*                       recvbuf,
                              int                         count,
                              MPI_Datatype                datatype,
                              MPI_Op                      op,
                              int                         root,
                              MPI_Comm                    comm,
                              uint64_t                    start_time_stamp,
                              int                         return_val );

void
SCOREP_Hooks_Post_MPI_Allreduce( SCOREP_MPI_CONST_DECL void* sendbuf,
                                 void*                       recvbuf,
                                 int                         count,
                                 MPI_Datatype                datatype,
                                 MPI_Op                      op,
                                 MPI_Comm                    comm,
                                 uint64_t                    start_time_stamp,
                                 int                         return_val );

void
SCOREP_Hooks_Post_MPI_Allgather( SCOREP_MPI_CONST_DECL void* sendbuf,
                                 int                         sendcount,
                                 MPI_Datatype                sendtype,
                                 void*                       recvbuf,
                                 int                         recvcount,
                                 MPI_Datatype                recvtype,
                                 MPI_Comm                    comm,
                                 uint64_t                    start_time_stamp,
                                 int                         return_val );

void
SCOREP_Hooks_Post_MPI_Allgatherv( SCOREP_MPI_CONST_DECL void* sendbuf,
                                  int                         sendcount,
                                  MPI_Datatype                sendtype,
                                  void*                       recvbuf,
                                  SCOREP_MPI_CONST_DECL int*  recvcounts,
                                  SCOREP_MPI_CONST_DECL int*  displs,
                                  MPI_Datatype                recvtype,
                                  MPI_Comm                    comm,
                                  uint64_t                    start_time_stamp,
                                  int                         return_val );

/*----------------------------------------------
 * 1xN pre- and post- communication hooks
   -----------------------------------------------*/

void
SCOREP_Hooks_Post_MPI_Bcast( void*        buffer,
                             int          count,
                             MPI_Datatype datatype,
                             int          root,
                             MPI_Comm     comm,
                             uint64_t     start_time_stamp,
                             int          return_val );

void
SCOREP_Hooks_Post_MPI_Scatter( SCOREP_MPI_CONST_DECL void* sendbuf,
                               int                         sendcount,
                               MPI_Datatype                sendtype,
                               void*                       recvbuf,
                               int                         recvcount,
                               MPI_Datatype                recvtype,
                               int                         root,
                               MPI_Comm                    comm,
                               uint64_t                    start_time_stamp,
                               int                         return_val );



#endif /* SCOREP_MPIHOOKS_H */
