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

/**
 * @file        scorep_profile_location.c
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status      alpha
 * @brief Contains implmentation for profile location data
 *
 */

#include <config.h>
#include <scorep_profile_location.h>
#include <SCOREP_Memory.h>
#include <UTILS_Error.h>

struct scorep_profile_fork_list_node
{
    scorep_profile_node*           fork_node;
    uint32_t                       nesting_level;
    uint32_t                       profile_depth;
    scorep_profile_fork_list_node* prev;
    scorep_profile_fork_list_node* next;
};

scorep_profile_node*
scorep_profile_get_current_node( SCOREP_Profile_LocationData* location )
{
    return location->current_task_node;
}

void
scorep_profile_set_current_node( SCOREP_Profile_LocationData* location,
                                 scorep_profile_node*         node )
{
    location->current_task_node = node;
}

void
scorep_profile_set_num_location_metrics( SCOREP_Profile_LocationData* location,
                                         uint32_t                     numLocationSpecificMetrics )
{
    location->num_location_specific_metrics = numLocationSpecificMetrics;
}

uint32_t
scorep_profile_get_num_location_metrics( SCOREP_Profile_LocationData* location )
{
    return location->num_location_specific_metrics;
}

void
scorep_profile_reinitialize_location( SCOREP_Profile_LocationData* location )
{
    /* Initialize locations task instance table */
    scorep_profile_task_initialize( location->location_data, location );
}

void
scorep_profile_finalize_location( SCOREP_Profile_LocationData* location )
{
    location->current_task          = SCOREP_PROFILE_IMPLICIT_TASK;
    location->current_task_node     = location->root_node;
    location->current_implicit_node = location->root_node;
    location->current_depth         = 0;
    location->implicit_depth        = 0;
    location->fork_list_head        = NULL;
    location->fork_list_tail        = NULL;
    location->free_nodes            = NULL;
    location->free_int_metrics      = NULL;
    location->free_double_metrics   = NULL;
    scorep_profile_task_finalize( location );
}

SCOREP_Profile_LocationData*
scorep_profile_create_location_data( SCOREP_Location* locationData )
{
    /* Create location data structure.
     * The location data structure must not be deleted when the profile is reset
     * in a persicope phase. Thus the memory is not allocated from the profile
     * memory pages.
     */
    SCOREP_Profile_LocationData* location
        = SCOREP_Location_AllocForMisc( locationData, sizeof( *location ) );

    /* Set default values. */
    location->current_implicit_node         = NULL;
    location->root_node                     = NULL;
    location->fork_list_head                = NULL;
    location->fork_list_tail                = NULL;
    location->creation_node                 = NULL;
    location->current_depth                 = 0;
    location->implicit_depth                = 0;
    location->num_location_specific_metrics = 0;
    location->free_nodes                    = NULL;
    location->free_int_metrics              = NULL;
    location->free_double_metrics           = NULL;
    location->current_task_node             = NULL;
    location->current_task                  = SCOREP_PROFILE_IMPLICIT_TASK;
    location->location_data                 = locationData;

    /* Initialize locations task instance table */
    scorep_profile_task_initialize( locationData, location );

    return location;
}


void
scorep_profile_delete_location_data( SCOREP_Profile_LocationData* location )
{
    if ( location == NULL )
    {
        return;
    }

    /* Finalize locations task instance table */
    scorep_profile_task_finalize( location );
}

static scorep_profile_fork_list_node*
create_fork_list_item( SCOREP_Profile_LocationData* location )
{
    scorep_profile_fork_list_node* new_list_item =
        SCOREP_Location_AllocForProfile( location->location_data, sizeof(  scorep_profile_fork_list_node ) );
    new_list_item->next = NULL;

    /* Append to end of list */
    if ( location->fork_list_tail == NULL )
    {
        new_list_item->prev      = NULL;
        location->fork_list_head = new_list_item;
    }
    else
    {
        new_list_item->prev            = location->fork_list_tail;
        location->fork_list_tail->next = new_list_item;
    }
    return new_list_item;
}

void
scorep_profile_add_fork_node( SCOREP_Profile_LocationData* location,
                              scorep_profile_node*         fork_node,
                              uint32_t                     profile_depth,
                              uint32_t                     nesting_level )
{
    /* Create or reuse new list item */
    scorep_profile_fork_list_node* new_list_item = NULL;
    if ( location->fork_list_tail == NULL )
    {
        if ( location->fork_list_head == NULL )
        {
            new_list_item = create_fork_list_item( location );
        }
        else
        {
            new_list_item = location->fork_list_head;
        }
    }
    else
    {
        if ( location->fork_list_tail->next == NULL )
        {
            new_list_item = create_fork_list_item( location );
        }
        else
        {
            new_list_item = location->fork_list_tail->next;
            if ( new_list_item == NULL )
            {
                new_list_item = create_fork_list_item( location );
            }
        }
    }

    UTILS_ASSERT( new_list_item );

    /* Initialize entry */
    new_list_item->fork_node     = fork_node;
    new_list_item->nesting_level = nesting_level;
    new_list_item->profile_depth = profile_depth;

    location->fork_list_tail = new_list_item;
}

void
scorep_profile_remove_fork_node( SCOREP_Profile_LocationData* location )
{
    /* The parallel regions should appear in sorted order */
    UTILS_ASSERT( location );
    if ( location->fork_list_tail == NULL )
    {
        return;
    }

    /* Move tail pointer to the previous element, the list element stays
       for later reuse. */
    location->fork_list_tail = location->fork_list_tail->prev;
}

scorep_profile_node*
scorep_profile_get_fork_node( SCOREP_Profile_LocationData* location,
                              uint32_t                     nesting_level )
{
    /* We assume that the fork node is never removed before this child thread
       is completed. And the list is sorted.  Thus the searched part is save
       to read. */
    scorep_profile_fork_list_node* current = location->fork_list_tail;
    while ( ( current != NULL ) && ( current->nesting_level > nesting_level ) )
    {
        current = current->prev;
    }
    if ( current == NULL )
    {
        return NULL;
    }
    //UTILS_ASSERT( current->nesting_level == nesting_level );
    return current->fork_node;
}

uint32_t
scorep_profile_get_fork_depth( SCOREP_Profile_LocationData* location,
                               uint32_t                     nesting_level )
{
    /* We assume that the fork node is never removed before this child thread
       is completed. And the list is sorted.  Thus the searched part is save
       to read. */
    scorep_profile_fork_list_node* current = location->fork_list_tail;
    while ( ( current != NULL ) && ( current->nesting_level > nesting_level ) )
    {
        current = current->prev;
    }
    if ( current == NULL )
    {
        return 0;
    }
    //UTILS_ASSERT( current->nesting_level == nesting_level );
    return current->profile_depth;
}
