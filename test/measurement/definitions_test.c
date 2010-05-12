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


SILC_Allocator_Allocator*     allocator;
SILC_Allocator_PageManager*   page_manager;
extern SILC_DefinitionManager silc_definition_manager;


/* *INDENT-OFF* */
#define MEMORY_DEREF_MOVABLE( movable_memory_ptr, target_type ) \
    ( ( target_type )SILC_Allocator_GetAddressFromMovableMemory( page_manager, \
          ( SILC_Allocator_MovableMemory* )movable_memory_ptr ) )


#define ALLOC_NEW_DEFINITION( Type, type ) \
    do { \
        new_movable = ( SILC_ ## Type ## _Definition_Movable* ) \
            SILC_Allocator_AllocMovable( page_manager, \
                sizeof( SILC_ ## Type ## _Definition ) ); \
        assert(new_movable->page_id < 10); \
        new_definition = \
            MEMORY_DEREF_MOVABLE( new_movable, \
                                       SILC_ ## Type ## _Definition* ); \
        *silc_definition_manager.type ## _definition_tail_pointer = \
            *new_movable; \
        silc_definition_manager.type ## _definition_tail_pointer = \
            &( new_definition )->next; \
        ( new_definition )->id = \
            silc_definition_manager.type ## _definition_counter++; \
    } while ( 0 )


#define DEFINITION_FOREACH_DO( manager_pointer, Type, type ) \
    do { \
        SILC_ ## Type ## _Definition *definition; \
        SILC_ ## Type ## _Definition_Movable moveable; \
        for ( moveable = ( manager_pointer )->type ## _definition_head; \
              !SILC_ALLOCATOR_MOVABLE_IS_NULL( moveable ); \
              moveable = definition->next ) \
        { \
                assert(&moveable); \
            definition = \
                MEMORY_DEREF_MOVABLE( &moveable, \
                                      SILC_ ## Type ## _Definition* ); \
            {


#define DEFINITION_FOREACH_WHILE() \
            } \
        } \
    } while ( 0 )
/* *INDENT-ON* */


void
Definitions_Initialize()
{
    memset( &silc_definition_manager, 0, sizeof( silc_definition_manager ) );

    // note, only lower-case type needed
    #define INIT_DEFINITION_LIST( type ) \
    do { \
        SILC_ALLOCATOR_MOVABLE_INIT_NULL( \
            silc_definition_manager.type ## _definition_head ); \
        silc_definition_manager.type ## _definition_tail_pointer = \
            &silc_definition_manager.type ## _definition_head; \
    } while ( 0 )

    INIT_DEFINITION_LIST( string );

    #undef INIT_DEFINITION_LIST

    // No need the create the definition writer, its only needed in the
    // finalization phase and will be created there.
}


void
Definitions_Finalize()
{
//    OTF2_DefWriter* definition_writer = silc_create_definition_writer();
//    silc_write_definitions( definition_writer );
//    silc_delete_definition_writer( definition_writer );
}


void
loop_over_string_definitions()
{
    int i = 0;
    DEFINITION_FOREACH_DO( &silc_definition_manager, String, string )
    {
        printf( "loop index = %d\n", i++ );
//        SILC_Error_Code status = OTF2_DefWriter_DefString(
//            definitionWriter,
//            definition->id,
//            SILC_MEMORY_DEREF_MOVABLE( &(definition->str), char* ) );
//        if ( status != SILC_SUCCESS )
//        {
//            silc_handle_definition_writing_error( status, "SILC_String_Definition" );
//        }
    }
    DEFINITION_FOREACH_WHILE();
}


SILC_StringHandle
DefineString( CuTest*     tc,
              const char* str )
{
    SILC_String_Definition*         new_definition = NULL;
    SILC_String_Definition_Movable* new_movable    = NULL;

    ALLOC_NEW_DEFINITION( String, string );
    CuAssertPtrNotNull( tc, new_definition );
    CuAssertPtrNotNull( tc, new_movable );
    CuAssert( tc, "page_id == 0", new_movable->page_id == 0 );

//    SILC_Memory_AllocForDefinitionsRaw( strlen( str ) + 1,
//              ( SILC_Allocator_MovableMemory* )&new_definition->str );
    SILC_Allocator_AllocMovableRaw( page_manager,
                                    strlen( str ) + 1,
                                    ( SILC_Allocator_MovableMemory* )&new_definition->str );
    CuAssert( tc, "page_id == 0", new_definition->str.page_id == 0 );
    strcpy( MEMORY_DEREF_MOVABLE( &new_definition->str, char* ), str );

    return new_movable;
}


void
allocator_initialize( CuTest* tc )
{
    int total_mem = 1024;
    int page_size = 512; // two pages
    allocator = SILC_Allocator_CreateAllocator( paged_alloc, total_mem, page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );
    page_manager = SILC_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager );
}


void
test_1( CuTest* tc )
{
    allocator_initialize( tc );

    Definitions_Initialize();
    loop_over_string_definitions();     // ok so far

    SILC_StringHandle bar = DefineString( tc, "foo" );
    CuAssert( tc, "page_id == 0", bar->page_id == 0 );

    //loop_over_string_definitions();     // endless loop

    Definitions_Finalize();
}


int
main()
{
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew();

    SUITE_ADD_TEST( suite, test_1 );

    CuSuiteRun( suite );
    CuSuiteSummary( suite, output );
    CuSuiteDetails( suite, output );
    printf( "%s\n", output->buffer );

    return suite->failCount;
}
