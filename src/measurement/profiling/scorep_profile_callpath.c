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
 * @file scorep_profile_callpath.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
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
#include <scorep_utility/SCOREP_Debug.h>
#include <scorep_utility/SCOREP_Error.h>

#include <scorep_profile_definition.h>
#include <scorep_definitions.h>
#include <scorep_profile_location.h>

/**
   Sorts the children of a node according to thair first entry.
 */
static void
sort_children( scorep_profile_node* parent )
{
    scorep_profile_node* child      = parent->first_child;
    scorep_profile_node* last_child = NULL;
    while ( child != NULL )
    {
        scorep_profile_node*  current      = parent->first_child;
        scorep_profile_node** last_pointer = &parent->first_child;
        bool                  update       = true;
        while ( current != child )
        {
            if ( current->first_enter_time > child->first_enter_time )
            {
                scorep_profile_node* next_child = child->next_sibling;
                *last_pointer            = child;
                child->next_sibling      = current;
                last_child->next_sibling = next_child;
                child                    = next_child;
                update                   = false;
                break;
            }
            last_pointer = &current->next_sibling;
            current      = current->next_sibling;
        }
        if ( update )
        {
            last_child = child;
            child      = child->next_sibling;
        }
    }
}

static void
sort_subtree( scorep_profile_node* parent )
{
    sort_children( parent );
    scorep_profile_node* child = parent->first_child;
    while ( child != NULL )
    {
        sort_subtree( child );
        child = child->next_sibling;
    }
}

static void
sort_tree()
{
    scorep_profile_node* root = scorep_profile.first_root_node;
    while ( root != NULL )
    {
        sort_subtree( root );
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

    SCOREP_ASSERT( current != NULL );

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
                    SCOREP_DefineCallpath( parent_path,
                                           scorep_profile_type_get_region_handle( current->type_specific_data ) );
                break;
            case scorep_profile_node_parameter_string:
                current->callpath_handle = SCOREP_DefineCallpathParameterString(
                    parent_path,
                    scorep_profile_type_get_parameter_handle( current->type_specific_data ),
                    scorep_profile_type_get_string_handle( current->type_specific_data ) );
                break;
            case scorep_profile_node_parameter_integer:
                current->callpath_handle = SCOREP_DefineCallpathParameterInteger(
                    parent_path,
                    scorep_profile_type_get_parameter_handle( current->type_specific_data ),
                    scorep_profile_type_get_int_value( current->type_specific_data ) );
                break;
            case scorep_profile_node_thread_root:
                /* Do no assign a callpath to the thread root node */
                break;
            case scorep_profile_node_thread_start:
                SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                              "Try to assign a callpath to a thread activation node. "
                              "This means that this is not the master thread and the worker "
                              "threads were not expanded before." );
                break;
            default:
                SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                              "Callpath assignedment to node type %d not supported.",
                              current->node_type );
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
scorep_profile_assign_callpath_to_master()
{
    scorep_profile_node* master = scorep_profile.first_root_node;

    sort_tree();

    /* Check consistency */
    if ( master == NULL )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_WARNING | SCOREP_DEBUG_PROFILE,
                             "Try to assign callpathes to empty callpath." );
        return;
    }
    SCOREP_ASSERT( master->node_type == scorep_profile_node_thread_root );
    if ( master->first_child == NULL )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_WARNING | SCOREP_DEBUG_PROFILE,
                             "Master thread contains no regions." );
        return;
    }

    /* Process subtree */
    scorep_profile_for_all( master, assign_callpath, NULL );
}

/**
   Traverses all threads and matches their callpathes to the master thread.
 */
void
scorep_profile_assign_callpath_to_workers()
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
