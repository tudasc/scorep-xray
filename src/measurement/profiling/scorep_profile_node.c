/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015-2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015-2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * Implementation for managing profile node trees.
 *
 *
 *
 */

#include <config.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include <SCOREP_Memory.h>
#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include <scorep_profile_node.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>

#include <SCOREP_Metric_Management.h>

/* ***************************************************************************************
   Creation / Destruction
*****************************************************************************************/

/** Creates a new child node of given type and data */
scorep_profile_node*
scorep_profile_create_node( SCOREP_Profile_LocationData* location,
                            scorep_profile_node*         parent,
                            scorep_profile_node_type     type,
                            scorep_profile_type_data_t   data,
                            uint64_t                     timestamp,
                            scorep_profile_task_context  context )
{
    scorep_profile_node* node = scorep_profile_alloc_node( location, type, context );
    if ( node == NULL )
    {
        return NULL;
    }

    /* Initialize values */
    node->callpath_handle     = SCOREP_INVALID_CALLPATH;
    node->parent              = parent;
    node->first_child         = NULL;
    node->next_sibling        = NULL;
    node->first_double_sparse = NULL;
    node->first_int_sparse    = NULL;
    node->flags               = 0;
    node->count               = 0;    /* Is increased to one during SCOREP_Profile_Enter() */
    node->hits                = 0;
    node->first_enter_time    = timestamp;
    node->last_exit_time      = timestamp;
    node->node_type           = type;
    scorep_profile_copy_type_data( &node->type_specific_data, data, type );

    /* Initialize dense metric values */
    scorep_profile_init_dense_metric( &node->inclusive_time );
    scorep_profile_init_dense_metric_array( node->dense_metrics,
                                            SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics() );

    return node;
}

/* Copies a node */
scorep_profile_node*
scorep_profile_copy_node( SCOREP_Profile_LocationData* location,
                          scorep_profile_node*         source )
{
    scorep_profile_sparse_metric_int*    dest_sparse_int      = NULL;
    scorep_profile_sparse_metric_int*    source_sparse_int    = source->first_int_sparse;
    scorep_profile_sparse_metric_double* dest_sparse_double   = NULL;
    scorep_profile_sparse_metric_double* source_sparse_double = source->first_double_sparse;

    /* Create node */
    scorep_profile_node* node = scorep_profile_create_node( location,
                                                            NULL,
                                                            source->node_type,
                                                            source->type_specific_data,
                                                            0,
                                                            scorep_profile_get_task_context( source ) );
    node->flags = source->flags;

    /* Copy dense metric values */
    scorep_profile_copy_all_dense_metrics( node, source );

    /* Copy sparse integer metrics */
    while ( source_sparse_int != NULL )
    {
        dest_sparse_int = scorep_profile_copy_sparse_int( location,
                                                          source_sparse_int );
        dest_sparse_int->next_metric = node->first_int_sparse;
        node->first_int_sparse       = dest_sparse_int;

        source_sparse_int = source_sparse_int->next_metric;
    }

    /* Copy sparse double metrics */
    while ( source_sparse_double != NULL )
    {
        dest_sparse_double = scorep_profile_copy_sparse_double( location,
                                                                source_sparse_double );
        dest_sparse_double->next_metric = node->first_double_sparse;
        node->first_double_sparse       = dest_sparse_double;

        source_sparse_double = source_sparse_double->next_metric;
    }

    return node;
}

void
scorep_profile_release_subtree( SCOREP_Profile_LocationData* location,
                                scorep_profile_node*         root )
{
    /* Process all children recursively */
    scorep_profile_node* child = root->first_child;
    while ( child != NULL )
    {
        scorep_profile_release_subtree( location, child );
        child = child->next_sibling;
    }

    /* Extract sparse integer metric chain */
    scorep_profile_sparse_metric_int* last_int_metric = root->first_int_sparse;
    if ( last_int_metric != NULL )
    {
        while ( last_int_metric->next_metric != NULL )
        {
            last_int_metric = last_int_metric->next_metric;
        }
        last_int_metric->next_metric = location->free_int_metrics;
        location->free_int_metrics   = root->first_int_sparse;
    }

    /* Extract sparse double metric chain */
    scorep_profile_sparse_metric_double* last_double_metric = root->first_double_sparse;
    if ( last_double_metric != NULL )
    {
        while ( last_double_metric->next_metric != NULL )
        {
            last_double_metric = last_double_metric->next_metric;
        }
        last_double_metric->next_metric = location->free_double_metrics;
        location->free_double_metrics   = root->first_double_sparse;
    }

    /* Insert this node into list of released nodes */
    if ( scorep_profile_get_task_context( root ) == SCOREP_PROFILE_TASK_CONTEXT_UNTIED )
    {
        scorep_profile_release_stubs( location, root, root, 1, false );
    }
    else
    {
        root->first_child    = location->free_nodes;
        location->free_nodes = root;
    }
}

scorep_profile_node*
scorep_profile_alloc_node( SCOREP_Profile_LocationData* location,
                           scorep_profile_node_type     type,
                           scorep_profile_task_context  context )
{
    scorep_profile_node* new_node;

    /* Try to recycle released nodes */
    if ( ( context == SCOREP_PROFILE_TASK_CONTEXT_TIED ) &&
         ( location != NULL ) &&
         ( location->free_nodes != NULL ) &&
         ( type != SCOREP_PROFILE_NODE_THREAD_ROOT ) )
    {
        new_node             = location->free_nodes;
        location->free_nodes = new_node->first_child;
        return new_node;
    }
    else if ( ( type != SCOREP_PROFILE_NODE_THREAD_ROOT ) &&
              ( ( new_node = scorep_profile_recycle_stub( location ) ) != NULL ) )
    {
        return new_node;
    }

    /* Allocate new memory if no released nodes are available */

    /* Reserve space for the node record and dense metrics.
     *  Thread root nodes must not be deleted in Periscope phases, while all other
     *  nodes are. The profile memory pages are deleted in Periscope phases.
     *  Thus, space for thread root nodes must not be allocated
     *  from profile memory.
     */
    if ( type == SCOREP_PROFILE_NODE_THREAD_ROOT )
    {
        new_node = ( scorep_profile_node* )
                   SCOREP_Location_AllocForMisc( location->location_data,
                                                 sizeof( scorep_profile_node ) );
    }
    else
    {
        new_node = ( scorep_profile_node* )
                   SCOREP_Location_AllocForProfile( location->location_data,
                                                    sizeof( scorep_profile_node ) );
    }
    memset( new_node, 0, sizeof( *new_node ) );

    /* Reserve space for dense metrics,
       Since the metric number may vary on reinitialization, allocate it
       also for root nodes from profile memory which get freed on
       finalization and reallocate the memory for root nodes on
       reinitialization
     */
    if ( SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics() > 0 )
    {
        /* Size of the allocated memory for dense metrics */
        uint32_t size = SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics() *
                        sizeof( scorep_profile_dense_metric );

        new_node->dense_metrics = ( scorep_profile_dense_metric* )
                                  SCOREP_Location_AllocForProfile( location->location_data, size );
    }
    else
    {
        new_node->dense_metrics = NULL;
    }

    scorep_profile_set_task_context( new_node, context );

    return new_node;
}

/* ***************************************************************************************
   Node operation
*****************************************************************************************/

void
scorep_profile_add_child( scorep_profile_node* parent,
                          scorep_profile_node* child )
{
    child->next_sibling = parent->first_child;
    parent->first_child = child;
    child->parent       = parent;
}

bool
scorep_profile_compare_nodes( scorep_profile_node* node1,
                              scorep_profile_node* node2 )
{
    if ( node1->node_type != node2->node_type )
    {
        return false;
    }
    return scorep_profile_compare_type_data( node1->type_specific_data,
                                             node2->type_specific_data,
                                             node1->node_type );
}

uint64_t
scorep_profile_node_hash( scorep_profile_node* node )
{
    uint64_t val;
    val  = node->node_type;
    val  =  ( val >> 1 ) | ( val << 31 );
    val += scorep_profile_hash_for_type_data( node->type_specific_data,
                                              node->node_type );

    return val;
}

/* Provides an ordering for nodes */
bool
scorep_profile_node_less_than( scorep_profile_node* a,
                               scorep_profile_node* b )
{
    if ( a->node_type < b->node_type )
    {
        return true;
    }
    if ( a->node_type > b->node_type )
    {
        return false;
    }
    return scorep_profile_less_than_for_type_data( a->type_specific_data,
                                                   b->type_specific_data,
                                                   a->node_type );
}

/* Copies all dense metrics from source to destination */
void
scorep_profile_copy_all_dense_metrics( scorep_profile_node* destination,
                                       scorep_profile_node* source )
{
    destination->count            = source->count;
    destination->first_enter_time = source->first_enter_time;
    destination->last_exit_time   = source->last_exit_time;

    /* Copy dense metric values */
    scorep_profile_copy_dense_metric( &destination->inclusive_time, &source->inclusive_time );
    for ( uint32_t i = 0; i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
    {
        scorep_profile_copy_dense_metric( &destination->dense_metrics[ i ],
                                          &source->dense_metrics[ i ] );
    }
}

/* Moves the children of a node to another node */
void
scorep_profile_move_children(  scorep_profile_node* destination,
                               scorep_profile_node* source )
{
    scorep_profile_node* child = NULL;

    UTILS_ASSERT( source != NULL );

    /* If source has no child -> nothing to do */
    child = source->first_child;
    if ( child == NULL )
    {
        return;
    }

    /* Set new parent of all children of source. */
    while ( child != NULL )
    {
        child->parent = destination;
        child         = child->next_sibling;
    }

    /* If destination is NULL -> append all children as root nodes */
    if ( destination == NULL )
    {
        child = scorep_profile.first_root_node;

        /* If profile is empty */
        if ( child == NULL )
        {
            scorep_profile.first_root_node = source->first_child;
            source->first_child            = NULL;
            return;
        }

        /* Append at end of root nodes, see below */
    }
    else
    {
        /* If destination has no children */
        child = destination->first_child;
        if ( child == NULL )
        {
            destination->first_child = source->first_child;
            source->first_child      = NULL;
            return;
        }
        /* Else append list of source to end of children list of destination, see below */
    }

    /* Search end of sibling list and append children */
    while ( child->next_sibling != NULL )
    {
        child = child->next_sibling;
    }
    child->next_sibling = source->first_child;
    source->first_child = NULL;
}

/* Removes a node with it subtree from its parents children */
void
scorep_profile_remove_node( scorep_profile_node* node )
{
    UTILS_ASSERT( node != NULL );

    scorep_profile_node* parent = node->parent;
    scorep_profile_node* before = NULL;

    /* Obtain start of the siblings list of node */
    if ( parent == NULL )
    {
        before = scorep_profile.first_root_node;
    }
    else
    {
        before = parent->first_child;
    }

    /* If node is the first entry */
    if ( before == node )
    {
        if ( parent == NULL )
        {
            scorep_profile.first_root_node = node->next_sibling;
        }
        else
        {
            parent->first_child = node->next_sibling;
        }
        node->parent       = NULL;
        node->next_sibling = NULL;
        return;
    }

    /* Else search for the sibling before node. */
    while ( ( before != NULL ) && ( before->next_sibling != node ) )
    {
        before = before->next_sibling;
    }

    /* Node is already removed */
    if ( before == NULL )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                            "Trying to remove a node which is not contained in the siblings "
                            "list.\nMaybe an inconsistent profile." );
        node->parent       = NULL;
        node->next_sibling = NULL;
        return;
    }

    /* Remove node from list */
    before->next_sibling = node->next_sibling;
    node->parent         = NULL;
    node->next_sibling   = NULL;
}

/* Traverse a subtree (depth search) and execute a given function on each node */
void
scorep_profile_for_all( scorep_profile_node*           root_node,
                        scorep_profile_process_func_t* func,
                        void*                          param )
{
    scorep_profile_node* current = root_node;

    /* Process root node */
    if ( current == NULL )
    {
        return;
    }
    ( *func )( current, param );

    /* Process children */
    current = current->first_child;
    if ( current == NULL )
    {
        return;
    }

    while ( current != root_node )
    {
        ( *func )( current, param );

        /* Find next node */
        if ( current->first_child != NULL )
        {
            current = current->first_child;
        }
        else
        {
            do
            {
                if ( current->next_sibling != NULL )
                {
                    current = current->next_sibling;
                    break;
                }
                current = current->parent;
            }
            while ( current != root_node );
        }
    }
}

/* Finds a child node of a specified type */
scorep_profile_node*
scorep_profile_find_child( scorep_profile_node* parent,
                           scorep_profile_node* type )
{
    /* Search matching node */
    UTILS_ASSERT( parent != NULL );
    scorep_profile_node* child = parent->first_child;
    while ( ( child != NULL ) &&
            !scorep_profile_compare_nodes( child, type ) )
    {
        child = child->next_sibling;
    }

    return child;
}

/* Find or create a child node of a specified type */
scorep_profile_node*
scorep_profile_find_create_child( SCOREP_Profile_LocationData* location,
                                  scorep_profile_node*         parent,
                                  scorep_profile_node_type     node_type,
                                  scorep_profile_type_data_t   specific_data,
                                  uint64_t                     timestamp )
{
    /* Search matching node */
    UTILS_ASSERT( parent != NULL );
    scorep_profile_node* child = parent->first_child;
    while ( ( child != NULL ) &&
            ( ( child->node_type != node_type ) ||
              ( !scorep_profile_compare_type_data( specific_data,
                                                   child->type_specific_data,
                                                   node_type ) ) ) )
    {
        child = child->next_sibling;
    }

    /* If not found -> create new node */
    if ( child == NULL )
    {
        child = scorep_profile_create_node( location, parent, node_type,
                                            specific_data,
                                            timestamp,
                                            scorep_profile_get_task_context( parent ) );
        child->next_sibling = parent->first_child;
        parent->first_child = child;
    }

    return child;
}

uint64_t
scorep_profile_get_number_of_children( scorep_profile_node* node )
{
    uint64_t             count = 0;
    scorep_profile_node* child = NULL;

    if ( node == NULL )
    {
        return 0;
    }

    child = node->first_child;
    while ( child != NULL )
    {
        count++;
        child = child->next_sibling;
    }
    return count;
}

uint64_t
scorep_profile_get_number_of_child_calls( scorep_profile_node* node )
{
    uint64_t             count = 0;
    scorep_profile_node* child = NULL;

    if ( node == NULL )
    {
        return 0;
    }

    child = node->first_child;
    while ( child != NULL )
    {
        count += child->count;
        child  = child->next_sibling;
    }
    return count;
}

uint64_t
scorep_profile_get_exclusive_time( scorep_profile_node* node )
{
    uint64_t             exclusive_time;
    scorep_profile_node* child;

    if ( node == NULL )
    {
        return 0;
    }

    exclusive_time = node->inclusive_time.sum;

    child = node->first_child;
    while ( child != NULL )
    {
        exclusive_time -= child->inclusive_time.sum;
        child           = child->next_sibling;
    }
    return exclusive_time;
}

void
scorep_profile_merge_node_inclusive( scorep_profile_node* destination,
                                     scorep_profile_node* source )
{
    int i;

    /* Merge static values */
    if ( destination->first_enter_time > source->first_enter_time )
    {
        destination->first_enter_time = source->first_enter_time;
    }
    if ( destination->last_exit_time < source->last_exit_time )
    {
        destination->last_exit_time = source->last_exit_time;
    }

    /* Merge dense metrics */
    scorep_profile_merge_dense_metric( &destination->inclusive_time, &source->inclusive_time );
    for ( i = 0; i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
    {
        scorep_profile_merge_dense_metric( &destination->dense_metrics[ i ],
                                           &source->dense_metrics[ i ] );
    }
}

SCOREP_Profile_LocationData*
scorep_profile_get_location_of_node( scorep_profile_node* node )
{
    while ( node != NULL )
    {
        if ( node->node_type == SCOREP_PROFILE_NODE_THREAD_ROOT )
        {
            return scorep_profile_type_get_location_data( node->type_specific_data );
        }
        node = node->parent;
    }
    return NULL;
}

void

scorep_profile_merge_node_dense( scorep_profile_node* destination,
                                 scorep_profile_node* source )
{
    /* Merge static values */
    destination->count += source->count;
    scorep_profile_merge_node_inclusive( destination, source );
}

void
scorep_profile_merge_node_sparse( SCOREP_Profile_LocationData* location,
                                  scorep_profile_node*         destination,
                                  scorep_profile_node*         source )
{
    scorep_profile_sparse_metric_int*    dest_sparse_int      = NULL;
    scorep_profile_sparse_metric_int*    source_sparse_int    = source->first_int_sparse;
    scorep_profile_sparse_metric_double* dest_sparse_double   = NULL;
    scorep_profile_sparse_metric_double* source_sparse_double = source->first_double_sparse;

    /* Merge sparse integer metrics */
    while ( source_sparse_int != NULL )
    {
        dest_sparse_int = destination->first_int_sparse;
        while ( ( dest_sparse_int != NULL ) &&
                ( dest_sparse_int->metric != source_sparse_int->metric ) )
        {
            dest_sparse_int = dest_sparse_int->next_metric;
        }
        if ( dest_sparse_int == NULL )
        {
            dest_sparse_int = scorep_profile_copy_sparse_int( location,
                                                              source_sparse_int );
            dest_sparse_int->next_metric  = destination->first_int_sparse;
            destination->first_int_sparse = dest_sparse_int;
        }
        else
        {
            scorep_profile_merge_sparse_metric_int( dest_sparse_int, source_sparse_int );
        }

        source_sparse_int = source_sparse_int->next_metric;
    }

    /* Merge sparse double metrics */
    while ( source_sparse_double != NULL )
    {
        dest_sparse_double = destination->first_double_sparse;
        while ( ( dest_sparse_double != NULL ) &&
                ( dest_sparse_double->metric != source_sparse_double->metric ) )
        {
            dest_sparse_double = dest_sparse_double->next_metric;
        }
        if ( dest_sparse_double == NULL )
        {
            dest_sparse_double               = scorep_profile_copy_sparse_double( location, source_sparse_double );
            dest_sparse_double->next_metric  = destination->first_double_sparse;
            destination->first_double_sparse = dest_sparse_double;
        }
        else
        {
            scorep_profile_merge_sparse_metric_double( dest_sparse_double, source_sparse_double );
        }

        source_sparse_double = source_sparse_double->next_metric;
    }
}

void
scorep_profile_subtract_node( scorep_profile_node* minuend,
                              scorep_profile_node* subtrahend )
{
    minuend->count                  -= subtrahend->count;
    minuend->inclusive_time.sum     -= subtrahend->inclusive_time.sum;
    minuend->inclusive_time.squares -= subtrahend->inclusive_time.squares;
    for ( uint64_t i = 0; i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
    {
        minuend->dense_metrics[ i ].sum     -= subtrahend->dense_metrics[ i ].sum;
        minuend->dense_metrics[ i ].squares -= subtrahend->dense_metrics[ i ].squares;
    }
}

/**
   Searches for a a thread start node to a given fork
   in a specific location subtree.
   @param root  The root node of a location subtree.
   @param fork  The node where the fork happened.
   @return The thread start node forked at @a fork if it exists.
           If no matching node is found, the function returns NULL.
 */
static scorep_profile_node*
get_thread_start_for_fork( scorep_profile_node* root,
                           scorep_profile_node* fork )
{
    scorep_profile_node* child = root->first_child;
    while ( child != NULL )
    {
        if ( ( child->node_type == SCOREP_PROFILE_NODE_THREAD_START ) &&
             ( scorep_profile_type_get_fork_node( child->type_specific_data ) == fork ) )
        {
            return child;
        }

        child = child->next_sibling;
    }
    return NULL;
}

void
scorep_profile_substitute_thread_starts( scorep_profile_node* old,
                                         scorep_profile_node* substitute )
{
    for ( scorep_profile_node* root = scorep_profile.first_root_node;
          root != NULL;
          root = root->next_sibling )
    {
        scorep_profile_node* child = get_thread_start_for_fork( root, old );
        if ( child != NULL )
        {
            scorep_profile_type_set_fork_node( &child->type_specific_data, substitute );
        }
    }
}

/**
   Merges all on all locations the subtrees rooted in a thread start node that
   point to @a source as its fork node into the subtree that
   point to @a destination.as its fork node.
   @param destination Pointer to a fork node.
   @param source      Pointer to a fork node.
 */
static void
merge_thread_starts( scorep_profile_node* destination,
                     scorep_profile_node* source )
{
    for ( scorep_profile_node* root = scorep_profile.first_root_node;
          root != NULL;
          root = root->next_sibling )
    {
        scorep_profile_node* src = get_thread_start_for_fork( root, source );
        if ( src == NULL )
        {
            continue;
        }

        scorep_profile_node* dest = get_thread_start_for_fork( root, destination );
        if ( dest == NULL )
        {
            scorep_profile_type_set_fork_node( &src->type_specific_data, destination );
            continue;
        }

        scorep_profile_remove_node( src );
        scorep_profile_merge_subtree( scorep_profile_get_location_of_node( root ),
                                      dest, src );
    }
}

void
scorep_profile_merge_subtree( SCOREP_Profile_LocationData* location,
                              scorep_profile_node*         destination,
                              scorep_profile_node*         source )
{
    assert( destination );
    assert( source );

    /* Handle forked subtrees */
    if ( scorep_profile_is_fork_node( source ) )
    {
        if ( scorep_profile_is_fork_node( destination ) )
        {
            /* Merge the subtrees of the nodes */
            merge_thread_starts( destination, source );
        }
        else
        {
            /* Set the thread start node data that points to the source tree to
               the destination tree */
            scorep_profile_substitute_thread_starts( source, destination );
        }
    }

    /* Merge current node data */
    scorep_profile_merge_node_dense( destination, source );
    scorep_profile_merge_node_sparse( location, destination, source );
    destination->flags = destination->flags | source->flags;

    /* Process children */
    scorep_profile_node* child = source->first_child;
    scorep_profile_node* next  = NULL;
    scorep_profile_node* match = NULL;
    while ( child != NULL )
    {
        next  = child->next_sibling;
        match = scorep_profile_find_child( destination, child );

        /* If no equal child node of destination exists, insert the child of source as
           first child of destination. */
        if ( match == NULL )
        {
            scorep_profile_add_child( destination, child );
        }
        /* If a matching node exists, merge the subtree recursively */
        else
        {
            scorep_profile_merge_subtree( location, match, child );
        }

        child = next;
    }

    /* Clean up. The children are either integrated into another tree, or already
       released, recursively. Thus we must release only this node. */
    source->first_child = NULL;
    scorep_profile_release_subtree( location, source );
}

/**
   Helper function for @ref scorep_profile_sort_subtree. Implements a recursive
   merge sort algorithm on a linked list of profile nodes.
   @param head    Head of the linked list of profile nodes. Will return the head of
                  the sorted list.
   @param tail    Returns the tail of the sorted list.
   @param number  Number of elements in the list.
   @param func    Comparison function.
 */

static void
sort_node_list( scorep_profile_node**          head,
                scorep_profile_node**          tail,
                uint32_t                       number,
                scorep_profile_compare_node_t* func )
{
    /* If it's a single element list, return that list */
    if ( number <= 1 )
    {
        *tail = *head;
        return;
    }
    uint32_t              pos;
    uint32_t              pos_end = number / 2;
    scorep_profile_node*  head_tail;
    scorep_profile_node*  mid;
    scorep_profile_node** last;
    /* Find the middle of the list */
    for ( pos = 0, mid = *head; pos < pos_end; pos++ )
    {
        last = &( mid->next_sibling );
        mid  = *last;
    }

    /* Cut the list in the middle */
    *last = NULL;

    /* Sort the two sub-lists */
    sort_node_list( head, &head_tail, pos_end, func );
    sort_node_list( &mid, tail, number - pos_end, func );

    /* See if it's a merge or a concatenation */
    // TODO: What is nodeid?
    if ( !func( head_tail, mid ) )
    {
        /* Concatenate the two lists */
        head_tail->next_sibling = mid;
    }
    else
    {
        /* Merge the two lists */
        last      = head;
        head_tail = *last;
        while ( NULL != mid && NULL != head_tail )
        {
            if ( !func( head_tail, mid ) )
            {
                /* Just advance head_tail */
                last      = &( head_tail->next_sibling );
                head_tail = *last;
            }
            else
            {
                /* Insert the element from mid before head_tail and advance mid */
                scorep_profile_node* mid_next = mid->next_sibling;
                *last = mid;
                last  = &( mid->next_sibling );
                *last = head_tail;
                mid   = mid_next;
            }
        }
        if ( NULL != mid )
        {
            /* Concatenate. tail is already at the end of the list */
            *last = mid;
        }
        else if ( NULL != head_tail )
        {
            /* Bring tail to the end of the list */
            while ( NULL != head_tail->next_sibling )
            {
                head_tail = head_tail->next_sibling;
            }
            *tail = head_tail;
        }
    }
}

void
scorep_profile_sort_subtree( scorep_profile_node*           root,
                             scorep_profile_compare_node_t* comparisionFunc )
{
    scorep_profile_node* curr;

    /* Sort children */
    sort_node_list( &( root->first_child ), &curr,
                    scorep_profile_get_number_of_children( root ),
                    comparisionFunc );

    /* Sort the subtrees of the children */
    for ( curr = root->first_child; NULL != curr; curr = curr->next_sibling )
    {
        scorep_profile_sort_subtree( curr, comparisionFunc );
    }
}

bool
scorep_profile_is_mpi_in_subtree( scorep_profile_node* node )
{
    return node->flags & SCOREP_PROFILE_FLAG_MPI_IN_SUBTREE;
}

void
scorep_profile_set_mpi_in_subtree( scorep_profile_node* node,
                                   bool                 mpiInSubtree )
{
    node->flags = ( mpiInSubtree ?
                    node->flags | SCOREP_PROFILE_FLAG_MPI_IN_SUBTREE :
                    node->flags & ( ~SCOREP_PROFILE_FLAG_MPI_IN_SUBTREE ) );
}

bool
scorep_profile_is_fork_node( scorep_profile_node* node )
{
    return node->flags & SCOREP_PROFILE_FLAG_IS_FORK_NODE;
}

void
scorep_profile_set_fork_node( scorep_profile_node* node,
                              bool                 isForkNode )
{
    node->flags = ( isForkNode ?
                    node->flags | SCOREP_PROFILE_FLAG_IS_FORK_NODE :
                    node->flags & ( ~SCOREP_PROFILE_FLAG_IS_FORK_NODE ) );
}

scorep_profile_task_context
scorep_profile_get_task_context( scorep_profile_node* node )
{
    return ( node->flags & SCOREP_PROFILE_FLAG_IN_UNTIED_TASK ) ?
           SCOREP_PROFILE_TASK_CONTEXT_UNTIED :
           SCOREP_PROFILE_TASK_CONTEXT_TIED;
}

void
scorep_profile_set_task_context( scorep_profile_node*        node,
                                 scorep_profile_task_context context )
{
    node->flags = ( context == SCOREP_PROFILE_TASK_CONTEXT_UNTIED ?
                    node->flags | SCOREP_PROFILE_FLAG_IN_UNTIED_TASK :
                    node->flags & ( ~SCOREP_PROFILE_FLAG_IN_UNTIED_TASK ) );
}
