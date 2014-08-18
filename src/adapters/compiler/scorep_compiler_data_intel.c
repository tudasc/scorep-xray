/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
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
    char*                             region_string;
    SCOREP_RegionHandle               region_handle;
    struct scorep_compiler_hash_node* next;
} scorep_compiler_hash_node;

/**
   A hash table which stores information about regions under their name as
   key. Mainly used to obtain the region handle from the region name.
 */
scorep_compiler_hash_node* region_hash_table[ SCOREP_COMPILER_REGION_SLOTS ];

/* ***************************************************************************************
   Region hash table functions
*****************************************************************************************/

/* Initialize slots of compiler hash table. */
void
scorep_compiler_hash_init( void )
{
    uint64_t i;

    for ( i = 0; i < SCOREP_COMPILER_REGION_SLOTS; i++ )
    {
        region_hash_table[ i ] = NULL;
    }
}

/* Get hash table entry for given name. */
SCOREP_RegionHandle
scorep_compiler_hash_get( const char* region_string )
{
    uint64_t hash_code = SCOREP_Hashtab_HashString( region_string ) % SCOREP_COMPILER_REGION_SLOTS;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " hash code %ld", hash_code );

    scorep_compiler_hash_node* curr = region_hash_table[ hash_code ];
    /* The tail after curr will never change because, new elements are inserted before
       curr. Thus, it allows a parallel @ref scorep_compiler_hash_put which can only
       insert a new element before curr.
     */
    while ( curr )
    {
        if ( strcmp( curr->region_string, region_string )  == 0 )
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
scorep_compiler_hash_put( const char*         region_string,
                          SCOREP_RegionHandle region_handle )
{
    uint64_t hash_code = SCOREP_Hashtab_HashString( region_string ) % SCOREP_COMPILER_REGION_SLOTS;

    scorep_compiler_hash_node* add = ( scorep_compiler_hash_node* )
                                     malloc( sizeof( scorep_compiler_hash_node ) );
    add->region_string = UTILS_CStr_dup( region_string );
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
                free( cur->region_string );
                free( cur );
                cur = next;
            }
            region_hash_table[ i ] = NULL;
        }
    }
}

static const char*
get_name_from_string( const char* str )
{
    while ( *str != '\0' )
    {
        if ( *str == ':' )
        {
            str++;
            break;
        }
        str++;
    }
    return str;
}

/* Register a new region to the measurement system */
SCOREP_RegionHandle
scorep_compiler_register_region( const char* str )
{
    SCOREP_RegionHandle region_handle = SCOREP_FILTERED_REGION;
    const char*         region_name   = get_name_from_string( str );

    /* Get file name */
    uint64_t len       = region_name - str;
    char*    file_name = malloc( len );
    assert( file_name );
    strncpy( file_name, str, len - 1 );
    file_name[ len - 1 ] = '\0';

    /* Get file handle */
    SCOREP_SourceFileHandle file_handle = SCOREP_Definitions_NewSourceFile( file_name );

    /* Register file */
    if ( ( file_handle != SCOREP_INVALID_SOURCE_FILE ) &&
         ( strncmp( region_name, "POMP", 4 ) != 0 ) &&
         ( strncmp( region_name, "Pomp", 4 ) != 0 ) &&
         ( strncmp( region_name, "pomp", 4 ) != 0 ) &&
         ( !SCOREP_Filter_Match( file_name, region_name, NULL ) ) )
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
    scorep_compiler_hash_put( str, region_handle );

    free( file_name );
    return region_handle;
}
