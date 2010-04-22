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
 * @file       SILC_Compiler_Data.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status     ALPHA
 *
 * @brief Implementation of helper functions which are common for all compiler
 *        adapters.
 */


#include "SILC_Definitions.h"
#include "SILC_DefinitionLocking.h"
#include "SILC_Compiler_Data.h"
#include "SILC_Hashtab.h"

/**
   A hash table which stores information about regions under their name as
   key. Mainly used to obtain the region handle from the region name.
 */
silc_compiler_hash_node* region_hash_table[ SILC_COMPILER_REGION_SLOTS ];

/**
    Hash table for mapping source file names to SILC file handles.
 */
SILC_Hashtab* silc_compiler_file_table = NULL;

/* ***************************************************************************************
   File hash table functions
*****************************************************************************************/

/**
   Deletes one file table entry.#
   @param entry Pointer to the entry to be deleted.
 */
void
silc_compiler_delete_file_entry( SILC_Hashtab_Entry* entry )
{
    SILC_ASSERT( entry );

    free( ( SILC_SourceFileHandle* )entry->value );
    free( ( char* )entry->key );
}

/* Initialize the file table */
void
silc_compiler_init_file_table()
{
    silc_compiler_file_table = SILC_Hashtab_CreateSize( 10, &SILC_Hashtab_HashString,
                                                        &SILC_Hashtab_CompareStrings );
}

/* Finalize the file table */
void
silc_compiler_final_file_table()
{
    SILC_Hashtab_Foreach( silc_compiler_file_table, &silc_compiler_delete_file_entry );
    SILC_Hashtab_Free( silc_compiler_file_table );
    silc_compiler_file_table = NULL;
}

/* Returns the file handle for a given file name. */
SILC_SourceFileHandle
silc_compiler_get_file( const char* file )
{
    size_t              index;
    SILC_LockSourceFileDefinition();
    SILC_Hashtab_Entry* entry = NULL;

    if ( file == NULL )
    {
        return SILC_INVALID_SOURCE_FILE;
    }

    entry = SILC_Hashtab_Find( silc_compiler_file_table, file,
                               &index );

    /* If not found register new file */
    if ( !entry )
    {
        /* Reserve own storage for file name */
        char* file_name = ( char* )malloc( ( strlen( file ) + 1 ) * sizeof( char ) );
        strcpy( file_name, file );

        /* Register file to measurement system */
        SILC_SourceFileHandle* handle = malloc( sizeof( SILC_SourceFileHandle ) );
        *handle = SILC_DefineSourceFile( file_name );

        /* Store handle in hashtable */
        SILC_Hashtab_Insert( silc_compiler_file_table, ( void* )file_name,
                             handle, &index );

        SILC_UnlockSourceFileDefinition();
        return *handle;
    }

    SILC_UnlockSourceFileDefinition();
    return *( SILC_SourceFileHandle* )entry->value;
}



/* ***************************************************************************************
   Region hash table functions
*****************************************************************************************/

/* Initialize slots of compiler hash table. */
void
silc_compiler_hash_init()
{
    int i;

    silc_compiler_init_file_table();

    for ( i = 0; i < SILC_COMPILER_REGION_SLOTS; i++ )
    {
        region_hash_table[ i ] = NULL;
    }
}

/* Get hash table entry for given ID. */
silc_compiler_hash_node*
silc_compiler_hash_get( long key )
{
    long hash_code = key % SILC_COMPILER_REGION_SLOTS;

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " hash code %ld: \n", hash_code );

    silc_compiler_hash_node* curr = region_hash_table[ hash_code ];
    while ( curr )
    {
        if ( curr->key == key )
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}


/* Stores function name under hash code */
silc_compiler_hash_node*
silc_compiler_hash_put
(
    long        key,
    const char* region_name,
    const char* file_name,
    int         line_no_begin
)
{
    long                     hash_code = key % SILC_COMPILER_REGION_SLOTS;
    silc_compiler_hash_node* add       = ( silc_compiler_hash_node* )
                                         malloc( sizeof( silc_compiler_hash_node ) );
    add->key                       = key;
    add->region_name               = region_name ? strdup( region_name ) : NULL;
    add->file_name                 = file_name ? strdup( file_name ) : NULL;
    add->line_no_begin             = line_no_begin;
    add->line_no_end               = SILC_INVALID_LINE_NO;
    add->region_handle             = SILC_INVALID_REGION;
    add->next                      = region_hash_table[ hash_code ];
    region_hash_table[ hash_code ] = add;
    return add;
}


/* Free elements of compiler hash table. */
void
silc_compiler_hash_free()
{
    silc_compiler_hash_node* next;
    silc_compiler_hash_node* cur;
    int                      i;
    for ( i = 0; i < SILC_COMPILER_REGION_SLOTS; i++ )
    {
        if ( region_hash_table[ i ] )
        {
            cur = region_hash_table[ i ];
            while ( cur != NULL )
            {
                next = cur->next;
                if ( cur->region_name != NULL )
                {
                    free( cur->region_name );
                }
                if ( cur->file_name != NULL )
                {
                    free( cur->file_name );
                }
                free( cur );
                cur = next;
            }
            region_hash_table[ i ] = NULL;
        }
    }

    silc_compiler_final_file_table();
}

/* Register a new region to the measuremnt system */
void
silc_compiler_register_region
(
    silc_compiler_hash_node* node
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "register a region! " );


    SILC_LockRegionDefinition();

    node->region_handle = SILC_DefineRegion( node->region_name,
                                             silc_compiler_get_file( node->file_name ),
                                             node->line_no_begin,
                                             node->line_no_end,
                                             SILC_ADAPTER_COMPILER,
                                             SILC_REGION_FUNCTION
                                             );
    SILC_UnlockRegionDefinition();
}
