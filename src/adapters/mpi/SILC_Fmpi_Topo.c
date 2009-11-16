/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "SILC_Fmpi.h"

/**
 * @file  SILC_Fmpi_Topo.c
 *
 * @brief Fortran interface wrappers for topologies
 */

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

/* If MPI_Comm_f2c is defined as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_coords
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_coords ) ( MPI_Comm * comm, int* rank, int* maxdims, int* coords, int* ierr )
{
    *ierr = MPI_Cart_coords( *comm, *rank, *maxdims, coords );
}
#endif
/**
 * Measurement wrapper for MPI_Cart_create
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_create ) ( MPI_Comm * comm_old, int* ndims, int* dims, int* periods, int* reorder, MPI_Comm * comm_cart, int* ierr )
{
    *ierr = MPI_Cart_create( *comm_old, *ndims, dims, periods, *reorder, comm_cart );
}
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_get
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_get ) ( MPI_Comm * comm, int* maxdims, int* dims, int* periods, int* coords, int* ierr )
{
    *ierr = MPI_Cart_get( *comm, *maxdims, dims, periods, coords );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_map
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_map ) ( MPI_Comm * comm, int* ndims, int* dims, int* periods, int* newrank, int* ierr )
{
    *ierr = MPI_Cart_map( *comm, *ndims, dims, periods, newrank );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_rank
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_rank ) ( MPI_Comm * comm, int* coords, int* rank, int* ierr )
{
    *ierr = MPI_Cart_rank( *comm, coords, rank );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_shift
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_shift ) ( MPI_Comm * comm, int* direction, int* disp, int* rank_source, int* rank_dest, int* ierr )
{
    *ierr = MPI_Cart_shift( *comm, *direction, *disp, rank_source, rank_dest );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_sub
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_sub ) ( MPI_Comm * comm, int* remain_dims, MPI_Comm * newcomm, int* ierr )
{
    *ierr = MPI_Cart_sub( *comm, remain_dims, newcomm );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cartdim_get
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cartdim_get ) ( MPI_Comm * comm, int* ndims, int* ierr )
{
    *ierr = MPI_Cartdim_get( *comm, ndims );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Dims_create
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Dims_create ) ( int* nnodes, int* ndims, int* dims, int* ierr )
{
    *ierr = MPI_Dims_create( *nnodes, *ndims, dims );
}
#endif
/**
 * Measurement wrapper for MPI_Graph_create
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_create ) ( MPI_Comm * comm_old, int* nnodes, int* index, int* edges, int* reorder, MPI_Comm * newcomm, int* ierr )
{
    *ierr = MPI_Graph_create( *comm_old, *nnodes, index, edges, *reorder, newcomm );
}
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Graph_get
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_get ) ( MPI_Comm * comm, int* maxindex, int* maxedges, int* index, int* edges, int* ierr )
{
    *ierr = MPI_Graph_get( *comm, *maxindex, *maxedges, index, edges );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Graph_map
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_map ) ( MPI_Comm * comm, int* nnodes, int* index, int* edges, int* newrank, int* ierr )
{
    *ierr = MPI_Graph_map( *comm, *nnodes, index, edges, newrank );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Graph_neighbors
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_neighbors ) ( MPI_Comm * comm, int* rank, int* maxneighbors, int* neighbors, int* ierr )
{
    *ierr = MPI_Graph_neighbors( *comm, *rank, *maxneighbors, neighbors );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Graph_neighbors_count
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_neighbors_count ) ( MPI_Comm * comm, int* rank, int* nneighbors, int* ierr )
{
    *ierr = MPI_Graph_neighbors_count( *comm, *rank, nneighbors );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Graphdims_get
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graphdims_get ) ( MPI_Comm * comm, int* nnodes, int* nedges, int* ierr )
{
    *ierr = MPI_Graphdims_get( *comm, nnodes, nedges );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Topo_test
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Topo_test ) ( MPI_Comm * comm, int* status, int* ierr )
{
    *ierr = MPI_Topo_test( *comm, status );
}
#endif

#else /* !NEED_F2C_CONV */

#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_coords
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_coords ) ( MPI_Fint * comm, MPI_Fint * rank, MPI_Fint * maxdims, MPI_Fint * coords, int* ierr )
{
    *ierr = MPI_Cart_coords( PMPI_Comm_f2c( *comm ), *rank, *maxdims, coords );
}
#endif
/**
 * Measurement wrapper for MPI_Cart_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_create ) ( MPI_Fint * comm_old, MPI_Fint * ndims, MPI_Fint * dims, MPI_Fint * periods, MPI_Fint * reorder, MPI_Fint * comm_cart, int* ierr )
{
    MPI_Comm c_comm_cart;
    *ierr      = MPI_Cart_create( PMPI_Comm_f2c( *comm_old ), *ndims, dims, periods, *reorder, &c_comm_cart );
    *comm_cart = PMPI_Comm_c2f( c_comm_cart );
}
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_get ) ( MPI_Fint * comm, MPI_Fint * maxdims, MPI_Fint * dims, MPI_Fint * periods, MPI_Fint * coords, int* ierr )
{
    *ierr = MPI_Cart_get( PMPI_Comm_f2c( *comm ), *maxdims, dims, periods, coords );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_map
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_map ) ( MPI_Fint * comm, MPI_Fint * ndims, MPI_Fint * dims, MPI_Fint * periods, MPI_Fint * newrank, int* ierr )
{
    *ierr = MPI_Cart_map( PMPI_Comm_f2c( *comm ), *ndims, dims, periods, newrank );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_rank
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_rank ) ( MPI_Fint * comm, MPI_Fint * coords, MPI_Fint * rank, int* ierr )
{
    *ierr = MPI_Cart_rank( PMPI_Comm_f2c( *comm ), coords, rank );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_shift
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_shift ) ( MPI_Fint * comm, MPI_Fint * direction, MPI_Fint * disp, MPI_Fint * rank_source, MPI_Fint * rank_dest, int* ierr )
{
    *ierr = MPI_Cart_shift( PMPI_Comm_f2c( *comm ), *direction, *disp, rank_source, rank_dest );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cart_sub
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cart_sub ) ( MPI_Fint * comm, MPI_Fint * remain_dims, MPI_Fint * newcomm, int* ierr )
{
    MPI_Comm c_newcomm;
    *ierr    = MPI_Cart_sub( PMPI_Comm_f2c( *comm ), remain_dims, &c_newcomm );
    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Cartdim_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Cartdim_get ) ( MPI_Fint * comm, MPI_Fint * ndims, int* ierr )
{
    *ierr = MPI_Cartdim_get( PMPI_Comm_f2c( *comm ), ndims );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Dims_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Dims_create ) ( MPI_Fint * nnodes, MPI_Fint * ndims, MPI_Fint * dims, int* ierr )
{
    *ierr = MPI_Dims_create( *nnodes, *ndims, dims );
}
#endif
/**
 * Measurement wrapper for MPI_Graph_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_create ) ( MPI_Fint * comm_old, MPI_Fint * nnodes, MPI_Fint * index, MPI_Fint * edges, MPI_Fint * reorder, MPI_Fint * newcomm, int* ierr )
{
    MPI_Comm c_newcomm;
    *ierr    = MPI_Graph_create( PMPI_Comm_f2c( *comm_old ), *nnodes, index, edges, *reorder, &c_newcomm );
    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Graph_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_get ) ( MPI_Fint * comm, MPI_Fint * maxindex, MPI_Fint * maxedges, MPI_Fint * index, MPI_Fint * edges, int* ierr )
{
    *ierr = MPI_Graph_get( PMPI_Comm_f2c( *comm ), *maxindex, *maxedges, index, edges );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Graph_map
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_map ) ( MPI_Fint * comm, MPI_Fint * nnodes, MPI_Fint * index, MPI_Fint * edges, MPI_Fint * newrank, int* ierr )
{
    *ierr = MPI_Graph_map( PMPI_Comm_f2c( *comm ), *nnodes, index, edges, newrank );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Graph_neighbors
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_neighbors ) ( MPI_Fint * comm, MPI_Fint * rank, MPI_Fint * maxneighbors, MPI_Fint * neighbors, int* ierr )
{
    *ierr = MPI_Graph_neighbors( PMPI_Comm_f2c( *comm ), *rank, *maxneighbors, neighbors );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Graph_neighbors_count
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graph_neighbors_count ) ( MPI_Fint * comm, MPI_Fint * rank, MPI_Fint * nneighbors, int* ierr )
{
    *ierr = MPI_Graph_neighbors_count( PMPI_Comm_f2c( *comm ), *rank, nneighbors );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Graphdims_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Graphdims_get ) ( MPI_Fint * comm, MPI_Fint * nnodes, MPI_Fint * nedges, int* ierr )
{
    *ierr = MPI_Graphdims_get( PMPI_Comm_f2c( *comm ), nnodes, nedges );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Topo_test
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup topo
 */
void
FSUB( MPI_Topo_test ) ( MPI_Fint * comm, MPI_Fint * status, int* ierr )
{
    *ierr = MPI_Topo_test( PMPI_Comm_f2c( *comm ), status );
}
#endif

#endif

/**
 * @}
 */
