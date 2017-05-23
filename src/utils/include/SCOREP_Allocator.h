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


#ifndef SCOREP_ALLOCATOR_H
#define SCOREP_ALLOCATOR_H


/**
 * @file
 *
 * @brief Score-P public types (for user-public types see
 * SCOREP_Allocator_PublicTypes.h) and functions needed
 * in other headers that declare memory management functionality.
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <stddef.h>
#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif


/**
 * The memory allocator object that needs to be provided in calls to
 * SCOREP_Allocator_AllocFor*() and SCOREP_Allocator_Free*() functions
 * (e.g. SCOREP_Allocator_AllocForSummary() and
 * SCOREP_Allocator_FreeSummaryMem)). This object can be obtained by a call to
 * SCOREP_Allocator_GetAllocator() passing a SCOREP_Allocator_AllocatorType.
 *
 * Currently the idea is that there are exactly two
 * allocator objects per process, a system and a paged one. This restriction
 * may be weakened in future because it might be useful to have a paged
 * allocator for otf2 and a paged allocator for the measurement/adapters with
 * different page sizes.
 */
typedef struct SCOREP_Allocator_Allocator SCOREP_Allocator_Allocator;


typedef struct SCOREP_Allocator_PageManager SCOREP_Allocator_PageManager;


typedef struct SCOREP_Allocator_Page SCOREP_Allocator_Page;


typedef struct SCOREP_Allocator_ObjectManager SCOREP_Allocator_ObjectManager;


typedef void* SCOREP_Allocator_GuardObject;
typedef void ( * SCOREP_Allocator_Guard )( SCOREP_Allocator_GuardObject );


UTILS_BEGIN_C_DECLS

size_t
SCOREP_Allocator_RoundupToAlignment( size_t size );

/**
 * Create a memory allocator object that uses at maximum @a totalMemory
 * of memory and a page size of @a pageSize.
 *
 * @param[out] totalMemory Amount of memory that the allocator may use.
 *                       The actual available memory will be round-down to
 *                       the greatest multiple of the final value of @a
 *                       pageSize.
 * @param[out] pageSize  The @a totalMemory will be split into pages of size @a
 *                       pageSize. This requires the @a pageSize to be less or
 *                       equal than @a totalMemory. The @a pageSize will be
 *                       round-up to the next power of two.
 * @param lockFunction   Pointer to a function that takes @a lockObject and
 *                       performs some kind of synchronization until @a
 *                       unlockFunction indicate the end of the synchronization.
 *                       Pass 0 when single-threaded.
 * @param unlockFunction Pointer to a function that takes @a lockObject and ends
 *                       the synchronization that was started by a call to @a
 *                       lockFunction. Pass 0 when single-threaded.
 * @param lockObject     If appropriate, pass a lock object to be used in calls
 *                       to @a lockFunction and @a unlockFunction. Pass 0 when
 *                       single-threaded or when no lock object is needed.
 *
 * @return A valid allocator object or a null pointer if the creation fails.
 */
SCOREP_Allocator_Allocator*
SCOREP_Allocator_CreateAllocator( uint32_t*                    totalMemory,
                                  uint32_t*                    pageSize,
                                  SCOREP_Allocator_Guard       lockFunction,
                                  SCOREP_Allocator_Guard       unlockFunction,
                                  SCOREP_Allocator_GuardObject lockObject );


/**
 * Delete the allocator object @a allocator and free all it's memory.
 *
 * @param allocator
 */
void
SCOREP_Allocator_DeleteAllocator( SCOREP_Allocator_Allocator* allocator );


SCOREP_Allocator_PageManager*
SCOREP_Allocator_CreatePageManager( SCOREP_Allocator_Allocator* allocator );


SCOREP_Allocator_PageManager*
SCOREP_Allocator_CreateMovedPageManager( SCOREP_Allocator_Allocator* allocator );


void
SCOREP_Allocator_DeletePageManager( SCOREP_Allocator_PageManager* pageManager );


/**
 * Returns the start address of a memory chunk of at least @a memorySize bytes
 * from a @a pageManager's page. The contents of the memory block is undetermined.
 * Returns 0 if the allocation failed; this indicates a out-of-memory situation.
 *
 * @see SCOREP_Allocator_AllocMovable()
 * @param pageManager A valid SCOREP_Allocator_PageManager object.
 * @param memorySize Size of the memory block, must be > 0.
 */
void*
SCOREP_Allocator_Alloc( SCOREP_Allocator_PageManager* pageManager,
                        size_t                        memorySize );


void
SCOREP_Allocator_Free( SCOREP_Allocator_PageManager* pageManager );


/**
 * Allocates a movable chunk of memory of at least @a memorySize bytes from
 * a @a pageManager's page. Returns a reference/handle to this memory chunk.
 * To access the real memory you need to dereference the handle using
 * SCOREP_Allocator_GetAddressFromMovableMemory().
 * Returns 0 if the allocation failed; this indicates a out-of-memory situation.
 *
 * @see SCOREP_Allocator_Alloc()
 * @see SCOREP_Allocator_RollbackAllocMovable()
 * @param pageManager A valid SCOREP_Allocator_PageManager object.
 * @param memorySize Size of the memory block, must be > 0.
 */
SCOREP_Allocator_MovableMemory
SCOREP_Allocator_AllocMovable( SCOREP_Allocator_PageManager* pageManager,
                               size_t                        memorySize );


void*
SCOREP_Allocator_AllocMovedPage( SCOREP_Allocator_PageManager* movedPageManager,
                                 uint32_t                      moved_page_id,
                                 uint32_t                      page_usage );


/**
 * Covert a pointer to a SCOREP_Allocator_MovableMemory object to a raw pointer
 * to the @e real memory.
 *
 * @param movableMemory The movable memory to be converted.
 * @param allocator The allocator used for allocating @a movableMemory.
 *
 * @return A pointer to the raw memory cooresponding to the movable memory
 * object.
 */
void*
SCOREP_Allocator_GetAddressFromMovableMemory(
    const SCOREP_Allocator_PageManager* pageManager,
    SCOREP_Allocator_MovableMemory      movableMemory );


/** Discard the last movable allocation */
void
SCOREP_Allocator_RollbackAllocMovable( SCOREP_Allocator_PageManager*  pageManager,
                                       SCOREP_Allocator_MovableMemory movableMemory );


typedef struct SCOREP_Allocator_PageManagerStats
{
    uint32_t pages_allocated;
    uint32_t pages_used;
    size_t   memory_allocated;
    size_t   memory_used;
    size_t   memory_available;
} SCOREP_Allocator_PageManagerStats;


/**
 * Returns the number of pages hosted by the @a allocator.
 * @param allocator
 */
uint32_t
SCOREP_Allocator_GetMaxNumberOfPages( const SCOREP_Allocator_Allocator* allocator );


/**
 * Fill @a stats partially with the maximum number of pages used at a time
 * (high watermark) and the current number of allocated pages, w.r.t.
 * @a allocator.
 */
void
SCOREP_Allocator_GetPageStats( SCOREP_Allocator_Allocator*        allocator,
                               SCOREP_Allocator_PageManagerStats* stats );


/**
 * Fill @a stats either with data from the @a pageManager's pages (pageManager != 0)
 * or from the @a allocator's maintenance pages (pageManager == 0 && allocator != 0).
 */
void
SCOREP_Allocator_GetPageManagerStats( const SCOREP_Allocator_PageManager* pageManager,
                                      const SCOREP_Allocator_Allocator*   allocator,
                                      SCOREP_Allocator_PageManagerStats*  stats );

/**
 * Get the number of used pages for this page manager.
 *
 * The result should be used to allocate memory for the arguments to @ref
 * SCOREP_Allocator_GetPageInfos.
 *
 * The number is only valid until the next call to this page manager.
 * Excluding this function and @ref SCOREP_Allocator_GetPageInfos.
 *
 * @param pageManager The page manager.
 *
 * @return The number of used pages in the page manager @a pageManager.
 */
uint32_t
SCOREP_Allocator_GetNumberOfUsedPages( const SCOREP_Allocator_PageManager* pageManager );


/**
 * Get informations from the pages used in this page manager.
 *
 * All three arrays needs to have a size at for at least that much
 * elements for a valid number returned by @ref
 * SCOREP_Allocator_GetNumberOfUsedPages for the same page manger @a pageManager.
 *
 * @param pageManager The page manager.
 * @param[out] pageIds Array where to store the page ids for the used pages.
 * @param[out] pageUsages Array where to store the used bytes for the page.
 *                            (Can be NULL)
 * @param[out] pageStarts Array where to store the start address of the page.
 *                            (Can be NULL)
 */
void
SCOREP_Allocator_GetPageInfos( const SCOREP_Allocator_PageManager* pageManager,
                               uint32_t*                           pageIds,
                               uint32_t*                           pageUsages,
                               void** const                        pageStarts );


UTILS_END_C_DECLS


#endif /* SCOREP_ALLOCATOR_H */
