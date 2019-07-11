/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2015,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @brief   Implementation of the Tasking functions of the profile interface
 *
 */

#include <config.h>
#include <SCOREP_Profile_Tasking.h>
#include <scorep_profile_converter.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_event_base.h>
#include <scorep_profile_location.h>
#include <scorep_profile_task_init.h>
#include <scorep_profile_task_switch.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Task.h>
#include <SCOREP_Types.h>
#include <SCOREP_Mutex.h>
#include <UTILS_Error.h>
#include <scorep_location_management.h>

#include <string.h>
#include <assert.h>

/* **************************************************************************************
 *                                                              Local types and variables
 * *************************************************************************************/

static uint32_t scorep_profile_has_tasks_flag = 0;

SCOREP_MetricHandle scorep_profile_migration_loss_metric = SCOREP_INVALID_METRIC;
SCOREP_MetricHandle scorep_profile_migration_win_metric  = SCOREP_INVALID_METRIC;

/* **************************************************************************************
 *                                                                 Local helper functions
 * *************************************************************************************/

static inline scorep_profile_task*
get_profile_task_data( SCOREP_TaskHandle task )
{
    return SCOREP_Task_GetSubstrateData( task, scorep_profile_substrate_id );
}

static scorep_profile_node*
create_task_root( SCOREP_Profile_LocationData* location,
                  SCOREP_RegionHandle          regionHandle,
                  uint64_t                     timestamp,
                  uint64_t*                    metricValues,
                  scorep_profile_task_context  taskContext )
{
    /* Create the data structure */
    scorep_profile_type_data_t specific_data;
    memset( &specific_data, 0, sizeof( specific_data ) );
    scorep_profile_type_set_region_handle( &specific_data,
                                           regionHandle );

    scorep_profile_node* new_node =
        scorep_profile_create_node( location, NULL,
                                    SCOREP_PROFILE_NODE_TASK_ROOT,
                                    specific_data,
                                    timestamp,
                                    taskContext );
    if ( new_node == NULL )
    {
        return NULL;
    }

    /* Set start values for all dense metrics */
    scorep_profile_update_on_resume( new_node, timestamp, metricValues );

    /* We have already our first enter */
    new_node->count = 1;

    return new_node;
}

static scorep_profile_task*
alloc_new_task( SCOREP_Location*             locationData,
                SCOREP_Profile_LocationData* location )
{
    scorep_profile_task* task =
        SCOREP_Location_AllocForProfile( locationData, sizeof( scorep_profile_task ) );

    task->creator = location;
    return task;
}

/* **************************************************************************************
 *                                                           Internally visible functions
 * *************************************************************************************/
void
scorep_profile_task_initialize( void )
{
    /* Initialize metric */
    if ( scorep_profile_migration_loss_metric == SCOREP_INVALID_METRIC )
    {
        scorep_profile_migration_loss_metric =
            SCOREP_Definitions_NewMetric( "task_migration_loss",
                                          "Number of task that migrated away.",
                                          SCOREP_METRIC_SOURCE_TYPE_TASK,
                                          SCOREP_METRIC_MODE_ACCUMULATED_START,
                                          SCOREP_METRIC_VALUE_INT64,
                                          SCOREP_METRIC_BASE_DECIMAL,
                                          0,
                                          "",
                                          SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                          SCOREP_INVALID_METRIC );
    }
    if ( scorep_profile_migration_win_metric == SCOREP_INVALID_METRIC )
    {
        scorep_profile_migration_win_metric =
            SCOREP_Definitions_NewMetric( "task_migration_win",
                                          "Number of tasks that migrated to this location.",
                                          SCOREP_METRIC_SOURCE_TYPE_TASK,
                                          SCOREP_METRIC_MODE_ACCUMULATED_START,
                                          SCOREP_METRIC_VALUE_UINT64,
                                          SCOREP_METRIC_BASE_DECIMAL,
                                          0,
                                          "",
                                          SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                          SCOREP_INVALID_METRIC );
    }
}

int32_t
scorep_profile_has_tasks( void )
{
    return scorep_profile_has_tasks_flag;
}

void
scorep_profile_update_task_metrics( SCOREP_Profile_LocationData* location )
{
    if ( location->migration_sum != 0 || location->migration_win != 0 )
    {
        uint64_t loss = location->migration_win - location->migration_sum;
        scorep_profile_trigger_int64( location,
                                      scorep_profile_migration_loss_metric,
                                      loss,
                                      scorep_profile_get_current_node( location ),
                                      SCOREP_PROFILE_TRIGGER_UPDATE_VALUE_AS_IS );
        scorep_profile_trigger_int64( location,
                                      scorep_profile_migration_win_metric,
                                      location->migration_win,
                                      scorep_profile_get_current_node( location ),
                                      SCOREP_PROFILE_TRIGGER_UPDATE_VALUE_AS_IS );

        location->migration_sum = 0;
        location->migration_win = 0;
    }
}

/* **************************************************************************************
 *                                                               Task interface functions
 * *************************************************************************************/

void
SCOREP_Profile_TaskBegin( SCOREP_Location*                 thread,
                          uint64_t                         timestamp,
                          SCOREP_RegionHandle              regionHandle,
                          uint64_t*                        metricValues,
                          SCOREP_ParadigmType              paradigm,
                          SCOREP_InterimCommunicatorHandle threadTeam,
                          uint32_t                         threadId,
                          uint32_t                         generationNumber,
                          SCOREP_TaskHandle                taskHandle )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Create new task entry */
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( thread );

    scorep_profile_type_data_t specific_data;
    memset( &specific_data, 0, sizeof( specific_data ) );
    scorep_profile_type_set_region_handle( &specific_data, regionHandle );

    scorep_profile_task* task = get_profile_task_data( taskHandle );

    scorep_profile_node* task_root
        = create_task_root( location, regionHandle, timestamp, metricValues,
                            task->can_migrate ? SCOREP_PROFILE_TASK_CONTEXT_UNTIED : SCOREP_PROFILE_TASK_CONTEXT_TIED );

    task->current_node = task_root;
    task->root_node    = task_root;
    task->depth        = 1;
    task->can_migrate
        = ( SCOREP_RegionHandle_GetType( regionHandle ) == SCOREP_REGION_TASK_UNTIED );

    /* Perform activation */
    scorep_profile_task_switch_start( location, task, timestamp, metricValues );
}


void
SCOREP_Profile_TaskSwitch( SCOREP_Location*                 thread,
                           uint64_t                         timestamp,
                           uint64_t*                        metricValues,
                           SCOREP_ParadigmType              paradigm,
                           SCOREP_InterimCommunicatorHandle threadTeam,
                           uint32_t                         threadId,
                           uint32_t                         generationNumber,
                           SCOREP_TaskHandle                taskHandle )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( thread );

    scorep_profile_task* task = get_profile_task_data( taskHandle );

    scorep_profile_task_switch( location, task, timestamp, metricValues );
}


void
SCOREP_Profile_TaskEnd( SCOREP_Location*                 thread,
                        uint64_t                         timestamp,
                        SCOREP_RegionHandle              regionHandle,
                        uint64_t*                        metricValues,
                        SCOREP_ParadigmType              paradigm,
                        SCOREP_InterimCommunicatorHandle threadTeam,
                        uint32_t                         threadId,
                        uint32_t                         generationNumber,
                        SCOREP_TaskHandle                taskHandle )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( thread );

    /* Remember some data before it has changed */
    scorep_profile_task* task      = location->current_task;
    scorep_profile_node* task_node = location->current_task_node;
    scorep_profile_node* root_node = location->root_node;

    /* Exit task region and switch control to implicit task to ensure that the
       current task is always valid */
    SCOREP_Profile_Exit( thread, timestamp, regionHandle, metricValues );
    scorep_profile_task_switch( location,
                                location->implicit_task,
                                timestamp,
                                metricValues );

    scorep_profile_node* match = scorep_profile_find_child( root_node,
                                                            task->root_node );
    if ( match == NULL )
    {
        scorep_profile_add_child( root_node, task->root_node );
    }
    else
    {
        scorep_profile_merge_subtree( location, match, task->root_node );
    }
}

void
SCOREP_Profile_CreateTaskData( SCOREP_Location*  locationData,
                               SCOREP_TaskHandle taskHandle )
{
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( locationData );

    scorep_profile_task* new_task = scorep_profile_recycle_task( location );

    if ( new_task == NULL )
    {
        new_task = alloc_new_task( locationData, location );
    }

    new_task->current_node  = NULL;
    new_task->root_node     = NULL;
    new_task->depth         = 0;
    new_task->can_migrate   = true;
    new_task->last_location = location;

    /* Every task created by this location is a possible task that can migrate away.
       If it does not, the increase on release will even out this decrease. */
    location->migration_sum--;

    /* The implicit task has always generation numner 0. */
    if ( SCOREP_Task_GetGenerationNumber( taskHandle ) == 0 )
    {
        location->implicit_task = new_task;
        location->current_task  = new_task;
    }
    else
    {
        /* Mark that we have explicit tasks */
        scorep_profile_has_tasks_flag = 1;
    }

    SCOREP_Task_SetSubstrateData( taskHandle, scorep_profile_substrate_id, new_task );
}

void
SCOREP_Profile_FreeTaskData( SCOREP_Location*  locationData,
                             SCOREP_TaskHandle taskHandle )
{
    scorep_profile_task* task = get_profile_task_data( taskHandle );
    assert( task );

    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( locationData );

    scorep_profile_release_task( location, task );

    /* Every task released by this location is a possible task that might have migrated
       towards this location. If it does not, we must even out the decrease on creation.
       Thus, we must increase this number in any case. */
    location->migration_sum++;
}
