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
 * @file scorep_profile_task_process.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 * Contains implmentation for post-processing operations for task structires and
 * task related metrics.
 * In particular, it moves the visits from the task pointer regions to a "no of switches"
 * metric
 *
 */

#include <config.h>

#include <SCOREP_Definitions.h>

#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>
#include <scorep_profile_event_base.h>
#include <scorep_profile_node.h>


static void
visit_to_switches( SCOREP_Location*     location,
                   scorep_profile_node* node,
                   void*                param )
{
    if ( node->node_type != scorep_profile_node_regular_region  )
    {
        return;
    }

    SCOREP_RegionHandle region = scorep_profile_type_get_region_handle( node->type_specific_data );
    if ( SCOREP_Region_GetType( region ) != SCOREP_REGION_TASK )
    {
        return;
    }

    static SCOREP_MetricHandle switches_metric = SCOREP_INVALID_METRIC;

    if ( switches_metric == SCOREP_INVALID_METRIC )
    {
        switches_metric = SCOREP_DefineMetric( "number of switches",
                                               "Number of switches to this type of tasks. "
                                               "This includes the number of task begins and "
                                               "the number of task resumes",
                                               SCOREP_METRIC_SOURCE_TYPE_TASK,
                                               SCOREP_METRIC_MODE_ACCUMULATED_START,
                                               SCOREP_METRIC_VALUE_UINT64,
                                               SCOREP_METRIC_BASE_DECIMAL,
                                               0,
                                               "",
                                               SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );
    }

    scorep_profile_trigger_int64( location,
                                  ( SCOREP_Profile_LocationData* )param,
                                  switches_metric,
                                  node->count,
                                  node );
    node->count = 0;
}

static scorep_profile_node*
chroot_tasks( SCOREP_Location*             location,
              SCOREP_Profile_LocationData* profileLocation,
              scorep_profile_node*         task_root,
              scorep_profile_node*         task )
{
    /* Register the region handle on first visist */
    static SCOREP_RegionHandle root_region = SCOREP_INVALID_REGION;
    if ( root_region == SCOREP_INVALID_REGION )
    {
        root_region = SCOREP_DefineRegion( "task_root",
                                           NULL,
                                           SCOREP_INVALID_SOURCE_FILE,
                                           SCOREP_INVALID_LINE_NO,
                                           SCOREP_INVALID_LINE_NO,
                                           SCOREP_ADAPTER_POMP,
                                           SCOREP_REGION_TASK );
    }

    /* Create root node for each new location */
    if ( task_root == NULL )
    {
        scorep_profile_type_data_t data;
        scorep_profile_type_set_region_handle( &data, root_region );

        task_root = scorep_profile_create_node( location,
                                                profileLocation,
                                                NULL,
                                                scorep_profile_node_task_root,
                                                data,
                                                0 );
    }

    /* move task tree to task_root */
    scorep_profile_remove_node( task );
    scorep_profile_add_child( task_root, task );
    scorep_profile_merge_node_inclusive( task_root, task );

    return task_root;
}


void
scorep_profile_process_tasks( SCOREP_Location* location )
{
    scorep_profile_node* thread_root = scorep_profile.first_root_node;

    while ( thread_root != NULL )
    {
        scorep_profile_node*         next             = NULL;
        scorep_profile_node*         node             = thread_root->first_child;
        scorep_profile_node*         task_root        = NULL;
        SCOREP_Profile_LocationData* profile_location = scorep_profile_type_get_location_data( thread_root->type_specific_data );
        while ( node != NULL )
        {
            next = node->next_sibling;

            /* If not a task root, traverse tree and process all task pointers */
            if ( node->node_type != scorep_profile_node_task_root  )
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
                task_root = chroot_tasks( location, profile_location, task_root, node );
            }

            node = next;
        }

        /* If tasks occured, add the task tree to the thread */
        if ( task_root != NULL )
        {
            scorep_profile_add_child( thread_root, task_root );
        }

        thread_root = thread_root->next_sibling;
    }
}
