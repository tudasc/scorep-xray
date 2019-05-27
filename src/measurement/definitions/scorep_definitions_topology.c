/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2017,
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


/**
 * @file
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 * General rule for locking definitions:
 *  If an access to the definitions is used during measurement, e.g., adding a
 *  new topology, it has to be locked.
 *  Functions, that are used only during unification, don't need to be locked
 *  as Score-P runs serial there.
 *  If a function can be used during both phases, the caller has to ensure
 *  locking as needed.
 *
 */


#include <config.h>


#include <SCOREP_Definitions.h>
#include "scorep_definitions_private.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>


#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME DEFINITIONS
#include <UTILS_Debug.h>


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_type_utils.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>
#include <scorep_status.h>
#include <scorep_substrates_definition.h>



/* *INDENT-OFF* */
/* local helper functions */
static SCOREP_CartesianCoordsHandle define_coords( SCOREP_DefinitionManager* definition_manager, SCOREP_CartesianTopologyHandle cartesianTopologyHandle, uint32_t rank, uint32_t thread, uint32_t nCoords,  const int coordsOfCurrentRank[] );
static SCOREP_CartesianTopologyHandle define_topology( SCOREP_DefinitionManager* definition_manager, SCOREP_InterimCommunicatorHandle communicatorHandle, uint32_t ndimensions, SCOREP_Topology_Type topologyType, SCOREP_Definitions_CartesianDimension  dimensions[], SCOREP_StringHandle topologyName );
static bool equal_cartesian_coords( const SCOREP_CartesianCoordsDef* existingDefinition, const SCOREP_CartesianCoordsDef* newDefinition );
static bool equal_cartesian_topology( const SCOREP_CartesianTopologyDef* existingDefinition, const SCOREP_CartesianTopologyDef* newDefinition );
/* *INDENT-ON* */

SCOREP_CartesianTopologyHandle
SCOREP_Definitions_NewCartesianTopology( const char*                      topologyName,
                                         SCOREP_InterimCommunicatorHandle communicatorHandle,
                                         uint32_t                         nDimensions,
                                         const int                        nProcessesPerDimension[],
                                         const int                        periodicityPerDimension[],
                                         const char*                      dimensionNames[],
                                         SCOREP_Topology_Type             topologyType )
{
    SCOREP_Definitions_CartesianDimension cartesian_dimensions[ nDimensions ];
    SCOREP_CartesianTopologyHandle        new_handle = SCOREP_INVALID_CART_TOPOLOGY;

    SCOREP_Definitions_Lock();

    /** create definitions for each dimension.*/
    for ( uint32_t i = 0; i < nDimensions; ++i )
    {
        //uses a default name for dimensions if non are given
        SCOREP_StringHandle name_handle = SCOREP_INVALID_STRING;
        if ( dimensionNames && dimensionNames[ i ] )
        {
            name_handle =  scorep_definitions_new_string(
                &scorep_local_definition_manager,
                dimensionNames[ i ], NULL );
        }
        else
        {
            /* initialize default dimension name for any unnamed dimensions */
            char buffer[ 20 ];
            snprintf( buffer, 20, "dimension %d", i );
            name_handle = scorep_definitions_new_string(
                &scorep_local_definition_manager,
                buffer, NULL );
        }

        cartesian_dimensions[ i ].dimension_name      = name_handle;
        cartesian_dimensions[ i ].n_processes_per_dim = ( uint32_t )nProcessesPerDimension[ i ];
        cartesian_dimensions[ i ].periodicity_per_dim = ( bool )periodicityPerDimension[ i ];
    }

    SCOREP_StringHandle topo_name_handle = SCOREP_INVALID_STRING;
    if ( topologyName )
    {
        topo_name_handle =  scorep_definitions_new_string(
            &scorep_local_definition_manager,
            topologyName, NULL );
    }
    else
    {
        /* initialize default topology name for any unnamed topologies */
        topo_name_handle = scorep_definitions_new_string(
            &scorep_local_definition_manager,
            "unnamed topology", NULL );
    }
    /** The handle to be returned from this function is actually
     *  defined in the define_topology internal function.
     */
    new_handle = define_topology(
        &scorep_local_definition_manager,
        communicatorHandle,
        nDimensions,
        topologyType,
        cartesian_dimensions,
        topo_name_handle );

    SCOREP_Definitions_Unlock();

    SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                ( new_handle, SCOREP_HANDLE_TYPE_CARTESIAN_TOPOLOGY ) );

    return new_handle;
}

void
SCOREP_Definitions_NewCartesianCoords( SCOREP_CartesianTopologyHandle cartesianTopologyHandle,
                                       uint32_t                       rank,
                                       uint32_t                       thread,
                                       uint32_t                       nCoords,
                                       const int                      coordsOfCurrentRank[] )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    SCOREP_CartesianCoordsHandle new_handle = define_coords(
        &scorep_local_definition_manager,
        cartesianTopologyHandle,
        rank,
        thread,
        nCoords,
        coordsOfCurrentRank );

    SCOREP_Definitions_Unlock();

    SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                ( new_handle, SCOREP_HANDLE_TYPE_CARTESIAN_COORDS ) );
}

void
SCOREP_Definitions_NewUnifiedCartesianCoords( SCOREP_CartesianTopologyHandle cartesianTopologyHandle,
                                              uint32_t                       rank,
                                              uint32_t                       thread,
                                              uint32_t                       nCoords,
                                              const int                      coordsOfCurrentRank[] )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    define_coords(
        scorep_unified_definition_manager,
        cartesianTopologyHandle,
        rank,
        thread,
        nCoords,
        coordsOfCurrentRank );

    SCOREP_Definitions_Unlock();
}


/**
 * local helper functions
 */

void
scorep_definitions_unify_cartesian_coords( SCOREP_CartesianCoordsDef*    definition,
                                           SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    definition->unified = define_coords(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->topology_handle,
            CartesianTopology,
            handlesPageManager ),
        definition->rank,
        definition->thread,
        definition->n_coords,
        definition->coords_of_current_rank );
}

void
scorep_definitions_unify_cartesian_topology( SCOREP_CartesianTopologyDef*  definition,
                                             SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_Definitions_CartesianDimension cartesian_dimensions[ definition->n_dimensions ];

    for ( uint32_t i = 0; i < definition->n_dimensions; ++i )
    {
        cartesian_dimensions[ i ].n_processes_per_dim = definition->cartesian_dims[ i ].n_processes_per_dim;
        cartesian_dimensions[ i ].periodicity_per_dim = definition->cartesian_dims[ i ].periodicity_per_dim;
        cartesian_dimensions[ i ].dimension_name      = SCOREP_HANDLE_GET_UNIFIED( definition->cartesian_dims[ i ].dimension_name,
                                                                                   String,
                                                                                   handlesPageManager );
    }

    definition->unified = define_topology(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->communicator_handle,
            Communicator,
            handlesPageManager ),
        definition->n_dimensions,
        definition->topology_type,
        cartesian_dimensions,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->topology_name,
            String,
            handlesPageManager ) );
}

/* as this is used during measurement as well as during unification the caller is
   responsible to lock as needed */
SCOREP_CartesianCoordsHandle
define_coords( SCOREP_DefinitionManager*      definition_manager,
               SCOREP_CartesianTopologyHandle cartesianTopologyHandle,
               uint32_t                       rank,
               uint32_t                       thread,
               uint32_t                       nCoords,
               const int                      coordsOfCurrentRank[] )

{
    UTILS_ASSERT( definition_manager );

    SCOREP_CartesianCoordsDef*   new_definition = NULL;
    SCOREP_CartesianCoordsHandle new_handle     = SCOREP_INVALID_CART_COORDS;
    SCOREP_DEFINITION_ALLOC_VARIABLE_ARRAY( CartesianCoords,
                                            uint32_t,
                                            nCoords );

    /* Init new_definition */
    new_definition->topology_handle = cartesianTopologyHandle;

    new_definition->n_coords = nCoords;

    new_definition->rank = rank;

    new_definition->thread = thread;

    /* Just copy array */
    for ( uint32_t i = 0; i < nCoords; i++ )
    {
        new_definition->coords_of_current_rank[ i ] = ( uint32_t )coordsOfCurrentRank[ i ];
    }

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( CartesianCoords, cartesian_coords );

    return new_handle;
}


/* only used during unification, no locking needed*/
static void
hash_cartesian_topology( SCOREP_CartesianTopologyDef* definition )
{
    HASH_ADD_HANDLE( definition, topology_name, String );
    HASH_ADD_POD( definition, n_dimensions );
    HASH_ADD_POD( definition, topology_type );
}

/* only used during unification, no locking needed*/
void
scorep_definitions_rehash_cartesian_topology( SCOREP_CartesianTopologyDef* definition )
{
    definition->hash_value = 0;

    hash_cartesian_topology( definition );
}

/* as this is used during measurement as well as during unification the caller is
   responsible to lock as needed */
SCOREP_CartesianTopologyHandle
define_topology( SCOREP_DefinitionManager*             definition_manager,
                 SCOREP_InterimCommunicatorHandle      communicatorHandle,
                 uint32_t                              ndimensions,
                 SCOREP_Topology_Type                  topologyType,
                 SCOREP_Definitions_CartesianDimension dimensions[],
                 SCOREP_StringHandle                   topologyName )

{
    UTILS_ASSERT( definition_manager );

    SCOREP_CartesianTopologyDef*   new_definition = NULL;
    SCOREP_CartesianTopologyHandle new_handle     = SCOREP_INVALID_CART_TOPOLOGY;
    SCOREP_DEFINITION_ALLOC_VARIABLE_ARRAY( CartesianTopology,
                                            SCOREP_Definitions_CartesianDimension,
                                            ndimensions );


    /* Init new_definition */
    new_definition->topology_name = topologyName;

    new_definition->communicator_handle = communicatorHandle;

    new_definition->n_dimensions = ndimensions;

    new_definition->topology_type = topologyType;

    hash_cartesian_topology( new_definition );

    /* Just copy array */
    memcpy( new_definition->cartesian_dims,
            dimensions,
            sizeof( new_definition->cartesian_dims[ 0 ] ) * ndimensions );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( CartesianTopology, cartesian_topology );

    return new_handle;
}


static bool
equal_cartesian_coords( const SCOREP_CartesianCoordsDef* existingDefinition,
                        const SCOREP_CartesianCoordsDef* newDefinition )
{
    return existingDefinition->topology_handle == newDefinition->topology_handle
           && existingDefinition->n_coords == newDefinition->n_coords
           && existingDefinition->rank == newDefinition->rank
           && existingDefinition->thread == newDefinition->thread
           && 0 == memcmp( existingDefinition->coords_of_current_rank,
                           newDefinition->coords_of_current_rank,
                           sizeof( existingDefinition->coords_of_current_rank[ 0 ] )
                           * existingDefinition->n_coords  );
}


static bool
equal_cartesian_topology( const SCOREP_CartesianTopologyDef* existingDefinition,
                          const SCOREP_CartesianTopologyDef* newDefinition )
{
    if ( existingDefinition->topology_type != newDefinition->topology_type )
    {
        return false;
    }

    if ( newDefinition->topology_type != SCOREP_TOPOLOGIES_MPI )
    {
        return existingDefinition->topology_name == newDefinition->topology_name
               && existingDefinition->n_dimensions == newDefinition->n_dimensions;
    }
    else
    {
        return existingDefinition->topology_name == newDefinition->topology_name
               && existingDefinition->communicator_handle == newDefinition->communicator_handle
               && existingDefinition->n_dimensions == newDefinition->n_dimensions;
    }
}
