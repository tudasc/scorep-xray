/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
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
#include <UTILS_Error.h>
#include <SCOREP_Mutex.h>
#include "scorep_environment.h"
#include "scorep_thread.h"
#include "scorep_status.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/* *INDENT-OFF* */
/* *INDENT-ON* */


static SCOREP_Mutex memory_lock;


/// @todo implement memory statistics


/// The one and only allocator for the measurement and the adapters
static SCOREP_Allocator_Allocator* scorep_memory_allocator = 0;
static uint64_t                    scorep_memory_total_memory;

static bool scorep_memory_is_initialized = false;

static SCOREP_Allocator_PageManager* scorep_definitions_page_manager;

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

    SCOREP_MutexCreate( &memory_lock );

    assert( scorep_memory_allocator == 0 );
    scorep_memory_allocator = SCOREP_Allocator_CreateAllocator(
        totalMemory,
        pageSize,
        ( SCOREP_Allocator_Guard )SCOREP_MutexLock,
        ( SCOREP_Allocator_Guard )SCOREP_MutexUnlock,
        ( SCOREP_Allocator_GuardObject )memory_lock );

    if ( !scorep_memory_allocator )
    {
        SCOREP_MutexDestroy( &memory_lock );
        scorep_memory_is_initialized = false;
        assert( false );
    }

    assert( scorep_definitions_page_manager == 0 );
    scorep_definitions_page_manager = SCOREP_Allocator_CreatePageManager( scorep_memory_allocator );
    if ( !scorep_definitions_page_manager )
    {
        SCOREP_MutexDestroy( &memory_lock );
        scorep_memory_is_initialized = false;
        SCOREP_Memory_HandleOutOfMemory();
    }

    scorep_memory_total_memory = totalMemory;
}


void
SCOREP_Memory_Finalize()
{
    if ( !scorep_memory_is_initialized )
    {
        return;
    }
    scorep_memory_is_initialized = false;

    assert( scorep_definitions_page_manager );
#if defined ( __INTEL_COMPILER ) && ( __INTEL_COMPILER < 1120 )
    // Do nothing here. Intel OpenMP RTL shuts down at the end of main
    // function, so omp_set/unset_lock, which is called after the end
    // of main from the atexit handler, causes segmentation fault. The
    // problem will be fixed in  Intel Compiler 11.1 update 6.
    // See http://software.intel.com/en-us/forums/showpost.php?p=110592
#else
    SCOREP_Allocator_DeletePageManager( scorep_definitions_page_manager );
#endif
    scorep_definitions_page_manager = 0;

    assert( scorep_memory_allocator );
    SCOREP_Allocator_DeleteAllocator( scorep_memory_allocator );
    scorep_memory_allocator = 0;

    SCOREP_MutexDestroy( &memory_lock );
}

void
SCOREP_Memory_HandleOutOfMemory( void )
{
    UTILS_ERROR( SCOREP_ERROR_MEMORY_OUT_OF_PAGES,
                 "Out of memory. Please increase SCOREP_TOTAL_MEMORY=%" PRIu64 " and try again.",
                 scorep_memory_total_memory );
    if ( SCOREP_Env_DoTracing() )
    {
        UTILS_ERROR( SCOREP_ERROR_MEMORY_OUT_OF_PAGES,
                     "Please ensure that there are at least 2MB available for each intended thread." );
        const char* omp_num_threads = getenv( "OMP_NUM_THREADS" );
        if ( omp_num_threads && strlen( omp_num_threads ) )
        {
            UTILS_ERROR( SCOREP_ERROR_MEMORY_OUT_OF_PAGES,
                         "And you seem to have requested %s threads for the measurement.",
                         omp_num_threads );
        }
    }
    abort();
}

SCOREP_Allocator_PageManager*
SCOREP_Memory_CreateTracingPageManager( void )
{
    SCOREP_Allocator_PageManager* page_manager =
        SCOREP_Allocator_CreatePageManager( scorep_memory_allocator );
    if ( !page_manager )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }
    return page_manager;
}


void
SCOREP_Memory_CreatePageManagers( SCOREP_Allocator_PageManager** pageManagers )
{
    for ( int i = 0; i < SCOREP_NUMBER_OF_MEMORY_TYPES; ++i )
    {
        if ( i == SCOREP_MEMORY_TYPE_PROFILING && !SCOREP_IsProfilingEnabled() )
        {
            pageManagers[ i ] = 0;
            continue;
        }
        pageManagers[ i ] =
            SCOREP_Allocator_CreatePageManager( scorep_memory_allocator );
        if ( !pageManagers[ i ] )
        {
            /* aborts */
            SCOREP_Memory_HandleOutOfMemory();
        }
    }
}


void
SCOREP_Memory_DeletePageManagers( SCOREP_Allocator_PageManager** pageManagers )
{
    // is there a need to free pages before deleting them?
    for ( int i = 0; i < SCOREP_NUMBER_OF_MEMORY_TYPES; ++i )
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
}


void*
SCOREP_Location_AllocForMisc( SCOREP_Location* locationData, size_t size )
{
    // collect statistics
    if ( size == 0 )
    {
        return NULL;
    }

    void* mem = SCOREP_Allocator_Alloc(
        SCOREP_Location_GetMemoryPageManager( locationData,
                                              SCOREP_MEMORY_TYPE_MISC ),
        size );
    if ( !mem )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }
    return mem;
}


void*
SCOREP_Memory_AllocForMisc( size_t size )
{
    return SCOREP_Location_AllocForMisc( SCOREP_Location_GetCurrentCPULocation(),
                                         size );
}


void
SCOREP_Memory_FreeMiscMem()
{
    // print mem usage statistics
    SCOREP_Allocator_Free(
        SCOREP_Location_GetMemoryPageManager(
            SCOREP_Location_GetCurrentCPULocation(),
            SCOREP_MEMORY_TYPE_MISC ) );
}


void*
SCOREP_Memory_AllocForProfile( SCOREP_Location* location, size_t size )
{
    // collect statistics
    if ( size == 0 )
    {
        return NULL;
    }

    void* mem = SCOREP_Allocator_Alloc(
        SCOREP_Location_GetMemoryPageManager( location,
                                              SCOREP_MEMORY_TYPE_PROFILING ),
        size );
    if ( !mem )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }
    return mem;
}


void
SCOREP_Memory_FreeProfileMem()
{
    // print mem usage statistics
    SCOREP_Allocator_Free(
        SCOREP_Location_GetMemoryPageManager(
            SCOREP_Location_GetCurrentCPULocation(),
            SCOREP_MEMORY_TYPE_PROFILING ) );
}


SCOREP_Allocator_MovableMemory
SCOREP_Memory_AllocForDefinitions( size_t size )
{
    // collect statistics
    if ( size == 0 )
    {
        return SCOREP_MOVABLE_NULL;
    }

    SCOREP_Allocator_MovableMemory mem =
        SCOREP_Allocator_AllocMovable( scorep_definitions_page_manager, size );
    if ( mem == SCOREP_MOVABLE_NULL )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }
    return mem;
}


void
SCOREP_Memory_FreeDefinitionMem()
{
    // print mem usage statistics
    SCOREP_Allocator_Free( scorep_definitions_page_manager );
}


void*
SCOREP_Memory_GetAddressFromMovableMemory( SCOREP_Allocator_MovableMemory movableMemory,
                                           SCOREP_Allocator_PageManager*  movablePageManager )
{
    return SCOREP_Allocator_GetAddressFromMovableMemory(
               movablePageManager,
               movableMemory );
}


SCOREP_Allocator_PageManager*
SCOREP_Memory_CreateMovedPagedMemory( void )
{
    return SCOREP_Allocator_CreateMovedPageManager( scorep_memory_allocator );
}

SCOREP_Allocator_PageManager*
SCOREP_Memory_GetLocalDefinitionPageManager()
{
    assert( scorep_memory_is_initialized );
    return scorep_definitions_page_manager;
}
