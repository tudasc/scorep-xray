/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
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

#include <scorep_substrates_definition.h>
#include <scorep_subsystem_management.h>

#include <SCOREP_Mutex.h>
#include <SCOREP_Paradigms.h>
#include <SCOREP_Properties.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Metric_Management.h>
#include <scorep_location_management.h>
#include <scorep_events_common.h>
#include <scorep_task_internal.h>
#include <SCOREP_Mutex.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME THREAD_FORK_JOIN
#include <UTILS_Debug.h>

#include <string.h>


/* Note: tpd is short for thread private data. This usually refers to
 * scorep_thread_private_data, not to thread private/local storage of a
 * particular threading model. */

static SCOREP_Location** first_fork_locations;
static SCOREP_Mutex      first_fork_locations_mutex;


void
scorep_thread_create_mutexes( void )
{
    SCOREP_ErrorCode result = SCOREP_MutexCreate( &first_fork_locations_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "" );
}


void
scorep_thread_destroy_mutexes( void )
{
    SCOREP_ErrorCode result = SCOREP_MutexDestroy( &first_fork_locations_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );
}


bool
SCOREP_Thread_InParallel( void )
{
    if ( scorep_thread_get_parent( scorep_thread_get_private_data() ) == NULL )
    {
        return false;
    }
    return true;
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

    SCOREP_CALL_SUBSTRATE( ThreadForkJoinFork, THREAD_FORK_JOIN_FORK,
                           ( location, timestamp, paradigm,
                             nRequestedThreads, sequence_count ) );

    /* We transit the master thread into the PAUSE phase, so that no
     * events are allowed until we enter the thread team */
    scorep_subsystems_deactivate_cpu_location( location, NULL,
                                               SCOREP_CPU_LOCATION_PHASE_PAUSE );
}


void
SCOREP_ThreadForkJoin_Join( SCOREP_ParadigmType                 paradigm,
                            struct scorep_thread_private_data** tpdFromNowOn )
{
    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_FORK_JOIN ),
                  "Provided paradigm not of fork/join class" );

    struct scorep_thread_private_data* tpd = scorep_thread_get_private_data();

    scorep_thread_on_join( tpd,
                           scorep_thread_get_parent( tpd ),
                           tpdFromNowOn,
                           paradigm );
    UTILS_BUG_ON( *tpdFromNowOn == 0, "" );
    UTILS_ASSERT( *tpdFromNowOn == scorep_thread_get_private_data() );

    SCOREP_InterimCommunicatorHandle team =
        scorep_thread_get_parent_team_handle( scorep_thread_get_team( tpd ) );
    scorep_thread_set_team( *tpdFromNowOn, team );

    SCOREP_Location* location  = scorep_thread_get_location( *tpdFromNowOn );
    uint64_t         timestamp = scorep_get_timestamp( location );

    SCOREP_CALL_SUBSTRATE( ThreadForkJoinJoin, THREAD_FORK_JOIN_JOIN,
                           ( location, timestamp, paradigm ) );

    /* The master thread was in the PAUSE phase, so that no events are
     * allowed since we left the thread team */
    scorep_subsystems_activate_cpu_location( location, NULL, 0,
                                             SCOREP_CPU_LOCATION_PHASE_PAUSE );
}


void
SCOREP_ThreadForkJoin_TeamBegin( SCOREP_ParadigmType                 paradigm,
                                 uint32_t                            threadId,
                                 uint32_t                            teamSize,
                                 uint32_t                            nestingLevel,
                                 void*                               ancestorInfo,
                                 struct scorep_thread_private_data** newTpd,
                                 SCOREP_TaskHandle*                  newTask )
{
    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_FORK_JOIN ),
                  "Provided paradigm not of fork/join class" );
    struct scorep_thread_private_data* parent_tpd = NULL;
    scorep_thread_on_team_begin_get_parent( nestingLevel, ancestorInfo, &parent_tpd );
    UTILS_BUG_ON( parent_tpd == NULL, "Valid parent required." );
    uint32_t sequence_count = scorep_thread_get_tmp_sequence_count( parent_tpd );
    UTILS_BUG_ON( sequence_count == SCOREP_THREAD_INVALID_SEQUENCE_COUNT,
                  "Valid sequence count required." );

    if ( sequence_count == 1 && teamSize > 1 )
    {
        SCOREP_MutexLock( first_fork_locations_mutex );
        if ( !first_fork_locations[ 0 ] )
        {
            char location_name[ 80 ];
            for ( int i = 0; i < teamSize - 1; ++i )
            {
                scorep_thread_create_location_name( location_name, 80, i + 1, parent_tpd );
                first_fork_locations[ i ] = SCOREP_Location_CreateCPULocation( location_name );
            }
        }
        SCOREP_MutexUnlock( first_fork_locations_mutex );
    }

    *newTpd = NULL;
    bool location_is_created = false;

    scorep_thread_on_team_begin( parent_tpd,
                                 newTpd,
                                 paradigm,
                                 threadId,
                                 teamSize,
                                 ( sequence_count == 1 ) ? first_fork_locations : 0,
                                 &location_is_created );

    UTILS_ASSERT( *newTpd );

    SCOREP_Location* parent_location  = scorep_thread_get_location( parent_tpd );
    SCOREP_Location* current_location = scorep_thread_get_location( *newTpd );
    uint64_t         timestamp        = scorep_get_timestamp( current_location );

    if ( location_is_created )
    {
        scorep_subsystems_initialize_location( current_location,
                                               parent_location );
    }

    /* handles recursion into the same singleton thread-team */
    SCOREP_InterimCommunicatorHandle team = scorep_thread_get_team_handle(
        current_location,
        scorep_thread_get_team( parent_tpd ),
        teamSize,
        threadId );
    scorep_thread_set_team( *newTpd, team );

    /* Only call into the substrate for newly activated locations. */
    if ( threadId != 0 )
    {
        scorep_subsystems_activate_cpu_location( current_location,
                                                 parent_location,
                                                 sequence_count,
                                                 SCOREP_CPU_LOCATION_PHASE_MGMT );
    }

    SCOREP_CALL_SUBSTRATE( ThreadForkJoinTeamBegin, THREAD_FORK_JOIN_TEAM_BEGIN,
                           ( current_location, timestamp, paradigm, team ) );

    /* Call subsystems on location activation, the master thread transits from
     * the PAUSE phase */
    scorep_subsystems_activate_cpu_location( current_location,
                                             NULL, 0,
                                             threadId == 0
                                             ? SCOREP_CPU_LOCATION_PHASE_PAUSE
                                             : SCOREP_CPU_LOCATION_PHASE_EVENTS );

    *newTask = SCOREP_Task_GetCurrentTask( current_location );
}


void
SCOREP_ThreadForkJoin_TeamEnd( SCOREP_ParadigmType paradigm,
                               int                 threadId,
                               int                 teamSize )
{
    UTILS_ASSERT( threadId >= 0 && teamSize > 0 );
    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_FORK_JOIN ),
                  "Provided paradigm not of fork/join class" );
    struct scorep_thread_private_data* tpd      = scorep_thread_get_private_data();
    struct scorep_thread_private_data* parent   = 0;
    SCOREP_Location*                   location = scorep_thread_get_location( tpd );
    SCOREP_InterimCommunicatorHandle   team     = scorep_thread_get_team( tpd );

    scorep_thread_on_team_end( tpd, &parent, threadId, teamSize, paradigm );
    UTILS_ASSERT( parent );

    /* First notify the subsystems about the deactivation of the location.
     * the master thread goes into the PAUSE phase. */
    scorep_subsystems_deactivate_cpu_location( location,
                                               NULL,
                                               threadId == 0
                                               ? SCOREP_CPU_LOCATION_PHASE_PAUSE
                                               : SCOREP_CPU_LOCATION_PHASE_EVENTS );

    uint64_t timestamp = scorep_get_timestamp( location );
    SCOREP_CALL_SUBSTRATE( ThreadForkJoinTeamEnd, THREAD_FORK_JOIN_TEAM_END,
                           ( location, timestamp, paradigm, team ) );

    if ( threadId != 0 )
    {
        scorep_subsystems_deactivate_cpu_location( location,
                                                   scorep_thread_get_location( parent ),
                                                   SCOREP_CPU_LOCATION_PHASE_MGMT );
    }
}

void
SCOREP_ThreadForkJoin_TaskCreate( SCOREP_ParadigmType paradigm,
                                  uint32_t            threadId,
                                  uint32_t            generationNumber )
{
    struct scorep_thread_private_data* tpd      = scorep_thread_get_private_data();
    SCOREP_Location*                   location = scorep_thread_get_location( tpd );
    /* use the timestamp from the associated enter */
    uint64_t                         timestamp = SCOREP_Location_GetLastTimestamp( location );
    SCOREP_InterimCommunicatorHandle team      = scorep_thread_get_team( tpd );

    SCOREP_CALL_SUBSTRATE( ThreadForkJoinTaskCreate, THREAD_FORK_JOIN_TASK_CREATE,
                           ( location, timestamp, paradigm,
                             team, threadId, generationNumber ) );
}


void
SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_ParadigmType paradigm,
                                  SCOREP_TaskHandle   task )
{
    struct scorep_thread_private_data* tpd           = scorep_thread_get_private_data();
    SCOREP_Location*                   location      = scorep_thread_get_location( tpd );
    uint64_t                           timestamp     = scorep_get_timestamp( location );
    uint64_t*                          metric_values = SCOREP_Metric_Read( location );
    SCOREP_InterimCommunicatorHandle   team          = scorep_thread_get_team( tpd );

    scorep_task_switch( location, task );

    uint32_t thread_id     = SCOREP_Task_GetThreadId( task );
    uint32_t generation_no = SCOREP_Task_GetGenerationNumber( task );

    SCOREP_CALL_SUBSTRATE( ThreadForkJoinTaskSwitch, THREAD_FORK_JOIN_TASK_SWITCH,
                           ( location, timestamp, metric_values, paradigm,
                             team, thread_id, generation_no, task ) );
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
    SCOREP_InterimCommunicatorHandle   team          = scorep_thread_get_team( tpd );

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

    SCOREP_CALL_SUBSTRATE( ThreadForkJoinTaskBegin, THREAD_FORK_JOIN_TASK_BEGIN,
                           ( location, timestamp, regionHandle, metric_values,
                             paradigm, team, threadId, generationNumber, new_task ) );

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
    SCOREP_InterimCommunicatorHandle   team          = scorep_thread_get_team( tpd );
    uint64_t*                          metric_values = SCOREP_Metric_Read( location );
    uint32_t                           thread_id     = SCOREP_Task_GetThreadId( task );
    uint32_t                           generation_no = SCOREP_Task_GetGenerationNumber( task );

    /*void*                      task_substrate_data = scorep_task_get_substrate_data( task );*/
    SCOREP_CALL_SUBSTRATE( ThreadForkJoinTaskEnd, THREAD_FORK_JOIN_TASK_END,
                           ( location, timestamp, regionHandle, metric_values,
                             paradigm, team, thread_id, generation_no, task ) );

    scorep_task_complete( location, task );
}
