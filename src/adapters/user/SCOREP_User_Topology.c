/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  This file contains the implementation of user adapter functions concerning
 *  user topologies.
 */

#include <config.h>

#include <string.h>

#include <scorep/SCOREP_User_Types.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <UTILS_CStr.h>
#include "SCOREP_User_Init.h"
#include <scorep_status.h>
#include <SCOREP_Location.h>

void
SCOREP_User_CartTopologyCreate( SCOREP_User_CartesianTopologyHandle* topologyHandle,
                                const char*                          name,
                                uint32_t                             nDims )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !scorep_user_enable_topologies )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_MutexLock( scorep_user_topo_mutex );
        if ( *topologyHandle == SCOREP_USER_INVALID_CARTESIAN_TOPOLOGY )
        {
            if ( name == NULL || *name == '\0' )
            {
                UTILS_FATAL( "User topologies are required to have an unique and non-zero name!" );
            }

            SCOREP_User_Topology* user_topo = SCOREP_Memory_AllocForMisc( sizeof( SCOREP_User_Topology ) );
            user_topo->name              = UTILS_CStr_dup( name );
            user_topo->handle            = SCOREP_MOVABLE_NULL;
            user_topo->n_dims            = nDims;
            user_topo->initialized       = false;
            user_topo->dimension_counter = 0;
            user_topo->dim_sizes         = SCOREP_Memory_AllocForMisc( nDims * sizeof( int ) );
            user_topo->dim_periodicity   = SCOREP_Memory_AllocForMisc( nDims * sizeof( int ) );
            user_topo->dim_names         = SCOREP_Memory_AllocForMisc( nDims * sizeof( char* ) );

            *topologyHandle = user_topo;
        }
        else
        {
            UTILS_WARNING( "Initializing a non empty topology!" );
        }
        SCOREP_MutexUnlock( scorep_user_topo_mutex );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
SCOREP_User_CartTopologyAddDim( SCOREP_User_CartesianTopologyHandle topologyHandle,
                                uint32_t                            size,
                                bool                                periodic,
                                const char*                         name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !scorep_user_enable_topologies )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        if ( topologyHandle != SCOREP_USER_INVALID_CARTESIAN_TOPOLOGY )
        {
            if ( topologyHandle->initialized )
            {
                UTILS_WARNING( "Adding unneeded topology dimension after SCOREP_USER_CARTESIAN_TOPOLOGY_INIT was already called; skipping" );
            }
            else
            {
                if ( topologyHandle->dimension_counter >= topologyHandle->n_dims )
                {
                    UTILS_FATAL( "Number of dimensions differs from the user topology definition; expected=%u, found=%u",
                                 topologyHandle->n_dims, topologyHandle->dimension_counter );
                }
                if ( size == 0 )
                {
                    UTILS_FATAL( "Dimensions require a size of at least 1." );
                }
                topologyHandle->dim_sizes[ topologyHandle->dimension_counter ]       = ( int )size;
                topologyHandle->dim_periodicity[ topologyHandle->dimension_counter ] = ( int )periodic;
                topologyHandle->dim_names[ topologyHandle->dimension_counter ]       = UTILS_CStr_dup( name );
                topologyHandle->dimension_counter                                   += 1;
            }
        }
        else
        {
            UTILS_WARNING( "Trying to add a dimension to a topology, which is not created yet! Call ignored." );
        }
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
SCOREP_User_CartTopologyInit( SCOREP_User_CartesianTopologyHandle topologyHandle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !scorep_user_enable_topologies )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        if ( topologyHandle != SCOREP_USER_INVALID_CARTESIAN_TOPOLOGY )
        {
            if ( topologyHandle->initialized )
            {
                UTILS_WARNING( "Multiple calls to SCOREP_USER_CARTESIAN_TOPOLOGY_INIT for this topology; skipping" );
            }
            else
            {
                /* correctness checking: number of dims */
                if ( topologyHandle->dimension_counter != topologyHandle->n_dims )
                {
                    UTILS_FATAL( "Number of dimensions differs from the user topology definition; expected=%u, found=%u",
                                 topologyHandle->n_dims, topologyHandle->dimension_counter );
                }

                SCOREP_InterimCommunicatorHandle user_interim_communicator_handle =
                    SCOREP_Definitions_NewInterimCommunicator(
                        SCOREP_INVALID_INTERIM_COMMUNICATOR,
                        SCOREP_PARADIGM_USER,
                        0,
                        NULL );

                topologyHandle->handle = SCOREP_Definitions_NewCartesianTopology( topologyHandle->name,
                                                                                  user_interim_communicator_handle,
                                                                                  topologyHandle->n_dims,
                                                                                  topologyHandle->dim_sizes,
                                                                                  topologyHandle->dim_periodicity,
                                                                                  ( const char** )topologyHandle->dim_names,
                                                                                  SCOREP_TOPOLOGIES_USER );
                topologyHandle->initialized = true;
                for ( uint32_t i = 0; i < topologyHandle->n_dims; i++ )
                {
                    free( topologyHandle->dim_names[ i ] );
                }
            }
        }
        else
        {
            UTILS_WARNING( "Trying to initialize a topology, which is not created yet! Call ignored." );
        }
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
SCOREP_User_CartTopologySetCoords( SCOREP_User_CartesianTopologyHandle topologyHandle,
                                   uint32_t                            nDims,
                                   ... )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !scorep_user_enable_topologies )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        if ( topologyHandle != SCOREP_USER_INVALID_CARTESIAN_TOPOLOGY )
        {
            SCOREP_CartesianTopologyDef* topo_handle = SCOREP_LOCAL_HANDLE_DEREF( topologyHandle->handle,
                                                                                  CartesianTopology );
            uint32_t n_dims = topo_handle->n_dimensions;

            if ( nDims != n_dims )
            {
                UTILS_FATAL( "Provided number of coords doesn't match the number of dimensions of the topology, num coords=%u, num dims=%u",
                             nDims, n_dims );
            }

            /* correctness checking: topology correctly initialized  */
            if ( !topologyHandle->initialized )
            {
                UTILS_FATAL( "The user topology hasn't been correctly initialized; probable cause: missing call to SCOREP_USER_CARTESIAN_TOPOLOGY_INIT" );
            }

            int coords[ n_dims ];
            memset( coords, -1, n_dims );

            va_list arguments;
            va_start( arguments, nDims );
            for ( int i = 0; i < n_dims; i++ )
            {
                coords[ i ] = va_arg( arguments, int );
                uint32_t procs_per_dim = topo_handle->cartesian_dims[ i ].n_processes_per_dim;
                if ( coords[ i ] >= procs_per_dim || coords[ i ] < 0 )
                {
                    UTILS_FATAL( "Coordinate out of bounds for coord[%d]=%d and dimension size %d or wrong number of coordinates.",
                                 i, coords[ i ], procs_per_dim );
                }
            }
            va_end( arguments );

            uint32_t my_thread = SCOREP_Location_GetId( SCOREP_Location_GetCurrentCPULocation() );
            uint32_t my_rank   = ( uint32_t )SCOREP_Status_GetRank();
            SCOREP_Definitions_NewCartesianCoords( topologyHandle->handle,
                                                   my_rank,
                                                   my_thread,
                                                   n_dims,
                                                   coords );
        }
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
