/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2013, 2017-2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 *
 * Contains implementation for post-processing operations for task structires and
 * task related metrics.
 * In particular, it moves the visits from the task pointer regions to a "no of switches"
 * metric
 *
 */

#include <config.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_RuntimeManagement.h>

#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>
#include <scorep_profile_event_base.h>
#include <scorep_profile_node.h>

#include <string.h>

static void
visit_to_switches( scorep_profile_node* node,
                   void*                param )
{
    if ( node->node_type != SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        return;
    }

    SCOREP_RegionHandle region = scorep_profile_type_get_region_handle( node->type_specific_data );
    if ( SCOREP_RegionHandle_GetType( region ) != SCOREP_REGION_TASK )
    {
        return;
    }

    static SCOREP_MetricHandle switches_metric = SCOREP_INVALID_METRIC;

    if ( switches_metric == SCOREP_INVALID_METRIC )
    {
        switches_metric = SCOREP_Definitions_NewMetric( "number of switches",
                                                        "Number of switches to this type of tasks. "
                                                        "This includes the number of task begins and "
                                                        "the number of task resumes",
                                                        SCOREP_METRIC_SOURCE_TYPE_TASK,
                                                        SCOREP_METRIC_MODE_ACCUMULATED_START,
                                                        SCOREP_METRIC_VALUE_UINT64,
                                                        SCOREP_METRIC_BASE_DECIMAL,
                                                        0,
                                                        "",
                                                        SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                                        SCOREP_INVALID_METRIC );
    }

    scorep_profile_trigger_int64( ( SCOREP_Profile_LocationData* )param,
                                  switches_metric,
                                  node->count,
                                  node,
                                  SCOREP_PROFILE_TRIGGER_UPDATE_VALUE_AS_IS );
    node->count = 0;
}

static scorep_profile_node*
chroot_tasks( SCOREP_Profile_LocationData* location,
              scorep_profile_node*         tasksRoot,
              scorep_profile_node*         task )
{
    /* Register the region handle on first visit */
    static SCOREP_RegionHandle root_region = SCOREP_INVALID_REGION;
    if ( root_region == SCOREP_INVALID_REGION )
    {
        root_region = SCOREP_Definitions_NewRegion( "TASKS",
                                                    NULL,
                                                    SCOREP_INVALID_SOURCE_FILE,
                                                    SCOREP_INVALID_LINE_NO,
                                                    SCOREP_INVALID_LINE_NO,
                                                    SCOREP_PARADIGM_MEASUREMENT,
                                                    SCOREP_REGION_ARTIFICIAL );
    }

    /* Create root nodes for each new location */
    scorep_profile_node* program_root = NULL;
    scorep_profile_node* tasks_root   = NULL;
    if ( tasksRoot == NULL )
    {
        scorep_profile_type_data_t program_region_data;
        memset( &program_region_data, 0, sizeof( program_region_data ) );
        scorep_profile_type_set_region_handle( &program_region_data, SCOREP_GetProgramRegion() );
        program_root = scorep_profile_create_node( location,
                                                   NULL,
                                                   SCOREP_PROFILE_NODE_TASK_ROOT,
                                                   program_region_data,
                                                   SCOREP_GetBeginEpoch(),
                                                   false );

        scorep_profile_type_data_t root_region_data;
        memset( &root_region_data, 0, sizeof( root_region_data ) );
        scorep_profile_type_set_region_handle( &root_region_data, root_region );
        tasks_root = scorep_profile_create_node( location,
                                                 NULL,
                                                 SCOREP_PROFILE_NODE_TASK_ROOT,
                                                 root_region_data,
                                                 UINT64_MAX, false );
        scorep_profile_add_child( program_root, tasks_root );
    }
    else
    {
        program_root = tasksRoot;
        UTILS_BUG_ON( program_root->first_child == NULL );
        tasks_root = program_root->first_child;
    }

    /* move task tree to task_root */
    scorep_profile_remove_node( task );
    scorep_profile_add_child( tasks_root, task );
    scorep_profile_merge_node_inclusive( tasks_root, task );

    /* give the task root the timestamp of the first task */
    if ( task->first_enter_time < tasks_root->first_enter_time )
    {
        tasks_root->first_enter_time = task->first_enter_time;
    }
    return program_root;
}


void
scorep_profile_process_tasks( void )
{
    scorep_profile_node*         thread_root = scorep_profile.first_root_node;
    SCOREP_Profile_LocationData* location    = NULL;

    while ( thread_root != NULL )
    {
        scorep_profile_node* next      = NULL;
        scorep_profile_node* node      = thread_root->first_child;
        scorep_profile_node* task_root = NULL;
        location = scorep_profile_type_get_location_data( thread_root->type_specific_data );
        while ( node != NULL )
        {
            next = node->next_sibling;

            /* If not a task root, traverse tree and process all task pointers */
            if ( node->node_type != SCOREP_PROFILE_NODE_TASK_ROOT )
            {
                /*
                   scorep_profile_for_all( node,
                                        visit_to_switches,
                                        location );
                 */
            }
            /* Else move the task tree to a common root */
            else
            {
                task_root = chroot_tasks( location, task_root, node );
            }

            node = next;
        }

        /* If tasks occurred, add the task tree to the thread */
        if ( task_root != NULL )
        {
            scorep_profile_add_child( thread_root, task_root );
        }

        thread_root = thread_root->next_sibling;
    }
}
