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
 * @file
 *
 *
 *
 *
 */

#include <config.h>
#include "scorep_allocator.h"
#include <SCOREP_Allocator.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCOREP_DEBUG_MODULE_NAME ALLOCATOR
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include "scorep_bitset.h"
#include "scorep_page.h"

#if HAVE( SCOREP_VALGRIND )
#  include <valgrind/memcheck.h>

#else // !HAVE( SCOREP_VALGRIND )

#  define VALGRIND_MAKE_MEM_NOACCESS( x, y )  do { } while ( 0 )
#  define VALGRIND_MAKE_MEM_UNDEFINED( x, y ) do { } while ( 0 )
#  define VALGRIND_CREATE_MEMPOOL( x, y, z )  do { } while ( 0 )
#  define VALGRIND_DESTROY_MEMPOOL( x )       do { } while ( 0 )
#  define VALGRIND_MEMPOOL_ALLOC( x, y, z )   do { } while ( 0 )
#  define VALGRIND_MEMPOOL_FREE( x, y )       do { } while ( 0 )
#  define VALGRIND_MEMPOOL_TRIM( x, y, z )    do { } while ( 0 )

#endif // HAVE( SCOREP_VALGRIND )

#define SCOREP_ALLOCATOR_MOVABLE_PAGE_SHIFT 32
#define SCOREP_ALLOCATOR_MOVABLE_PAGE_MASK  0xffffffff

/* 8 objects per page should be minimum to be efficient */
#define MIN_NUMBER_OF_OBJECTS_PER_PAGE 8


#define roundupto( x, to ) ( ( x ) + ( ( to ) - 1 ) - ( ( ( x ) - 1 ) % ( to ) ) )


#define roundup( x ) roundupto( x, SCOREP_ALLOCATOR_ALIGNMENT )

/**
 * Calculate the smallest power-of-two number which is greater/equal to @a v.
 */
static inline uint32_t
npot( uint32_t v )
{
    v--;
    v |= v >>  1;
    v |= v >>  2;
    v |= v >>  4;
    v |= v >>  8;
    v |= v >> 16;
    return v + 1;
}


static inline size_t
union_size( void )
{
    return npot( sizeof( SCOREP_Allocator_Object ) );
}


static void
null_guard( SCOREP_Allocator_GuardObject guardObject )
{
}


static inline void
lock_allocator( SCOREP_Allocator_Allocator* allocator )
{
    allocator->lock( allocator->lock_object );
}


static inline void
unlock_allocator( SCOREP_Allocator_Allocator* allocator )
{
    allocator->unlock( allocator->lock_object );
}


static inline void*
page_bitset( SCOREP_Allocator_Allocator* allocator )
{
    return ( char* )allocator + union_size();
}


static inline void
track_update_high_watermark( SCOREP_Allocator_Allocator* allocator )
{
    if ( allocator->n_pages_allocated > allocator->n_pages_high_watermark )
    {
        allocator->n_pages_high_watermark = allocator->n_pages_allocated;
    }
}

static inline void
track_bitset_set_range( SCOREP_Allocator_Allocator* allocator,
                        uint32_t                    offset,
                        uint32_t                    length )
{
    bitset_set_range( page_bitset( allocator ), allocator->n_pages_capacity, offset, length );
    allocator->n_pages_allocated += length;
    track_update_high_watermark( allocator );
}

static inline void
track_bitset_clear( SCOREP_Allocator_Allocator* allocator, uint32_t pos )
{
    bitset_clear( page_bitset( allocator ), allocator->n_pages_capacity, pos );
    allocator->n_pages_allocated--;
}

static inline void
track_bitset_clear_range( SCOREP_Allocator_Allocator* allocator,
                          uint32_t                    offset,
                          uint32_t                    length )
{
    bitset_clear_range( page_bitset( allocator ), allocator->n_pages_capacity, offset, length );
    allocator->n_pages_allocated -= length;
}

static inline uint32_t
track_bitset_find_and_set( SCOREP_Allocator_Allocator* allocator )
{
    allocator->n_pages_allocated++;  /* increment even if there is no new page */
    track_update_high_watermark( allocator );
    return bitset_find_and_set( page_bitset( allocator ), allocator->n_pages_capacity );
}

static inline uint32_t
track_bitset_find_and_set_range( SCOREP_Allocator_Allocator* allocator,
                                 uint32_t                    rangeLength )
{
    allocator->n_pages_allocated += rangeLength;
    track_update_high_watermark( allocator );
    return bitset_find_and_set_range( page_bitset( allocator ), allocator->n_pages_capacity, rangeLength );
}


static inline void
fill_with_union_objects( uint32_t freeMemory, char* startAddr, SCOREP_Allocator_Allocator* allocator )
{
    while ( freeMemory > union_size() )
    {
        SCOREP_Allocator_Object* object = ( SCOREP_Allocator_Object* )startAddr;
        object->next            = allocator->free_objects;
        allocator->free_objects = object;
        startAddr              += union_size();
        freeMemory             -= union_size();
    }
}


/*
 * Caller needs to hold the allocator lock.
 */
static void*
get_union_object( SCOREP_Allocator_Allocator* allocator )
{
    UTILS_DEBUG_ENTRY();
    if ( !allocator->free_objects )
    {
        /* try to get a new maintenance page */
        uint32_t page_id = track_bitset_find_and_set( allocator );
        if ( page_id >= allocator->n_pages_capacity )
        {
            UTILS_DEBUG_EXIT( "out-of-memory: no free page" );
            return NULL;
        }
        char*    start_addr  = ( char* )allocator + ( page_id << allocator->page_shift );
        uint32_t free_memory = page_size( allocator );
        fill_with_union_objects( free_memory, start_addr, allocator );
        allocator->n_pages_maintenance++;
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "\'allocated\' 1 maintenance page." );
    }

    SCOREP_Allocator_Object* object = allocator->free_objects;
    allocator->free_objects = object->next;
    object->next            = NULL;

    UTILS_DEBUG_EXIT();
    return object;
}


/*
 * Caller needs to hold the allocator lock.
 */
static void
put_union_object( SCOREP_Allocator_Allocator* allocator, void* object_ptr )
{
    UTILS_DEBUG_ENTRY();
    SCOREP_Allocator_Object* object = object_ptr;
    object->next            = allocator->free_objects;
    allocator->free_objects = object;
    UTILS_DEBUG_EXIT();
}


/*
 * Caller needs to hold the allocator lock.
 */
static void
put_page( SCOREP_Allocator_Allocator* allocator,
          SCOREP_Allocator_Page*      page )
{
    uint32_t order   = get_page_order( page );
    uint32_t page_id = get_page_id( page );
    UTILS_DEBUG_ENTRY( "release page=%p, order=%" PRIu32 ", page_id=%" PRIu32 "" );
    if ( order == 1 )
    {
        track_bitset_clear( allocator, page_id );
    }
    else
    {
        track_bitset_clear_range( allocator, page_id, order );
    }

    put_union_object( allocator, page );
    UTILS_DEBUG_EXIT();
}


/*
 * Caller needs to hold the allocator lock.
 */
static SCOREP_Allocator_Page*
get_page( SCOREP_Allocator_Allocator* allocator, uint32_t order )
{
    UTILS_DEBUG_ENTRY();
    uint32_t page_id;

    SCOREP_Allocator_Page* page = get_union_object( allocator );
    if ( !page )
    {
        UTILS_DEBUG_EXIT( "out-of-memory: no free union_object, order=%" PRIu32 "", order );
        return 0;
    }

    if ( order == 1 )
    {
        page_id = track_bitset_find_and_set( allocator );
    }
    else
    {
        page_id = track_bitset_find_and_set_range( allocator, order );
    }

    if ( page_id >= allocator->n_pages_capacity )
    {
        put_union_object( allocator, page );
        UTILS_DEBUG_EXIT( "out-of-memory: no free page(s), order=%" PRIu32 "", order );
        return 0;
    }
    init_page( allocator, page, page_id, order );

    UTILS_DEBUG_EXIT( "new page=%p, order=%" PRIu32 ", page_id=%" PRIu32 "", page, order, page_id );
    return page;
}


static SCOREP_Allocator_Page*
page_manager_get_new_page( SCOREP_Allocator_PageManager* pageManager,
                           uint32_t                      min_page_size )
{
    uint32_t order = get_order( pageManager->allocator, min_page_size );
    UTILS_DEBUG_ENTRY( "min_page_size=%" PRIu32 " -> order=%" PRIu32 "", min_page_size, order );

    lock_allocator( pageManager->allocator );
    SCOREP_Allocator_Page* page = get_page( pageManager->allocator, order );
    unlock_allocator( pageManager->allocator );

    if ( page )
    {
        page->next                     = pageManager->pages_in_use_list;
        pageManager->pages_in_use_list = page;
        UTILS_DEBUG_EXIT( "new page = %p", page );
        return page;
    }
    else
    {
        UTILS_DEBUG_EXIT( "out-of-memory: no free page" );
        return 0;
    }
}


static inline void*
grab_memory( SCOREP_Allocator_Page* page,
             size_t                 requestedSize )
{
    void* memory = page->memory_current_address;
    page->memory_current_address += roundup( requestedSize );
    return memory;
}


static void*
page_manager_alloc( SCOREP_Allocator_PageManager* pageManager,
                    size_t                        requestedSize )
{
    assert( pageManager );
    assert( pageManager->moved_page_id_mapping == 0 );
    assert( requestedSize > 0 );

    /* do not try to allocate more than the allocator has memory */
    if ( requestedSize > total_memory( pageManager->allocator ) )
    {
        UTILS_DEBUG_EXIT( "out-of-memory: requestedSize > total_memory" );
        return 0;
    }

    /* search in all pages for space */
    SCOREP_Allocator_Page* page = pageManager->pages_in_use_list;
    while ( page )
    {
        if ( roundup( requestedSize ) <= get_page_avail( page ) )
        {
            break;
        }
        page = page->next;
    }

    /* no page found, request new one */
    if ( !page )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "requesting new page ..." );
        page = page_manager_get_new_page( pageManager, requestedSize );
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "... got page %p.", page );
    }

    /* still no page, out of memory */
    if ( !page )
    {
        UTILS_DEBUG_EXIT( "out-of-memory: no free page" );
        return 0;
    }

    return grab_memory( page, requestedSize );
}


size_t
SCOREP_Allocator_RoundupToAlignment( size_t size )
{
    return roundup( size );
}


SCOREP_Allocator_Allocator*
SCOREP_Allocator_CreateAllocator( uint32_t*                    totalMemory,
                                  uint32_t*                    pageSize,
                                  SCOREP_Allocator_Guard       lockFunction,
                                  SCOREP_Allocator_Guard       unlockFunction,
                                  SCOREP_Allocator_GuardObject lockObject )
{
    UTILS_DEBUG_ENTRY();
    *pageSize = npot( *pageSize );

    if ( *totalMemory <= *pageSize || *totalMemory == 0 || *pageSize == 0 )
    {
        return 0;
    }
    /* min page size 512? */
    if ( *pageSize / union_size() < MIN_NUMBER_OF_OBJECTS_PER_PAGE )
    {
        return 0;
    }

    uint32_t page_shift = 0;
    while ( ( *pageSize ) >> ( page_shift + 1 ) )
    {
        page_shift++;
    }

    uint32_t n_pages = ( *totalMemory ) / ( *pageSize );
    /* round the total memory down to a multiple of pageSize */
    *totalMemory = n_pages * ( *pageSize );

    uint32_t maint_memory_needed = union_size() + bitset_size( n_pages );
    maint_memory_needed = roundupto( maint_memory_needed, 64 ); // why 64?
    if ( ( *totalMemory ) <= maint_memory_needed )
    {
        /* too few memory to hold maintenance stuff */
        return 0;
    }

    /* mark the pages used we need for our own maintenance
     * (i.e. this object and the page_map)
     */
    uint32_t already_used_pages = maint_memory_needed >> page_shift;
    /* always use one more page for the allocator union object */
    already_used_pages++;
    uint32_t free_memory_in_last_page = ( already_used_pages << page_shift ) - maint_memory_needed;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "0: m=%zu ps=%u np=%zu mm=%zu fm=%zu aup=%zu puor=%f",
                        totalMemory, page_shift, n_pages,
                        maint_memory_needed,
                        free_memory_in_last_page,
                        already_used_pages,
                        ( double )( free_memory_in_last_page / union_size() ) / n_pages );

    /* guarantee at least for .5% of the total number of pages pre-allocated page structs */ // why .5% ?
    while ( ( free_memory_in_last_page / union_size() ) < ( n_pages / 200 ) )
    {
        already_used_pages++;
        free_memory_in_last_page += ( *pageSize );
    }
    if ( already_used_pages >= n_pages )
    {
        return 0;
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "1: m=%zu ps=%u np=%zu mm=%zu fm=%zu aup=%zu puor=%f",
                        totalMemory, page_shift, n_pages,
                        maint_memory_needed,
                        free_memory_in_last_page,
                        already_used_pages,
                        ( double )( free_memory_in_last_page / union_size() ) / n_pages );

    SCOREP_Allocator_Allocator* allocator = calloc( 1, *totalMemory );
    if ( !allocator )
    {
        return 0;
    }
    allocator->page_shift          = page_shift;
    allocator->n_pages_capacity    = n_pages;
    allocator->n_pages_maintenance = already_used_pages;
    allocator->free_objects        = NULL;

    allocator->lock        = null_guard;
    allocator->unlock      = null_guard;
    allocator->lock_object = 0;
    if ( lockFunction && unlockFunction )
    {
        allocator->lock        = lockFunction;
        allocator->unlock      = unlockFunction;
        allocator->lock_object = lockObject;
    }

    bitset_mark_invalid( page_bitset( allocator ), allocator->n_pages_capacity );

    track_bitset_set_range( allocator, 0, allocator->n_pages_maintenance );

    char* start_addr = ( char* )allocator + maint_memory_needed;
    fill_with_union_objects( free_memory_in_last_page, start_addr, allocator );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "\'allocated\' %" PRIu32 " maintenance page(s).",
                        allocator->n_pages_maintenance );

    UTILS_DEBUG_EXIT();
    return allocator;
}


void
SCOREP_Allocator_DeleteAllocator( SCOREP_Allocator_Allocator* allocator )
{
    free( allocator );
}


uint32_t
SCOREP_Allocator_GetMaxNumberOfPages( const SCOREP_Allocator_Allocator* allocator )
{
    return allocator->n_pages_capacity;
}


#define get_page_manager( alloc, pm ) \
    do { \
        assert( alloc ); \
        lock_allocator( alloc ); \
        pm = get_union_object( alloc ); \
        unlock_allocator( alloc ); \
        if ( !pm ) \
        { \
            UTILS_DEBUG_EXIT( "out-of-memory: no union object" ); \
            return 0; \
        } \
        pm->allocator             = alloc; \
        pm->pages_in_use_list     = 0; \
        pm->moved_page_id_mapping = 0; \
        pm->last_allocation       = 0; \
    } while ( 0 )


SCOREP_Allocator_PageManager*
SCOREP_Allocator_CreatePageManager( SCOREP_Allocator_Allocator* allocator )
{
    UTILS_DEBUG_ENTRY();
    SCOREP_Allocator_PageManager* page_manager;
    get_page_manager( allocator, page_manager );

    /* may fail, but maybe we have free pages later */
    page_manager_get_new_page( page_manager, page_size( allocator ) );

    UTILS_DEBUG_EXIT();
    return page_manager;
}


SCOREP_Allocator_PageManager*
SCOREP_Allocator_CreateMovedPageManager( SCOREP_Allocator_Allocator* allocator )
{
    UTILS_DEBUG_ENTRY();
    SCOREP_Allocator_PageManager* page_manager;
    get_page_manager( allocator, page_manager );

    uint32_t order = get_order( allocator,
                                sizeof( *page_manager->moved_page_id_mapping )
                                * allocator->n_pages_capacity );

    lock_allocator( allocator );
    SCOREP_Allocator_Page* page = get_page( allocator, order );
    unlock_allocator( allocator );

    if ( !page )
    {
        UTILS_DEBUG_EXIT( "out-of-memory: no free page" );
        return 0;
    }

    page_manager->moved_page_id_mapping = ( uint32_t* )page->memory_start_address;
    memset( page_manager->moved_page_id_mapping, 0, order << allocator->page_shift );

    UTILS_DEBUG_EXIT();
    return page_manager;
}


#undef get_page_manager


void
SCOREP_Allocator_DeletePageManager( SCOREP_Allocator_PageManager* pageManager )
{
    UTILS_DEBUG_ENTRY();
    assert( pageManager );
    SCOREP_Allocator_Allocator* allocator = pageManager->allocator;

    SCOREP_Allocator_Page* page = pageManager->pages_in_use_list;
    lock_allocator( allocator );
    while ( page )
    {
        SCOREP_Allocator_Page* next_page = page->next;
        put_page( allocator, page );
        page = next_page;
    }

    if ( pageManager->moved_page_id_mapping )
    {
        /* put the page used for the mapping */
        intptr_t offset  = ( char* )pageManager->moved_page_id_mapping - ( char* )allocator;
        uint32_t page_id = offset >> allocator->page_shift;
        uint32_t order   = get_order( allocator,
                                      sizeof( *pageManager->moved_page_id_mapping )
                                      * allocator->n_pages_capacity );
        if ( order == 1 )
        {
            track_bitset_clear( allocator, page_id );
        }
        else
        {
            track_bitset_clear_range( allocator, page_id, order );
        }
    }

    put_union_object( allocator, pageManager );

    unlock_allocator( allocator );
    UTILS_DEBUG_EXIT();
}


void*
SCOREP_Allocator_Alloc( SCOREP_Allocator_PageManager* pageManager,
                        size_t                        memorySize )
{
    return page_manager_alloc( pageManager, memorySize );
}


void
SCOREP_Allocator_Free( SCOREP_Allocator_PageManager* pageManager )
{
    UTILS_DEBUG_ENTRY();
    assert( pageManager );
    assert( pageManager->allocator );

    lock_allocator( pageManager->allocator );
    while ( pageManager->pages_in_use_list )
    {
        SCOREP_Allocator_Page* next_page = pageManager->pages_in_use_list->next;
        put_page( pageManager->allocator, pageManager->pages_in_use_list );
        pageManager->pages_in_use_list = next_page;
    }
    unlock_allocator( pageManager->allocator );

    if ( pageManager->moved_page_id_mapping )
    {
        memset( pageManager->moved_page_id_mapping,
                0,
                sizeof( *pageManager->moved_page_id_mapping )
                * pageManager->allocator->n_pages_capacity );
    }

    pageManager->last_allocation = 0;
    UTILS_DEBUG_EXIT();
}


SCOREP_Allocator_MovableMemory
SCOREP_Allocator_AllocMovable( SCOREP_Allocator_PageManager* pageManager,
                               size_t                        memorySize )
{
    /// @todo padding?
    void* memory = page_manager_alloc( pageManager, memorySize );
    if ( !memory )
    {
        UTILS_DEBUG_EXIT( "out-of-memory" );
        return 0;
    }

    pageManager->last_allocation = ( char* )memory - ( char* )pageManager->allocator;
    return pageManager->last_allocation;
}


void*
SCOREP_Allocator_AllocMovedPage( SCOREP_Allocator_PageManager* movedPageManager,
                                 uint32_t                      moved_page_id,
                                 uint32_t                      page_usage )
{
    UTILS_DEBUG_ENTRY();
    assert( movedPageManager );
    assert( movedPageManager->moved_page_id_mapping != 0 );
    assert( moved_page_id != 0 );
    assert( moved_page_id < movedPageManager->allocator->n_pages_capacity );
    assert( movedPageManager->moved_page_id_mapping[ moved_page_id ] == 0 );

    SCOREP_Allocator_Page* page = page_manager_get_new_page( movedPageManager,
                                                             page_usage );
    if ( !page )
    {
        UTILS_DEBUG_EXIT( "out-of-memory: no free page" );
        return 0;
    }

    uint32_t local_page_id = get_page_id( page );
    uint32_t order         = get_page_order( page );
    while ( order )
    {
        movedPageManager->moved_page_id_mapping[ moved_page_id++ ] =
            local_page_id++;
        order--;
    }
    set_page_usage( page, page_usage );

    UTILS_DEBUG_EXIT();
    return page->memory_start_address;
}


void*
SCOREP_Allocator_GetAddressFromMovableMemory(
    const SCOREP_Allocator_PageManager* pageManager,
    SCOREP_Allocator_MovableMemory      movableMemory )
{
    assert( pageManager );
    assert( movableMemory >= page_size( pageManager->allocator ) );
    assert( movableMemory < total_memory( pageManager->allocator ) );

    if ( pageManager->moved_page_id_mapping )
    {
        uint32_t page_id     = movableMemory >> pageManager->allocator->page_shift;
        uint32_t page_offset = movableMemory & page_mask( pageManager->allocator );
        assert( pageManager->moved_page_id_mapping[ page_id ] != 0 );
        page_id       = pageManager->moved_page_id_mapping[ page_id ];
        movableMemory = ( page_id << pageManager->allocator->page_shift )
                        | page_offset;
    }

    return ( char* )pageManager->allocator + movableMemory;
}


void
SCOREP_Allocator_RollbackAllocMovable( SCOREP_Allocator_PageManager*  pageManager,
                                       SCOREP_Allocator_MovableMemory movableMemory )
{
    assert( pageManager );
    assert( !pageManager->moved_page_id_mapping );
    assert( movableMemory >= page_size( pageManager->allocator ) );
    assert( pageManager->last_allocation == movableMemory );

    char* memory = SCOREP_Allocator_GetAddressFromMovableMemory( pageManager,
                                                                 movableMemory );
    SCOREP_Allocator_Page* page = pageManager->pages_in_use_list;
    while ( page )
    {
        if ( page->memory_start_address <= memory
             && memory < page->memory_current_address )
        {
            page->memory_current_address = memory;
            pageManager->last_allocation = 0;
            return;
        }
        page = page->next;
    }
    assert( page );
}


uint32_t
SCOREP_Allocator_GetNumberOfUsedPages( const SCOREP_Allocator_PageManager* pageManager )
{
    assert( pageManager );

    uint32_t                     number_of_used_pages = 0;
    const SCOREP_Allocator_Page* page                 = pageManager->pages_in_use_list;
    while ( page )
    {
        if ( get_page_usage( page ) )
        {
            number_of_used_pages++;
        }
        page = page->next;
    }

    return number_of_used_pages;
}


void
SCOREP_Allocator_GetPageInfos( const SCOREP_Allocator_PageManager* pageManager,
                               uint32_t*                           pageIds,
                               uint32_t*                           pageUsages,
                               void** const                        pageStarts )
{
    assert( pageManager );
    assert( pageIds );

    const SCOREP_Allocator_Page* page     = pageManager->pages_in_use_list;
    uint32_t                     position = 0;
    while ( page )
    {
        uint32_t usage = get_page_usage( page );
        if ( usage )
        {
            /* We expose only non-zero page ids, to let 0 be the invalid pointer */
            pageIds[ position ] = get_page_id( page );
            if ( pageUsages )
            {
                pageUsages[ position ] = usage;
            }
            if ( pageStarts )
            {
                pageStarts[ position ] = page->memory_start_address;
            }

            position++;
        }

        page = page->next;
    }
}


void
SCOREP_Allocator_GetPageStats( SCOREP_Allocator_Allocator*        allocator,
                               SCOREP_Allocator_PageManagerStats* stats )
{
    lock_allocator( allocator );
    stats->pages_allocated = allocator->n_pages_high_watermark;
    stats->pages_used      = allocator->n_pages_allocated;
    unlock_allocator( allocator );
}


void
SCOREP_Allocator_GetPageManagerStats( const SCOREP_Allocator_PageManager* pageManager,
                                      const SCOREP_Allocator_Allocator*   allocator,
                                      SCOREP_Allocator_PageManagerStats*  stats )
{
    assert( stats );

    if ( pageManager )
    {
        assert( allocator == 0 );
        const SCOREP_Allocator_Page* page = pageManager->pages_in_use_list;
        while ( page )
        {
            uint32_t page_multiple = get_order( page->allocator, get_page_length( page ) );
            assert( page_multiple > 0 );
            stats->pages_allocated  += page_multiple;
            stats->memory_allocated += get_page_length( page );
            uint32_t usage = get_page_usage( page );
            stats->memory_used      += usage;
            stats->memory_available += get_page_avail( page );
            if ( usage )
            {
                stats->pages_used += page_multiple;
            }
            page = page->next;
        }

        if ( pageManager->moved_page_id_mapping ) /* moved page manager */
        {
            uint32_t order = get_order( pageManager->allocator,
                                        sizeof( *pageManager->moved_page_id_mapping )
                                        * pageManager->allocator->n_pages_capacity );
            stats->pages_allocated += order;
        }
    }
    else /* maintenance pages */
    {
        assert( allocator );
        stats->pages_allocated  = allocator->n_pages_maintenance;
        stats->pages_used       = stats->pages_allocated;
        stats->memory_allocated = stats->pages_allocated * page_size( allocator );
        SCOREP_Allocator_Object* free_obj = allocator->free_objects;
        while ( free_obj )
        {
            stats->memory_available += union_size();
            free_obj                 = free_obj->next;
        }
        stats->memory_used = stats->memory_allocated - stats->memory_available;
    }
}
