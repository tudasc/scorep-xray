/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016,
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


#ifndef SCOREP_INTERNAL_UNIFY_HELPERS_H
#define SCOREP_INTERNAL_UNIFY_HELPERS_H


/**
 * @file
 *
 *
 */


#include <SCOREP_Types.h>


#include <stdint.h>


/**
 * Helper to define the communication locations for an paradigm.
 *
 * This helper can be used by single process paradigms, which need to
 * declare its communication group when it is used in an multi process
 * paradigm. Each rank has its own list of locations, which will be
 * collected at the root rank, where the group definition is triggered
 * into the definition manager by rank 0. It returns the offset of this rank
 * into the collated list of locations. This function is best called in a
 * pre_unify subsystem hook and not later. The paradigm must ensure that
 * no previous Group definition is triggered before this call.
 */
uint32_t
scorep_unify_helper_define_comm_locations( SCOREP_GroupType type,
                                           const char*      name,
                                           uint64_t         numberOfMembers,
                                           const uint64_t*  localMembers );


/**
 * Helper to gather global CPU location numbers, including total number of
 * locations, the maximum number of locations per process and an array
 * of location counts per process.
 *
 * @param numberOfLocationsPerRank   Array of locations per rank.
 * @param totalNumberOfLocations     Global number of locations.
 * @param maxLocationsPerRank        Maximum location number of all ranks.
 */
void
scorep_unify_helper_get_number_of_cpu_locations( int*      numberOfLocationsPerRank,
                                                 uint32_t* totalNumberOfLocations,
                                                 int*      maxLocationsPerRank );

/**
 * Helper to exchange cpu location ids between all processes
 * Use scorep_unify_helper_get_number_of_cpu_locations to get the input array of
 * locations counts per rank and to determine the total number of CPU locations.
 * The locations array allLocations has to be preallocated with the size of
 * totalNumberOfLocations.
 *
 * @param allLocations               Array in size of totalNumberOfLocations
 *                                   of all location ids.
 * @param totalNumberOfLocations     Global number of locations.
 * @param numberOfLocationsPerRank   Array of locations per rank.
 */
void
scorep_unify_helper_exchange_all_cpu_locations( uint64_t* allLocations,
                                                uint32_t  totalNumberOfLocations,
                                                int*      numberOfLocationsPerRank );



struct scorep_definitions_manager_entry;

/**
 * Helper function to apply the mapping from the unified Communicator definition
 * to the InterimCommunicator definitions.
 */
void
scorep_unify_helper_create_interim_comm_mapping( struct scorep_definitions_manager_entry* entry );


#endif /* SCOREP_INTERNAL_UNIFY_HELPERS_H */
