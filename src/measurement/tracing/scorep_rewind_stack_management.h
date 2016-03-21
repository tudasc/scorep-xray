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
 * Copyright (c) 2009-2014,
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

#ifndef SCOREP_INTERNAL_REWIND_STACK_MANAGEMENT_H
#define SCOREP_INTERNAL_REWIND_STACK_MANAGEMENT_H



/**
 * @file
 *
 *
 *
 */


#include <stdbool.h>
#include <stdlib.h>

#include <SCOREP_Location.h>


/**
 * List of known rewind-related paradigms.
 */
typedef enum
{
    SCOREP_REWIND_PARADIGM_MPI,
    SCOREP_REWIND_PARADIGM_THREAD_FORK_JOIN,
    SCOREP_REWIND_PARADIGM_THREAD_CREATE_WAIT,
    SCOREP_REWIND_PARADIGM_THREAD_LOCKING,
    SCOREP_REWIND_PARADIGM_MAX
} SCOREP_Rewind_Paradigm;


typedef struct scorep_rewind_stack scorep_rewind_stack;


struct scorep_rewind_stack
{
    uint32_t             id;
    uint64_t             entertimestamp;
    scorep_rewind_stack* prev;

    /** paradigm event occurred in rewind region */
    bool paradigm_affected[ SCOREP_REWIND_PARADIGM_MAX ];
};


/**
 * Search for a specific id of a rewind region in the rewind stack.
 *
 * @param  location     Location to look for the id.
 * @param  id           The identification number of the rewind region.
 *
 * @return Boolean, whether the id was found in the stack or not.
 */
bool
scorep_rewind_stack_find( SCOREP_Location* location,
                          uint32_t         id );


/**
 * Save the id of the rewind region handle with it's timestamp on the stack.
 * It's not a real stack, the elements are unique, no multiple id is allowed.
 * The function manipulates the stack_head pointer.
 *
 * @param  location         Location where to push the id.
 * @param  id               The identification number of the rewind region.
 * @param  entertimestamp   Time stamp, when the rewind region was entered.
 */
void
scorep_rewind_stack_push( SCOREP_Location* location,
                          uint32_t         id,
                          uint64_t         entertimestamp );


/**
 * Removes the head element from the stack and returns the removed id and
 * timestamp of the original enter event for the rewind region.
 * The function manipulates the stack_head pointer.
 *
 * @param  location              Location where to pop the id.
 * @param  id                    Address to store the removed id as a return
 *                               value of this function.
 * @param  entertimestamp        Address to store the removed time stamp as a
 *                               return value of this function.
 * @param[out] paradigmAffected  The affected paradigms of the top level entry.
 */
void
scorep_rewind_stack_pop( SCOREP_Location* location,
                         uint32_t*        id,
                         uint64_t*        entertimestamp,
                         bool*            paradigmAffected /* [ SCOREP_REWIND_PARADIGM_MAX ] */ );


/**
 * Remove all stack elements and assign NULL to the stack_head pointer.
 *
 * @param location      Location where to remove the stack.
 *
 */
void
scorep_rewind_stack_delete( SCOREP_Location* location );


/**
 * Set paradigm event flag for all current rewind regions.
 *
 * @param  location     Location to flag the rewind regions
 *
 * @param  paradigm     used paradigm
 *
 */
void
scorep_rewind_set_affected_paradigm( SCOREP_Location*       location,
                                     SCOREP_Rewind_Paradigm paradigm );

#endif /* SCOREP_INTERNAL_REWIND_STACK_MANAGEMENT_H */
