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
 * @file       SCOREP_Fmpi_Topo.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    MPI_Wrapper
 *
 * @brief Fortran interface wrappers for topologies
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

/* uppercase defines */
/** @def MPI_Cart_coords_U
    Exchange MPI_Cart_coords_U by MPI_CART_COORDS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Cart_coords_U MPI_CART_COORDS

/** @def MPI_Cart_create_U
    Exchange MPI_Cart_create_U by MPI_CART_CREATE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Cart_create_U MPI_CART_CREATE

/** @def MPI_Cart_get_U
    Exchange MPI_Cart_get_U by MPI_CART_GET.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Cart_get_U MPI_CART_GET

/** @def MPI_Cart_map_U
    Exchange MPI_Cart_map_U by MPI_CART_MAP.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Cart_map_U MPI_CART_MAP

/** @def MPI_Cart_rank_U
    Exchange MPI_Cart_rank_U by MPI_CART_RANK.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Cart_rank_U MPI_CART_RANK

/** @def MPI_Cart_shift_U
    Exchange MPI_Cart_shift_U by MPI_CART_SHIFT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Cart_shift_U MPI_CART_SHIFT

/** @def MPI_Cart_sub_U
    Exchange MPI_Cart_sub_U by MPI_CART_SUB.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Cart_sub_U MPI_CART_SUB

/** @def MPI_Cartdim_get_U
    Exchange MPI_Cartdim_get_U by MPI_CARTDIM_GET.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Cartdim_get_U MPI_CARTDIM_GET

/** @def MPI_Dims_create_U
    Exchange MPI_Dims_create_U by MPI_DIMS_CREATE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Dims_create_U MPI_DIMS_CREATE

/** @def MPI_Dist_graph_create_U
    Exchange MPI_Dist_graph_create_U by MPI_DIST_GRAPH_CREATE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Dist_graph_create_U MPI_DIST_GRAPH_CREATE

/** @def MPI_Dist_graph_create_adjacent_U
    Exchange MPI_Dist_graph_create_adjacent_U by MPI_DIST_GRAPH_CREATE_ADJACENT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Dist_graph_create_adjacent_U MPI_DIST_GRAPH_CREATE_ADJACENT

/** @def MPI_Dist_graph_neighbors_U
    Exchange MPI_Dist_graph_neighbors_U by MPI_DIST_GRAPH_NEIGHBORS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Dist_graph_neighbors_U MPI_DIST_GRAPH_NEIGHBORS

/** @def MPI_Dist_graph_neighbors_count_U
    Exchange MPI_Dist_graph_neighbors_count_U by MPI_DIST_GRAPH_NEIGHBORS_COUNT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Dist_graph_neighbors_count_U MPI_DIST_GRAPH_NEIGHBORS_COUNT

/** @def MPI_Graph_create_U
    Exchange MPI_Graph_create_U by MPI_GRAPH_CREATE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Graph_create_U MPI_GRAPH_CREATE

/** @def MPI_Graph_get_U
    Exchange MPI_Graph_get_U by MPI_GRAPH_GET.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Graph_get_U MPI_GRAPH_GET

/** @def MPI_Graph_map_U
    Exchange MPI_Graph_map_U by MPI_GRAPH_MAP.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Graph_map_U MPI_GRAPH_MAP

/** @def MPI_Graph_neighbors_U
    Exchange MPI_Graph_neighbors_U by MPI_GRAPH_NEIGHBORS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Graph_neighbors_U MPI_GRAPH_NEIGHBORS

/** @def MPI_Graph_neighbors_count_U
    Exchange MPI_Graph_neighbors_count_U by MPI_GRAPH_NEIGHBORS_COUNT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Graph_neighbors_count_U MPI_GRAPH_NEIGHBORS_COUNT

/** @def MPI_Graphdims_get_U
    Exchange MPI_Graphdims_get_U by MPI_GRAPHDIMS_GET.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Graphdims_get_U MPI_GRAPHDIMS_GET

/** @def MPI_Topo_test_U
    Exchange MPI_Topo_test_U by MPI_TOPO_TEST.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Topo_test_U MPI_TOPO_TEST


/* lowercase defines */
/** @def MPI_Cart_coords_L
    Exchanges MPI_Cart_coords_L by mpi_cart_coords.
    It is used for the Forran wrappers of me).
 */
#define MPI_Cart_coords_L mpi_cart_coords

/** @def MPI_Cart_create_L
    Exchanges MPI_Cart_create_L by mpi_cart_create.
    It is used for the Forran wrappers of me).
 */
#define MPI_Cart_create_L mpi_cart_create

/** @def MPI_Cart_get_L
    Exchanges MPI_Cart_get_L by mpi_cart_get.
    It is used for the Forran wrappers of me).
 */
#define MPI_Cart_get_L mpi_cart_get

/** @def MPI_Cart_map_L
    Exchanges MPI_Cart_map_L by mpi_cart_map.
    It is used for the Forran wrappers of me).
 */
#define MPI_Cart_map_L mpi_cart_map

/** @def MPI_Cart_rank_L
    Exchanges MPI_Cart_rank_L by mpi_cart_rank.
    It is used for the Forran wrappers of me).
 */
#define MPI_Cart_rank_L mpi_cart_rank

/** @def MPI_Cart_shift_L
    Exchanges MPI_Cart_shift_L by mpi_cart_shift.
    It is used for the Forran wrappers of me).
 */
#define MPI_Cart_shift_L mpi_cart_shift

/** @def MPI_Cart_sub_L
    Exchanges MPI_Cart_sub_L by mpi_cart_sub.
    It is used for the Forran wrappers of me).
 */
#define MPI_Cart_sub_L mpi_cart_sub

/** @def MPI_Cartdim_get_L
    Exchanges MPI_Cartdim_get_L by mpi_cartdim_get.
    It is used for the Forran wrappers of me).
 */
#define MPI_Cartdim_get_L mpi_cartdim_get

/** @def MPI_Dims_create_L
    Exchanges MPI_Dims_create_L by mpi_dims_create.
    It is used for the Forran wrappers of me).
 */
#define MPI_Dims_create_L mpi_dims_create

/** @def MPI_Dist_graph_create_L
    Exchanges MPI_Dist_graph_create_L by mpi_dist_graph_create.
    It is used for the Forran wrappers of me).
 */
#define MPI_Dist_graph_create_L mpi_dist_graph_create

/** @def MPI_Dist_graph_create_adjacent_L
    Exchanges MPI_Dist_graph_create_adjacent_L by mpi_dist_graph_create_adjacent.
    It is used for the Forran wrappers of me).
 */
#define MPI_Dist_graph_create_adjacent_L mpi_dist_graph_create_adjacent

/** @def MPI_Dist_graph_neighbors_L
    Exchanges MPI_Dist_graph_neighbors_L by mpi_dist_graph_neighbors.
    It is used for the Forran wrappers of me).
 */
#define MPI_Dist_graph_neighbors_L mpi_dist_graph_neighbors

/** @def MPI_Dist_graph_neighbors_count_L
    Exchanges MPI_Dist_graph_neighbors_count_L by mpi_dist_graph_neighbors_count.
    It is used for the Forran wrappers of me).
 */
#define MPI_Dist_graph_neighbors_count_L mpi_dist_graph_neighbors_count

/** @def MPI_Graph_create_L
    Exchanges MPI_Graph_create_L by mpi_graph_create.
    It is used for the Forran wrappers of me).
 */
#define MPI_Graph_create_L mpi_graph_create

/** @def MPI_Graph_get_L
    Exchanges MPI_Graph_get_L by mpi_graph_get.
    It is used for the Forran wrappers of me).
 */
#define MPI_Graph_get_L mpi_graph_get

/** @def MPI_Graph_map_L
    Exchanges MPI_Graph_map_L by mpi_graph_map.
    It is used for the Forran wrappers of me).
 */
#define MPI_Graph_map_L mpi_graph_map

/** @def MPI_Graph_neighbors_L
    Exchanges MPI_Graph_neighbors_L by mpi_graph_neighbors.
    It is used for the Forran wrappers of me).
 */
#define MPI_Graph_neighbors_L mpi_graph_neighbors

/** @def MPI_Graph_neighbors_count_L
    Exchanges MPI_Graph_neighbors_count_L by mpi_graph_neighbors_count.
    It is used for the Forran wrappers of me).
 */
#define MPI_Graph_neighbors_count_L mpi_graph_neighbors_count

/** @def MPI_Graphdims_get_L
    Exchanges MPI_Graphdims_get_L by mpi_graphdims_get.
    It is used for the Forran wrappers of me).
 */
#define MPI_Graphdims_get_L mpi_graphdims_get

/** @def MPI_Topo_test_L
    Exchanges MPI_Topo_test_L by mpi_topo_test.
    It is used for the Forran wrappers of me).
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
    *ierr = MPI_Cart_coords( *comm, *rank, *maxdims, coords );
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
    *ierr = MPI_Cart_create( *comm_old, *ndims, dims, periods, *reorder, comm_cart );
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
    *ierr = MPI_Cart_get( *comm, *maxdims, dims, periods, coords );
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
    *ierr = MPI_Cart_map( *comm, *ndims, dims, periods, newrank );
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
    *ierr = MPI_Cart_rank( *comm, coords, rank );
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
    *ierr = MPI_Cart_shift( *comm, *direction, *disp, rank_source, rank_dest );
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
    *ierr = MPI_Cart_sub( *comm, remain_dims, newcomm );
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
    *ierr = MPI_Cartdim_get( *comm, ndims );
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
    *ierr = MPI_Dims_create( *nnodes, *ndims, dims );
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
    #if HAVE( MPI_UNWEIGHTED )
    if ( weights == scorep_mpi_fortran_unweighted )
    {
        weights = MPI_UNWEIGHTED;
    }
    #endif


    *ierr = MPI_Dist_graph_create( *comm_old, *n, sources, degrees, destinations, weights, *info, *reorder, newcomm );
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
    *ierr = MPI_Dist_graph_neighbors_count( *comm, indegree, outdegree, weighted );
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
    *ierr = MPI_Graph_create( *comm_old, *nnodes, index, edges, *reorder, newcomm );
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
    *ierr = MPI_Graph_get( *comm, *maxindex, *maxedges, index, edges );
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
    *ierr = MPI_Graph_map( *comm, *nnodes, index, edges, newrank );
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
    *ierr = MPI_Graph_neighbors( *comm, *rank, *maxneighbors, neighbors );
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
    *ierr = MPI_Graph_neighbors_count( *comm, *rank, nneighbors );
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
    *ierr = MPI_Graphdims_get( *comm, nnodes, nedges );
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
    *ierr = MPI_Topo_test( *comm, status );
}
#endif

#else /* !NEED_F2C_CONV */

#if HAVE( DECL_PMPI_CART_COORDS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_coords )
/**
 * Measurement wrapper for MPI_Cart_coords
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_coords
 */
void
FSUB( MPI_Cart_coords )( MPI_Fint* comm, MPI_Fint* rank, MPI_Fint* maxdims, MPI_Fint* coords, MPI_Fint* ierr )
{
    *ierr = MPI_Cart_coords( PMPI_Comm_f2c( *comm ), *rank, *maxdims, coords );
}
#endif
#if HAVE( DECL_PMPI_CART_CREATE )
/**
 * Measurement wrapper for MPI_Cart_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_create
 */
void
FSUB( MPI_Cart_create )( MPI_Fint* comm_old, MPI_Fint* ndims, MPI_Fint* dims, MPI_Fint* periods, MPI_Fint* reorder, MPI_Fint* comm_cart, MPI_Fint* ierr )
{
    MPI_Comm c_comm_cart;


    *ierr = MPI_Cart_create( PMPI_Comm_f2c( *comm_old ), *ndims, dims, periods, *reorder, &c_comm_cart );

    *comm_cart = PMPI_Comm_c2f( c_comm_cart );
}
#endif
#if HAVE( DECL_PMPI_CART_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_get )
/**
 * Measurement wrapper for MPI_Cart_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_get
 */
void
FSUB( MPI_Cart_get )( MPI_Fint* comm, MPI_Fint* maxdims, MPI_Fint* dims, MPI_Fint* periods, MPI_Fint* coords, MPI_Fint* ierr )
{
    *ierr = MPI_Cart_get( PMPI_Comm_f2c( *comm ), *maxdims, dims, periods, coords );
}
#endif
#if HAVE( DECL_PMPI_CART_MAP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_map )
/**
 * Measurement wrapper for MPI_Cart_map
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_map
 */
void
FSUB( MPI_Cart_map )( MPI_Fint* comm, MPI_Fint* ndims, MPI_Fint* dims, MPI_Fint* periods, MPI_Fint* newrank, MPI_Fint* ierr )
{
    *ierr = MPI_Cart_map( PMPI_Comm_f2c( *comm ), *ndims, dims, periods, newrank );
}
#endif
#if HAVE( DECL_PMPI_CART_RANK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_rank )
/**
 * Measurement wrapper for MPI_Cart_rank
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_rank
 */
void
FSUB( MPI_Cart_rank )( MPI_Fint* comm, MPI_Fint* coords, MPI_Fint* rank, MPI_Fint* ierr )
{
    *ierr = MPI_Cart_rank( PMPI_Comm_f2c( *comm ), coords, rank );
}
#endif
#if HAVE( DECL_PMPI_CART_SHIFT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_shift )
/**
 * Measurement wrapper for MPI_Cart_shift
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_shift
 */
void
FSUB( MPI_Cart_shift )( MPI_Fint* comm, MPI_Fint* direction, MPI_Fint* disp, MPI_Fint* rank_source, MPI_Fint* rank_dest, MPI_Fint* ierr )
{
    *ierr = MPI_Cart_shift( PMPI_Comm_f2c( *comm ), *direction, *disp, rank_source, rank_dest );
}
#endif
#if HAVE( DECL_PMPI_CART_SUB )
/**
 * Measurement wrapper for MPI_Cart_sub
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cart_sub
 */
void
FSUB( MPI_Cart_sub )( MPI_Fint* comm, MPI_Fint* remain_dims, MPI_Fint* newcomm, MPI_Fint* ierr )
{
    MPI_Comm c_newcomm;


    *ierr = MPI_Cart_sub( PMPI_Comm_f2c( *comm ), remain_dims, &c_newcomm );

    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#endif
#if HAVE( DECL_PMPI_CARTDIM_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cartdim_get )
/**
 * Measurement wrapper for MPI_Cartdim_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Cartdim_get
 */
void
FSUB( MPI_Cartdim_get )( MPI_Fint* comm, MPI_Fint* ndims, MPI_Fint* ierr )
{
    *ierr = MPI_Cartdim_get( PMPI_Comm_f2c( *comm ), ndims );
}
#endif
#if HAVE( DECL_PMPI_DIMS_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dims_create )
/**
 * Measurement wrapper for MPI_Dims_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Dims_create
 */
void
FSUB( MPI_Dims_create )( MPI_Fint* nnodes, MPI_Fint* ndims, MPI_Fint* dims, MPI_Fint* ierr )
{
    *ierr = MPI_Dims_create( *nnodes, *ndims, dims );
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_CREATE ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_create )
/**
 * Measurement wrapper for MPI_Dist_graph_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * For the order of events see @ref MPI_Dist_graph_create
 */
void
FSUB( MPI_Dist_graph_create )( MPI_Fint* comm_old, MPI_Fint* n, MPI_Fint* sources, MPI_Fint* degrees, MPI_Fint* destinations, MPI_Fint* weights, MPI_Fint* info, MPI_Fint* reorder, MPI_Fint* newcomm, MPI_Fint* ierr )
{
    MPI_Comm c_newcomm;


    #if HAVE( MPI_UNWEIGHTED )
    if ( weights == scorep_mpi_fortran_unweighted )
    {
        weights = MPI_UNWEIGHTED;
    }
    #endif


    *ierr = MPI_Dist_graph_create( PMPI_Comm_f2c( *comm_old ), *n, sources, degrees, destinations, weights, PMPI_Info_f2c( *info ), *reorder, &c_newcomm );

    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_CREATE_ADJACENT ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_create_adjacent )
/**
 * Measurement wrapper for MPI_Dist_graph_create_adjacent
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * For the order of events see @ref MPI_Dist_graph_create_adjacent
 */
void
FSUB( MPI_Dist_graph_create_adjacent )( MPI_Fint* comm_old, MPI_Fint* indegree, MPI_Fint* sources, MPI_Fint* sourceweights, MPI_Fint* outdegree, MPI_Fint* destinations, MPI_Fint* destweights, MPI_Fint* info, MPI_Fint* reorder, MPI_Fint* newcomm, MPI_Fint* ierr )
{
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
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_NEIGHBORS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_neighbors )
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * For the order of events see @ref MPI_Dist_graph_neighbors
 */
void
FSUB( MPI_Dist_graph_neighbors )( MPI_Fint* comm, MPI_Fint* maxindegree, MPI_Fint* sources, MPI_Fint* sourceweights, MPI_Fint* maxoutdegree, MPI_Fint* destinations, MPI_Fint* destweights, MPI_Fint* ierr )
{
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
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_NEIGHBORS_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_neighbors_count )
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors_count
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * For the order of events see @ref MPI_Dist_graph_neighbors_count
 */
void
FSUB( MPI_Dist_graph_neighbors_count )( MPI_Fint* comm, MPI_Fint* indegree, MPI_Fint* outdegree, MPI_Fint* weighted, MPI_Fint* ierr )
{
    *ierr = MPI_Dist_graph_neighbors_count( PMPI_Comm_f2c( *comm ), indegree, outdegree, weighted );
}
#endif
#if HAVE( DECL_PMPI_GRAPH_CREATE )
/**
 * Measurement wrapper for MPI_Graph_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graph_create
 */
void
FSUB( MPI_Graph_create )( MPI_Fint* comm_old, MPI_Fint* nnodes, MPI_Fint* index, MPI_Fint* edges, MPI_Fint* reorder, MPI_Fint* newcomm, MPI_Fint* ierr )
{
    MPI_Comm c_newcomm;


    *ierr = MPI_Graph_create( PMPI_Comm_f2c( *comm_old ), *nnodes, index, edges, *reorder, &c_newcomm );

    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#endif
#if HAVE( DECL_PMPI_GRAPH_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_get )
/**
 * Measurement wrapper for MPI_Graph_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graph_get
 */
void
FSUB( MPI_Graph_get )( MPI_Fint* comm, MPI_Fint* maxindex, MPI_Fint* maxedges, MPI_Fint* index, MPI_Fint* edges, MPI_Fint* ierr )
{
    *ierr = MPI_Graph_get( PMPI_Comm_f2c( *comm ), *maxindex, *maxedges, index, edges );
}
#endif
#if HAVE( DECL_PMPI_GRAPH_MAP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_map )
/**
 * Measurement wrapper for MPI_Graph_map
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graph_map
 */
void
FSUB( MPI_Graph_map )( MPI_Fint* comm, MPI_Fint* nnodes, MPI_Fint* index, MPI_Fint* edges, MPI_Fint* newrank, MPI_Fint* ierr )
{
    *ierr = MPI_Graph_map( PMPI_Comm_f2c( *comm ), *nnodes, index, edges, newrank );
}
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors )
/**
 * Measurement wrapper for MPI_Graph_neighbors
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graph_neighbors
 */
void
FSUB( MPI_Graph_neighbors )( MPI_Fint* comm, MPI_Fint* rank, MPI_Fint* maxneighbors, MPI_Fint* neighbors, MPI_Fint* ierr )
{
    *ierr = MPI_Graph_neighbors( PMPI_Comm_f2c( *comm ), *rank, *maxneighbors, neighbors );
}
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors_count )
/**
 * Measurement wrapper for MPI_Graph_neighbors_count
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graph_neighbors_count
 */
void
FSUB( MPI_Graph_neighbors_count )( MPI_Fint* comm, MPI_Fint* rank, MPI_Fint* nneighbors, MPI_Fint* ierr )
{
    *ierr = MPI_Graph_neighbors_count( PMPI_Comm_f2c( *comm ), *rank, nneighbors );
}
#endif
#if HAVE( DECL_PMPI_GRAPHDIMS_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graphdims_get )
/**
 * Measurement wrapper for MPI_Graphdims_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Graphdims_get
 */
void
FSUB( MPI_Graphdims_get )( MPI_Fint* comm, MPI_Fint* nnodes, MPI_Fint* nedges, MPI_Fint* ierr )
{
    *ierr = MPI_Graphdims_get( PMPI_Comm_f2c( *comm ), nnodes, nedges );
}
#endif
#if HAVE( DECL_PMPI_TOPO_TEST ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Topo_test )
/**
 * Measurement wrapper for MPI_Topo_test
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * For the order of events see @ref MPI_Topo_test
 */
void
FSUB( MPI_Topo_test )( MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr )
{
    *ierr = MPI_Topo_test( PMPI_Comm_f2c( *comm ), status );
}
#endif

#endif

/**
 * @}
 */
