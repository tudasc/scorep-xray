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


/// The one and only allocator for the measurement and the adapters
static SILC_Memory_Allocator* silc_memory_allocator = 0;


enum silc_memory_page_type
{
    profile_pages = 0,
    //adapter_pages, // do we need extra pages for adapters or shall they use the subsequent two?
    //multithreaded_misc_pages,
    //singlethreaded_misc_pages,
    misc_pages,
    definitions_pages,
    number_of_page_types
};


void
SILC_Memory_Initialize()
{
    size_t total_memory = 0; // read from configuration or pass in
    size_t page_size    = 0; // dito, or have suitable default

    silc_memory_allocator = SILC_Memory_CreateAllocator( system_alloc, total_memory, page_size );
    if ( !silc_memory_allocator )
    {
        assert( false );
    }
}


void
SILC_Memory_Finalize()
{
    SILC_Memory_DeleteAllocator( silc_memory_allocator );
}


SILC_Memory_PageManager**
SILC_Memory_CreatePageManagers()
{
    return SILC_Memory_CreatePageManagerArray( silc_memory_allocator, number_of_page_types );
}


void
SILC_Memory_DeletePageManagers( SILC_Memory_PageManager** pageManagers )
{
    SILC_Memory_DeletePageManagerArray( pageManagers );
}


void*
SILC_Memory_AllocForProfile( size_t size  )
{
    // collect statistics
    return SILC_Memory_Alloc( SILC_Thread_GetLocationLocalMemoryPageManagers()[ profile_pages ], size );
}


void
SILC_Memory_FreeProfileMem()
{
    // print mem usage statistics
    SILC_Memory_Free( SILC_Thread_GetLocationLocalMemoryPageManagers()[ profile_pages ] );
}



void*
SILC_Memory_AllocForMultithreadedMisc( size_t size  )
{
    // collect statistics
    return SILC_Memory_Alloc( SILC_Thread_GetLocationLocalMemoryPageManagers()[ misc_pages ], size );
}


void
SILC_Memory_FreeMultithreadedMiscMem()
{
    // print mem usage statistics
    SILC_Memory_Free( SILC_Thread_GetLocationLocalMemoryPageManagers()[ misc_pages ] );
}

void*
SILC_Memory_AllocForSinglethreadedMisc( size_t size  )
{
    // collect statistics
    return SILC_Memory_Alloc( SILC_Thread_GetGlobalMemoryPageManagers()[ misc_pages ], size );
}

void
SILC_Memory_FreeSinglethreadedMiscMem()
{
    // print mem usage statistics
    SILC_Memory_Free( SILC_Thread_GetGlobalMemoryPageManagers()[ misc_pages ] );
}


SILC_Allocator_MoveableMemory*
SILC_Memory_AllocForDefinitions( size_t size )
{
    // collect statistics
    return SILC_Memory_AllocMoveable( SILC_Thread_GetGlobalMemoryPageManagers()[ definitions_pages ], size );
}

void*
SILC_Memory_AllocForDefinitionsRaw( size_t                         size,
                                    SILC_Allocator_MoveableMemory* moveableMemory )
{
    // collect statistics
    // alloc only size
    // let moveableMemory point to newly allocated mem
    return 0;
}

void
SILC_Memory_FreeDefinitionMem()
{
    // print mem usage statistics
    SILC_Memory_Free( SILC_Thread_GetGlobalMemoryPageManagers()[ definitions_pages ] );
}
