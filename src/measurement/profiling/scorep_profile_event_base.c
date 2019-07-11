/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
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
 *
 *
 *
 * Implementation of the base event functions for enter, exit, trigger metric, ...
 */

#include <config.h>

#include "scorep_profile_event_base.h"
#include <UTILS_Error.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Metric_Management.h>

#include <scorep_profile_node.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>

#include <string.h>
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
    memset( &node_data, 0, sizeof( node_data ) );

    /* If we are already in a collapse node -> do nothing more */
    if ( ( current_node != NULL ) &&
         ( current_node->node_type == SCOREP_PROFILE_NODE_COLLAPSE ) )
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
                                                 SCOREP_PROFILE_NODE_COLLAPSE,
                                                 node_data,
                                                 timestamp );
    }
    /* Regular enter */
    else
    {
        scorep_profile_type_set_region_handle( &node_data, region );
        node = scorep_profile_find_create_child( location,
                                                 current_node,
                                                 SCOREP_PROFILE_NODE_REGULAR_REGION,
                                                 node_data,
                                                 timestamp );
    }
    /* Disable profiling if node creation failed */
    if ( node == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Failed to create location. Disable profiling" );
        SCOREP_PROFILE_STOP( location );
        return NULL;
    }

    /* Store start values for dense metrics */
    node->count++;
    node->inclusive_time.start_value = timestamp;
    uint32_t number_of_synchronus_strict_metrics = SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics();
    for ( uint32_t i = 0; i < number_of_synchronus_strict_metrics; i++ )
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
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Exit event occurred in a thread which never entered a region" );
        SCOREP_PROFILE_STOP( location );
        return NULL;
    }

    /* If we are in a collapse node, check whether the current depth is still
       larger than the creation depth of the collapse node */
    if ( ( node->node_type == SCOREP_PROFILE_NODE_COLLAPSE ) &&
         ( location->current_depth > scorep_profile_type_get_depth( node->type_specific_data ) ) )
    {
        location->current_depth--;
        return node;
    }

    /* Exit all parameters and the region itself. Thus, more than one node may be exited.
       Initialize loop: start with this node. Further iterations should work on the
       parent. */
    parent = node;

    uint32_t number_of_synchronus_strict_metrics = SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics();

    do
    {
        location->current_depth--;
        node = parent;

        /* Update metrics */
        node->last_exit_time = timestamp;
        scorep_profile_update_dense_metric( &node->inclusive_time, timestamp );
        for ( uint32_t i = 0; i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
        {
            scorep_profile_update_dense_metric( &node->dense_metrics[ i ], metrics[ i ] );
        }

        parent = node->parent;
    }
    while ( ( node->node_type != SCOREP_PROFILE_NODE_REGULAR_REGION ) &&
            ( node->node_type != SCOREP_PROFILE_NODE_COLLAPSE ) &&
            ( parent != NULL ) );
    /* If this was a parameter node also exit next level node */

    if ( ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION ) &&
         ( scorep_profile_type_get_region_handle( node->type_specific_data ) != region ) )
    {
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Exit event for other than current region occurred at "
                     "location %" PRIu64 ": Expected exit for region '%s'. "
                     "Exited region '%s'",
                     scorep_profile_type_get_int_value( location->root_node->type_specific_data ),
                     SCOREP_RegionHandle_GetName( scorep_profile_type_get_region_handle( node->type_specific_data ) ),
                     SCOREP_RegionHandle_GetName( region ) );
        SCOREP_PROFILE_STOP( location );
        return NULL;
    }
    return parent;
}


void
scorep_profile_trigger_int64( SCOREP_Profile_LocationData*         location,
                              SCOREP_MetricHandle                  metric,
                              uint64_t                             value,
                              scorep_profile_node*                 node,
                              scorep_profile_trigger_update_scheme scheme )
{
    scorep_profile_sparse_metric_int* current = NULL;
    scorep_profile_sparse_metric_int* next    = NULL;

    /* If no sparse metrics are stored so far. */
    next = node->first_int_sparse;
    if ( next == NULL )
    {
        node->first_int_sparse = scorep_profile_create_sparse_int( location,
                                                                   metric, value, scheme );
        return;
    }

    /* Iterate all existing sparse metrics */
    do
    {
        current = next;
        if ( current->metric == metric )
        {
            scorep_profile_update_sparse_int( current, value, scheme );
            return;
        }
        next = current->next_metric;
    }
    while ( next != NULL );

    /* Append new sparse metric */
    current->next_metric = scorep_profile_create_sparse_int( location, metric, value, scheme );
}

void
scorep_profile_trigger_double(  SCOREP_Profile_LocationData*         location,
                                SCOREP_MetricHandle                  metric,
                                double                               value,
                                scorep_profile_node*                 node,
                                scorep_profile_trigger_update_scheme scheme )
{
    scorep_profile_sparse_metric_double* current = NULL;
    scorep_profile_sparse_metric_double* next    = NULL;

    /* If no sparse metrics are stored so far. */
    next = node->first_double_sparse;
    if ( next == NULL )
    {
        node->first_double_sparse = scorep_profile_create_sparse_double( location, metric, value, scheme );
        return;
    }

    /* Iterate all existing sparse metrics */
    do
    {
        current = next;
        if ( current->metric == metric )
        {
            scorep_profile_update_sparse_double( current, value, scheme );
            return;
        }
        next = current->next_metric;
    }
    while ( next != NULL );

    /* Append new sparse metric */
    current->next_metric = scorep_profile_create_sparse_double( location, metric, value, scheme );
}
