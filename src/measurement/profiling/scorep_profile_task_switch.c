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
 * Copyright (c) 2009-2014,
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
 * @brief   Implements the task switch algorithms
 *
 */

#include <config.h>
#include <scorep_profile_task_switch.h>
#include <scorep_profile_event_base.h>
#include <scorep_profile_location.h>

#include <SCOREP_Metric_Management.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Profile_Tasking.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Definitions.h>
#include <UTILS_Error.h>

#include <assert.h>


/* **************************************************************************************
 *                                                           Internally visible functions
 * *************************************************************************************/

static inline void
scorep_profile_metric_copy_on_untied_suspend( scorep_profile_dense_metric* metric )
{
    metric->sum = -metric->intermediate_sum;
}

static scorep_profile_node*
scorep_profile_copy_callpath( SCOREP_Profile_LocationData* location,
                              scorep_profile_node**        callpath )
{
    scorep_profile_node* parent = ( *callpath )->parent;
    scorep_profile_node* root   = NULL;

    *callpath = scorep_profile_copy_node( location, *callpath );
    scorep_profile_metric_copy_on_untied_suspend( &( *callpath )->inclusive_time );
    for ( uint32_t i = 0;
          i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
    {
        scorep_profile_metric_copy_on_untied_suspend( &( *callpath )->dense_metrics[ i ] );
    }

    if ( parent != NULL )
    {
        root = scorep_profile_copy_callpath( location, &parent );
        scorep_profile_add_child( parent, *callpath );
    }
    else
    {
        root = *callpath;
    }

    return root;
}

static void
scorep_profile_add_task_tree( SCOREP_Profile_LocationData* location,
                              scorep_profile_task*         task )
{
    if ( task->current_node == NULL )
    {
        return;
    }

    scorep_profile_node* root_node = location->root_node;
    scorep_profile_node* task_root = task->root_node;
    scorep_profile_node* new_task  = scorep_profile_copy_callpath( location,
                                                                   &task->current_node );
    scorep_profile_node* match = scorep_profile_find_child( root_node, task_root );

    if ( match == NULL )
    {
        scorep_profile_add_child( root_node, task_root );
    }
    else
    {
        scorep_profile_merge_subtree( location, match, task_root );
    }

    task->root_node = new_task;
}

static inline void
scorep_profile_update_untied_on_suspend( scorep_profile_dense_metric* metric,
                                         uint64_t                     end_value )
{
    metric->sum              += end_value - metric->start_value;
    metric->intermediate_sum += end_value - metric->start_value;
}

static inline void
scorep_profile_update_tied_on_suspend( scorep_profile_dense_metric* metric,
                                       uint64_t                     end_value )
{
    metric->intermediate_sum += end_value - metric->start_value;
}

static inline void
scorep_profile_update_dense_on_resume( scorep_profile_dense_metric* metric,
                                       uint64_t                     start_value )
{
    metric->start_value = start_value;
}

static void
scorep_profile_update_on_suspend( SCOREP_Profile_LocationData* location,
                                  uint64_t                     timestamp,
                                  uint64_t*                    metric_values )
{
    scorep_profile_task* task = location->current_task;
    scorep_profile_node* node = task->current_node;

    if ( task->can_migrate )
    {
        /* Updated data on the path from root to current node */
        while ( node != NULL )
        {
            scorep_profile_update_untied_on_suspend( &node->inclusive_time, timestamp );
            for ( uint32_t i = 0;
                  i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
            {
                scorep_profile_update_untied_on_suspend( &node->dense_metrics[ i ],
                                                         metric_values[ i ] );
            }
            node = node->parent;
        }

        scorep_profile_add_task_tree( location, task );
    }
    else
    {
        /* Updated data on the path from root to current node */
        while ( node != NULL )
        {
            scorep_profile_update_tied_on_suspend( &node->inclusive_time, timestamp );
            for ( uint32_t i = 0;
                  i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
            {
                scorep_profile_update_tied_on_suspend( &node->dense_metrics[ i ],
                                                       metric_values[ i ] );
            }
            node = node->parent;
        }
    }
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
                                 SCOREP_RegionHandle_GetType( region ),
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


static bool
scorep_profile_is_implicit_task( SCOREP_Profile_LocationData* location,
                                 scorep_profile_task*         task )
{
    return task == location->implicit_task;
}



static void
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

static void
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

static inline void
scorep_profile_task_suspend( SCOREP_Profile_LocationData* location,
                             uint64_t                     timestamp,
                             uint64_t*                    metric_values )
{
    scorep_profile_store_task( location );

    if ( !scorep_profile_is_implicit_task( location, location->current_task ) )
    {
        scorep_profile_exit_task_pointer( location, timestamp, metric_values );
        scorep_profile_update_on_suspend( location,
                                          timestamp,
                                          metric_values );
    }
}

/* **************************************************************************************
 *                                                                     Declared in header
 * *************************************************************************************/

void
scorep_profile_update_on_resume( scorep_profile_node* node,
                                 uint64_t             timestamp,
                                 uint64_t*            metricValues )
{
    while ( node != NULL )
    {
        scorep_profile_update_dense_on_resume( &node->inclusive_time, timestamp );
        for ( uint32_t i = 0;
              i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
        {
            scorep_profile_update_dense_on_resume( &node->dense_metrics[ i ],
                                                   metricValues[ i ] );
        }
        node = node->parent;
    }
}

void
scorep_profile_task_switch_start( SCOREP_Profile_LocationData* location,
                                  scorep_profile_task*         task,
                                  uint64_t                     timestamp,
                                  uint64_t*                    metricValues )
{
    scorep_profile_task_suspend( location, timestamp, metricValues );

    /* Activate new task */
    location->current_task = task;
    scorep_profile_restore_task( location );

    scorep_profile_node* current = scorep_profile_get_current_node( location );

    scorep_profile_update_on_resume( current,
                                     timestamp,
                                     metricValues );

    scorep_profile_enter_task_pointer( location, task,
                                       timestamp, metricValues );
}

void
scorep_profile_task_switch( SCOREP_Profile_LocationData* location,
                            scorep_profile_task*         task,
                            uint64_t                     timestamp,
                            uint64_t*                    metricValues )
{
    scorep_profile_task_suspend( location, timestamp, metricValues );

    /* Activate new task */
    location->current_task = task;
    scorep_profile_restore_task( location );

    if ( !scorep_profile_is_implicit_task( location, task ) )
    {
        scorep_profile_node* current = scorep_profile_get_current_node( location );
        scorep_profile_update_on_resume( current,
                                         timestamp,
                                         metricValues );

        scorep_profile_enter_task_pointer( location, task,
                                           timestamp, metricValues );

        if ( task->last_location != location )
        {
            location->migration_win++;
            task->last_location = location;
        }
    }
}
