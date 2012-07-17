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
 * @file scorep_profile_event_base.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 * Implementation of the base event functions for enter, exit, trigger metric, ...
 */

#include <config.h>

#include <SCOREP_Error.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Definitions.h>

#include <scorep_profile_node.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>

#include <inttypes.h>

scorep_profile_node*
scorep_profile_enter( SCOREP_Profile_LocationData* location,
                      scorep_profile_node*         current_node,
                      SCOREP_RegionHandle          region,
                      SCOREP_RegionType            type,
                      uint64_t                     timestamp,
                      uint64_t*                    metrics )
{
    scorep_profile_node*       node;
    scorep_profile_type_data_t node_data;

    /* If we are already in a collapse node -> do nothing more */
    if ( ( current_node != NULL ) &&
         ( current_node->node_type == scorep_profile_node_collapse ) )
    {
        if ( scorep_profile.reached_depth <  location->current_depth )
        {
            scorep_profile.reached_depth = location->current_depth;
        }
        return current_node;
    }

    /* If we just reached the depth limit */
    if ( location->current_depth > scorep_profile.max_callpath_depth )
    {
        scorep_profile.has_collapse_node = true;
        if ( scorep_profile.reached_depth <  location->current_depth )
        {
            scorep_profile.reached_depth = location->current_depth;
        }
        scorep_profile_type_set_depth( &node_data, location->current_depth );
        node = scorep_profile_find_create_child( location,
                                                 current_node,
                                                 scorep_profile_node_collapse,
                                                 node_data,
                                                 timestamp );
    }
    /* Regular enter */
    else
    {
        scorep_profile_type_set_region_handle( &node_data, region );
        node = scorep_profile_find_create_child( location,
                                                 current_node,
                                                 scorep_profile_node_regular_region,
                                                 node_data,
                                                 timestamp );
    }
    /* Disable profiling if node creation failed */
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Failed to create location. Disable profiling" );
        SCOREP_PROFILE_STOP;
        return NULL;
    }

    /* Store start values for dense metrics */
    node->count++;
    node->inclusive_time.start_value = timestamp;
    for ( uint32_t i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
    {
        node->dense_metrics[ i ].start_value = metrics[ i ];
    }
    return node;
}


scorep_profile_node*
scorep_profile_exit( SCOREP_Profile_LocationData* location,
                     scorep_profile_node*         node,
                     SCOREP_RegionHandle          region,
                     uint64_t                     timestamp,
                     uint64_t*                    metrics )
{
    scorep_profile_node* parent = NULL;

    /* validity checks */
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Exit event occured in a thread which never entered a region" );
        SCOREP_PROFILE_STOP;
        return NULL;
    }

    /* If we are in a collapse node, check whether the current depth is still
       larger than the creation depth of the collapse node */
    if ( ( node->node_type == scorep_profile_node_collapse ) &&
         ( location->current_depth > scorep_profile_type_get_depth( node->type_specific_data ) ) )
    {
        location->current_depth--;
        return node;
    }

    /* Exit all parameters and the region itself. Thus, more than one node may be exited.
       Initialize loop: start with this node. Further iterations should work on the
       parent. */
    parent = node;
    do
    {
        location->current_depth--;
        node = parent;

        /* Update metrics */
        node->last_exit_time = timestamp;
        scorep_profile_update_dense_metric( &node->inclusive_time, timestamp );
        for ( uint32_t i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
        {
            scorep_profile_update_dense_metric( &node->dense_metrics[ i ], metrics[ i ] );
        }
        parent = node->parent;
    }
    while ( ( node->node_type != scorep_profile_node_regular_region ) &&
            ( node->node_type != scorep_profile_node_collapse ) &&
            ( parent != NULL ) );
    /* If this was a parameter node also exit next level node */

    if ( ( node->node_type == scorep_profile_node_regular_region ) &&
         ( scorep_profile_type_get_region_handle( node->type_specific_data ) != region ) )
    {
        SCOREP_PROFILE_STOP;
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Exit event for other than current region occured at "
                      "location %u: Expected exit for region %s. "
                      "Exited region %s",
                      scorep_profile_type_get_int_value( location->root_node->type_specific_data ),
                      SCOREP_Region_GetName( scorep_profile_type_get_region_handle( node->type_specific_data ) ),
                      SCOREP_Region_GetName( region ) );
        return NULL;
    }
    return parent;
}


void
scorep_profile_trigger_int64( SCOREP_Profile_LocationData* location,
                              SCOREP_MetricHandle          metric,
                              uint64_t                     value,
                              scorep_profile_node*         node )
{
    scorep_profile_sparse_metric_int* current = NULL;
    scorep_profile_sparse_metric_int* next    = NULL;

    /* If no sparse metrics are stored so far. */
    next = node->first_int_sparse;
    if ( next == NULL )
    {
        node->first_int_sparse = scorep_profile_create_sparse_int( location,
                                                                   metric, value );
        return;
    }

    /* Iterate all existing sparse metrics */
    do
    {
        current = next;
        if ( current->metric == metric )
        {
            scorep_profile_update_sparse_int( current, value );
            return;
        }
        next = current->next_metric;
    }
    while ( next != NULL );

    /* Append new sparse metric */
    current->next_metric = scorep_profile_create_sparse_int( location, metric, value );
}

void
scorep_profile_trigger_double(  SCOREP_Profile_LocationData* location,
                                SCOREP_MetricHandle          metric,
                                double                       value,
                                scorep_profile_node*         node )
{
    scorep_profile_sparse_metric_double* current = NULL;
    scorep_profile_sparse_metric_double* next    = NULL;

    /* If no sparse metrics are stored so far. */
    next = node->first_double_sparse;
    if ( next == NULL )
    {
        node->first_double_sparse = scorep_profile_create_sparse_double( location, metric, value );
        return;
    }

    /* Iterate all existing sparse metrics */
    do
    {
        current = next;
        if ( current->metric == metric )
        {
            scorep_profile_update_sparse_double( current, value );
            return;
        }
        next = current->next_metric;
    }
    while ( next != NULL );

    /* Append new sparse metric */
    current->next_metric = scorep_profile_create_sparse_double( location, metric, value );
}
