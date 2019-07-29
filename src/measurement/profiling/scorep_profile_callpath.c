/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
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
 * Contains algorithms to assign callpaths to each node during post-processing.
 * Therefore we assume that the first root node of the forest is the master thread.
 * First all nodes in the master thread get an callpath ID. Afterwards all other threads
 * try to match their nodes to the master thread. If a matching node exist, the callpath
 * handle is copied. If no matching callpath exist, the master thread is extended by the
 * new nodes (with zero values for all metrics) and a new callpath is registered.
 * At the end, the master thread's calltree contains all callpaths that occur in the
 * whole profile. All other threads contain only those callpaths which contains leaves
 * which they really executed.
 *
 * While the fist step can happen before the thread start nodes are expanded, the second
 * step requires the thread start nodes to be expanded.
 *
 */

#include <config.h>
#include <SCOREP_Memory.h>
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include <scorep_profile_definition.h>
#include <SCOREP_Definitions.h>
#include <scorep_profile_location.h>
#include <scorep_profile_converter.h>

static bool
compare_first_enter_time( scorep_profile_node* node_a,
                          scorep_profile_node* node_b )
{
    return node_a->first_enter_time > node_b->first_enter_time;
}

static void
sort_tree( void )
{
    scorep_profile_node* root = scorep_profile.first_root_node;
    while ( root != NULL )
    {
        scorep_profile_sort_subtree( root, compare_first_enter_time );
        root = root->next_sibling;
    }
}

/**
   Assigns a callpath to a node, if it has none so far. It is used by a
   @ref scorep_profile_for_all traversal, thus it has to fit the
   @ref scorep_profile_process_func_t.
 */
static void
assign_callpath( scorep_profile_node* current, void* param )
{
    SCOREP_CallpathHandle parent_path = SCOREP_INVALID_CALLPATH;

    UTILS_ASSERT( current != NULL );

    if ( current->callpath_handle == SCOREP_INVALID_CALLPATH )
    {
        /* Get parent path. Check whether it is an extension or the first one */
        if ( current->parent == NULL )
        {
            parent_path = SCOREP_INVALID_CALLPATH;
        }
        else if ( current->parent->node_type == SCOREP_PROFILE_NODE_THREAD_ROOT )
        {
            parent_path = SCOREP_INVALID_CALLPATH;
        }
        else
        {
            parent_path = current->parent->callpath_handle;
        }

        /* register new callpath */
        switch ( current->node_type )
        {
            case SCOREP_PROFILE_NODE_TASK_ROOT:
            case SCOREP_PROFILE_NODE_REGULAR_REGION:
            {
                scorep_profile_callpath_parameters_t* parameters = scorep_profile_type_get_ptr_value( current->type_specific_data );
                current->callpath_handle =
                    SCOREP_Definitions_NewCallpath( parent_path,
                                                    scorep_profile_type_get_region_handle( current->type_specific_data ),
                                                    parameters ? parameters->number : 0,
                                                    parameters ? parameters->parameters : NULL );
            }
            break;
            case SCOREP_PROFILE_NODE_PARAMETER_STRING:
                UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                             "Creating string parameter callpath." );
                scorep_profile_on_error( NULL );
                break;
            case SCOREP_PROFILE_NODE_PARAMETER_INTEGER:
                UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                             "Creating integer parameter callpath." );
                scorep_profile_on_error( NULL );
                break;
            case SCOREP_PROFILE_NODE_THREAD_ROOT:
                /* Do not assign a callpath to the thread root node */
                break;
            case SCOREP_PROFILE_NODE_THREAD_START:
                UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                             "Try to assign a callpath to a thread activation node. "
                             "This means that this is not the master thread and the worker "
                             "threads were not expanded before." );
                scorep_profile_on_error( NULL );
                break;
            default:
                UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                             "Callpath assignment to node type %d not supported.",
                             current->node_type );
                scorep_profile_on_error( NULL );
        }
    }
}

/**
   Matches a node to one of the child nodes of a given master. If the master does not
   have a matching child, a matching child for the master is created.
   Recursively, it processes all children of @a current.
 */
static void
match_callpath( SCOREP_Profile_LocationData* location,
                scorep_profile_node*         master,
                scorep_profile_node*         current )
{
    scorep_profile_node* child = NULL;

    /* Find a matching node in the master thread */
    scorep_profile_node* match =
        scorep_profile_find_create_child( location,
                                          master,
                                          current->node_type,
                                          current->type_specific_data,
                                          current->first_enter_time );

    /* Make sure the matching node has a callpath assigned */
    if ( match->callpath_handle == SCOREP_INVALID_CALLPATH )
    {
        assign_callpath( match, NULL );
    }

    /* Copy callpath handle */
    current->callpath_handle = match->callpath_handle;

    /* Process children */
    child = current->first_child;
    while ( child != NULL )
    {
        match_callpath( location, match, child );
        child = child->next_sibling;
    }
}

/**
   Walks through the master thread and assigns new callpath ids.
 */
void
scorep_profile_assign_callpath_to_master( void )
{
    scorep_profile_node* master = scorep_profile.first_root_node;

    sort_tree();

    /* Check consistency */
    if ( master == NULL )
    {
        UTILS_WARNING( "Try to assign callpathes to empty callpath." );
        return;
    }
    UTILS_ASSERT( master->node_type == SCOREP_PROFILE_NODE_THREAD_ROOT );
    if ( master->first_child == NULL )
    {
        UTILS_WARNING( "Master thread contains no regions." );
        return;
    }

    /* Process subtree */
    scorep_profile_for_all( master, assign_callpath, NULL );
}

/**
   Traverses all threads and matches their callpaths to the master thread.
 */
void
scorep_profile_assign_callpath_to_workers( void )
{
    scorep_profile_node*         master   = scorep_profile.first_root_node;
    scorep_profile_node*         thread   = NULL;
    scorep_profile_node*         child    = NULL;
    SCOREP_Profile_LocationData* location = NULL;

    if ( master == NULL )
    {
        return;
    }
    location = scorep_profile_type_get_location_data( master->type_specific_data );
    thread   = master->next_sibling;

    /* For each worker thread ... */
    while ( thread != NULL )
    {
        child = thread->first_child;

        /* For each subtree in the worker thread ... */
        while ( child != NULL )
        {
            /* match callpath */
            match_callpath( location, master, child );

            child = child->next_sibling;
        }

        thread = thread->next_sibling;
    }
}
