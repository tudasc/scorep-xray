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
 * @file       SILC_Memory.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 *
 */


#include <SILC_Memory.h>
#include <SILC_AllocatorImpl.h>
#include "silc_thread.h"
#include <assert.h>
#include <stdbool.h>

/// @todo implement memory statistics

/// The one and only allocator for the measurement and the adapters
SILC_Allocator_Allocator* silc_memory_allocator = 0;

static bool               silc_memory_is_initialized = false;

enum silc_memory_page_type
{
    profile_pages = 0,  // separate because we might clear them for periscope from time to time
    //adapter_pages, // do we need extra pages for adapters or shall they use the subsequent two?
    //multithreaded_misc_pages,
    //singlethreaded_misc_pages,
    misc_pages,
    definitions_pages,
    number_of_page_types
};


void
SILC_Memory_Initialize( size_t totalMemory,
                        size_t pageSize )
{
    assert( totalMemory >= pageSize );

    if ( silc_memory_is_initialized )
    {
        return;
    }
    silc_memory_is_initialized = true;

    silc_memory_allocator = SILC_Allocator_CreateAllocator( system_alloc,
                                                            totalMemory,
                                                            pageSize );
    if ( !silc_memory_allocator )
    {
        assert( false );
    }
}


void
SILC_Memory_Finalize()
{
    if ( !silc_memory_is_initialized )
    {
        return;
    }
    silc_memory_is_initialized = false;

    SILC_Allocator_DeleteAllocator( silc_memory_allocator );
    silc_memory_allocator = 0;
}


SILC_Allocator_PageManager**
SILC_Memory_CreatePageManagers()
{
    SILC_Allocator_PageManager** array =
        malloc( number_of_page_types * sizeof( SILC_Allocator_PageManager* ) );
    assert( array );
    for ( int i = 0; i < number_of_page_types; ++i )
    {
        array[ i ] = SILC_Allocator_CreatePageManager( silc_memory_allocator );
        assert( array[ i ] );
    }
    return array;
}


void
SILC_Memory_DeletePageManagers( SILC_Allocator_PageManager** pageManagers )
{
    // is there a need to free pages before deleting them?
    for ( int i = 0; i < number_of_page_types; ++i )
    {
        SILC_Allocator_DeletePageManager( pageManagers[ i ] );
    }
}


void*
SILC_Memory_AllocForProfile( size_t size  )
{
    // collect statistics
    return SILC_Allocator_Alloc(
               SILC_Thread_GetLocationLocalMemoryPageManagers()[ profile_pages ], size );
}


void
SILC_Memory_FreeProfileMem()
{
    // print mem usage statistics
    SILC_Allocator_Free(
        SILC_Thread_GetLocationLocalMemoryPageManagers()[ profile_pages ] );
}



void*
SILC_Memory_AllocForMisc( size_t size  )
{
    // collect statistics
    return SILC_Allocator_Alloc(
               SILC_Thread_GetLocationLocalMemoryPageManagers()[ misc_pages ], size );
}


void
SILC_Memory_FreeMiscMem()
{
    // print mem usage statistics
    SILC_Allocator_Free(
        SILC_Thread_GetLocationLocalMemoryPageManagers()[ misc_pages ] );
}


SILC_Allocator_MovableMemory*
SILC_Memory_AllocForDefinitions( size_t size )
{
    // collect statistics
    return SILC_Allocator_AllocMovable(
               SILC_Thread_GetGlobalMemoryPageManagers()[ definitions_pages ], size );
}


void
SILC_Memory_AllocForDefinitionsRaw( size_t                        size,
                                    SILC_Allocator_MovableMemory* movableMemory )
{
    // collect statistics
    SILC_Allocator_AllocMovableRaw(
        SILC_Thread_GetGlobalMemoryPageManagers()[ definitions_pages ],
        size,
        movableMemory );
}


void
SILC_Memory_FreeDefinitionMem()
{
    // print mem usage statistics
    SILC_Allocator_Free(
        SILC_Thread_GetGlobalMemoryPageManagers()[ definitions_pages ] );
}
