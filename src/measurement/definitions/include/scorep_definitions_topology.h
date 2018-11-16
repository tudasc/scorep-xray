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
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_TOPOLOGY_H
#define SCOREP_PRIVATE_DEFINITIONS_TOPOLOGY_H


/**
 * @file
 *
 *  Header for the topology definition calls
 *  contains calls for mpi, user, platform and process x threads topologies
 *
 *  Variables are copied during definition creation. Therefore,
 *  local stack variables can be used as input parameters.
 *
 */


SCOREP_DEFINE_DEFINITION_TYPE( CartesianCoords )
{
    SCOREP_DEFINE_DEFINITION_HEADER( CartesianCoords );
    // Add SCOREP_CartesianCoords stuff from here on.
    SCOREP_CartesianTopologyHandle topology_handle;
    uint32_t                       n_coords;
    uint32_t                       rank;
    uint32_t                       thread;
    // variable array member
    uint32_t coords_of_current_rank[];
};


typedef struct SCOREP_Definitions_CartesianDimension
{
    uint32_t            n_processes_per_dim;
    bool                periodicity_per_dim;
    SCOREP_StringHandle dimension_name;
}SCOREP_Definitions_CartesianDimension;


SCOREP_DEFINE_DEFINITION_TYPE( CartesianTopology )
{
    SCOREP_DEFINE_DEFINITION_HEADER( CartesianTopology );

    // Add SCOREP_CartesianTopology stuff from here on.
    SCOREP_StringHandle              topology_name;
    SCOREP_InterimCommunicatorHandle communicator_handle;
    SCOREP_Topology_Type             topology_type;
    uint32_t                         n_dimensions;
    // variable array member
    SCOREP_Definitions_CartesianDimension cartesian_dims[];
};

/**
 * Create a new cartesian topology with a process unique topology handle.
 *
 * @param topologyName            Name generated for the specific platform.
 *
 * @param communicatorHandle      Inter Communicator, for non-MPI topologies
 *                                SCOREP_INVALID_INTERIM_COMMUNICATOR during
 *                                initialization
 *
 * @param nDimensions             Number of dimensions of the topology.
 *
 * @param nProcessesPerDimension  Number of processes in each dimension.
 *
 * @param periodicityPerDimension Periodicity in each dimension.
 *
 * @param dimensionNames          Names for each dimension.
 *
 * @param topologyType            Type of the topology.
 *
 * @return                        A process unique topology handle.
 *
 */
SCOREP_CartesianTopologyHandle
SCOREP_Definitions_NewCartesianTopology( const char*                      topologyName,
                                         SCOREP_InterimCommunicatorHandle communicatorHandle,
                                         uint32_t                         nDimensions,
                                         const int                        nProcessesPerDimension[],
                                         const int                        periodicityPerDimension[],
                                         const char*                      dimensionNames[],
                                         SCOREP_Topology_Type             topologyType );


/**
 * Create a cartesian coordinate definition.
 *
 * @param cartesianTopologyHandle    The handle of the respective topology
 *
 * @param rank                       rank of the coordinate
 *
 * @param thread                     thread of the coordinate
 *
 * @param nCoords                    number of coordinates for this definition
 *
 * @param coordsOfCurrentRank        Array containing the coordinates
 *
 */
void
SCOREP_Definitions_NewCartesianCoords( SCOREP_CartesianTopologyHandle cartesianTopologyHandle,
                                       uint32_t                       rank,
                                       uint32_t                       thread,
                                       uint32_t                       nCoords,
                                       const int                      coordsOfCurrentRank[] );

/**
 * Create a cartesian coordinate definition after the unification in the unified
 * definitions manager.
 *
 * @param cartesianTopologyHandle    The handle of the respective topology
 *
 * @param rank                       rank of the coordinate
 *
 * @param thread                     thread of the coordinate
 *
 * @param nCoords                    number of coordinates for this definition
 *
 * @param coordsOfCurrentRank        Array containing the coordinates
 *
 */
void
SCOREP_Definitions_NewUnifiedCartesianCoords( SCOREP_CartesianTopologyHandle cartesianTopologyHandle,
                                              uint32_t                       rank,
                                              uint32_t                       thread,
                                              uint32_t                       nCoords,
                                              const int                      coordsOfCurrentRank[] );



void
scorep_definitions_unify_cartesian_coords( SCOREP_CartesianCoordsDef*           definition,
                                           struct SCOREP_Allocator_PageManager* handlesPageManager );

void
scorep_definitions_unify_cartesian_topology( SCOREP_CartesianTopologyDef*         definition,
                                             struct SCOREP_Allocator_PageManager* handlesPageManager );



void
scorep_definitions_rehash_cartesian_topology( SCOREP_CartesianTopologyDef* definition );


#endif /* SCOREP_PRIVATE_DEFINITIONS_TOPOLOGY_H */
