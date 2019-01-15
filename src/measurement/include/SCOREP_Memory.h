/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2015, 2017-2018,
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

#ifndef SCOREP_MEMORY_H
#define SCOREP_MEMORY_H




/**
 * @file
 * adapters.
 *
 *
 *
 * @todo add comments about synchronization needs
 */


#include <SCOREP_Allocator.h>
#include <stddef.h>


#include <SCOREP_Location.h>


/**
 * Types of memory pages a location has.
 *
 */
typedef enum SCOREP_MemoryType
{
    SCOREP_MEMORY_TYPE_MISC = 0,

    /* For per-location definitions */
    SCOREP_MEMORY_TYPE_DEFINITIONS,

    /** separate because we might clear them for periscope from time to time */
    SCOREP_MEMORY_TYPE_PROFILING,

    /** separate because we need to flush these independently */
    SCOREP_MEMORY_TYPE_TRACING_EVENTS,

    SCOREP_NUMBER_OF_MEMORY_TYPES
} SCOREP_MemoryType;


/**
 * Initialize the memory system for the measurement core, the adapters and the
 * online interface, i.e. everything except otf2.
 *
 * @param totalMemory total amount of memory in bytes the measurement system
 * should use.
 * @param pageSize size of a single page in bytes, needs to be <= @a totalMemory
 */
void
SCOREP_Memory_Initialize( uint64_t totalMemory,
                          uint64_t pageSize );


/**
 * Informs the user of an out of memory situation and aborts the measurement.
 */
void
SCOREP_Memory_HandleOutOfMemory( void );


/**
 * Dumps memory usage stats to stderr. Note that the trace buffers are currently
 * not accessible and are not shown in this stats.
 * @param message Message printed in front of the statistics to distinguish output
 * in case of multiple usage
 */
void
SCOREP_Memory_DumpStats( const char* message );


/**
 * Clean up the memory system.
 */
void
SCOREP_Memory_Finalize( void );


/**
 * Creates a page manager for the tracing event writer.
 */
SCOREP_Allocator_PageManager*
SCOREP_Memory_CreateTracingPageManager( void );


/**
 * Create a page manager on the fly. Triggers SCOREP_Memory_HandleOutOfMemory()
 * if creation fails.
 * By creation on the fly we will only create the least amount of page managers,
 * thus reducing the number of pages wasted if the page manager stays unused.
 */
SCOREP_Allocator_PageManager*
SCOREP_Memory_CreatePageManager( void );


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
 * The malloc function for miscellaneous data. It reserves a
 * contiguous memory block whose size in bytes it at least @a size. The
 * contents of the memory block is undetermined.
 *
 * @note @a SCOREP_Location_AllocForMisc is not thread safe. This function is
 *       only useful when called in a init_location subsystem callback.
 *
 * @param size The size of the requested memory block in bytes. @a size == 0
 * leads to undefined behaviour.
 *
 * @return The address of the first byte in the memory block allocated, or
 *         aborts if the request could not handled.
 *
 * @see SCOREP_Memory_FreeMiscMem()
 *
 * @{
 */
void*
SCOREP_Memory_AllocForMisc( size_t size );

void*
SCOREP_Location_AllocForMisc( SCOREP_Location* locationData,
                              size_t           size );

/**
 * @}
 */


/**
 * Release the entire allocated memory for miscellaneous stuff.
 *
 * @see SCOREP_Memory_AllocForMisc()
 */
void
SCOREP_Memory_FreeMiscMem( void );


/**
 * The malloc function for runtime summarization data (e.g. calltree nodes
 * etc.). It reserves a contiguous memory block whose size in bytes it at
 * least @a size. The contents of the memory block is undetermined.
 *
 * @param size The size of the requested memory block in bytes. @a size == 0
 * leads to undefined behaviour.
 *
 * @return The address of the first byte in the memory block allocated, or
 *         aborts if the request could not handled.
 *
 * @see SCOREP_Memory_FreeSummaryMem()
 *
 * @{
 */
void*
SCOREP_Location_AllocForProfile( SCOREP_Location* location,
                                 size_t           size );


/**
 * @}
 */

/**
 * Release the allocated runtime profile memory for @a location.
 *
 * @see SCOREP_Memory_AllocForProfile()
 */
void
SCOREP_Memory_FreeProfileMem( SCOREP_Location* location );


/**
 * The malloc function for definition data. It reserves a contiguous memory
 * block whose size in bytes it at least @a size. The contents of the memory
 * block is undetermined.
 *
 * @param location When not NULL, allocate memory from the definition memory
 *        pool of that location.
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
SCOREP_Memory_AllocForDefinitions( SCOREP_Location* location,
                                   size_t           size );

/**
 * Release the entire allocated definition memory.
 *
 * @see SCOREP_Memory_AllocForDefinitions()
 */
void
SCOREP_Memory_FreeDefinitionMem( void );


void*
SCOREP_Memory_GetAddressFromMovableMemory( SCOREP_Allocator_MovableMemory movableMemory,
                                           SCOREP_Allocator_PageManager*  movablePageManager );

SCOREP_Allocator_PageManager*
SCOREP_Memory_CreateMovedPagedMemory( void );


SCOREP_Allocator_PageManager*
SCOREP_Memory_GetLocalDefinitionPageManager( void );


/*@}*/

#endif /* SCOREP_MEMORY_H */
