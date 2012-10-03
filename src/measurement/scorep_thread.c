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
 * @file       scorep_thread.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */

#include <config.h>
#include "scorep_thread.h"
#include <scorep_definitions.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Omp.h>
#include <scorep_mpi.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "scorep_runtime_management.h"
#include "scorep_status.h"
#include "scorep_subsystem.h"
#include <tracing/SCOREP_Tracing_ThreadInteraction.h>
#include <SCOREP_Mutex.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include "scorep_environment.h"

#define POMP_TPD_MANGLED FORTRAN_MANGLED( pomp_tpd )


// The thread private variable that points to a
// SCOREP_Thread_ThreadPrivateData object. It needs to be a 64bit integer
// to smoothly work with OPARI2 instrumented Fortran code. It will be
// casted to SCOREP_Thread_ThreadPrivateData* before usage, see the macro
// TPD. Dependent on the compiler it will be pomp_tdp or pomp_tpd_, see
// config.h.
int64_t FORTRAN_ALIGNED POMP_TPD_MANGLED = 0;


// Easy access to the thread private variable. TPD stands for ThreadPrivateData
#if SIZEOF_VOID_P == 8
#define TPD ( ( SCOREP_Thread_ThreadPrivateData* )POMP_TPD_MANGLED )
#elif SIZEOF_VOID_P == 4
#define TPD ( ( SCOREP_Thread_ThreadPrivateData* )( uint32_t )POMP_TPD_MANGLED )
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif

// We want to write #pragma omp threadprivate(POMP_TPD_MANGLED) but as
// POMP_TPD_MANGLED is a macro itself, we need to do some preprocessor
// magic to be on the safe side.
SCOREP_PRAGMA_OMP( threadprivate( POMP_TPD_MANGLED ) )


typedef struct SCOREP_Thread_ThreadPrivateData SCOREP_Thread_ThreadPrivateData;


/* *INDENT-OFF* */
static SCOREP_Location* scorep_thread_create_location_data_for(SCOREP_Thread_ThreadPrivateData* tpd);
static SCOREP_Thread_ThreadPrivateData* scorep_thread_create_thread_private_data();
static void scorep_thread_call_externals_on_new_location(SCOREP_Location* locationData, SCOREP_Location* parent, bool isMainLocation );
static void scorep_thread_call_externals_on_new_thread(SCOREP_Location* locationData, SCOREP_Location* parent);
static void scorep_thread_call_externals_on_thread_activation(SCOREP_Location* locationData, SCOREP_Location* parent);
static void scorep_thread_call_externals_on_thread_deactivation(SCOREP_Location* locationData, SCOREP_Location* parent);
static void scorep_thread_delete_thread_private_data_recursively( SCOREP_Thread_ThreadPrivateData* tpd );
static void scorep_thread_init_children_to_null(SCOREP_Thread_ThreadPrivateData** children, size_t startIndex, size_t endIndex);
static void scorep_thread_update_tpd(SCOREP_Thread_ThreadPrivateData* newTPD);
static void scorep_defer_location_initialization( SCOREP_Location* locationData, SCOREP_Location* parent );
/* *INDENT-ON* */


typedef struct scorep_deferred_location scorep_deferred_location;
struct scorep_deferred_location
{
    SCOREP_Location*          location;
    SCOREP_Location*          parent;
    scorep_deferred_location* next;
};

static scorep_deferred_location*  scorep_deferred_locations_head;
static scorep_deferred_location** scorep_deferred_locations_tail = &scorep_deferred_locations_head;


struct SCOREP_Thread_ThreadPrivateData
{
    SCOREP_Thread_ThreadPrivateData*  parent;
    SCOREP_Thread_ThreadPrivateData** children;
    uint32_t                          n_children;
    bool                              is_active;
    uint32_t                          n_reusages;
    SCOREP_Location*                  location_data;
};


#define INVALID_LOCATION_DEFINITION_ID UINT64_MAX


// locations live inside SCOREP_Thread_ThreadPrivateData, may be referenced by
// multiple ones.
struct SCOREP_Location
{
    uint32_t                      local_id;    // process local id, 0, 1, ...
    uint64_t                      location_id; // global id
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
static struct SCOREP_Location*                 location_list_head;
static struct SCOREP_Location**                location_list_tail = &location_list_head;
static struct SCOREP_Thread_ThreadPrivateData* initial_thread;
static struct SCOREP_Location*                 initial_location;
static uint32_t                                location_counter;
static SCOREP_Mutex                            scorep_location_list_mutex;


void
SCOREP_Thread_Initialize()
{
    assert( !omp_in_parallel() );
    assert( initial_thread == 0 );
    assert( initial_location == 0 );
    assert( location_counter == 0 );
    assert( POMP_TPD_MANGLED == 0 );

    initial_thread         = scorep_thread_create_thread_private_data();
    initial_thread->parent = 0;

    scorep_thread_create_location_data_for( initial_thread );

    assert( POMP_TPD_MANGLED );
    assert( TPD );
    assert( TPD->is_active );
    assert( TPD->location_data );
    assert( TPD->location_data->local_id == 0 );

    initial_location = TPD->location_data;

    scorep_thread_call_externals_on_new_thread( TPD->location_data, 0 );
    scorep_thread_call_externals_on_new_location( TPD->location_data, 0, true );
    scorep_thread_call_externals_on_thread_activation( TPD->location_data, 0 );
}


void
SCOREP_Location_Initialize()
{
    SCOREP_ErrorCode result = SCOREP_MutexCreate( &scorep_location_list_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );
}

SCOREP_Location*
SCOREP_Location_CreateNonCPULocation( SCOREP_Location*    parent,
                                      SCOREP_LocationType type,
                                      const char*         name )
{
    /*
     * At the moment this function just supports creation of non-CPU
     * locations (e.g. locations for GPU threads).
     */
    UTILS_BUG_ON( type == SCOREP_LOCATION_TYPE_CPU_THREAD,
                  "At the moment SCOREP_CreateLocation() does not support creation of CPU locations." );

    if ( parent == NULL )
    {
        parent = SCOREP_Location_GetCurrentCPULocation();
    }

    /*
     * As long as this function is not used to create locations of type
     * SCOREP_LOCATION_TYPE_CPU_THREAD, no SCOREP_Thread_ThreadPrivateData
     * is needed.
     */

    SCOREP_Location* new_location = scorep_thread_create_location_data_for( NULL );

    new_location->type = type;

    scorep_thread_call_externals_on_new_location( new_location,
                                                  parent,
                                                  false );
    scorep_thread_call_externals_on_new_thread( new_location,
                                                parent );
    scorep_thread_call_externals_on_thread_activation( new_location,
                                                       parent );

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

uint64_t
SCOREP_Location_GetLastTimestamp( SCOREP_Location* locationData )
{
    return locationData->last_timestamp;
}


void
scorep_thread_update_tpd( SCOREP_Thread_ThreadPrivateData* newTPD )
{
#if SIZEOF_VOID_P == 8
    POMP_TPD_MANGLED = ( uint64_t )newTPD;
#elif SIZEOF_VOID_P == 4
    POMP_TPD_MANGLED = ( uint32_t )newTPD;
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif
}


void
scorep_thread_call_externals_on_new_thread( SCOREP_Location* locationData,
                                            SCOREP_Location* parent )
{
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_OnThreadCreation( locationData, parent );
    }
    SCOREP_Tracing_OnThreadCreation( locationData, parent );
}


void
scorep_thread_call_externals_on_new_location( SCOREP_Location* locationData,
                                              SCOREP_Location* parent,
                                              bool             isMainLocation )
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
            "" );
        scorep_defer_location_initialization( locationData, parent );
    }
    else
    {
        SCOREP_Location_GetGlobalId( locationData );
        locationData->location_handle = SCOREP_DefineLocation(
            locationData->location_id,
            locationData->type,
            parent ? parent->location_handle : SCOREP_INVALID_LOCATION,
            "" );
    }

    /* For the main location (e.g. first thread ) we will initialize subsystem later on,
     * as we need an already initialized metric subsystem at this point. */
    if ( isMainLocation == false )
    {
        scorep_subsystems_initialize_location( locationData );
    }
}

void
scorep_thread_call_externals_on_thread_activation( SCOREP_Location* locationData,
                                                   SCOREP_Location* parent )
{
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_OnThreadActivation( locationData, parent );
    }
    SCOREP_Tracing_OnThreadActivation( locationData, parent );
}


SCOREP_Location*
scorep_thread_create_location_data_for( SCOREP_Thread_ThreadPrivateData* tpd )
{
    SCOREP_Location* new_location;
    size_t           total_memory =
        sizeof( *new_location ) + ( sizeof( *new_location->per_subsystem_data )
                                    * scorep_subsystems_get_number() );
    // need synchronized malloc here
    new_location = calloc( 1, total_memory );
    assert( new_location );

    if ( tpd != NULL )
    {
        assert( tpd->location_data == 0 );
        tpd->location_data = new_location;

        // To make sure that we don't access TPD during page manager creation
        scorep_thread_update_tpd( 0 );
        // Locking here?
        SCOREP_Memory_CreatePageManagers( new_location->page_managers );

        // From here on clients can use
        // SCOREP_Location_GetCurrentCPULocation, i.e. TPD
        scorep_thread_update_tpd( tpd );
    }
    else
    {
        // Locking here?
        SCOREP_Memory_CreatePageManagers( new_location->page_managers );
    }

    new_location->type        = SCOREP_LOCATION_TYPE_CPU_THREAD;
    new_location->location_id = INVALID_LOCATION_DEFINITION_ID;

    new_location->profile_data = 0;
    if ( SCOREP_IsProfilingEnabled() )
    {
        new_location->profile_data = SCOREP_Profile_CreateLocationData( new_location );
        assert( new_location->profile_data );
    }

    new_location->tracing_data = 0;
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

    return new_location;
}


SCOREP_Thread_ThreadPrivateData*
scorep_thread_create_thread_private_data()
{
    // need synchronized malloc here
    SCOREP_Thread_ThreadPrivateData* new_tpd;
    new_tpd                = malloc( sizeof( SCOREP_Thread_ThreadPrivateData ) );
    new_tpd->parent        = TPD;
    new_tpd->children      = 0;
    new_tpd->n_children    = 0;
    new_tpd->is_active     = true;
    new_tpd->n_reusages    = 0;
    new_tpd->location_data = 0;
    return new_tpd;
}


void
SCOREP_Thread_Finalize()
{
    assert( !omp_in_parallel() );
    assert( initial_thread != 0 );
    assert( initial_location != 0 );
    assert( location_counter == 0 );
    assert( POMP_TPD_MANGLED != 0 );

    scorep_thread_delete_thread_private_data_recursively( initial_thread );

    location_list_head = 0;
    location_list_tail = &location_list_head;
    initial_thread     = 0;
    initial_location   = 0;
    location_counter   = 0;
}


void
scorep_thread_delete_thread_private_data_recursively( SCOREP_Thread_ThreadPrivateData* tpd )
{
    assert( tpd );
    bool have_unused_child = false;
    for ( uint32_t i = 0; i < tpd->n_children; ++i )
    {
        if ( tpd->children[ i ] )
        {
            assert( have_unused_child == false ); // no used child after first unused allowed
            assert( !tpd->children[ i ]->is_active );
            scorep_thread_delete_thread_private_data_recursively( tpd->children[ i ] );
        }
        else
        {
            if ( have_unused_child == false )
            {
                have_unused_child = true;
                continue;
            }
        }
    }
    free( tpd->children ); /// @todo remove if SCOREP_Memory is used for child allocation
    free( tpd );
}


void
SCOREP_Location_Finalize()
{
    assert( !omp_in_parallel() );
    assert( initial_thread != 0 );
    assert( initial_location != 0 );
    assert( location_counter > 0 );
    assert( POMP_TPD_MANGLED != 0 );

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
}


void
SCOREP_Thread_OnThreadFork( size_t nRequestedThreads )
{
    assert( TPD->is_active );
    TPD->is_active = false;

    /// @todo replace malloc/realloc with SCOREPs memory management to get rid of
    ///  locking. we have access to TPD->location_data->page_managers.
    if ( !TPD->children || TPD->n_children < nRequestedThreads )
    {
        TPD->children = realloc( TPD->children, nRequestedThreads *
                                 sizeof( SCOREP_Thread_ThreadPrivateData* ) );
        scorep_thread_init_children_to_null( TPD->children, TPD->n_children, nRequestedThreads - 1 );
        TPD->n_children = nRequestedThreads;
    }
}


void
scorep_thread_init_children_to_null( SCOREP_Thread_ThreadPrivateData** children,
                                     size_t                            firstIndex,
                                     size_t                            lastIndex )
{
    assert( firstIndex <= lastIndex );
    for ( size_t i = firstIndex; i <= lastIndex; ++i )
    {
        children[ i ] = 0;
    }
}


void
SCOREP_Thread_OnThreadJoin()
{
    if ( TPD->n_reusages == 0 )
    {
        assert( TPD->parent );
        scorep_thread_update_tpd( TPD->parent );
    }
    else
    {
        assert( TPD->is_active );
        // There was no parallelism in the previous parallel region
        // so we must not update TPD.
    }

    if ( !TPD->is_active )
    {
        // last parallel region used by more than one thread
        for ( size_t i = 0; i < TPD->n_children; ++i )
        {
            if ( TPD->children[ i ]->is_active )
            {
                scorep_thread_call_externals_on_thread_deactivation(
                    TPD->children[ i ]->location_data, TPD->location_data );
                TPD->children[ i ]->is_active = false;
            }
        }
        TPD->is_active = true;
        // need activation here?
    }
    else
    {
        assert( TPD->n_reusages > 0 );
        TPD->n_reusages--;
        // no parallelism in last parallel region, parent == child
        scorep_thread_call_externals_on_thread_deactivation( TPD->location_data,
                                                             TPD->location_data );
    }
}


void
scorep_thread_call_externals_on_thread_deactivation( SCOREP_Location* locationData,
                                                     SCOREP_Location* parent )
{
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_OnThreadDeactivation( locationData, parent );
    }
    SCOREP_Tracing_OnThreadDeactivation( locationData, parent );
}


SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation()
{
    if ( TPD->is_active )
    {
        return TPD->location_data;
    }

    // entered parallel region. thread will be activated, maybe created. TPD
    // is the parent
    if ( omp_get_num_threads() == 1 )
    {
        // there is no additional parallelism in this parallel region. don't
        // update TPD with a child but reuse the parent.
        TPD->is_active = true;
        TPD->n_reusages++;
        if ( !TPD->children[ 0 ] )
        {
            /// @todo do we see this as a new thread?
            scorep_thread_call_externals_on_new_thread( TPD->location_data,
                                                        TPD->location_data );
        }
        scorep_thread_call_externals_on_thread_activation( TPD->location_data,
                                                           TPD->location_data );
    }
    else
    {
        // set TPD to a child of itself, create new one if neccessary
        size_t my_thread_id = omp_get_thread_num();
        assert( my_thread_id < TPD->n_children );
        SCOREP_Thread_ThreadPrivateData** my_tpd = &( TPD->children[ my_thread_id ] );
        if ( *my_tpd )
        {
            // already been in this thread
            assert( !( *my_tpd )->is_active );
            ( *my_tpd )->is_active = true;
            scorep_thread_update_tpd( *my_tpd );
        }
        else
        {
            // never been here before
            *my_tpd = scorep_thread_create_thread_private_data();
            if ( my_thread_id == 0 )
            {
                // reuse parents location data
                ( *my_tpd )->location_data = TPD->location_data;
                scorep_thread_update_tpd( *my_tpd );
            }
            else
            {
                scorep_thread_create_location_data_for( *my_tpd );
                scorep_thread_call_externals_on_new_location( ( *my_tpd )->location_data,
                                                              TPD->parent->location_data,
                                                              false );
            }
            scorep_thread_call_externals_on_new_thread( ( *my_tpd )->location_data,
                                                        TPD->parent->location_data );
        }
        scorep_thread_call_externals_on_thread_activation( TPD->location_data,
                                                           TPD->parent->location_data );
    }

    return TPD->location_data;
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

    if ( locationData->location_id == INVALID_LOCATION_DEFINITION_ID )
    {
        uint64_t local_location_id = SCOREP_Location_GetId( locationData );
        uint64_t rank              = SCOREP_Mpi_GetRank();

        assert( rank >> 32 == 0 );
        assert( local_location_id >> 32 == 0 );

        locationData->location_id = ( local_location_id << 32 ) | rank;
    }

    return locationData->location_id;
}


void
SCOREP_Location_SetLastTimestamp( SCOREP_Location* locationData,
                                  int64_t          timestamp )
{
    locationData->last_timestamp = timestamp;
}


void
scorep_defer_location_initialization( SCOREP_Location* locationData,
                                      SCOREP_Location* parent )
{
    scorep_deferred_location* deferred_location = SCOREP_Memory_AllocForMisc( sizeof( scorep_deferred_location ) );
    assert( deferred_location );

    deferred_location->location = locationData;
    deferred_location->parent   = parent;
    deferred_location->next     = 0;
    SCOREP_PRAGMA_OMP( critical( deferred_locations ) )
    {
        *scorep_deferred_locations_tail = deferred_location;
        scorep_deferred_locations_tail  = &deferred_location->next;
    }
}


void
SCOREP_Location_ProcessDeferredOnes()
{
    SCOREP_Location* current_location = SCOREP_Location_GetCurrentCPULocation();

    SCOREP_PRAGMA_OMP( critical( deferred_locations ) )
    {
        scorep_deferred_location* deferred_location                      = scorep_deferred_locations_head;
        bool                      current_location_in_deferred_locations = false;

        while ( deferred_location )
        {
            SCOREP_Location* location = deferred_location->location;
            if ( location == current_location )
            {
                current_location_in_deferred_locations = true;
            }

            /* Ensure that the location id is available */
            SCOREP_Location_GetGlobalId( location );

            SCOREP_LOCAL_HANDLE_DEREF( location->location_handle, Location )->global_location_id =
                location->location_id;

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
    }
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
