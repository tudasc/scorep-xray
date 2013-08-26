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
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


/**
 * @file       SCOREP_Fmpi_Coll.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    MPI_Wrapper
 *
 * @brief Fortran interface wrappers for collective communication
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

/* uppercase defines */
/** @def MPI_Allgather_U
    Exchange MPI_Allgather_U by MPI_ALLGATHER.
    It is used for the Fortran wrappers of MPI_Allgather.
 */
#define MPI_Allgather_U MPI_ALLGATHER

/** @def MPI_Allgatherv_U
    Exchange MPI_Allgatherv_U by MPI_ALLGATHERV.
    It is used for the Fortran wrappers of MPI_Allgatherv.
 */
#define MPI_Allgatherv_U MPI_ALLGATHERV

/** @def MPI_Allreduce_U
    Exchange MPI_Allreduce_U by MPI_ALLREDUCE.
    It is used for the Fortran wrappers of MPI_Allreduce.
 */
#define MPI_Allreduce_U MPI_ALLREDUCE

/** @def MPI_Alltoall_U
    Exchange MPI_Alltoall_U by MPI_ALLTOALL.
    It is used for the Fortran wrappers of MPI_Alltoall.
 */
#define MPI_Alltoall_U MPI_ALLTOALL

/** @def MPI_Alltoallv_U
    Exchange MPI_Alltoallv_U by MPI_ALLTOALLV.
    It is used for the Fortran wrappers of MPI_Alltoallv.
 */
#define MPI_Alltoallv_U MPI_ALLTOALLV

/** @def MPI_Alltoallw_U
    Exchange MPI_Alltoallw_U by MPI_ALLTOALLW.
    It is used for the Fortran wrappers of MPI_Alltoallw.
 */
#define MPI_Alltoallw_U MPI_ALLTOALLW

/** @def MPI_Barrier_U
    Exchange MPI_Barrier_U by MPI_BARRIER.
    It is used for the Fortran wrappers of MPI_Barrier.
 */
#define MPI_Barrier_U MPI_BARRIER

/** @def MPI_Bcast_U
    Exchange MPI_Bcast_U by MPI_BCAST.
    It is used for the Fortran wrappers of MPI_Bcast.
 */
#define MPI_Bcast_U MPI_BCAST

/** @def MPI_Exscan_U
    Exchange MPI_Exscan_U by MPI_EXSCAN.
    It is used for the Fortran wrappers of MPI_Exscan.
 */
#define MPI_Exscan_U MPI_EXSCAN

/** @def MPI_Gather_U
    Exchange MPI_Gather_U by MPI_GATHER.
    It is used for the Fortran wrappers of MPI_Gather.
 */
#define MPI_Gather_U MPI_GATHER

/** @def MPI_Gatherv_U
    Exchange MPI_Gatherv_U by MPI_GATHERV.
    It is used for the Fortran wrappers of MPI_Gatherv.
 */
#define MPI_Gatherv_U MPI_GATHERV

/** @def MPI_Reduce_U
    Exchange MPI_Reduce_U by MPI_REDUCE.
    It is used for the Fortran wrappers of MPI_Reduce.
 */
#define MPI_Reduce_U MPI_REDUCE

/** @def MPI_Reduce_local_U
    Exchange MPI_Reduce_local_U by MPI_REDUCE_LOCAL.
    It is used for the Fortran wrappers of MPI_Reduce_local.
 */
#define MPI_Reduce_local_U MPI_REDUCE_LOCAL

/** @def MPI_Reduce_scatter_U
    Exchange MPI_Reduce_scatter_U by MPI_REDUCE_SCATTER.
    It is used for the Fortran wrappers of MPI_Reduce_scatter.
 */
#define MPI_Reduce_scatter_U MPI_REDUCE_SCATTER

/** @def MPI_Reduce_scatter_block_U
    Exchange MPI_Reduce_scatter_block_U by MPI_REDUCE_SCATTER_BLOCK.
    It is used for the Fortran wrappers of MPI_Reduce_scatter_block.
 */
#define MPI_Reduce_scatter_block_U MPI_REDUCE_SCATTER_BLOCK

/** @def MPI_Scan_U
    Exchange MPI_Scan_U by MPI_SCAN.
    It is used for the Fortran wrappers of MPI_Scan.
 */
#define MPI_Scan_U MPI_SCAN

/** @def MPI_Scatter_U
    Exchange MPI_Scatter_U by MPI_SCATTER.
    It is used for the Fortran wrappers of MPI_Scatter.
 */
#define MPI_Scatter_U MPI_SCATTER

/** @def MPI_Scatterv_U
    Exchange MPI_Scatterv_U by MPI_SCATTERV.
    It is used for the Fortran wrappers of MPI_Scatterv.
 */
#define MPI_Scatterv_U MPI_SCATTERV


/* lowercase defines */
/** @def MPI_Allgather_L
    Exchanges MPI_Allgather_L by mpi_allgather.
    It is used for the Fortran wrappers of MPI_Allgather.
 */
#define MPI_Allgather_L mpi_allgather

/** @def MPI_Allgatherv_L
    Exchanges MPI_Allgatherv_L by mpi_allgatherv.
    It is used for the Fortran wrappers of MPI_Allgatherv.
 */
#define MPI_Allgatherv_L mpi_allgatherv

/** @def MPI_Allreduce_L
    Exchanges MPI_Allreduce_L by mpi_allreduce.
    It is used for the Fortran wrappers of MPI_Allreduce.
 */
#define MPI_Allreduce_L mpi_allreduce

/** @def MPI_Alltoall_L
    Exchanges MPI_Alltoall_L by mpi_alltoall.
    It is used for the Fortran wrappers of MPI_Alltoall.
 */
#define MPI_Alltoall_L mpi_alltoall

/** @def MPI_Alltoallv_L
    Exchanges MPI_Alltoallv_L by mpi_alltoallv.
    It is used for the Fortran wrappers of MPI_Alltoallv.
 */
#define MPI_Alltoallv_L mpi_alltoallv

/** @def MPI_Alltoallw_L
    Exchanges MPI_Alltoallw_L by mpi_alltoallw.
    It is used for the Fortran wrappers of MPI_Alltoallw.
 */
#define MPI_Alltoallw_L mpi_alltoallw

/** @def MPI_Barrier_L
    Exchanges MPI_Barrier_L by mpi_barrier.
    It is used for the Fortran wrappers of MPI_Barrier.
 */
#define MPI_Barrier_L mpi_barrier

/** @def MPI_Bcast_L
    Exchanges MPI_Bcast_L by mpi_bcast.
    It is used for the Fortran wrappers of MPI_Bcast.
 */
#define MPI_Bcast_L mpi_bcast

/** @def MPI_Exscan_L
    Exchanges MPI_Exscan_L by mpi_exscan.
    It is used for the Fortran wrappers of MPI_Exscan.
 */
#define MPI_Exscan_L mpi_exscan

/** @def MPI_Gather_L
    Exchanges MPI_Gather_L by mpi_gather.
    It is used for the Fortran wrappers of MPI_Gather.
 */
#define MPI_Gather_L mpi_gather

/** @def MPI_Gatherv_L
    Exchanges MPI_Gatherv_L by mpi_gatherv.
    It is used for the Fortran wrappers of MPI_Gatherv.
 */
#define MPI_Gatherv_L mpi_gatherv

/** @def MPI_Reduce_L
    Exchanges MPI_Reduce_L by mpi_reduce.
    It is used for the Fortran wrappers of MPI_Reduce.
 */
#define MPI_Reduce_L mpi_reduce

/** @def MPI_Reduce_local_L
    Exchanges MPI_Reduce_local_L by mpi_reduce_local.
    It is used for the Fortran wrappers of MPI_Reduce_local.
 */
#define MPI_Reduce_local_L mpi_reduce_local

/** @def MPI_Reduce_scatter_L
    Exchanges MPI_Reduce_scatter_L by mpi_reduce_scatter.
    It is used for the Fortran wrappers of MPI_Reduce_scatter.
 */
#define MPI_Reduce_scatter_L mpi_reduce_scatter

/** @def MPI_Reduce_scatter_block_L
    Exchanges MPI_Reduce_scatter_block_L by mpi_reduce_scatter_block.
    It is used for the Fortran wrappers of MPI_Reduce_scatter_block.
 */
#define MPI_Reduce_scatter_block_L mpi_reduce_scatter_block

/** @def MPI_Scan_L
    Exchanges MPI_Scan_L by mpi_scan.
    It is used for the Fortran wrappers of MPI_Scan.
 */
#define MPI_Scan_L mpi_scan

/** @def MPI_Scatter_L
    Exchanges MPI_Scatter_L by mpi_scatter.
    It is used for the Fortran wrappers of MPI_Scatter.
 */
#define MPI_Scatter_L mpi_scatter

/** @def MPI_Scatterv_L
    Exchanges MPI_Scatterv_L by mpi_scatterv.
    It is used for the Fortran wrappers of MPI_Scatterv.
 */
#define MPI_Scatterv_L mpi_scatterv


/**
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if HAVE( DECL_PMPI_ALLGATHER )
/**
 * Measurement wrapper for MPI_Allgather
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Allgather )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Allgather( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm );
}
#endif
#if HAVE( DECL_PMPI_ALLGATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allgatherv )
/**
 * Measurement wrapper for MPI_Allgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Allgatherv )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* displs, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Allgatherv( sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *comm );
}
#endif
#if HAVE( DECL_PMPI_ALLREDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allreduce )
/**
 * Measurement wrapper for MPI_Allreduce
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Allreduce )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Allreduce( sendbuf, recvbuf, *count, *datatype, *op, *comm );
}
#endif
#if HAVE( DECL_PMPI_ALLTOALL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoall )
/**
 * Measurement wrapper for MPI_Alltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Alltoall )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Alltoall( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm );
}
#endif
#if HAVE( DECL_PMPI_ALLTOALLV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoallv )
/**
 * Measurement wrapper for MPI_Alltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Alltoallv )( void* sendbuf, int* sendcounts, int* sdispls, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* rdispls, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Alltoallv( sendbuf, sendcounts, sdispls, *sendtype, recvbuf, recvcounts, rdispls, *recvtype, *comm );
}
#endif
#if HAVE( DECL_PMPI_ALLTOALLW ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoallw )
/**
 * Measurement wrapper for MPI_Alltoallw
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup coll
 */
void
FSUB( MPI_Alltoallw )( void* sendbuf, int sendcounts[], int sdispls[], MPI_Datatype sendtypes[], void* recvbuf, int recvcounts[], int rdispls[], MPI_Datatype recvtypes[], MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Alltoallw( sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, *comm );
}
#endif
#if HAVE( DECL_PMPI_BARRIER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Barrier )
/**
 * Measurement wrapper for MPI_Barrier
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Barrier )( MPI_Comm* comm, int* ierr )
{
    *ierr = MPI_Barrier( *comm );
}
#endif
#if HAVE( DECL_PMPI_BCAST ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Bcast )
/**
 * Measurement wrapper for MPI_Bcast
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Bcast )( void* buffer, int* count, MPI_Datatype* datatype, int* root, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_BOTTOM )
    if ( buffer == scorep_mpi_fortran_bottom )
    {
        buffer = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Bcast( buffer, *count, *datatype, *root, *comm );
}
#endif
#if HAVE( DECL_PMPI_EXSCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Exscan )
/**
 * Measurement wrapper for MPI_Exscan
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup coll
 */
void
FSUB( MPI_Exscan )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Exscan( sendbuf, recvbuf, *count, *datatype, *op, *comm );
}
#endif
#if HAVE( DECL_PMPI_GATHER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gather )
/**
 * Measurement wrapper for MPI_Gather
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Gather )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Gather( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm );
}
#endif
#if HAVE( DECL_PMPI_GATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gatherv )
/**
 * Measurement wrapper for MPI_Gatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Gatherv )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* displs, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Gatherv( sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *root, *comm );
}
#endif
#if HAVE( DECL_PMPI_REDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce )
/**
 * Measurement wrapper for MPI_Reduce
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Reduce )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, int* root, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Reduce( sendbuf, recvbuf, *count, *datatype, *op, *root, *comm );
}
#endif
#if HAVE( DECL_PMPI_REDUCE_LOCAL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_local )
/**
 * Measurement wrapper for MPI_Reduce_local
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup coll
 */
void
FSUB( MPI_Reduce_local )( void* inbuf, void* inoutbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, int* ierr )
{
    *ierr = MPI_Reduce_local( inbuf, inoutbuf, *count, *datatype, *op );
}
#endif
#if HAVE( DECL_PMPI_REDUCE_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter )
/**
 * Measurement wrapper for MPI_Reduce_scatter
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Reduce_scatter )( void* sendbuf, void* recvbuf, int* recvcounts, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Reduce_scatter( sendbuf, recvbuf, recvcounts, *datatype, *op, *comm );
}
#endif
#if HAVE( DECL_PMPI_REDUCE_SCATTER_BLOCK ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter_block )
/**
 * Measurement wrapper for MPI_Reduce_scatter_block
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup coll
 */
void
FSUB( MPI_Reduce_scatter_block )( void* sendbuf, void* recvbuf, int* recvcount, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Reduce_scatter_block( sendbuf, recvbuf, *recvcount, *datatype, *op, *comm );
}
#endif
#if HAVE( DECL_PMPI_SCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scan )
/**
 * Measurement wrapper for MPI_Scan
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Scan )( void* sendbuf, void* recvbuf, int* count, MPI_Datatype* datatype, MPI_Op* op, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Scan( sendbuf, recvbuf, *count, *datatype, *op, *comm );
}
#endif
#if HAVE( DECL_PMPI_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatter )
/**
 * Measurement wrapper for MPI_Scatter
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Scatter )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Scatter( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm );
}
#endif
#if HAVE( DECL_PMPI_SCATTERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatterv )
/**
 * Measurement wrapper for MPI_Scatterv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 */
void
FSUB( MPI_Scatterv )( void* sendbuf, int* sendcounts, int* displs, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* root, MPI_Comm* comm, int* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Scatterv( sendbuf, sendcounts, displs, *sendtype, recvbuf, *recvcount, *recvtype, *root, *comm );
}
#endif

#else /* !NEED_F2C_CONV */

#if HAVE( DECL_PMPI_ALLGATHER )
/**
 * Measurement wrapper for MPI_Allgather
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Allgather
 */
void
FSUB( MPI_Allgather )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Allgather( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_ALLGATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allgatherv )
/**
 * Measurement wrapper for MPI_Allgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Allgatherv
 */
void
FSUB( MPI_Allgatherv )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Allgatherv( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, displs, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_ALLREDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allreduce )
/**
 * Measurement wrapper for MPI_Allreduce
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Allreduce
 */
void
FSUB( MPI_Allreduce )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Allreduce( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_ALLTOALL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoall )
/**
 * Measurement wrapper for MPI_Alltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Alltoall
 */
void
FSUB( MPI_Alltoall )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Alltoall( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_ALLTOALLV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoallv )
/**
 * Measurement wrapper for MPI_Alltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Alltoallv
 */
void
FSUB( MPI_Alltoallv )( void* sendbuf, MPI_Fint* sendcounts, MPI_Fint* sdispls, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* rdispls, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Alltoallv( sendbuf, sendcounts, sdispls, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, rdispls, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_BARRIER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Barrier )
/**
 * Measurement wrapper for MPI_Barrier
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Barrier
 */
void
FSUB( MPI_Barrier )( MPI_Fint* comm, MPI_Fint* ierr )
{
    *ierr = MPI_Barrier( PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_BCAST ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Bcast )
/**
 * Measurement wrapper for MPI_Bcast
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Bcast
 */
void
FSUB( MPI_Bcast )( void* buffer, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_BOTTOM )
    if ( buffer == scorep_mpi_fortran_bottom )
    {
        buffer = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Bcast( buffer, *count, PMPI_Type_f2c( *datatype ), *root, PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_GATHER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gather )
/**
 * Measurement wrapper for MPI_Gather
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Gather
 */
void
FSUB( MPI_Gather )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Gather( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_GATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gatherv )
/**
 * Measurement wrapper for MPI_Gatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Gatherv
 */
void
FSUB( MPI_Gatherv )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Gatherv( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, displs, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_REDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce )
/**
 * Measurement wrapper for MPI_Reduce
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Reduce
 */
void
FSUB( MPI_Reduce )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Reduce( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), *root, PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_REDUCE_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter )
/**
 * Measurement wrapper for MPI_Reduce_scatter
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Reduce_scatter
 */
void
FSUB( MPI_Reduce_scatter )( void* sendbuf, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Reduce_scatter( sendbuf, recvbuf, recvcounts, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_SCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scan )
/**
 * Measurement wrapper for MPI_Scan
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Scan
 */
void
FSUB( MPI_Scan )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Scan( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatter )
/**
 * Measurement wrapper for MPI_Scatter
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Scatter
 */
void
FSUB( MPI_Scatter )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Scatter( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_SCATTERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatterv )
/**
 * Measurement wrapper for MPI_Scatterv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup coll
 * For the order of events see @ref MPI_Scatterv
 */
void
FSUB( MPI_Scatterv )( void* sendbuf, MPI_Fint* sendcounts, MPI_Fint* displs, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* root, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( recvbuf == scorep_mpi_fortran_in_place )
    {
        recvbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Scatterv( sendbuf, sendcounts, displs, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *root, PMPI_Comm_f2c( *comm ) );
}
#endif

#if HAVE( DECL_PMPI_ALLTOALLW ) && !defined( SCOREP_MPI_NO_COLL )
/**
 * Measurement wrapper for MPI_Alltoallv
 * @note Manually created wrapper
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup coll
 */
void
FSUB( MPI_Alltoallw )( void*     sendbuf,
                       MPI_Fint* sendcounts,
                       MPI_Fint* sdispls,
                       MPI_Fint* sendtypes,
                       void*     recvbuf,
                       MPI_Fint* recvcounts,
                       MPI_Fint* rdispls,
                       MPI_Fint* recvtypes,
                       MPI_Fint* comm,
                       MPI_Fint* ierr )
{
    MPI_Datatype* csendtypes;
    MPI_Datatype* crecvtypes;
    MPI_Comm      ccomm;
    int           size;

  #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
  #endif
  #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
  #endif
  #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
  #endif

    ccomm = PMPI_Comm_f2c( *comm );
    PMPI_Comm_size( ccomm, &size );

    csendtypes = malloc( size * sizeof( MPI_Datatype ) );
    crecvtypes = malloc( size * sizeof( MPI_Datatype ) );

    while ( size > 0 )
    {
        csendtypes[ size - 1 ] = PMPI_Type_f2c( sendtypes[ size - 1 ] );
        crecvtypes[ size - 1 ] = PMPI_Type_f2c( recvtypes[ size - 1 ] );
        --size;
    }

    *ierr = MPI_Alltoallw( sendbuf, sendcounts, sdispls, csendtypes, recvbuf,
                           recvcounts, rdispls, crecvtypes, ccomm );

    free( csendtypes );
    free( crecvtypes );
}
#endif

#if HAVE( DECL_PMPI_EXSCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Exscan )
/**
 * Measurement wrapper for MPI_Exscan
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup coll
 * For the order of events see @ref MPI_Exscan
 */
void
FSUB( MPI_Exscan )( void* sendbuf, void* recvbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Exscan( sendbuf, recvbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ) );
}
#endif
#if HAVE( DECL_PMPI_REDUCE_LOCAL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_local )
/**
 * Measurement wrapper for MPI_Reduce_local
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup coll
 * For the order of events see @ref MPI_Reduce_local
 */
void
FSUB( MPI_Reduce_local )( void* inbuf, void* inoutbuf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* ierr )
{
    *ierr = MPI_Reduce_local( inbuf, inoutbuf, *count, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ) );
}
#endif
#if HAVE( DECL_PMPI_REDUCE_SCATTER_BLOCK ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter_block )
/**
 * Measurement wrapper for MPI_Reduce_scatter_block
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup coll
 * For the order of events see @ref MPI_Reduce_scatter_block
 */
void
FSUB( MPI_Reduce_scatter_block )( void* sendbuf, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* datatype, MPI_Fint* op, MPI_Fint* comm, MPI_Fint* ierr )
{
    #if HAVE( MPI_IN_PLACE )
    if ( sendbuf == scorep_mpi_fortran_in_place )
    {
        sendbuf = MPI_IN_PLACE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Reduce_scatter_block( sendbuf, recvbuf, *recvcount, PMPI_Type_f2c( *datatype ), PMPI_Op_f2c( *op ), PMPI_Comm_f2c( *comm ) );
}
#endif
#endif

/**
 * @}
 */
