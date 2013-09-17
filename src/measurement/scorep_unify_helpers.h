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


#endif /* SCOREP_INTERNAL_UNIFY_HELPERS_H */
