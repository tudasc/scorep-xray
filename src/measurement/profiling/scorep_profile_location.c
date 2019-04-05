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
 * Copyright (c) 2009-2013, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2015,
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
 * @brief Contains implementation for profile location data
 *
 */

#include <config.h>
#include <scorep_profile_location.h>
#include <scorep_profile_io.h>
#include <scorep_profile_task_switch.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Task.h>
#include <UTILS_Error.h>

#include <assert.h>

/* **************************************************************************************
 *                                                              Local types and variables
 * *************************************************************************************/

struct scorep_profile_fork_list_node
{
    scorep_profile_node*           fork_node;
    uint32_t                       fork_sequence_count;
    uint32_t                       profile_depth;
    scorep_profile_fork_list_node* prev;
    scorep_profile_fork_list_node* next;
};

static scorep_profile_task* scorep_profile_task_exchange = NULL;

static SCOREP_Mutex scorep_task_object_exchange_lock;

static scorep_profile_node* scorep_profile_stub_exchange = NULL;

static SCOREP_Mutex scorep_stub_exchange_lock;

size_t scorep_profile_substrate_id;

/* **************************************************************************************
 *                                                                            Data access
 * *************************************************************************************/

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

/* **************************************************************************************
 *                                                          Initialization / Finalization
 * *************************************************************************************/

void
scorep_profile_initialize_exchange( void )
{
    SCOREP_MutexCreate( &scorep_task_object_exchange_lock );
    SCOREP_MutexCreate( &scorep_stub_exchange_lock );
    scorep_profile_task_exchange = NULL;
    scorep_profile_stub_exchange = NULL;
}

void
scorep_profile_finalize_exchange( void )
{
    SCOREP_MutexDestroy( &scorep_task_object_exchange_lock );
    SCOREP_MutexDestroy( &scorep_stub_exchange_lock );
}

void
scorep_profile_reinitialize_location( SCOREP_Profile_LocationData* location )
{
    SCOREP_Location*  location_data = location->location_data;
    SCOREP_TaskHandle task_handle   = SCOREP_Task_GetCurrentTask( location_data );
    SCOREP_Profile_CreateTaskData( location_data, task_handle );
}

void
scorep_profile_finalize_location( SCOREP_Profile_LocationData* location )
{
    location->current_task          = NULL;
    location->current_task_node     = location->root_node;
    location->current_implicit_node = location->root_node;
    location->current_depth         = 0;
    location->implicit_depth        = 0;
    location->fork_list_head        = NULL;
    location->fork_list_tail        = NULL;
    location->free_nodes            = NULL;
    location->free_int_metrics      = NULL;
    location->free_double_metrics   = NULL;
    location->free_tasks            = NULL;
    location->foreign_tasks         = NULL;
    location->free_stubs            = NULL;
    location->foreign_stubs         = NULL;
    location->num_foreign_tasks     = 0;
    location->num_foreign_stubs     = 0;
}


/**
 * Allocate and initialize a valid SCOREP_Profile_LocationData object.
 *
 */
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
    location->current_implicit_node = NULL;
    location->root_node             = NULL;
    location->fork_list_head        = NULL;
    location->fork_list_tail        = NULL;
    location->creation_node         = NULL;
    location->current_depth         = 0;
    location->implicit_depth        = 0;
    location->free_nodes            = NULL;
    location->free_int_metrics      = NULL;
    location->free_double_metrics   = NULL;
    location->current_task_node     = NULL;
    location->current_task          = NULL;
    location->implicit_task         = NULL;
    location->free_tasks            = NULL;
    location->foreign_tasks         = NULL;
    location->free_stubs            = NULL;
    location->num_foreign_tasks     = 0;
    location->num_foreign_stubs     = 0;
    location->location_data         = locationData;
    location->migration_sum         = 1;
    location->migration_win         = 0;
    location->current_io_paradigm   = NULL;

    scorep_profile_io_init_location( location );

    return location;
}


void
scorep_profile_delete_location_data( SCOREP_Profile_LocationData* location )
{
}

/* **************************************************************************************
 *                                                                     Fork data handling
 * *************************************************************************************/

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
                              scorep_profile_node*         forkNode,
                              uint32_t                     profileDepth,
                              uint32_t                     forkSequenceCount )
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
    new_list_item->fork_node           = forkNode;
    new_list_item->fork_sequence_count = forkSequenceCount;
    new_list_item->profile_depth       = profileDepth;

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
                              uint32_t                     forkSequenceCount )
{
    /* We assume that the fork node is never removed before this child thread
       is completed. And the list is sorted.  Thus the searched part is save
       to read. */
    scorep_profile_fork_list_node* current = location->fork_list_tail;
    while ( ( current != NULL ) && ( current->fork_sequence_count > forkSequenceCount ) )
    {
        current = current->prev;
    }
    if ( current == NULL )
    {
        return NULL;
    }
    //UTILS_ASSERT( current->fork_sequence_count == forkSequenceCount );
    return current->fork_node;
}

uint32_t
scorep_profile_get_fork_depth( SCOREP_Profile_LocationData* location,
                               uint32_t                     forkSequenceCount )
{
    /* We assume that the fork node is never removed before this child thread
       is completed. And the list is sorted.  Thus the searched part is save
       to read. */
    scorep_profile_fork_list_node* current = location->fork_list_tail;
    while ( ( current != NULL ) && ( current->fork_sequence_count > forkSequenceCount ) )
    {
        current = current->prev;
    }
    if ( current == NULL )
    {
        return 0;
    }
    //UTILS_ASSERT( current->fork_sequence_count == forkSequenceCount );
    return current->profile_depth;
}

/* **************************************************************************************
 *                                                                       Memory recycling
 * *************************************************************************************/

void
scorep_profile_release_stubs( SCOREP_Profile_LocationData* location,
                              scorep_profile_node*         root,
                              scorep_profile_node*         leaf,
                              uint32_t                     num,
                              bool                         localObjects )
{
    assert( root );
    assert( leaf );
    if ( localObjects )
    {
        if ( location->free_stubs != NULL )
        {
            scorep_profile_add_child( leaf, location->free_stubs );
        }
        location->free_stubs = root;
    }
    else
    {
        if ( location->foreign_stubs != NULL )
        {
            scorep_profile_add_child( leaf,  location->foreign_stubs );
        }
        location->foreign_stubs      = root;
        location->num_foreign_stubs += num;

        if ( location->num_foreign_stubs > scorep_profile_get_task_exchange_num() )
        {
            UTILS_WARNING( "Collected too many foreign stub objects. Trigger backflow "
                           "of stub objects. This requires locking and, thus, can "
                           "have an impact on the behavior of your application. You "
                           "can influence the frequency of the backflow by specifying "
                           "a higher value in SCOREP_PROFILE_TASK_EXCHANGE_NUM." );

            scorep_profile_node* current = leaf;
            while ( current->first_child != NULL )
            {
                current = current->first_child;
            }
            SCOREP_MutexLock( scorep_stub_exchange_lock );
            if ( scorep_profile_stub_exchange != NULL )
            {
                scorep_profile_add_child( current, scorep_profile_stub_exchange );
            }
            scorep_profile_stub_exchange = root;
            SCOREP_MutexUnlock( scorep_stub_exchange_lock );

            location->foreign_stubs     = NULL;
            location->num_foreign_stubs = 0;
        }
    }
}

scorep_profile_node*
scorep_profile_recycle_stub( SCOREP_Profile_LocationData* location )
{
    scorep_profile_node* new_stub = NULL;

    if ( location->free_stubs != NULL )
    {
        new_stub             = location->free_stubs;
        location->free_stubs = location->free_stubs->first_child;
        return new_stub;
    }
    if ( location->foreign_stubs != NULL )
    {
        new_stub                = location->foreign_stubs;
        location->foreign_stubs = location->foreign_stubs->first_child;
        location->num_foreign_stubs--;
        return new_stub;
    }
    if ( scorep_profile_stub_exchange != NULL )
    {
        SCOREP_MutexLock( scorep_stub_exchange_lock );
        if ( scorep_profile_stub_exchange != NULL )
        {
            location->free_stubs         = scorep_profile_stub_exchange;
            scorep_profile_stub_exchange = NULL;
        }
        SCOREP_MutexUnlock( scorep_stub_exchange_lock );
        if ( location->free_stubs != NULL )
        {
            new_stub             = location->free_stubs;
            location->free_stubs = location->free_stubs->first_child;
            return new_stub;
        }
    }
    return NULL;
}

void
scorep_profile_release_task( SCOREP_Profile_LocationData* location,
                             scorep_profile_task*         task )
{
    assert( task );
    if ( task->creator == location )
    {
        /* task did not migrate (or at least migrated back at the end) */
        task->next           = location->free_tasks;
        location->free_tasks = task;
    }
    else
    {
        /* task did migrate */
        task->next              = location->foreign_tasks;
        location->foreign_tasks = task;
        location->num_foreign_tasks++;

        if ( location->num_foreign_tasks > scorep_profile_get_task_exchange_num() )
        {
            UTILS_WARNING( "Collected too many foreign task objects. Trigger backflow "
                           "of task objects. This requires locking and, thus, can "
                           "have an impact on the behavior of your application. You "
                           "can influence the frequency of the backflow by specifying "
                           "a higher value in SCOREP_PROFILE_TASK_EXCHANGE_NUM." );

            scorep_profile_task* current = task;
            while ( current->next != NULL )
            {
                current = current->next;
            }
            SCOREP_MutexLock( scorep_task_object_exchange_lock );
            current->next                = scorep_profile_task_exchange;
            scorep_profile_task_exchange = task;
            SCOREP_MutexUnlock( scorep_task_object_exchange_lock );

            location->foreign_tasks     = NULL;
            location->num_foreign_tasks = 0;
        }
    }
}

scorep_profile_task*
scorep_profile_recycle_task( SCOREP_Profile_LocationData* location )
{
    scorep_profile_task* new_task = NULL;

    /* Check for released entries */
    if ( location->free_tasks != NULL )
    {
        new_task             = location->free_tasks;
        location->free_tasks = new_task->next;
    }
    else if ( location->foreign_tasks != NULL )
    {
        new_task                = location->foreign_tasks;
        location->foreign_tasks = new_task->next;
        location->num_foreign_tasks--;
    }
    else if ( scorep_profile_task_exchange != NULL )
    {
        SCOREP_MutexLock( scorep_task_object_exchange_lock );
        if ( scorep_profile_task_exchange != NULL )
        {
            new_task                     = scorep_profile_task_exchange;
            scorep_profile_task_exchange = NULL;
        }
        SCOREP_MutexUnlock( scorep_task_object_exchange_lock );
        if ( new_task != NULL )
        {
            location->free_tasks = new_task->next;
        }
    }
    return new_task;
}
