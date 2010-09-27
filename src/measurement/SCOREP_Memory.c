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


/**
 * @file       SCOREP_Memory.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status  alpha
 *
 *
 */

#include <config.h>
#include <SCOREP_Memory.h>
#include <scorep_utility/SCOREP_Allocator.h>
#include "scorep_thread.h"
#include "scorep_status.h"
#include <assert.h>
#include <stdbool.h>


/// The memory guard stuff is implemented in paradigm/scorep_memory_locking_*
/* *INDENT-OFF* */
void scorep_memory_guard_initialze();
void scorep_memory_guard_finalize();
/* *INDENT-ON* */


extern SCOREP_Allocator_Guard scorep_memory_lock;
extern SCOREP_Allocator_Guard       scorep_memory_unlock;
extern SCOREP_Allocator_GuardObject scorep_memory_guard_object_ptr;


/// @todo implement memory statistics


/// The one and only allocator for the measurement and the adapters
static SCOREP_Allocator_Allocator*   scorep_memory_allocator = 0;

static bool                          scorep_memory_is_initialized = false;

static SCOREP_Allocator_PageManager* scorep_memory_definition_pagemanager = 0;


enum scorep_memory_page_type
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
SCOREP_Memory_Initialize( size_t totalMemory,
                          size_t pageSize )
{
    assert( totalMemory >= pageSize );

    if ( scorep_memory_is_initialized )
    {
        return;
    }
    scorep_memory_is_initialized = true;

    scorep_memory_guard_initialze();

    assert( scorep_memory_allocator == 0 );
    scorep_memory_allocator = SCOREP_Allocator_CreateAllocator( totalMemory,
                                                                pageSize,
                                                                scorep_memory_lock,
                                                                scorep_memory_unlock,
                                                                scorep_memory_guard_object_ptr );
    if ( !scorep_memory_allocator )
    {
        scorep_memory_guard_finalize();
        scorep_memory_is_initialized = false;
        assert( false );
    }

    assert( scorep_memory_definition_pagemanager == 0 );
    scorep_memory_definition_pagemanager = SCOREP_Allocator_CreatePageManager( scorep_memory_allocator );
    if ( !scorep_memory_definition_pagemanager )
    {
        scorep_memory_guard_finalize();
        scorep_memory_is_initialized = false;
        SCOREP_ERROR( SCOREP_ERROR_MEMORY_OUT_OF_PAGES,
                      "Can't create new page manager due to lack of free pages." );
        assert( false );
    }
}


void
SCOREP_Memory_Finalize()
{
    if ( !scorep_memory_is_initialized )
    {
        return;
    }
    scorep_memory_is_initialized = false;

    assert( scorep_memory_definition_pagemanager );
#if defined ( __INTEL_COMPILER ) && ( __INTEL_COMPILER < 1120 )
    // Do nothing here. Intel OpenMP RTL shuts down at the end of main
    // function, so omp_set/unset_lock, which is called after the end
    // of main from the atexit handler, causes segmentation fault. The
    // problem will be fixed in  Intel Compiler 11.1 update 6.
    // See http://software.intel.com/en-us/forums/showpost.php?p=110592
#else
    SCOREP_Allocator_DeletePageManager( scorep_memory_definition_pagemanager );
#endif
    scorep_memory_definition_pagemanager = 0;

    assert( scorep_memory_allocator );
    SCOREP_Allocator_DeleteAllocator( scorep_memory_allocator );
    scorep_memory_allocator = 0;

    scorep_memory_guard_finalize();
}


SCOREP_Allocator_PageManager**
SCOREP_Memory_CreatePageManagers()
{
    SCOREP_Allocator_PageManager** array =
        malloc( number_of_page_types * sizeof( SCOREP_Allocator_PageManager* ) );
    assert( array );
    for ( int i = 0; i < number_of_page_types; ++i )
    {
        if ( i == profile_pages && !SCOREP_IsProfilingEnabled() )
        {
            array[ i ] = 0;
            continue;
        }
        array[ i ] = SCOREP_Allocator_CreatePageManager( scorep_memory_allocator );
        if ( !array[ i ] )
        {
            SCOREP_ERROR( SCOREP_ERROR_MEMORY_OUT_OF_PAGES,
                          "Can't create new page manager due to lack of free pages." );
            assert( 0 );
        }
    }
    return array;
}


void
SCOREP_Memory_DeletePageManagers( SCOREP_Allocator_PageManager** pageManagers )
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
            SCOREP_Allocator_DeletePageManager( pageManagers[ i ] );
#endif
        }
    }
    free( pageManagers );
}


void*
SCOREP_Memory_AllocForProfile( size_t size  )
{
    // collect statistics
    return SCOREP_Allocator_Alloc(
               SCOREP_Thread_GetLocationLocalMemoryPageManagers()[ profile_pages ], size );
}


void
SCOREP_Memory_FreeProfileMem()
{
    // print mem usage statistics
    SCOREP_Allocator_Free(
        SCOREP_Thread_GetLocationLocalMemoryPageManagers()[ profile_pages ] );
}



void*
SCOREP_Memory_AllocForMisc( size_t size  )
{
    // collect statistics
    return SCOREP_Allocator_Alloc(
               SCOREP_Thread_GetLocationLocalMemoryPageManagers()[ misc_pages ], size );
}


void
SCOREP_Memory_FreeMiscMem()
{
    // print mem usage statistics
    SCOREP_Allocator_Free(
        SCOREP_Thread_GetLocationLocalMemoryPageManagers()[ misc_pages ] );
}


SCOREP_Allocator_MovableMemory
SCOREP_Memory_AllocForDefinitions( size_t size )
{
    // collect statistics
    return SCOREP_Allocator_AllocMovable( scorep_memory_definition_pagemanager, size );
}


void
SCOREP_Memory_FreeDefinitionMem()
{
    // print mem usage statistics
    SCOREP_Allocator_Free( scorep_memory_definition_pagemanager );
}


void*
SCOREP_Memory_GetAddressFromMovableMemory( SCOREP_Allocator_MovableMemory movableMemory )
{
    return SCOREP_Allocator_GetAddressFromMovableMemory(
               scorep_memory_definition_pagemanager,
               movableMemory );
}
