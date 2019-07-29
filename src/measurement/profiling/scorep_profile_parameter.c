/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * Copyright (c) 2016-2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * Contains a post-processing algorithm to substitute nested parameter nodes
 * by a region that contains the region name and all parameters.
 */

#include <config.h>
#include <scorep_profile_converter.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

static void
traverse_rec( scorep_profile_node* node );

/**
 * Checks whether @a node has children that are parameter nodes.
 */
static bool
has_parameter_children( scorep_profile_node* node )
{
    for ( scorep_profile_node* child = node->first_child;
          child != NULL;
          child = child->next_sibling )
    {
        switch ( child->node_type )
        {
            case SCOREP_PROFILE_NODE_PARAMETER_INTEGER:
            case SCOREP_PROFILE_NODE_PARAMETER_STRING:
                return true;

            default:
                /* continue with next child */
                break;
        }
    }
    return false;
}

/**
 * Creates the parameter list recursively directing the root of the region.
 * @param location  The location for memory allocation
 * @param node      A node on the path from the target node to the root
 *                  of the parameter-subtree.
 * @returns the parameter list.
 */
static scorep_profile_callpath_parameters_t*
create_parameter_list( SCOREP_Location* location, scorep_profile_node* node, uint32_t num_params )
{
    scorep_profile_callpath_parameters_t* parameters = 0;
    switch ( node->node_type )
    {
        case SCOREP_PROFILE_NODE_REGULAR_REGION:
            /* We reached the region for which we are creating the parameters, allocate and return the list */
            parameters         = SCOREP_Location_AllocForProfile( location, num_params * sizeof( scorep_definitions_callpath_parameter ) + sizeof( scorep_profile_callpath_parameters_t ) );
            parameters->number = 0;
            break;

        case SCOREP_PROFILE_NODE_PARAMETER_INTEGER:
        case SCOREP_PROFILE_NODE_PARAMETER_STRING:
            /* Still on the way up to reach the region, recursive call on parent */
            parameters = create_parameter_list( location, node->parent, num_params + 1 );
            /* parameters are stored in order from top to bottom */
            parameters->parameters[ parameters->number ].parameter_handle              = scorep_profile_type_get_parameter_handle( node->type_specific_data );
            parameters->parameters[ parameters->number ].parameter_value.integer_value = scorep_profile_type_get_int_value( node->type_specific_data );
            parameters->number++;
            break;

        default:
            /* nothing to do */
            break;
    }
    return parameters;
}

/**
 * Traverses the complete call-tree and merges all parameter nodes as a parameter
 * array in the region call node.
 * @param location    Pointer to the location to allocate memory
 * @param current     Pointer to the current node that is either a
 *                    parameter node or a region with parameters
 * @param destination The parent of the region that contains the parameters
 *                    The new nodes will become children of @a destination.
 */
static void
create_parameters_rec( SCOREP_Location*     location,
                       scorep_profile_node* current,
                       scorep_profile_node* destination,
                       SCOREP_RegionHandle  regionHandle )
{
    scorep_profile_node* next;
    for ( scorep_profile_node* child = current->first_child;
          child != NULL;
          child = next )
    {
        /* Because we might remove current, store the next pointer first */
        next = child->next_sibling;

        switch ( child->node_type )
        {
            case SCOREP_PROFILE_NODE_PARAMETER_INTEGER:
            case SCOREP_PROFILE_NODE_PARAMETER_STRING:
                scorep_profile_subtract_node( current, child );
                create_parameters_rec( location, child, destination, regionHandle );
                break;
            default:
                /* nothing to do */
                break;
        }
    }

    if ( current->count > 0 )
    {
        scorep_profile_callpath_parameters_t* parameters = create_parameter_list( location, current, 0 );
        scorep_profile_remove_node( current );
        current->node_type = SCOREP_PROFILE_NODE_REGULAR_REGION;
        memset( &current->type_specific_data, 0, sizeof( current->type_specific_data ) );
        scorep_profile_type_set_region_handle( &current->type_specific_data, regionHandle );
        scorep_profile_type_set_ptr_value( &current->type_specific_data, parameters );
        scorep_profile_add_child( destination, current );

        traverse_rec( current );
    }
    else
    {
        scorep_profile_remove_node( current );
    }
}

/**
   Checks whether a node has parameters and replaces the node with parameters by
   a list of nodes for region that contain the parameter.
   @param node  The node which get changed.
 */
static void
traverse_rec( scorep_profile_node* node )
{
    for ( scorep_profile_node* next = node;
          next != NULL;
          node = next )
    {
        /* node might be removed, thus, store next element first */
        next = node->next_sibling;
        if ( has_parameter_children( node ) )
        {
            SCOREP_RegionHandle  region = scorep_profile_type_get_region_handle( node->type_specific_data );
            scorep_profile_node* parent = node->parent;
            scorep_profile_remove_node( node );
            SCOREP_Location* location = scorep_profile_get_location_of_node( parent )->location_data;
            create_parameters_rec( location, node, parent, region );
        }
        else
        {
            traverse_rec( node->first_child );
        }
    }
}

/**
   Changes the dynamic region instance parameter node into a regular region node
   and reduces the parent's visit count accordingly.
   Its a processing function handled to @ref scorep_profile_for_all. If @node is
   not a parameter node, nothing happens. The name of the region change from
   'instance' to 'instance=<value>'.

   @param node  The node which get changed.
   @param param unused.
 */
static void
substitute_dynamic_instances_in_node( scorep_profile_node* node,
                                      void*                param )
{
    SCOREP_RegionHandle handle = scorep_profile_type_get_region_handle( node->type_specific_data );

    /* process 'instance' parameter nodes only */
    if ( node->node_type == SCOREP_PROFILE_NODE_PARAMETER_INTEGER
         && scorep_profile_param_instance == scorep_profile_type_get_parameter_handle( node->type_specific_data ) )
    {
        /* DYNAMIC regions cannot have parameters for now */
        UTILS_BUG_ON( has_parameter_children( node ), "DYNAMIC instance parameter with more parameters" );

        const char* name = SCOREP_ParameterHandle_GetName( handle );

        char region_name[ strlen( name ) + 23 ]; /* why 23? strlen("instance=") == 9 */
        sprintf( region_name, "%s=%" PRIi64, name,
                 scorep_profile_type_get_int_value( node->type_specific_data ) );

        /* Reduce exclusive visit count of parent by own visit count (should be always one).
           This way, a root of only instance regions will show an exclusive count of 0. */
        scorep_profile_node* parent = node->parent;
        UTILS_BUG_ON( parent->node_type != SCOREP_PROFILE_NODE_REGULAR_REGION );
        parent->count -= node->count;

        /* Register new region handle and modify node data */
        handle = SCOREP_Definitions_NewRegion( region_name,
                                               NULL,
                                               SCOREP_INVALID_SOURCE_FILE,
                                               SCOREP_INVALID_LINE_NO,
                                               SCOREP_INVALID_LINE_NO,
                                               SCOREP_PARADIGM_USER,
                                               SCOREP_REGION_UNKNOWN );
        node->node_type = SCOREP_PROFILE_NODE_REGULAR_REGION;
        memset( &node->type_specific_data, 0, sizeof( node->type_specific_data ) );
        scorep_profile_type_set_region_handle( &node->type_specific_data, handle );
    }
}

/* ***************************************************************************************
   Main algorithm function
*****************************************************************************************/

void
scorep_profile_process_parameters( void )
{
    /*
     * We first special handle the DYNAMIC regions instance parameter, so that
     * the clustering still works, this is the old parameter handling, by changing
     * the call node type with a new region
     */
    scorep_profile_node* node = scorep_profile.first_root_node;
    while ( node != NULL )
    {
        scorep_profile_for_all( node, substitute_dynamic_instances_in_node, NULL );
        node = node->next_sibling;
    }

    /*
     * Special traversal, because we need to be able to cut subtrees and we will
     * replace nodes
     */
    traverse_rec( scorep_profile.first_root_node );
}
