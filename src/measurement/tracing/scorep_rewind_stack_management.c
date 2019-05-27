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
 * Copyright (c) 2009-2013, 2015,
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


/**
 * @file
 *
 *
 *
 */

#include <config.h>

#include <string.h>

#include <UTILS_Error.h>

#include "scorep_rewind_stack_management.h"
#include <scorep_location_management.h>

#include <otf2/otf2.h>
#include "scorep_tracing_internal.h"

#include <UTILS_Error.h>


/**
 * Search for a specific id of a rewind region in the rewind stack.
 */
bool
scorep_rewind_stack_find( SCOREP_Location* location,
                          uint32_t         id )
{
    scorep_rewind_stack* stack_item =
        scorep_tracing_get_trace_data( location )->rewind_stack;

    while ( stack_item && stack_item->id != id )
    {
        stack_item = stack_item->prev;
    }

    if ( stack_item && stack_item->id == id )
    {
        return true;
    }

    return false;
}


/**
 * Save the id of the rewind region handle with it's timestamp on the stack.
 * It's not a real stack, the elements are unique, no multiple id is allowed.
 * The function manipulates the stack_head pointer.
 */
void
scorep_rewind_stack_push( SCOREP_Location* location,
                          uint32_t         id,
                          uint64_t         entertimestamp )
{
    SCOREP_TracingData*  tracing_data    = scorep_tracing_get_trace_data( location );
    scorep_rewind_stack* stack_head      = tracing_data->rewind_stack;
    scorep_rewind_stack* stack_item      = stack_head;
    scorep_rewind_stack* last_stack_item = stack_head;

    /* If the id was stored before, then */
    if ( scorep_rewind_stack_find( location, id ) )
    {
        /* Search for the id in the stack */
        while ( stack_item && stack_item->id != id )
        {
            last_stack_item = stack_item;
            stack_item      = stack_item->prev;
        }
        /* If id is NOT on top of the stack, then move id on top and
         * fix broken stack */
        if ( last_stack_item != stack_head )
        {
            last_stack_item->prev = stack_item->prev;
            stack_item->prev      = stack_head;
            stack_head            = stack_item;
        }
        /* And renew timestamp for id */
        stack_head->entertimestamp = entertimestamp;
    }
    else
    {
        /* else, push the new id on top of the stack */
        stack_item = tracing_data->rewind_free_list;
        if ( stack_item )
        {
            tracing_data->rewind_free_list = stack_item->prev;
        }
        else
        {
            stack_item = SCOREP_Location_AllocForMisc( location,
                                                       sizeof( *stack_item ) );
        }
        stack_item->id             = id;
        stack_item->entertimestamp = entertimestamp;
        for ( int i = 0; i < SCOREP_REWIND_PARADIGM_MAX; i++ )
        {
            stack_item->paradigm_affected[ i ] = false;
        }
        stack_item->prev = stack_head;
        stack_head       = stack_item;
    }

    tracing_data->rewind_stack = stack_head;
}


/**
 * Removes the head element from the stack and returns the removed id and
 * timestamp of the original enter event for the rewind region.
 * The function manipulates the stack_head pointer.
 */
void
scorep_rewind_stack_pop( SCOREP_Location* location,
                         uint32_t*        id,
                         uint64_t*        entertimestamp,
                         bool*            paradigmAffected /* [ SCOREP_REWIND_PARADIGM_MAX ] */ )
{
    SCOREP_TracingData*  tracing_data = scorep_tracing_get_trace_data( location );
    scorep_rewind_stack* stack_head   = tracing_data->rewind_stack;
    scorep_rewind_stack* stack_item   = NULL;

    if ( stack_head )
    {
        *id             = stack_head->id;
        *entertimestamp = stack_head->entertimestamp;
        memcpy( paradigmAffected, stack_head->paradigm_affected,
                sizeof( bool ) * SCOREP_REWIND_PARADIGM_MAX );

        stack_item = stack_head;
        stack_head = stack_head->prev;

        stack_item->prev               = tracing_data->rewind_free_list;
        tracing_data->rewind_free_list = stack_item;
    }

    tracing_data->rewind_stack = stack_head;
}


/**
 * Remove all stack elements and assign NULL to the stack_head pointer.
 */
void
scorep_rewind_stack_delete( SCOREP_Location* location )
{
    SCOREP_TracingData*  tracing_data = scorep_tracing_get_trace_data( location );
    scorep_rewind_stack* stack_head   = tracing_data->rewind_stack;
    scorep_rewind_stack* stack_item   = NULL;

    /* Iterate over all elements and free the allocated buffer. */
    while ( stack_head )
    {
        stack_item = stack_head;
        stack_head = stack_head->prev;

        stack_item->prev               = tracing_data->rewind_free_list;
        tracing_data->rewind_free_list = stack_item;
    }

    tracing_data->rewind_stack = NULL;
}

/**
 * Set paradigm event flag for all current rewind regions.
 */
void
scorep_rewind_set_affected_paradigm( SCOREP_Location* location, SCOREP_Rewind_Paradigm paradigm )
{
    UTILS_ASSERT( paradigm < SCOREP_REWIND_PARADIGM_MAX );

    SCOREP_TracingData*  tracing_data = scorep_tracing_get_trace_data( location );
    scorep_rewind_stack* stack_item   = tracing_data->rewind_stack;

    while ( stack_item )
    {
        stack_item->paradigm_affected[ paradigm ] = true;

        stack_item = stack_item->prev;
    }
}
