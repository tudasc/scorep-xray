/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       definitions_test.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <CuTest.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <silc_definitions.h>
#include <silc_definition_structs.h>
#include <SILC_AllocatorImpl.h>

/* *INDENT-OFF* */
/* *INDENT-ON*  */


// CuTest's assert functions:
// void CuAssert(CuTest* tc, char* message, int condition);
// void CuAssertTrue(CuTest* tc, int condition);
// void CuAssertStrEquals(CuTest* tc, char* expected, char* actual);
// void CuAssertIntEquals(CuTest* tc, int expected, int actual);
// void CuAssertPtrEquals(CuTest* tc, void* expected, void* actual);
// void CuAssertPtrNotNull(CuTest* tc, void* pointer);


SILC_Allocator_Allocator*   allocator;
SILC_Allocator_PageManager* page_manager;

/* *INDENT-OFF* */
typedef struct test_definition_manager test_definition_manager;
struct test_definition_manager
{
    // note: no ';'
    #define TEST_DEFINE_DEFINITION_LIST( counter_type, Type, type ) \
        SILC_ ## Type ## _Definition_Movable type ## _definition_head; \
        SILC_ ## Type ## _Definition_Movable* type ## _definition_tail_pointer; \
        counter_type type ## _definition_counter;

    TEST_DEFINE_DEFINITION_LIST( uint32_t, String, string )

    #undef TEST_DEFINE_DEFINITION_LIST
};
static test_definition_manager definition_manager;
/* *INDENT-ON* */

/* *INDENT-OFF* */
#define TEST_MEMORY_DEREF_MOVABLE( movable_memory_ptr, target_type ) \
    ( ( target_type )SILC_Allocator_GetAddressFromMovableMemory( page_manager, \
          ( SILC_Allocator_MovableMemory* )movable_memory_ptr ) )


#define TEST_ALLOC_NEW_DEFINITION( Type, type ) \
    do { \
        new_movable = ( SILC_ ## Type ## _Definition_Movable* ) \
            SILC_Allocator_AllocMovable( page_manager, \
                sizeof( SILC_ ## Type ## _Definition ) ); \
        assert(new_movable->page_id < 10); \
        new_definition = \
            TEST_MEMORY_DEREF_MOVABLE( new_movable, \
                                       SILC_ ## Type ## _Definition* ); \
        SILC_ALLOCATOR_MOVABLE_INIT_NULL( ( new_definition )->next ); \
        *definition_manager.type ## _definition_tail_pointer = \
            *new_movable; \
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
        new_movable = ( SILC_ ## Type ## _Definition_Movable* ) \
            SILC_Allocator_AllocMovable( page_manager, \
                sizeof( SILC_ ## Type ## _Definition ) + \
                ( ( number_of_members ) - 1 ) * sizeof( array_type ) ); \
        new_definition = \
            TEST_MEMORY_DEREF_MOVABLE( new_movable, \
                                       SILC_ ## Type ## _Definition* ); \
        SILC_ALLOCATOR_MOVABLE_INIT_NULL( ( new_definition )->next ); \
        *definition_manager.type ## _definition_tail_pointer = \
            *new_movable; \
        definition_manager.type ## _definition_tail_pointer = \
            &( new_definition )->next; \
        ( new_definition )->sequence_number = \
            definition_manager.type ## _definition_counter++; \
    } while ( 0 )

#define TEST_DEFINITION_FOREACH_DO( manager_pointer, Type, type ) \
    do { \
        SILC_ ## Type ## _Definition* definition; \
        SILC_ ## Type ## _Definition_Movable* moveable; \
        for ( moveable = &( manager_pointer )->type ## _definition_head; \
              !SILC_ALLOCATOR_MOVABLE_IS_NULL( *moveable ); \
              moveable = &definition->next ) \
        { \
            definition = \
                TEST_MEMORY_DEREF_MOVABLE( moveable, \
                                           SILC_ ## Type ## _Definition* ); \
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
        SILC_ALLOCATOR_MOVABLE_INIT_NULL( \
            definition_manager.type ## _definition_head ); \
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
//    OTF2_DefWriter* definition_writer = silc_create_definition_writer();
//    silc_write_definitions( definition_writer );
//    silc_delete_definition_writer( definition_writer );
}

static const char const* test_stings[] = {
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


static SILC_StringHandle
test_define_string( CuTest*     tc,
                    const char* str )
{
    SILC_String_Definition*         new_definition = NULL;
    SILC_String_Definition_Movable* new_movable    = NULL;

    uint32_t                        string_length = strlen( str );
    TEST_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY( String,
                                              string,
                                              char,
                                              string_length + 1 );

    CuAssertPtrNotNull( tc, new_definition );
    CuAssertPtrNotNull( tc, new_movable );

    new_definition->string_length = string_length;
    strcpy( new_definition->string_data, str );

    return new_movable;
}


static void
allocator_initialize( CuTest* tc )
{
    int total_mem = 1024;
    int page_size = 64; // that makes two strings of length 3 per page
    allocator = SILC_Allocator_CreateAllocator( paged_alloc, total_mem, page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );
    page_manager = SILC_Allocator_CreatePageManager( allocator );
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
        SILC_StringHandle str = test_define_string( tc, test_stings[ i ] );
    }

    loop_over_string_definitions( tc );

    test_definitions_finalize();
}


int
main()
{
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "definition manager" );

    SUITE_ADD_TEST( suite, test_1 );

    CuSuiteRun( suite );
    CuSuiteSummary( suite, output );
    printf( "%s", output->buffer );

    return suite->failCount;
}
