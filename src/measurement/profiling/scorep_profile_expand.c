/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file scorep_profile_expand.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 * Contains implmentation for post-processing operations to expand thread start nodes.
 * In this first prost-processing step the nodes of type
 * @ref scorep_profile_node_thread_start are replaced by the callpath to their creation
 * point.
 *
 */

#include <config.h>
#include "SCOREP_Memory.h"
#include "scorep_utility/SCOREP_Utils.h"

#include "scorep_profile_definition.h"

/**
   Finds or creates a child of parent which matches type and adds dense metrics from
   source.
   @param parent Pointer to a node whose children are are searched for a node that
                 matches @a type.
   @param type   Pointer to a node which serves as pattern of the node to find.
   @param source Pointer to a node from which the dense metrics are added to the
                 found or created node.
 */
scorep_profile_node*
scorep_profile_merge_child( scorep_profile_node* parent,
                            scorep_profile_node* type,
                            scorep_profile_node* source )
{
    /* Search matching node */
    SCOREP_ASSERT( parent != NULL );
    scorep_profile_node* child = scorep_profile_find_create_child( parent, type,
                                                                   source->first_enter_time );
    /* Add statistics */
    scorep_profile_merge_node_dense( child, source );
    return child;
}

/**
   Adds the callpath to callpath_leaf to destination_root with the metrics from
   data_source.
   @param destination_root Pointer to a node to which the callpath is added.
   @param callpath_leaf Pointer to a node which represents the callpath which is added
          to @ref destination_root. The path from this node to its thread root or thread
          start is added.
   @param data_source Pointer to a node from which the dense metrics are added to the
          nodes on the newly created callpath.
 */
scorep_profile_node*
scorep_profile_add_callpath( scorep_profile_node* destination_root,
                             scorep_profile_node* callpath_leaf,
                             scorep_profile_node* data_source )
{
    scorep_profile_node* parent = callpath_leaf->parent;

    /* If parent is root node of the thread */
    if ( ( parent == NULL ) ||
         ( ( parent->node_type == scorep_profile_node_thread_root ) ||
           ( parent->node_type == scorep_profile_node_thread_start ) ) )
    {
        return scorep_profile_merge_child( destination_root, callpath_leaf, data_source );
    }

    /* Else reconstruct the new callpath */
    parent = scorep_profile_add_callpath( destination_root,
                                          callpath_leaf->parent,
                                          data_source );
    return scorep_profile_merge_child( parent, callpath_leaf, data_source );
}

/**
   Calculates the implicit metrics of parent from all its children.
   @param parent Pointer to a node which implcit (dense) metrics are calculated.
 */
void
scorep_profile_sum_children( scorep_profile_node* parent )
{
    scorep_profile_node* child = NULL;

    SCOREP_ASSERT( parent != NULL );

    child = parent->first_child;
    if ( child == NULL )
    {
        return;              /* Nothing to do */
    }
    /* Reinitialize the metrics with values from first child */
    scorep_profile_copy_all_dense_metrics( parent, child );
    parent->count = 0;

    /* Add the statistics of all children */
    while ( child->next_sibling != NULL )
    {
        child = child->next_sibling;
        scorep_profile_merge_node_dense( parent, child );
    }
}

/**
    Replaces a node of type @ref scorep_profile_node_thread_start with the callpath
    from the creation point.
    @param thread_start Pointer to the node which is replaced by the callpath of its
           creation point. The creation point should be stored in its type dependent
           data.
 */
void
scorep_profile_expand_thread_start( scorep_profile_node* thread_start )
{
    scorep_profile_node* creation_point = NULL;
    scorep_profile_node* thread_root    = NULL;
    scorep_profile_node* last_child     = NULL;

    /* Assertions and setting of relatives */
    SCOREP_ASSERT( thread_start != NULL );
    SCOREP_ASSERT( thread_start->node_type == scorep_profile_node_thread_start );
    thread_root = thread_start->parent;
    SCOREP_ASSERT( thread_root != NULL ); /* Thread activation without location creation */
    SCOREP_ASSERT( thread_root->node_type == scorep_profile_node_thread_root );

    creation_point = ( scorep_profile_node* )
                     SCOREP_PROFILE_DATA2POINTER( thread_start->type_specific_data );

    /* Separate the thread_start node from the profile */
    scorep_profile_remove_node( thread_start );

    /* If it has no children -> nothing to do */
    if ( thread_start->first_child == NULL )
    {
        return;
    }

    /* If no creation point is available, append the child list to the thread_root */
    if ( creation_point == NULL )
    {
        scorep_profile_move_children( thread_root, thread_start );
    }
    /* Else insert callpath */
    else
    {
        /* Sum up child statistics */
        scorep_profile_sum_children( thread_start );

        /* Add callpath */
        creation_point = scorep_profile_add_callpath( thread_root, creation_point,
                                                      thread_start );

        /* Move the subforest to the inserted callpath: */
        scorep_profile_move_children( creation_point, thread_start );
    }
}

/**
   Expand all thread start nodes of a thread_root.
   @param thread_root Pointer to a thread node whose children of type
          @ref scorep_profile_node_thread_start are expanded.
 */
void
scorep_profile_expand_thread_root( scorep_profile_node* thread_root )
{
    /* Expand the start nodes */
    scorep_profile_node* thread_start = thread_root->first_child;
    scorep_profile_node* next_node    = NULL;
    while ( thread_start != NULL )
    {
        /* Need to store the next sibling, because the current is removed
           -> next_sibling is set to NULL */
        next_node = thread_start->next_sibling;

        /* Expand thread_start node */
        if ( thread_start->node_type == scorep_profile_node_thread_start )
        {
            scorep_profile_expand_thread_start( thread_start );
        }

        /* Go to next node */
        thread_start = next_node;
    }

    /* Calculate thread statistics */
    scorep_profile_sum_children( thread_root );
}

/**
   Expands all threads. All nodes of type @ref scorep_profile_node_thread_start
   in the profile are substituted by the callpath to the node where the thread was
   activated. In OpenMP this is the callpath which contained the parallel region
   statement.
 */
void
scorep_profile_expand_threads()
{
    scorep_profile_node* thread_root = scorep_profile.first_root_node;
    while ( thread_root != NULL )
    {
        if ( thread_root->node_type == scorep_profile_node_thread_root )
        {
            scorep_profile_expand_thread_root( thread_root );
        }
        thread_root = thread_root->next_sibling;
    }
}
