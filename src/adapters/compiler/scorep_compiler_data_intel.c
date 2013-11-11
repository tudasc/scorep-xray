/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file
 *
 * @brief Implementation of helper functions for managing region data.
 */

#include <config.h>
#include "scorep_compiler_data_intel.h"

#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <SCOREP_Definitions.h>
#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_Filter.h>

/**
   @def SCOREP_COMPILER_HASH_MAX The number of slots in the region hash table.
 */
#define SCOREP_COMPILER_REGION_SLOTS 1021

/**
   @def SCOREP_COMPILER_FILE_SLOTS The number of slots in the file hash table.
 */
#define SCOREP_COMPILER_FILE_SLOTS 15

/**
 * @brief Hash table to map function addresses to region identifier
 * identifier is called region handle
 *
 * @param region_name       function name as it appears in the definitions
 * @param file_name         file name
 * @param region_handle     region identifier
 * @param next              pointer to next element with the same hash value.
 */
typedef struct scorep_compiler_hash_node
{
    char*                             region_name;
    char*                             file_name;
    SCOREP_RegionHandle               region_handle;
    struct scorep_compiler_hash_node* next;
} scorep_compiler_hash_node;

/**
   A hash table which stores information about regions under their name as
   key. Mainly used to obtain the region handle from the region name.
 */
scorep_compiler_hash_node* region_hash_table[ SCOREP_COMPILER_REGION_SLOTS ];

/**
   Hash table for mapping source file names to SCOREP file handles.
 */
SCOREP_Hashtab* scorep_compiler_file_table = NULL;

/* ***************************************************************************************
   File hash table functions
*****************************************************************************************/

/**
   Deletes one file table entry.
   @param entry Pointer to the entry to be deleted.
 */
static void
scorep_compiler_delete_file_entry( SCOREP_Hashtab_Entry* entry )
{
    UTILS_ASSERT( entry );

    free( ( SCOREP_SourceFileHandle* )entry->value );
    free( ( char* )entry->key );
}

/* Initialize the file table */
static void
scorep_compiler_init_file_table( void )
{
    scorep_compiler_file_table = SCOREP_Hashtab_CreateSize( SCOREP_COMPILER_FILE_SLOTS,
                                                            &SCOREP_Hashtab_HashString,
                                                            &SCOREP_Hashtab_CompareStrings );
}

/* Finalize the file table */
static void
scorep_compiler_final_file_table( void )
{
    SCOREP_Hashtab_Foreach( scorep_compiler_file_table, &scorep_compiler_delete_file_entry );
    SCOREP_Hashtab_Free( scorep_compiler_file_table );
    scorep_compiler_file_table = NULL;
}

/**
   Returns the file handle for a given file name. It searches in the hash table if the
   requested name is already there and returns the stored value. If the file name is not
   found in the hash table, it creates a new entry and registers the file to the SCOREP
   measurement system.
   @param file The file name for which the handle is returned.
   @returns the handle for the @a file.
 */
static SCOREP_SourceFileHandle
scorep_compiler_get_file( const char* file )
{
    size_t                index;
    SCOREP_Hashtab_Entry* entry = NULL;

    if ( file == NULL )
    {
        return SCOREP_INVALID_SOURCE_FILE;
    }

    entry = SCOREP_Hashtab_Find( scorep_compiler_file_table, file,
                                 &index );

    /* If not found, register new file */
    if ( !entry )
    {
        /* Reserve own storage for file name */
        char* file_name = ( char* )malloc( ( strlen( file ) + 1 ) * sizeof( char ) );
        strcpy( file_name, file );

        /* Register file to measurement system */
        SCOREP_SourceFileHandle* handle = malloc( sizeof( SCOREP_SourceFileHandle ) );
        if ( SCOREP_Filter_MatchFile( file_name ) )
        {
            *handle = SCOREP_INVALID_SOURCE_FILE;
        }
        else
        {
            *handle = SCOREP_Definitions_NewSourceFile( file_name );
        }

        /* Store handle in hashtable */
        SCOREP_Hashtab_Insert( scorep_compiler_file_table, ( void* )file_name,
                               handle, &index );

        return *handle;
    }

    return *( SCOREP_SourceFileHandle* )entry->value;
}



/* ***************************************************************************************
   Region hash table functions
*****************************************************************************************/

/* Initialize slots of compiler hash table. */
void
scorep_compiler_hash_init( void )
{
    uint64_t i;

    scorep_compiler_init_file_table();

    for ( i = 0; i < SCOREP_COMPILER_REGION_SLOTS; i++ )
    {
        region_hash_table[ i ] = NULL;
    }
}

/* Get hash table entry for given name. */
SCOREP_RegionHandle
scorep_compiler_hash_get( const char* region_name )
{
    uint64_t hash_code = SCOREP_Hashtab_HashString( region_name ) % SCOREP_COMPILER_REGION_SLOTS;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " hash code %ld", hash_code );

    scorep_compiler_hash_node* curr = region_hash_table[ hash_code ];
    /* The tail after curr will never change because, new elements are inserted before
       curr. Thus, it allows a parallel @ref scorep_compiler_hash_put which can only
       insert a new element before curr.
     */
    while ( curr )
    {
        if ( strcmp( curr->region_name, region_name )  == 0 )
        {
            return curr->region_handle;
        }
        curr = curr->next;
    }
    return SCOREP_INVALID_REGION;
}

/**
   Creates a new entry for the region hashtable with the given values.
   @param region_name     The name of the region.
   @param file_name       The name of the source file of the registered region.
   @param region_handle   The region handle.
   @returns a pointer to the newly created hash node.
 */
static void
scorep_compiler_hash_put( const char*         region_name,
                          const char*         file_name,
                          SCOREP_RegionHandle region_handle )
{
    uint64_t hash_code = SCOREP_Hashtab_HashString( region_name ) % SCOREP_COMPILER_REGION_SLOTS;

    scorep_compiler_hash_node* add = ( scorep_compiler_hash_node* )
                                     malloc( sizeof( scorep_compiler_hash_node ) );
    add->region_name   = UTILS_CStr_dup( region_name );
    add->file_name     = UTILS_CStr_dup( file_name );
    add->region_handle = region_handle;
    /* Inserting elements at the head allows parallel calls to
     * @ref scorep_compiler_hash_get
     */
    add->next                      = region_hash_table[ hash_code ];
    region_hash_table[ hash_code ] = add;
}


/* Free elements of compiler hash table. */
void
scorep_compiler_hash_free( void )
{
    scorep_compiler_hash_node* next;
    scorep_compiler_hash_node* cur;
    uint64_t                   i;
    for ( i = 0; i < SCOREP_COMPILER_REGION_SLOTS; i++ )
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

    scorep_compiler_final_file_table();
}

/* Register a new region to the measurement system */
SCOREP_RegionHandle
scorep_compiler_register_region( const char* str, const char* region_name )
{
    SCOREP_RegionHandle region_handle = SCOREP_FILTERED_REGION;

    /* Get file name */
    uint64_t len       = region_name - str;
    char*    file_name = malloc( len );
    assert( file_name );
    strncpy( file_name, str, len - 1 );
    file_name[ len - 1 ] = '\0';

    /* Get file handle */
    SCOREP_SourceFileHandle file_handle = scorep_compiler_get_file( file_name );

    /* Register file */
    if ( ( file_handle != SCOREP_INVALID_SOURCE_FILE ) &&
         ( strncmp( region_name, "POMP", 4 ) != 0 ) &&
         ( strncmp( region_name, "Pomp", 4 ) != 0 ) &&
         ( strncmp( region_name, "pomp", 4 ) != 0 ) &&
         ( !SCOREP_Filter_MatchFunction( region_name, NULL ) ) )
    {
        region_handle = SCOREP_Definitions_NewRegion( region_name,
                                                      NULL,
                                                      file_handle,
                                                      SCOREP_INVALID_LINE_NO,
                                                      SCOREP_INVALID_LINE_NO,
                                                      SCOREP_PARADIGM_COMPILER,
                                                      SCOREP_REGION_FUNCTION );
    }

    /* Add entry in hash table */
    scorep_compiler_hash_put( region_name, file_name, region_handle );

    free( file_name );
    return region_handle;
}
