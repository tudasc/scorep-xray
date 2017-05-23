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
 * Copyright (c) 2009-2011, 2014, 2017,
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
#include <stdio.h>
#include <string.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Memory.h>

/* *INDENT-OFF* */
/* *INDENT-ON*  */


// CuTest's assert functions:
// void CuAssert(CuTest* tc, char* message, int condition);
// void CuAssertTrue(CuTest* tc, int condition);
// void CuAssertStrEquals(CuTest* tc, char* expected, char* actual);
// void CuAssertIntEquals(CuTest* tc, int expected, int actual);
// void CuAssertPtrEquals(CuTest* tc, void* expected, void* actual);
// void CuAssertPtrNotNull(CuTest* tc, void* pointer);


SCOREP_Allocator_Allocator*   allocator;
SCOREP_Allocator_PageManager* page_manager;

/* *INDENT-OFF* */
typedef struct test_definition_manager test_definition_manager;
struct test_definition_manager
{
    // note: no ';'
    #define TEST_DEFINE_DEFINITION_LIST( counter_type, Type, type ) \
        SCOREP_ ## Type ## Handle type ## _definition_head; \
        SCOREP_ ## Type ## Handle* type ## _definition_tail_pointer; \
        counter_type type ## _definition_counter;

    TEST_DEFINE_DEFINITION_LIST( uint32_t, String, string )

    #undef TEST_DEFINE_DEFINITION_LIST
};
static test_definition_manager definition_manager;
/* *INDENT-ON* */

/* *INDENT-OFF* */
#define TEST_MEMORY_DEREF_MOVABLE( movable_memory, target_type ) \
    ( ( target_type )SCOREP_Allocator_GetAddressFromMovableMemory( \
          page_manager, \
          movable_memory ) )


#define TEST_ALLOC_NEW_DEFINITION( Type, type ) \
    do { \
        new_handle = SCOREP_Allocator_AllocMovable( \
                page_manager, \
                sizeof( SCOREP_ ## Type ## Def ) ); \
        new_definition = \
            TEST_MEMORY_DEREF_MOVABLE( new_handle, \
                                       SCOREP_ ## Type ## Def* ); \
        new_definition->next = SCOREP_MOVABLE_NULL; \
        *definition_manager.type ## _definition_tail_pointer = \
            new_handle; \
        definition_manager.type ## _definition_tail_pointer = \
            &( new_definition )->next; \
        ( new_definition )->sequence_number = \
            definition_manager.type ## _definition_counter++; \
    } while ( 0 )


#define TEST_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY( Type, \
                                                  type, \
                                                  array_type, \
                                                  number_of_members ) \
    do { \
        new_handle = SCOREP_Allocator_AllocMovable( \
                page_manager, \
                sizeof( SCOREP_ ## Type ## Def ) + \
                ( ( number_of_members ) - 1 ) * sizeof( array_type ) ); \
        new_definition = \
            TEST_MEMORY_DEREF_MOVABLE( new_handle, \
                                       SCOREP_ ## Type ## Def* ); \
        new_definition->next = SCOREP_MOVABLE_NULL; \
        *definition_manager.type ## _definition_tail_pointer = \
            new_handle; \
        definition_manager.type ## _definition_tail_pointer = \
            &( new_definition )->next; \
        ( new_definition )->sequence_number = \
            definition_manager.type ## _definition_counter++; \
    } while ( 0 )

#define TEST_DEFINITION_FOREACH_DO( manager_pointer, Type, type ) \
    do { \
        SCOREP_ ## Type ## Def* definition; \
        SCOREP_ ## Type ## Handle handle; \
        for ( handle = ( manager_pointer )->type ## _definition_head; \
              handle != SCOREP_MOVABLE_NULL; \
              handle = definition->next ) \
        { \
            definition = TEST_MEMORY_DEREF_MOVABLE( \
                handle, SCOREP_ ## Type ## Def* ); \
            {


#define TEST_DEFINITION_FOREACH_WHILE() \
            } \
        } \
    } while ( 0 )
/* *INDENT-ON* */


static void
test_definitions_initialize()
{
    memset( &definition_manager, 0, sizeof( definition_manager ) );

    // note, only lower-case type needed
    #define TEST_INIT_DEFINITION_LIST( type ) \
    do { \
        definition_manager.type ## _definition_tail_pointer = \
            &definition_manager.type ## _definition_head; \
    } while ( 0 )

    TEST_INIT_DEFINITION_LIST( string );

    #undef TEST_INIT_DEFINITION_LIST

    // No need the create the definition writer, its only needed in the
    // finalization phase and will be created there.
}


static void
test_definitions_finalize()
{
//    OTF2_DefWriter* definition_writer = scorep_create_definition_writer();
//    scorep_write_definitions( definition_writer );
//    scorep_delete_definition_writer( definition_writer );
}

static const char* const test_stings[] = {
    "foo",
    "bar",
    "baz"
};


static void
loop_over_string_definitions( CuTest* tc )
{
    int i = 0;
    TEST_DEFINITION_FOREACH_DO( &definition_manager, String, string )
    {
        CuAssertStrEquals(
            tc,
            test_stings[ i ],
            definition->string_data );
        i++;
    }
    TEST_DEFINITION_FOREACH_WHILE();
}


static SCOREP_StringHandle
test_define_string( CuTest*     tc,
                    const char* str )
{
    SCOREP_StringDef*   new_definition = NULL;
    SCOREP_StringHandle new_handle     = SCOREP_INVALID_STRING;

    uint32_t string_length = strlen( str );
    TEST_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY( String,
                                              string,
                                              char,
                                              string_length + 1 );

    CuAssertPtrNotNull( tc, new_definition );
    CuAssertTrue( tc, new_handle != SCOREP_INVALID_STRING );

    new_definition->string_length = string_length;
    memcpy( new_definition->string_data, str, string_length + 1 );

    return new_handle;
}


static void
allocator_initialize( CuTest* tc )
{
    int total_mem = 1024 * 16;
    int page_size = 1024;
    allocator = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );
    page_manager = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager );
}


static void
test_1( CuTest* tc )
{
    allocator_initialize( tc );

    test_definitions_initialize();

    loop_over_string_definitions( tc );

    for ( size_t i = 0;
          i < sizeof( test_stings ) / sizeof( test_stings[ 0 ] );
          i++ )
    {
        SCOREP_StringHandle str = test_define_string( tc, test_stings[ i ] );
    }

    loop_over_string_definitions( tc );

    test_definitions_finalize();
}


int
main()
{
    CuUseColors();
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "definition manager" );

    SUITE_ADD_TEST_NAME( suite, test_1, "string definitions" );

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
