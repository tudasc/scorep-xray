/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 *
 * Contains a post-processing algorithm to substitute parameter nodes by
 * regular region nodes which have a regions name like '<parameter name>=<value>'
 * This allows to write valid profiles for formats which do not support
 * parameter based profiles.
 */

#include <config.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <UTILS_Error.h>
#include <SCOREP_Memory.h>
#include <UTILS_Debug.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Types.h>

#include <scorep_profile_definition.h>
#include <SCOREP_Definitions.h>
#include <scorep_profile_location.h>

/**
    Hash table for mapping already registered region names region handles.
 */
static SCOREP_Hashtab* scorep_profile_name_table = NULL;

/* ***************************************************************************************
   Hash table functions
*****************************************************************************************/
/**
   Deletes one hash table entry
   @param entry Pointer to the entry to be deleted.
 */
static void
delete_name_table_entry( SCOREP_Hashtab_Entry* entry )
{
    UTILS_ASSERT( entry );

    free( ( SCOREP_RegionHandle* )entry->value );
    free( ( char* )entry->key );
}

/**
   Initialize the name table
 */
static void
init_name_table( void )
{
    scorep_profile_name_table = SCOREP_Hashtab_CreateSize( 10, &SCOREP_Hashtab_HashString,
                                                           &SCOREP_Hashtab_CompareStrings );
}

/**
   Finalize the file table
 */
static void
finalize_name_table( void )
{
    SCOREP_Hashtab_Foreach( scorep_profile_name_table, &delete_name_table_entry );
    SCOREP_Hashtab_Free( scorep_profile_name_table );
    scorep_profile_name_table = NULL;
}


/* ***************************************************************************************
   static helper functions
*****************************************************************************************/

/**
    Checks whether a given region name was already used before. If not it registers a new
    region. In both cases modifies the profile node @a node in a way that it becomes a
    regular region node which represents a region with the name given in @a region.
    @param node   The node that is modified to become a regular region.
    @param region A string containing a region name.
 */
static void
substitute_parameter_data( scorep_profile_node* node,
                           char*                region )
{
    size_t                index;
    SCOREP_Hashtab_Entry* entry  = NULL;
    SCOREP_RegionHandle*  handle = NULL;

    /* check whether a region of this name is already registered */
    entry = SCOREP_Hashtab_Find( scorep_profile_name_table, region,
                                 &index );

    /* If not found, register new name */
    if ( !entry )
    {
        /* Reserve own storage for region name */
        char* region_name = ( char* )malloc( ( strlen( region ) + 1 ) * sizeof( char ) );
        strcpy( region_name, region );

        /* Register region to measurement system */
        handle  = malloc( sizeof( SCOREP_RegionHandle ) );
        *handle = SCOREP_Definitions_NewRegion( region_name,
                                                NULL,
                                                SCOREP_INVALID_SOURCE_FILE,
                                                SCOREP_INVALID_LINE_NO,
                                                SCOREP_INVALID_LINE_NO,
                                                SCOREP_PARADIGM_USER,
                                                SCOREP_REGION_UNKNOWN );

        /* Store handle in hashtable */
        SCOREP_Hashtab_Insert( scorep_profile_name_table, ( void* )region_name,
                               handle, &index );
    }
    else
    {
        handle = ( SCOREP_RegionHandle* )entry->value;
    }

    /* Modify node data */
    node->node_type = SCOREP_PROFILE_NODE_REGULAR_REGION;
    scorep_profile_type_set_region_handle( &node->type_specific_data, *handle );
}

/**
   Changes a parameter node into a regular region node. Its a processing function
   handed to @ref scorep_profile_for_all. If @node is not a parameter node, nothing
   happens. The name of the region will be '<parameter name>=<value>'
   @param node  The node which get changed.
   @param param unused.
 */
static void
substitute_parameter_in_node( scorep_profile_node* node,
                              void*                param )
{
    SCOREP_RegionHandle handle = scorep_profile_type_get_region_handle( node->type_specific_data );

    /* process integer parameter nodes */
    if ( node->node_type == SCOREP_PROFILE_NODE_PARAMETER_INTEGER )
    {
        const char* name = SCOREP_ParameterHandle_GetName( handle );

        SCOREP_ParameterType type = SCOREP_ParameterHandle_GetType( handle );


        /* Use malloc, because its in post-processing => not time critical
           and its immediately freed => saves memory */
        char* region_name = ( char* )malloc( strlen( name ) + 23 );

        /* construct region name */
        if ( type == SCOREP_PARAMETER_INT64 )
        {
            sprintf( region_name, "%s=%" PRIi64, name,
                     scorep_profile_type_get_int_value( node->type_specific_data ) );
        }
        else
        {
            sprintf( region_name, "%s=%" PRIu64, name,
                     scorep_profile_type_get_int_value( node->type_specific_data ) );
        }

        /* Register region and modify node data */
        substitute_parameter_data( node, region_name );

        /* Clean up */
        free( region_name );
    }

    /* process string parameter nodes */
    else if ( node->node_type == SCOREP_PROFILE_NODE_PARAMETER_STRING )
    {
        const char* name  = SCOREP_ParameterHandle_GetName( handle );
        const char* value = SCOREP_StringHandle_Get( scorep_profile_type_get_string_handle( node->type_specific_data ) );

        /* Use malloc, because its in post-processing => not time critical
           and its immediately freed => saves memory */
        char* region_name = ( char* )malloc( strlen( name ) + strlen( value ) + 2 );

        /* construct region name */
        sprintf( region_name, "%s=%s", name, value );

        /* Register region and modify node data */
        substitute_parameter_data( node, region_name );

        /* Clean up */
        free( region_name );
    }
}

/* ***************************************************************************************
   Main algorithm function
*****************************************************************************************/

/**
   Traverses the profile and changes a parameter nodes to regular regions, where the
   region has the name '<parameter name>=<value>'.
 */
void
scorep_profile_substitute_parameter( void )
{
    scorep_profile_node* node = scorep_profile.first_root_node;

    init_name_table();

    while ( node != NULL )
    {
        scorep_profile_for_all( node, substitute_parameter_in_node, NULL );
        node = node->next_sibling;
    }

    finalize_name_table();
}
