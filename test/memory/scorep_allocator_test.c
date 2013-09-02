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
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrEquals( tc, 0, allocator );
}


void
allocator_test_2( CuTest* tc )
{
    uint32_t total_mem = 1024;
    uint32_t page_size = 1024;

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrEquals( tc, 0, allocator );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_3( CuTest* tc )
{
    uint32_t total_mem = 0;
    uint32_t page_size = 0;                    // no pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrEquals( tc, 0, allocator );
}


void
allocator_test_4( CuTest* tc )
{
    uint32_t total_mem = 42;
    uint32_t page_size = 0;                    // invalid page size

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrEquals( tc, 0, allocator );
}


void
allocator_test_5( CuTest* tc )
{
    uint32_t total_mem = 1024 * 1024;
    uint32_t page_size = 512;                    // 2048 pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_6( CuTest* tc )
{
    uint32_t total_mem = 1024 * 1024;
    uint32_t page_size = 511;                    // 2052.0078 pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_7( CuTest* tc )
{
    uint32_t total_mem = 2048;
    uint32_t page_size = 1024;                    // two page

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
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
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
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
allocator_test_9( CuTest* tc )
{
    uint32_t total_mem = 2048;
    uint32_t page_size = 1024;                    // two page

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_Page* page_1 = SCOREP_Allocator_AcquirePage( allocator );
    CuAssertPtrNotNull( tc, page_1 );

    SCOREP_Allocator_Page* page_2 = SCOREP_Allocator_AcquirePage( allocator );
    CuAssert( tc, "can't fetch initial page as there is just one",
              page_2 == 0 );

    SCOREP_Allocator_ReleasePage( page_1 );

    page_2 = SCOREP_Allocator_AcquirePage( allocator );
    CuAssertPtrNotNull( tc, page_2 );

    void* begin = SCOREP_Allocator_GetStartAddressFromPage( page_2 );
    void* end   = SCOREP_Allocator_GetEndAddressFromPage( page_2 );

    CuAssert( tc, "end-begin != page_size",
              ( ptrdiff_t )end - ( ptrdiff_t )begin == page_size );

    SCOREP_Allocator_ReleasePage( page_2 );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_10( CuTest* tc )
{
    uint32_t total_mem = 1536;
    uint32_t page_size = 512;                    // three pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_PageManager* page_manager_1
        = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager_1 );

    void* memory = SCOREP_Allocator_Alloc( page_manager_1, 0 );
    CuAssertPtrEquals( tc, 0, memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 257 );
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
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
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
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
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
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
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
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
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
allocator_test_15( CuTest* tc )
{
    uint32_t total_mem = 4096;
    uint32_t page_size = 1024; // four pages

    /* the remote page manager, will be 'moved' */

    SCOREP_Allocator_Allocator* remote_allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, remote_allocator );

    /*
     * pop some pages before creating the page manager, so we don't get the
     * first page
     */
    SCOREP_Allocator_Page* page_1
        = SCOREP_Allocator_AcquirePage( remote_allocator );
    SCOREP_Allocator_Page* page_2
        = SCOREP_Allocator_AcquirePage( remote_allocator );


    SCOREP_Allocator_PageManager* remote_page_manager
        = SCOREP_Allocator_CreatePageManager( remote_allocator );
    CuAssertPtrNotNull( tc, remote_page_manager );

    /* put back the unused pages */
    SCOREP_Allocator_ReleasePage( page_1 );
    SCOREP_Allocator_ReleasePage( page_2 );

    /* force a non-zero offset in the movable */
    SCOREP_Allocator_AllocMovable( remote_page_manager, 512 );
    SCOREP_Allocator_MovableMemory the_movable
        = SCOREP_Allocator_AllocMovable( remote_page_manager, sizeof( int ) );
    CuAssert( tc, "check movable pointer", the_movable != 0 );


    int* int_memory
        = SCOREP_Allocator_GetAddressFromMovableMemory( remote_page_manager,
                                                        the_movable );
    CuAssertPtrNotNull( tc, int_memory );

    *int_memory = 42;

    /* prepare the remote for moving */
    uint32_t number_of_used_pages
        = SCOREP_Allocator_GetNumberOfUsedPages( remote_page_manager );
    CuAssertIntEquals( tc, 1, number_of_used_pages );

    uint32_t moved_page_ids[ 1 ];
    uint32_t moved_page_usages[ 1 ];
    void*    moved_page_starts[ 1 ];
    SCOREP_Allocator_GetPageInfos( remote_page_manager,
                                   moved_page_ids,
                                   moved_page_usages,
                                   moved_page_starts );
    CuAssert( tc, "page id is not 0", moved_page_ids[ 0 ] != 0 );
    CuAssert( tc, "usage is non-zero",
              moved_page_usages[ 0 ] >= ( 512 + sizeof( int ) ) );
    CuAssertPtrNotNull( tc, moved_page_starts[ 0 ] );

    /*
     * Now transfer number_of_used_pages, moved_page_ids, and moved_page_usages
     * to the receiving partner
     */

    /* the local page manager */

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_PageManager* moved_page_manager
        = SCOREP_Allocator_CreateMovedPageManager( allocator );
    CuAssertPtrNotNull( tc, moved_page_manager );

    /* For each page in {moved_page_ids[ i ],moved_page_usages[ i ]} do */
    {
        /* Get memory to hold the moved page */
        void* moved_memory
            = SCOREP_Allocator_AllocMovedPage( moved_page_manager,
                                               moved_page_ids[ 0 ],
                                               moved_page_usages[ 0 ] );
        CuAssertPtrNotNull( tc, moved_memory );

        /*
         * Now copy the page, moved_page_starts[ 0 ] should be used in MPI_Send
         * and moved_memory in MPI_Recv.
         *
         * We get here invalid reads from the source in valgrind,
         * because of the padding between allocs.
         */
        memcpy( moved_memory, moved_page_starts[ 0 ], moved_page_usages[ 0 ] );
    }
    /* For each end */

    /* finally transfer the the_movable */

    int_memory = SCOREP_Allocator_GetAddressFromMovableMemory( moved_page_manager,
                                                               the_movable );
    CuAssertPtrNotNull( tc, int_memory );
    CuAssertIntEquals( tc, 42, *int_memory );

    SCOREP_Allocator_DeletePageManager( moved_page_manager );
    SCOREP_Allocator_DeleteAllocator( allocator );
    SCOREP_Allocator_DeletePageManager( remote_page_manager );
    SCOREP_Allocator_DeleteAllocator( remote_allocator );
}


void
allocator_test_16( CuTest* tc )
{
    uint32_t total_mem = 1024;
    uint32_t page_size = 512; // two pages
    struct obj
    {
        uint32_t foo;
        void*    bar;
        uint8_t  baz;
    }* obj_1, * obj_2;


    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_ObjectManager* object_manager
        = SCOREP_Allocator_CreateObjectManager( allocator,
                                                sizeof( struct obj ) );
    CuAssertPtrNotNull( tc, object_manager );

    obj_1 = SCOREP_Allocator_GetObject( object_manager );
    CuAssertPtrNotNull( tc, obj_1 );

    obj_1->foo = __LINE__;
    obj_1->bar = allocator;
    obj_1->baz = 23;

    obj_2 = SCOREP_Allocator_GetObject( object_manager );
    CuAssertPtrNotNull( tc, obj_2 );

    obj_2->foo = __LINE__;
    obj_2->bar = object_manager;
    obj_2->baz = 42;

    CuAssertPtrEquals( tc, allocator, obj_1->bar );

    SCOREP_Allocator_PutObject( object_manager, obj_1 );
    SCOREP_Allocator_PutObject( object_manager, obj_2 );

    SCOREP_Allocator_DeleteObjectManager( object_manager );

    SCOREP_Allocator_DeleteAllocator( allocator );
}


void
allocator_test_17( CuTest* tc )
{
    uint64_t total_mem = UINT64_MAX;
    uint32_t page_size = 8; // too many pages to fit into 32bit

    if ( sizeof( size_t ) == sizeof( uint64_t ) )
    {
        SCOREP_Allocator_Allocator* allocator
            = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );

        CuAssertPtrEquals( tc, 0, allocator );
    }
}


void
allocator_test_18( CuTest* tc )
{
    uint32_t total_mem = 10240;
    uint32_t page_size = 512;                    // twenty pages

    SCOREP_Allocator_Allocator* allocator
        = SCOREP_Allocator_CreateAllocator( total_mem, page_size, 0, 0, 0 );
    CuAssertPtrNotNull( tc, allocator );

    SCOREP_Allocator_PageManager* page_manager_1
        = SCOREP_Allocator_CreatePageManager( allocator );
    CuAssertPtrNotNull( tc, page_manager_1 );

    void* memory = SCOREP_Allocator_Alloc( page_manager_1, 0 );
    CuAssertPtrEquals( tc, 0, memory );

    memory = SCOREP_Allocator_Alloc( page_manager_1, 1024 );
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
    SUITE_ADD_TEST_NAME( suite, allocator_test_9,
                         "acquire pages" );
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
    SUITE_ADD_TEST_NAME( suite, allocator_test_15,
                         "move page manager" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_16,
                         "object manager" );
    SUITE_ADD_TEST_NAME( suite, allocator_test_17,
                         "too many pages" );
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
