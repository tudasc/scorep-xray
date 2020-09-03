/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014, 2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2017, 2019-2020,
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
#include <SCOREP_Allocator.h>
#include "scorep_allocator.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define SCOREP_DEBUG_MODULE_NAME ALLOCATOR
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#define roundup( x ) roundupto( x, SCOREP_ALLOCATOR_ALIGNMENT )

#include "scorep_bitset.h"
#include "scorep_page.h"

/* 8 objects per page should be minimum to be efficient */
#define MIN_NUMBER_OF_OBJECTS_PER_PAGE 8

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
track_bitset_clear( SCOREP_Allocator_Allocator* allocator,
                    uint32_t                    pos )
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
fill_with_union_objects( SCOREP_Allocator_Allocator* allocator,
                         uint32_t                    freeMemory,
                         char*                       startAddr )
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
        fill_with_union_objects( allocator, free_memory, start_addr );
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
put_union_object( SCOREP_Allocator_Allocator* allocator,
                  void*                       objectPtr )
{
    UTILS_DEBUG_ENTRY();
    SCOREP_Allocator_Object* object = objectPtr;
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
get_page( SCOREP_Allocator_Allocator* allocator,
          uint32_t                    order )
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
                           uint32_t                      minPageSize )
{
    uint32_t order = get_order( pageManager->allocator, minPageSize );
    UTILS_DEBUG_ENTRY( "minPageSize=%" PRIu32 " -> order=%" PRIu32 "", minPageSize, order );

    lock_allocator( pageManager->allocator );
    SCOREP_Allocator_Page* page = get_page( pageManager->allocator, order );
    unlock_allocator( pageManager->allocator );

    if ( !page )
    {
        UTILS_DEBUG_EXIT( "out-of-memory: no free page" );
        return 0;
    }

    page->next                     = pageManager->pages_in_use_list;
    pageManager->pages_in_use_list = page;

    UTILS_DEBUG_EXIT( "new page = %p", page );
    return page;
}


/** @a alignment needs to be a power-of-two */
static void*
page_manager_alloc( SCOREP_Allocator_PageManager* pageManager,
                    size_t                        requestedSize,
                    size_t                        alignment )
{
    assert( pageManager );
    assert( pageManager->moved_page_id_mapping_page == 0 );
    assert( requestedSize > 0 );
    assert( alignment <= page_size( pageManager->allocator ) );

    /* do not try to allocate more than the allocator has memory */
    if ( requestedSize > total_memory( pageManager->allocator ) )
    {
        UTILS_DEBUG_EXIT( "out-of-memory: requestedSize > total_memory" );
        return 0;
    }

    /* search in all pages for space */
    SCOREP_Allocator_Page* page   = pageManager->pages_in_use_list;
    void*                  memory = NULL;
    while ( page )
    {
        if ( grab_page_memory( page, requestedSize, alignment, &memory ) )
        {
            return memory;
        }
        page = page->next;
    }

    /* no page found, request new one */
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "requesting new page ..." );
    page = page_manager_get_new_page( pageManager, requestedSize );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "... got page %p.", page );

    /* still no page, out of memory */
    if ( !page )
    {
        UTILS_DEBUG_EXIT( "out-of-memory: no free page" );
        return 0;
    }

    /* memory stays NULL, if page cannot satisfy request */
    grab_page_memory( page, requestedSize, alignment, &memory );
    return memory;
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

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "0: m=%u p=%u",
                        *totalMemory, *pageSize );

    if ( *totalMemory <= *pageSize || *totalMemory == 0 || *pageSize == 0 || *pageSize < SCOREP_ALLOCATOR_ALIGNMENT )
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

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "1: m=%u p=%u ps=%u",
                        *totalMemory, *pageSize,
                        page_shift );

    uint32_t n_pages = ( *totalMemory ) / ( *pageSize );
    /* round the total memory down to a multiple of pageSize */
    *totalMemory = n_pages * ( *pageSize );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "2: m=%u p=%u ps=%u np=%u",
                        *totalMemory, *pageSize,
                        page_shift, n_pages );

    uint32_t maint_memory_needed = union_size() + bitset_size( n_pages );
    maint_memory_needed = roundupto( maint_memory_needed, 64 ); // why 64?
    if ( ( *totalMemory ) <= maint_memory_needed )
    {
        /* too few memory to hold maintenance stuff */
        return 0;
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "3: m=%u p=%u ps=%u np=%u mm=%u",
                        *totalMemory, *pageSize,
                        page_shift, n_pages,
                        maint_memory_needed );

    /* determine the pages used we need for our own maintenance
     * (i.e., this object and the page_map plus the union-object pool)
     */
    uint32_t already_used_pages = ( maint_memory_needed >> page_shift ) +
                                  !!( maint_memory_needed & ( *pageSize - 1 ) );
    uint32_t free_memory_in_last_page = ( already_used_pages << page_shift ) - maint_memory_needed;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "4: m=%u p=%u ps=%u np=%u mm=%u fm=%u aup=%u puor=%f",
                        *totalMemory, *pageSize,
                        page_shift, n_pages,
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
    /* we may loose one page because of alignment */
    if ( already_used_pages >= ( n_pages - 1 ) )
    {
        return 0;
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "5: m=%u p=%u ps=%u np=%u mm=%u fm=%u aup=%u puor=%f",
                        *totalMemory, *pageSize,
                        page_shift, n_pages,
                        maint_memory_needed,
                        free_memory_in_last_page,
                        already_used_pages,
                        ( double )( free_memory_in_last_page / union_size() ) / n_pages );

    void* raw = calloc( 1, *totalMemory );
    if ( !raw )
    {
        return 0;
    }
    SCOREP_Allocator_Allocator* allocator = ( void* )roundupto( raw, *pageSize );
    allocator->allocated_memory = raw;
    allocator->page_shift       = page_shift;
    allocator->n_pages_capacity = n_pages;
    if ( allocator != allocator->allocated_memory )
    {
        /* we already ensured that we can loose one page */
        allocator->n_pages_capacity--;
    }
    allocator->n_pages_maintenance = already_used_pages;
    allocator->free_objects        = NULL;

    /* announce the final usable total memory back to the caller */
    *totalMemory = allocator->n_pages_capacity << page_shift;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "6: m=%u p=%u ps=%u np=%u mm=%u fm=%u aup=%u",
                        *totalMemory, *pageSize,
                        page_shift, allocator->n_pages_capacity,
                        maint_memory_needed,
                        free_memory_in_last_page,
                        allocator->n_pages_maintenance );

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
    fill_with_union_objects( allocator, free_memory_in_last_page, start_addr );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_ALLOCATOR, "'allocated' %" PRIu32 " maintenance page(s).",
                        allocator->n_pages_maintenance );

    UTILS_DEBUG_EXIT();
    return allocator;
}


void
SCOREP_Allocator_DeleteAllocator( SCOREP_Allocator_Allocator* allocator )
{
    if ( allocator )
    {
        free( allocator->allocated_memory );
    }
}


uint32_t
SCOREP_Allocator_GetMaxNumberOfPages( const SCOREP_Allocator_Allocator* allocator )
{
    return allocator->n_pages_capacity;
}


static inline SCOREP_Allocator_PageManager*
get_page_manager( SCOREP_Allocator_Allocator* allocator )
{
    lock_allocator( allocator );
    SCOREP_Allocator_PageManager* page_manager = get_union_object( allocator );
    unlock_allocator( allocator );

    if ( !page_manager )
    {
        return 0;
    }

    page_manager->allocator                  = allocator;
    page_manager->pages_in_use_list          = 0;
    page_manager->moved_page_id_mapping_page = 0;
    page_manager->last_allocation            = 0;

    return page_manager;
}


SCOREP_Allocator_PageManager*
SCOREP_Allocator_CreatePageManager( SCOREP_Allocator_Allocator* allocator )
{
    UTILS_DEBUG_ENTRY();
    assert( allocator );

    SCOREP_Allocator_PageManager* page_manager = get_page_manager( allocator );
    if ( !page_manager )
    {
        UTILS_DEBUG_EXIT( "out-of-memory: no union object" );
        return 0;
    }

    /* may fail, but maybe we have free pages later */
    page_manager_get_new_page( page_manager, page_size( allocator ) );

    UTILS_DEBUG_EXIT();
    return page_manager;
}


SCOREP_Allocator_PageManager*
SCOREP_Allocator_CreateMovedPageManager( SCOREP_Allocator_Allocator* allocator )
{
    UTILS_DEBUG_ENTRY();
    assert( allocator );

    SCOREP_Allocator_PageManager* page_manager = get_page_manager( allocator );
    if ( !page_manager )
    {
        UTILS_DEBUG_EXIT( "out-of-memory: no union object" );
        return 0;
    }

    size_t   mapping_size = sizeof( uint32_t ) * allocator->n_pages_capacity;
    uint32_t order        = get_order( allocator, mapping_size );

    lock_allocator( allocator );
    page_manager->moved_page_id_mapping_page = get_page( allocator, order );
    if ( !page_manager->moved_page_id_mapping_page )
    {
        put_union_object( allocator, page_manager );
        unlock_allocator( allocator );

        UTILS_DEBUG_EXIT( "out-of-memory: no free page" );
        return 0;
    }
    unlock_allocator( allocator );

    /* Allocate what we need for the mapping, should not fail */
    void* mapping;
    grab_page_memory( page_manager->moved_page_id_mapping_page,
                      mapping_size, SCOREP_ALLOCATOR_ALIGNMENT, &mapping );
    memset( mapping, 0, mapping_size );

    UTILS_DEBUG_EXIT();
    return page_manager;
}


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

    if ( pageManager->moved_page_id_mapping_page )
    {
        put_page( allocator, pageManager->moved_page_id_mapping_page );
    }

    put_union_object( allocator, pageManager );

    unlock_allocator( allocator );
    UTILS_DEBUG_EXIT();
}


void*
SCOREP_Allocator_Alloc( SCOREP_Allocator_PageManager* pageManager,
                        size_t                        memorySize )
{
    return page_manager_alloc( pageManager, memorySize, SCOREP_ALLOCATOR_ALIGNMENT );
}


void*
SCOREP_Allocator_AlignedAlloc( SCOREP_Allocator_PageManager* pageManager,
                               size_t                        alignment,
                               size_t                        memorySize )
{
    assert( alignment >= SCOREP_ALLOCATOR_ALIGNMENT );
    assert( ( alignment & ( alignment - 1 ) ) == 0 ); /* power of two */

    return page_manager_alloc( pageManager, memorySize, alignment );
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

    if ( pageManager->moved_page_id_mapping_page )
    {
        memset( pageManager->moved_page_id_mapping_page->memory_start_address,
                0, get_page_usage( pageManager->moved_page_id_mapping_page ) );
    }

    pageManager->last_allocation = 0;
    UTILS_DEBUG_EXIT();
}


SCOREP_Allocator_MovableMemory
SCOREP_Allocator_AllocMovable( SCOREP_Allocator_PageManager* pageManager,
                               size_t                        memorySize )
{
    /// @todo padding?
    void* memory = page_manager_alloc( pageManager, memorySize, SCOREP_ALLOCATOR_ALIGNMENT );
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
                                 uint32_t                      movedPageId,
                                 uint32_t                      pageUsage )
{
    UTILS_DEBUG_ENTRY();
    assert( movedPageManager );
    assert( movedPageManager->moved_page_id_mapping_page != 0 );
    assert( movedPageId != 0 );
    assert( movedPageId < movedPageManager->allocator->n_pages_capacity );

    uint32_t* moved_page_id_mapping =
        ( uint32_t* )movedPageManager->moved_page_id_mapping_page->memory_start_address;
    assert( moved_page_id_mapping[ movedPageId ] == 0 );

    SCOREP_Allocator_Page* page = page_manager_get_new_page( movedPageManager,
                                                             pageUsage );
    if ( !page )
    {
        UTILS_DEBUG_EXIT( "out-of-memory: no free page" );
        return 0;
    }

    uint32_t local_page_id = get_page_id( page );
    uint32_t order         = get_page_order( page );
    while ( order )
    {
        moved_page_id_mapping[ movedPageId++ ] = local_page_id++;
        order--;
    }
    set_page_usage( page, pageUsage );

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

    if ( pageManager->moved_page_id_mapping_page )
    {
        uint32_t* moved_page_id_mapping =
            ( uint32_t* )pageManager->moved_page_id_mapping_page->memory_start_address;
        uint32_t page_id     = movableMemory >> pageManager->allocator->page_shift;
        uint32_t page_offset = movableMemory & page_mask( pageManager->allocator );
        assert( moved_page_id_mapping[ page_id ] != 0 );
        page_id       = moved_page_id_mapping[ page_id ];
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
    assert( !pageManager->moved_page_id_mapping_page );
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
            /* This may leak memory due to alignment */
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
SCOREP_Allocator_GetStats( SCOREP_Allocator_Allocator*        allocator,
                           SCOREP_Allocator_PageManagerStats* pageStats,
                           SCOREP_Allocator_PageManagerStats* maintStats )
{
    assert( allocator );
    assert( pageStats );
    assert( maintStats );

    lock_allocator( allocator );

    pageStats->pages_allocated = allocator->n_pages_high_watermark;
    pageStats->pages_used      = allocator->n_pages_allocated;

    maintStats->pages_allocated       = allocator->n_pages_maintenance;
    maintStats->pages_used            = maintStats->pages_allocated;
    maintStats->memory_allocated      = maintStats->pages_allocated * page_size( allocator );
    maintStats->memory_alignment_loss = ( char* )allocator - ( char* )allocator->allocated_memory;
    SCOREP_Allocator_Object* free_obj = allocator->free_objects;
    while ( free_obj )
    {
        maintStats->memory_available += union_size();
        free_obj                      = free_obj->next;
    }
    maintStats->memory_used = maintStats->memory_allocated - maintStats->memory_available;

    unlock_allocator( allocator );
}


static inline void
update_page_stats( const SCOREP_Allocator_Page*       page,
                   SCOREP_Allocator_PageManagerStats* stats )
{
    assert( page );
    assert( stats );

    uint32_t page_multiple = get_order( page->allocator, get_page_length( page ) );
    assert( page_multiple > 0 );
    stats->pages_allocated  += page_multiple;
    stats->memory_allocated += get_page_length( page );
    uint32_t usage = get_page_usage( page );
    stats->memory_used           += usage;
    stats->memory_available      += get_page_avail( page );
    stats->memory_alignment_loss += page->memory_alignment_loss;
    if ( usage )
    {
        stats->pages_used += page_multiple;
    }
}


void
SCOREP_Allocator_GetPageManagerStats( SCOREP_Allocator_PageManager*      pageManager,
                                      SCOREP_Allocator_PageManagerStats* stats )
{
    assert( stats );
    assert( pageManager );

    lock_allocator( pageManager->allocator );

    const SCOREP_Allocator_Page* page = pageManager->pages_in_use_list;
    while ( page )
    {
        update_page_stats( page, stats );
        page = page->next;
    }

    if ( pageManager->moved_page_id_mapping_page ) /* moved page manager */
    {
        update_page_stats( pageManager->moved_page_id_mapping_page, stats );
    }

    unlock_allocator( pageManager->allocator );
}
