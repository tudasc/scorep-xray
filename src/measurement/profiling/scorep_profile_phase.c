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
 * @file scorep_profile_phase.c
 * Implementation for postprocessing of phases.
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <stddef.h>

#include <SCOREP_Types.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Memory.h>
#include <scorep_utility/SCOREP_Utils.h>

#include <scorep_profile_node.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>


/**
   Removes a phase from its current position and insert it at the thread root level
 */
static void
scorep_profile_setup_phase( SCOREP_Profile_LocationData* location,
                            scorep_profile_node*         thread_root,
                            scorep_profile_node*         phase )
{
    assert( thread_root );
    assert( phase );

    scorep_profile_node* match = scorep_profile_find_child( thread_root, phase );

    /* Replace phase root node and leave original phase node as remainder. */
    scorep_profile_node* copy = scorep_profile_copy_node( location, phase );
    scorep_profile_move_children( copy, phase );

    /* If this top-level phase was not yet created */
    if ( match == NULL )
    {
        /* Move subtree to top level */
        scorep_profile_add_child( thread_root, copy );
    }
    /* We found this phase at the top-level  */
    else
    {
        /* Update phase at top level */
        scorep_profile_merge_subtree( location, match, copy );
    }
}

/**
   Returns true if a node represents a region that is a phase
 */
static bool
scorep_profile_node_is_phase( scorep_profile_node* node )
{
    if ( node->node_type != scorep_profile_node_regular_region )
    {
        return false;
    }

    SCOREP_RegionHandle region = scorep_profile_type_get_region_handle( node->type_specific_data );
    SCOREP_RegionType   type   = SCOREP_Region_GetType( region );
    if ( ( type == SCOREP_REGION_PHASE ) ||
         ( type == SCOREP_REGION_DYNAMIC_PHASE ) ||
         ( type == SCOREP_REGION_DYNAMIC_LOOP_PHASE ) )
    {
        return true;
    }
    return false;
}

/**
   Searches the subtree of a node, whether there are phases. If phases are found
   they are moved to the thread_root.
 */
static void
scorep_profile_search_subtree_for_phases( SCOREP_Profile_LocationData* location,
                                          scorep_profile_node*         thread_root,
                                          scorep_profile_node*         subtree_root )
{
    assert( subtree_root );
    assert( thread_root );

    scorep_profile_node* child = subtree_root->first_child;
    scorep_profile_node* next  = NULL;
    while ( child != NULL )
    {
        /* If child is a phase it will be cut out. Thus, we need to store the
           next sibling before we deal with the child */
        next = child->next_sibling;

        /* We need to remove all phases that are in the child's subtree, before we can
           deal with the child, else nested phases cause errors. */
        scorep_profile_search_subtree_for_phases( location, thread_root, child );

        /* Here we know that no phases are left in the subtree of child */
        if ( scorep_profile_node_is_phase( child ) )
        {
            scorep_profile_setup_phase( location, thread_root, child );
        }

        child = next;
    }
}

/* Post-processing for phases */
void
scorep_profile_process_phases()
{
    scorep_profile_node*         thread_root = scorep_profile.first_root_node;
    scorep_profile_node*         child       = NULL;
    SCOREP_Profile_LocationData* location;

    while ( thread_root != NULL )
    {
        /* Phases that are alread at the root level need no processing
           Thus, process only children of them. Hereby, we assume that
           theads are already expanded */

        child = thread_root->first_child;
        while ( child != NULL )
        {
            location = scorep_profile_type_get_location_data( thread_root->type_specific_data );
            scorep_profile_search_subtree_for_phases( location, thread_root, child );
            child = child->next_sibling;
        }
        thread_root = thread_root->next_sibling;
    }
}
