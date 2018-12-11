/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2018,
 * Technische Universitaet Dresden, Germany
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
 *  user topologies for Fortran macros.
 */

#include <config.h>

#include <scorep/SCOREP_User_Types.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Fortran_Wrapper.h>
#include <scorep_status.h>
#include <SCOREP_Location.h>

#include <string.h>
#include <UTILS_CStr.h>

#include "SCOREP_User_Init.h"

#define SCOREP_F_CartTopologyCreate_L    scorep_f_carttopologycreate
#define SCOREP_F_CartTopologyAddDim_L    scorep_f_carttopologyadddim
#define SCOREP_F_CartTopologyInit_L      scorep_f_carttopologyinit
#define SCOREP_F_CartTopologySetCoords_L scorep_f_carttopologysetcoords

#define SCOREP_F_CartTopologyCreate_U    SCOREP_F_CARTTOPOLOGYCREATE
#define SCOREP_F_CartTopologyAddDim_U    SCOREP_F_CARTTOPOLOGYADDDIM
#define SCOREP_F_CartTopologyInit_U      SCOREP_F_CARTTOPOLOGYINIT
#define SCOREP_F_CartTopologySetCoords_U SCOREP_F_CARTTOPOLOGYSETCOORDS

void
FSUB( SCOREP_F_CartTopologyCreate )( SCOREP_Fortran_TopologyHandle* topologyHandle,
                                     const char*                    name,
                                     uint32_t*                      nDims,
                                     scorep_fortran_charlen_t       nameLen )
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
        if ( *topologyHandle == SCOREP_USER_INVALID_TOPOLOGY )
        {
            /* Convert name to C-String, like in SCOREP_User_RegionF.c */
            char* name_c = malloc( ( nameLen + 1 ) * sizeof( char ) );
            strncpy( name_c, name, nameLen );
            name_c[ nameLen ] = '\0';

            if ( name_c == NULL || name_c[ 0 ] == '\0' )
            {
                UTILS_FATAL( "User topologies are required to have an unique and non-zero name!" );
            }

            SCOREP_User_Topology* user_topo = SCOREP_Memory_AllocForMisc( sizeof( SCOREP_User_Topology ) );
            user_topo->name              = UTILS_CStr_dup( name_c );
            user_topo->handle            = SCOREP_MOVABLE_NULL;
            user_topo->n_dims            = *nDims;
            user_topo->initialized       = false;
            user_topo->dimension_counter = 0;
            user_topo->dim_sizes         = SCOREP_Memory_AllocForMisc( *nDims * sizeof( int ) );
            user_topo->dim_periodicity   = SCOREP_Memory_AllocForMisc( *nDims * sizeof( int ) );
            user_topo->dim_names         = SCOREP_Memory_AllocForMisc( *nDims * sizeof( char* ) );

            *topologyHandle = ( SCOREP_Fortran_TopologyHandle )user_topo;

            free( name_c );
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
FSUB( SCOREP_F_CartTopologyAddDim )( SCOREP_Fortran_TopologyHandle* topologyHandle,
                                     int*                           size,
                                     int*                           periodic,
                                     const char*                    name,
                                     scorep_fortran_charlen_t       nameLen )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !scorep_user_enable_topologies )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }


    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        if ( *topologyHandle != SCOREP_USER_INVALID_TOPOLOGY )
        {
            SCOREP_User_Topology* topo_handle = ( SCOREP_User_Topology* )( *topologyHandle );

            /* correctness checking: call order before init */
            if ( topo_handle->initialized )
            {
                UTILS_WARNING( "Adding unneeded topology dimension after SCOREP_USER_CARTESIAN_TOPOLOGY_INIT was already called; skipping" );
            }
            else
            {
                if ( topo_handle->dimension_counter >= topo_handle->n_dims )
                {
                    UTILS_FATAL( "Number of dimensions differs from the user topology definition; expected=%u, found=%u",
                                 topo_handle->n_dims, topo_handle->dimension_counter );
                }
                if ( *size <= 0 )
                {
                    UTILS_FATAL( "Dimensions require a size of at least 1." );
                }
                char* name_c = malloc( ( nameLen + 1 ) * sizeof( char ) );
                strncpy( name_c, name, nameLen );
                name_c[ nameLen ] = '\0';

                topo_handle->dim_sizes[ topo_handle->dimension_counter ]       = *size;
                topo_handle->dim_periodicity[ topo_handle->dimension_counter ] = *periodic;
                topo_handle->dim_names[ topo_handle->dimension_counter ]       = UTILS_CStr_dup( name_c );
                topo_handle->dimension_counter                                += 1;

                free( name_c );
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
FSUB( SCOREP_F_CartTopologyInit )( SCOREP_Fortran_TopologyHandle* topologyHandle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !scorep_user_enable_topologies )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        if ( *topologyHandle != SCOREP_USER_INVALID_TOPOLOGY )
        {
            SCOREP_User_Topology* topo_handle = ( SCOREP_User_Topology* )( *topologyHandle );
            if ( topo_handle->initialized )
            {
                UTILS_WARNING( "Multiple calls to SCOREP_USER_CARTESIAN_TOPOLOGY_INIT for this topology; skipping" );
            }
            else
            {
                /* correctness checking: number of dims */
                if ( topo_handle->dimension_counter != topo_handle->n_dims )
                {
                    UTILS_FATAL( "Number of dimensions differs from the user topology definition; expected=%u, found=%u",
                                 topo_handle->n_dims, topo_handle->dimension_counter );
                }

                SCOREP_InterimCommunicatorHandle user_interim_communicator_handle =
                    SCOREP_Definitions_NewInterimCommunicator(
                        SCOREP_INVALID_INTERIM_COMMUNICATOR,
                        SCOREP_PARADIGM_USER,
                        0,
                        NULL );

                topo_handle->handle = SCOREP_Definitions_NewCartesianTopology( topo_handle->name,
                                                                               user_interim_communicator_handle,
                                                                               topo_handle->n_dims,
                                                                               topo_handle->dim_sizes,
                                                                               topo_handle->dim_periodicity,
                                                                               ( const char** )topo_handle->dim_names,
                                                                               SCOREP_TOPOLOGIES_USER );
                topo_handle->initialized = true;
                for ( uint32_t i = 0; i < topo_handle->n_dims; i++ )
                {
                    free( topo_handle->dim_names[ i ] );
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
FSUB( SCOREP_F_CartTopologySetCoords )( SCOREP_Fortran_TopologyHandle* topologyHandle,
                                        uint32_t*                      nDims,
                                        int*                           coords )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !scorep_user_enable_topologies )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_User_Topology* f2c_topo_handle = ( SCOREP_User_Topology* )( *topologyHandle );
        if ( f2c_topo_handle->handle !=  SCOREP_INVALID_CART_TOPOLOGY )
        {
            SCOREP_CartesianTopologyDef* topo_handle = SCOREP_LOCAL_HANDLE_DEREF( f2c_topo_handle->handle,
                                                                                  CartesianTopology );
            uint32_t n_dims = topo_handle->n_dimensions;

            if ( *nDims != n_dims )
            {
                UTILS_FATAL( "Provided number of coords doesn't match the number of dimensions of the topology, num coords=%u, num dims=%u",
                             nDims, n_dims );
            }

            /* correctness checking: topology correctly initialized  */
            if ( !f2c_topo_handle->initialized )
            {
                UTILS_FATAL( "The user topology hasn't been correctly initialized; probable cause: missing call to SCOREP_USER_CARTESIAN_TOPOLOGY_INIT" );
            }

            for ( uint32_t i = 0; i < n_dims; i++ )
            {
                uint32_t procs_per_dim = topo_handle->cartesian_dims[ i ].n_processes_per_dim;
                if ( coords[ i ] >= procs_per_dim  || coords[ i ] < 0 )
                {
                    UTILS_FATAL( "Coordinate out of bounds for coord %d and dimension size %d.", coords[ i ], procs_per_dim );
                }
            }

            uint32_t my_thread = SCOREP_Location_GetId( SCOREP_Location_GetCurrentCPULocation() );
            uint32_t my_rank   = ( uint32_t )SCOREP_Status_GetRank();
            SCOREP_Definitions_NewCartesianCoords( f2c_topo_handle->handle,
                                                   my_rank,
                                                   my_thread,
                                                   n_dims,
                                                   coords );
        }
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
