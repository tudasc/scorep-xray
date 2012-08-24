/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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

#ifndef SCOREP_INTERNAL_REWIND_STACK_MANAGEMENT_H
#define SCOREP_INTERNAL_REWIND_STACK_MANAGEMENT_H



/**
 * @file       scorep_rewind_stack_management.h
 * @maintainer Jens Domke <jens.domke@tu-dresden.de>
 *
 * @status alpha
 *
 *
 */

#include "scorep_types.h"

#include <stdbool.h>

#include <SCOREP_Location.h>

typedef struct scorep_rewind_stack scorep_rewind_stack;


struct scorep_rewind_stack
{
    uint32_t             id;
    uint64_t             entertimestamp;
    scorep_rewind_stack* prev;

    /** paradigm event occurred in rewind region */
    bool paradigm_affected[ SCOREP_PARADIGM_MAX ];
};


/**
 * Search for a specific id of a rewind region in the rewind stack.
 *
 * @param  id           The identification number of the rewind region.
 *
 * @return Boolean, whether the id was found in the stack or not.
 */
bool
scorep_rewind_stack_find( uint32_t id );


/**
 * Save the id of the rewind region handle with it's timestamp on the stack.
 * It's not a real stack, the elements are unique, no multiple id is allowed.
 * The function manipulates the stack_head pointer.
 *
 * @param  id               The identification number of the rewind region.
 * @param  entertimestamp   Time stamp, when the rewind region was entered.
 */
void
scorep_rewind_stack_push( uint32_t id,
                          uint64_t entertimestamp );


/**
 * Removes the head element from the stack and returns the removed id and
 * timestamp of the original enter event for the rewind region.
 * The function manipulates the stack_head pointer.
 *
 * @param  id               Address to store the removed id as a return value
 *                          of this function.
 * @param  entertimestamp   Address to store the removed time stamp as a return
 *                          value of this function.
 */
void
scorep_rewind_stack_pop( uint32_t * id,
                         uint64_t * entertimestamp,
                         bool paradigm_affected[ SCOREP_PARADIGM_MAX ] );


/**
 * Remove all stack elements and assign NULL to the stack_head pointer.
 */
void
scorep_rewind_stack_delete();


/**
 * Set paradigm event flag for all current rewind regions.
 *
 * @param  location     Location to flag the rewind regions
 *
 * @param  paradigm     used paradigm
 *
 * Don't call SCOREP_Location_GetCurrentCPULocation
 * in scorep_rewind_set_affected_paradigm since this leads to
 * case omp_get_num_threads()==1 for SCOREP_OmpFork events!
 */
void
scorep_rewind_set_affected_paradigm( SCOREP_Location* location,
                                     SCOREP_Paradigm  paradigm );

#endif /* SCOREP_INTERNAL_REWIND_STACK_MANAGEMENT_H */
