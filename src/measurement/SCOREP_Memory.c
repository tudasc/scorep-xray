/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012, 2022,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014, 2019-2020,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2015, 2017-2019, 2022,
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
#include <UTILS_Mutex.h>
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
    SCORER_MEMORY_TRACKING_TRACING_EVENTS,
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


static UTILS_Mutex memory_lock;
static UTILS_Mutex out_of_memory_mutex;

/// The one and only allocator for the measurement and the adapters
static SCOREP_Allocator_Allocator* allocator;
static uint32_t                    total_memory;
static uint32_t                    page_size;

static bool is_initialized;
static bool out_of_memory;

SCOREP_Allocator_PageManager* scorep_definitions_page_manager = NULL;

/*
 * All tracing event page managers. Memory stats needs to access these,
 * but they cannot be assigned reliable to the location.
 */
struct tracing_page_manager_list
{
    struct tracing_page_manager_list* next;
    SCOREP_Allocator_PageManager*     page_manager;
};
/* protected by memory_lock */
static struct tracing_page_manager_list* tracing_page_managers_head;

void
SCOREP_Memory_Initialize( uint64_t totalMemory,
                          uint64_t pageSize )
{
    if ( is_initialized )
    {
        return;
    }
    is_initialized = true;

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
        ( SCOREP_Allocator_Guard )UTILS_MutexLock,
        ( SCOREP_Allocator_Guard )UTILS_MutexUnlock,
        ( SCOREP_Allocator_GuardObject )( &memory_lock ) );

    UTILS_BUG_ON( !allocator,
                  "Cannot create memory manager for "
                  "SCOREP_TOTAL_MEMORY=%" PRIu64 " and SCOREP_PAGE_SIZE=%" PRIu64,
                  totalMemory, pageSize );

    assert( scorep_definitions_page_manager == NULL );
    scorep_definitions_page_manager = SCOREP_Allocator_CreatePageManager( allocator );
    UTILS_BUG_ON( !scorep_definitions_page_manager,
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

    assert( scorep_definitions_page_manager );
    SCOREP_Allocator_DeletePageManager( scorep_definitions_page_manager );
    scorep_definitions_page_manager = NULL;

    assert( allocator );
    SCOREP_Allocator_DeleteAllocator( allocator );
    allocator = 0;
}

void
SCOREP_Memory_HandleOutOfMemory( void )
{
    /* let only first thread do the OOM handling */
    UTILS_MutexLock( &out_of_memory_mutex );
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
    UTILS_MutexUnlock( &out_of_memory_mutex );
}

SCOREP_Allocator_PageManager*
SCOREP_Memory_CreatePageManager( void )
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
SCOREP_Memory_CreateTracingPageManager( bool forEvents )
{
    SCOREP_Allocator_PageManager* page_manager =
        SCOREP_Memory_CreatePageManager();

    if ( forEvents )
    {
        /* do not allocate under the memory_lock */
        struct tracing_page_manager_list* new_entry =
            SCOREP_Memory_AllocForMisc( sizeof( *new_entry ) );

        new_entry->page_manager = page_manager;

        UTILS_MutexLock( &memory_lock );
        new_entry->next            = tracing_page_managers_head;
        tracing_page_managers_head = new_entry;
        UTILS_MutexUnlock( &memory_lock );
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
            SCOREP_Allocator_DeletePageManager( pageManagers[ i ] );
        }
    }
}


void
SCOREP_Memory_DeleteTracingPageManager( SCOREP_Allocator_PageManager* pageManager,
                                        bool                          forEvents )
{
    if ( forEvents )
    {
        /* only called once per location at pre-unify, thus no reuse possible */

        UTILS_MutexLock( &memory_lock );
        struct tracing_page_manager_list** it = &tracing_page_managers_head;
        while ( *it )
        {
            if ( ( *it )->page_manager == pageManager )
            {
                break;
            }
            it = &( *it )->next;
        }
        if ( *it )
        {
            /* Remove element from list */
            *it = ( *it )->next;
        }
        UTILS_MutexUnlock( &memory_lock );
    }

    SCOREP_Allocator_DeletePageManager( pageManager );
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


void*
SCOREP_Location_AlignedAllocForMisc( SCOREP_Location* locationData, size_t alignment, size_t size )
{
    if ( size == 0 )
    {
        return NULL;
    }
    if ( alignment < SCOREP_ALLOCATOR_ALIGNMENT )
    {
        return NULL;
    }
    /* alignment power of two */
    if ( ( alignment & ( alignment - 1 ) ) != 0 )
    {
        return NULL;
    }

    void* mem = SCOREP_Allocator_AlignedAlloc(
        SCOREP_Location_GetOrCreateMemoryPageManager( locationData,
                                                      SCOREP_MEMORY_TYPE_MISC ),
        alignment,
        size );
    if ( !mem )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }
    return mem;
}


void*
SCOREP_Memory_AlignedAllocForMisc( size_t alignment, size_t size )
{
    return SCOREP_Location_AlignedAllocForMisc( SCOREP_Location_GetCurrentCPULocation(),
                                                alignment,
                                                size );
}


/* Support AlignedMalloc/Free alignment up to UINT16_MAX. To free()
   aligned memory we need to access the address returned by
   malloc(). Therefore, store the offset as uint16_t in front of the
   aligned address. */
typedef uint16_t alignment_offset_t;

void*
SCOREP_Memory_AlignedMalloc( size_t alignment,
                             size_t size )
{
    if ( size == 0 )
    {
        return NULL;
    }
    if ( alignment < SCOREP_ALLOCATOR_ALIGNMENT )
    {
        return NULL;
    }
    if ( alignment > UINT16_MAX )
    {
        return NULL;
    }
    /* alignment power of two */
    if ( ( alignment & ( alignment - 1 ) ) != 0 )
    {
        return NULL;
    }

    void* aligned = NULL;

    /* Allocate extra bytes for storing the offset value and for
       the alignment. */
    void* raw = malloc( size + sizeof( alignment_offset_t ) + alignment - 1 );
    if ( raw )
    {
        #define roundupto( x, to ) ( ( ( intptr_t )( x ) + ( ( intptr_t )( to ) - 1 ) ) & ~( ( intptr_t )( to ) - 1 ) )
        aligned = ( void* )roundupto( ( ( intptr_t )raw + sizeof( alignment_offset_t ) ),
                                      alignment );
        #undef roundupto

        /* Calculate the offset and store it in front of the
           aligned pointer. */
        *( ( alignment_offset_t* )aligned - 1 ) =
            ( alignment_offset_t )( ( uintptr_t )aligned - ( uintptr_t )raw );
    }
    else if ( errno == ENOMEM )
    {
        /* aborts */
        SCOREP_Memory_HandleOutOfMemory();
    }
    return aligned;
}


void
SCOREP_Memory_AlignedFree( void* aligned )
{
    if ( aligned == NULL )
    {
        return;
    }

    /* Get the offset from the uint16_t stored in front of the aligned
       pointer to calculate the address of the malloced/raw pointer
       that can be passed to free(). */
    alignment_offset_t offset = *( ( alignment_offset_t* )aligned - 1 );
    void*              raw    = ( void* )( ( uint8_t* )aligned - offset );
    free( raw );
}


static bool
free_memory_type_for_location( SCOREP_Location* location,
                               void*            arg )
{
    SCOREP_MemoryType             type         = *( SCOREP_MemoryType* )arg;
    SCOREP_Allocator_PageManager* page_manager =
        SCOREP_Location_GetMemoryPageManager( location, type );
    if ( page_manager )
    {
        SCOREP_Allocator_Free( page_manager );
    }
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

    SCOREP_Allocator_PageManager* page_manager = scorep_definitions_page_manager;
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
    SCOREP_Allocator_Free( scorep_definitions_page_manager );
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
        SCOREP_Allocator_PageManager* page_manager =
            SCOREP_Location_GetMemoryPageManager( location, i );
        if ( !page_manager )
        {
            continue;
        }
        SCOREP_Allocator_GetPageManagerStats( page_manager, &stats[ i + SCORER_MEMORY_TRACKING_SHIFT ] );
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
    SCOREP_Allocator_GetStats( allocator,
                               &stats[ SCORER_MEMORY_TRACKING_TOTAL ],
                               &stats[ SCORER_MEMORY_TRACKING_MAINTENANCE ] );
    if ( scorep_definitions_page_manager )
    {
        SCOREP_Allocator_GetPageManagerStats( scorep_definitions_page_manager, &stats[ SCORER_MEMORY_TRACKING_DEFINITIONS ] );
    }
    SCOREP_Location_ForAll( memory_dump_for_location, NULL );

    /* Tracing page managers are collected separately */
    struct tracing_page_manager_list* tracing_page_manager = tracing_page_managers_head;
    while ( tracing_page_manager )
    {
        SCOREP_Allocator_GetPageManagerStats( tracing_page_manager->page_manager,
                                              &stats[ SCORER_MEMORY_TRACKING_TRACING_EVENTS ] );
        tracing_page_manager = tracing_page_manager->next;
    }

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
        case -( SCORER_MEMORY_TRACKING_SHIFT - SCORER_MEMORY_TRACKING_TRACING_EVENTS ):
            return "Tracing (events)";
        case SCOREP_MEMORY_TYPE_MISC:
            return "Location-Misc";
        case SCOREP_MEMORY_TYPE_DEFINITIONS:
            return "Location-Definitions";
        case SCOREP_MEMORY_TYPE_PROFILING:
            return "Location-Profiling";
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
    for ( int i = 0; i < SCORER_MEMORY_STATS_SIZE; ++i )
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
    for ( int i = 1; i < SCORER_MEMORY_STATS_SIZE; ++i )
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
            fprintf( stderr, "[Score-P] %-55s %-15zu %-15zu %-15zu\n", "Alignment loss [bytes]",
                     stats_min[ i ].memory_alignment_loss,
                     stats_mean[ i ].memory_alignment_loss,
                     stats_max[ i ].memory_alignment_loss  );
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
            fprintf( stderr, "[Score-P] %-55s %-15zu\n", "Alignment loss [bytes]",
                     stats[ i ].memory_alignment_loss  );
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
    for ( int i = 1; i < SCORER_MEMORY_STATS_SIZE; ++i )
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
        fprintf( stderr, "[Score-P] %-55s %-15zu\n", "Alignment loss [bytes]",
                 stats[ i ].memory_alignment_loss  );
        fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 "\n", "Number of pages allocated",
                 stats[ i ].pages_allocated  );
        fprintf( stderr, "[Score-P] %-55s %-15" PRIu32 "\n\n", "Number of pages used",
                 stats[ i ].pages_used );
    }
}


uint64_t
SCOREP_Memory_GetPageSize( void )
{
    return SCOREP_Env_GetPageSize();
}


uint32_t
SCOREP_Memory_GetDefinitionHandlesBitWidth( void )
{
    return SCOREP_Allocator_GetPageOffsetBitWidth( allocator ) +
           SCOREP_Allocator_GetNPagesBitWidth( allocator );
}
