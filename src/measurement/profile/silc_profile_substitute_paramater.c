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
 * @file silc_profile_callpath.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status ALPHA
 *
 * Contains a post-processing algorithm to substitute parameter nodes by
 * regular region nodes which have a regions name like '<parameter name>=<value>'
 * This allows to write valid profiles for formats which do not support
 * parameter based profiles.
 */

#include "SILC_Memory.h"
#include "SILC_Utils.h"
#include "SILC_Definitions.h"
#include "SILC_Types.h"

#include "silc_profile_definition.h"
#include "silc_definitions.h"

/**
    Hash table for mapping already registered region names region handles.
 */
static SILC_Hashtab* silc_profile_name_table = NULL;

/* ***************************************************************************************
   Hash table functions
*****************************************************************************************/
/**
   Deletes one hash table entry
   @param entry Pointer to the entry to be deleted.
 */
void
silc_profile_delete_name_table_entry( SILC_Hashtab_Entry* entry )
{
    SILC_ASSERT( entry );

    free( ( SILC_RegionHandle* )entry->value );
    free( ( char* )entry->key );
}

/**
   Initialize the name table
 */
void
silc_compiler_init_name_table()
{
    silc_profile_name_table = SILC_Hashtab_CreateSize( 10, &SILC_Hashtab_HashString,
                                                       &SILC_Hashtab_CompareStrings );
}

/**
   Finalize the file table
 */
void
silc_compiler_final_file_table()
{
    SILC_Hashtab_Foreach( silc_profile_name_table, &silc_profile_delete_name_entry );
    SILC_Hashtab_Free( silc_profile_name_table );
    silc_compiler_file_table = NULL;
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
silc_profile_substitute_parameter_data( silc_profile_node* node,
                                        char*              region )
{
    size_t              index;
    SILC_Hashtab_Entry* entry  = NULL;
    SILC_Region_Handle* handle = NULL;

    /* check whether a region of this name is already registered */
    entry = SILC_Hashtab_Find( silc_profile_name_table, region,
                               &index );

    /* If not found, register new name */
    if ( !entry )
    {
        /* Reserve own storage for region name */
        char* region_name = ( char* )malloc( ( strlen( region ) + 1 ) * sizeof( char ) );
        strcpy( region_name, region );

        /* Register region to measurement system */
        handle = malloc( sizeof( SILC_RegionHandle ) );
        SILC_LockRegionDefinition();
        *handle = SILC_DefineRegion( region_name,
                                     SILC_INVALID_SOURCE_FILE,
                                     SILC_INVALID_LINE_NO,
                                     SILC_INVALID_LINE_NO,
                                     SILC_ADAPTER_USER,
                                     SILC_REGION_UNKNOWN );
        SILC_UnlockRegionDefinition();

        /* Store handle in hashtable */
        SILC_Hashtab_Insert( silc_profile_name_table, ( void* )region_name,
                             handle, &index );
    }
    else
    {
        handle = ( SILC_RegionHandle* )entry->value;
    }

    /* Modify node data */
    node->node_type          = silc_profile_node_regular_region;
    node->type_specific_data = SILC_PROFILE_REGION2DATA( *handle );
}

/**
   Changes a parameter node into a regular region node. Its a processing function
   handed to @ref silc_profile_for_all. If @node is not a parameter node, nothing
   happens. The name of the region will be '<parameter name>=<value>'
   @param node  The node which get changed.
   @param param unused.
 */
static void
silc_profile_substitute_paramter_in_node( silc_profile_node* node,
                                          void*              param )
{
    /* process integer parameter nodes */
    if ( node->node_type == silc_profile_node_paramter_integer )
    {
        silc_profile_integer_node_data* data = SILC_PROFILE_DATA2PARAMINT( node->type_specific_data );
        char*                           name = SILC_Parameter_GetName( data->handle );

        /* Use malloc, because its in post-processing => not time critical
           and its immediately freed => saves memory */
        char* region_name = ( char* )malloc( strlen( name ) + 18 );

        /* construct region name */
        sprintf( region_name, "%s=%d", name, data->value );

        /* Register region and modify node data */
        silc_profile_substitute_parameter_data( node, region_name );

        /* Clean up */
        free( region_name );
    }

    /* process string parameter nodes */
    else if ( node->node_type == silc_profile_node_paramter_string )
    {
        silc_profile_string_node_data* data
            = SILC_PROFILE_DATA2PARAMSTR( node->type_specific_data );
        char*                          name  = SILC_Parameter_GetName( data->handle );
        char*                          value = SILC_String_Get( data->value );

        /* Use malloc, because its in post-processing => not time critical
           and its immediately freed => saves memory */
        char* region_name = ( char* )malloc( strlen( name ) + strlen( value ) + 2 );

        /* construct region name */
        sprintf( region_name, "%s=%s", name, value );

        /* Register region and modify node data */
        silc_profile_substitute_parameter_data( node, region_name );

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
silc_profile_subsitute_parameter()
{
    silc_profile_node* node = silc_profile.first_root_node;

    silc_profile_initialize_name_table();

    while ( node != NULL )
    {
        silc_profile_for_all( node, silc_profile_substitute_parameter_in_node, NULL );
        node = node->next_sibling;
    }

    silc_profile_finalize_name_table();
}
