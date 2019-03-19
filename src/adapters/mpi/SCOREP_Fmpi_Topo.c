/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
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
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief Fortran interface wrappers for topologies
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

/* uppercase defines */
/** @def MPI_Cart_coords_U
    Exchange MPI_Cart_coords_U by MPI_CART_COORDS.
    It is used for the Fortran wrappers of MPI_Cart_coords.
 */
#define MPI_Cart_coords_U MPI_CART_COORDS

/** @def MPI_Cart_create_U
    Exchange MPI_Cart_create_U by MPI_CART_CREATE.
    It is used for the Fortran wrappers of MPI_Cart_create.
 */
#define MPI_Cart_create_U MPI_CART_CREATE

/** @def MPI_Cart_get_U
    Exchange MPI_Cart_get_U by MPI_CART_GET.
    It is used for the Fortran wrappers of MPI_Cart_get.
 */
#define MPI_Cart_get_U MPI_CART_GET

/** @def MPI_Cart_map_U
    Exchange MPI_Cart_map_U by MPI_CART_MAP.
    It is used for the Fortran wrappers of MPI_Cart_map.
 */
#define MPI_Cart_map_U MPI_CART_MAP

/** @def MPI_Cart_rank_U
    Exchange MPI_Cart_rank_U by MPI_CART_RANK.
    It is used for the Fortran wrappers of MPI_Cart_rank.
 */
#define MPI_Cart_rank_U MPI_CART_RANK

/** @def MPI_Cart_shift_U
    Exchange MPI_Cart_shift_U by MPI_CART_SHIFT.
    It is used for the Fortran wrappers of MPI_Cart_shift.
 */
#define MPI_Cart_shift_U MPI_CART_SHIFT

/** @def MPI_Cart_sub_U
    Exchange MPI_Cart_sub_U by MPI_CART_SUB.
    It is used for the Fortran wrappers of MPI_Cart_sub.
 */
#define MPI_Cart_sub_U MPI_CART_SUB

/** @def MPI_Cartdim_get_U
    Exchange MPI_Cartdim_get_U by MPI_CARTDIM_GET.
    It is used for the Fortran wrappers of MPI_Cartdim_get.
 */
#define MPI_Cartdim_get_U MPI_CARTDIM_GET

/** @def MPI_Dims_create_U
    Exchange MPI_Dims_create_U by MPI_DIMS_CREATE.
    It is used for the Fortran wrappers of MPI_Dims_create.
 */
#define MPI_Dims_create_U MPI_DIMS_CREATE

/** @def MPI_Dist_graph_create_U
    Exchange MPI_Dist_graph_create_U by MPI_DIST_GRAPH_CREATE.
    It is used for the Fortran wrappers of MPI_Dist_graph_create.
 */
#define MPI_Dist_graph_create_U MPI_DIST_GRAPH_CREATE

/** @def MPI_Dist_graph_create_adjacent_U
    Exchange MPI_Dist_graph_create_adjacent_U by MPI_DIST_GRAPH_CREATE_ADJACENT.
    It is used for the Fortran wrappers of MPI_Dist_graph_create_adjacent.
 */
#define MPI_Dist_graph_create_adjacent_U MPI_DIST_GRAPH_CREATE_ADJACENT

/** @def MPI_Dist_graph_neighbors_U
    Exchange MPI_Dist_graph_neighbors_U by MPI_DIST_GRAPH_NEIGHBORS.
    It is used for the Fortran wrappers of MPI_Dist_graph_neighbors.
 */
#define MPI_Dist_graph_neighbors_U MPI_DIST_GRAPH_NEIGHBORS

/** @def MPI_Dist_graph_neighbors_count_U
    Exchange MPI_Dist_graph_neighbors_count_U by MPI_DIST_GRAPH_NEIGHBORS_COUNT.
    It is used for the Fortran wrappers of MPI_Dist_graph_neighbors_count.
 */
#define MPI_Dist_graph_neighbors_count_U MPI_DIST_GRAPH_NEIGHBORS_COUNT

/** @def MPI_Graph_create_U
    Exchange MPI_Graph_create_U by MPI_GRAPH_CREATE.
    It is used for the Fortran wrappers of MPI_Graph_create.
 */
#define MPI_Graph_create_U MPI_GRAPH_CREATE

/** @def MPI_Graph_get_U
    Exchange MPI_Graph_get_U by MPI_GRAPH_GET.
    It is used for the Fortran wrappers of MPI_Graph_get.
 */
#define MPI_Graph_get_U MPI_GRAPH_GET

/** @def MPI_Graph_map_U
    Exchange MPI_Graph_map_U by MPI_GRAPH_MAP.
    It is used for the Fortran wrappers of MPI_Graph_map.
 */
#define MPI_Graph_map_U MPI_GRAPH_MAP

/** @def MPI_Graph_neighbors_U
    Exchange MPI_Graph_neighbors_U by MPI_GRAPH_NEIGHBORS.
    It is used for the Fortran wrappers of MPI_Graph_neighbors.
 */
#define MPI_Graph_neighbors_U MPI_GRAPH_NEIGHBORS

/** @def MPI_Graph_neighbors_count_U
    Exchange MPI_Graph_neighbors_count_U by MPI_GRAPH_NEIGHBORS_COUNT.
    It is used for the Fortran wrappers of MPI_Graph_neighbors_count.
 */
#define MPI_Graph_neighbors_count_U MPI_GRAPH_NEIGHBORS_COUNT

/** @def MPI_Graphdims_get_U
    Exchange MPI_Graphdims_get_U by MPI_GRAPHDIMS_GET.
    It is used for the Fortran wrappers of MPI_Graphdims_get.
 */
#define MPI_Graphdims_get_U MPI_GRAPHDIMS_GET

/** @def MPI_Ineighbor_allgather_U
    Exchange MPI_Ineighbor_allgather_U by MPI_INEIGHBOR_ALLGATHER.
    It is used for the Fortran wrappers of MPI_Ineighbor_allgather.
 */
#define MPI_Ineighbor_allgather_U MPI_INEIGHBOR_ALLGATHER

/** @def MPI_Ineighbor_allgatherv_U
    Exchange MPI_Ineighbor_allgatherv_U by MPI_INEIGHBOR_ALLGATHERV.
    It is used for the Fortran wrappers of MPI_Ineighbor_allgatherv.
 */
#define MPI_Ineighbor_allgatherv_U MPI_INEIGHBOR_ALLGATHERV

/** @def MPI_Ineighbor_alltoall_U
    Exchange MPI_Ineighbor_alltoall_U by MPI_INEIGHBOR_ALLTOALL.
    It is used for the Fortran wrappers of MPI_Ineighbor_alltoall.
 */
#define MPI_Ineighbor_alltoall_U MPI_INEIGHBOR_ALLTOALL

/** @def MPI_Ineighbor_alltoallv_U
    Exchange MPI_Ineighbor_alltoallv_U by MPI_INEIGHBOR_ALLTOALLV.
    It is used for the Fortran wrappers of MPI_Ineighbor_alltoallv.
 */
#define MPI_Ineighbor_alltoallv_U MPI_INEIGHBOR_ALLTOALLV

/** @def MPI_Ineighbor_alltoallw_U
    Exchange MPI_Ineighbor_alltoallw_U by MPI_INEIGHBOR_ALLTOALLW.
    It is used for the Fortran wrappers of MPI_Ineighbor_alltoallw.
 */
#define MPI_Ineighbor_alltoallw_U MPI_INEIGHBOR_ALLTOALLW

/** @def MPI_Neighbor_allgather_U
    Exchange MPI_Neighbor_allgather_U by MPI_NEIGHBOR_ALLGATHER.
    It is used for the Fortran wrappers of MPI_Neighbor_allgather.
 */
#define MPI_Neighbor_allgather_U MPI_NEIGHBOR_ALLGATHER

/** @def MPI_Neighbor_allgatherv_U
    Exchange MPI_Neighbor_allgatherv_U by MPI_NEIGHBOR_ALLGATHERV.
    It is used for the Fortran wrappers of MPI_Neighbor_allgatherv.
 */
#define MPI_Neighbor_allgatherv_U MPI_NEIGHBOR_ALLGATHERV

/** @def MPI_Neighbor_alltoall_U
    Exchange MPI_Neighbor_alltoall_U by MPI_NEIGHBOR_ALLTOALL.
    It is used for the Fortran wrappers of MPI_Neighbor_alltoall.
 */
#define MPI_Neighbor_alltoall_U MPI_NEIGHBOR_ALLTOALL

/** @def MPI_Neighbor_alltoallv_U
    Exchange MPI_Neighbor_alltoallv_U by MPI_NEIGHBOR_ALLTOALLV.
    It is used for the Fortran wrappers of MPI_Neighbor_alltoallv.
 */
#define MPI_Neighbor_alltoallv_U MPI_NEIGHBOR_ALLTOALLV

/** @def MPI_Neighbor_alltoallw_U
    Exchange MPI_Neighbor_alltoallw_U by MPI_NEIGHBOR_ALLTOALLW.
    It is used for the Fortran wrappers of MPI_Neighbor_alltoallw.
 */
#define MPI_Neighbor_alltoallw_U MPI_NEIGHBOR_ALLTOALLW

/** @def MPI_Topo_test_U
    Exchange MPI_Topo_test_U by MPI_TOPO_TEST.
    It is used for the Fortran wrappers of MPI_Topo_test.
 */
#define MPI_Topo_test_U MPI_TOPO_TEST


/* lowercase defines */
/** @def MPI_Cart_coords_L
    Exchanges MPI_Cart_coords_L by mpi_cart_coords.
    It is used for the Fortran wrappers of MPI_Cart_coords.
 */
#define MPI_Cart_coords_L mpi_cart_coords

/** @def MPI_Cart_create_L
    Exchanges MPI_Cart_create_L by mpi_cart_create.
    It is used for the Fortran wrappers of MPI_Cart_create.
 */
#define MPI_Cart_create_L mpi_cart_create

/** @def MPI_Cart_get_L
    Exchanges MPI_Cart_get_L by mpi_cart_get.
    It is used for the Fortran wrappers of MPI_Cart_get.
 */
#define MPI_Cart_get_L mpi_cart_get

/** @def MPI_Cart_map_L
    Exchanges MPI_Cart_map_L by mpi_cart_map.
    It is used for the Fortran wrappers of MPI_Cart_map.
 */
#define MPI_Cart_map_L mpi_cart_map

/** @def MPI_Cart_rank_L
    Exchanges MPI_Cart_rank_L by mpi_cart_rank.
    It is used for the Fortran wrappers of MPI_Cart_rank.
 */
#define MPI_Cart_rank_L mpi_cart_rank

/** @def MPI_Cart_shift_L
    Exchanges MPI_Cart_shift_L by mpi_cart_shift.
    It is used for the Fortran wrappers of MPI_Cart_shift.
 */
#define MPI_Cart_shift_L mpi_cart_shift

/** @def MPI_Cart_sub_L
    Exchanges MPI_Cart_sub_L by mpi_cart_sub.
    It is used for the Fortran wrappers of MPI_Cart_sub.
 */
#define MPI_Cart_sub_L mpi_cart_sub

/** @def MPI_Cartdim_get_L
    Exchanges MPI_Cartdim_get_L by mpi_cartdim_get.
    It is used for the Fortran wrappers of MPI_Cartdim_get.
 */
#define MPI_Cartdim_get_L mpi_cartdim_get

/** @def MPI_Dims_create_L
    Exchanges MPI_Dims_create_L by mpi_dims_create.
    It is used for the Fortran wrappers of MPI_Dims_create.
 */
#define MPI_Dims_create_L mpi_dims_create

/** @def MPI_Dist_graph_create_L
    Exchanges MPI_Dist_graph_create_L by mpi_dist_graph_create.
    It is used for the Fortran wrappers of MPI_Dist_graph_create.
 */
#define MPI_Dist_graph_create_L mpi_dist_graph_create

/** @def MPI_Dist_graph_create_adjacent_L
    Exchanges MPI_Dist_graph_create_adjacent_L by mpi_dist_graph_create_adjacent.
    It is used for the Fortran wrappers of MPI_Dist_graph_create_adjacent.
 */
#define MPI_Dist_graph_create_adjacent_L mpi_dist_graph_create_adjacent

/** @def MPI_Dist_graph_neighbors_L
    Exchanges MPI_Dist_graph_neighbors_L by mpi_dist_graph_neighbors.
    It is used for the Fortran wrappers of MPI_Dist_graph_neighbors.
 */
#define MPI_Dist_graph_neighbors_L mpi_dist_graph_neighbors

/** @def MPI_Dist_graph_neighbors_count_L
    Exchanges MPI_Dist_graph_neighbors_count_L by mpi_dist_graph_neighbors_count.
    It is used for the Fortran wrappers of MPI_Dist_graph_neighbors_count.
 */
#define MPI_Dist_graph_neighbors_count_L mpi_dist_graph_neighbors_count

/** @def MPI_Graph_create_L
    Exchanges MPI_Graph_create_L by mpi_graph_create.
    It is used for the Fortran wrappers of MPI_Graph_create.
 */
#define MPI_Graph_create_L mpi_graph_create

/** @def MPI_Graph_get_L
    Exchanges MPI_Graph_get_L by mpi_graph_get.
    It is used for the Fortran wrappers of MPI_Graph_get.
 */
#define MPI_Graph_get_L mpi_graph_get

/** @def MPI_Graph_map_L
    Exchanges MPI_Graph_map_L by mpi_graph_map.
    It is used for the Fortran wrappers of MPI_Graph_map.
 */
#define MPI_Graph_map_L mpi_graph_map

/** @def MPI_Graph_neighbors_L
    Exchanges MPI_Graph_neighbors_L by mpi_graph_neighbors.
    It is used for the Fortran wrappers of MPI_Graph_neighbors.
 */
#define MPI_Graph_neighbors_L mpi_graph_neighbors

/** @def MPI_Graph_neighbors_count_L
    Exchanges MPI_Graph_neighbors_count_L by mpi_graph_neighbors_count.
    It is used for the Fortran wrappers of MPI_Graph_neighbors_count.
 */
#define MPI_Graph_neighbors_count_L mpi_graph_neighbors_count

/** @def MPI_Graphdims_get_L
    Exchanges MPI_Graphdims_get_L by mpi_graphdims_get.
    It is used for the Fortran wrappers of MPI_Graphdims_get.
 */
#define MPI_Graphdims_get_L mpi_graphdims_get

/** @def MPI_Ineighbor_allgather_L
    Exchanges MPI_Ineighbor_allgather_L by mpi_ineighbor_allgather.
    It is used for the Fortran wrappers of MPI_Ineighbor_allgather.
 */
#define MPI_Ineighbor_allgather_L mpi_ineighbor_allgather

/** @def MPI_Ineighbor_allgatherv_L
    Exchanges MPI_Ineighbor_allgatherv_L by mpi_ineighbor_allgatherv.
    It is used for the Fortran wrappers of MPI_Ineighbor_allgatherv.
 */
#define MPI_Ineighbor_allgatherv_L mpi_ineighbor_allgatherv

/** @def MPI_Ineighbor_alltoall_L
    Exchanges MPI_Ineighbor_alltoall_L by mpi_ineighbor_alltoall.
    It is used for the Fortran wrappers of MPI_Ineighbor_alltoall.
 */
#define MPI_Ineighbor_alltoall_L mpi_ineighbor_alltoall

/** @def MPI_Ineighbor_alltoallv_L
    Exchanges MPI_Ineighbor_alltoallv_L by mpi_ineighbor_alltoallv.
    It is used for the Fortran wrappers of MPI_Ineighbor_alltoallv.
 */
#define MPI_Ineighbor_alltoallv_L mpi_ineighbor_alltoallv

/** @def MPI_Ineighbor_alltoallw_L
    Exchanges MPI_Ineighbor_alltoallw_L by mpi_ineighbor_alltoallw.
    It is used for the Fortran wrappers of MPI_Ineighbor_alltoallw.
 */
#define MPI_Ineighbor_alltoallw_L mpi_ineighbor_alltoallw

/** @def MPI_Neighbor_allgather_L
    Exchanges MPI_Neighbor_allgather_L by mpi_neighbor_allgather.
    It is used for the Fortran wrappers of MPI_Neighbor_allgather.
 */
#define MPI_Neighbor_allgather_L mpi_neighbor_allgather

/** @def MPI_Neighbor_allgatherv_L
    Exchanges MPI_Neighbor_allgatherv_L by mpi_neighbor_allgatherv.
    It is used for the Fortran wrappers of MPI_Neighbor_allgatherv.
 */
#define MPI_Neighbor_allgatherv_L mpi_neighbor_allgatherv

/** @def MPI_Neighbor_alltoall_L
    Exchanges MPI_Neighbor_alltoall_L by mpi_neighbor_alltoall.
    It is used for the Fortran wrappers of MPI_Neighbor_alltoall.
 */
#define MPI_Neighbor_alltoall_L mpi_neighbor_alltoall

/** @def MPI_Neighbor_alltoallv_L
    Exchanges MPI_Neighbor_alltoallv_L by mpi_neighbor_alltoallv.
    It is used for the Fortran wrappers of MPI_Neighbor_alltoallv.
 */
#define MPI_Neighbor_alltoallv_L mpi_neighbor_alltoallv

/** @def MPI_Neighbor_alltoallw_L
    Exchanges MPI_Neighbor_alltoallw_L by mpi_neighbor_alltoallw.
    It is used for the Fortran wrappers of MPI_Neighbor_alltoallw.
 */
#define MPI_Neighbor_alltoallw_L mpi_neighbor_alltoallw

/** @def MPI_Topo_test_L
    Exchanges MPI_Topo_test_L by mpi_topo_test.
    It is used for the Fortran wrappers of MPI_Topo_test.
 */
#define MPI_Topo_test_L mpi_topo_test


/**
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if HAVE( DECL_PMPI_CART_COORDS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_coords )
/**
 * Measurement wrapper for MPI_Cart_coords
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_coords )( MPI_Comm* comm, int* rank, int* maxdims, int* coords, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_coords( *comm, *rank, *maxdims, coords );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_CREATE )
/**
 * Measurement wrapper for MPI_Cart_create
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_create )( MPI_Comm* comm_old, int* ndims, int* dims, int* periods, int* reorder, MPI_Comm* comm_cart, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_create( *comm_old, *ndims, dims, periods, *reorder, comm_cart );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_get )
/**
 * Measurement wrapper for MPI_Cart_get
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_get )( MPI_Comm* comm, int* maxdims, int* dims, int* periods, int* coords, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_get( *comm, *maxdims, dims, periods, coords );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_MAP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_map )
/**
 * Measurement wrapper for MPI_Cart_map
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_map )( MPI_Comm* comm, int* ndims, int* dims, int* periods, int* newrank, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_map( *comm, *ndims, dims, periods, newrank );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_RANK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_rank )
/**
 * Measurement wrapper for MPI_Cart_rank
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_rank )( MPI_Comm* comm, int* coords, int* rank, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_rank( *comm, coords, rank );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_SHIFT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_shift )
/**
 * Measurement wrapper for MPI_Cart_shift
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_shift )( MPI_Comm* comm, int* direction, int* disp, int* rank_source, int* rank_dest, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_shift( *comm, *direction, *disp, rank_source, rank_dest );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_SUB )
/**
 * Measurement wrapper for MPI_Cart_sub
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_sub )( MPI_Comm* comm, int* remain_dims, MPI_Comm* newcomm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_sub( *comm, remain_dims, newcomm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CARTDIM_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cartdim_get )
/**
 * Measurement wrapper for MPI_Cartdim_get
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cartdim_get )( MPI_Comm* comm, int* ndims, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cartdim_get( *comm, ndims );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_DIMS_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dims_create )
/**
 * Measurement wrapper for MPI_Dims_create
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Dims_create )( int* nnodes, int* ndims, int* dims, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Dims_create( *nnodes, *ndims, dims );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_CREATE ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_create )
/**
 * Measurement wrapper for MPI_Dist_graph_create
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 */
void
FSUB( MPI_Dist_graph_create )( MPI_Comm* comm_old, int* n, int sources[], int degrees[], int destinations[], int weights[], MPI_Info* info, int* reorder, MPI_Comm* newcomm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_UNWEIGHTED )
    if ( weights == scorep_mpi_fortran_unweighted )
    {
        weights = MPI_UNWEIGHTED;
    }
    #endif


    *ierr = MPI_Dist_graph_create( *comm_old, *n, sources, degrees, destinations, weights, *info, *reorder, newcomm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_CREATE_ADJACENT ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_create_adjacent )
/**
 * Measurement wrapper for MPI_Dist_graph_create_adjacent
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 */
void
FSUB( MPI_Dist_graph_create_adjacent )( MPI_Comm* comm_old, int* indegree, int sources[], int sourceweights[], int* outdegree, int destinations[], int destweights[], MPI_Info* info, int* reorder, MPI_Comm* newcomm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_UNWEIGHTED )
    if ( sourceweights == scorep_mpi_fortran_unweighted )
    {
        sourceweights = MPI_UNWEIGHTED;
    }
    #endif
    #if HAVE( MPI_UNWEIGHTED )
    if ( destweights == scorep_mpi_fortran_unweighted )
    {
        destweights = MPI_UNWEIGHTED;
    }
    #endif


    *ierr = MPI_Dist_graph_create_adjacent( *comm_old, *indegree, sources, sourceweights, *outdegree, destinations, destweights, *info, *reorder, newcomm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_NEIGHBORS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_neighbors )
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 */
void
FSUB( MPI_Dist_graph_neighbors )( MPI_Comm* comm, int* maxindegree, int sources[], int sourceweights[], int* maxoutdegree, int destinations[], int destweights[], int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_UNWEIGHTED )
    if ( sourceweights == scorep_mpi_fortran_unweighted )
    {
        sourceweights = MPI_UNWEIGHTED;
    }
    #endif
    #if HAVE( MPI_UNWEIGHTED )
    if ( destweights == scorep_mpi_fortran_unweighted )
    {
        destweights = MPI_UNWEIGHTED;
    }
    #endif


    *ierr = MPI_Dist_graph_neighbors( *comm, *maxindegree, sources, sourceweights, *maxoutdegree, destinations, destweights );


    #if HAVE( MPI_UNWEIGHTED )
    if ( sourceweights == MPI_UNWEIGHTED )
    {
        sourceweights = scorep_mpi_fortran_unweighted;
    }
    #endif
    #if HAVE( MPI_UNWEIGHTED )
    if ( destweights == MPI_UNWEIGHTED )
    {
        destweights = scorep_mpi_fortran_unweighted;
    }
    #endif

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_NEIGHBORS_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_neighbors_count )
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors_count
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 */
void
FSUB( MPI_Dist_graph_neighbors_count )( MPI_Comm* comm, int* indegree, int* outdegree, int* weighted, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Dist_graph_neighbors_count( *comm, indegree, outdegree, weighted );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPH_CREATE )
/**
 * Measurement wrapper for MPI_Graph_create
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_create )( MPI_Comm* comm_old, int* nnodes, int* index, int* edges, int* reorder, MPI_Comm* newcomm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Graph_create( *comm_old, *nnodes, index, edges, *reorder, newcomm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPH_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_get )
/**
 * Measurement wrapper for MPI_Graph_get
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_get )( MPI_Comm* comm, int* maxindex, int* maxedges, int* index, int* edges, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Graph_get( *comm, *maxindex, *maxedges, index, edges );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPH_MAP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_map )
/**
 * Measurement wrapper for MPI_Graph_map
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_map )( MPI_Comm* comm, int* nnodes, int* index, int* edges, int* newrank, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Graph_map( *comm, *nnodes, index, edges, newrank );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors )
/**
 * Measurement wrapper for MPI_Graph_neighbors
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_neighbors )( MPI_Comm* comm, int* rank, int* maxneighbors, int* neighbors, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Graph_neighbors( *comm, *rank, *maxneighbors, neighbors );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors_count )
/**
 * Measurement wrapper for MPI_Graph_neighbors_count
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_neighbors_count )( MPI_Comm* comm, int* rank, int* nneighbors, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Graph_neighbors_count( *comm, *rank, nneighbors );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPHDIMS_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graphdims_get )
/**
 * Measurement wrapper for MPI_Graphdims_get
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graphdims_get )( MPI_Comm* comm, int* nnodes, int* nedges, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Graphdims_get( *comm, nnodes, nedges );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLGATHER )
/**
 * Measurement wrapper for MPI_Ineighbor_allgather
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 */
void
FSUB( MPI_Ineighbor_allgather )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Ineighbor_allgather( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLGATHERV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Ineighbor_allgatherv )
/**
 * Measurement wrapper for MPI_Ineighbor_allgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 */
void
FSUB( MPI_Ineighbor_allgatherv )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* displs, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Ineighbor_allgatherv( sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLTOALL ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Ineighbor_alltoall )
/**
 * Measurement wrapper for MPI_Ineighbor_alltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 */
void
FSUB( MPI_Ineighbor_alltoall )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Ineighbor_alltoall( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLTOALLV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Ineighbor_alltoallv )
/**
 * Measurement wrapper for MPI_Ineighbor_alltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 */
void
FSUB( MPI_Ineighbor_alltoallv )( void* sendbuf, int* sendcounts, int* sdispls, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* rdispls, MPI_Datatype* recvtype, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Ineighbor_alltoallv( sendbuf, sendcounts, sdispls, *sendtype, recvbuf, recvcounts, rdispls, *recvtype, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLTOALLW ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Ineighbor_alltoallw )
/**
 * Measurement wrapper for MPI_Ineighbor_alltoallw
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 */
void
FSUB( MPI_Ineighbor_alltoallw )( void* sendbuf, int sendcounts[], MPI_Aint sdispls[], MPI_Datatype sendtypes[], void* recvbuf, int recvcounts[], MPI_Aint rdispls[], MPI_Datatype recvtypes[], MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Ineighbor_alltoallw( sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLGATHER )
/**
 * Measurement wrapper for MPI_Neighbor_allgather
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 */
void
FSUB( MPI_Neighbor_allgather )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Neighbor_allgather( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLGATHERV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Neighbor_allgatherv )
/**
 * Measurement wrapper for MPI_Neighbor_allgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 */
void
FSUB( MPI_Neighbor_allgatherv )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* displs, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Neighbor_allgatherv( sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLTOALL ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Neighbor_alltoall )
/**
 * Measurement wrapper for MPI_Neighbor_alltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 */
void
FSUB( MPI_Neighbor_alltoall )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Neighbor_alltoall( sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLTOALLV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Neighbor_alltoallv )
/**
 * Measurement wrapper for MPI_Neighbor_alltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 */
void
FSUB( MPI_Neighbor_alltoallv )( void* sendbuf, int* sendcounts, int* sdispls, MPI_Datatype* sendtype, void* recvbuf, int* recvcounts, int* rdispls, MPI_Datatype* recvtype, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Neighbor_alltoallv( sendbuf, sendcounts, sdispls, *sendtype, recvbuf, recvcounts, rdispls, *recvtype, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLTOALLW ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Neighbor_alltoallw )
/**
 * Measurement wrapper for MPI_Neighbor_alltoallw
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 */
void
FSUB( MPI_Neighbor_alltoallw )( void* sendbuf, int sendcounts[], MPI_Aint sdispls[], MPI_Datatype sendtypes[], void* recvbuf, int recvcounts[], MPI_Aint rdispls[], MPI_Datatype recvtypes[], MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Neighbor_alltoallw( sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TOPO_TEST ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Topo_test )
/**
 * Measurement wrapper for MPI_Topo_test
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Topo_test )( MPI_Comm* comm, int* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Topo_test( *comm, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#else /* !NEED_F2C_CONV */

#if HAVE( DECL_PMPI_NEIGHBOR_ALLTOALLW ) && !defined( SCOREP_MPI_NO_TOPO )
/**
 * Measurement wrapper for MPI_Neighbor_alltoallw
 * @note Manually created wrapper
 * @note Fortran interface
 * @note Introduced with MPI 3.0
 * @ingroup topo
 */
void
FSUB( MPI_Neighbor_alltoallw )( void*     sendbuf,
                                MPI_Fint* sendcounts,
                                MPI_Aint* sdispls,
                                MPI_Fint* sendtypes,
                                void*     recvbuf,
                                MPI_Fint* recvcounts,
                                MPI_Aint* rdispls,
                                MPI_Fint* recvtypes,
                                MPI_Fint* comm,
                                MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
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

    *ierr = MPI_Neighbor_alltoallw( sendbuf, sendcounts, sdispls, csendtypes, recvbuf,
                                    recvcounts, rdispls, crecvtypes, ccomm );

    free( csendtypes );
    free( crecvtypes );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_INEIGHBOR_ALLTOALLW ) && !defined( SCOREP_MPI_NO_TOPO )
/**
 * Measurement wrapper for MPI_Ineighbor_alltoallw
 * @note Manually created wrapper
 * @note Fortran interface
 * @note Introduced with MPI 3.0
 * @ingroup topo
 */
void
FSUB( MPI_INeighbor_alltoallw )( void*     sendbuf,
                                 MPI_Fint* sendcounts,
                                 MPI_Aint* sdispls,
                                 MPI_Fint* sendtypes,
                                 void*     recvbuf,
                                 MPI_Fint* recvcounts,
                                 MPI_Aint* rdispls,
                                 MPI_Fint* recvtypes,
                                 MPI_Fint* comm,
                                 MPI_Fint* request,
                                 MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype* csendtypes;
    MPI_Datatype* crecvtypes;
    MPI_Comm      ccomm;
    MPI_Request   crequest;
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

    *ierr = MPI_Ineighbor_alltoallw( sendbuf, sendcounts, sdispls, csendtypes, recvbuf,
                                     recvcounts, rdispls, crecvtypes, ccomm, &crequest );

    *request = PMPI_Request_c2f( crequest );

    free( csendtypes );
    free( crecvtypes );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_CART_COORDS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_coords )
/**
 * Measurement wrapper for MPI_Cart_coords
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_coords
 */
void
FSUB( MPI_Cart_coords )( MPI_Fint* comm, MPI_Fint* rank, MPI_Fint* maxdims, MPI_Fint* coords, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_coords( PMPI_Comm_f2c( *comm ), *rank, *maxdims, coords );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_CREATE )
/**
 * Measurement wrapper for MPI_Cart_create
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_create
 */
void
FSUB( MPI_Cart_create )( MPI_Fint* comm_old, MPI_Fint* ndims, MPI_Fint* dims, MPI_Fint* periods, MPI_Fint* reorder, MPI_Fint* comm_cart, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Comm c_comm_cart;


    *ierr = MPI_Cart_create( PMPI_Comm_f2c( *comm_old ), *ndims, dims, periods, *reorder, &c_comm_cart );

    *comm_cart = PMPI_Comm_c2f( c_comm_cart );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_get )
/**
 * Measurement wrapper for MPI_Cart_get
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_get
 */
void
FSUB( MPI_Cart_get )( MPI_Fint* comm, MPI_Fint* maxdims, MPI_Fint* dims, MPI_Fint* periods, MPI_Fint* coords, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_get( PMPI_Comm_f2c( *comm ), *maxdims, dims, periods, coords );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_MAP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_map )
/**
 * Measurement wrapper for MPI_Cart_map
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_map
 */
void
FSUB( MPI_Cart_map )( MPI_Fint* comm, MPI_Fint* ndims, MPI_Fint* dims, MPI_Fint* periods, MPI_Fint* newrank, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_map( PMPI_Comm_f2c( *comm ), *ndims, dims, periods, newrank );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_RANK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_rank )
/**
 * Measurement wrapper for MPI_Cart_rank
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_rank
 */
void
FSUB( MPI_Cart_rank )( MPI_Fint* comm, MPI_Fint* coords, MPI_Fint* rank, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_rank( PMPI_Comm_f2c( *comm ), coords, rank );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_SHIFT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_shift )
/**
 * Measurement wrapper for MPI_Cart_shift
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_shift
 */
void
FSUB( MPI_Cart_shift )( MPI_Fint* comm, MPI_Fint* direction, MPI_Fint* disp, MPI_Fint* rank_source, MPI_Fint* rank_dest, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cart_shift( PMPI_Comm_f2c( *comm ), *direction, *disp, rank_source, rank_dest );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CART_SUB )
/**
 * Measurement wrapper for MPI_Cart_sub
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_sub
 */
void
FSUB( MPI_Cart_sub )( MPI_Fint* comm, MPI_Fint* remain_dims, MPI_Fint* newcomm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Comm c_newcomm;


    *ierr = MPI_Cart_sub( PMPI_Comm_f2c( *comm ), remain_dims, &c_newcomm );

    *newcomm = PMPI_Comm_c2f( c_newcomm );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CARTDIM_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cartdim_get )
/**
 * Measurement wrapper for MPI_Cartdim_get
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cartdim_get
 */
void
FSUB( MPI_Cartdim_get )( MPI_Fint* comm, MPI_Fint* ndims, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cartdim_get( PMPI_Comm_f2c( *comm ), ndims );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_DIMS_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dims_create )
/**
 * Measurement wrapper for MPI_Dims_create
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Dims_create
 */
void
FSUB( MPI_Dims_create )( MPI_Fint* nnodes, MPI_Fint* ndims, MPI_Fint* dims, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Dims_create( *nnodes, *ndims, dims );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_CREATE ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_create )
/**
 * Measurement wrapper for MPI_Dist_graph_create
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * For the order of events see @ref MPI_Dist_graph_create
 */
void
FSUB( MPI_Dist_graph_create )( MPI_Fint* comm_old, MPI_Fint* n, MPI_Fint* sources, MPI_Fint* degrees, MPI_Fint* destinations, MPI_Fint* weights, MPI_Fint* info, MPI_Fint* reorder, MPI_Fint* newcomm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Comm c_newcomm;


    #if HAVE( MPI_UNWEIGHTED )
    if ( weights == scorep_mpi_fortran_unweighted )
    {
        weights = MPI_UNWEIGHTED;
    }
    #endif


    *ierr = MPI_Dist_graph_create( PMPI_Comm_f2c( *comm_old ), *n, sources, degrees, destinations, weights, PMPI_Info_f2c( *info ), *reorder, &c_newcomm );

    *newcomm = PMPI_Comm_c2f( c_newcomm );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_CREATE_ADJACENT ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_create_adjacent )
/**
 * Measurement wrapper for MPI_Dist_graph_create_adjacent
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * For the order of events see @ref MPI_Dist_graph_create_adjacent
 */
void
FSUB( MPI_Dist_graph_create_adjacent )( MPI_Fint* comm_old, MPI_Fint* indegree, MPI_Fint* sources, MPI_Fint* sourceweights, MPI_Fint* outdegree, MPI_Fint* destinations, MPI_Fint* destweights, MPI_Fint* info, MPI_Fint* reorder, MPI_Fint* newcomm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Comm c_newcomm;


    #if HAVE( MPI_UNWEIGHTED )
    if ( sourceweights == scorep_mpi_fortran_unweighted )
    {
        sourceweights = MPI_UNWEIGHTED;
    }
    #endif
    #if HAVE( MPI_UNWEIGHTED )
    if ( destweights == scorep_mpi_fortran_unweighted )
    {
        destweights = MPI_UNWEIGHTED;
    }
    #endif


    *ierr = MPI_Dist_graph_create_adjacent( PMPI_Comm_f2c( *comm_old ), *indegree, sources, sourceweights, *outdegree, destinations, destweights, PMPI_Info_f2c( *info ), *reorder, &c_newcomm );

    *newcomm = PMPI_Comm_c2f( c_newcomm );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_NEIGHBORS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_neighbors )
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * For the order of events see @ref MPI_Dist_graph_neighbors
 */
void
FSUB( MPI_Dist_graph_neighbors )( MPI_Fint* comm, MPI_Fint* maxindegree, MPI_Fint* sources, MPI_Fint* sourceweights, MPI_Fint* maxoutdegree, MPI_Fint* destinations, MPI_Fint* destweights, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_UNWEIGHTED )
    if ( sourceweights == scorep_mpi_fortran_unweighted )
    {
        sourceweights = MPI_UNWEIGHTED;
    }
    #endif
    #if HAVE( MPI_UNWEIGHTED )
    if ( destweights == scorep_mpi_fortran_unweighted )
    {
        destweights = MPI_UNWEIGHTED;
    }
    #endif


    *ierr = MPI_Dist_graph_neighbors( PMPI_Comm_f2c( *comm ), *maxindegree, sources, sourceweights, *maxoutdegree, destinations, destweights );


    #if HAVE( MPI_UNWEIGHTED )
    if ( sourceweights == MPI_UNWEIGHTED )
    {
        sourceweights = scorep_mpi_fortran_unweighted;
    }
    #endif
    #if HAVE( MPI_UNWEIGHTED )
    if ( destweights == MPI_UNWEIGHTED )
    {
        destweights = scorep_mpi_fortran_unweighted;
    }
    #endif

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_NEIGHBORS_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_neighbors_count )
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors_count
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * For the order of events see @ref MPI_Dist_graph_neighbors_count
 */
void
FSUB( MPI_Dist_graph_neighbors_count )( MPI_Fint* comm, MPI_Fint* indegree, MPI_Fint* outdegree, MPI_Fint* weighted, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Dist_graph_neighbors_count( PMPI_Comm_f2c( *comm ), indegree, outdegree, weighted );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPH_CREATE )
/**
 * Measurement wrapper for MPI_Graph_create
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graph_create
 */
void
FSUB( MPI_Graph_create )( MPI_Fint* comm_old, MPI_Fint* nnodes, MPI_Fint* index, MPI_Fint* edges, MPI_Fint* reorder, MPI_Fint* newcomm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Comm c_newcomm;


    *ierr = MPI_Graph_create( PMPI_Comm_f2c( *comm_old ), *nnodes, index, edges, *reorder, &c_newcomm );

    *newcomm = PMPI_Comm_c2f( c_newcomm );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPH_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_get )
/**
 * Measurement wrapper for MPI_Graph_get
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graph_get
 */
void
FSUB( MPI_Graph_get )( MPI_Fint* comm, MPI_Fint* maxindex, MPI_Fint* maxedges, MPI_Fint* index, MPI_Fint* edges, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Graph_get( PMPI_Comm_f2c( *comm ), *maxindex, *maxedges, index, edges );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPH_MAP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_map )
/**
 * Measurement wrapper for MPI_Graph_map
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graph_map
 */
void
FSUB( MPI_Graph_map )( MPI_Fint* comm, MPI_Fint* nnodes, MPI_Fint* index, MPI_Fint* edges, MPI_Fint* newrank, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Graph_map( PMPI_Comm_f2c( *comm ), *nnodes, index, edges, newrank );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors )
/**
 * Measurement wrapper for MPI_Graph_neighbors
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graph_neighbors
 */
void
FSUB( MPI_Graph_neighbors )( MPI_Fint* comm, MPI_Fint* rank, MPI_Fint* maxneighbors, MPI_Fint* neighbors, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Graph_neighbors( PMPI_Comm_f2c( *comm ), *rank, *maxneighbors, neighbors );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors_count )
/**
 * Measurement wrapper for MPI_Graph_neighbors_count
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graph_neighbors_count
 */
void
FSUB( MPI_Graph_neighbors_count )( MPI_Fint* comm, MPI_Fint* rank, MPI_Fint* nneighbors, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Graph_neighbors_count( PMPI_Comm_f2c( *comm ), *rank, nneighbors );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_GRAPHDIMS_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graphdims_get )
/**
 * Measurement wrapper for MPI_Graphdims_get
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graphdims_get
 */
void
FSUB( MPI_Graphdims_get )( MPI_Fint* comm, MPI_Fint* nnodes, MPI_Fint* nedges, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Graphdims_get( PMPI_Comm_f2c( *comm ), nnodes, nedges );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLGATHER )
/**
 * Measurement wrapper for MPI_Ineighbor_allgather
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * For the order of events see @ref MPI_Ineighbor_allgather
 */
void
FSUB( MPI_Ineighbor_allgather )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


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


    *ierr = MPI_Ineighbor_allgather( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLGATHERV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Ineighbor_allgatherv )
/**
 * Measurement wrapper for MPI_Ineighbor_allgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * For the order of events see @ref MPI_Ineighbor_allgatherv
 */
void
FSUB( MPI_Ineighbor_allgatherv )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


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


    *ierr = MPI_Ineighbor_allgatherv( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, displs, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLTOALL ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Ineighbor_alltoall )
/**
 * Measurement wrapper for MPI_Ineighbor_alltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * For the order of events see @ref MPI_Ineighbor_alltoall
 */
void
FSUB( MPI_Ineighbor_alltoall )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


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


    *ierr = MPI_Ineighbor_alltoall( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLTOALLV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Ineighbor_alltoallv )
/**
 * Measurement wrapper for MPI_Ineighbor_alltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * For the order of events see @ref MPI_Ineighbor_alltoallv
 */
void
FSUB( MPI_Ineighbor_alltoallv )( void* sendbuf, MPI_Fint* sendcounts, MPI_Fint* sdispls, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* rdispls, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


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


    *ierr = MPI_Ineighbor_alltoallv( sendbuf, sendcounts, sdispls, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, rdispls, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLGATHER )
/**
 * Measurement wrapper for MPI_Neighbor_allgather
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * For the order of events see @ref MPI_Neighbor_allgather
 */
void
FSUB( MPI_Neighbor_allgather )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Neighbor_allgather( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLGATHERV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Neighbor_allgatherv )
/**
 * Measurement wrapper for MPI_Neighbor_allgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * For the order of events see @ref MPI_Neighbor_allgatherv
 */
void
FSUB( MPI_Neighbor_allgatherv )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* displs, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Neighbor_allgatherv( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, displs, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLTOALL ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Neighbor_alltoall )
/**
 * Measurement wrapper for MPI_Neighbor_alltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * For the order of events see @ref MPI_Neighbor_alltoall
 */
void
FSUB( MPI_Neighbor_alltoall )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Neighbor_alltoall( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLTOALLV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Neighbor_alltoallv )
/**
 * Measurement wrapper for MPI_Neighbor_alltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * For the order of events see @ref MPI_Neighbor_alltoallv
 */
void
FSUB( MPI_Neighbor_alltoallv )( void* sendbuf, MPI_Fint* sendcounts, MPI_Fint* sdispls, MPI_Fint* sendtype, void* recvbuf, MPI_Fint* recvcounts, MPI_Fint* rdispls, MPI_Fint* recvtype, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


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


    *ierr = MPI_Neighbor_alltoallv( sendbuf, sendcounts, sdispls, PMPI_Type_f2c( *sendtype ), recvbuf, recvcounts, rdispls, PMPI_Type_f2c( *recvtype ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TOPO_TEST ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Topo_test )
/**
 * Measurement wrapper for MPI_Topo_test
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Topo_test
 */
void
FSUB( MPI_Topo_test )( MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Topo_test( PMPI_Comm_f2c( *comm ), status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#endif

/**
 * @}
 */
