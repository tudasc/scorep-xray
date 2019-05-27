/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2016, 2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */


#include <config.h>
#include "scorep_location_management.h"

#include <inttypes.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <SCOREP_Mutex.h>
#include "scorep_subsystem_management.h"
#include "scorep_substrates_definition.h"
#include "scorep_status.h"
#include <SCOREP_Thread_Mgmt.h>
#include <SCOREP_Definitions.h>

#include "scorep_events_common.h"

#include <UTILS_Error.h>

#if HAVE( THREAD_LOCAL_STORAGE )

/*  Declare and initialize thread local storage for in-measurement counter */
SCOREP_THREAD_LOCAL_STORAGE_SPECIFIER volatile sig_atomic_t scorep_in_measurement
SCOREP_THREAD_LOCAL_STORAGE_MODEL( "initial-exec" ) = 0;

#if HAVE( SAMPLING_SUPPORT )

/*  Declare and initialize thread local storage for in-signal-context counter */
SCOREP_THREAD_LOCAL_STORAGE_SPECIFIER volatile sig_atomic_t scorep_in_signal_context
SCOREP_THREAD_LOCAL_STORAGE_MODEL( "initial-exec" ) = 0;

#endif

volatile sig_atomic_t*
scorep_get_in_measurement( void )
{
    return &scorep_in_measurement;
}

#endif /* HAVE( THREAD_LOCAL_STORAGE ) */

// locations live inside SCOREP_Thread_ThreadPrivateData, may be referenced by
// multiple ones.
struct SCOREP_Location
{
    uint64_t                      last_timestamp;
    SCOREP_LocationType           type;
    SCOREP_LocationHandle         location_handle;
    SCOREP_Allocator_PageManager* page_managers[ SCOREP_NUMBER_OF_MEMORY_TYPES ];
    void*                         substrate_data[ SCOREP_SUBSTRATES_NUM_SUBSTRATES ];

    SCOREP_Location*              parent;
    SCOREP_Location*              next;    // store location objects in list for easy cleanup

    /** Flexible array member with length scorep_subsystems_get_number() */
    void* per_subsystem_data[];
};
static struct SCOREP_Location*  location_list_head;
static struct SCOREP_Location** location_list_tail = &location_list_head;

/* We defer all new locations until the SCOREP_Location_ActivateInitLocations() call */
static bool defer_init_locations = true;

static SCOREP_Mutex     scorep_location_list_mutex;
static SCOREP_Mutex     per_process_metrics_location_mutex;
static SCOREP_Location* per_process_metrics_location;

void
SCOREP_Location_Initialize( void )
{
    SCOREP_ErrorCode result = SCOREP_MutexCreate( &scorep_location_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "" );
    result = SCOREP_MutexCreate( &per_process_metrics_location_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "" );
}


SCOREP_Location*
scorep_location_create_location( SCOREP_LocationType type,
                                 const char*         name )
{
    SCOREP_Location* new_location;
    size_t           total_memory = sizeof( *new_location )
                                    + ( sizeof( *new_location->per_subsystem_data )
                                        * scorep_subsystems_get_number() );

    /* Trigger the definition for this new location and let it allocate
       also the SCOREP_Location, will abort on out-of-memory */
    SCOREP_LocationHandle location_handle = SCOREP_Definitions_NewLocation(
        type,
        name,
        total_memory,
        ( void* )&new_location );
    memset( new_location, 0, total_memory );
    new_location->location_handle = location_handle;

    new_location->type = type;
    new_location->next = NULL;

    SCOREP_ErrorCode result = SCOREP_MutexLock( scorep_location_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "" );

    *location_list_tail = new_location;
    location_list_tail  = &new_location->next;

    result = SCOREP_MutexUnlock( scorep_location_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "" );

    return new_location;
}

SCOREP_Location*
SCOREP_Location_CreateNonCPULocation( SCOREP_Location*    parent,
                                      SCOREP_LocationType type,
                                      const char*         name )
{
    UTILS_BUG_ON( type == SCOREP_LOCATION_TYPE_CPU_THREAD,
                  "SCOREP_CreateNonCPULocation() does not support creation of CPU locations." );

    SCOREP_Location* new_location = scorep_location_create_location( type, name );
    new_location->parent = parent;
    if ( !defer_init_locations )
    {
        scorep_subsystems_initialize_location( new_location, parent );
    }

    return new_location;
}


SCOREP_Location*
SCOREP_Location_CreateCPULocation( const char* name )
{
    SCOREP_Location* new_location =
        scorep_location_create_location( SCOREP_LOCATION_TYPE_CPU_THREAD, name );
#if HAVE( THREAD_LOCAL_STORAGE )
    sig_atomic_t touch = scorep_in_measurement;

#if HAVE( SAMPLING_SUPPORT )
    touch = scorep_in_signal_context;
#endif

#endif

    return new_location;
}

char scorep_per_process_metrics_location_name[] = "Per process metrics";

SCOREP_Location*
SCOREP_Location_AcquirePerProcessMetricsLocation( uint64_t* timestamp )
{
    SCOREP_ErrorCode result = SCOREP_MutexLock( per_process_metrics_location_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "Cannot lock per_process_metrics_location_mutex" );

    if ( !per_process_metrics_location )
    {
        per_process_metrics_location = SCOREP_Location_CreateNonCPULocation(
            SCOREP_Location_GetCurrentCPULocation(),
            SCOREP_LOCATION_TYPE_METRIC,
            scorep_per_process_metrics_location_name );
    }

    if ( timestamp )
    {
        *timestamp = scorep_get_timestamp( per_process_metrics_location );
    }

    return per_process_metrics_location;
}


void
SCOREP_Location_ReleasePerProcessMetricsLocation( void )
{
    SCOREP_ErrorCode result = SCOREP_MutexUnlock( per_process_metrics_location_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "Cannot unlock per_process_metrics_location_mutex" );
}


void
SCOREP_Location_ActivateInitLocations( void )
{
    UTILS_ASSERT( defer_init_locations );

    /* Note: all new locations created in any of the init-location callbacks
     *       are appended to the location list and the calls to there
     *       init-location are also postponed. thats why defer_init_locations is
     *       set to false at the end.
     */

    SCOREP_Location* location = location_list_head;
    while ( location )
    {
        scorep_subsystems_initialize_location( location, location->parent );

        if ( location->type == SCOREP_LOCATION_TYPE_CPU_THREAD )
        {
            SCOREP_Thread_ActivateLocation( location,
                                            location->parent );
        }

        /* We need to take the next pointer after the initialization of the
         * current location, as it may have created a new one, which alters the
         * ->next pointer of the current one.
         */
        location = location->next;
    }

    defer_init_locations = false;
}


uint32_t
SCOREP_Location_GetCountOfLocations( void )
{
    /* read should be atomic, thus not locking needed */
    return scorep_local_definition_manager.location.counter;
}


uint32_t
SCOREP_Location_GetId( SCOREP_Location* locationData )
{
    /*
     * We use the definition sequence number as the local ID
     */
    return SCOREP_LOCAL_HANDLE_TO_ID( locationData->location_handle, Location );
}


SCOREP_LocationType
SCOREP_Location_GetType( SCOREP_Location* locationData )
{
    return locationData->type;
}


void*
SCOREP_Location_GetSubsystemData( SCOREP_Location* locationData,
                                  size_t           subsystem_id )
{
    UTILS_BUG_ON( subsystem_id >= scorep_subsystems_get_number(), "subsystem_id out of range." );

    return locationData->per_subsystem_data[ subsystem_id ];
}


void
SCOREP_Location_SetSubsystemData( SCOREP_Location* locationData,
                                  size_t           subsystem_id,
                                  void*            subsystem_data )
{
    UTILS_BUG_ON( subsystem_id >= scorep_subsystems_get_number(), "subsystem_id out of range." );

    locationData->per_subsystem_data[ subsystem_id ] = subsystem_data;
}


SCOREP_LocationHandle
SCOREP_Location_GetLocationHandle( SCOREP_Location* locationData )
{
    return locationData->location_handle;
}


void
SCOREP_Location_FinalizeLocations( void )
{
    UTILS_BUG_ON( SCOREP_Thread_InParallel(), "Threads other than the master active." );

    SCOREP_Location* location = location_list_head;
    while ( location )
    {
        SCOREP_Location* tmp = location->next;

        scorep_subsystems_finalize_location( location );
        SCOREP_Memory_DeletePageManagers( location->page_managers );
        location = tmp;
    }
}


void
SCOREP_Location_Finalize( void )
{
    UTILS_BUG_ON( SCOREP_Thread_InParallel(), "Threads other than the master active." );

    location_list_head = 0;
    location_list_tail = &location_list_head;

    SCOREP_ErrorCode result = SCOREP_MutexDestroy( &scorep_location_list_mutex );
    UTILS_ASSERT( result == SCOREP_SUCCESS );

    result = SCOREP_MutexDestroy( &per_process_metrics_location_mutex );
    UTILS_ASSERT( result == SCOREP_SUCCESS );
}


SCOREP_Allocator_PageManager*
SCOREP_Location_GetMemoryPageManager( SCOREP_Location*  locationData,
                                      SCOREP_MemoryType type )
{
    UTILS_BUG_ON( 0 > type || type >= SCOREP_NUMBER_OF_MEMORY_TYPES,
                  "Invalid memory type given." );
    return locationData->page_managers[ type ];
}

SCOREP_Allocator_PageManager*
SCOREP_Location_GetOrCreateMemoryPageManager( SCOREP_Location*  locationData,
                                              SCOREP_MemoryType type )
{
    UTILS_BUG_ON( 0 > type || type >= SCOREP_NUMBER_OF_MEMORY_TYPES,
                  "Invalid memory type given." );
    /* Create page_manager on the fly */
    if ( locationData->page_managers[ type ] == NULL )
    {
        locationData->page_managers[ type ] = SCOREP_Memory_CreatePageManager();
    }
    return locationData->page_managers[ type ];
}


void*
SCOREP_Location_GetSubstrateData( SCOREP_Location* location,
                                  size_t           substrateId )
{
    return location->substrate_data[ substrateId ];
}


void
SCOREP_Location_SetSubstrateData( SCOREP_Location* location,
                                  void*            substrateData,
                                  size_t           substrateId )
{
    location->substrate_data[ substrateId ] = substrateData;
}

uint64_t
SCOREP_Location_CalculateGlobalId( int rank, uint32_t thread )
{
    return ( ( uint64_t )thread << 32 ) | ( uint64_t )rank;
}


uint64_t
SCOREP_Location_GetGlobalId( SCOREP_Location* locationData )
{
    UTILS_BUG_ON( !SCOREP_Status_IsMppInitialized(),
                  "Should only be called after the MPP was initialized." );

    return SCOREP_Location_CalculateGlobalId( SCOREP_Status_GetRank(), SCOREP_Location_GetId( locationData ) );
}

uint32_t
SCOREP_Location_CalculateLocalId( uint64_t globalId )
{
    return ( uint32_t )( (  globalId ) >> 32 );
}


uint32_t
SCOREP_Location_CalculateRank( uint64_t globalId )
{
    return ( uint32_t )( ( globalId ) & 0xFFFFFFFFu );
}

void
SCOREP_Location_SetLastTimestamp( SCOREP_Location* location,
                                  uint64_t         timestamp )
{
    UTILS_BUG_ON( timestamp < location->last_timestamp,
                  "Wrong timestamp order on location %" PRIu32 ": %" PRIu64 " (last recorded) > %" PRIu64 " (current). "
                  "This might be an indication of thread migration. Please pin your threads. "
                  "Using a SCOREP_TIMER different from tsc might also help.",
                  SCOREP_Location_GetId( location ),
                  location->last_timestamp,
                  timestamp );
    location->last_timestamp = timestamp;
}


uint64_t
SCOREP_Location_GetLastTimestamp( SCOREP_Location* locationData )
{
    return locationData->last_timestamp;
}

void
SCOREP_Location_EnsureGlobalId( SCOREP_Location* location )
{
    SCOREP_LocationDef* location_definition =
        SCOREP_LOCAL_HANDLE_DEREF( location->location_handle, Location );
    if ( location_definition->global_location_id == UINT64_MAX )
    {
        location_definition->global_location_id =
            SCOREP_Location_GetGlobalId( location );

        SCOREP_CALL_SUBSTRATE_MGMT( EnsureGlobalId, ENSURE_GLOBAL_ID, ( location ) );
    }
}


void
SCOREP_Location_FinalizeDefinitions( void )
{
    SCOREP_Location* location = location_list_head;
    while ( location )
    {
        SCOREP_Location_EnsureGlobalId( location );
        location = location->next;
    }
}

void
SCOREP_Location_ForAll( bool ( * cb )( SCOREP_Location*,
                                       void* ),
                        void*    data )
{
    UTILS_BUG_ON( cb == NULL, "Callback invalid." );

    for ( SCOREP_Location* location_data = location_list_head;
          location_data;
          location_data = location_data->next )
    {
        if ( cb( location_data, data ) )
        {
            break;
        }
    }
}


const char*
SCOREP_Location_GetName( SCOREP_Location* locationData )
{
    return SCOREP_LOCAL_HANDLE_DEREF(
        SCOREP_LOCAL_HANDLE_DEREF(
            locationData->location_handle,
            Location )->name_handle,
        String )->string_data;
}
