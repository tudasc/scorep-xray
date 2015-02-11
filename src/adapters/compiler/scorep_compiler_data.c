/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief Implementation of helper functions which are common for all compiler
 *        adapters.
 */

#include <config.h>

#include "scorep_compiler_data.h"

#include <stdlib.h>
#include <inttypes.h>

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>

#include <SCOREP_Definitions.h>

/**
   A hash table which stores information about regions under their name as
   key. Mainly used to obtain the region handle from the region name.
 */
static
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

/* Get hash table entry for given ID. */
scorep_compiler_hash_node*
scorep_compiler_hash_get( uint64_t key )
{
    uint64_t hash_code = key % SCOREP_COMPILER_REGION_SLOTS;

    UTILS_DEBUG( "hash code %" PRIx64, hash_code );

    scorep_compiler_hash_node* curr = region_hash_table[ hash_code ];
    /* The tail after curr will never change because, new elements are inserted before
       curr. Thus, it allows a parallel @ref scorep_compiler_hash_put which can only
       insert a new element before curr.
     */
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
scorep_compiler_hash_node*
scorep_compiler_hash_put( uint64_t      key,
                          const char*   region_name_mangled,
                          const char*   region_name_demangled,
                          const char*   file_name,
                          SCOREP_LineNo line_no_begin )
{
    uint64_t                   hash_code = key % SCOREP_COMPILER_REGION_SLOTS;
    scorep_compiler_hash_node* add       = ( scorep_compiler_hash_node* )
                                           malloc( sizeof( scorep_compiler_hash_node ) );
    add->key                   = key;
    add->region_name_mangled   = UTILS_CStr_dup( region_name_mangled );
    add->region_name_demangled = UTILS_CStr_dup( region_name_demangled );
    add->file_name             = UTILS_CStr_dup( file_name );
    add->line_no_begin         = line_no_begin;
    add->line_no_end           = SCOREP_INVALID_LINE_NO;
    add->region_handle         = SCOREP_INVALID_REGION;
    /* Inserting elements at the head allows parallel calls to
       @ref scorep_compiler_hash_get
     */
    add->next                      = region_hash_table[ hash_code ];
    region_hash_table[ hash_code ] = add;
    return add;
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
                if ( cur->region_name_mangled != NULL )
                {
                    free( cur->region_name_mangled );
                }
                if ( cur->region_name_demangled != NULL )
                {
                    free( cur->region_name_demangled );
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
}

/* Register a new region to the measurement system */
void
scorep_compiler_register_region( scorep_compiler_hash_node* node )
{
    SCOREP_SourceFileHandle file_handle = SCOREP_Definitions_NewSourceFile( node->file_name );

    UTILS_DEBUG( "Define region %s", node->region_name_demangled );

    node->region_handle = SCOREP_Definitions_NewRegion( node->region_name_demangled,
                                                        node->region_name_mangled,
                                                        file_handle,
                                                        node->line_no_begin,
                                                        node->line_no_end,
                                                        SCOREP_PARADIGM_COMPILER,
                                                        SCOREP_REGION_FUNCTION );

    UTILS_DEBUG( "Define region %s done", node->region_name_demangled );
}


void
scorep_compiler_get_hash_statistics( void )
{
#if HAVE( UTILS_DEBUG )
    unsigned elements         = 0;
    unsigned occupied_buckets = 0;
    unsigned collisions       = 0;
    unsigned buckets          = SCOREP_COMPILER_REGION_SLOTS;
    for ( int i = 0; i < SCOREP_COMPILER_REGION_SLOTS; ++i )
    {
        if ( !region_hash_table[ i ] )
        {
            continue;
        }
        scorep_compiler_hash_node* node = region_hash_table[ i ];
        ++elements;
        ++occupied_buckets;
        while ( node->next )
        {
            ++elements;
            ++collisions;
            node = node->next;
        }
    }
    UTILS_DEBUG( "Hashtable of size %d contains %d"
                 " elements and exhibits %d collisions. Fill rate is %d%%.",
                 buckets,
                 elements,
                 collisions,
                 ( unsigned )( occupied_buckets / ( double )buckets * 100 ) );
#endif
}
