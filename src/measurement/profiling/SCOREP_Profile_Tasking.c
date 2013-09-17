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
 * Copyright (c) 2009-2013,
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
 * @brief   Implementation of the Tasking functions of the profile interface
 *
 */

#include <config.h>
#include <SCOREP_Profile_Tasking.h>
#include <scorep_location.h>
#include <SCOREP_Definitions.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>
#include <scorep_profile_task_table.h>
#include <scorep_profile_event_base.h>
#include <SCOREP_Metric_Management.h>
#include <inttypes.h>
#include <assert.h>

/* **************************************************************************************
   Local helper functions
****************************************************************************************/

static inline void
update_dense_on_suspend( scorep_profile_dense_metric* metric,
                         uint64_t                     end_value )
{
    metric->intermediate_sum += end_value - metric->start_value;
}

static inline void
update_dense_on_resume( scorep_profile_dense_metric* metric,
                        uint64_t                     start_value )
{
    metric->start_value = start_value;
}

static void
update_on_suspend( scorep_profile_node* node,
                   uint64_t             timestamp,
                   uint64_t*            metric_values )
{
    while ( node != NULL )
    {
        update_dense_on_suspend( &node->inclusive_time, timestamp );
        for ( uint32_t i = 0; i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
        {
            update_dense_on_suspend( &node->dense_metrics[ i ],
                                     metric_values[ i ] );
        }
        node = node->parent;
    }
}

static void
update_on_resume( scorep_profile_node* node,
                  uint64_t             timestamp,
                  uint64_t*            metric_values )
{
    while ( node != NULL )
    {
        update_dense_on_resume( &node->inclusive_time, timestamp );
        for ( uint32_t i = 0; i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
        {
            update_dense_on_resume( &node->dense_metrics[ i ],
                                    metric_values[ i ] );
        }
        node = node->parent;
    }
}

scorep_profile_node*
create_task_root( SCOREP_Profile_LocationData* location,
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
    update_on_resume( new_node, timestamp, metric_values );

    /* We have already our first enter */
    new_node->count = 1;

    return new_node;
}

static void
enter_task_pointer( SCOREP_Profile_LocationData* location,
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
                                 SCOREP_RegionHandle_GetType( region ),
                                 timestamp,
                                 metric_values );

    assert( node != NULL );
    location->current_implicit_node = node;

    /* reset depth information */
    location->current_depth =  task_depth;
}

static void
exit_task_pointer( SCOREP_Profile_LocationData* location,
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
static void
task_switch( SCOREP_Profile_LocationData* location,
             scorep_profile_task*         task,
             uint64_t                     timestamp,
             uint64_t*                    metric_values )
{
    /* Suspend old task */
    scorep_profile_store_task( location );

    if ( !scorep_profile_is_implicit_task( location, location->current_task ) )
    {
        exit_task_pointer( location, timestamp, metric_values );
        update_on_suspend( scorep_profile_get_current_node( location ),
                           timestamp,
                           metric_values );
    }

    /* Activate new task */
    location->current_task = task;
    scorep_profile_restore_task( location );

    if ( !scorep_profile_is_implicit_task( location, task ) )
    {
        scorep_profile_node* current = scorep_profile_get_current_node( location );
        update_on_resume( current,
                          timestamp,
                          metric_values );

        enter_task_pointer( location, task,
                            timestamp, metric_values );
    }
}

/* **************************************************************************************
   Task interface functions
****************************************************************************************/

#define create_task_id( threadId, generationNumber ) \
    ( ( ( uint64_t )( threadId ) << 32 ) | ( generationNumber ) )

void
SCOREP_Profile_TaskCreate( SCOREP_Location* thread,
                           uint64_t         timestamp,
                           uint32_t         threadId,
                           uint32_t         generationNumber )
{
}


void
SCOREP_Profile_TaskBegin( SCOREP_Location*    thread,
                          uint64_t            timestamp,
                          uint64_t*           metricValues,
                          SCOREP_RegionHandle regionHandle,
                          uint32_t            threadId,
                          uint32_t            generationNumber )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Generate a unique task id out of the task-creating threads ID and the
       geneartion number */
    uint64_t task_id = create_task_id( threadId, generationNumber );

    /* Create new task entry */
    SCOREP_Profile_LocationData* location =
        SCOREP_Location_GetProfileData( thread );

    scorep_profile_type_data_t specific_data;
    scorep_profile_type_set_region_handle( &specific_data, regionHandle );

    scorep_profile_node* task_root =
        create_task_root( location, regionHandle,
                          timestamp, metricValues );

    scorep_profile_task* task = scorep_profile_create_task( location, task_id, task_root );
    if ( task == NULL )
    {
        return;
    }

    /* Perform activation */
    task_switch( location, task, timestamp, metricValues );
}


void
SCOREP_Profile_TaskSwitch( SCOREP_Location* thread,
                           uint64_t         timestamp,
                           uint64_t*        metricValues,
                           uint32_t         threadId,
                           uint32_t         generationNumber )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Generate a unique task id out of the task-creating threads ID and the
       geneartion number */
    uint64_t task_id = create_task_id( threadId, generationNumber );

    SCOREP_Profile_LocationData* location =
        SCOREP_Location_GetProfileData( thread );

    scorep_profile_task* task = scorep_profile_task_find( location, task_id );

    task_switch( location, task, timestamp, metricValues );
}


void
SCOREP_Profile_TaskEnd( SCOREP_Location*    thread,
                        uint64_t            timestamp,
                        uint64_t*           metricValues,
                        SCOREP_RegionHandle regionHandle,
                        uint32_t            threadId,
                        uint32_t            generationNumber )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Generate a unique task id out of the task-creating threads ID and the
       geneartion number */
    uint64_t task_id = create_task_id( threadId, generationNumber );

    SCOREP_Profile_LocationData* location =
        SCOREP_Location_GetProfileData( thread );

    /* Remember some data before it has changed */
    scorep_profile_task* task      = location->current_task;
    scorep_profile_node* task_node = location->current_task_node;
    scorep_profile_node* root_node = location->root_node;

    /* Exit task region and switch control to implicit task to ensure that the
       current task is always valid */
    SCOREP_Profile_Exit( thread, regionHandle, timestamp, metricValues );
    task_switch( location,
                 SCOREP_PROFILE_IMPLICIT_TASK,
                 timestamp,
                 metricValues );

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
    scorep_profile_remove_task( location, task_id );
}

/* used to fool the linker, so that this unit is always linked into the
 * library/binary. */
UTILS_FOOL_LINKER_DECLARE( SCOREP_Profile_Tasking );
