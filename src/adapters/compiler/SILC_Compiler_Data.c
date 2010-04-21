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
 * @maintainer Rene Jaekel <rene.jaekel@tu-dresden.de>
 *
 * @status     ALPHA
 *
 * @brief General functions for Compiler Instrumentation
 */



#include "SILC_Definitions.h"
#include "SILC_DefinitionLocking.h"
#include <SILC_Compiler_Data.h>


HashNode* htab[ HASH_MAX ];

/**
 * Initialize slots of compiler hash table.
 */
void
hash_init()
{
    int i;
    for ( i = 0; i < HASH_MAX; i++ )
    {
        htab[ i ] = NULL;
    }
}

/**
 * @brief Get hash table entry for given ID.
 *
 * @param h   Hash node key ID
 *
 * @return Returns pointer to hash table entry according to key
 */
HashNode*
hash_get( long h )
{
    long id = h % HASH_MAX;

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " hash id %ld: \n", id );

    HashNode* curr = htab[ id ];
    while ( curr )
    {
        if ( curr->id == h )
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}


/**
 * Stores function name under hash code
 *
 * @param h    Hash node key
 * @param n    file name
 * @param fn   function name
 * @param lno  line number
 */
void
hash_put
(
    long        h,
    const char* n,
    const char* fn,
    int         lno
)
{
    long      id  = h % HASH_MAX;
    HashNode* add = ( HashNode* )malloc( sizeof( HashNode ) );
    add->id        = h;
    add->name      = n;
    add->fname     = fn ? ( const char* )strdup( fn ) : fn;
    add->lnobegin  = lno;
    add->lnoend    = SILC_INVALID_LINE_NO;
    add->reghandle = SILC_INVALID_REGION;
    add->next      = htab[ id ];
    htab[ id ]     = add;
}


/**
 * Free elements of compiler hash table.
 */
void
hash_free()
{
    HashNode* next;
    HashNode* cur;
    int       i;
    for ( i = 0; i < HASH_MAX; i++ )
    {
        if ( htab[ i ] )
        {
            cur = htab[ i ];
            while ( cur != NULL )
            {
                next = cur->next;
                free( cur );
                cur = next;
            }
            htab[ i ] = NULL;
        }
    }
}

/**
 * @brief Register a new region to the measuremnt system
 *
 * @param hn   Hash node which stores the registered regions
 */
void
silc_compiler_register_region
(
    struct HashNode* hn
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "register a region! " );


    SILC_LockRegionDefinition();

    hn->reghandle = SILC_DefineRegion( hn->name,
                                       SILC_INVALID_SOURCE_FILE,
                                       SILC_INVALID_LINE_NO,
                                       SILC_INVALID_LINE_NO,
                                       SILC_ADAPTER_COMPILER,
                                       SILC_REGION_FUNCTION
                                       );
    SILC_UnlockRegionDefinition();
}
