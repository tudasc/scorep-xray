/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file strutil.c
 *
 * This file contains implementation of strings utility functions
 */
#include <config.h>
#include "strutil.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define WHITESPACE " \t"


char*
mystrndup( const char* str,
           size_t      len )
{
    size_t n;
    char*  dst;

    n = strlen( str );
    if ( len < n )
    {
        n = len;
    }

    dst = ( char* )calloc( n + 1, 1 );
    if ( dst )
    {
        memcpy( dst, str, n );
    }

    return dst;
}




static size_t
skip_ws( const char* str,
         size_t      pos )
{
    size_t len;

    len = strspn( str + pos, WHITESPACE );
    return pos + len;
}


static size_t
next_token( const char* str,
            size_t      pos,
            const char* delim,
            char**      token )
{
    size_t len;

    len = strcspn( str + pos, delim );

    ( *token ) = ( char* )malloc( len + 1 );
    strncpy( ( *token ), str + pos, len );
    ( *token )[ len ] = 0;

    return pos + len;
}



size_t
make_key_value_pairs( const char* str,
                      kv_pair**   kv )
{
    size_t len;
    size_t pos;

    char* key, * val;

    const size_t kv_initial  = 10;
    const size_t kv_increase = 10;
    size_t       kv_size, kv_free;

    len = strlen( str );

    ( *kv ) = malloc( sizeof( kv_pair ) * kv_initial );
    kv_size = 0;
    kv_free = kv_initial;


    pos = 0;
    while ( pos < len )
    {
        pos = skip_ws( str, pos );

        pos = next_token( str, pos, " =\t", &key );

        pos = skip_ws( str, pos );
        if ( str[ pos ] != '=' )
        {
            free( key );
            break;
        }

        pos = skip_ws( str, pos + 1 );

        if ( str[ pos ] == '"' )
        {
            pos++;
            pos = next_token( str, pos, "\"", &val );
            pos++;
        }
        else
        {
            pos = next_token( str, pos, " \t", &val );
        }


        if ( kv_free == 0 )
        {
            ( *kv ) = realloc( ( *kv ),
                               sizeof( kv_pair ) * ( kv_size + kv_increase ) );
            kv_free = kv_increase;
        }

        ( *kv )[ kv_size ].key = key;
        ( *kv )[ kv_size ].val = val;

        ++kv_size;
        --kv_free;
    }

    return kv_size;
}
