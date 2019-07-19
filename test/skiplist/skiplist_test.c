/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include <config.h>

#include <UTILS_Error.h>

#include <CuTest.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// CuTest's assert functions:
// void CuAssert(CuTest* tc, char* message, int condition);
// void CuAssertTrue(CuTest* tc, int condition);
// void CuAssertStrEquals(CuTest* tc, char* expected, char* actual);
// void CuAssertIntEquals(CuTest* tc, int expected, int actual);
// void CuAssertPtrEquals(CuTest* tc, void* expected, void* actual);
// void CuAssertPtrNotNull(CuTest* tc, void* pointer);

// --- mock memory allocation function

void*
SCOREP_Memory_AllocForMisc( size_t size )
{
    return malloc( size );
}

#define SCOREP_SKIPLIST_TYPE unsigned int
#define SCOREP_SKIPLIST_PREFIX scorep_uint
#include <scorep_skiplist.h>

// --- skiplist callbacks

int
scorep_uint_cmp( const unsigned int* lhs, const unsigned int* rhs )
{
    if ( *lhs < *rhs )
    {
        return -1;
    }
    if ( *lhs > *rhs )
    {
        return 1;
    }
    return 0;
}

void
scorep_uint_print_payload( const char* prefix, const unsigned int* payload )
{
}

void
scorep_uint_reset_payload( unsigned int* payload )
{
    *payload = 0;
}

// --- end of skiplist callbacks

void
skiplist_test_create_and_destroy_list( CuTest* tc )
{
    scorep_uint_skiplist_t* list = scorep_uint_create_list( 4, NULL, NULL, NULL );

    CuAssertPtrNotNull( tc, list );

    scorep_uint_destroy_list( list );
}


void
skiplist_test_new_list_is_empty( CuTest* tc )
{
    scorep_uint_skiplist_t* list = scorep_uint_create_list( 4, NULL, NULL, NULL );

    CuAssertTrue( tc, scorep_uint_is_empty( list ) );

    scorep_uint_destroy_list( list );
}


void
skiplist_test_ensure_height_distribution( CuTest* tc )
{
    unsigned int max_height   = 6;
    unsigned int max_elements =  1000 * pow( 2, 8 );
    unsigned int histogram[ max_height ];
    double       epsilon = 0.05;

    scorep_uint_skiplist_t* list = scorep_uint_create_list( max_height, NULL, NULL, NULL );
    CuAssertPtrNotNull( tc, list );

    for ( unsigned int i = 0; i < max_height; i++ )
    {
        histogram[ i ] = 0;
    }

    for ( unsigned int i = 0; i < max_elements; i++ )
    {
        histogram[ scorep_uint_compute_random_height( list ) ]++;
    }

    for ( unsigned int i = 1; i < max_height - 1; i++ )
    {
        CuAssertTrue( tc, ( double )histogram[ i ] / max_elements > pow( 0.5, i ) * ( 1 - epsilon ) );
        CuAssertTrue( tc, ( double )histogram[ i ] / max_elements < pow( 0.5, i ) * ( 1 + epsilon ) );
    }

    // Check last bin (that should have (roughly) the same number of hits as
    // the last one of the loop.
    CuAssertTrue( tc, ( double )histogram[ max_height - 1 ] / max_elements > pow( 0.5, max_height - 2 ) * ( 1 - epsilon ) );
    CuAssertTrue( tc, ( double )histogram[ max_height - 1 ] / max_elements < pow( 0.5, max_height - 2 ) * ( 1 + epsilon ) );

    scorep_uint_destroy_list( list );
}


void
skiplist_test_ensure_order( CuTest* tc )
{
    scorep_uint_skiplist_t* list = scorep_uint_create_list( 4, NULL, NULL, NULL );
    CuAssertPtrNotNull( tc, list );

    for ( int i = 10; i >= 0; i-- )
    {
        scorep_uint_node* node = scorep_uint_create_node( list );
        CuAssertPtrNotNull( tc, node );

        node->payload = i;

        scorep_uint_insert_node( list, node );
    }

    unsigned int      search_info = 1;
    scorep_uint_node* found_node  = scorep_uint_lower_bound( list, &search_info );

    for ( int i = 1; i <= 10; i++ )
    {
        CuAssertPtrNotNull( tc, found_node );
        CuAssertIntEquals( tc, i, found_node->payload );
        found_node = found_node->next[ 0 ];
    }

    scorep_uint_destroy_list( list );
}


void
skiplist_test_find_first_node( CuTest* tc )
{
    scorep_uint_skiplist_t* list = scorep_uint_create_list( 4, NULL, NULL, NULL );
    CuAssertPtrNotNull( tc, list );

    for ( int i = 1; i <= 5; i++ )
    {
        scorep_uint_node* node = scorep_uint_create_node( list );
        CuAssertPtrNotNull( tc, node );

        node->payload = i;
        scorep_uint_insert_node( list, node );
    }

    unsigned int      search_info = 1;
    scorep_uint_node* found_node  = scorep_uint_lower_bound( list, &search_info );
    CuAssertPtrNotNull( tc, found_node );
    CuAssertIntEquals( tc, 1, found_node->payload );

    scorep_uint_destroy_list( list );
}


void
skiplist_test_find_inner_node( CuTest* tc )
{
    scorep_uint_skiplist_t* list = scorep_uint_create_list( 4, NULL, NULL, NULL );
    CuAssertPtrNotNull( tc, list );

    for ( int i = 1; i <= 5; i++ )
    {
        scorep_uint_node* node = scorep_uint_create_node( list );
        CuAssertPtrNotNull( tc, node );

        node->payload = i;
        scorep_uint_insert_node( list, node );
    }

    unsigned int      search_info = 3;
    scorep_uint_node* found_node  = scorep_uint_lower_bound( list, &search_info );
    CuAssertPtrNotNull( tc, found_node );
    CuAssertIntEquals( tc, 3, found_node->payload );

    scorep_uint_destroy_list( list );
}


void
skiplist_test_find_last_node( CuTest* tc )
{
    scorep_uint_skiplist_t* list = scorep_uint_create_list( 4, NULL, NULL, NULL );
    CuAssertPtrNotNull( tc, list );

    for ( int i = 1; i <= 5; i++ )
    {
        scorep_uint_node* node = scorep_uint_create_node( list );
        CuAssertPtrNotNull( tc, node );

        node->payload = i;
        scorep_uint_insert_node( list, node );
    }

    unsigned int      search_info = 5;
    scorep_uint_node* found_node  = scorep_uint_lower_bound( list, &search_info );
    CuAssertPtrNotNull( tc, found_node );
    CuAssertIntEquals( tc, 5, found_node->payload );

    scorep_uint_destroy_list( list );
}


void
skiplist_test_remove_first_node( CuTest* tc )
{
    scorep_uint_skiplist_t* list = scorep_uint_create_list( 4, NULL, NULL, NULL );
    CuAssertPtrNotNull( tc, list );

    for ( int i = 1; i <= 5; i++ )
    {
        scorep_uint_node* node = scorep_uint_create_node( list );
        CuAssertPtrNotNull( tc, node );

        node->payload = i;
        scorep_uint_insert_node( list, node );
    }

    unsigned int      search_info = 1;
    scorep_uint_node* found_node  = scorep_uint_lower_bound( list, &search_info );
    CuAssertPtrNotNull( tc, found_node );
    CuAssertIntEquals( tc, 1, found_node->payload );

    scorep_uint_remove_node( list, found_node );

    found_node = scorep_uint_lower_bound( list, &search_info );
    CuAssertPtrNotNull( tc, found_node );
    CuAssertIntEquals( tc, 0, found_node->payload );

    scorep_uint_destroy_list( list );
}


void
skiplist_test_remove_inner_node( CuTest* tc )
{
    scorep_uint_skiplist_t* list = scorep_uint_create_list( 4, NULL, NULL, NULL );
    CuAssertPtrNotNull( tc, list );

    for ( int i = 1; i <= 5; i++ )
    {
        scorep_uint_node* node = scorep_uint_create_node( list );
        CuAssertPtrNotNull( tc, node );

        node->payload = i;
        scorep_uint_insert_node( list, node );
    }

    unsigned int      search_info = 3;
    scorep_uint_node* found_node  = scorep_uint_lower_bound( list, &search_info );
    CuAssertPtrNotNull( tc, found_node );
    CuAssertIntEquals( tc, 3, found_node->payload );

    scorep_uint_remove_node( list, found_node );

    found_node = scorep_uint_lower_bound( list, &search_info );
    CuAssertPtrNotNull( tc, found_node );
    CuAssertIntEquals( tc, 2, found_node->payload );

    scorep_uint_destroy_list( list );
}


void
skiplist_test_remove_last_node( CuTest* tc )
{
    scorep_uint_skiplist_t* list = scorep_uint_create_list( 4, NULL, NULL, NULL );
    CuAssertPtrNotNull( tc, list );

    for ( int i = 1; i <= 5; i++ )
    {
        scorep_uint_node* node = scorep_uint_create_node( list );
        CuAssertPtrNotNull( tc, node );

        node->payload = i;
        scorep_uint_insert_node( list, node );
    }

    unsigned int      search_info = 5;
    scorep_uint_node* found_node  = scorep_uint_lower_bound( list, &search_info );
    CuAssertPtrNotNull( tc, found_node );
    CuAssertIntEquals( tc, 5, found_node->payload );

    scorep_uint_remove_node( list, found_node );

    found_node = scorep_uint_lower_bound( list, &search_info );
    CuAssertPtrNotNull( tc, found_node );
    CuAssertIntEquals( tc, 4, found_node->payload );

    scorep_uint_destroy_list( list );
}


void
skiplist_test_remove_only_element_leaves_empty_list( CuTest* tc )
{
    scorep_uint_skiplist_t* list = scorep_uint_create_list( 4, NULL, NULL, NULL );
    CuAssertPtrNotNull( tc, list );

    scorep_uint_node* node = scorep_uint_create_node( list );
    CuAssertPtrNotNull( tc, node );

    node->payload = 1;

    scorep_uint_insert_node( list, node );

    scorep_uint_remove_node( list, node );

    CuAssertTrue( tc, scorep_uint_is_empty( list ) );

    scorep_uint_destroy_list( list );
}


int
main()
{
    CuUseColors();
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "scorep_skiplist" );

    SUITE_ADD_TEST_NAME( suite, skiplist_test_create_and_destroy_list,
                         "Create and destroy list" );

    SUITE_ADD_TEST_NAME( suite, skiplist_test_new_list_is_empty,
                         "New list is empty" );

    SUITE_ADD_TEST_NAME( suite, skiplist_test_ensure_height_distribution,
                         "Height distribution" );

    SUITE_ADD_TEST_NAME( suite, skiplist_test_ensure_order,
                         "List ordering" );

    SUITE_ADD_TEST_NAME( suite, skiplist_test_find_first_node,
                         "Find first node" );

    SUITE_ADD_TEST_NAME( suite, skiplist_test_find_inner_node,
                         "Find inner node" );

    SUITE_ADD_TEST_NAME( suite, skiplist_test_find_last_node,
                         "Find last node" );

    SUITE_ADD_TEST_NAME( suite, skiplist_test_remove_first_node,
                         "Remove first node" );

    SUITE_ADD_TEST_NAME( suite, skiplist_test_remove_inner_node,
                         "Remove inner node" );

    SUITE_ADD_TEST_NAME( suite, skiplist_test_remove_last_node,
                         "Remove last node" );

    SUITE_ADD_TEST_NAME( suite, skiplist_test_remove_only_element_leaves_empty_list,
                         "Removing the only element leaves empty list" );

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
