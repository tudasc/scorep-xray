/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014, 2016,
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
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */



/**
 * @file       scorep_allocator_test.c
 *
 *
 */


#include <config.h>
#include <SCOREP_Allocator.h>

#include <CuTest.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if HAVE( SCOREP_VALGRIND )
#include <valgrind/memcheck.h>
#endif // HAVE( SCOREP_VALGRIND )

/* *INDENT-OFF* */
/* *INDENT-ON*  */


// CuTest's assert functions:
// void CuAssert(CuTest* tc, char* message, int condition);
// void CuAssertTrue(CuTest* tc, int condition);
// void CuAssertStrEquals(CuTest* tc, char* expected, char* actual);
// void CuAssertIntEquals(CuTest* tc, int expected, int actual);
// void CuAssertPtrEquals(CuTest* tc, void* expected, void* actual);
// void CuAssertPtrNotNull(CuTest* tc, void* pointer);


void
allocator_test_1( CuTest* tc )
{
    uint32_t total_mem = 1024;
    uint32_t page_size = 2048;                    // page_size > total_mem

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrEquals( tc, 0, allocator );
}


void
allocator_test_2( CuTest* tc )
{
    uint32_t total_mem = 1024;
    uint32_t page_size = 1024;

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrEquals( tc, 0, allocator );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_3( CuTest* tc )
{
    uint32_t total_mem = 0;
    uint32_t page_size = 0;                    // no pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrEquals( tc, 0, allocator );
}


void
allocator_test_4( CuTest* tc )
{
    uint32_t total_mem = 42;
    uint32_t page_size = 0;                    // invalid page size

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrEquals( tc, 0, allocator );
}


void
allocator_test_5( CuTest* tc )
{
    uint32_t total_mem = 1024 * 1024;
    uint32_t page_size = 512;                    // 2048 pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_6( CuTest* tc )
{
    uint32_t total_mem = 1024 * 1024;
    uint32_t page_size = 511;                    // 2052.0078 pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_7( CuTest* tc )
{
    uint32_t total_mem = 2048;
    uint32_t page_size = 1024;                    // two page

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_PageManager* page_manager_1
        = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager_1 );

    SCOREP_Allocator_DeletePageManager( page_manager_1 );
    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_8( CuTest* tc )
{
    uint32_t total_mem = 1024;
    uint32_t page_size =  512;                    // two page

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_PageManager* page_manager_1
        = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager_1 );

    SCOREP_Allocator_PageManager* page_manager_2
        = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager_2 );

    void* memory = SCOREP_Allocator_Alloc( page_manager_2, 8 );
    CuAssertPtrEqualsMsg( tc, "out of pages for second pool", 0, memory );

    SCOREP_Allocator_DeletePageManager( page_manager_1 );

    memory = SCOREP_Allocator_Alloc( page_manager_2, 8 );
    CuAssertPtrNotNullMsg( tc, "new pages available for second pool", memory );

    SCOREP_Allocator_DeletePageManager( page_manager_2 );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_10( CuTest* tc )
{
    uint32_t total_mem = 1536;
    uint32_t page_size = 512;                    // three pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_PageManager* page_manager_1
        = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager_1 );

    void* memory = SCOREP_Allocator_Alloc( page_manager_1, 257 );
    CuAssertPtrNotNullMsg( tc, "from first page", memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 255 );
    CuAssertPtrNotNullMsg( tc, "from second page (257 + alignment)", memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 257 );
    CuAssertPtrEqualsMsg( tc, "mem not available", 0, memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 256 );
    CuAssertPtrNotNullMsg( tc, "from first page", memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1,
                                     512 - ( 257 + ( SCOREP_ALLOCATOR_ALIGNMENT - 1 ) -
                                             ( ( 257 - 1 ) % SCOREP_ALLOCATOR_ALIGNMENT ) ) );
    CuAssertPtrNotNullMsg( tc, "from second page (512 = 257 + alignment + x)",
                           memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 1 );
    CuAssertPtrEqualsMsg( tc, "mem not available", 0, memory );

    SCOREP_Allocator_Free( page_manager_1 );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 512 );
    CuAssertPtrNotNullMsg( tc, "from first page", memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 512 );
    CuAssertPtrNotNullMsg( tc, "from second page", memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 1 );
    CuAssertPtrEqualsMsg( tc, "mem not available", 0, memory );

    SCOREP_Allocator_Free( page_manager_1 );

    SCOREP_Allocator_DeletePageManager( page_manager_1 );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_11( CuTest* tc )
{
    uint32_t total_mem = 1024;
    uint32_t page_size = 512;                    // two pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_PageManager* page_manager_1
        = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager_1 );

    SCOREP_Allocator_MovableMemory movable_mem_2
        = SCOREP_Allocator_AllocMovable( page_manager_1, 128 );
    CuAssert( tc, "movable_mem_2 != 0", movable_mem_2 != 0 );

    void* mem_2 = SCOREP_Allocator_GetAddressFromMovableMemory( page_manager_1,
                                                                movable_mem_2 );
    CuAssertPtrNotNull( tc, mem_2 );

    SCOREP_Allocator_Free( page_manager_1 );

    SCOREP_Allocator_DeletePageManager( page_manager_1 );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_12( CuTest* tc )
{
    uint32_t total_mem = 1024;
    uint32_t page_size = 512;                    // two pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_PageManager* page_manager_1
        = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager_1 );

    SCOREP_Allocator_Free( page_manager_1 );

    SCOREP_Allocator_DeletePageManager( page_manager_1 );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


#if HAVE( SCOREP_VALGRIND )
#ifndef NVALGRIND
void
allocator_test_13( CuTest* tc )
{
    uint32_t total_mem = 1024;
    uint32_t page_size = 512;                    // two pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_PageManager* page_manager_1
        = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager_1 );

    volatile void* memory = SCOREP_Allocator_Alloc( page_manager_1, 64 );

    if ( RUNNING_ON_VALGRIND )
    {
        CuAssertPtrEqualsMsg( tc, "is addressable",
                              ( ( char* )memory + 64 ),
                              ( void* )VALGRIND_CHECK_MEM_IS_ADDRESSABLE( memory, 512 ) );
        CuAssertPtrEqualsMsg( tc, "not defined",
                              memory,
                              ( void* )VALGRIND_CHECK_MEM_IS_DEFINED( memory, 512 ) );
    }

    /* invalid read? */
    unsigned int invalid_read_4 = *( volatile unsigned int* )memory;

    /* valid write */
    *( volatile unsigned int* )memory = 23;

    /*  invalid write */
    *( volatile unsigned int* )( ( char* )memory + 64 ) = 42;

    SCOREP_Allocator_Free( page_manager_1 );

    if ( RUNNING_ON_VALGRIND )
    {
        CuAssertPtrEqualsMsg( tc, "not addressable",
                              memory,
                              ( void* )VALGRIND_CHECK_MEM_IS_ADDRESSABLE( memory, 512 ) );
    }

    /* invalid read? */
    invalid_read_4 = *( volatile unsigned int* )memory;

    /* invalid write */
    *( volatile unsigned int* )memory = 23;

    /*  invalid write */
    *( volatile unsigned int* )( ( char* )memory + 64 ) = 42;

    SCOREP_Allocator_DeletePageManager( page_manager_1 );

    if ( RUNNING_ON_VALGRIND )
    {
        CuAssertPtrEqualsMsg( tc, "not addressable",
                              memory,
                              ( void* )VALGRIND_CHECK_MEM_IS_ADDRESSABLE( memory, 512 ) );
    }

    /* invalid read */
    invalid_read_4 = *( volatile unsigned int* )memory;

    /* invalid write */
    *( volatile unsigned int* )memory = 23;

    /*  invalid write */
    *( volatile unsigned int* )( ( char* )memory + 64 ) = 42;

    SCOREP_Allocator_DeleteAllocator( allocator );

    if ( RUNNING_ON_VALGRIND )
    {
        CuAssertPtrEqualsMsg( tc, "not addressable",
                              memory,
                              ( void* )VALGRIND_CHECK_MEM_IS_ADDRESSABLE( memory, 512 ) );
    }
}
#endif // NVALGRIND
#endif // HAVE( SCOREP_VALGRIND )


void
allocator_test_14( CuTest* tc )
{
    uint32_t total_mem = 4608;
    uint32_t page_size = 512; // eight pages
    uint32_t page_ids[ total_mem / page_size ];
    uint32_t page_usage[ total_mem / page_size ];
    void*    page_start[ total_mem / page_size ];


    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_PageManager* page_manager
        = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager );
    CuAssertIntEquals( tc, 0, SCOREP_Allocator_GetNumberOfUsedPages( page_manager ) );

    memset( page_ids, 0, sizeof( page_ids ) );
    memset( page_usage, 0, sizeof( page_usage ) );
    memset( page_start, 0, sizeof( page_start ) );
    SCOREP_Allocator_GetPageInfos( page_manager, page_ids, page_usage, page_start );
    CuAssert( tc, "no valid page id at 0", page_ids[ 0 ] == 0 );
    CuAssert( tc, "empty usage at 0", page_usage[ 0 ] == 0 );
    CuAssert( tc, "NULL start at 0", page_start[ 0 ] == NULL );

    void* memory = SCOREP_Allocator_Alloc( page_manager, page_size );
    CuAssertPtrNotNull( tc, memory );
    CuAssertIntEquals( tc, 1, SCOREP_Allocator_GetNumberOfUsedPages( page_manager ) );

    memset( page_ids, 0, sizeof( page_ids ) );
    memset( page_usage, 0, sizeof( page_usage ) );
    memset( page_start, 0, sizeof( page_start ) );
    SCOREP_Allocator_GetPageInfos( page_manager, page_ids, page_usage, page_start );
    CuAssert( tc, "valid page id at 0", page_ids[ 0 ] != 0 );
    CuAssert( tc, "usage is page_size", page_usage[ 0 ] == page_size );
    CuAssertPtrNotNull( tc, page_start[ 0 ] );

    memory = SCOREP_Allocator_Alloc( page_manager, page_size / 2 );
    CuAssertPtrNotNull( tc, memory );
    CuAssertIntEquals( tc, 2, SCOREP_Allocator_GetNumberOfUsedPages( page_manager ) );

    memset( page_ids, 0, sizeof( page_ids ) );
    memset( page_usage, 0, sizeof( page_usage ) );
    memset( page_start, 0, sizeof( page_start ) );
    SCOREP_Allocator_GetPageInfos( page_manager, page_ids, page_usage, page_start );
    CuAssert( tc, "valid page id at 0", page_ids[ 0 ] != 0 );
    CuAssert( tc, "valid page id at 1", page_ids[ 1 ] != 0 );
    /* order is undeterministic, so can't check the usage */
    CuAssertPtrNotNull( tc, page_start[ 0 ] );
    CuAssertPtrNotNull( tc, page_start[ 1 ] );

    memory = SCOREP_Allocator_Alloc( page_manager, page_size / 2 );
    CuAssertPtrNotNull( tc, memory );
    CuAssertIntEquals( tc, 2, SCOREP_Allocator_GetNumberOfUsedPages( page_manager ) );

    memset( page_ids, 0, sizeof( page_ids ) );
    memset( page_usage, 0, sizeof( page_usage ) );
    memset( page_start, 0, sizeof( page_start ) );
    SCOREP_Allocator_GetPageInfos( page_manager, page_ids, page_usage, page_start );
    CuAssert( tc, "valid page id at 0", page_ids[ 0 ] != 0 );
    CuAssert( tc, "valid page id at 1", page_ids[ 1 ] != 0 );
    CuAssert( tc, "usage is page_size at 0", page_usage[ 0 ] == page_size );
    CuAssert( tc, "usage is page_size at 1", page_usage[ 1 ] == page_size );
    CuAssertPtrNotNull( tc, page_start[ 0 ] );
    CuAssertPtrNotNull( tc, page_start[ 1 ] );

    memory = SCOREP_Allocator_Alloc( page_manager, page_size / 2 );
    CuAssertPtrNotNull( tc, memory );
    CuAssertIntEquals( tc, 3, SCOREP_Allocator_GetNumberOfUsedPages( page_manager ) );

    memset( page_ids, 0, sizeof( page_ids ) );
    memset( page_usage, 0, sizeof( page_usage ) );
    memset( page_start, 0, sizeof( page_start ) );
    SCOREP_Allocator_GetPageInfos( page_manager, page_ids, page_usage, page_start );
    CuAssert( tc, "valid page id at 0", page_ids[ 0 ] != 0 );
    CuAssert( tc, "valid page id at 1", page_ids[ 1 ] != 0 );
    CuAssert( tc, "valid page id at 2", page_ids[ 2 ] != 0 );
    /* order is undeterministic, so can't check the usage */
    CuAssertPtrNotNull( tc, page_start[ 0 ] );
    CuAssertPtrNotNull( tc, page_start[ 1 ] );
    CuAssertPtrNotNull( tc, page_start[ 2 ] );

    memory = SCOREP_Allocator_Alloc( page_manager, page_size );
    CuAssertPtrNotNull( tc, memory );
    CuAssertIntEquals( tc, 4, SCOREP_Allocator_GetNumberOfUsedPages( page_manager ) );

    memset( page_ids, 0, sizeof( page_ids ) );
    memset( page_usage, 0, sizeof( page_usage ) );
    memset( page_start, 0, sizeof( page_start ) );
    SCOREP_Allocator_GetPageInfos( page_manager, page_ids, page_usage, page_start );
    CuAssert( tc, "valid page id at 0", page_ids[ 0 ] != 0 );
    CuAssert( tc, "valid page id at 1", page_ids[ 1 ] != 0 );
    CuAssert( tc, "valid page id at 2", page_ids[ 2 ] != 0 );
    CuAssert( tc, "valid page id at 2", page_ids[ 3 ] != 0 );
    /* order is undeterministic, so can't check the usage */
    CuAssertPtrNotNull( tc, page_start[ 0 ] );
    CuAssertPtrNotNull( tc, page_start[ 1 ] );
    CuAssertPtrNotNull( tc, page_start[ 2 ] );
    CuAssertPtrNotNull( tc, page_start[ 3 ] );

    memory = SCOREP_Allocator_Alloc( page_manager, page_size / 2 );
    CuAssertPtrNotNull( tc, memory );
    CuAssertIntEquals( tc, 4, SCOREP_Allocator_GetNumberOfUsedPages( page_manager ) );

    memset( page_ids, 0, sizeof( page_ids ) );
    memset( page_usage, 0, sizeof( page_usage ) );
    memset( page_start, 0, sizeof( page_start ) );
    SCOREP_Allocator_GetPageInfos( page_manager, page_ids, page_usage, page_start );
    CuAssert( tc, "valid page id at 0", page_ids[ 0 ] != 0 );
    CuAssert( tc, "valid page id at 1", page_ids[ 1 ] != 0 );
    CuAssert( tc, "valid page id at 2", page_ids[ 2 ] != 0 );
    CuAssert( tc, "valid page id at 2", page_ids[ 3 ] != 0 );
    CuAssert( tc, "usage is page_size at 0", page_usage[ 0 ] == page_size );
    CuAssert( tc, "usage is page_size at 1", page_usage[ 1 ] == page_size );
    CuAssert( tc, "usage is page_size at 2", page_usage[ 2 ] == page_size );
    CuAssert( tc, "usage is page_size at 3", page_usage[ 3 ] == page_size );
    CuAssertPtrNotNull( tc, page_start[ 0 ] );
    CuAssertPtrNotNull( tc, page_start[ 1 ] );
    CuAssertPtrNotNull( tc, page_start[ 2 ] );
    CuAssertPtrNotNull( tc, page_start[ 3 ] );

    SCOREP_Allocator_Free( page_manager );
    CuAssertIntEquals( tc, 0, SCOREP_Allocator_GetNumberOfUsedPages( page_manager ) );

    memset( page_ids, 0, sizeof( page_ids ) );
    memset( page_usage, 0, sizeof( page_usage ) );
    memset( page_start, 0, sizeof( page_start ) );
    SCOREP_Allocator_GetPageInfos( page_manager, page_ids, page_usage, page_start );
    CuAssert( tc, "no valid page id at 0", page_ids[ 0 ] == 0 );
    CuAssert( tc, "empty usage at 0", page_usage[ 0 ] == 0 );
    CuAssert( tc, "NULL start at 0", page_start[ 0 ] == NULL );

    SCOREP_Allocator_DeletePageManager( page_manager );
    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_17( CuTest* tc )
{
    uint32_t total_mem = UINT32_MAX;
    uint32_t page_size = 256; // min page size 512,
                              // from 257 till 512 page_size is rounded to 512

    if ( sizeof( size_t ) == sizeof( uint64_t ) )
    {
        SCOREP_Allocator_Allocator* allocator
            = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );

        CuAssertPtrEquals( tc, 0, allocator );
    }
}


void
allocator_test_18( CuTest* tc )
{
    uint32_t total_mem = 10240;
    uint32_t page_size = 512;                    // twenty pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( &total_mem, &page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_PageManager* page_manager_1
        = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager_1 );

    void* memory = SCOREP_Allocator_Alloc( page_manager_1, 1024 );
    CuAssertPtrNotNullMsg( tc, "2 pages", memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 512 );
    CuAssertPtrNotNullMsg( tc, "one page", memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 1024 );
    CuAssertPtrNotNullMsg( tc, "two pages", memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 2048 );
    CuAssertPtrNotNullMsg( tc, "four pages", memory );

    SCOREP_Allocator_Free( page_manager_1 );

    SCOREP_Allocator_DeletePageManager( page_manager_1 );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


int
main()
{
    CuUseColors();
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "allocator" );

    SUITE_ADD_TEST_NAME( suite, allocator_test_1,
                         "page_size > total_mem" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_2,
                         "page_size == total_mem" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_3,
                         "0 memory" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_4,
                         "0 pages" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_5,
                         "many pages" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_6,
                         "un-aligned page size" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_7,
                         "create page manager" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_8,
                         "two page manager competing for one page" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_10,
                         "alignment" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_11,
                         "alloc movable" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_12,
                         "basic page manager functionality" );
#if HAVE( SCOREP_VALGRIND )
#ifndef NVALGRIND
    SUITE_ADD_TEST_NAME( suite, allocator_test_13,
                         "valgrind" );
#endif // NVALGRIND
#endif // HAVE( SCOREP_VALGRIND )
    SUITE_ADD_TEST_NAME( suite, allocator_test_14,
                         "page infos" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_17,
                         "min page size 512" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_18,
                         "big pages" );

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
