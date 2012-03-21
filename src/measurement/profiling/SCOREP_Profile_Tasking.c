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
 * @file        SCOREP_Profile_Tasking.c
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief   Implementation of the Tasking functions of the profile interface
 *
 */

#include <config.h>
#include <SCOREP_Profile_Tasking.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <scorep_thread.h>
#include <SCOREP_Definitions.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>
#include <scorep_profile_task_table.h>
#include <scorep_profile_event_base.h>
#include <inttypes.h>

/* **************************************************************************************
   Local helper functions
****************************************************************************************/

static void
scorep_profile_update_dense_on_suspend( scorep_profile_dense_metric* metric,
                                        uint64_t                     end_value )
{
    metric->intermediate_sum += end_value - metric->start_value;
}

static void
scorep_profile_update_dense_on_resume( scorep_profile_dense_metric* metric,
                                       uint64_t                     start_value )
{
    metric->start_value = start_value;
}

static void
scorep_profile_update_on_suspend( scorep_profile_node* node,
                                  uint64_t             timestamp,
                                  uint64_t*            metric_values )
{
    while ( node != NULL )
    {
        scorep_profile_update_dense_on_suspend( &node->inclusive_time, timestamp );
        for ( uint32_t i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
        {
            scorep_profile_update_dense_on_suspend( &node->dense_metrics[ i ],
                                                    metric_values[ i ] );
        }
        node = node->parent;
    }
}

static void
scorep_profile_update_on_resume( scorep_profile_node* node,
                                 uint64_t             timestamp,
                                 uint64_t*            metric_values )
{
    while ( node != NULL )
    {
        scorep_profile_update_dense_on_resume( &node->inclusive_time, timestamp );
        for ( uint32_t i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
        {
            scorep_profile_update_dense_on_resume( &node->dense_metrics[ i ],
                                                   metric_values[ i ] );
        }
        node = node->parent;
    }
}

scorep_profile_node*
scorep_profile_create_task_root( SCOREP_Profile_LocationData* location,
                                 SCOREP_RegionHandle          regionHandle,
                                 uint64_t                     timestamp,
                                 uint64_t*                    metric_values )
{
    /* Create the data structure */
    scorep_profile_type_data_t specific_data;
    scorep_profile_type_set_region_handle( &specific_data,
                                           regionHandle );

    scorep_profile_node* new_node =
        scorep_profile_create_node( location, NULL,
                                    scorep_profile_node_task_root,
                                    specific_data,
                                    timestamp );
    if ( new_node == NULL )
    {
        return NULL;
    }

    /* Set start values for all dense metrics */
    scorep_profile_update_on_resume( new_node, timestamp, metric_values );

    /* We have already our first enter */
    new_node->count = 1;

    return new_node;
}

static void
scorep_profile_enter_task_pointer( SCOREP_Profile_LocationData* location,
                                   scorep_profile_task*         task,
                                   uint64_t                     timestamp,
                                   uint64_t*                    metric_values )
{
    SCOREP_RegionHandle region = SCOREP_INVALID_REGION;

    /* The pointer region enter must be performed with the depth information of the
       implicit task. The implicit task never executes this function. Thus, we
       temporarily set the location depth information to the depth of the
       implicit task */
    uint32_t task_depth = location->current_depth;
    location->current_depth = location->implicit_depth;

    /* Determine the region handle of the tasks root region */
    scorep_profile_node* node = task->root_node;
    region = scorep_profile_type_get_region_handle( node->type_specific_data );
    assert( region != SCOREP_INVALID_REGION );
    assert( location->current_implicit_node != NULL );

    /* Enter the task pointer region with the implicit task */
    node = scorep_profile_enter( location,
                                 location->current_implicit_node,
                                 region,
                                 SCOREP_Region_GetType( region ),
                                 timestamp,
                                 metric_values );

    assert( node != NULL );
    location->current_implicit_node = node;

    /* reset depth information */
    location->current_depth =  task_depth;
}

static void
scorep_profile_exit_task_pointer( SCOREP_Profile_LocationData* location,
                                  uint64_t                     timestamp,
                                  uint64_t*                    metric_values )
{
    scorep_profile_node* node   = NULL;
    SCOREP_RegionHandle  region =
        /* Determine the region handle of the tasks root region */
        scorep_profile_type_get_region_handle( location->current_implicit_node->type_specific_data );

    /* The pointer region exit must be performed with the depth information of the
       implicit task. The implicit task never ececutes this function. Thus, we
       temporarily set the location depth information to the depth of the
       implicit task */
    uint32_t task_depth = location->current_depth;
    location->current_depth = location->implicit_depth;

    /* Exit the task pointer region with the implicit task */
    assert( location->current_implicit_node != NULL );
    node = scorep_profile_exit( location,
                                location->current_implicit_node,
                                region,
                                timestamp,
                                metric_values );

    assert( node != NULL );
    location->current_implicit_node = node;

    /* reset depth information */
    location->current_depth = task_depth;
}

/* **************************************************************************************
   internal implementation of events
****************************************************************************************/
void
scorep_profile_task_switch( SCOREP_Profile_LocationData* location,
                            scorep_profile_task*         task,
                            uint64_t                     timestamp,
                            uint64_t*                    metric_values )
{
    /* Suspend old task */
    scorep_profile_store_task( location );

    if ( !scorep_profile_is_implicit_task( location, location->current_task ) )
    {
        scorep_profile_exit_task_pointer( location, timestamp, metric_values );
        scorep_profile_update_on_suspend( scorep_profile_get_current_node( location ),
                                          timestamp,
                                          metric_values );
    }

    /* Activate new task */
    location->current_task = task;
    scorep_profile_restore_task( location );

    if ( !scorep_profile_is_implicit_task( location, task ) )
    {
        scorep_profile_node* current = scorep_profile_get_current_node( location );
        scorep_profile_update_on_resume( current,
                                         timestamp,
                                         metric_values );

        scorep_profile_enter_task_pointer( location, task,
                                           timestamp, metric_values );
    }
}

/* **************************************************************************************
   Task interface functions
****************************************************************************************/

void
SCOREP_Profile_TaskCreate( SCOREP_Location* location,
                           uint64_t         timestamp,
                           uint64_t         taskId )
{
}


void
SCOREP_Profile_TaskBegin( SCOREP_Location*    thread,
                          SCOREP_RegionHandle regionHandle,
                          uint64_t            taskId,
                          uint64_t            timestamp,
                          uint64_t*           metric_values )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Create new task entry */
    SCOREP_Profile_LocationData* location =
        SCOREP_Location_GetProfileData( thread );

    scorep_profile_type_data_t specific_data;
    scorep_profile_type_set_region_handle( &specific_data, regionHandle );

    scorep_profile_node* task_root =
        scorep_profile_create_task_root( location, regionHandle,
                                         timestamp, metric_values );

    scorep_profile_task* task = scorep_profile_create_task( location, taskId, task_root );
    if ( task == NULL )
    {
        return;
    }

    /* Perform activation */
    scorep_profile_task_switch( location, task, timestamp, metric_values );
}


void
SCOREP_Profile_TaskSwitch( SCOREP_Location* thread,
                           uint64_t         taskId,
                           uint64_t         timestamp,
                           uint64_t*        metric_values )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    SCOREP_Profile_LocationData* location =
        SCOREP_Location_GetProfileData( thread );

    scorep_profile_task* task = scorep_profile_task_find( location, taskId );

    scorep_profile_task_switch( location, task, timestamp, metric_values );
}


void
SCOREP_Profile_TaskEnd( SCOREP_Location*    thread,
                        SCOREP_RegionHandle regionHandle,
                        uint64_t            taskId,
                        uint64_t            timestamp,
                        uint64_t*           metric_values )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    SCOREP_Profile_LocationData* location =
        SCOREP_Location_GetProfileData( thread );

    /* Remember some data before it has changed */
    scorep_profile_task* task      = location->current_task;
    scorep_profile_node* task_node = location->current_task_node;
    scorep_profile_node* root_node = location->root_node;

    /* Exit task region and switch control to implicit task to ensure that the
       current task is always valid */
    SCOREP_Profile_Exit( thread, regionHandle, timestamp, metric_values );
    scorep_profile_task_switch( location,
                                SCOREP_PROFILE_IMPLICIT_TASK,
                                timestamp,
                                metric_values );

    /* Merge subtree and release unnecessary node records */
    scorep_profile_node* match = scorep_profile_find_child( root_node, task->root_node );

    if ( match == NULL )
    {
        scorep_profile_add_child( root_node, task->root_node );
    }
    else
    {
        scorep_profile_merge_subtree( location, match, task->root_node );
    }

    /* Delete task entry from hastable */
    scorep_profile_remove_task( location, taskId );
}
