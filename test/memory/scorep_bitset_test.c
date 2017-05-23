/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
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
 *
 */


#include <config.h>

#include <CuTest.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SCOREP_DEBUG_MODULE_NAME ALLOCATOR
#include <UTILS_Debug.h>
#include <memory/scorep_bitset.h>

/* 0 bits */
void
bitset_test_0( CuTest* tc )
{
    uint32_t n_bits  = 0;
    size_t   n_bytes = bitset_size( n_bits );
    CuAssertIntEquals( tc, 0, n_bytes );
}

/* just allocate a bitset */
void
bitset_test_1( CuTest* tc )
{
    uint32_t n_bits  = 1;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );
    free( set );
}

/* don't exceed number of members */
void
bitset_test_2( CuTest* tc )
{
    uint32_t n_bits  = 1;
    size_t   n_bytes = bitset_size( n_bits );

    void* set = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_mark_invalid( set, n_bits );

    uint32_t bit = bitset_find_and_set( set, n_bits );
    CuAssertIntEquals( tc, 0, bit );

    bit = bitset_find_and_set( set, n_bits );
    CuAssertIntEquals( tc, 1, bit );

    free( set );
}

/* find/clear/find should work */
void
bitset_test_3( CuTest* tc )
{
    uint32_t n_bits  = 1;
    size_t   n_bytes = bitset_size( n_bits );

    void* set = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_mark_invalid( set, n_bits );

    uint32_t bit = bitset_find_and_set( set, n_bits );
    CuAssertIntEquals( tc, 0, bit );

    bitset_clear( set, n_bits, bit );

    bit = bitset_find_and_set( set, n_bits );
    CuAssertIntEquals( tc, 0, bit );

    free( set );
}

/* iterate over all even unset bits */
void
bitset_test_4( CuTest* tc )
{
    uint32_t n_bits  = 4096;
    size_t   n_bytes = bitset_size( n_bits );

    void* set = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_mark_invalid( set, n_bits );

    /* set all bits */
    for ( uint32_t i = 0; i < n_bits; i++ )
    {
        uint32_t bit = bitset_find_and_set( set, n_bits );
        CuAssertIntEquals( tc, i, bit );
    }

    /* clear all even bits */
    for ( uint32_t i = 0; i < n_bits; i += 2 )
    {
        bitset_clear( set, n_bits, i );
    }

    /* iterate over all unset bits, which are all even  */
    uint32_t count = 0;
    for ( uint32_t i = bitset_next_free( set, n_bits, 0 );
          i < n_bits;
          i = bitset_next_free( set, n_bits, i + 1 ) )
    {
        count++;
        CuAssertTrue( tc, !( i & 1 ) );
        CuAssertTrue( tc, i < n_bits );
    }
    CuAssertIntEquals( tc, n_bits / 2, count );

    /* iterate over all set bits, which are all odd  */
    count = 0;
    for ( uint32_t i = bitset_next_used( set, n_bits, 0 );
          i < n_bits;
          i = bitset_next_used( set, n_bits, i + 1 ) )
    {
        count++;
        CuAssertTrue( tc, i & 1 );
        CuAssertTrue( tc, i < n_bits );
    }
    CuAssertIntEquals( tc, n_bits / 2, count );

    free( set );
}

/* find range */
void
bitset_test_5( CuTest* tc )
{
    uint32_t n_bits  = 8;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    uint32_t range = bitset_find_and_set_range( set, n_bits, 8 );
    CuAssertIntEquals( tc, 0, range );

    free( set );
}

/* find range */
void
bitset_test_6( CuTest* tc )
{
    uint32_t n_bits  = 4 * BITSET_WORDSIZE;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    uint32_t range = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, 0, range );

    range = bitset_find_and_set_range( set, n_bits, 2 * BITSET_WORDSIZE );
    CuAssertIntEquals( tc, BITSET_WORDSIZE / 2, range );

    range = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, 2 * BITSET_WORDSIZE + BITSET_WORDSIZE / 2, range );

    bitset_clear_range( set, n_bits, BITSET_WORDSIZE, BITSET_WORDSIZE + BITSET_WORDSIZE / 2 );

    range = bitset_find_and_set_range( set, n_bits, 2 * BITSET_WORDSIZE );
    CuAssertIntEquals( tc, n_bits, range );

    bitset_clear_range( set, n_bits, BITSET_WORDSIZE / 4, BITSET_WORDSIZE / 2 );
    range = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, BITSET_WORDSIZE / 4, range );

    free( set );
}

/* find range */
void
bitset_test_10( CuTest* tc )
{
    uint32_t n_bits  = 3 * BITSET_WORDSIZE;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_set_range( set, n_bits, 0, BITSET_WORDSIZE / 2 );

    uint32_t start = bitset_find_and_set_range( set, n_bits, 2 * BITSET_WORDSIZE + BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, BITSET_WORDSIZE / 2, start );

    free( set );
}

/* find range */
void
bitset_test_11( CuTest* tc )
{
    uint32_t n_bits  = 3 * BITSET_WORDSIZE;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_set_range( set, n_bits, BITSET_WORDSIZE / 4, BITSET_WORDSIZE / 2 );

    uint32_t start = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE / 4 );
    CuAssertIntEquals( tc, 0, start );

    start = bitset_find_and_set_range( set, n_bits, 2 * BITSET_WORDSIZE + BITSET_WORDSIZE / 4 );
    CuAssertIntEquals( tc, BITSET_WORDSIZE / 2 + BITSET_WORDSIZE / 4, start );

    free( set );
}

/* find range */
void
bitset_test_12( CuTest* tc )
{
    uint32_t n_bits  = 3 * BITSET_WORDSIZE;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_set_range( set, n_bits, BITSET_WORDSIZE / 2, BITSET_WORDSIZE / 2 );

    uint32_t start = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, 0, start );

    start = bitset_find_and_set_range( set, n_bits, 2 * BITSET_WORDSIZE );
    CuAssertIntEquals( tc, BITSET_WORDSIZE, start );

    free( set );
}

/* find range */
void
bitset_test_13( CuTest* tc )
{
    uint32_t n_bits  = 3 * BITSET_WORDSIZE;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_set_range( set, n_bits, 0, BITSET_WORDSIZE + BITSET_WORDSIZE / 2 );

    uint32_t start = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE + BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, BITSET_WORDSIZE + BITSET_WORDSIZE / 2, start );

    free( set );
}

/* find range */
void
bitset_test_14( CuTest* tc )
{
    uint32_t n_bits  = 3 * BITSET_WORDSIZE;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_set_range( set, n_bits, BITSET_WORDSIZE / 2, BITSET_WORDSIZE );

    uint32_t start = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, 0, start );

    start = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE + BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, BITSET_WORDSIZE + BITSET_WORDSIZE / 2, start );

    free( set );
}

/* find range */
void
bitset_test_15( CuTest* tc )
{
    uint32_t n_bits  = 3 * BITSET_WORDSIZE;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_set_range( set, n_bits, 0, 2 * BITSET_WORDSIZE );

    uint32_t start = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE );
    CuAssertIntEquals( tc, 2 * BITSET_WORDSIZE, start );

    free( set );
}

/* find range */
void
bitset_test_16( CuTest* tc )
{
    uint32_t n_bits  = 3 * BITSET_WORDSIZE;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_set_range( set, n_bits, BITSET_WORDSIZE / 2, BITSET_WORDSIZE + BITSET_WORDSIZE / 2 );

    uint32_t start = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, 0, start );

    start = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE );
    CuAssertIntEquals( tc, 2 * BITSET_WORDSIZE, start );

    free( set );
}

/* find range */
void
bitset_test_17( CuTest* tc )
{
    uint32_t n_bits  = 3 * BITSET_WORDSIZE;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_set_range( set, n_bits, BITSET_WORDSIZE / 2, 2 * BITSET_WORDSIZE );

    uint32_t start = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, 0, start );

    start = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, 2 * BITSET_WORDSIZE + BITSET_WORDSIZE / 2, start );

    free( set );
}

/* find range */
void
bitset_test_18( CuTest* tc )
{
    uint32_t n_bits  = 3 * BITSET_WORDSIZE;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_set_range( set, n_bits, BITSET_WORDSIZE + BITSET_WORDSIZE / 2, BITSET_WORDSIZE + BITSET_WORDSIZE / 2 );

    uint32_t start = bitset_find_and_set_range( set, n_bits, BITSET_WORDSIZE + BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, 0, start );

    free( set );
}

/* find range */
void
bitset_test_19( CuTest* tc )
{
    uint32_t n_bits  = 3 * BITSET_WORDSIZE;
    size_t   n_bytes = bitset_size( n_bits );
    void*    set     = calloc( 1, n_bytes );
    CuAssertPtrNotNull( tc, set );

    bitset_set_range( set, n_bits, 2 * BITSET_WORDSIZE + BITSET_WORDSIZE / 2, BITSET_WORDSIZE / 2 );

    uint32_t start = bitset_find_and_set_range( set, n_bits, 2 * BITSET_WORDSIZE + BITSET_WORDSIZE / 2 );
    CuAssertIntEquals( tc, 0, start );

    free( set );
}

int
main()
{
    CuUseColors();
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "bitset" );

    SUITE_ADD_TEST_NAME( suite, bitset_test_0, "0 bits" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_1, "1 bit" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_2, "don't exceed number of members" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_3, "simple find/clear/find" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_4, "iteration over even bits" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_5, "simple find range" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_6, "find ranges" );

    SUITE_ADD_TEST_NAME( suite, bitset_test_10, "set range 0" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_11, "set range 1" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_12, "set range 2" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_13, "set range 3" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_14, "set range 4" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_15, "set range 5" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_16, "set range 6" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_17, "set range 7" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_18, "set range 8" );
    SUITE_ADD_TEST_NAME( suite, bitset_test_19, "set range 9" );

    CuSuiteRun( suite );
    CuSuiteSummary( suite, output );

    int failCount = suite->failCount;
    if ( failCount )
    {
        printf( "%s", output->buffer );
    }

    CuSuiteFree( suite );
    CuStringFree( output );

    return failCount ? EXIT_FAILURE : EXIT_SUCCESS;
}
