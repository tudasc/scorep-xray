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
 * @file
 *
 *
 *
 * Contains algorithms to assign callpathes to each node during post-processing.
 * Therefore we assume that the first root node of the forest is the master thread.
 * First all nodes in the master thread get an callpath ID. Afterwards all other threads
 * try to match their nodes to the master thread. If a matching node exist, the callpath
 * handle is copied. If no matching callpath exist, the master thread is extended by the
 * new nodes (with zero values for all metrics) and a new callpath is registered.
 * At the end, the master thread's calltree contains all callpathes that occur in the
 * whole profile. All other threads contain only those callpathes which contains leaves
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
#include <definitions/SCOREP_Definitions.h>
#include <scorep_profile_location.h>

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
   Assignes a callpath to a node, if it has none so far. It is used by a
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
        /* Get parent path. Check wether it is an extention or the first one */
        if ( current->parent == NULL )
        {
            parent_path = SCOREP_INVALID_CALLPATH;
        }
        else if ( current->parent->node_type == scorep_profile_node_thread_root )
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
            case scorep_profile_node_task_root:
            case scorep_profile_node_regular_region:
                current->callpath_handle =
                    SCOREP_Definitions_NewCallpath( parent_path,
                                                    scorep_profile_type_get_region_handle( current->type_specific_data ) );
                break;
            case scorep_profile_node_parameter_string:
                current->callpath_handle = SCOREP_Definitions_NewCallpathParameterString(
                    parent_path,
                    scorep_profile_type_get_parameter_handle( current->type_specific_data ),
                    scorep_profile_type_get_string_handle( current->type_specific_data ) );
                break;
            case scorep_profile_node_parameter_integer:
                current->callpath_handle = SCOREP_Definitions_NewCallpathParameterInteger(
                    parent_path,
                    scorep_profile_type_get_parameter_handle( current->type_specific_data ),
                    scorep_profile_type_get_int_value( current->type_specific_data ) );
                break;
            case scorep_profile_node_thread_root:
                /* Do no assign a callpath to the thread root node */
                break;
            case scorep_profile_node_thread_start:
                UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                             "Try to assign a callpath to a thread activation node. "
                             "This means that this is not the master thread and the worker "
                             "threads were not expanded before." );
                scorep_profile_on_error( NULL );
                break;
            default:
                UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                             "Callpath assignedment to node type %d not supported.",
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
        scorep_profile_find_create_child( location,                                                                                 master,
                                          current->node_type,
                                          current->type_specific_data,
                                          current->first_enter_time );

    /* Make sure the mathcing node has a callpath assigned */
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
    UTILS_ASSERT( master->node_type == scorep_profile_node_thread_root );
    if ( master->first_child == NULL )
    {
        UTILS_WARNING( "Master thread contains no regions." );
        return;
    }

    /* Process subtree */
    scorep_profile_for_all( master, assign_callpath, NULL );
}

/**
   Traverses all threads and matches their callpathes to the master thread.
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
