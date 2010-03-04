#ifndef SILC_MEMORY_H
#define SILC_MEMORY_H


/*
 * This file is part of the SILC project (http://www.silc.de)
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


/**
 * @file SILC_Memory.h Memory management interface for measurement and
 * adapters.
 *
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 * @todo add comments about synchronization needs
 */


#include <SILC_Allocator.h>
#include <stddef.h>


/**
 * Initialize the memory system for the measurement core, the adapters and the
 * online interface, i.e. everything except otf2.
 */
void
SILC_Memory_Initialize();


/**
 * Clean up the memory system.
 */
void
SILC_Memory_Finalize();


/**
 * If we encounter a new thread, we need to add page managers for all page
 * types to the newly created thread local data structures. But only here we
 * know how many and from which allocator.
 *
 * @see SILC_Thread.h
 *
 * @return
 */
SILC_Memory_PageManager**
SILC_Memory_CreatePageManagers();


void
SILC_Memory_DeletePageManagers( SILC_Memory_PageManager** pageManagers );


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
 * @param allocator The allocator object that should handle the request.
 *
 * @return The address of the first byte in the memory block allocated, or a
 * null pointer if the memory requested is not available.
 *
 * @see SILC_Memory_FreeSummaryMem()
 */
void*
SILC_Memory_AllocForProfile( size_t size  );


/**
 * Release the entire allocated runtime summarization data memory.
 *
 * @param allocator The allocator object that was used for allocation.
 *
 * @see SILC_Memory_AllocForSummary()
 */
void
SILC_Memory_FreeProfileMem();


/**
 * The malloc function for miscellaneous multithreaded data. It reserves a
 * contiguous memory block whose size in bytes it at least @a size. The
 * contents of the memory block is undetermined.
 *
 * @param size The size of the requested memory block in bytes. @a size == 0
 * leads to undefined behaviour.
 * @param allocator The allocator object that should handle the request.
 *
 * @return The address of the first byte in the memory block allocated, or a
 * null pointer if the memory requested is not available.
 *
 * @see SILC_Memory_FreeMultithreadedMiscMem()
 */
void*
SILC_Memory_AllocForMultithreadedMisc( size_t size  );


/**
 * Release the entire allocated memory for miscellaneous multithreaded stuff.
 *
 * @param allocator The allocator object that was used for allocation.
 *
 * @see SILC_Memory_AllocForMultithreadedMisc()
 */
void
SILC_Memory_FreeMultithreadedMiscMem();


/**
 * The malloc function for miscellaneous singlehreaded data. It reserves a
 * contiguous memory block whose size in bytes it at least @a size. The
 * contents of the memory block is undetermined.
 *
 * @param size The size of the requested memory block in bytes. @a size == 0
 * leads to undefined behaviour.
 * @param allocator The allocator object that should handle the request.
 *
 * @return The address of the first byte in the memory block allocated, or a
 * null pointer if the memory requested is not available.
 *
 * @see SILC_Memory_FreeSinglethreadedMiscMem()
 */
void*
SILC_Memory_AllocForSinglethreadedMisc( size_t size  );


/**
 * Release the entire allocated memory for miscellaneous singlethreaded stuff.
 *
 * @param allocator The allocator object that was used for allocation.
 *
 * @see SILC_Memory_AllocForSinglethreadedMisc()
 */
void
SILC_Memory_FreeSinglethreadedMiscMem();


/**
 * Here we use moveable memory for definitions
 * only. SILC_Memory_DefinitionMemory may then be the appropriate name.
 *
 */
typedef SILC_Memory_MoveableMemory SILC_Memory_DefinitionMemory;


/**
 * The malloc function for definition data. It reserves a contiguous memory
 * block whose size in bytes it at least @a size. The contents of the memory
 * block is undetermined.
 *
 * @param size The size of the requested memory block in bytes. @a size == 0
 * leads to undefined behaviour.
 * @param allocator The allocator object that should handle the request.
 *
 * @return A pointer to a valid SILC_Memory_DefinitionMemory object or a null
 * pointer if the memory requested is not available.
 *
 * @note You can obtain the real address of the memory and cast it to the
 * desired type with the help of the macro
 * SILC_MEMORY_CAST_DEFINITION_MEMORY_TO_TYPE.
 *
 * @see SILC_Memory_FreeDefinitionMem()
 */
SILC_Memory_DefinitionMemory*
SILC_Memory_AllocForDefinitions( size_t size );


/**
 * Release the entire allocated definition memory.
 *
 * @param allocator The allocator object that was used for allocation.
 *
 * @see SILC_Memory_AllocForDefinitions()
 */
void
SILC_Memory_FreeDefinitionMem();


/**
 * Just a better-named wrapper around SILC_MEMORY_CAST_MOVEABLE_MEMORY_TO_TYPE
 *
 * @param definition_memory_ptr Pointer to some SILC_Memory_DefinitionMemory object.
 * @param target_type The type @a definition_memory_ptr should be converted to.
 * @param allocator The allocator that was used to allocate the
 * SILC_Memory_DefinitionMemory object referenced by @a definition_memory_ptr.
 *
 * @return A pointer to an object of type @a target_type.
 */
#define SILC_MEMORY_CAST_DEFINITION_MEMORY_TO_TYPE( definition_memory_ptr, target_type, allocator ) \
    ( SILC_MEMORY_CAST_MOVEABLE_MEMORY_TO_TYPE( definition_memory_ptr, target_type, allocator ) )


/*@}*/

#endif /* SILC_MEMORY_H */
