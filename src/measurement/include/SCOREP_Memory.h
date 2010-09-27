/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_MEMORY_H
#define SCOREP_MEMORY_H




/**
 * @file SCOREP_Memory.h Memory management interface for measurement and
 * adapters.
 *
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * @todo add comments about synchronization needs
 */


#include <scorep_utility/SCOREP_Allocator.h>
#include <stddef.h>


/**
 * Initialize the memory system for the measurement core, the adapters and the
 * online interface, i.e. everything except otf2.
 *
 * @param totalMemory total amount of memory in bytes the measurement system
 * should use.
 * @param pageSize size of a single page in bytes, needs to be <= @a totalMemory
 */
void
SCOREP_Memory_Initialize( size_t totalMemory,
                          size_t pageSize );


/**
 * Clean up the memory system.
 */
void
SCOREP_Memory_Finalize();


/**
 * If we encounter a new thread, we need to add page managers for all page
 * types to the newly created thread local data structures. But only here we
 * know how many and from which allocator.
 *
 * @see SCOREP_Thread.h
 *
 * @return
 */
SCOREP_Allocator_PageManager**
SCOREP_Memory_CreatePageManagers();


void
SCOREP_Memory_DeletePageManagers( SCOREP_Allocator_PageManager** pageManagers );


/**
 * @name Memory allocation functions for the measurement core and the adapters.
 *
 * These functions are the replacement of malloc and free. Note that there is
 * no possibility to free a single allocation but only to free the entire
 * allocated memory of a specific type. Due to the usual memory access
 * patterns during measurement this design is hopefully justified.
 *
 * We provide specific functions for different types of memory (like summary,
 * definitions, etc.) instead of a generic interface (that accepts e.g. a enum
 * to specify the type) to be more flexible and performant during
 * implementation (e.g. the summary needs a multithreaded implementation
 * whereas the definition memory is per process).
 *
 * We do not provide a memory interface for otf2 as it may not be possible to
 * harmonize the parameters for the paged allocator.
 *
 */
/*@{*/

/**
 * The malloc function for runtime summarization data (e.g. calltree nodes
 * etc.). It reserves a contiguous memory block whose size in bytes it at
 * least @a size. The contents of the memory block is undetermined.
 *
 * @param size The size of the requested memory block in bytes. @a size == 0
 * leads to undefined behaviour.
 *
 * @return The address of the first byte in the memory block allocated, or a
 * null pointer if the memory requested is not available.
 *
 * @see SCOREP_Memory_FreeSummaryMem()
 */
void*
SCOREP_Memory_AllocForProfile( size_t size  );


/**
 * Release the entire allocated runtime summarization data memory.
 *
 * @see SCOREP_Memory_AllocForSummary()
 */
void
SCOREP_Memory_FreeProfileMem();


/**
 * The malloc function for miscellaneous data. It reserves a
 * contiguous memory block whose size in bytes it at least @a size. The
 * contents of the memory block is undetermined.
 *
 * @param size The size of the requested memory block in bytes. @a size == 0
 * leads to undefined behaviour.
 *
 * @return The address of the first byte in the memory block allocated, or a
 * null pointer if the memory requested is not available.
 *
 * @see SCOREP_Memory_FreeMiscMem()
 */
void*
SCOREP_Memory_AllocForMisc( size_t size  );


/**
 * Release the entire allocated memory for miscellaneous stuff.
 *
 * @see SCOREP_Memory_AllocForMisc()
 */
void
SCOREP_Memory_FreeMiscMem();


/**
 * The malloc function for definition data. It reserves a contiguous memory
 * block whose size in bytes it at least @a size. The contents of the memory
 * block is undetermined.
 *
 * @param size The size of the requested memory block in bytes. @a size == 0
 * leads to undefined behaviour.
 *
 * @return A pseudo pointer of type SCOREP_Memory_DefinitionMemory object or a null
 * pointer if the memory requested is not available.
 *
 * @note You can obtain the real address of the memory and cast it to the
 * desired type with the help of the macro SCOREP_MEMORY_DEREF_MOVABLE.
 *
 * @see SCOREP_Memory_FreeDefinitionMem()
 */
SCOREP_Allocator_MovableMemory
SCOREP_Memory_AllocForDefinitions( size_t size );

/**
 * Release the entire allocated definition memory.
 *
 * @see SCOREP_Memory_AllocForDefinitions()
 */
void
SCOREP_Memory_FreeDefinitionMem();


void*
SCOREP_Memory_GetAddressFromMovableMemory( SCOREP_Allocator_MovableMemory movableMemory );


//void*
//SCOREP_Memory_GetAddressFromMovedMemory( SCOREP_Allocator_MovableMemory     movedMemory,
//                                       SCOREP_Allocator_MovedPageManager* movedPageManager );


//void
//SCOREP_Memory_FreeMovedPages( SCOREP_Allocator_PageManager* movedPageManager );


/**
 * Just a convenience macro to access the @e real memory a
 * SCOREP_Allocator_MovableMemory object is referring to.
 *
 * @param definition_memory_ptr Pointer to some SCOREP_Memory_DefinitionMemory
 * object.
 * @param target_type The type @a definition_memory_ptr should be converted to.
 *
 * @return A pointer to an object of type @a target_type.
 */
#define SCOREP_MEMORY_DEREF_MOVABLE( movable_memory, target_type ) \
    ( ( target_type )SCOREP_Memory_GetAddressFromMovableMemory( movable_memory ) )


#define SCOREP_MEMORY_DEREF_MOVED( moved_memory, moved_page_manager, target_type )  \
    ( ( target_type )SCOREP_Allocator_GetAddressFromMovedMemory(                        \
          movable_memory, moved_page_manager ) )


/*@}*/

#endif /* SCOREP_MEMORY_H */
