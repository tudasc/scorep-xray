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
 * Copyright (c) 2009-2016,
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
 * @brief C interface wrappers for topologies
 */

#include <config.h>

#include "SCOREP_Mpi.h"
#include "scorep_mpi_communicator.h"
#include <UTILS_Error.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include <stdio.h>
#include <string.h>

/**
 * @name C wrappers
 * @{
 */

#if HAVE( DECL_PMPI_CART_CREATE )
/**
 * Measurement wrapper for MPI_Cart_create
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Sequence of events:
 * @li enter region 'MPI_Cart_create'
 * @li define communicator
 * @li define topology
 * @li define coordinates
 * @li exit region 'MPI_Cart_create'
 */
int
MPI_Cart_create( MPI_Comm                   comm_old,
                 int                        ndims,
                 SCOREP_MPI_CONST_DECL int* dims,
                 SCOREP_MPI_CONST_DECL int* periodv,
                 int                        reorder,
                 MPI_Comm*                  comm_cart )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_CREATE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_CREATE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Cart_create( comm_old, ndims, dims, periodv, reorder, comm_cart );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( *comm_cart != MPI_COMM_NULL )
    {
        /* register the new topology communicator
           creation independent from actual recording of the topology */
        scorep_mpi_comm_create( *comm_cart, comm_old );

        if ( scorep_mpi_enable_topologies )
        {
            /* get the internal communicator id for the communicator of the new topology */
            SCOREP_InterimCommunicatorHandle comm_id = scorep_mpi_comm_handle( *comm_cart );

            /* find the rank of the calling process */
            int my_rank;
            PMPI_Comm_rank( *comm_cart, &my_rank );

            /* create topology name*/
            /* length = prefix + ndims separators + 11 bytes per dimension */
            char topo_name[ 14 + ndims + 11 * ndims ];
            sprintf( topo_name, "MPI_Cartesian" );
            for ( int i = 0; i < ndims; ++i )
            {
                char sep = 'x';
                if ( i == 0 )
                {
                    sep = '_';
                }
                sprintf( topo_name + strlen( topo_name ), "%c%d", sep, dims[ i ] );
            }

            /* create the cartesian topology definition record */
            SCOREP_CartesianTopologyHandle topo_id = SCOREP_Definitions_NewCartesianTopology( topo_name,
                                                                                              comm_id,
                                                                                              ndims,
                                                                                              dims,
                                                                                              periodv,
                                                                                              NULL,
                                                                                              SCOREP_TOPOLOGIES_MPI );

            int coordv[ ndims ];
            PMPI_Cart_coords( *comm_cart, my_rank, ndims, coordv );

            /* create the coordinates definition record */
            SCOREP_Definitions_NewCartesianCoords( topo_id, my_rank, 0, ndims, coordv );
        }
    }
    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_CREATE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_CREATE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_CART_SUB )
/**
 * Measurement wrapper for MPI_Cart_sub
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_Definitions_NewInterimCommunicator is called.
 */
int
MPI_Cart_sub( MPI_Comm comm, SCOREP_MPI_CONST_DECL int* remain_dims, MPI_Comm* newcomm )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_SUB ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_SUB ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Cart_sub( comm, remain_dims, newcomm );
    SCOREP_EXIT_WRAPPED_REGION();
    if ( *newcomm != MPI_COMM_NULL )
    {
        scorep_mpi_comm_create( *newcomm, comm );

        SCOREP_InterimCommunicatorHandle comm_id     = scorep_mpi_comm_handle( comm );
        SCOREP_InterimCommunicatorHandle sub_comm_id = scorep_mpi_comm_handle( *newcomm );

        int ndims, nparentdims;
        PMPI_Cartdim_get( *newcomm, &ndims );

        int subdims[ ndims ], subperiods[ ndims ], subcoords[ ndims ];
        PMPI_Cart_get( *newcomm, ndims, subdims, subperiods, subcoords );

        /* create topology name*/
        /* length = prefix + ndims separators + 11 bytes per dimension */
        char topo_name[ 18 + ndims + 11 * ndims ];
        sprintf( topo_name, "Sub MPI_Cartesian" );
        for ( int i = 0; i < ndims; ++i )
        {
            char sep = 'x';
            if ( i == 0 )
            {
                sep = '_';
            }
            sprintf( topo_name + strlen( topo_name ), "%c%d", sep, subdims[ i ] );
        }

        /* create the cartesian topology definition record */
        SCOREP_CartesianTopologyHandle topo_id = SCOREP_Definitions_NewCartesianTopology( topo_name,
                                                                                          sub_comm_id,
                                                                                          ndims,
                                                                                          subdims,
                                                                                          subperiods,
                                                                                          NULL,
                                                                                          SCOREP_TOPOLOGIES_MPI );
        int my_rank;
        PMPI_Comm_rank( *newcomm, &my_rank );

        /* create the coordinates definition record */
        SCOREP_Definitions_NewCartesianCoords( topo_id, my_rank, 0, ndims, subcoords );
    }

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_SUB ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_SUB ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_GRAPH_CREATE )
/**
 * Measurement wrapper for MPI_Graph_create
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_Definitions_NewInterimCommunicator is called.
 */
int
MPI_Graph_create( MPI_Comm comm_old, int nnodes, SCOREP_MPI_CONST_DECL int* index, SCOREP_MPI_CONST_DECL int* edges, int reorder, MPI_Comm* newcomm )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_CREATE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_CREATE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Graph_create( comm_old, nnodes, index, edges, reorder, newcomm );
    SCOREP_EXIT_WRAPPED_REGION();
    if ( *newcomm != MPI_COMM_NULL )
    {
        scorep_mpi_comm_create( *newcomm, comm_old );
    }

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_CREATE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_CREATE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_DIST_GRAPH_CREATE ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_create )
/**
 * Measurement wrapper for MPI_Dist_graph_create
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_Definitions_NewInterimCommunicator is called.
 */
int
MPI_Dist_graph_create( MPI_Comm comm_old, int n, SCOREP_MPI_CONST_DECL int sources[], SCOREP_MPI_CONST_DECL int degrees[], SCOREP_MPI_CONST_DECL int destinations[], SCOREP_MPI_CONST_DECL int weights[], MPI_Info info, int reorder, MPI_Comm* newcomm )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_CREATE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_CREATE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Dist_graph_create( comm_old, n, sources, degrees, destinations, weights, info, reorder, newcomm );
    SCOREP_EXIT_WRAPPED_REGION();
    if ( *newcomm != MPI_COMM_NULL )
    {
        scorep_mpi_comm_create( *newcomm, comm_old );
    }

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_CREATE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_CREATE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_CREATE_ADJACENT ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_create_adjacent )
/**
 * Measurement wrapper for MPI_Dist_graph_create_adjacent
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SCOREP_Definitions_NewInterimCommunicator is called.
 */
int
MPI_Dist_graph_create_adjacent( MPI_Comm comm_old, int indegree, SCOREP_MPI_CONST_DECL int sources[], SCOREP_MPI_CONST_DECL int sourceweights[], int outdegree, SCOREP_MPI_CONST_DECL int destinations[], SCOREP_MPI_CONST_DECL int destweights[], MPI_Info info, int reorder, MPI_Comm* newcomm )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_CREATE_ADJACENT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_CREATE_ADJACENT ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Dist_graph_create_adjacent( comm_old, indegree, sources, sourceweights, outdegree, destinations, destweights, info, reorder, newcomm );
    SCOREP_EXIT_WRAPPED_REGION();
    if ( *newcomm != MPI_COMM_NULL )
    {
        scorep_mpi_comm_create( *newcomm, comm_old );
    }

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_CREATE_ADJACENT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_CREATE_ADJACENT ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

#if HAVE( DECL_PMPI_CART_COORDS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_coords )
/**
 * Measurement wrapper for MPI_Cart_coords
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Cart_coords call with enter and exit events.
 */
int
MPI_Cart_coords( MPI_Comm comm, int rank, int maxdims, int* coords )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_COORDS ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_COORDS ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Cart_coords( comm, rank, maxdims, coords );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_COORDS ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_COORDS ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_CART_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_get )
/**
 * Measurement wrapper for MPI_Cart_get
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Cart_get call with enter and exit events.
 */
int
MPI_Cart_get( MPI_Comm comm, int maxdims, int* dims, int* periods, int* coords )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_GET ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_GET ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Cart_get( comm, maxdims, dims, periods, coords );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_GET ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_GET ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_CART_MAP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_map )
/**
 * Measurement wrapper for MPI_Cart_map
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Cart_map call with enter and exit events.
 */
int
MPI_Cart_map( MPI_Comm comm, int ndims, SCOREP_MPI_CONST_DECL int* dims, SCOREP_MPI_CONST_DECL int* periods, int* newrank )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_MAP ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_MAP ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Cart_map( comm, ndims, dims, periods, newrank );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_MAP ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_MAP ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_CART_RANK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_rank )
/**
 * Measurement wrapper for MPI_Cart_rank
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Cart_rank call with enter and exit events.
 */
int
MPI_Cart_rank( MPI_Comm comm, SCOREP_MPI_CONST_DECL int* coords, int* rank )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_RANK ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_RANK ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Cart_rank( comm, coords, rank );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_RANK ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_RANK ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_CART_SHIFT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_shift )
/**
 * Measurement wrapper for MPI_Cart_shift
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Cart_shift call with enter and exit events.
 */
int
MPI_Cart_shift( MPI_Comm comm, int direction, int disp, int* rank_source, int* rank_dest )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_SHIFT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_SHIFT ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Cart_shift( comm, direction, disp, rank_source, rank_dest );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_SHIFT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CART_SHIFT ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_CARTDIM_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cartdim_get )
/**
 * Measurement wrapper for MPI_Cartdim_get
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Cartdim_get call with enter and exit events.
 */
int
MPI_Cartdim_get( MPI_Comm comm, int* ndims )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CARTDIM_GET ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CARTDIM_GET ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Cartdim_get( comm, ndims );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CARTDIM_GET ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_CARTDIM_GET ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_NEIGHBORS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_neighbors )
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Dist_graph_neighbors call with enter and exit events.
 */
int
MPI_Dist_graph_neighbors( MPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_NEIGHBORS ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_NEIGHBORS ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Dist_graph_neighbors( comm, maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_NEIGHBORS ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_NEIGHBORS ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_NEIGHBORS_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_neighbors_count )
/**
 * Measurement wrapper for MPI_Dist_graph_neighbors_count
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Dist_graph_neighbors_count call with enter and exit events.
 */
int
MPI_Dist_graph_neighbors_count( MPI_Comm comm, int* indegree, int* outdegree, int* weighted )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_NEIGHBORS_COUNT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_NEIGHBORS_COUNT ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Dist_graph_neighbors_count( comm, indegree, outdegree, weighted );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_NEIGHBORS_COUNT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIST_GRAPH_NEIGHBORS_COUNT ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_GRAPH_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_get )
/**
 * Measurement wrapper for MPI_Graph_get
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Graph_get call with enter and exit events.
 */
int
MPI_Graph_get( MPI_Comm comm, int maxindex, int maxedges, int* index, int* edges )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_GET ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_GET ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Graph_get( comm, maxindex, maxedges, index, edges );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_GET ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_GET ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_GRAPH_MAP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_map )
/**
 * Measurement wrapper for MPI_Graph_map
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Graph_map call with enter and exit events.
 */
int
MPI_Graph_map( MPI_Comm comm, int nnodes, SCOREP_MPI_CONST_DECL int* index, SCOREP_MPI_CONST_DECL int* edges, int* newrank )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_MAP ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_MAP ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Graph_map( comm, nnodes, index, edges, newrank );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_MAP ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_MAP ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors )
/**
 * Measurement wrapper for MPI_Graph_neighbors
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Graph_neighbors call with enter and exit events.
 */
int
MPI_Graph_neighbors( MPI_Comm comm, int rank, int maxneighbors, int* neighbors )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_NEIGHBORS ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_NEIGHBORS ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Graph_neighbors( comm, rank, maxneighbors, neighbors );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_NEIGHBORS ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_NEIGHBORS ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors_count )
/**
 * Measurement wrapper for MPI_Graph_neighbors_count
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Graph_neighbors_count call with enter and exit events.
 */
int
MPI_Graph_neighbors_count( MPI_Comm comm, int rank, int* nneighbors )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_NEIGHBORS_COUNT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_NEIGHBORS_COUNT ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Graph_neighbors_count( comm, rank, nneighbors );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_NEIGHBORS_COUNT ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPH_NEIGHBORS_COUNT ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_GRAPHDIMS_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graphdims_get )
/**
 * Measurement wrapper for MPI_Graphdims_get
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Graphdims_get call with enter and exit events.
 */
int
MPI_Graphdims_get( MPI_Comm comm, int* nnodes, int* nedges )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPHDIMS_GET ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPHDIMS_GET ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Graphdims_get( comm, nnodes, nedges );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPHDIMS_GET ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_GRAPHDIMS_GET ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLGATHER )
/**
 * Measurement wrapper for MPI_Ineighbor_allgather
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Ineighbor_allgather call with enter and exit events.
 */
int
MPI_Ineighbor_allgather( SCOREP_MPI_CONST_DECL void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLGATHER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLGATHER ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Ineighbor_allgather( sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLGATHER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLGATHER ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLGATHERV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Ineighbor_allgatherv )
/**
 * Measurement wrapper for MPI_Ineighbor_allgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Ineighbor_allgatherv call with enter and exit events.
 */
int
MPI_Ineighbor_allgatherv( SCOREP_MPI_CONST_DECL void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, SCOREP_MPI_CONST_DECL int* recvcounts, SCOREP_MPI_CONST_DECL int* displs, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLGATHERV ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLGATHERV ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Ineighbor_allgatherv( sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLGATHERV ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLGATHERV ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLTOALL ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Ineighbor_alltoall )
/**
 * Measurement wrapper for MPI_Ineighbor_alltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Ineighbor_alltoall call with enter and exit events.
 */
int
MPI_Ineighbor_alltoall( SCOREP_MPI_CONST_DECL void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALL ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Ineighbor_alltoall( sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALL ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLTOALLV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Ineighbor_alltoallv )
/**
 * Measurement wrapper for MPI_Ineighbor_alltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Ineighbor_alltoallv call with enter and exit events.
 */
int
MPI_Ineighbor_alltoallv( SCOREP_MPI_CONST_DECL void* sendbuf, SCOREP_MPI_CONST_DECL int* sendcounts, SCOREP_MPI_CONST_DECL int* sdispls, MPI_Datatype sendtype, void* recvbuf, SCOREP_MPI_CONST_DECL int* recvcounts, SCOREP_MPI_CONST_DECL int* rdispls, MPI_Datatype recvtype, MPI_Comm comm, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALLV ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALLV ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Ineighbor_alltoallv( sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALLV ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALLV ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_INEIGHBOR_ALLTOALLW ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Ineighbor_alltoallw )
/**
 * Measurement wrapper for MPI_Ineighbor_alltoallw
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Ineighbor_alltoallw call with enter and exit events.
 */
int
MPI_Ineighbor_alltoallw( SCOREP_MPI_CONST_DECL void* sendbuf, SCOREP_MPI_CONST_DECL int sendcounts[], SCOREP_MPI_CONST_DECL MPI_Aint sdispls[], SCOREP_MPI_CONST_DECL MPI_Datatype sendtypes[], void* recvbuf, SCOREP_MPI_CONST_DECL int recvcounts[], SCOREP_MPI_CONST_DECL MPI_Aint rdispls[], SCOREP_MPI_CONST_DECL MPI_Datatype recvtypes[], MPI_Comm comm, MPI_Request* request )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALLW ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALLW ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Ineighbor_alltoallw( sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm, request );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALLW ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALLW ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLGATHER )
/**
 * Measurement wrapper for MPI_Neighbor_allgather
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Neighbor_allgather call with enter and exit events.
 */
int
MPI_Neighbor_allgather( SCOREP_MPI_CONST_DECL void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLGATHER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLGATHER ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Neighbor_allgather( sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLGATHER ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLGATHER ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLGATHERV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Neighbor_allgatherv )
/**
 * Measurement wrapper for MPI_Neighbor_allgatherv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Neighbor_allgatherv call with enter and exit events.
 */
int
MPI_Neighbor_allgatherv( SCOREP_MPI_CONST_DECL void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, SCOREP_MPI_CONST_DECL int* recvcounts, SCOREP_MPI_CONST_DECL int* displs, MPI_Datatype recvtype, MPI_Comm comm )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLGATHERV ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLGATHERV ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Neighbor_allgatherv( sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLGATHERV ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLGATHERV ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLTOALL ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Neighbor_alltoall )
/**
 * Measurement wrapper for MPI_Neighbor_alltoall
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Neighbor_alltoall call with enter and exit events.
 */
int
MPI_Neighbor_alltoall( SCOREP_MPI_CONST_DECL void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALL ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Neighbor_alltoall( sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALL ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALL ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLTOALLV ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Neighbor_alltoallv )
/**
 * Measurement wrapper for MPI_Neighbor_alltoallv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Neighbor_alltoallv call with enter and exit events.
 */
int
MPI_Neighbor_alltoallv( SCOREP_MPI_CONST_DECL void* sendbuf, SCOREP_MPI_CONST_DECL int* sendcounts, SCOREP_MPI_CONST_DECL int* sdispls, MPI_Datatype sendtype, void* recvbuf, SCOREP_MPI_CONST_DECL int* recvcounts, SCOREP_MPI_CONST_DECL int* rdispls, MPI_Datatype recvtype, MPI_Comm comm )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALLV ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALLV ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Neighbor_alltoallv( sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALLV ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALLV ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_NEIGHBOR_ALLTOALLW ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Neighbor_alltoallw )
/**
 * Measurement wrapper for MPI_Neighbor_alltoallw
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-3
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Neighbor_alltoallw call with enter and exit events.
 */
int
MPI_Neighbor_alltoallw( SCOREP_MPI_CONST_DECL void* sendbuf, SCOREP_MPI_CONST_DECL int sendcounts[], SCOREP_MPI_CONST_DECL MPI_Aint sdispls[], SCOREP_MPI_CONST_DECL MPI_Datatype sendtypes[], void* recvbuf, SCOREP_MPI_CONST_DECL int recvcounts[], SCOREP_MPI_CONST_DECL MPI_Aint rdispls[], SCOREP_MPI_CONST_DECL MPI_Datatype recvtypes[], MPI_Comm comm )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALLW ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALLW ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Neighbor_alltoallw( sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALLW ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALLW ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_TOPO_TEST ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Topo_test )
/**
 * Measurement wrapper for MPI_Topo_test
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Topo_test call with enter and exit events.
 */
int
MPI_Topo_test( MPI_Comm comm, int* status )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TOPO_TEST ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TOPO_TEST ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Topo_test( comm, status );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TOPO_TEST ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_TOPO_TEST ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif
#if HAVE( DECL_PMPI_DIMS_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dims_create )
/**
 * Measurement wrapper for MPI_Dims_create
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the MPI_Dims_create call with enter and exit events.
 */
int
MPI_Dims_create( int nnodes, int ndims, int* dims )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    const int event_gen_active           = SCOREP_MPI_IS_EVENT_GEN_ON;
    const int event_gen_active_for_group = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        if ( event_gen_active_for_group )
        {
            SCOREP_EnterWrappedRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIMS_CREATE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_EnterWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIMS_CREATE ] );
        }
    }

    SCOREP_ENTER_WRAPPED_REGION();
    return_val = PMPI_Dims_create( nnodes, ndims, dims );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( event_gen_active )
    {
        if ( event_gen_active_for_group )
        {
            SCOREP_ExitRegion( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIMS_CREATE ] );
        }
        else if ( SCOREP_IsUnwindingEnabled() )
        {
            SCOREP_ExitWrapper( scorep_mpi_regions[ SCOREP_MPI_REGION__MPI_DIMS_CREATE ] );
        }
        SCOREP_MPI_EVENT_GEN_ON();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return return_val;
}
#endif

/**
 * @}
 */
