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
 * Copyright (c) 2009-2013,
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
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( MPICartesianTopology )
{
    SCOREP_DEFINE_DEFINITION_HEADER( MPICartesianTopology );

    // Add SCOREP_MPICartesianTopology stuff from here on.
};


SCOREP_DEFINE_DEFINITION_TYPE( MPICartesianCoords )
{
    SCOREP_DEFINE_DEFINITION_HEADER( MPICartesianCoords );

    // Add SCOREP_MPICartesianCoords stuff from here on.
};


/**
 * Associate a MPI cartesian topology with a process unique topology handle.
 *
 * @param topologyName A meaningful name for the topology. The string will be
 * copied.
 *
 * @param communicatorHandle A handle to the associated communicator,
 * previously defined by DefineInterimCommunicator().
 *
 * @param nDimensions Number of dimensions of the cartesian topology.
 *
 * @param nProcessesPerDimension Number of processes in each dimension.
 *
 * @param periodicityPerDimension Periodicity in each dimension, true (1) or
 * false (0).
 *
 * @note The @a topologyName and the @a communicatorHandle will be used to
 * determine uniqueness during unification (only). It's the user's
 * responsibility to define unique topologies.
 *
 * @return A process unique topology handle to be used in calls to
 * SCOREP_Definitions_NewMPICartesianCoords().
 *
 */
SCOREP_MPICartesianTopologyHandle
SCOREP_Definitions_NewMPICartesianTopology( const char*                      topologyName,
                                            SCOREP_InterimCommunicatorHandle communicatorHandle,
                                            uint32_t                         nDimensions,
                                            const uint32_t                   nProcessesPerDimension[],
                                            const uint8_t                    periodicityPerDimension[] );


/**
 * Define the coordinates of the current rank in the cartesian topology
 * referenced by @a cartesianTopologyHandle.
 *
 * @param cartesianTopologyHandle Handle to the cartesian topology for which
 * the coordinates are defines.
 *
 * @param nCoords Number of dimensions of the cartesian topology.
 *
 * @param coordsOfCurrentRank Coordinates of current rank.
 *
 */
void
SCOREP_Definitions_NewMPICartesianCoords( SCOREP_MPICartesianTopologyHandle cartesianTopologyHandle,
                                          uint32_t                          nCoords,
                                          const uint32_t                    coordsOfCurrentRank[] );


#endif /* SCOREP_PRIVATE_DEFINITIONS_TOPOLOGY_H */
