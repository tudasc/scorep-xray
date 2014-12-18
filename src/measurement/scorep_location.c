/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
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
#include "scorep_location.h"

#include <assert.h>
#include <inttypes.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <SCOREP_Mutex.h>
#include "scorep_subsystem.h"
#include "scorep_status.h"
#include <SCOREP_Thread_Mgmt.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Definitions.h>

#include <UTILS_Error.h>

#if HAVE( THREAD_LOCAL_STORAGE )
/*  Declare and initialize thread local storage for in-measurement counter */
__thread volatile sig_atomic_t scorep_in_measurement = 0;
#endif /* HAVE( THREAD_LOCAL_STORAGE ) */

// locations live inside SCOREP_Thread_ThreadPrivateData, may be referenced by
// multiple ones.
struct SCOREP_Location
{
    uint64_t                      last_timestamp;
    SCOREP_LocationType           type;
    SCOREP_LocationHandle         location_handle;
    SCOREP_Allocator_PageManager* page_managers[ SCOREP_NUMBER_OF_MEMORY_TYPES ];

    SCOREP_Profile_LocationData*  profile_data;
    SCOREP_TracingData*           tracing_data;

#if !HAVE( THREAD_LOCAL_STORAGE )
    volatile sig_atomic_t scorep_in_measurement;
#endif /* !HAVE( THREAD_LOCAL_STORAGE ) */

    SCOREP_Location* next;               // store location objects in list for easy cleanup

    /** Flexible array member with length scorep_subsystems_get_number() */
    void* per_subsystem_data[];
};
static struct SCOREP_Location*  location_list_head;
static struct SCOREP_Location** location_list_tail = &location_list_head;

static SCOREP_Mutex scorep_location_list_mutex;

void
SCOREP_Location_Initialize( void )
{
    SCOREP_ErrorCode result = SCOREP_MutexCreate( &scorep_location_list_mutex );
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

    /* Locking done in SCOREP_Allocator_CreatePageManager */
    SCOREP_Memory_CreatePageManagers( new_location->page_managers );

    new_location->type = type;

    if ( SCOREP_IsProfilingEnabled() )
    {
        new_location->profile_data = SCOREP_Profile_CreateLocationData( new_location );
        assert( new_location->profile_data );
    }

    if ( SCOREP_IsTracingEnabled() )
    {
        new_location->tracing_data = SCOREP_Tracing_CreateLocationData( new_location );
        assert( new_location->tracing_data );
    }

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
    SCOREP_Location_CallSubstratesOnNewLocation( new_location, parent );

    return new_location;
}


SCOREP_Location*
SCOREP_Location_CreateCPULocation( const char* name )
{
    return scorep_location_create_location( SCOREP_LOCATION_TYPE_CPU_THREAD,
                                            name );
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
    assert( subsystem_id < scorep_subsystems_get_number() );

    return locationData->per_subsystem_data[ subsystem_id ];
}


void
SCOREP_Location_SetSubsystemData( SCOREP_Location* locationData,
                                  size_t           subsystem_id,
                                  void*            subsystem_data )
{
    assert( subsystem_id < scorep_subsystems_get_number() );

    locationData->per_subsystem_data[ subsystem_id ] = subsystem_data;
}


SCOREP_LocationHandle
SCOREP_Location_GetLocationHandle( SCOREP_Location* locationData )
{
    return locationData->location_handle;
}


void
SCOREP_Location_CallSubstratesOnNewLocation( SCOREP_Location* locationData,
                                             SCOREP_Location* parent )
{
    // Where to do the locking? Well, at the moment we do the locking
    // in SCOREP_Profile_OnLocationCreation, SCOREP_Tracing_OnLocationCreation
    // and below for the location definition. The alternative is to lock
    // this entire function.
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_OnLocationCreation( locationData, parent );
    }
    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_Tracing_OnLocationCreation( locationData, parent );
    }

    scorep_subsystems_initialize_location( locationData );
}


void
SCOREP_Location_CallSubstratesOnActivation( SCOREP_Location* current,
                                            SCOREP_Location* parent,
                                            uint32_t         forkSequenceCount )
{
    /*
     * Initialize the per-thread in-measurement counter
     *
     * If we have thread local storage support the corresponding
     * variable is initialized at its declaration (see above)
     */
#if !HAVE( THREAD_LOCAL_STORAGE )
    current->scorep_in_measurement = 0;
#endif /* !HAVE( THREAD_LOCAL_STORAGE ) */

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_OnLocationActivation( current, parent, forkSequenceCount );
    }
    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_Tracing_OnLocationActivation( current, parent );
    }
}


void
SCOREP_Location_CallSubstratesOnDeactivation( SCOREP_Location* current,
                                              SCOREP_Location* parent )
{
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_OnLocationDeactivation( current, parent );
    }
    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_Tracing_OnLocationDeactivation( current, parent );
    }
}


void
SCOREP_Location_FinalizeLocations( void )
{
    assert( !SCOREP_Thread_InParallel() );

    SCOREP_Location* location_data = location_list_head;
    while ( location_data )
    {
        SCOREP_Location* tmp = location_data->next;

        scorep_subsystems_finalize_location( location_data );
        if ( SCOREP_IsTracingEnabled() )
        {
            SCOREP_Tracing_DeleteLocationData( location_data->tracing_data );
        }
        if ( SCOREP_IsProfilingEnabled() )
        {
            SCOREP_Profile_DeleteLocationData( location_data->profile_data );
        }
        SCOREP_Memory_DeletePageManagers( location_data->page_managers );
        location_data = tmp;
    }
}


void
SCOREP_Location_Finalize( void )
{
    assert( !SCOREP_Thread_InParallel() );

    location_list_head = 0;
    location_list_tail = &location_list_head;

    SCOREP_ErrorCode result = SCOREP_MutexDestroy( &scorep_location_list_mutex );
    UTILS_ASSERT( result == SCOREP_SUCCESS );
    scorep_location_list_mutex = 0;
}


SCOREP_Allocator_PageManager*
SCOREP_Location_GetMemoryPageManager( SCOREP_Location*  locationData,
                                      SCOREP_MemoryType type )
{
    UTILS_BUG_ON( 0 > type || type >= SCOREP_NUMBER_OF_MEMORY_TYPES,
                  "Invalid memory type given." );
    return locationData->page_managers[ type ];
}


SCOREP_Profile_LocationData*
SCOREP_Location_GetProfileData( SCOREP_Location* locationData )
{
    return locationData->profile_data;
}


SCOREP_TracingData*
SCOREP_Location_GetTracingData( SCOREP_Location* locationData )
{
    return locationData->tracing_data;
}


uint64_t
SCOREP_Location_GetGlobalId( SCOREP_Location* locationData )
{
    UTILS_BUG_ON( !SCOREP_Status_IsMppInitialized(),
                  "Should only be called after the MPP was initialized." );

    uint64_t local_location_id = SCOREP_Location_GetId( locationData );
    uint64_t rank              = SCOREP_Status_GetRank();

    return ( local_location_id << 32 ) | rank;
}


void
SCOREP_Location_SetLastTimestamp( SCOREP_Location* locationData,
                                  uint64_t         timestamp )
{
    UTILS_BUG_ON( timestamp < locationData->last_timestamp,
                  "Wrong timestamp order: %" PRIu64 " (last recorded) > %" PRIu64 " (current).",
                  locationData->last_timestamp, timestamp );
    locationData->last_timestamp = timestamp;
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
        if ( SCOREP_IsTracingEnabled() )
        {
            SCOREP_Tracing_AssignLocationId( location );
        }
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
SCOREP_Location_ForAll( bool  ( * cb )( SCOREP_Location*,
                                        void* ),
                        void* data )
{
    assert( cb );

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


#if !HAVE( THREAD_LOCAL_STORAGE )

void
SCOREP_Location_InMeasurementIncrement( SCOREP_Location* locationData )
{
    locationData->scorep_in_measurement++;
}


void
SCOREP_Location_InMeasurementDecrement( SCOREP_Location* locationData )
{
    locationData->scorep_in_measurement--;
}


bool
SCOREP_Location_InMeasurement( SCOREP_Location* locationData )
{
    return locationData->scorep_in_measurement > 0;
}

#endif /* !HAVE( THREAD_LOCAL_STORAGE ) */
