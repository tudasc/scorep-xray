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
 * @status      alpha
 * @file        scorep_profile_task_table.c
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief   Implementation of the tasking internal data structure to manage task
 *          instances.
 *
 */


#include <config.h>
#include <stdlib.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Memory.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Definitions.h>
#include <scorep_thread.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>
#include <scorep_profile_task_table.h>

/* **************************************************************************************
   Internal data table
****************************************************************************************/

SCOREP_MetricHandle scorep_profile_active_task_metric = SCOREP_INVALID_METRIC;

static int32_t scorep_profile_has_tasks_flag = 0;

/**
   Define name for the task table entry
 */
typedef struct scorep_profile_task_table_entry scorep_profile_task_table_entry;

/**
   Entry in the task table.
 */
struct scorep_profile_task_table_entry
{
    scorep_profile_task_table_entry* next;
    scorep_profile_task_t            task_id;
    scorep_profile_node*             current_node;
    uint32_t                         depth;
};

struct scorep_profile_task_table
{
    scorep_profile_task_table_entry** items;
    uint64_t                          size;
    scorep_profile_task_table_entry*  free_entries;
    uint64_t                          fill_level;
    uint64_t                          max_tasks;
};

static scorep_profile_task_table_entry*
scorep_profile_task_find( scorep_profile_task_table* table,
                          scorep_profile_task_t      task_id )
{
    uint64_t hash = SCOREP_Hashtab_HashInt64( &task_id ) % table->size;

    scorep_profile_task_table_entry* entry = table->items[ hash ];
    while ( entry != NULL )
    {
        if ( entry->task_id == task_id )
        {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}

static void
scorep_profile_task_insert( scorep_profile_task_table* table,
                            scorep_profile_task_t      task_id,
                            scorep_profile_node*       current_node,
                            uint32_t                   depth )
{
    /* Try to recycle older entry or allocate new memory */
    scorep_profile_task_table_entry* new_entry = NULL;
    if ( table->free_entries != NULL )
    {
        new_entry           = table->free_entries;
        table->free_entries =  new_entry->next;
    }
    else
    {
        new_entry = ( scorep_profile_task_table_entry* )
                    SCOREP_Memory_AllocForProfile( sizeof( scorep_profile_task_table_entry ) );
    }
    if ( new_entry == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Failed to allocate memory for task instance tracking." );
        SCOREP_PROFILE_STOP;
    }

    /* Initialize entry */
    new_entry->task_id      = task_id;
    new_entry->current_node = current_node;
    new_entry->depth        = depth;

    /* Insert entry in table */
    uint64_t hash = SCOREP_Hashtab_HashInt64( &task_id ) % table->size;
    new_entry->next      = table->items[ hash ];
    table->items[ hash ] = new_entry;

    /* Update statistics */
    table->fill_level++;
    table->max_tasks = ( table->max_tasks < table->fill_level ?
                         table->fill_level :
                         table->max_tasks );
}

/* **************************************************************************************
   Data access interface
****************************************************************************************/

void
scorep_profile_remove_task( scorep_profile_task_table* table,
                            scorep_profile_task_t      task_id )
{
    uint64_t hash = SCOREP_Hashtab_HashInt64( &task_id ) % table->size;

    /* Find and remove entry */
    scorep_profile_task_table_entry* last  = NULL;
    scorep_profile_task_table_entry* entry = table->items[ hash ];
    while ( entry != NULL )
    {
        if ( entry->task_id == task_id )
        {
            /* Remove entry from hash table */
            if ( last == NULL )
            {
                table->items[ hash ] = entry->next;
            }
            else
            {
                last->next = entry->next;
            }

            /* Enter entry to the list of recyclable entries */
            entry->next         = table->free_entries;
            table->free_entries = entry;

            /* Update statistics */
            table->fill_level--;
            return;
        }

        /* Jump to next entry */
        last  = entry;
        entry = entry->next;
    }
}

void
scorep_profile_task_initialize( SCOREP_Profile_LocationData* location )
{
    location->tasks = ( scorep_profile_task_table* )
                      SCOREP_Memory_AllocForProfile( sizeof( scorep_profile_task_table ) );
    if ( location->tasks == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Failed to allocate memory for task table." );
        SCOREP_PROFILE_STOP;
        return;
    }

    /* The task table must have at least one bin, else we need a lot of extra checks for
       this special case, or the program seg faults when a task occur.
     */
    if ( scorep_profile.task_table_size == 0 )
    {
        scorep_profile.task_table_size = 1;
    }

    scorep_profile_task_table_entry** items =
        ( scorep_profile_task_table_entry** )
        SCOREP_Memory_AllocForProfile( sizeof( scorep_profile_task_table_entry* ) *
                                       scorep_profile.task_table_size );

    if ( items == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Failed to allocate memory for task table." );
        SCOREP_PROFILE_STOP;
        return;
    }

    for ( int32_t i = 0; i < scorep_profile.task_table_size; i++ )
    {
        items[ i ] = NULL;
    }

    location->tasks->size         = scorep_profile.task_table_size;
    location->tasks->items        = items;
    location->tasks->free_entries = NULL;
    location->tasks->fill_level   = 0;
    location->tasks->max_tasks    = 0;

    /* Initialize metric */
    if ( scorep_profile_active_task_metric == SCOREP_INVALID_METRIC )
    {
        scorep_profile_active_task_metric =
            SCOREP_DefineMetric( "max active tasks",
                                 "Maximum number of tasks which have started execution "
                                 "but are not yet done at the same point of time.",
                                 SCOREP_METRIC_SOURCE_TYPE_TASK,
                                 SCOREP_METRIC_MODE_ACCUMULATED_START,
                                 SCOREP_METRIC_VALUE_DOUBLE,
                                 SCOREP_METRIC_BASE_DECIMAL,
                                 0,
                                 "",
                                 SCOREP_METRIC_PROFILING_TYPE_MAX );
    }
}


void
scorep_profile_task_finalize( SCOREP_Profile_LocationData* location )
{
    location->tasks = NULL;
}

void
scorep_profile_create_task( SCOREP_Profile_LocationData* location,
                            scorep_profile_task_t        task_id,
                            scorep_profile_node*         task_root )
{
    scorep_profile_task_insert( location->tasks, task_id, task_root, 1 );
}

void
scorep_profile_store_task( SCOREP_Profile_LocationData* location )
{
    if ( scorep_profile_is_implicit_task( location, location->current_task_id ) )
    {
        location->implicit_depth        = location->current_depth;
        location->current_implicit_node = location->current_task_node;
        return;
    }

    scorep_profile_task_table_entry* task_entry =
        scorep_profile_task_find( location->tasks, location->current_task_id );

    if ( task_entry == NULL )
    {
        SCOREP_PROFILE_STOP;
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Encountered unknown task ID" );
        return;
    }

    task_entry->depth        = location->current_depth;
    task_entry->current_node = location->current_task_node;
}

void
scorep_profile_restore_task( SCOREP_Profile_LocationData* location )
{
    if ( scorep_profile_is_implicit_task( location, location->current_task_id ) )
    {
        location->current_depth     = location->implicit_depth;
        location->current_task_node = location->current_implicit_node;
        return;
    }

    scorep_profile_task_table_entry* task_entry =
        scorep_profile_task_find( location->tasks, location->current_task_id );

    if ( task_entry == NULL )
    {
        SCOREP_PROFILE_STOP;
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Encountered unknown task ID" );
        return;
    }

    location->current_depth     = task_entry->depth;
    location->current_task_node = task_entry->current_node;
}

bool
scorep_profile_is_implicit_task( SCOREP_Profile_LocationData* location,
                                 scorep_profile_task_t        task_id )
{
    return task_id == SCOREP_PROFILE_IMPLICIT_TASK_ID;
}

void
scorep_profile_task_parallel_exit( SCOREP_Thread_LocationData* thread )
{
    SCOREP_Profile_LocationData* location =
        SCOREP_Thread_GetProfileLocationData( thread );

    if ( location->tasks->max_tasks > 0 )
    {
        SCOREP_Profile_TriggerDouble( thread,
                                      scorep_profile_active_task_metric,
                                      location->tasks->max_tasks );
        location->tasks->max_tasks    = 0;
        scorep_profile_has_tasks_flag = 1;
    }
}

int32_t
scorep_profile_has_tasks()
{
    return scorep_profile_has_tasks_flag;
}
