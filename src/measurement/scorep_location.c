/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen, Germany
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
 * @file       src/measurement/scorep_location.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include "scorep_location.h"

#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>

#include <SCOREP_Mutex.h>
#include "scorep_subsystem.h"
#include "scorep_status.h"
#include <scorep_mpi.h>
#include <SCOREP_Omp.h>
#include <scorep_definitions.h>

#include <UTILS_Error.h>


/* *INDENT-OFF* */
static void scorep_location_defer_definition_closure( SCOREP_Location* locationData, SCOREP_Location* parent );
/* *INDENT-ON*  */


typedef struct scorep_deferred_location scorep_deferred_location;
struct scorep_deferred_location
{
    SCOREP_Location*          location;
    SCOREP_Location*          parent;
    scorep_deferred_location* next;
};

static scorep_deferred_location*  scorep_deferred_locations_head;
static scorep_deferred_location** scorep_deferred_locations_tail = &scorep_deferred_locations_head;

#define INVALID_LOCATION_DEFINITION_ID UINT64_MAX


// locations live inside SCOREP_Thread_ThreadPrivateData, may be referenced by
// multiple ones.
struct SCOREP_Location
{
    uint32_t                      local_id;    // process local id, 0, 1, ...
    uint64_t                      last_timestamp;
    SCOREP_LocationType           type;
    SCOREP_Allocator_PageManager* page_managers[ SCOREP_NUMBER_OF_MEMORY_TYPES ];
    SCOREP_LocationHandle         location_handle;

    SCOREP_Profile_LocationData*  profile_data;
    SCOREP_TracingData*           tracing_data;

    SCOREP_Location*              next;  // store location objects in list for easy cleanup

    /** Flexible array member with length scorep_subsystems_get_number() */
    void* per_subsystem_data[];
};
static struct SCOREP_Location*  location_list_head;
static struct SCOREP_Location** location_list_tail = &location_list_head;

//static struct SCOREP_Location*                 initial_location;
static uint32_t     location_counter;
static SCOREP_Mutex scorep_location_list_mutex;
static SCOREP_Mutex scorep_location_deferred_list_mutex;

void
SCOREP_Location_Initialize()
{
    //assert( initial_location == 0 );
    assert( location_counter == 0 );
    SCOREP_ErrorCode result = SCOREP_MutexCreate( &scorep_location_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );

    result = SCOREP_MutexCreate( &scorep_location_deferred_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );
}


SCOREP_Location*
scorep_location_create_location( SCOREP_Location*    parent,
                                 SCOREP_LocationType type,
                                 const char*         name )
{
    SCOREP_Location* new_location;
    size_t           total_memory = sizeof( *new_location )
                                    + ( sizeof( *new_location->per_subsystem_data )
                                        * scorep_subsystems_get_number() );
    // Need synchronized malloc here
    new_location = calloc( 1, total_memory );
    assert( new_location );

    // Locking done in SCOREP_Allocator_CreatePageManager
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

    SCOREP_ErrorCode result = SCOREP_MutexLock( scorep_location_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );

    new_location->local_id = location_counter++;
    new_location->next     = NULL;
    *location_list_tail    = new_location;
    location_list_tail     = &new_location->next;

    result = SCOREP_MutexUnlock( scorep_location_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );

    scorep_location_call_externals_on_new_location( new_location, name, parent );

    return new_location;
}

SCOREP_Location*
SCOREP_Location_CreateNonCPULocation( SCOREP_Location*    parent,
                                      SCOREP_LocationType type,
                                      const char*         name )
{
    UTILS_BUG_ON( type == SCOREP_LOCATION_TYPE_CPU_THREAD,
                  "SCOREP_CreateNonCPULocation() does not support creation of CPU locations." );

    SCOREP_Location* new_location = scorep_location_create_location( parent, type, name );

    return new_location;
}


SCOREP_Location*
SCOREP_Location_CreateCPULocation( SCOREP_Location* parent,
                                   const char*      name )
{
    SCOREP_Location* new_location = scorep_location_create_location( parent,
                                                                     SCOREP_LOCATION_TYPE_CPU_THREAD,
                                                                     name );
    return new_location;
}


uint32_t
SCOREP_Location_GetId( SCOREP_Location* locationData )
{
    return locationData->local_id;
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
scorep_location_call_externals_on_new_location( SCOREP_Location* locationData,
                                                const char*      name,
                                                SCOREP_Location* parent )
{
    // Where to do the locking? Well, at the moment we do the locking
    // in SCOREP_Profile_OnLocationCreation, SCOREP_Tracing_OnLocationCreation
    // and below for the location definition. The alternative is to lock
    // this entire function.
    SCOREP_Profile_OnLocationCreation( locationData, parent );
    SCOREP_Tracing_OnLocationCreation( locationData, parent );

    if ( !SCOREP_Mpi_IsInitialized() )
    {
        locationData->location_handle = SCOREP_DefineLocation(
            INVALID_LOCATION_DEFINITION_ID,
            locationData->type,
            SCOREP_INVALID_LOCATION,
            name );
        scorep_location_defer_definition_closure( locationData, parent );
    }
    else
    {
        SCOREP_Location_GetGlobalId( locationData );
        locationData->location_handle = SCOREP_DefineLocation(
            SCOREP_Location_GetGlobalId( locationData ),
            locationData->type,
            parent ? parent->location_handle : SCOREP_INVALID_LOCATION,
            name );
    }

    /* For the main location (e.g. first thread ) we will initialize subsystem later on,
     * as we need an already initialized metric subsystem at this point. */
    if ( locationData->local_id != 0 )
    {
        scorep_subsystems_initialize_location( locationData );
    }
}


void
SCOREP_Location_Finalize()
{
    assert( !omp_in_parallel() );
    assert( location_counter > 0 );

    SCOREP_Location* location_data = location_list_head;
    while ( location_data )
    {
        SCOREP_Location* tmp = location_data->next;

        scorep_subsystems_finalize_location( location_data );
        SCOREP_Tracing_DeleteLocationData( location_data->tracing_data );
        SCOREP_Profile_DeleteLocationData( location_data->profile_data );
        SCOREP_Memory_DeletePageManagers( location_data->page_managers );
        free( location_data );

        location_data = tmp;
        location_counter--;
    }
    assert( location_counter == 0 );
    location_list_head = 0;
    location_list_tail = &location_list_head;

    SCOREP_ErrorCode result = SCOREP_MutexDestroy( &scorep_location_list_mutex );
    UTILS_ASSERT( result == SCOREP_SUCCESS );
    scorep_location_list_mutex = 0;

    result = SCOREP_MutexDestroy( &scorep_location_deferred_list_mutex );
    UTILS_ASSERT( result == SCOREP_SUCCESS );
    scorep_location_deferred_list_mutex = 0;
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
    assert( SCOREP_Mpi_IsInitialized() );

    uint64_t local_location_id = locationData->local_id;
    uint64_t rank              = SCOREP_Mpi_GetRank();

    assert( rank >> 32 == 0 );
    assert( local_location_id >> 32 == 0 );

    return ( local_location_id << 32 ) | rank;
}


void
SCOREP_Location_SetLastTimestamp( SCOREP_Location* locationData,
                                  int64_t          timestamp )
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
scorep_location_defer_definition_closure( SCOREP_Location* locationData,
                                          SCOREP_Location* parent )
{
    scorep_deferred_location* deferred_location = SCOREP_Location_AllocForMisc( locationData, sizeof( scorep_deferred_location ) );
    assert( deferred_location );

    deferred_location->location = locationData;
    deferred_location->parent   = parent;
    deferred_location->next     = 0;

    SCOREP_ErrorCode result = SCOREP_MutexLock( scorep_location_deferred_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );

    *scorep_deferred_locations_tail = deferred_location;
    scorep_deferred_locations_tail  = &deferred_location->next;

    result = SCOREP_MutexUnlock( scorep_location_deferred_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );
}


void
SCOREP_Location_CloseDeferredDefinitions()
{
    SCOREP_Location* current_location = SCOREP_Location_GetCurrentCPULocation();

    SCOREP_ErrorCode result = SCOREP_MutexLock( scorep_location_deferred_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );

    scorep_deferred_location* deferred_location                      = scorep_deferred_locations_head;
    bool                      current_location_in_deferred_locations = false;

    while ( deferred_location )
    {
        SCOREP_Location* location = deferred_location->location;
        if ( location == current_location )
        {
            current_location_in_deferred_locations = true;
        }

        SCOREP_LOCAL_HANDLE_DEREF( location->location_handle, Location )->global_location_id =
            SCOREP_Location_GetGlobalId( location );

        SCOREP_Tracing_AssignLocationId( location );

        deferred_location = deferred_location->next;
    }

    assert( current_location_in_deferred_locations );

    // update parents
    deferred_location = scorep_deferred_locations_head;
    while ( deferred_location )
    {
        SCOREP_Location* location = deferred_location->location;
        SCOREP_Location* parent   = deferred_location->parent;
        if ( parent )
        {
            SCOREP_LOCAL_HANDLE_DEREF( location->location_handle, Location )->parent =
                parent->location_handle;
        }
        else
        {
            assert( SCOREP_LOCAL_HANDLE_DEREF( location->location_handle, Location )->parent
                    == SCOREP_INVALID_LOCATION );
        }
        deferred_location = deferred_location->next;
    }

    result = SCOREP_MutexUnlock( scorep_location_deferred_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );
}

void
SCOREP_Location_ForAll( void  ( * cb )( SCOREP_Location*,
                                        void* ),
                        void* data )
{
    assert( cb );

    for ( SCOREP_Location* location_data = location_list_head;
          location_data;
          location_data = location_data->next )
    {
        cb( location_data, data );
    }
}
