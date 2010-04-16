/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file silc_profile_algorithm.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status ALPHA
 *
 * Contains logic for post measurment profile modifications.
 *
 */

#include "SILC_Memory.h"
#include "SILC_Utils.h"

#include "silc_profile_definition.h"

/* Add dense metrics from source to destination */
void
silc_profile_merge_node_dense( silc_profile_node* destination,
                               silc_profile_node* source )
{
    int i;

    /* Merge static values */
    destination->count += source->count;
    if ( destination->first_enter_time > source->first_enter_time )
    {
        destination->first_enter_time = source->first_enter_time;
    }
    if ( destination->last_exit_time < source->last_exit_time )
    {
        destination->last_exit_time = source->last_exit_time;
    }

    /* Merge dense metrics */
    silc_profile_merge_dense_metric( &destination->implicit_time, &source->implicit_time );
    for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
    {
        silc_profile_merge_dense_metric( &destination->dense_metrics[ i ],
                                         &source->dense_metrics[ i ] );
    }
}

/* Add sparse metric from source to destination */
void
silc_profile_merge_node_sparse( silc_profile_node* destination,
                                silc_profile_node* source )
{
    silc_profile_sparse_metric_int*    dest_sparse_int      = NULL;
    silc_profile_sparse_metric_int*    source_sparse_int    = source->first_int_sparse;
    silc_profile_sparse_metric_double* dest_sparse_double   = NULL;
    silc_profile_sparse_metric_double* source_sparse_double = source->first_double_sparse;

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
            dest_sparse_int = silc_profile_copy_sparse_int( source_sparse_int );
        }
        else
        {
            silc_profile_merge_sparse_metric_int( dest_sparse_int, source_sparse_int );
            dest_sparse_int->next_metric  = destination->first_int_sparse;
            destination->first_int_sparse = dest_sparse_int;
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
            dest_sparse_double               = silc_profile_copy_sparse_double( source_sparse_double );
            dest_sparse_double->next_metric  = destination->first_double_sparse;
            destination->first_double_sparse = dest_sparse_double;
        }
        else
        {
            silc_profile_merge_sparse_metric_double( dest_sparse_double, source_sparse_double );
        }

        source_sparse_double = source_sparse_double->next_metric;
    }
}

/* Finds or creates a child of parent which matches type and adds statistics from
   source */
silc_profile_node*
silc_profile_merge_child( silc_profile_node* parent,
                          silc_profile_node* type,
                          silc_profile_node* source )
{
    /* Search matching node */
    SILC_ASSERT( parent != NULL );
    silc_profile_node* child = parent->first_child;
    while ( ( child != NULL ) &&
            !silc_profile_compare_nodes( child, type ) )
    {
        child = child->next_sibling;
    }

    /* If not found -> create new node */
    if ( child == NULL )
    {
        child = silc_profile_create_node( parent, type->node_type,
                                          type->type_specific_data, source->first_enter_time );
        child->next_sibling = parent->first_child;
        parent->first_child = child;
    }
    /* Add statistics */
    silc_profile_merge_node_dense( child, source );
    return child;
}

/* Adds a callpath to callpath_leaf to destination_root with the metrics from
   data_source.
 */
silc_profile_node*
silc_profile_add_callpath( silc_profile_node* destination_root,
                           silc_profile_node* callpath_leaf,
                           silc_profile_node* data_source )
{
    silc_profile_node* parent = callpath_leaf->parent;

    /* If parent is root node of the thread */
    if ( ( parent == NULL ) ||
         ( ( parent->node_type == silc_profile_node_thread_root ) ||
           ( parent->node_type == silc_profile_node_thread_start ) ) )
    {
        return silc_profile_merge_child( destination_root, callpath_leaf, data_source );
        //return destination_root;
    }

    /* Else reconstruct the new callpath */
    parent = silc_profile_add_callpath( destination_root,
                                        callpath_leaf->parent,
                                        data_source );
    return silc_profile_merge_child( parent, callpath_leaf, data_source );
}

/* Calculates the implicit metrics of parent from all its childs */
void
silc_profile_sum_children( silc_profile_node* parent )
{
    silc_profile_node* child = NULL;

    SILC_ASSERT( parent != NULL );

    child = parent->first_child;
    if ( child == NULL )
    {
        return;              /* Nothing to do */
    }
    /* Reinitialize thee metrics with values from first child */
    silc_profile_copy_all_dense_metrics( parent, child );

    /* Add the statistics of all children */
    while ( child->next_sibling != NULL )
    {
        child = child->next_sibling;
        silc_profile_merge_node_dense( parent, child );
    }
}

/* Replaces a node of type @ref silc_profile_node_thread_start with the callpath
   from the creation point.
 */
void
silc_profile_expand_thread_start( silc_profile_node* thread_start )
{
    silc_profile_node* creation_point = NULL;
    silc_profile_node* thread_root    = NULL;
    silc_profile_node* last_child     = NULL;

    /* Assertions and setting of relatives */
    SILC_ASSERT( thread_start != NULL );
    SILC_ASSERT( thread_start->node_type == silc_profile_node_thread_start );
    thread_root = thread_start->parent;
    SILC_ASSERT( thread_root != NULL ); /* Thread activation without location creation */
    SILC_ASSERT( thread_root->node_type == silc_profile_node_thread_root );

    creation_point = ( silc_profile_node* )
                     SILC_PROFILE_DATA2POINTER( thread_start->type_specific_data );

    /* Separate the thread_start node from the profile */
    silc_profile_remove_node( thread_start );

    /* If it has no children -> nothing to do */
    if ( thread_start->first_child == NULL )
    {
        return;
    }

    /* If no creation point is available, append the child list to the thread_root */
    if ( creation_point == NULL )
    {
        silc_profile_move_children( thread_root, thread_start );
    }
    /* Else insert callpath */
    else
    {
        /* Sum up shild statistics */
        silc_profile_sum_children( thread_start );

        /* Add callpath */
        creation_point = silc_profile_add_callpath( thread_root, creation_point,
                                                    thread_start );

        /* Move the subforest to the inserted callpath: */
        silc_profile_move_children( creation_point, thread_start );
    }
}

/* Expand all thread_start nodes of a thread_root */
void
silc_profile_expand_thread_root( silc_profile_node* thread_root )
{
    /* Expand the start nodes */
    silc_profile_node* thread_start = thread_root->first_child;
    silc_profile_node* next_node    = NULL;
    while ( thread_start != NULL )
    {
        /* Need to store the next sibling, because the current is removed
           -> next_sibling is set to NULL */
        next_node = thread_start->next_sibling;

        /* Expand thread_start node */
        if ( thread_start->node_type == silc_profile_node_thread_start )
        {
            silc_profile_expand_thread_start( thread_start );
        }

        /* Go to next node */
        thread_start = next_node;
    }

    /* Calculate thread statistics */
    silc_profile_sum_children( thread_root );
}

/* Expands all threads */
void
silc_profile_expand_threads()
{
    silc_profile_node* thread_root = silc_profile.first_root_node;
    while ( thread_root != NULL )
    {
        if ( thread_root->node_type == silc_profile_node_thread_root )
        {
            silc_profile_expand_thread_root( thread_root );
        }
        thread_root = thread_root->next_sibling;
    }
}
