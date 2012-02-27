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
 * @file        SCOREP_Profile_TaskingSum.c
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief   Implementation of the Tasking functions of the profile interface.
 *          Implements the task profiling algorithm that does only calculate the sum
 *          correctly.
 *
 */

#include <config.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Profile_Tasking.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <scorep_thread.h>
#include <SCOREP_Definitions.h>

#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>
#include <scorep_profile_task_table.h>

scorep_profile_node*
scorep_profile_enter( SCOREP_Profile_LocationData* location,
                      scorep_profile_node*         current_node,
                      SCOREP_RegionHandle          region,
                      SCOREP_RegionType            type,
                      uint64_t                     timestamp,
                      uint64_t*                    metrics );

scorep_profile_node*
scorep_profile_exit( SCOREP_Profile_LocationData* location,
                     scorep_profile_node*         node,
                     SCOREP_RegionHandle          region,
                     uint64_t                     timestamp,
                     uint64_t*                    metrics );

/* **************************************************************************************
   Local helper functions
****************************************************************************************/

static void
scorep_profile_update_dense_on_suspend( scorep_profile_dense_metric* metric,
                                        uint64_t                     end_value )
{
    metric->sum += end_value - metric->start_value;
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
    while ( node->node_type != scorep_profile_node_thread_root )
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
    while (  node->node_type != scorep_profile_node_thread_root )
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

static scorep_profile_node*
scorep_profile_create_task_root( SCOREP_Profile_LocationData* location,
                                 SCOREP_RegionHandle          regionHandle,
                                 uint64_t                     timestamp,
                                 uint64_t*                    metric_values )
{
    /* Create the data structure */
    scorep_profile_type_data_t specific_data;
    scorep_profile_type_set_region_handle( &specific_data, regionHandle );
    scorep_profile_type_set_depth( &specific_data, 1 );

    scorep_profile_node* new_node =
        scorep_profile_find_create_child( location,
                                          location->root_node,
                                          scorep_profile_node_regular_region,
                                          specific_data,
                                          timestamp );
    if ( new_node == NULL )
    {
        return NULL;
    }

    /* Set start values for all dense metrics */
    scorep_profile_update_on_resume( new_node, timestamp, metric_values );

    /* We have already our first enter */
    new_node->count++;

    return new_node;
}

static void
scorep_profile_enter_task_pointer( SCOREP_Profile_LocationData* location,
                                   scorep_profile_node*         task_node,
                                   uint64_t                     timestamp,
                                   uint64_t*                    metric_values )
{
    SCOREP_RegionHandle region = SCOREP_INVALID_REGION;

    /* The pointer region enter must be performed with the depth information of the
       implicit task. The implicit task never ececutes this function. Thus, we
       temporarily set the location depth information to the depth of the
       implicit task */
    uint32_t task_depth = location->current_depth;
    location->current_depth = location->implicit_depth;

    /* Determine the region handle of the tasks root region */
    scorep_profile_node* node = task_node;
    while ( node->parent->node_type != scorep_profile_node_thread_root )
    {
        node = node->parent;
    }
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
   Task interface functions
****************************************************************************************/

void
SCOREP_Profile_TaskCreateBegin( SCOREP_Thread_LocationData* thread,
                                SCOREP_RegionHandle         regionHandle,
                                uint64_t                    taskId,
                                uint64_t                    timestamp,
                                uint64_t*                   metric_values )
{
    SCOREP_Profile_Enter( thread, regionHandle, SCOREP_REGION_OMP_CREATE_TASK, timestamp, metric_values );
}

void
SCOREP_Profile_TaskCreateEnd( SCOREP_Thread_LocationData* thread,
                              SCOREP_RegionHandle         regionHandle,
                              uint64_t                    taskId,
                              uint64_t                    timestamp,
                              uint64_t*                   metric_values )
{
    SCOREP_Profile_LocationData* location =
        SCOREP_Thread_GetProfileLocationData( thread );
    if ( taskId != location->current_task_id )
    {
        SCOREP_Profile_TaskResume( thread, regionHandle, taskId, timestamp, metric_values );
    }
    SCOREP_Profile_Exit( thread, regionHandle, timestamp, metric_values );
}

void
SCOREP_Profile_TaskBegin( SCOREP_Thread_LocationData* thread,
                          SCOREP_RegionHandle         regionHandle,
                          uint64_t                    taskId,
                          uint64_t                    timestamp,
                          uint64_t*                   metric_values )
{
    /* Create new task entry */
    SCOREP_Profile_LocationData* location =
        SCOREP_Thread_GetProfileLocationData( thread );

    scorep_profile_type_data_t specific_data;
    scorep_profile_type_set_region_handle( &specific_data, regionHandle );

    scorep_profile_node* task_root =
        scorep_profile_create_task_root( location, regionHandle,
                                         timestamp, metric_values );

    scorep_profile_create_task( location, taskId, task_root );

    /* Perform activation */
    SCOREP_Profile_TaskResume( thread, regionHandle, taskId, timestamp, metric_values );
}

void
SCOREP_Profile_TaskResume( SCOREP_Thread_LocationData* thread,
                           SCOREP_RegionHandle         regionHandle,
                           uint64_t                    taskId,
                           uint64_t                    timestamp,
                           uint64_t*                   metric_values )
{
    SCOREP_Profile_LocationData* location =
        SCOREP_Thread_GetProfileLocationData( thread );

    /* Suspend old task */
    scorep_profile_store_task( location );

    if ( !scorep_profile_is_implicit_task( location, location->current_task_id ) )
    {
        scorep_profile_exit_task_pointer( location, timestamp, metric_values );
        scorep_profile_update_on_suspend( scorep_profile_get_current_node( thread ),
                                          timestamp,
                                          metric_values );
    }

    /* Activate new task */
    location->current_task_id = taskId;
    scorep_profile_restore_task( location );


    if ( !scorep_profile_is_implicit_task( location, taskId ) )
    {
        scorep_profile_node* current = scorep_profile_get_current_node( thread );

        scorep_profile_update_on_resume( current,
                                         timestamp,
                                         metric_values );

        scorep_profile_enter_task_pointer( location, current,
                                           timestamp, metric_values );
    }
}

void
SCOREP_Profile_TaskCompleted( SCOREP_Thread_LocationData* thread,
                              SCOREP_RegionHandle         regionHandle,
                              uint64_t                    timestamp,
                              uint64_t*                   metric_values )
{
    SCOREP_Profile_LocationData* location =
        SCOREP_Thread_GetProfileLocationData( thread );

    /* Remember some data before it has changed */
    scorep_profile_node* root_node = location->root_node;
    scorep_profile_node* task_node = location->current_task_node;
    uint64_t             task_id   = location->current_task_id;

    /* Exit task region and switch control to implicit task to ensure that the
       current task is always valid */
    SCOREP_Profile_Exit( thread, regionHandle, timestamp, metric_values );
    SCOREP_Profile_TaskResume( thread,
                               regionHandle,
                               SCOREP_PROFILE_IMPLICIT_TASK_ID,
                               timestamp,
                               metric_values );

    /* Delete task entry from hastable */
    scorep_profile_remove_task( location->tasks, task_id );
}
