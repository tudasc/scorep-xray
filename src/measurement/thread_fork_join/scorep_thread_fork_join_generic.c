/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
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
 */


/**
 * @file
 */


#include <config.h>
#include <SCOREP_ThreadForkJoin_Mgmt.h>
#include <SCOREP_ThreadForkJoin_Event.h>
#include <scorep_thread_fork_join_generic.h>
#include <scorep_thread_fork_join_model_specific.h>
#include "scorep_thread_fork_join_team.h"

#include <SCOREP_Mutex.h>
#include <SCOREP_Properties.h>
#include <tracing/SCOREP_Tracing_Events.h>
#include <SCOREP_Profile_Tasking.h>
#include <SCOREP_Metric_Management.h>
#include "scorep_events_common.h"

#include <UTILS_Error.h>

#include <string.h>


/**
 * Symbolic constant denoting that the fork sequence count
 * is not in a vaid state.
 */
#define SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT UINT32_MAX

/* Note: tpd is short for thread private data. This usually refers to
 * scorep_thread_private_data, not to thread private/local storage of a
 * particular threading model. */

typedef struct scorep_thread_private_data scorep_thread_private_data;
struct scorep_thread_private_data
{
    SCOREP_Location*            location;
    scorep_thread_private_data* parent;

    /** Used to transfer the sequence count from SCOREP_Thread_Fork()
     * to SCOREP_Thread_Begin(). */
    uint32_t tmp_fork_sequence_count;

    /** Holds the reference to the current thread team */
    SCOREP_InterimCommunicatorHandle thread_team;
};

/* Define SIZEOF_GENERIC_THREAD_PRIVATE_DATA as the smallest multiple of
 * SCOREP_ALLOCATOR_ALIGNMENT that can hold a scorep_thread_private_data object.*/
#define SIZEOF_GENERIC_THREAD_PRIVATE_DATA  \
    sizeof( scorep_thread_private_data ) +  \
    ( SCOREP_ALLOCATOR_ALIGNMENT - 1 ) -    \
    ( ( sizeof( scorep_thread_private_data ) - 1 ) % SCOREP_ALLOCATOR_ALIGNMENT )

static scorep_thread_private_data* scorep_thread_initial_tpd;

static SCOREP_Mutex scorep_thread_fork_sequence_count_lock;
static SCOREP_Mutex first_fork_locations_mutex;

static SCOREP_Location** first_fork_locations;

void
SCOREP_ThreadForkJoin_Initialize()
{
    SCOREP_ErrorCode result = SCOREP_MutexCreate( &scorep_thread_fork_sequence_count_lock );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "" );
    result = SCOREP_MutexCreate( &first_fork_locations_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "" );

    UTILS_BUG_ON( scorep_thread_initial_tpd != 0, "" );
    scorep_thread_initial_tpd           = scorep_thread_create_private_data( 0 /* parent_tpd */ );
    scorep_thread_initial_tpd->location = SCOREP_Location_CreateCPULocation(
        0 /* parent_location */,
        "Master thread",
        /* deferNewLocationNotification = */ true );

    scorep_thread_on_initialize( scorep_thread_initial_tpd );

    SCOREP_Location_CallSubstratesOnNewLocation( scorep_thread_initial_tpd->location,
                                                 "" /* name */,
                                                 0 /* parent_location */ );
    SCOREP_Location_CallSubstratesOnActivation( scorep_thread_initial_tpd->location,
                                                0 /* parent_location */,
                                                scorep_thread_get_next_sequence_count() );
}


scorep_thread_private_data*
scorep_thread_create_private_data( scorep_thread_private_data* parent )
{
    /* Create scorep_thread_private_data and the model-specific data in one
     * allocation. scorep_thread_private_data is SCOREP_ALLOCATOR_ALIGNMENT
     * aligned. The model-specific part starts at
     * ( char* )new_tpd + SIZEOF_GENERIC_THREAD_PRIVATE_DATA.
     * See scorep_thread_get_model_data().
     */
    scorep_thread_private_data* new_tpd = calloc( 1, SIZEOF_GENERIC_THREAD_PRIVATE_DATA +
                                                  scorep_thread_get_sizeof_model_data() );
    UTILS_ASSERT( new_tpd != 0 );
    new_tpd->parent                  = parent;
    new_tpd->tmp_fork_sequence_count = SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT;
    scorep_thread_on_create_private_data( new_tpd,
                                          scorep_thread_get_model_data( new_tpd ) );

    return new_tpd;
}


void
SCOREP_ThreadForkJoin_Finalize( void )
{
    UTILS_BUG_ON( scorep_thread_initial_tpd == 0, "" );
    UTILS_BUG_ON( scorep_thread_get_private_data() != scorep_thread_initial_tpd, "" );

    scorep_thread_on_finalize( scorep_thread_initial_tpd );
    scorep_thread_delete_private_data( scorep_thread_initial_tpd );

    scorep_thread_initial_tpd              = 0;
    scorep_thread_fork_sequence_count_lock = 0;

    SCOREP_ErrorCode result =
        SCOREP_MutexDestroy( &first_fork_locations_mutex );
    UTILS_ASSERT( result == SCOREP_SUCCESS );
    result =
        SCOREP_MutexDestroy( &scorep_thread_fork_sequence_count_lock );
    UTILS_ASSERT( result == SCOREP_SUCCESS );
}


static uint32_t fork_sequence_count = 0;


uint32_t
scorep_thread_get_next_sequence_count( void )
{
    SCOREP_MutexLock( scorep_thread_fork_sequence_count_lock );
    uint32_t tmp = fork_sequence_count++;
    SCOREP_MutexUnlock( scorep_thread_fork_sequence_count_lock );
    return tmp;
}


void*
scorep_thread_get_model_data( scorep_thread_private_data* tpd )
{
    /* For memory layout, see scorep_thread_create_private_data(). */
    UTILS_ASSERT( tpd );
    return ( char* )tpd + SIZEOF_GENERIC_THREAD_PRIVATE_DATA;
}


bool
scorep_thread_is_initial_thread( scorep_thread_private_data* tpd )
{
    UTILS_ASSERT( tpd );
    return tpd->parent == 0;
}


scorep_thread_private_data*
scorep_thread_get_parent( scorep_thread_private_data* tpd )
{
    UTILS_ASSERT( tpd );
    return tpd->parent;
}


SCOREP_Location*
scorep_thread_get_location( scorep_thread_private_data* tpd )
{
    UTILS_ASSERT( tpd );
    return tpd->location;
}


void
scorep_thread_set_location( scorep_thread_private_data* tpd, SCOREP_Location* location )
{
    UTILS_ASSERT( tpd->location == 0 );
    tpd->location = location;
}


void
SCOREP_ThreadForkJoin_Fork( SCOREP_ParadigmType paradigm,
                            uint32_t            nRequestedThreads )
{
    UTILS_ASSERT( paradigm & SCOREP_PARADIGM_THREAD_FORK_JOIN );
    scorep_thread_private_data* tpd       = scorep_thread_get_private_data();
    SCOREP_Location*            location  = scorep_thread_get_location( tpd );
    uint64_t                    timestamp = scorep_get_timestamp( location );

    tpd->tmp_fork_sequence_count = scorep_thread_get_next_sequence_count();

    if ( tpd->tmp_fork_sequence_count == 1 )
    {
        UTILS_ASSERT( first_fork_locations == NULL );
        size_t first_fork_locations_size = sizeof( SCOREP_Location* ) * ( nRequestedThreads - 1 );
        first_fork_locations = SCOREP_Location_AllocForMisc( location,
                                                             first_fork_locations_size );
        memset( first_fork_locations, 0, first_fork_locations_size );
    }

    scorep_thread_on_fork( nRequestedThreads,
                           paradigm,
                           scorep_thread_get_model_data( tpd ),
                           location );

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_ThreadFork( location, nRequestedThreads, tpd->tmp_fork_sequence_count );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadFork( location,
                                   timestamp,
                                   paradigm,
                                   nRequestedThreads );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }
}


void
SCOREP_ThreadForkJoin_Join( SCOREP_ParadigmType paradigm )
{
    UTILS_ASSERT( paradigm & SCOREP_PARADIGM_THREAD_FORK_JOIN );
    scorep_thread_private_data* tpd             = scorep_thread_get_private_data();
    scorep_thread_private_data* tpd_from_now_on = 0;

    scorep_thread_on_join( tpd,
                           scorep_thread_get_parent( tpd ),
                           &tpd_from_now_on,
                           paradigm );
    UTILS_BUG_ON( tpd_from_now_on == 0, "" );
    UTILS_ASSERT( tpd_from_now_on == scorep_thread_get_private_data() );

    tpd_from_now_on->thread_team = scorep_thread_get_parent_team_handle( tpd->thread_team );

    SCOREP_Location* location  = scorep_thread_get_location( tpd_from_now_on );
    uint64_t         timestamp = scorep_get_timestamp( location );

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_ThreadJoin( location );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadJoin( location,
                                   timestamp,
                                   paradigm );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }
}


void
SCOREP_ThreadForkJoin_TeamBegin( SCOREP_ParadigmType paradigm,
                                 uint32_t            threadId )
{
    scorep_thread_private_data* parent_tpd          = scorep_thread_on_team_begin_get_parent();
    uint32_t                    fork_sequence_count = parent_tpd->tmp_fork_sequence_count;
    UTILS_ASSERT( fork_sequence_count != SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT );
    uint32_t thread_team_size = scorep_thread_get_team_size();
    if ( fork_sequence_count == 1 )
    {
        SCOREP_MutexLock( first_fork_locations_mutex );
        if ( !first_fork_locations[ 0 ] )
        {
            char location_name[ 80 ];
            for ( int i = 0; i < thread_team_size - 1; ++i )
            {
                scorep_thread_create_location_name( location_name, 80, i + 1, parent_tpd );
                first_fork_locations[ i ] = SCOREP_Location_CreateCPULocation( scorep_thread_get_location( parent_tpd ),
                                                                               location_name,
                                                                               /* deferNewLocationNotification = */ true );
            }
        }
        SCOREP_MutexUnlock( first_fork_locations_mutex );
    }

    UTILS_ASSERT( paradigm & SCOREP_PARADIGM_THREAD_FORK_JOIN );
    scorep_thread_private_data* current_tpd         = 0;
    int                         thread_id           = -1;
    bool                        location_is_created = false;

    scorep_thread_on_team_begin( parent_tpd,
                                 &current_tpd,
                                 paradigm,
                                 &thread_id,
                                 ( fork_sequence_count == 1 ) ? first_fork_locations : 0,
                                 &location_is_created );

    UTILS_ASSERT( current_tpd );
    UTILS_ASSERT( thread_id >= 0 );

    SCOREP_Location* parent_location  = scorep_thread_get_location( parent_tpd );
    SCOREP_Location* current_location = scorep_thread_get_location( current_tpd );
    uint64_t         timestamp        = scorep_get_timestamp( current_location );

    if ( location_is_created )
    {
        SCOREP_Location_CallSubstratesOnNewLocation( current_location,
                                                     0,
                                                     parent_location );
    }

    /* handles recursion into the same singleton thread-team */
    current_tpd->thread_team = scorep_thread_get_team_handle(
        current_location,
        parent_tpd->thread_team,
        thread_team_size,
        thread_id );

    SCOREP_Location_CallSubstratesOnActivation( current_location,
                                                parent_location,
                                                fork_sequence_count );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTeamBegin( current_location,
                                        timestamp,
                                        paradigm,
                                        current_tpd->thread_team );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }

    if ( scorep_profiling_consume_event() )
    {
        //SCOREP_Profiling_ThreadTeamBegin( current_location, timestamp, forkSequenceCount, threadId, paradigm );
    }
}


void
SCOREP_ThreadForkJoin_TeamEnd( SCOREP_ParadigmType paradigm )
{
    UTILS_ASSERT( paradigm & SCOREP_PARADIGM_THREAD_FORK_JOIN );
    scorep_thread_private_data* tpd       = scorep_thread_get_private_data();
    scorep_thread_private_data* parent    = 0;
    SCOREP_Location*            location  = scorep_thread_get_location( tpd );
    uint64_t                    timestamp = scorep_get_timestamp( location );

    scorep_thread_on_end( tpd, &parent, paradigm );
    UTILS_ASSERT( parent );

    SCOREP_Location_CallSubstratesOnDeactivation(
        location,
        scorep_thread_get_location( parent ) );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTeamEnd( location,
                                      timestamp,
                                      paradigm,
                                      tpd->thread_team );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }
}

void
SCOREP_ThreadForkJoin_TaskCreate( SCOREP_ParadigmType paradigm,
                                  uint32_t            threadId,
                                  uint32_t            generationNumber )
{
    scorep_thread_private_data* tpd      = scorep_thread_get_private_data();
    SCOREP_Location*            location = scorep_thread_get_location( tpd );
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTaskCreate( location,
                                         timestamp,
                                         paradigm,
                                         tpd->thread_team,
                                         threadId,
                                         generationNumber );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }
}


void
SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_ParadigmType paradigm,
                                  uint32_t            threadId,
                                  uint32_t            generationNumber )
{
    scorep_thread_private_data* tpd       = scorep_thread_get_private_data();
    SCOREP_Location*            location  = scorep_thread_get_location( tpd );
    uint64_t                    timestamp = scorep_get_timestamp( location );

    if ( scorep_profiling_consume_event() )
    {
        uint64_t* metric_values = SCOREP_Metric_Read( location );
        SCOREP_Profile_TaskSwitch( location,
                                   timestamp,
                                   metric_values,
                                   threadId,
                                   generationNumber );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTaskSwitch( location,
                                         timestamp,
                                         paradigm,
                                         tpd->thread_team,
                                         threadId,
                                         generationNumber );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }
}


void
SCOREP_ThreadForkJoin_TaskBegin( SCOREP_ParadigmType paradigm,
                                 SCOREP_RegionHandle regionHandle,
                                 uint32_t            threadId,
                                 uint32_t            generationNumber )
{
    scorep_thread_private_data* tpd           = scorep_thread_get_private_data();
    SCOREP_Location*            location      = scorep_thread_get_location( tpd );
    uint64_t                    timestamp     = scorep_get_timestamp( location );
    uint64_t*                   metric_values = SCOREP_Metric_Read( location );

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_TaskBegin( location,
                                  timestamp,
                                  metric_values,
                                  regionHandle,
                                  threadId,
                                  generationNumber );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTaskSwitch( location,
                                         timestamp,
                                         paradigm,
                                         tpd->thread_team,
                                         threadId,
                                         generationNumber );

        if ( metric_values )
        {
            /* @todo: Writing metrics to trace file will be improved in the near future */

            SCOREP_Metric_WriteToTrace( location,
                                        timestamp );
        }

        SCOREP_Tracing_Enter( location, timestamp, regionHandle );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }
}


void
SCOREP_ThreadForkJoin_TaskEnd( SCOREP_ParadigmType paradigm,
                               SCOREP_RegionHandle regionHandle,
                               uint32_t            threadId,
                               uint32_t            generationNumber )
{
    scorep_thread_private_data* tpd           = scorep_thread_get_private_data();
    SCOREP_Location*            location      = scorep_thread_get_location( tpd );
    uint64_t                    timestamp     = scorep_get_timestamp( location );
    uint64_t*                   metric_values = SCOREP_Metric_Read( location );

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_TaskEnd( location,
                                timestamp,
                                metric_values,
                                regionHandle,
                                threadId,
                                generationNumber );
    }

    if ( scorep_tracing_consume_event() )
    {
        if ( metric_values )
        {
            /* @todo: Writing metrics to trace file will be improved in the near future */

            SCOREP_Metric_WriteToTrace( location,
                                        timestamp );
        }

        SCOREP_Tracing_Leave( location, timestamp, regionHandle );
        SCOREP_Tracing_ThreadTaskComplete( location,
                                           timestamp,
                                           paradigm,
                                           tpd->thread_team,
                                           threadId,
                                           generationNumber );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }
}
