/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
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
 * @brief   Implementation of the tasking internal data structure to manage task
 *          instances.
 *
 */


#include <config.h>
#include <scorep_profile_task_table.h>
#include <stdlib.h>
#include <string.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Memory.h>
#include <UTILS_Error.h>
#include <SCOREP_Definitions.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>
#include <scorep_profile_event_base.h>

/* **************************************************************************************
   Internal data table
****************************************************************************************/

SCOREP_MetricHandle scorep_profile_active_task_metric = SCOREP_INVALID_METRIC;

static int32_t scorep_profile_has_tasks_flag = 0;

/**
   Hash table for active tasks.
 */
struct scorep_profile_task_table
{
    uint64_t             size;
    scorep_profile_task* free_entries;
    uint64_t             fill_level;
    uint64_t             max_tasks;
    scorep_profile_task* items[];
};

static scorep_profile_task*
insert_task( SCOREP_Profile_LocationData* location,
             scorep_profile_task_table*   table,
             scorep_profile_task_id       task_id,
             scorep_profile_node*         current_node,
             uint32_t                     depth )
{
    /* Try to recycle older entry or allocate new memory */
    scorep_profile_task* new_entry = NULL;
    if ( table->free_entries != NULL )
    {
        new_entry           = table->free_entries;
        table->free_entries =  new_entry->next;
    }
    else
    {
        new_entry = ( scorep_profile_task* )
                    SCOREP_Location_AllocForProfile( location->location_data, sizeof( scorep_profile_task ) );
    }
    if ( new_entry == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Failed to allocate memory for task instance tracking." );
        SCOREP_PROFILE_STOP( location );
        return NULL;
    }

    /* Initialize entry */
    new_entry->task_id      = task_id;
    new_entry->current_node = current_node;
    new_entry->root_node    = current_node;
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

    return new_entry;
}

/* **************************************************************************************
   Data access interface
****************************************************************************************/

scorep_profile_task*
scorep_profile_task_find( SCOREP_Profile_LocationData* location,
                          scorep_profile_task_id       task_id )
{
    scorep_profile_task_table* table = location->tasks;

    if ( task_id == SCOREP_PROFILE_IMPLICIT_TASK_ID )
    {
        return NULL;
    }

    uint64_t hash = SCOREP_Hashtab_HashInt64( &task_id ) % table->size;

    scorep_profile_task* entry = table->items[ hash ];
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

void
scorep_profile_remove_task( SCOREP_Profile_LocationData* location,
                            scorep_profile_task_id       task_id )
{
    scorep_profile_task_table* table = location->tasks;

    uint64_t hash = SCOREP_Hashtab_HashInt64( &task_id ) % table->size;

    /* Find and remove entry */
    scorep_profile_task* last  = NULL;
    scorep_profile_task* entry = table->items[ hash ];
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
scorep_profile_task_initialize( SCOREP_Location*             locationData,
                                SCOREP_Profile_LocationData* location )
{
    /* The task table must have at least one bin, else we need a lot of extra checks for
       this special case, or the program seg faults when a task occur.
     */
    uint64_t table_bins = scorep_profile_get_task_table_size();
    if ( table_bins == 0 )
    {
        table_bins = 1;
    }

    size_t task_table_size = sizeof( scorep_profile_task* ) * table_bins;
    location->tasks = SCOREP_Location_AllocForProfile(
        locationData,
        sizeof( *location->tasks ) + task_table_size );
    memset( location->tasks->items, 0, task_table_size );

    location->tasks->size         = table_bins;
    location->tasks->free_entries = NULL;
    location->tasks->fill_level   = 0;
    location->tasks->max_tasks    = 0;

    /* Initialize metric */
    if ( scorep_profile_active_task_metric == SCOREP_INVALID_METRIC )
    {
        scorep_profile_active_task_metric =
            SCOREP_Definitions_NewMetric( "max active tasks",
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

scorep_profile_task*
scorep_profile_create_task( SCOREP_Profile_LocationData* location,
                            scorep_profile_task_id       task_id,
                            scorep_profile_node*         task_root )
{
    return insert_task( location, location->tasks, task_id, task_root, 1 );
}

void
scorep_profile_store_task( SCOREP_Profile_LocationData* location )
{
    if ( scorep_profile_is_implicit_task( location, location->current_task ) )
    {
        location->implicit_depth        = location->current_depth;
        location->current_implicit_node = location->current_task_node;
        return;
    }

    scorep_profile_task* current_task = location->current_task;

    if ( current_task == NULL )
    {
        SCOREP_PROFILE_STOP( location );
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Encountered unknown task ID" );
        return;
    }

    current_task->depth        = location->current_depth;
    current_task->current_node = location->current_task_node;
}

void
scorep_profile_restore_task( SCOREP_Profile_LocationData* location )
{
    if ( scorep_profile_is_implicit_task( location, location->current_task ) )
    {
        location->current_depth     = location->implicit_depth;
        location->current_task_node = location->current_implicit_node;
        return;
    }

    scorep_profile_task* current_task = location->current_task;

    if ( current_task == NULL )
    {
        SCOREP_PROFILE_STOP( location );
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Encountered unknown task ID" );
        return;
    }

    location->current_depth     = current_task->depth;
    location->current_task_node = current_task->current_node;
}

bool
scorep_profile_is_implicit_task( SCOREP_Profile_LocationData* location,
                                 scorep_profile_task*         task )
{
    //return task->task_id == SCOREP_PROFILE_IMPLICIT_TASK_ID;
    return task == SCOREP_PROFILE_IMPLICIT_TASK;
}

void
scorep_profile_task_parallel_exit( SCOREP_Profile_LocationData* location )
{
    if ( location->tasks->max_tasks > 0 )
    {
        scorep_profile_trigger_double( location,
                                       scorep_profile_active_task_metric,
                                       location->tasks->max_tasks,
                                       scorep_profile_get_current_node( location ) );

        location->tasks->max_tasks    = 0;
        scorep_profile_has_tasks_flag = 1;
    }
}

int32_t
scorep_profile_has_tasks( void )
{
    return scorep_profile_has_tasks_flag;
}
