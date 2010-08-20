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
 *
 *
 *
 */


#include <SILC_Memory.h>
#include <SILC_Allocator.h>
#include "silc_thread.h"
#include "silc_status.h"
#include <assert.h>
#include <stdbool.h>


/// The memory guard stuff is implemented in paradigm/silc_memory_locking_*
/* *INDENT-OFF* */
void silc_memory_guard_initialze();
void silc_memory_guard_finalize();
/* *INDENT-ON* */


extern SILC_Allocator_Guard silc_memory_lock;
extern SILC_Allocator_Guard       silc_memory_unlock;
extern SILC_Allocator_GuardObject silc_memory_guard_object_ptr;


/// @todo implement memory statistics


/// The one and only allocator for the measurement and the adapters
static SILC_Allocator_Allocator*   silc_memory_allocator = 0;

static bool                        silc_memory_is_initialized = false;

static SILC_Allocator_PageManager* silc_memory_definition_pagemanager = 0;


enum silc_memory_page_type
{
    profile_pages = 0,  // separate because we might clear them for periscope from time to time
    //adapter_pages, // do we need extra pages for adapters or shall they use the subsequent two?
    //multithreaded_misc_pages,
    //singlethreaded_misc_pages,
    misc_pages,
    //definitions_pages, we need only one page manager for definition as they are collected by process
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

    silc_memory_guard_initialze();

    assert( silc_memory_allocator == 0 );
    silc_memory_allocator = SILC_Allocator_CreateAllocator( totalMemory,
                                                            pageSize,
                                                            silc_memory_lock,
                                                            silc_memory_unlock,
                                                            silc_memory_guard_object_ptr );
    if ( !silc_memory_allocator )
    {
        silc_memory_guard_finalize();
        silc_memory_is_initialized = false;
        assert( false );
    }

    assert( silc_memory_definition_pagemanager == 0 );
    silc_memory_definition_pagemanager = SILC_Allocator_CreatePageManager( silc_memory_allocator );
    if ( !silc_memory_definition_pagemanager )
    {
        silc_memory_guard_finalize();
        silc_memory_is_initialized = false;
        SILC_ERROR( SILC_ERROR_MEMORY_OUT_OF_PAGES,
                    "Can't create new page manager due to lack of free pages." );
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

    assert( silc_memory_definition_pagemanager );
#if defined ( __INTEL_COMPILER ) && ( __INTEL_COMPILER < 1120 )
    // Do nothing here. Intel OpenMP RTL shuts down at the end of main
    // function, so omp_set/unset_lock, which is called after the end
    // of main from the atexit handler, causes segmentation fault. The
    // problem will be fixed in  Intel Compiler 11.1 update 6.
    // See http://software.intel.com/en-us/forums/showpost.php?p=110592
#else
    SILC_Allocator_DeletePageManager( silc_memory_definition_pagemanager );
#endif
    silc_memory_definition_pagemanager = 0;

    assert( silc_memory_allocator );
    SILC_Allocator_DeleteAllocator( silc_memory_allocator );
    silc_memory_allocator = 0;

    silc_memory_guard_finalize();
}


SILC_Allocator_PageManager**
SILC_Memory_CreatePageManagers()
{
    SILC_Allocator_PageManager** array =
        malloc( number_of_page_types * sizeof( SILC_Allocator_PageManager* ) );
    assert( array );
    for ( int i = 0; i < number_of_page_types; ++i )
    {
        if ( i == profile_pages && !SILC_IsProfilingEnabled() )
        {
            array[ i ] = 0;
            continue;
        }
        array[ i ] = SILC_Allocator_CreatePageManager( silc_memory_allocator );
        if ( !array[ i ] )
        {
            SILC_ERROR( SILC_ERROR_MEMORY_OUT_OF_PAGES,
                        "Can't create new page manager due to lack of free pages." );
            assert( 0 );
        }
    }
    return array;
}


void
SILC_Memory_DeletePageManagers( SILC_Allocator_PageManager** pageManagers )
{
    // is there a need to free pages before deleting them?
    for ( int i = 0; i < number_of_page_types; ++i )
    {
        if ( pageManagers[ i ] )
        {
#if defined ( __INTEL_COMPILER ) && ( __INTEL_COMPILER < 1120 )
            // Do nothing here. Intel OpenMP RTL shuts down at the end of main
            // function, so omp_set/unset_lock, which is called after the end
            // of main from the atexit handler, causes segmentation fault. The
            // problem will be fixed in  Intel Compiler 11.1 update 6.
            // See http://software.intel.com/en-us/forums/showpost.php?p=110592
#else
            SILC_Allocator_DeletePageManager( pageManagers[ i ] );
#endif
        }
    }
    free( pageManagers );
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


SILC_Allocator_MovableMemory
SILC_Memory_AllocForDefinitions( size_t size )
{
    // collect statistics
    return SILC_Allocator_AllocMovable( silc_memory_definition_pagemanager, size );
}


void
SILC_Memory_FreeDefinitionMem()
{
    // print mem usage statistics
    SILC_Allocator_Free( silc_memory_definition_pagemanager );
}


void*
SILC_Memory_GetAddressFromMovableMemory( SILC_Allocator_MovableMemory movableMemory )
{
    return SILC_Allocator_GetAddressFromMovableMemory(
               silc_memory_definition_pagemanager,
               movableMemory );
}
