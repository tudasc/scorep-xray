/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universität Darmstadt, Darmstadt, Germany
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
 * that clusters threads with the same calltree structure within a process
 * in a postprocessing step.
 */

#include <config.h>
#include <scorep_profile_node.h>
#include <scorep_profile_definition.h>

#include <SCOREP_Definitions.h>
#include <UTILS_Error.h>

/* **************************************************************************************
 * static functions
 ***************************************************************************************/
static bool
compare_structure( scorep_profile_node* rootA, scorep_profile_node* rootB )
{
    if ( rootA == NULL && rootB == NULL )
    {
        return true;
    }
    if ( rootA == NULL || rootB == NULL )
    {
        return false;
    }

    if ( rootA->node_type != SCOREP_PROFILE_NODE_THREAD_ROOT )
    {
        /* Use visit count as indicator whether with callpath was visited by both. */
        if ( !scorep_profile_compare_nodes( rootA, rootB ) ||
             ( rootA->count > 0 && rootB->count == 0 ) ||
             ( rootA->count == 0 && rootB->count > 0 ) )
        {
            return false;
        }
    }

    scorep_profile_node* child_a = rootA->first_child;
    scorep_profile_node* child_b = rootB->first_child;
    for (;
         child_a != NULL && child_b != NULL;
         child_a = child_a->next_sibling,
         child_b = child_b->next_sibling )
    {
        if ( !compare_structure( child_a, child_b ) )
        {
            return false;
        }
    }
    return true;
}


static void
merge_locations( scorep_profile_node* destination,
                 scorep_profile_node* source )
{
    UTILS_ASSERT( destination );
    UTILS_ASSERT( source );
    SCOREP_Profile_LocationData* location = scorep_profile_get_location_of_node( destination );
    UTILS_ASSERT( location );

    scorep_profile_merge_subtree( location, destination, source );
}

/* **************************************************************************************
 * interface functions
 ***************************************************************************************/

void
scorep_profile_cluster_same_location( void )
{
    for ( scorep_profile_node* root = scorep_profile.first_root_node;
          root != NULL; )
    {
        scorep_profile_node* next = root->next_sibling;

        /* Make sure everything is sorted. This is required for comparison */
        scorep_profile_sort_subtree( root, &scorep_profile_compare_nodes );

        for ( scorep_profile_node* cluster = scorep_profile.first_root_node;
              cluster != root;
              cluster = cluster->next_sibling )
        {
            if ( compare_structure( cluster, root ) )
            {
                scorep_profile_remove_node( root );
                merge_locations( cluster, root );
                break;
            }
        }

        root = next;
    }
}
