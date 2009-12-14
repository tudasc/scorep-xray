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
 * @ file      SILC_compiler_gnu.c
 * @maintainer Rene Jaekel <rene.jaekel@tu-dresden.de>
 *
 * @status     ALPHA
 *
 * @brief Support for GNU-Compiler
 * Will be triggered by the '-finstrument-functions' flag of the GNU
 * compiler.
 */


#include <SILC_Compiler_Data.h>


HashNode* htab[ HASH_MAX ];


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

    printf( " hash id %ld: \n", id );

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

void
hash_free()
{
    int i;
    for ( i = 0; i < HASH_MAX; i++ )
    {
        if ( htab[ i ] )
        {
            free( htab[ i ] );
            htab[ i ] = NULL;
        }
    }
}
