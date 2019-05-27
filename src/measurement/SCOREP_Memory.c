/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2015, 2017-2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 *
 */

#include <config.h>
#include <SCOREP_Memory.h>
#include <UTILS_Error.h>
#include <SCOREP_Mutex.h>
#include "scorep_environment.h"
#include "scorep_location_management.h"
#include "scorep_status.h"
#include "scorep_ipc.h"
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/* *INDENT-OFF* */
static void memory_dump_stats_aggr( void );
static void memory_dump_stats_full( void );
static void memory_dump_stats_common( const char* message, bool report );
/* *INDENT-ON* */

/*
 * Memory tracking uses SCOREP_NUMBER_OF_MEMORY_TYPES + SCORER_MEMORY_TRACKING_SHIFT
 * elements to resolve memory usage by the memory type. SCORER_MEMORY_TRACKING_SHIFT
 * is needed to track additionally DEFINITIONS, MAINTENANCE, and TOTAL memory usage.
 */
typedef enum memory_tracking
{
    SCORER_MEMORY_TRACKING_TOTAL = 0,
    SCORER_MEMORY_TRACKING_MAINTENANCE,
    SCORER_MEMORY_TRACKING_DEFINITIONS,
    SCORER_MEMORY_TRACKING_SHIFT
} memory_tracking;

/*
 * Global in order to have it allocated during initialization time and not
 * in the event of out-of-memory
 */
#define SCORER_MEMORY_STATS_SIZE ( SCOREP_NUMBER_OF_MEMORY_TYPES + SCORER_MEMORY_TRACKING_SHIFT )
static SCOREP_Allocator_PageManagerStats stats_min[ SCORER_MEMORY_STATS_SIZE ];
static SCOREP_Allocator_PageManagerStats stats_mean[ SCORER_MEMORY_STATS_SIZE ];
static SCOREP_Allocator_PageManagerStats stats_max[ SCORER_MEMORY_STATS_SIZE ];
static SCOREP_Allocator_PageManagerStats stats[ SCORER_MEMORY_STATS_SIZE ];


static SCOREP_Mutex memory_lock;
static SCOREP_Mutex out_of_memory_mutex;

/// The one and only allocator for the measurement and the adapters
static SCOREP_Allocator_Allocator* allocator;
static uint32_t                    total_memory;
static uint32_t                    page_size;

static bool is_initialized;
static bool out_of_memory;

static SCOREP_Allocator_PageManager* definitions_page_manager;

void
SCOREP_Memory_Initialize( uint64_t totalMemory,
                          uint64_t pageSize )
{
    if ( is_initialized )
    {
        return;
    }
    is_initialized = true;

    SCOREP_MutexCreate( &memory_lock );
    SCOREP_MutexCreate( &out_of_memory_mutex );

    if ( totalMemory > ( uint64_t )UINT32_MAX )
    {
        UTILS_WARNING( "Too many memory requested. "
                       "Score-P supports only up to, but not including, 4 GiB of "
                       "total memory per process. Reducing to its maximum value." );
        totalMemory = UINT32_MAX;
    }

    UTILS_BUG_ON( totalMemory < pageSize,
                  "Requested page size must fit into the total memory "
                  "(SCOREP_TOTAL_MEMORY=%" PRIu64 ", SCOREP_PAGE_SIZE=%" PRIu64 ")",
                  totalMemory, pageSize );

    total_memory = totalMemory;
    page_size    = pageSize;

    allocator = SCOREP_Allocator_CreateAllocator(
        &total_memory,
        &page_size,
        ( SCOREP_Allocator_Guard )SCOREP_MutexLock,
        ( SCOREP_Allocator_Guard )SCOREP_MutexUnlock,
        ( SCOREP_Allocator_GuardObject )memory_lock );

    UTILS_BUG_ON( !allocator,
                  "Cannot create memory manager for "
                  "SCOREP_TOTAL_MEMORY=%" PRIu64 " and SCOREP_PAGE_SIZE=%" PRIu64,
                  totalMemory, pageSize );

    assert( definitions_page_manager == 0 );
    definitions_page_manager = SCOREP_Allocator_CreatePageManager( allocator );
    UTILS_BUG_ON( !definitions_page_manager,
                  "Cannot create definitions manager." );
}


void
SCOREP_Memory_Finalize( void )
{
    if ( !is_initialized )
    {
        return;
    }
    is_initialized = false;

    assert( definitions_page_manager );
#if defined ( __INTEL_COMPILER ) && ( __INTEL_COMPILER < 1120 )
    // Do nothing here. Intel OpenMP RTL shuts down at the end of main
    // function, so omp_set/unset_lock, which is called after the end
    // of main from the atexit handler, causes segmentation fault. The
    // problem will be fixed in  Intel Compiler 11.1 update 6.
    // See http://software.intel.com/en-us/forums/showpost.php?p=110592
#else
    SCOREP_Allocator_DeletePageManager( definitions_page_manager );
#endif
    definitions_page_manager = 0;

    assert( allocator );
    SCOREP_Allocator_DeleteAllocator( allocator );
    allocator = 0;

    SCOREP_MutexDestroy( &out_of_memory_mutex );
    SCOREP_MutexDestroy( &memory_lock );
}

void
SCOREP_Memory_HandleOutOfMemory( void )
{
    /* let only first thread do the OOM handling */
    SCOREP_MutexLock( out_of_memory_mutex );
    if ( !out_of_memory )
    {
        out_of_memory = true;
    }
    else
    {
        abort();
    }

    UTILS_ERROR( SCOREP_ERROR_MEMORY_OUT_OF_PAGES,
                 "Out of memory. Please increase SCOREP_TOTAL_MEMORY=%zu and try again.",
                 total_memory );
    if ( SCOREP_Env_DoTracing() )
    {
        UTILS_ERROR( SCOREP_ERROR_MEMORY_OUT_OF_PAGES,
                     "Please ensure that there are at least 2MB available for each intended location." );
        uint32_t num_locations = SCOREP_Location_GetCountOfLocations();
        UTILS_ERROR( SCOREP_ERROR_MEMORY_OUT_OF_PAGES,
                     "Where there are currently %u locations in use in this failing process.",
                     num_locations );
    }
    /* For emergency report we dump full stats in order to avoid dead lock */
    fprintf( stderr, "[Score-P] Memory usage of rank %u\n",  SCOREP_Status_GetRank() );
    memory_dump_stats_common( "[Score-P] Memory used so far:", true );
    fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 "\n\n", "Number of locations", SCOREP_Location_GetCountOfLocations() );
    memory_dump_stats_full();

    abort();
    SCOREP_MutexUnlock( out_of_memory_mutex );
}

SCOREP_Allocator_PageManager*
SCOREP_Memory_CreateTracingPageManager( void )
{
    SCOREP_Allocator_PageManager* page_manager =
        SCOREP_Allocator_CreatePageManager( allocator );
    if ( !page_manager )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }
    return page_manager;
}


SCOREP_Allocator_PageManager*
SCOREP_Memory_CreatePageManager( void )
{
    SCOREP_Allocator_PageManager* page_manager = SCOREP_Allocator_CreatePageManager( allocator );
    if ( !page_manager )
    {
        SCOREP_Memory_HandleOutOfMemory();
    }
    return page_manager;
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
        SCOREP_Location_GetOrCreateMemoryPageManager( locationData,
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

static bool
free_memory_type_for_location( SCOREP_Location* location,
                               void*            arg )
{
    SCOREP_MemoryType type = *( SCOREP_MemoryType* )arg;
    SCOREP_Allocator_Free( SCOREP_Location_GetOrCreateMemoryPageManager( location, type ) );
    return false;
}

void
SCOREP_Memory_FreeMiscMem( void )
{
    // print mem usage statistics
    SCOREP_MemoryType type = SCOREP_MEMORY_TYPE_MISC;
    SCOREP_Location_ForAll( free_memory_type_for_location, &type );
}


void*
SCOREP_Location_AllocForProfile( SCOREP_Location* location, size_t size )
{
    // collect statistics
    if ( size == 0 )
    {
        return NULL;
    }

    void* mem = SCOREP_Allocator_Alloc(
        SCOREP_Location_GetOrCreateMemoryPageManager( location,
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
SCOREP_Memory_FreeProfileMem( SCOREP_Location* location )
{
    // print mem usage statistics
    SCOREP_MemoryType type = SCOREP_MEMORY_TYPE_PROFILING;
    free_memory_type_for_location( location, &type );
}


SCOREP_Allocator_MovableMemory
SCOREP_Memory_AllocForDefinitions( SCOREP_Location* location,
                                   size_t           size )
{
    // collect statistics
    if ( size == 0 )
    {
        return SCOREP_MOVABLE_NULL;
    }

    SCOREP_Allocator_PageManager* page_manager = definitions_page_manager;
    if ( location )
    {
        page_manager = SCOREP_Location_GetOrCreateMemoryPageManager(
            location,
            SCOREP_MEMORY_TYPE_DEFINITIONS );
    }

    SCOREP_Allocator_MovableMemory mem =
        SCOREP_Allocator_AllocMovable( page_manager, size );
    if ( !mem )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }
    return mem;
}


void
SCOREP_Memory_FreeDefinitionMem( void )
{
    // print mem usage statistics
    SCOREP_Allocator_Free( definitions_page_manager );
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
    SCOREP_Allocator_PageManager* page_manager = SCOREP_Allocator_CreateMovedPageManager( allocator );
    if ( !page_manager )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }
    return page_manager;
}

SCOREP_Allocator_PageManager*
SCOREP_Memory_GetLocalDefinitionPageManager( void )
{
    assert( is_initialized );
    return definitions_page_manager;
}


void
SCOREP_Memory_DumpStats( const char* message )
{
    if ( getenv( "SCOREP_DEVELOPMENT_MEMORY_STATS" ) != NULL )
    {
        if ( strcmp( getenv( "SCOREP_DEVELOPMENT_MEMORY_STATS" ), "aggregated" ) == 0 )
        {
            memory_dump_stats_common( message, SCOREP_Status_GetRank() == 0 );
            memory_dump_stats_aggr();
        }
        else
        if ( strcmp( getenv( "SCOREP_DEVELOPMENT_MEMORY_STATS" ), "full" ) == 0 )
        {
            memory_dump_stats_common( message, SCOREP_Status_GetRank() == 0 );
            memory_dump_stats_full();
        }
    }
}


static bool
memory_dump_for_location( SCOREP_Location* location,
                          void*            dummy )
{
    for ( int i = 0; i < SCOREP_NUMBER_OF_MEMORY_TYPES; ++i )
    {
        const SCOREP_Allocator_PageManager* page_manager =
            SCOREP_Location_GetMemoryPageManager( location, i );
        if ( !page_manager )
        {
            continue;
        }
        SCOREP_Allocator_GetPageManagerStats( page_manager, 0, &stats[ i + SCORER_MEMORY_TRACKING_SHIFT ] );
    }
    return false;
}


static void
memory_dump_stats_common( const char* message, bool report )
{
    if ( message != 0 )
    {
        if ( report )
        {
            fprintf( stderr, "%s\n", message );
        }
    }

    memset( stats_min, 0, sizeof( struct  SCOREP_Allocator_PageManagerStats ) * (  SCORER_MEMORY_STATS_SIZE ) );
    memset( stats_mean, 0, sizeof( struct SCOREP_Allocator_PageManagerStats ) * (  SCORER_MEMORY_STATS_SIZE ) );
    memset( stats_max, 0, sizeof( struct SCOREP_Allocator_PageManagerStats ) * (  SCORER_MEMORY_STATS_SIZE ) );
    memset( stats, 0, sizeof( struct SCOREP_Allocator_PageManagerStats ) * (  SCORER_MEMORY_STATS_SIZE ) );

    /* collect stats[i] */
    SCOREP_Allocator_GetPageStats( allocator, &stats[ SCORER_MEMORY_TRACKING_TOTAL ] );
    SCOREP_Allocator_GetPageManagerStats( 0, allocator, &stats[ SCORER_MEMORY_TRACKING_MAINTENANCE ] );
    if ( definitions_page_manager )
    {
        SCOREP_Allocator_GetPageManagerStats( definitions_page_manager, 0, &stats[ SCORER_MEMORY_TRACKING_DEFINITIONS ] );
    }
    SCOREP_Location_ForAll( memory_dump_for_location, NULL );

    if ( report )
    {
        /* header */
        fprintf( stderr, "[Score-P] Score-P runtime-management memory tracking:\n" );

        /* requested */
        fprintf( stderr,     "[Score-P] Memory: Requested:\n" );
        fprintf( stderr,     "[Score-P] %-55s %-15" PRIu32 "\n", "SCOREP_TOTAL_MEMORY [bytes]", total_memory );
        fprintf( stderr,     "[Score-P] %-55s %-15" PRIu32 "\n", "SCOREP_PAGE_SIZE [bytes]", page_size );
        fprintf( stderr,     "[Score-P] %-55s %-15" PRIu32 "\n\n", "Number of pages of size SCOREP_PAGE_SIZE",
                 SCOREP_Allocator_GetMaxNumberOfPages( allocator ) );
    }
}


static const char*
memory_type_2_string( int memoryType )
{
    switch ( memoryType - SCORER_MEMORY_TRACKING_SHIFT  )
    {
        case -( SCORER_MEMORY_TRACKING_SHIFT - SCORER_MEMORY_TRACKING_MAINTENANCE ):
            return "Maintenance";
        case -( SCORER_MEMORY_TRACKING_SHIFT - SCORER_MEMORY_TRACKING_DEFINITIONS ):
            return "Definitions";
        case SCOREP_MEMORY_TYPE_MISC:
            return "Location-Misc";
        case SCOREP_MEMORY_TYPE_DEFINITIONS:
            return "Location-Definitions";
        case SCOREP_MEMORY_TYPE_PROFILING:
            return "Location-Profiling";
        case SCOREP_MEMORY_TYPE_TRACING_EVENTS:
            return "Location-Tracing (events)";
        default:
            UTILS_FATAL( "Unknown memory type." );
            return "Silence compiler warning";
    }
}


static void
memory_reduce_stats( SCOREP_Allocator_PageManagerStats* totalStats,
                     SCOREP_Allocator_PageManagerStats* totalStatsMin,
                     SCOREP_Allocator_PageManagerStats* totalStatsMean,
                     SCOREP_Allocator_PageManagerStats* totalStatsMax )
{
    int size = SCOREP_Ipc_GetSize();
    SCOREP_Ipc_Reduce( &( ( *totalStats ).pages_allocated ),
                       &( ( *totalStatsMin ).pages_allocated ),
                       1,
                       SCOREP_IPC_UINT32_T,
                       SCOREP_IPC_MIN,
                       0 );
    SCOREP_Ipc_Reduce( &( ( *totalStats ).pages_used ),
                       &( ( *totalStatsMin ).pages_used ),
                       1,
                       SCOREP_IPC_UINT32_T,
                       SCOREP_IPC_MIN,
                       0 );
    SCOREP_Ipc_Reduce( &( ( *totalStats ).memory_allocated ),
                       &( ( *totalStatsMin ).memory_allocated ),
                       1,
                       SCOREP_IPC_UINT64_T,
                       SCOREP_IPC_MIN,
                       0 );
    SCOREP_Ipc_Reduce( &( ( *totalStats ).memory_used ),
                       &( ( *totalStatsMin ).memory_used ),
                       1,
                       SCOREP_IPC_UINT64_T,
                       SCOREP_IPC_MIN,
                       0 );
    SCOREP_Ipc_Reduce( &( ( *totalStats ).memory_available ),
                       &( ( *totalStatsMin ).memory_available ),
                       1,
                       SCOREP_IPC_UINT64_T,
                       SCOREP_IPC_MIN,
                       0 );

    SCOREP_Ipc_Reduce( &( ( *totalStats ).pages_allocated ),
                       &( ( *totalStatsMax ).pages_allocated ),
                       1,
                       SCOREP_IPC_UINT32_T,
                       SCOREP_IPC_MAX,
                       0 );
    SCOREP_Ipc_Reduce( &( ( *totalStats ).pages_used ),
                       &( ( *totalStatsMax ).pages_used ),
                       1,
                       SCOREP_IPC_UINT32_T,
                       SCOREP_IPC_MAX,
                       0 );
    SCOREP_Ipc_Reduce( &( ( *totalStats ).memory_allocated ),
                       &( ( *totalStatsMax ).memory_allocated ),
                       1,
                       SCOREP_IPC_UINT64_T,
                       SCOREP_IPC_MAX,
                       0 );
    SCOREP_Ipc_Reduce( &( ( *totalStats ).memory_used ),
                       &( ( *totalStatsMax ).memory_used ),
                       1,
                       SCOREP_IPC_UINT64_T,
                       SCOREP_IPC_MAX,
                       0 );
    SCOREP_Ipc_Reduce( &( ( *totalStats ).memory_available ),
                       &( ( *totalStatsMax ).memory_available ),
                       1,
                       SCOREP_IPC_UINT64_T,
                       SCOREP_IPC_MAX,
                       0 );

    SCOREP_Ipc_Reduce( &( ( *totalStats ).pages_allocated ),
                       &( ( *totalStatsMean ).pages_allocated ),
                       1,
                       SCOREP_IPC_UINT32_T,
                       SCOREP_IPC_SUM,
                       0 );
    totalStatsMean->pages_allocated /= size;

    SCOREP_Ipc_Reduce( &( ( *totalStats ).pages_used ),
                       &( ( *totalStatsMean ).pages_used ),
                       1,
                       SCOREP_IPC_UINT32_T,
                       SCOREP_IPC_SUM,
                       0 );
    totalStatsMean->pages_used /= size;

    SCOREP_Ipc_Reduce( &( ( *totalStats ).memory_allocated ),
                       &( ( *totalStatsMean ).memory_allocated ),
                       1,
                       SCOREP_IPC_UINT64_T,
                       SCOREP_IPC_SUM,
                       0 );
    totalStatsMean->memory_allocated /= size;

    SCOREP_Ipc_Reduce( &( ( *totalStats ).memory_used ),
                       &( ( *totalStatsMean ).memory_used ),
                       1,
                       SCOREP_IPC_UINT64_T,
                       SCOREP_IPC_SUM,
                       0 );
    totalStatsMean->memory_used /= size;

    SCOREP_Ipc_Reduce( &( ( *totalStats ).memory_available ),
                       &( ( *totalStatsMean ).memory_available ),
                       1,
                       SCOREP_IPC_UINT64_T,
                       SCOREP_IPC_SUM,
                       0 );
    totalStatsMean->memory_available /= size;
}


static void
memory_dump_stats_aggr( void )
{
    /* get stat's min/max/mean */
    for ( int i = 0; i < SCOREP_NUMBER_OF_MEMORY_TYPES + SCORER_MEMORY_TRACKING_SHIFT; ++i )
    {
        memory_reduce_stats( &( stats[ i ] ),
                             &( stats_min[ i ] ),
                             &( stats_mean[ i ] ),
                             &( stats_max[ i ] ) );
    }

    if ( SCOREP_Status_GetRank() != 0 )
    {
        /* let only rank 0 do the dump */
        return;
    }

    if ( SCOREP_Status_IsMpp() )
    {
        fprintf( stderr, "%101s\n", "min             mean            max" );
    }

    /* total pages */
    fprintf( stderr, "[Score-P] Memory: Pages overall\n" );
    if ( SCOREP_Status_IsMpp() )
    {
        fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 " %-15" PRIu32 " %-15"  PRIu32 "\n", "Maximum number of pages used at a time",
                 stats_min[ SCORER_MEMORY_TRACKING_TOTAL ].pages_allocated,
                 stats_mean[ SCORER_MEMORY_TRACKING_TOTAL ].pages_allocated,
                 stats_max[ SCORER_MEMORY_TRACKING_TOTAL ].pages_allocated );
        fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 " %-15" PRIu32 " %-15"  PRIu32 "\n\n", "Number of pages currently allocated",
                 stats_min[ SCORER_MEMORY_TRACKING_TOTAL ].pages_used,
                 stats_mean[ SCORER_MEMORY_TRACKING_TOTAL ].pages_used,
                 stats_max[ SCORER_MEMORY_TRACKING_TOTAL ].pages_used );
    }
    else
    {
        fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 "\n", "Maximum number of pages allocated at a time",
                 stats[ SCORER_MEMORY_TRACKING_TOTAL ].pages_allocated  );
        fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 "\n\n", "Number of pages currently allocated",
                 stats[ SCORER_MEMORY_TRACKING_TOTAL ].pages_used );
    }

    /* maintenance, definitions, location-memory */
    for ( int i = 1; i < SCOREP_NUMBER_OF_MEMORY_TYPES + SCORER_MEMORY_TRACKING_SHIFT; ++i )
    {
        /* header */
        fprintf( stderr, "[Score-P] Memory: %s\n", memory_type_2_string( i ) );

        /* data */
        if ( SCOREP_Status_IsMpp() )
        {
            fprintf( stderr, "[Score-P] %-55s %-15zu %-15zu %-15zu\n", "Memory allocated [bytes]",
                     stats_min[ i ].memory_allocated,
                     stats_mean[ i ].memory_allocated,
                     stats_max[ i ].memory_allocated );
            fprintf( stderr, "[Score-P] %-55s %-15zu %-15zu %-15zu\n", "Memory used [bytes]",
                     stats_min[ i ].memory_used,
                     stats_mean[ i ].memory_used,
                     stats_max[ i ].memory_used );
            fprintf( stderr, "[Score-P] %-55s %-15zu %-15zu %-15zu\n", "Memory available [bytes]",
                     stats_min[ i ].memory_available,
                     stats_mean[ i ].memory_available,
                     stats_max[ i ].memory_available  );
            fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 " %-15" PRIu32 " %-15"  PRIu32 "\n", "Number of pages allocated",
                     stats_min[ i ].pages_allocated,
                     stats_mean[ i ].pages_allocated,
                     stats_max[ i ].pages_allocated );
            fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 " %-15" PRIu32 " %-15"  PRIu32 "\n\n", "Number of pages used",
                     stats_min[ i ].pages_used,
                     stats_mean[ i ].pages_used,
                     stats_max[ i ].pages_used );
        }
        else
        {
            fprintf( stderr, "[Score-P] %-55s %-15zu\n", "Memory allocated [bytes]",
                     stats[ i ].memory_allocated );
            fprintf( stderr, "[Score-P] %-55s %-15zu\n", "Memory used [bytes]",
                     stats[ i ].memory_used );
            fprintf( stderr, "[Score-P] %-55s %-15zu\n", "Memory available [bytes]",
                     stats[ i ].memory_available );
            fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 "\n", "Number of pages allocated",
                     stats[ i ].pages_allocated  );
            fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 "\n\n", "Number of pages used",
                     stats[ i ].pages_used );
        }
    }
}


static void
memory_dump_stats_full( void )
{
    fprintf( stderr, "[Score-P] Memory: Pages\n" );
    fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 "\n", "Maximum number of pages allocated at a time",
             stats[ SCORER_MEMORY_TRACKING_TOTAL ].pages_allocated  );
    fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 "\n\n", "Number of pages currently allocated",
             stats[ SCORER_MEMORY_TRACKING_TOTAL ].pages_used );

    /* maintenance, definitions, location-memory */
    for ( int i = 1; i < SCOREP_NUMBER_OF_MEMORY_TYPES + SCORER_MEMORY_TRACKING_SHIFT; ++i )
    {
        /* header */
        fprintf( stderr, "[Score-P] Memory: %s\n", memory_type_2_string( i ) );

        /* data */
        fprintf( stderr, "[Score-P] %-55s %-15zu\n", "Memory allocated [bytes]",
                 stats[ i ].memory_allocated );
        fprintf( stderr, "[Score-P] %-55s %-15zu\n", "Memory used [bytes]",
                 stats[ i ].memory_used );
        fprintf( stderr, "[Score-P] %-55s %-15zu\n", "Memory available [bytes]",
                 stats[ i ].memory_available );
        fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 "\n", "Number of pages allocated",
                 stats[ i ].pages_allocated  );
        fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 "\n\n", "Number of pages used",
                 stats[ i ].pages_used );
    }
}
