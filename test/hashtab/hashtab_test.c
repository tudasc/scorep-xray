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
 * Test for the hash table.
 */

#include <config.h>
#include <SCOREP_Hashtab.h>
#include <stdio.h>

/* Simple hashfunction which returns the pointer address as hash value */
size_t
hashfunc( const void* key )
{
    #if SIZEOF_VOID_P == 4
    return ( int32_t )key;
    #elif SIZEOF_VOID_P == 8
    return ( int64_t )key;
    #else
    #error Unsupported architecture.
    #endif
}

/* Prints the value and key (both integers) */
void
procfunc( SCOREP_Hashtab_Entry* entry )
{
    int* v = ( int* )entry->value.ptr;
    int* k = ( int* )entry->key;
    printf( "value: %d, key: %d\n", *v, *k );
}

/* Main program for hash table test */
int
main()
{
    SCOREP_Hashtab*       ht;      /* the hash table */
    SCOREP_Hashtab_Entry* e;       /* pointer ro one entry */
    int                   a[ 24 ]; /* Array with integer values */
    int                   k[ 24 ]; /* Array with keys */
    int                   i;       /* Counter for loops */

    /* Fill keys wih numbers */
    for ( i = 0; i < 24; i++ )
    {
        k[ i ] = i;
    }

    /* Create hash table */
    ht = SCOREP_Hashtab_CreateSize( 30, &hashfunc, &SCOREP_Hashtab_CompareInt32 );

    /* Fill hashtable */
    for ( i = 0; i < 24; i++ )
    {
        a[ i ] = i * i;
        SCOREP_Hashtab_InsertPtr( ht, &k[ i ], &a[ i ], 0 );
    }

    /* Print all values */
    SCOREP_Hashtab_Foreach( ht, &procfunc );

    /* Find a specific element */
    e = SCOREP_Hashtab_Find( ht, &k[ 10 ], 0 );

    /* Print hashtabel size, key of the element and value */
    printf( "Hastable size %d, %d. Element: %d\n", ( int )SCOREP_Hashtab_Size( ht ), k[ 10 ], *( ( int* )e->value.ptr ) );

    /* Delete hash table */
    SCOREP_Hashtab_Free( ht );

    /* String hash function */
    char* str = "Hallo Welt";
    printf( "String hash: %d\n", ( int )SCOREP_Hashtab_HashString( str ) );
    return 0;
}
