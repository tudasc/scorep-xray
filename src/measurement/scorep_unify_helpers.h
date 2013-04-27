/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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


#ifndef SCOREP_INTERNAL_UNIFY_HELPERS_H
#define SCOREP_INTERNAL_UNIFY_HELPERS_H


/**
 * @file       src/measurement/scorep_unify_helpers.h
 * @maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>
 *
 * @status alpha
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
 * into the unified definition manager. It returns the offset of this rank
 * into the collated list of locations.
 */
uint32_t
scorep_unify_helper_define_comm_locations( SCOREP_GroupType type,
                                           const char*      name,
                                           uint64_t         numberOfMembers,
                                           const uint64_t*  localMembers );


#endif /* SCOREP_INTERNAL_UNIFY_HELPERS_H */
