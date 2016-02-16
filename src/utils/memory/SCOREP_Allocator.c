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
 * Copyright (c) 2009-2011,
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
 * @file SCOREP_Allocator.c Implementation of the paged memory allocator.
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

#include <UTILS_Debug.h>

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


/*
 * Caller needs to hold the allocator lock.
 */
static void*
get_union_object( SCOREP_Allocator_Allocator* allocator )
{
    if ( !allocator->free_objects )
    {
        uint32_t page_id = bitset_find_and_set( page_bitset( allocator ),
                                                allocator->n_pages );
        if ( page_id >= allocator->n_pages )
        {
            return NULL;
        }
        char*    start       = ( char* )allocator + ( page_id << allocator->page_shift );
        uint32_t free_memory = page_size( allocator );
        while ( free_memory > union_size() )
        {
            SCOREP_Allocator_Object* object = ( SCOREP_Allocator_Object* )start;
            object->next            = allocator->free_objects;
            allocator->free_objects = object;
            start                  += union_size();
            free_memory            -= union_size();
        }
    }

    SCOREP_Allocator_Object* object = allocator->free_objects;
    allocator->free_objects = object->next;
    object->next            = NULL;

    return object;
}


/*
 * Caller needs to hold the allocator lock.
 */
static void
put_union_object( SCOREP_Allocator_Allocator* allocator, void* object_ptr )
{
    SCOREP_Allocator_Object* object = object_ptr;
    object->next            = allocator->free_objects;
    allocator->free_objects = object;
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

    if ( order == 1 )
    {
        bitset_clear( page_bitset( allocator ),
                      allocator->n_pages,
                      page_id );
    }
    else
    {
        bitset_clear_range( page_bitset( allocator ),
                            allocator->n_pages,
                            page_id,
                            order );
    }

    put_union_object( allocator, page );
}


/*
 * Caller needs to hold the allocator lock.
 */
static SCOREP_Allocator_Page*
get_page( SCOREP_Allocator_Allocator* allocator, uint32_t order )
{
    uint32_t page_id;

    SCOREP_Allocator_Page* page = get_union_object( allocator );
    if ( !page )
    {
        return 0;
    }

    if ( order == 1 )
    {
        page_id = bitset_find_and_set( page_bitset( allocator ),
                                       allocator->n_pages );
    }
    else
    {
        page_id = bitset_find_and_set_range( page_bitset( allocator ),
                                             allocator->n_pages,
                                             order );
    }

    if ( page_id >= allocator->n_pages )
    {
        put_union_object( allocator, page );
        return 0;
    }
    init_page( allocator, page, page_id, order );

    return page;
}


static SCOREP_Allocator_Page*
page_manager_get_new_page( SCOREP_Allocator_PageManager* pageManager,
                           uint32_t                      min_page_size )
{
    uint32_t order = get_order( pageManager->allocator, min_page_size );

    lock_allocator( pageManager->allocator );
    SCOREP_Allocator_Page* page = get_page( pageManager->allocator, order );
    unlock_allocator( pageManager->allocator );

    if ( page )
    {
        page->next                     = pageManager->pages_in_use_list;
        pageManager->pages_in_use_list = page;
    }

    return page;
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
    if ( requestedSize == 0 )
    {
        return 0;
    }

    /* do not try to allocate more than the allocator has memory */
    if ( requestedSize > total_memory( pageManager->allocator ) )
    {
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
        page = page_manager_get_new_page( pageManager, requestedSize );
    }

    /* still no page, out of memory */
    if ( !page )
    {
        return NULL;
    }

    return grab_memory( page, requestedSize );
}


/* put all objects from the page into the free list */
static void
slurp_page( SCOREP_Allocator_ObjectManager* objectManager,
            SCOREP_Allocator_Page*          page )
{
    uint32_t number_of_objects_per_page = get_page_length( page )
                                          / objectManager->object_size;

    for ( uint32_t i = 0;
          i < number_of_objects_per_page;
          i++, page->memory_current_address += objectManager->object_size )
    {
        void** object = ( void* )page->memory_current_address;
        *object                  = objectManager->free_list;
        objectManager->free_list = object;
    }
}


static void
object_manager_get_new_page( SCOREP_Allocator_ObjectManager* objectManager )
{
    uint32_t order = get_order( objectManager->allocator,
                                MIN_NUMBER_OF_OBJECTS_PER_PAGE * objectManager->object_size );

    lock_allocator( objectManager->allocator );
    SCOREP_Allocator_Page* page = get_page( objectManager->allocator, order );
    unlock_allocator( objectManager->allocator );

    if ( !page )
    {
        return;
    }
    page->next                       = objectManager->pages_in_use_list;
    objectManager->pages_in_use_list = page;

    slurp_page( objectManager, page );
}


size_t
SCOREP_Allocator_RoundupToAlignment( size_t size )
{
    return roundup( size );
}


SCOREP_Allocator_Allocator*
SCOREP_Allocator_CreateAllocator( uint32_t                     totalMemory,
                                  uint32_t                     pageSize,
                                  SCOREP_Allocator_Guard       lockFunction,
                                  SCOREP_Allocator_Guard       unlockFunction,
                                  SCOREP_Allocator_GuardObject lockObject )
{
    pageSize = npot( pageSize );

    if ( totalMemory <= pageSize || totalMemory == 0 || pageSize == 0 )
    {
        return 0;
    }
    /* min page size 512? */
    if ( pageSize / union_size() < MIN_NUMBER_OF_OBJECTS_PER_PAGE )
    {
        return 0;
    }

    uint32_t page_shift = 0;
    while ( pageSize >> ( page_shift + 1 ) )
    {
        page_shift++;
    }

    uint32_t n_pages = totalMemory / pageSize;
    /* round the total memory down to a multiple of pageSize */
    totalMemory = n_pages * pageSize;

    uint32_t maint_memory_needed = union_size() + bitset_size( n_pages );
    maint_memory_needed = roundupto( maint_memory_needed, 64 );
    if ( totalMemory <= maint_memory_needed )
    {
        /* too few memory to hold maintainance stuff */
        return 0;
    }

    /* mark the pages used we need for our own maintanance
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

    /* guaranty at least for .5% of the total number of pages pre-allocated page structs */
    while ( ( free_memory_in_last_page / union_size() ) < ( n_pages / 200 ) )
    {
        already_used_pages++;
        free_memory_in_last_page += pageSize;
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

    SCOREP_Allocator_Allocator* allocator = calloc( 1, totalMemory );
    if ( !allocator )
    {
        return 0;
    }
    allocator->page_shift   = page_shift;
    allocator->n_pages      = n_pages;
    allocator->free_objects = NULL;

    allocator->lock        = null_guard;
    allocator->unlock      = null_guard;
    allocator->lock_object = 0;
    if ( lockFunction && unlockFunction )
    {
        allocator->lock        = lockFunction;
        allocator->unlock      = unlockFunction;
        allocator->lock_object = lockObject;
    }

    bitset_mark_invalid( page_bitset( allocator ), allocator->n_pages );

    bitset_set_range( page_bitset( allocator ),
                      allocator->n_pages,
                      0,
                      already_used_pages );

    /*
     * slurp the reminding memory of the already used pages to have a pool
     * of pre-allocated union objects
     */
    char* start = ( char* )allocator + maint_memory_needed;
    while ( free_memory_in_last_page >= union_size() )
    {
        SCOREP_Allocator_Object* object = ( SCOREP_Allocator_Object* )start;
        object->next              = allocator->free_objects;
        allocator->free_objects   = object;
        start                    += union_size();
        free_memory_in_last_page -= union_size();
    }

    return allocator;
}


void
SCOREP_Allocator_DeleteAllocator( SCOREP_Allocator_Allocator* allocator )
{
    free( allocator );
}


SCOREP_Allocator_PageManager*
SCOREP_Allocator_CreatePageManager( SCOREP_Allocator_Allocator* allocator )
{
    assert( allocator );

    lock_allocator( allocator );
    SCOREP_Allocator_PageManager* page_manager = get_union_object( allocator );
    unlock_allocator( allocator );

    if ( !page_manager )
    {
        return 0;
    }

    page_manager->allocator             = allocator;
    page_manager->pages_in_use_list     = 0;
    page_manager->moved_page_id_mapping = 0;
    page_manager->last_allocation       = 0;

    /* may fail, but maybe we have free pages later */
    page_manager_get_new_page( page_manager, page_size( allocator ) );

    return page_manager;
}


SCOREP_Allocator_PageManager*
SCOREP_Allocator_CreateMovedPageManager( SCOREP_Allocator_Allocator* allocator )
{
    assert( allocator );

    lock_allocator( allocator );
    SCOREP_Allocator_PageManager* page_manager = get_union_object( allocator );
    unlock_allocator( allocator );

    if ( !page_manager )
    {
        return 0;
    }

    page_manager->allocator             = allocator;
    page_manager->pages_in_use_list     = 0;
    page_manager->moved_page_id_mapping = 0;
    page_manager->last_allocation       = 0;

    uint32_t order = get_order( allocator,
                                sizeof( *page_manager->moved_page_id_mapping )
                                * allocator->n_pages );

    lock_allocator( allocator );
    SCOREP_Allocator_Page* page = get_page( allocator, order );
    unlock_allocator( allocator );

    page_manager->moved_page_id_mapping = ( uint32_t* )page->memory_start_address;
    memset( page_manager->moved_page_id_mapping, 0, order << allocator->page_shift );

    return page_manager;
}


void
SCOREP_Allocator_DeletePageManager( SCOREP_Allocator_PageManager* pageManager )
{
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
                                      * allocator->n_pages );
        if ( order == 1 )
        {
            bitset_clear( page_bitset( allocator ),
                          allocator->n_pages,
                          page_id );
        }
        else
        {
            bitset_clear_range( page_bitset( allocator ),
                                allocator->n_pages,
                                page_id,
                                order );
        }
    }

    put_union_object( allocator, pageManager );

    unlock_allocator( allocator );
}


void*
SCOREP_Allocator_Alloc( SCOREP_Allocator_PageManager* pageManager,
                        size_t                        memorySize )
{
    assert( pageManager );
    assert( pageManager->moved_page_id_mapping == 0 );

    if ( memorySize == 0 )
    {
        return 0;
    }

    return page_manager_alloc( pageManager, memorySize );
}


void
SCOREP_Allocator_Free( SCOREP_Allocator_PageManager* pageManager )
{
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
                * pageManager->allocator->n_pages );
    }

    pageManager->last_allocation = 0;
}


SCOREP_Allocator_MovableMemory
SCOREP_Allocator_AllocMovable( SCOREP_Allocator_PageManager* pageManager,
                               size_t                        memorySize )
{
    assert( pageManager );
    assert( pageManager->moved_page_id_mapping == 0 );

    if ( memorySize == 0 )
    {
        return 0;
    }

    /// @todo padding?
    void* memory = page_manager_alloc( pageManager, memorySize );
    if ( !memory )
    {
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
    assert( movedPageManager );
    assert( movedPageManager->moved_page_id_mapping != 0 );
    assert( moved_page_id != 0 );
    assert( moved_page_id < movedPageManager->allocator->n_pages );
    assert( movedPageManager->moved_page_id_mapping[ moved_page_id ] == 0 );

    SCOREP_Allocator_Page* page = page_manager_get_new_page( movedPageManager,
                                                             page_usage );
    if ( !page )
    {
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


void*
SCOREP_Allocator_GetAddressFromMovedMemory(
    const SCOREP_Allocator_PageManager* movedPageManager,
    SCOREP_Allocator_MovableMemory      movedMemory )
{
    assert( movedPageManager );
    assert( movedPageManager->moved_page_id_mapping );
    return SCOREP_Allocator_GetAddressFromMovableMemory( movedPageManager, movedMemory );
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


SCOREP_Allocator_Page*
SCOREP_Allocator_AcquirePage( SCOREP_Allocator_Allocator* allocator )
{
    assert( allocator );

    lock_allocator( allocator );
    SCOREP_Allocator_Page* page = get_page( allocator, 1 );
    unlock_allocator( allocator );

    if ( !page )
    {
        return 0;
    }

    // to be on the safe side, ensure that page is "full" in order to not being
    // used in alloc calls (using entire pages and "small allocations" together
    // might not be a good idea. With getting pages from allocators and small
    // amount of memory via page managers this should not be possible though.)
    page->memory_current_address = page->memory_end_address;

    return page;
}


void
SCOREP_Allocator_ReleasePage( SCOREP_Allocator_Page* page )
{
    assert( page );

    SCOREP_Allocator_Allocator* allocator = page->allocator;

    lock_allocator( allocator );
    put_page( allocator, page );
    unlock_allocator( allocator );
}


void*
SCOREP_Allocator_GetStartAddressFromPage( SCOREP_Allocator_Page* page )
{
    assert( page );
    return page->memory_start_address;
}


void*
SCOREP_Allocator_GetEndAddressFromPage( SCOREP_Allocator_Page* page )
{
    assert( page );
    return page->memory_end_address;
}

SCOREP_Allocator_ObjectManager*
SCOREP_Allocator_CreateObjectManager( SCOREP_Allocator_Allocator* allocator,
                                      size_t                      objectSize )
{
    assert( allocator && objectSize );

    lock_allocator( allocator );
    SCOREP_Allocator_ObjectManager* object_manager = get_union_object( allocator );
    unlock_allocator( allocator );

    if ( !object_manager )
    {
        return 0;
    }

    object_manager->allocator         = allocator;
    object_manager->pages_in_use_list = NULL;
    object_manager->free_list         = NULL;
    object_manager->object_size       = roundup( objectSize );
    /* we need to store a pointer in the object, when it is in the free list */
    assert( object_manager->object_size >= sizeof( void* ) );

    /* may fail, but maybe there are free pages, when the first real request comes */
    object_manager_get_new_page( object_manager );

    return object_manager;
}

void
SCOREP_Allocator_DeleteObjectManager( SCOREP_Allocator_ObjectManager* objectManager )
{
    assert( objectManager );

    SCOREP_Allocator_Allocator* allocator = objectManager->allocator;

    lock_allocator( allocator );
    SCOREP_Allocator_Page* page = objectManager->pages_in_use_list;
    while ( page )
    {
        SCOREP_Allocator_Page* next_page = page->next;
        put_page( allocator, page );
        page = next_page;
    }
    put_union_object( allocator, objectManager );
    unlock_allocator( allocator );
}

void*
SCOREP_Allocator_GetObject( SCOREP_Allocator_ObjectManager* objectManager )
{
    assert( objectManager );

    if ( !objectManager->free_list )
    {
        object_manager_get_new_page( objectManager );
    }

    /* still no free objects */
    if ( !objectManager->free_list )
    {
        return NULL;
    }

    void** object = objectManager->free_list;

    objectManager->free_list = *object;
    *object                  = NULL;

    return object;
}

void
SCOREP_Allocator_PutObject( SCOREP_Allocator_ObjectManager* objectManager,
                            void*                           object )
{
    assert( objectManager && object );

    void** free_object = object;
    *free_object             = objectManager->free_list;
    objectManager->free_list = free_object;
}
