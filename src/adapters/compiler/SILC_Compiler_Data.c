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


silc_compiler_hash_node* region_hash_table[ SILC_COMPILER_HASH_MAX ];

/* Initialize slots of compiler hash table. */
void
silc_compiler_hash_init()
{
    int i;
    for ( i = 0; i < SILC_COMPILER_HASH_MAX; i++ )
    {
        region_hash_table[ i ] = NULL;
    }
}

/* Get hash table entry for given ID. */
silc_compiler_hash_node*
silc_compiler_hash_get( long key )
{
    long hash_code = key % SILC_COMPILER_HASH_MAX;

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
    long                     hash_code = key % SILC_COMPILER_HASH_MAX;
    silc_compiler_hash_node* add       = ( silc_compiler_hash_node* )
                                         malloc( sizeof( silc_compiler_hash_node ) );
    add->key                       = key;
    add->region_name               = region_name ? ( const char* )strdup( region_name ) : region_name;
    add->file_name                 = file_name ? ( const char* )strdup( file_name ) : file_name;
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
    for ( i = 0; i < SILC_COMPILER_HASH_MAX; i++ )
    {
        if ( region_hash_table[ i ] )
        {
            cur = region_hash_table[ i ];
            while ( cur != NULL )
            {
                next = cur->next;
                free( cur );
                cur = next;
            }
            region_hash_table[ i ] = NULL;
        }
    }
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
                                             SILC_INVALID_SOURCE_FILE,
                                             node->line_no_begin,
                                             node->line_no_end,
                                             SILC_ADAPTER_COMPILER,
                                             SILC_REGION_FUNCTION
                                             );
    SILC_UnlockRegionDefinition();
}
