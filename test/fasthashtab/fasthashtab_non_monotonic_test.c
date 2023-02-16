/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2023,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * Tests for the non-monotonic FastHashtab.
 */

#include <config.h>

#include <SCOREP_FastHashtab.h>

#include <jenkins_hash.h>

#include <CuTest.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/************************** table *********************************************/

typedef uint64_t table_key_t;
typedef uint64_t table_value_t;

#define TABLE_HASH_EXPONENT 8

static uint32_t
table_bucket_idx( table_key_t key )
{
#if defined( USE_JENKINS )
    return jenkins_hash( &key, sizeof( key ), 0 ) & hashmask( TABLE_HASH_EXPONENT );
#else
    /* use key directly to allow to trigger collisions */
    return key & hashmask( TABLE_HASH_EXPONENT );
#endif
}

static bool
table_equals( table_key_t key1,
              table_key_t key2 )
{
    return key1 == key2;
}

static void*
table_allocate_chunk( size_t chunkSize )
{
    void* ptr;
    assert( posix_memalign( &ptr, SCOREP_CACHELINESIZE, chunkSize ) == 0 );
    return ptr;
}

static void
table_free_chunk( void* chunk )
{
}

static table_value_t
table_value_ctor( table_key_t* key,
                  void*        ctorData )
{
    return *( table_value_t* )ctorData;
}

static void
table_value_dtor( table_key_t   key,
                  table_value_t value )
{
}

SCOREP_HASH_TABLE_NON_MONOTONIC( table,
                                 8,
                                 hashsize( TABLE_HASH_EXPONENT ) );

/************************** tests *********************************************/

static void
count_cb( table_key_t   key,
          table_value_t value,
          void*         cbData )
{
    int* count = cbData;
    ( *count )++;
}

static int
count( void )
{
    int count = 0;
    table_iterate_key_value_pairs( count_cb, &count );
    return count;
}

static void
test_01( CuTest* tc )
{
    CuAssertIntEquals( tc, 0, count() );
}

static void
test_02( CuTest* tc )
{
    table_key_t value = 0;
    CuAssertTrue( tc, !table_get( value, &value ) );
    CuAssertIntEquals( tc, 0, count() );
}

static void
test_03( CuTest* tc )
{
    table_key_t   key      = 0;
    table_value_t value    = 0;
    bool          inserted = table_get_and_insert( key, &key, &value );
    CuAssertTrue( tc, inserted );
    CuAssertIntEquals( tc, key, value );
    CuAssertTrue( tc, table_get( key, &value ) );
    CuAssertIntEquals( tc, key, value );
    CuAssertIntEquals( tc, 1, count() );
}

static void
test_04( CuTest* tc )
{
    table_key_t   key      = 0;
    table_value_t value    = 0;
    bool          inserted = table_get_and_insert( key, &key, &value );
    CuAssertTrue( tc, !inserted );
    CuAssertIntEquals( tc, 1, count() );
}

static void
test_05( CuTest* tc )
{
    table_key_t   key = 0;
    table_value_t value;
    CuAssertTrue( tc, table_get( key, &value ) );
    CuAssertIntEquals( tc, key, value );
    CuAssertIntEquals( tc, 1, count() );
}

static void
test_06( CuTest* tc )
{
    table_key_t   key      = 1;
    table_value_t value    = 0;
    bool          inserted = table_get_and_insert( key, &key, &value );
    CuAssertTrue( tc, inserted );
    CuAssertIntEquals( tc, key, value );
    CuAssertTrue( tc, table_get( key, &value ) );
    CuAssertIntEquals( tc, key, value );
    CuAssertIntEquals( tc, 2, count() );
}

static void
test_07( CuTest* tc )
{
    table_key_t   key      = hashsize( TABLE_HASH_EXPONENT );
    table_value_t value    = 0;
    bool          inserted = table_get_and_insert( key, &key, &value );
    CuAssertTrue( tc, inserted );
    CuAssertIntEquals( tc, key, value );
    CuAssertTrue( tc, table_get( key, &value ) );
    CuAssertIntEquals( tc, key, value );
    CuAssertIntEquals( tc, 3, count() );
}

static void
test_08( CuTest* tc )
{
    table_key_t   key = 0;
    table_value_t value;
    CuAssertTrue( tc, table_get( key, &value ) );
    CuAssertIntEquals( tc, key, value );
    CuAssertIntEquals( tc, 3, count() );
}

static void
test_09( CuTest* tc )
{
    table_key_t   key = hashsize( TABLE_HASH_EXPONENT );
    table_value_t value;
    CuAssertTrue( tc, table_get( key, &value ) );
    CuAssertIntEquals( tc, key, value );
    CuAssertIntEquals( tc, 3, count() );
}

static void
test_10( CuTest* tc )
{
    table_key_t key = 2;
    CuAssertTrue( tc, !table_remove( key ) );
    CuAssertIntEquals( tc, 3, count() );
}

static void
test_11( CuTest* tc )
{
    table_key_t key = 1;
    CuAssertTrue( tc, table_remove( key ) );
    CuAssertIntEquals( tc, 2, count() );
    table_value_t value;
    CuAssertTrue( tc, !table_get( key, &value ) );
    CuAssertIntEquals( tc, 2, count() );
}

static void
test_12( CuTest* tc )
{
    table_key_t   key      = 1;
    table_value_t value    = 0;
    bool          inserted = table_get_and_insert( key, &key, &value );
    CuAssertTrue( tc, inserted );
    CuAssertIntEquals( tc, key, value );
    CuAssertTrue( tc, table_get( key, &value ) );
    CuAssertIntEquals( tc, key, value );
    CuAssertIntEquals( tc, 3, count() );
}

static void
test_13( CuTest* tc )
{
    table_key_t key = hashsize( TABLE_HASH_EXPONENT );
    CuAssertTrue( tc, table_remove( key ) );
    CuAssertIntEquals( tc, 2, count() );
    table_value_t value;
    CuAssertTrue( tc, !table_get( key, &value ) );
    CuAssertIntEquals( tc, 2, count() );
}

static void
test_14( CuTest* tc )
{
    table_key_t   key      = 2 * hashsize( TABLE_HASH_EXPONENT );
    table_value_t value    = 0;
    bool          inserted = table_get_and_insert( key, &key, &value );
    CuAssertTrue( tc, inserted );
    CuAssertIntEquals( tc, key, value );
    CuAssertTrue( tc, table_get( key, &value ) );
    CuAssertIntEquals( tc, key, value );
    CuAssertIntEquals( tc, 3, count() );
}

static bool
remove_even( table_key_t   key,
             table_value_t value,
             void*         data )
{
    return value % 2 == 0;
}

static void
test_15( CuTest* tc )
{
    table_remove_if( remove_even, NULL );
    CuAssertIntEquals( tc, 1, count() );
}

static void
test_16( CuTest* tc )
{
    table_key_t   key          = 2;
    table_value_t insert_value = 0;
    bool          inserted     = table_get_and_insert( key, &key, &insert_value );
    CuAssertTrue( tc, inserted );
    CuAssertIntEquals( tc, 2, count() );
    CuAssertIntEquals( tc, 2, insert_value );

    table_value_t get_value = 0;
    bool          removed   = table_get_and_remove( key, &get_value );
    CuAssertTrue( tc, removed );
    CuAssertIntEquals( tc, 2, get_value );
}

static void
test_17( CuTest* tc )
{
    table_value_t get_value = 0;
    bool          removed   = table_get_and_remove( 3, &get_value );
    CuAssertTrue( tc, !removed );
    CuAssertIntEquals( tc, 0, get_value );
}

int
main( int argc, char** argv )
{
    CuUseColors();
    CuString* output = CuStringNew();
#if defined( USE_JENKINS )
    CuSuite* suite = CuSuiteNew( "FastHashtab: non-monotonic jenkins" );
#else
    CuSuite* suite = CuSuiteNew( "FastHashtab: non-monotonic" );
#endif

    SUITE_ADD_TEST_NAME( suite, test_01, "empty" );
    SUITE_ADD_TEST_NAME( suite, test_02, "get from empty" );
    SUITE_ADD_TEST_NAME( suite, test_03, "insert first bucket" );
    SUITE_ADD_TEST_NAME( suite, test_04, "insert same key" );
    SUITE_ADD_TEST_NAME( suite, test_05, "get from non-empty" );
    SUITE_ADD_TEST_NAME( suite, test_06, "insert second bucket" );
    SUITE_ADD_TEST_NAME( suite, test_07, "insert same bucket" );
    SUITE_ADD_TEST_NAME( suite, test_08, "get same bucket first" );
    SUITE_ADD_TEST_NAME( suite, test_09, "get same bucket second" );
    SUITE_ADD_TEST_NAME( suite, test_10, "remove non-existent" );
    SUITE_ADD_TEST_NAME( suite, test_11, "remove second bucket" );
    SUITE_ADD_TEST_NAME( suite, test_12, "insert second bucket again" );
    SUITE_ADD_TEST_NAME( suite, test_13, "remove first bucket" );
    SUITE_ADD_TEST_NAME( suite, test_14, "insert first bucket again" );
    SUITE_ADD_TEST_NAME( suite, test_15, "remove all even" );
    SUITE_ADD_TEST_NAME( suite, test_16, "get and remove existing key" );
    SUITE_ADD_TEST_NAME( suite, test_17, "get and remove non-existing key" );
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
