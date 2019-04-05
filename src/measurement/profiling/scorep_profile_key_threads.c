/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universität Darmstadt, Darmstadt, Germany
 *
 * Copyright (c) 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * This file contains functions to aggregate profile data into a profile,
 * that contains:
 * <ol>
 *  <li> one location for the master thread, </li>
 *  <li> one location for the fastest thread, </li>
 *  <li> one location for the slowest thread, </li>
 *  <li> one location for the aggregated value of all other threads. </li>
 * </ol>
 *
 * The aggregation is part of the postprocessing.
 * The algorithm uses the thread start nodes to identify where to evaluate
 * the set of key threads. Thus, it must be executed before thread
 * expasion happens
 */

#include <config.h>
#include <scorep_profile_node.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_event_base.h>
#include <scorep_profile_location.h>

#include <SCOREP_Definitions.h>
#include <UTILS_Error.h>
#include <SCOREP_Types.h>

//#include <stdio.h>

#define MASTER_ID 0
#define FASTEST_ID 1
#define SLOWEST_ID 2
#define AGGREGATED_ID 3
#define INVALID_ID UINT64_MAX

/**
 * We add a metric that contains the number of threads that are contained in an
 * aggregated location. The handle to this metric is stored in this variable
 */
static SCOREP_MetricHandle number_of_threads_metric = SCOREP_INVALID_METRIC;

/* **************************************************************************************
 * A synchonuous iteratior over the call tree for all threads.
 * It traverses the calltree in depth-first order of the initial location.
 ***************************************************************************************/

/**
 * Data structure that holds the data needed by the iterator.
 */
typedef struct
{
    uint64_t              num_locations; /* Number of locations */
    scorep_profile_node** nodes;         /* Current position of every thread in the tree */
    uint64_t*             valid_dist;    /* Number of exists until current */
    scorep_profile_node*  start;         /* Remembers the start node */
} scorep_profile_sync_iterator;

/**
 * Allocates memory for an iterator.
 */
static scorep_profile_sync_iterator*
alloc_sync_iterator( uint64_t numLocations )
{
    scorep_profile_sync_iterator* new_iterator = malloc( sizeof( scorep_profile_sync_iterator ) );
    UTILS_ASSERT( new_iterator );
    new_iterator->num_locations = numLocations;
    new_iterator->nodes         = calloc( numLocations, sizeof( scorep_profile_node* ) );
    UTILS_ASSERT( new_iterator->nodes );
    new_iterator->valid_dist = calloc( numLocations, sizeof( uint64_t ) );
    UTILS_ASSERT( new_iterator->valid_dist );
    return new_iterator;
}

/**
 * Frees an iterator object
 */
static void
free_sync_iterator( scorep_profile_sync_iterator* iterator )
{
    free( iterator->valid_dist );
    free( iterator->nodes );
    free( iterator );
}

/**
 * Creates an iterator object
 */
static scorep_profile_sync_iterator*
create_sync_iterator( scorep_profile_node* firstRoot )
{
    uint64_t             num_locations = 0;
    scorep_profile_node* current       = firstRoot;

    for (;
         current != NULL;
         current = current->next_sibling )
    {
        num_locations++;
    }
    scorep_profile_sync_iterator* new_iterator = alloc_sync_iterator( num_locations );
    new_iterator->start = firstRoot;

    current = firstRoot;
    for ( uint64_t i = 0; i < num_locations; i++ )
    {
        new_iterator->nodes[ i ] = current;
        current                  = current->next_sibling;
    }
    return new_iterator;
}

/**
 * Increases the iterator to the next node.
 */
static void
inc_sync_iterator( scorep_profile_sync_iterator* iterator )
{
    scorep_profile_node* master = iterator->nodes[ 0 ];
    UTILS_ASSERT( master != NULL );

    if ( master->first_child != NULL )
    {
        master               = master->first_child;
        iterator->nodes[ 0 ] = master;

        for ( uint64_t i = 1; i < iterator->num_locations; i++ )
        {
            if ( iterator->valid_dist[ i ] == 0 )
            {
                scorep_profile_node* child =
                    scorep_profile_find_child( iterator->nodes[ i ], master );
                if ( child != NULL )
                {
                    iterator->nodes[ i ] = child;
                }
                else
                {
                    iterator->valid_dist[ i ] = 1;
                }
            }
            else
            {
                iterator->valid_dist[ i ]++;
            }
        }
    }
    else
    {
        while ( master->next_sibling == NULL )
        {
            for ( uint64_t i = 0; i < iterator->num_locations; i++ )
            {
                if ( iterator->valid_dist[ i ] == 0 )
                {
                    iterator->nodes[ i ] = iterator->nodes[ i ]->parent;
                }
                else
                {
                    iterator->valid_dist[ i ]--;
                }
            }
            master = iterator->nodes[ 0 ];
            if ( master == iterator->start )
            {
                return;
            }
        }
        master               = master->next_sibling;
        iterator->nodes[ 0 ] = master;

        for ( uint64_t i = 1; i < iterator->num_locations; i++ )
        {
            if ( iterator->valid_dist[ i ] == 1 )
            {
                scorep_profile_node* sibling =
                    scorep_profile_find_child( iterator->nodes[ i ], master );
                if ( sibling != NULL )
                {
                    iterator->nodes[ i ]      = sibling;
                    iterator->valid_dist[ i ] = 0;
                }
            }
            else if ( iterator->valid_dist[ i ] == 0 )
            {
                scorep_profile_node* sibling =
                    scorep_profile_find_child( iterator->nodes[ i ]->parent, master );
                if ( sibling != NULL )
                {
                    iterator->nodes[ i ] = sibling;
                }
                else
                {
                    iterator->nodes[ i ]      = iterator->nodes[ i ]->parent;
                    iterator->valid_dist[ i ] = 1;
                }
            }
        }
    }
}

/**
 * Returns the current profile node for location @a thread_no.
 * If this location has no node at the current callpath, It returns
 * NULL.
 */
static inline scorep_profile_node*
sync_iterator_get_node( scorep_profile_sync_iterator* iterator,
                        uint64_t                      threadNum,
                        uint64_t                      ancestorLevel )
{
    if ( iterator->valid_dist[ threadNum ] > ancestorLevel )
    {
        return NULL;
    }

    scorep_profile_node* current = iterator->nodes[ threadNum ];
    for ( uint64_t i = 0; i < ancestorLevel - iterator->valid_dist[ threadNum ]; i++ )
    {
        current = current->parent;
        UTILS_ASSERT( current != NULL );
    }
    return current;
}

/* **************************************************************************************
 * other static functions
 ***************************************************************************************/

/**
 * Helper function to create the data for the number of threads metric.
 * It is called for every node before node and sets the number of
 * threads metric to 1. Through the aggregation scheme, the correct value
 * will be aggregated.
 */
static void
add_num_threads_metric( scorep_profile_node* node, void* param )
{
    if ( node->count > 0 )
    {
        scorep_profile_trigger_int64( ( SCOREP_Profile_LocationData* )param,
                                      number_of_threads_metric,
                                      1,
                                      node,
                                      SCOREP_PROFILE_TRIGGER_UPDATE_VALUE_AS_IS );
    }
}

static void
merge_nodes( SCOREP_Profile_LocationData*  location,
             scorep_profile_sync_iterator* iterator )
{
    scorep_profile_node* destination = sync_iterator_get_node( iterator,
                                                               AGGREGATED_ID, 0 );
    for ( uint64_t i = AGGREGATED_ID + 1; i < iterator->num_locations; i++ )
    {
        scorep_profile_node* current = sync_iterator_get_node( iterator, i, 0 );
        if ( current != NULL )
        {
            if ( destination == NULL )
            {
                iterator->nodes[ i ]      = current->parent;
                iterator->valid_dist[ i ] = 1;
                scorep_profile_remove_node( current );
                scorep_profile_add_child( sync_iterator_get_node( iterator,
                                                                  AGGREGATED_ID, 1 ),
                                          current );
                iterator->nodes[ AGGREGATED_ID ]      = current;
                iterator->valid_dist[ AGGREGATED_ID ] = 0;
                destination                           = current;
            }
            else
            {
                scorep_profile_merge_node_dense( destination, current );
                scorep_profile_merge_node_sparse( location, destination, current );
            }
        }
    }
}

static uint64_t
get_execution_time( scorep_profile_node* node );

/**
 * Returns the idle time inside a subtree. As idle time all time is
 * counted that is exclusive time to synchronization constructs.
 */
static uint64_t
get_idle_time( scorep_profile_node* node )
{
    uint64_t idle_time = 0;

    /* Check whether this node represents an idle node */
    if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        SCOREP_RegionHandle region
            = scorep_profile_type_get_region_handle( node->type_specific_data );
        SCOREP_RegionType type = SCOREP_RegionHandle_GetType( region );

        switch ( type )
        {
            case SCOREP_REGION_CRITICAL:
            case SCOREP_REGION_TASK_WAIT:
            case SCOREP_REGION_BARRIER:
            case SCOREP_REGION_IMPLICIT_BARRIER:
                return node->inclusive_time.sum - get_execution_time( node );
        }
    }

    /* Sum up idle time from children */
    for ( scorep_profile_node* child = node->first_child;
          child != NULL;
          child = child->next_sibling )
    {
        idle_time += get_idle_time( child );
    }

    return idle_time;
}

/**
 * Returns the execution time contained in a subtree.
 * This time includes everything except exclusive time of
 * synchronization constructs.
 */
static uint64_t
get_execution_time( scorep_profile_node* node )
{
    uint64_t execution_time = 0;

    for ( scorep_profile_node* child = node->first_child;
          child != NULL;
          child = child->next_sibling )
    {
        execution_time += child->inclusive_time.sum - get_idle_time( child );
    }

    return execution_time;
}

static void
switch_locations( scorep_profile_sync_iterator* iterator,
                  uint64_t a, uint64_t b )
{
    scorep_profile_node* child_a = sync_iterator_get_node( iterator, a, 0 );
    scorep_profile_node* child_b = sync_iterator_get_node( iterator, b, 0 );

    UTILS_ASSERT( child_a != NULL );
    UTILS_ASSERT( child_b != NULL );

    scorep_profile_node* parent_a = child_a->parent;
    scorep_profile_node* parent_b = child_b->parent;

    /* We switch root nodes */
    if ( parent_a == NULL && parent_b == NULL )
    {
        iterator->nodes[ a ] = child_b;
        iterator->nodes[ b ] = child_a;
        for ( uint64_t i = 1; i < iterator->num_locations; i++ )
        {
            iterator->nodes[ i - 1 ]->next_sibling = iterator->nodes[ i ];
        }
        iterator->nodes[ iterator->num_locations - 1 ]->next_sibling = NULL;
        return;
    }

    /* Fails if the callpath to here is missing on one thread */
    UTILS_ASSERT( parent_a != NULL );
    UTILS_ASSERT( parent_b != NULL );

    scorep_profile_remove_node( child_a );
    scorep_profile_remove_node( child_b );
    scorep_profile_add_child( parent_a, child_b );
    scorep_profile_add_child( parent_b, child_a );

    iterator->nodes[ a ] = child_b;
    iterator->nodes[ b ] = child_a;
}

static void
calculate_key_locations( scorep_profile_sync_iterator* iterator )
{
    uint64_t fastest_id   = INVALID_ID;
    uint64_t slowest_id   = INVALID_ID;
    uint64_t fastest_time = UINT64_MAX;
    uint64_t slowest_time = 0;

    for ( uint64_t i = 1; i < iterator->num_locations; i++ )
    {
        scorep_profile_node* current = sync_iterator_get_node( iterator, i, 0 );
        if ( current != NULL )
        {
            uint64_t time = current->inclusive_time.sum - get_idle_time( current );
            if ( time < fastest_time )
            {
                fastest_id   = i;
                fastest_time = time;
            }
            else if ( time > slowest_time )
            {
                slowest_id   = i;
                slowest_time = time;
            }
        }
    }

    if ( fastest_id != INVALID_ID && fastest_id != FASTEST_ID )
    {
        switch_locations( iterator, fastest_id, FASTEST_ID );
    }
    if ( slowest_id != INVALID_ID && slowest_id != SLOWEST_ID )
    {
        switch_locations( iterator, slowest_id, SLOWEST_ID );
    }
}

/* **************************************************************************************
 * external visible functions
 ***************************************************************************************/
void
scorep_profile_init_num_threads_metric( void )
{
    /* Initialize all nodes with the number of threads metric */
    number_of_threads_metric =
        SCOREP_Definitions_NewMetric( "number of threads",
                                      "num_threads",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "threads",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                      SCOREP_INVALID_METRIC );

    for ( scorep_profile_node* current = scorep_profile.first_root_node;
          current != NULL;
          current = current->next_sibling )
    {
        SCOREP_Profile_LocationData* location =
            scorep_profile_type_get_location_data( current->type_specific_data );
        scorep_profile_for_all( current,
                                &add_num_threads_metric,
                                location );
    }
}

void
scorep_profile_cluster_key_threads( void )
{
    scorep_profile_init_num_threads_metric();

    scorep_profile_sync_iterator* iterator =
        create_sync_iterator( scorep_profile.first_root_node );

    SCOREP_Profile_LocationData* location =
        scorep_profile_get_location_of_node( scorep_profile.first_root_node );

    calculate_key_locations( iterator );

    do
    {
        if ( scorep_profile_is_fork_node( iterator->nodes[ MASTER_ID ] ) )
        {
            calculate_key_locations( iterator );
        }
        merge_nodes( location, iterator  );

        inc_sync_iterator( iterator );
    }
    while ( iterator->nodes[ MASTER_ID ] != scorep_profile.first_root_node );

    free_sync_iterator( iterator );
}
