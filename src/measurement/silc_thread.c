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
 * @file       silc_thread.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 *
 */


#include "silc_thread.h"
//#include <pomp_lib.h>
#include <SILC_Memory.h>
#include <SILC_Omp.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>


/// @todo move this definition to the pomp adapter, in pomp_base.c e.g., like
/// in Scalasca. Will there be two pomp adapters, one base and one for openmp?
void* pomp_tpd = 0;
#ifdef _OPENMP
#pragma omp threadprivate(pomp_tpd)
#endif


typedef struct SILC_Thread_ThreadPrivateData SILC_Thread_ThreadPrivateData;


/* *INDENT-OFF* */
static void silc_thread_create_location_data_for(SILC_Thread_ThreadPrivateData* tpd);
static SILC_Thread_ThreadPrivateData* silc_thread_create_thread_private_data();
static void silc_thread_call_externals_on_new_location(SILC_Thread_LocationData* locationData, SILC_Thread_LocationData* parent);
static void silc_thread_call_externals_on_new_thread(SILC_Thread_LocationData* locationData, SILC_Thread_LocationData* parent);
static void silc_thread_call_externals_on_thread_activation(SILC_Thread_LocationData* locationData, SILC_Thread_LocationData* parent);
static void silc_thread_call_externals_on_thread_deactivation(SILC_Thread_LocationData* locationData, SILC_Thread_LocationData* parent);
static void silc_thread_delete_location_data();
static void silc_thread_delete_thread_private_data_recursively( SILC_Thread_ThreadPrivateData* tpd );
static void silc_thread_init_childs_to_null(SILC_Thread_ThreadPrivateData** childs, size_t startIndex, size_t endIndex);
static void silc_thread_update_tpd(SILC_Thread_ThreadPrivateData* newTPD);
/* *INDENT-ON* */


// easy access to thread private variable. TPD == ThreadPrivateData
#define TPD ( ( SILC_Thread_ThreadPrivateData* )pomp_tpd )


struct SILC_Thread_ThreadPrivateData
{
    SILC_Thread_ThreadPrivateData*  parent;
    SILC_Thread_ThreadPrivateData** childs;
    size_t                          n_childs;
    bool                            is_active;
    SILC_Thread_LocationData*       location_data;
};


// locations live inside SILC_Thread_ThreadPrivateData, may be referenced by
// multiple ones.
struct SILC_Thread_LocationData
{
    size_t                     location_id;
    SILC_Memory_PageManager**  page_managers;
    SILC_Profile_LocationData* profile_data;
    SILC_Trace_LocationData*   trace_data;
    SILC_Thread_LocationData*  next; // store location objects in list for easy cleanup
};


struct SILC_Thread_LocationData       location_list_head_dummy = { 0, 0, 0, 0, 0 };
struct SILC_Thread_ThreadPrivateData* initial_thread   = 0;
struct SILC_Thread_LocationData*      initial_location = 0;
size_t                                location_counter = 0;


void
SILC_Thread_Initialize()
{
    assert( !omp_in_parallel() );
    assert( initial_thread == 0 );
    assert( initial_location == 0 );
    assert( location_counter == 0 );
    assert( pomp_tpd == 0 );

    initial_thread         = silc_thread_create_thread_private_data();
    initial_thread->parent = 0;

    silc_thread_create_location_data_for( initial_thread );

    assert( pomp_tpd );
    assert( TPD );
    assert( TPD->is_active );
    assert( TPD->location_data->location_id == 0 );

    silc_thread_call_externals_on_new_thread( TPD->location_data, 0 );
    silc_thread_call_externals_on_new_location( TPD->location_data, 0 );
    silc_thread_call_externals_on_thread_activation( TPD->location_data, 0 );
}


void
silc_thread_update_tpd( SILC_Thread_ThreadPrivateData* newTPD )
{
    pomp_tpd = ( void* )newTPD;
}


void
silc_thread_call_externals_on_new_thread( SILC_Thread_LocationData* locationData,
                                          SILC_Thread_LocationData* parent )
{
    SILC_Profile_OnThreadCreation( locationData, parent );
    SILC_Trace_OnThreadCreation( locationData, parent );
}


void
silc_thread_call_externals_on_new_location( SILC_Thread_LocationData* locationData,
                                            SILC_Thread_LocationData* parent )
{
    SILC_Profile_OnLocationCreation( locationData, parent );
    SILC_Trace_OnLocationCreation( locationData, parent );
}


void
silc_thread_call_externals_on_thread_activation( SILC_Thread_LocationData* locationData,
                                                 SILC_Thread_LocationData* parent )
{
    SILC_Profile_OnThreadActivation( locationData, parent );
    SILC_Trace_OnThreadActivation( locationData, parent );
}


void
silc_thread_create_location_data_for( SILC_Thread_ThreadPrivateData* tpd )
{
    // need synchronized malloc here
    SILC_Thread_LocationData* new_location;
    new_location = malloc( sizeof( SILC_Thread_LocationData ) );
    assert( new_location );
    assert( tpd->location_data == 0 );
    tpd->location_data = new_location;

    silc_thread_update_tpd( 0 );   // don't access TPD during page
                                   // manager creation
    new_location->page_managers = SILC_Memory_CreatePageManagers();
    silc_thread_update_tpd( tpd ); // from here on clients can use
                                   // SILC_Thread_GetLocationData, i.e. TPD

    new_location->profile_data = SILC_Profile_CreateLocationData();
    new_location->trace_data   = SILC_Trace_CreateLocationData();

    #pragma omp critical (new_location)
    {
        new_location->location_id     = location_counter++;
        new_location->next            = location_list_head_dummy.next;
        location_list_head_dummy.next = new_location;
    }
}


SILC_Thread_ThreadPrivateData*
silc_thread_create_thread_private_data()
{
    // need synchronized malloc here
    SILC_Thread_ThreadPrivateData* new_tpd;
    new_tpd                = malloc( sizeof( SILC_Thread_ThreadPrivateData ) );
    new_tpd->parent        = TPD;
    new_tpd->childs        = 0;
    new_tpd->n_childs      = 0;
    new_tpd->is_active     = true;
    new_tpd->location_data = 0;
    return new_tpd;
}


void
SILC_Thread_Finalize()
{
    assert( !omp_in_parallel() );
    assert( initial_thread != 0 );
    assert( initial_location != 0 );
    assert( location_counter > 0 );
    assert( pomp_tpd != 0 );

    silc_thread_delete_thread_private_data_recursively( initial_thread );
    silc_thread_delete_location_data();

    location_list_head_dummy.next = 0;
    initial_thread                = 0;
    initial_location              = 0;
    location_counter              = 0;
}


void
silc_thread_delete_thread_private_data_recursively( SILC_Thread_ThreadPrivateData* tpd )
{
    assert( tpd );
    bool have_unused_child = false;
    for ( int i = 0; i < tpd->n_childs; ++i )
    {
        if ( tpd->childs[ i ] )
        {
            assert( have_unused_child == false ); // no used child after first unused allowed
            assert( !tpd->childs[ i ]->is_active );
            silc_thread_delete_thread_private_data_recursively( tpd->childs[ i ] );
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
    free( tpd->childs ); /// @todo remove if SILC_Memory is used for child allocation
    free( tpd );
}


void
silc_thread_delete_location_data( SILC_Thread_LocationData* locationData )
{
    size_t                    count         = 0;
    SILC_Thread_LocationData* location_data = location_list_head_dummy.next;
    while ( location_data )
    {
        SILC_Thread_LocationData* tmp = location_data->next;

        SILC_Trace_DeleteLocationData( location_data->trace_data );
        SILC_Profile_DeleteLocationData( location_data->profile_data );
        SILC_Memory_DeletePageManagers( location_data->page_managers ); // too early?
        free( locationData );

        location_data = tmp;
        count++;
    }
    assert( count == location_counter );
}


void
SILC_Thread_OnThreadFork( size_t nRequestedThreads )
{
    assert( TPD->is_active );
    TPD->is_active = false;

    /// @todo replace malloc/realloc with SILCs memory management to get rid of
    ///  locking. we have access to TPD->location_data->page_managers.
    if ( !TPD->childs || TPD->n_childs < nRequestedThreads )
    {
        TPD->childs = realloc( TPD->childs, nRequestedThreads *
                               sizeof( SILC_Thread_ThreadPrivateData* ) );
        silc_thread_init_childs_to_null( TPD->childs, TPD->n_childs, nRequestedThreads - 1 );
        TPD->n_childs = nRequestedThreads;
    }
}


void
silc_thread_init_childs_to_null( SILC_Thread_ThreadPrivateData** childs,
                                 size_t                          firstIndex,
                                 size_t                          lastIndex )
{
    assert( firstIndex <= lastIndex );
    for ( size_t i = firstIndex; i <= lastIndex; ++i )
    {
        childs[ i ] = 0;
    }
}


void
SILC_Thread_OnThreadJoin()
{
    if ( !TPD->is_active )
    {
        // last parallel region used more than one thread
        for ( size_t i = 0; i < TPD->n_childs; ++i )
        {
            if ( TPD->childs[ i ]->is_active )
            {
                silc_thread_call_externals_on_thread_deactivation(
                    TPD->childs[ i ]->location_data, TPD->location_data );
                TPD->childs[ i ]->is_active = false;
            }
        }
        TPD->is_active = true;
    }
    else
    {
        // no parallelism in last parallel region, parent == child
        silc_thread_call_externals_on_thread_deactivation( TPD->location_data,
                                                           TPD->location_data );
    }
}


void
silc_thread_call_externals_on_thread_deactivation( SILC_Thread_LocationData* locationData,
                                                   SILC_Thread_LocationData* parent )
{
    SILC_Profile_OnThreadDectivation( locationData, parent );
    SILC_Trace_OnThreadDectivation( locationData, parent );
}


SILC_Thread_LocationData*
SILC_Thread_GetLocationData()
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
        if ( !TPD->childs[ 0 ] )
        {
            /// @todo do we see this as a new thread?
            silc_thread_call_externals_on_new_thread( TPD->location_data,
                                                      TPD->location_data );
        }
    }
    else
    {
        // set TPD to a child of itself, create new one if neccessary
        size_t                          my_thread_id = omp_get_thread_num();
        SILC_Thread_ThreadPrivateData** my_tpd       = &( TPD->childs[ my_thread_id ] );
        if ( *my_tpd )
        {
            // already been in this thread
            assert( !( *my_tpd )->is_active );
            ( *my_tpd )->is_active = true;
            silc_thread_update_tpd( *my_tpd );
        }
        else
        {
            // never been here before
            *my_tpd = silc_thread_create_thread_private_data();
            if ( my_thread_id == 0 )
            {
                // reuse parents location data
                ( *my_tpd )->location_data = TPD->location_data;
                silc_thread_update_tpd( *my_tpd );
            }
            else
            {
                silc_thread_create_location_data_for( *my_tpd );
                silc_thread_call_externals_on_new_location( ( *my_tpd )->location_data,
                                                            TPD->location_data );
            }
            silc_thread_call_externals_on_new_thread( ( *my_tpd )->location_data,
                                                      TPD->location_data );
        }
    }
    silc_thread_call_externals_on_thread_activation( TPD->location_data,
                                                     TPD->parent->location_data );
    return TPD->location_data;
}


SILC_Memory_PageManager**
SILC_Thread_GetLocationLocalMemoryPageManagers()
{
    return SILC_Thread_GetLocationData()->page_managers;
}


SILC_Memory_PageManager**
SILC_Thread_GetGlobalMemoryPageManagers()
{
    return initial_location->page_managers;
}


SILC_Profile_LocationData*
SILC_Thread_GetProfileLocationData( SILC_Thread_LocationData* locationData )
{
    return locationData->profile_data;
}


SILC_Trace_LocationData*
SILC_Thread_GetTraceLocationData( SILC_Thread_LocationData* locationData )
{
    return locationData->trace_data;
}


uint64_t
SILC_Thread_GetLocationId( SILC_Thread_LocationData* locationData )
{
    return locationData->location_id;
}
