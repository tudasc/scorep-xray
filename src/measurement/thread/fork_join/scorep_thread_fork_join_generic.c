/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
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
#include <SCOREP_Thread_Mgmt.h>
#include <SCOREP_ThreadForkJoin_Event.h>
#include <scorep_thread_generic.h>
#include <scorep_thread_model_specific.h>
#include <scorep_thread_fork_join_model_specific.h>
#include "scorep_thread_fork_join_team.h"

#include <SCOREP_Mutex.h>
#include <SCOREP_Paradigms.h>
#include <SCOREP_Properties.h>
#include <tracing/SCOREP_Tracing_Events.h>
#include <SCOREP_Profile_Tasking.h>
#include <SCOREP_Metric_Management.h>
#include "scorep_events_common.h"
#include "scorep_task_internal.h"

#include <UTILS_Error.h>

#include <string.h>


/* Note: tpd is short for thread private data. This usually refers to
 * scorep_thread_private_data, not to thread private/local storage of a
 * particular threading model. */

static SCOREP_Location** first_fork_locations;
static SCOREP_Mutex      first_fork_locations_mutex;


void
scorep_thread_create_first_fork_locations_mutex( void )
{
    SCOREP_ErrorCode result = SCOREP_MutexCreate( &first_fork_locations_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "" );
}


void
scorep_thread_destroy_first_fork_locations_mutex( void )
{
    SCOREP_ErrorCode result = SCOREP_MutexDestroy( &first_fork_locations_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );
}


void
SCOREP_ThreadForkJoin_Fork( SCOREP_ParadigmType paradigm,
                            uint32_t            nRequestedThreads )
{
    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_FORK_JOIN ),
                  "Provided paradigm not of class fork/join" );
    struct scorep_thread_private_data* tpd       = scorep_thread_get_private_data();
    SCOREP_Location*                   location  = scorep_thread_get_location( tpd );
    uint64_t                           timestamp = scorep_get_timestamp( location );

    uint32_t sequence_count = scorep_thread_get_next_sequence_count();
    scorep_thread_set_tmp_sequence_count( tpd, sequence_count );

    if ( sequence_count == 1 )
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
        SCOREP_Profile_ThreadFork( location, nRequestedThreads, sequence_count );
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
    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_FORK_JOIN ),
                  "Provided paradigm not of fork/join class" );
    struct scorep_thread_private_data* tpd             = scorep_thread_get_private_data();
    struct scorep_thread_private_data* tpd_from_now_on = 0;

    scorep_thread_on_join( tpd,
                           scorep_thread_get_parent( tpd ),
                           &tpd_from_now_on,
                           paradigm );
    UTILS_BUG_ON( tpd_from_now_on == 0, "" );
    UTILS_ASSERT( tpd_from_now_on == scorep_thread_get_private_data() );

    SCOREP_InterimCommunicatorHandle team =
        scorep_thread_get_parent_team_handle( scorep_thread_get_team( tpd ) );
    scorep_thread_set_team( tpd_from_now_on, team );

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

SCOREP_TaskHandle
SCOREP_ThreadForkJoin_TeamBegin( SCOREP_ParadigmType paradigm,
                                 uint32_t            threadId )
{
    struct scorep_thread_private_data* parent_tpd     = scorep_thread_on_team_begin_get_parent();
    uint32_t                           sequence_count = scorep_thread_get_tmp_sequence_count( parent_tpd );
    UTILS_ASSERT( sequence_count != SCOREP_THREAD_INVALID_SEQUENCE_COUNT );
    uint32_t thread_team_size = scorep_thread_get_team_size();
    if ( sequence_count == 1 && thread_team_size > 1 )
    {
        SCOREP_MutexLock( first_fork_locations_mutex );
        if ( !first_fork_locations[ 0 ] )
        {
            char location_name[ 80 ];
            for ( int i = 0; i < thread_team_size - 1; ++i )
            {
                scorep_thread_create_location_name( location_name, 80, i + 1, parent_tpd );
                first_fork_locations[ i ] = SCOREP_Location_CreateCPULocation( location_name );
            }
        }
        SCOREP_MutexUnlock( first_fork_locations_mutex );
    }

    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_FORK_JOIN ),
                  "Provided paradigm not of fork/join class" );
    struct scorep_thread_private_data* current_tpd         = 0;
    int                                thread_id           = -1;
    bool                               location_is_created = false;

    scorep_thread_on_team_begin( parent_tpd,
                                 &current_tpd,
                                 paradigm,
                                 &thread_id,
                                 ( sequence_count == 1 ) ? first_fork_locations : 0,
                                 &location_is_created );

    UTILS_ASSERT( current_tpd );
    UTILS_ASSERT( thread_id >= 0 );

    SCOREP_Location* parent_location  = scorep_thread_get_location( parent_tpd );
    SCOREP_Location* current_location = scorep_thread_get_location( current_tpd );
    uint64_t         timestamp        = scorep_get_timestamp( current_location );

    if ( location_is_created )
    {
        SCOREP_Location_CallSubstratesOnNewLocation( current_location,
                                                     parent_location );
    }

    /* handles recursion into the same singleton thread-team */
    SCOREP_InterimCommunicatorHandle team = scorep_thread_get_team_handle(
        current_location,
        scorep_thread_get_team( parent_tpd ),
        thread_team_size,
        thread_id );
    scorep_thread_set_team( current_tpd, team );

    SCOREP_Location_CallSubstratesOnActivation( current_location,
                                                parent_location,
                                                sequence_count );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTeamBegin( current_location,
                                        timestamp,
                                        paradigm,
                                        scorep_thread_get_team( current_tpd ) );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }

    if ( scorep_profiling_consume_event() )
    {
        //SCOREP_Profiling_ThreadTeamBegin( current_location, timestamp, forkSequenceCount, threadId, paradigm );
    }

    return SCOREP_Task_GetCurrentTask( current_location );
}


void
SCOREP_ThreadForkJoin_TeamEnd( SCOREP_ParadigmType paradigm )
{
    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_FORK_JOIN ),
                  "Provided paradigm not of fork/join class" );
    struct scorep_thread_private_data* tpd       = scorep_thread_get_private_data();
    struct scorep_thread_private_data* parent    = 0;
    SCOREP_Location*                   location  = scorep_thread_get_location( tpd );
    uint64_t                           timestamp = scorep_get_timestamp( location );

    scorep_thread_on_team_end( tpd, &parent, paradigm );
    UTILS_ASSERT( parent );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTeamEnd( location,
                                      timestamp,
                                      paradigm,
                                      scorep_thread_get_team( tpd ) );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }

    SCOREP_Location_CallSubstratesOnDeactivation(
        location,
        scorep_thread_get_location( parent ) );
}

void
SCOREP_ThreadForkJoin_TaskCreate( SCOREP_ParadigmType paradigm,
                                  uint32_t            threadId,
                                  uint32_t            generationNumber )
{
    struct scorep_thread_private_data* tpd      = scorep_thread_get_private_data();
    SCOREP_Location*                   location = scorep_thread_get_location( tpd );
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTaskCreate( location,
                                         timestamp,
                                         paradigm,
                                         scorep_thread_get_team( tpd ),
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
                                  SCOREP_TaskHandle   task )
{
    struct scorep_thread_private_data* tpd       = scorep_thread_get_private_data();
    SCOREP_Location*                   location  = scorep_thread_get_location( tpd );
    uint64_t                           timestamp = scorep_get_timestamp( location );

    scorep_task_switch( location, task );

    uint32_t thread_id     = SCOREP_Task_GetThreadId( task );
    uint32_t generation_no = SCOREP_Task_GetGenerationNumber( task );

    if ( scorep_profiling_consume_event() )
    {
        uint64_t* metric_values = SCOREP_Metric_Read( location );
        SCOREP_Profile_TaskSwitch( location,
                                   timestamp,
                                   metric_values,
                                   task );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTaskSwitch( location,
                                         timestamp,
                                         paradigm,
                                         scorep_thread_get_team( tpd ),
                                         thread_id,
                                         generation_no );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }
}


SCOREP_TaskHandle
SCOREP_ThreadForkJoin_TaskBegin( SCOREP_ParadigmType paradigm,
                                 SCOREP_RegionHandle regionHandle,
                                 uint32_t            threadId,
                                 uint32_t            generationNumber )
{
    struct scorep_thread_private_data* tpd           = scorep_thread_get_private_data();
    SCOREP_Location*                   location      = scorep_thread_get_location( tpd );
    uint64_t                           timestamp     = scorep_get_timestamp( location );
    uint64_t*                          metric_values = SCOREP_Metric_Read( location );

    /* We create the task data construct late when the tasks starts running, because
     * the number of tasks that are running concurrently is usually much smaller
     * then the number of tasks in the creation queue. Thus, we need only a few
     * task data objects. Furthermore, we take the memory from the location memory
     * pool. Thus, if we create the data structure on another location than
     * the location that executes a task, we have a memory transfer problem.
     * However, task migration is very rare, usually the location that started the
     * execution will finish it. Thus, the memory flow is low. If we would create
     * the task data structure at task creation time, the memory transfer might be
     * significant (e.g. with master/worker schemes).
     */
    SCOREP_TaskHandle new_task = scorep_task_create( location,
                                                     threadId,
                                                     generationNumber );
    scorep_task_switch( location, new_task );

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_TaskBegin( location,
                                  timestamp,
                                  metric_values,
                                  regionHandle,
                                  new_task );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTaskSwitch( location,
                                         timestamp,
                                         paradigm,
                                         scorep_thread_get_team( tpd ),
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

    return new_task;
}


void
SCOREP_ThreadForkJoin_TaskEnd( SCOREP_ParadigmType paradigm,
                               SCOREP_RegionHandle regionHandle,
                               SCOREP_TaskHandle   task )
{
    struct scorep_thread_private_data* tpd           = scorep_thread_get_private_data();
    SCOREP_Location*                   location      = scorep_thread_get_location( tpd );
    uint64_t                           timestamp     = scorep_get_timestamp( location );
    uint64_t*                          metric_values = SCOREP_Metric_Read( location );
    uint32_t                           thread_id     = SCOREP_Task_GetThreadId( task );
    uint32_t                           generation_no = SCOREP_Task_GetGenerationNumber( task );

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_TaskEnd( location,
                                timestamp,
                                metric_values,
                                regionHandle,
                                task );
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
                                           scorep_thread_get_team( tpd ),
                                           thread_id,
                                           generation_no );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
    }

    scorep_task_complete( location, task );
}
