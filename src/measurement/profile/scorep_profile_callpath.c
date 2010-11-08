/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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
#include "SCOREP_Memory.h"
#include "scorep_utility/SCOREP_Utils.h"

#include "scorep_profile_definition.h"
#include "scorep_definitions.h"

/**
   Assignes a callpath to a node, if it has none so far. It is used by a
   @ref scorep_profile_for_all traversal, thus it has to fit the
   @ref scorep_profile_process_func_t.
 */
void
scorep_profile_assign_callpath( scorep_profile_node* current,
                                void*                param )
{
    SCOREP_CallpathHandle             parent_path = SCOREP_INVALID_CALLPATH;
    scorep_profile_string_node_data*  string_data = NULL;
    scorep_profile_integer_node_data* int_data    = NULL;

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
            case scorep_profile_node_regular_region:
                current->callpath_handle =
                    SCOREP_DefineCallpath( parent_path,
                                           SCOREP_PROFILE_DATA2REGION( current->type_specific_data ) );
                break;
            case scorep_profile_node_parameter_string:
                string_data = ( scorep_profile_string_node_data* )
                              SCOREP_PROFILE_DATA2POINTER( current->type_specific_data );
                current->callpath_handle =
                    SCOREP_DefineCallpathParameterString( parent_path, string_data->handle,
                                                          string_data->value );
                break;
            case scorep_profile_node_parameter_integer:
                int_data = ( scorep_profile_integer_node_data* )
                           SCOREP_PROFILE_DATA2POINTER( current->type_specific_data );
                current->callpath_handle =
                    SCOREP_DefineCallpathParameterInteger( parent_path, int_data->handle,
                                                           int_data->value );
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
   Walks through the master thread and assigns new callpath ids.
 */
void
scorep_profile_assign_callpath_to_master()
{
    scorep_profile_node* master = scorep_profile.first_root_node;

    /* Check consistency */
    if ( master == NULL )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_WARNING | SCOREP_DEBUG_PROFILE,
                             "Try to assign callpathes to empty callpath." );
        return;
    }
    SCOREP_ASSERT( master->node_type == scorep_profile_node_thread_root );
    if ( master->first_child == NULL )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_WARNING | SCOREP_DEBUG_PROFILE,
                             "Master thread contains no regions." );
        return;
    }

    /* Process subtree */
    scorep_profile_for_all( master, scorep_profile_assign_callpath, NULL );
}

/**
   Matches a node to one of the child nodes of a given master. If the master does not
   have a mathcing child, a matching child for the master is created.
   Recursively, it processes all children of @a current.
 */
void
scorep_profile_match_callpath( scorep_profile_node* master,
                               scorep_profile_node* current )
{
    scorep_profile_node* child = NULL;

    /* Find a matching node in the master thread */
    scorep_profile_node* match = scorep_profile_find_create_child( master, current,
                                                                   current->first_enter_time );

    /* Make sure the mathcing node has a callpath assigned */
    if ( match->callpath_handle == SCOREP_INVALID_CALLPATH )
    {
        scorep_profile_assign_callpath( match, NULL );
    }

    /* Copy callpath handle */
    current->callpath_handle = match->callpath_handle;

    /* Process children */
    child = current->first_child;
    while ( child != NULL )
    {
        scorep_profile_match_callpath( match, child );
        child = child->next_sibling;
    }
}

/**
   Traverses all threads and matches their callpathes to the master thread.
 */
void
scorep_profile_assign_callpath_to_workers()
{
    scorep_profile_node* master = scorep_profile.first_root_node;
    scorep_profile_node* thread = NULL;
    scorep_profile_node* child  = NULL;

    if ( master == NULL )
    {
        return;
    }
    thread = master->next_sibling;

    /* For each worker thread ... */
    while ( thread != NULL )
    {
        child = thread->first_child;

        /* For each subtree in the worker thread ... */
        while ( child != NULL )
        {
            /* match callpath */
            scorep_profile_match_callpath( master, child );

            child = child->next_sibling;
        }

        thread = thread->next_sibling;
    }
}
