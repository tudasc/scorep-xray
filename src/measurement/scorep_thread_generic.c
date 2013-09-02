/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       src/measurement/scorep_thread_generic.c
 *
 *
 */

#include <config.h>
#include "scorep_thread.h"

#include <SCOREP_Mutex.h>
#include <SCOREP_Properties.h>

#include <tracing/SCOREP_Tracing_Events.h>
#include <profiling/SCOREP_Profile_Tasking.h>
#include <SCOREP_Metric_Management.h>

#include "scorep_location.h"
#include "scorep_events_common.h"

#include <UTILS_Error.h>

#include <stdlib.h>

/* Note: tpd is short for thread private data. This usually refers to
 * scorep_thread_private_data, not to thread private/local storage of a
 * particular threading model. */

struct scorep_thread_private_data
{
    SCOREP_Location*            location;
    scorep_thread_private_data* parent;
    /* SCOREP_Thread_CreatePrivateData() not only creates and inits
     * new scorep_thread_private_data objects, it also creates the
     * model_data objects. This creations are done in a *single*
     * allocation of size sizeof(scorep_thread_private_data) +
     * SCOREP_Thread_GetSizeofModelData().  This way, the first bytes
     * belong to the scorep_thread_private_data object tpd and the
     * model_data starts at tpd + 1. See also
     * SCOREP_Thread_GetModelData(). */
    /* void*                      model_data; */
};


static scorep_thread_private_data* scorep_thread_initial_tpd;

static SCOREP_Mutex scorep_thread_fork_sequence_count_lock;


void
SCOREP_Thread_Initialize()
{
    SCOREP_ErrorCode result = SCOREP_MutexCreate( &scorep_thread_fork_sequence_count_lock );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );

    UTILS_BUG_ON( scorep_thread_initial_tpd != 0 );
    scorep_thread_initial_tpd = scorep_thread_create_private_data(
        0 /* parent_tpd */,
        0 /* fork_sequence_count */ );
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
                                                scorep_thread_get_next_fork_sequence_count() );
}


scorep_thread_private_data*
scorep_thread_create_private_data( scorep_thread_private_data* parent,
                                   uint32_t                    forkSequenceCount )
{
    scorep_thread_private_data* new_tpd = calloc( 1, sizeof( scorep_thread_private_data )
                                                  + scorep_thread_get_sizeof_model_data() );
    UTILS_ASSERT( new_tpd != 0 );
    new_tpd->parent = parent;

    scorep_thread_on_create_private_data( new_tpd,
                                          scorep_thread_get_model_data( new_tpd ),
                                          forkSequenceCount );

    return new_tpd;
}


void
SCOREP_Thread_Finalize( void )
{
    UTILS_BUG_ON( scorep_thread_initial_tpd == 0 );
    UTILS_BUG_ON( scorep_thread_get_private_data() != scorep_thread_initial_tpd );

    scorep_thread_on_finalize( scorep_thread_initial_tpd );
    scorep_thread_delete_private_data( scorep_thread_initial_tpd );

    scorep_thread_initial_tpd              = 0;
    scorep_thread_fork_sequence_count_lock = 0;

    SCOREP_ErrorCode result =
        SCOREP_MutexDestroy( &scorep_thread_fork_sequence_count_lock );
    UTILS_ASSERT( result == SCOREP_SUCCESS );
}


static uint32_t fork_sequence_count = 0;


uint32_t
scorep_thread_get_next_fork_sequence_count( void )
{
    SCOREP_MutexLock( scorep_thread_fork_sequence_count_lock );
    uint32_t tmp = fork_sequence_count++;
    SCOREP_MutexUnlock( scorep_thread_fork_sequence_count_lock );
    return tmp;
}


void*
scorep_thread_get_model_data( scorep_thread_private_data* tpd )
{
    UTILS_ASSERT( tpd );
    /* See comment in struct scorep_thread_private_data definition.
     * return tpd->model_data; */
    return tpd + 1;
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
SCOREP_ThreadTeamBegin( SCOREP_ThreadModel model,
                        uint32_t           forkSequenceCount,
                        uint32_t           threadId )
{
    UTILS_ASSERT( model < SCOREP_INVALID_THREAD_MODEL );
    scorep_thread_private_data* parent_tpd  = 0;
    scorep_thread_private_data* current_tpd = 0;

    scorep_thread_on_team_begin( &parent_tpd,
                                 &current_tpd,
                                 &forkSequenceCount,
                                 model );

    UTILS_ASSERT( parent_tpd );
    UTILS_ASSERT( current_tpd );

    SCOREP_Location*                 parent_location  = scorep_thread_get_location( parent_tpd );
    SCOREP_Location*                 current_location = scorep_thread_get_location( current_tpd );
    uint64_t                         timestamp        = scorep_get_timestamp( current_location );
    SCOREP_InterimCommunicatorHandle thread_team      = scorep_thread_get_thread_team( current_tpd );

    SCOREP_Location_CallSubstratesOnActivation( current_location,
                                                parent_location,
                                                forkSequenceCount );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTeamBegin( current_location,
                                        timestamp,
                                        model,
                                        thread_team );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }

    if ( scorep_profiling_consume_event() )
    {
        //SCOREP_Profiling_ThreadTeamBegin( current_location, timestamp, forkSequenceCount, threadId, model );
    }
}


void
SCOREP_ThreadEnd( SCOREP_ThreadModel model,
                  uint32_t           forkSequenceCount )
{
    UTILS_ASSERT( model < SCOREP_INVALID_THREAD_MODEL );
    scorep_thread_private_data*      tpd         = scorep_thread_get_private_data();
    scorep_thread_private_data*      parent      = 0;
    SCOREP_Location*                 location    = scorep_thread_get_location( tpd );
    uint64_t                         timestamp   = scorep_get_timestamp( location );
    SCOREP_InterimCommunicatorHandle thread_team = scorep_thread_get_thread_team( tpd );

    scorep_thread_on_end( tpd, &parent, &forkSequenceCount, model );
    UTILS_ASSERT( parent );

    SCOREP_Location_CallSubstratesOnDeactivation(
        location,
        scorep_thread_get_location( parent ) );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTeamEnd( location,
                                      timestamp,
                                      model,
                                      thread_team );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }
}

void
SCOREP_ThreadTaskCreate( SCOREP_ThreadModel model,
                         uint32_t           threadId,
                         uint32_t           generationNumber )
{
    scorep_thread_private_data* tpd      = scorep_thread_get_private_data();
    SCOREP_Location*            location = scorep_thread_get_location( tpd );
    /* use the timestamp from the associated enter */
    uint64_t                         timestamp   = SCOREP_Location_GetLastTimestamp( location );
    SCOREP_InterimCommunicatorHandle thread_team = scorep_thread_get_thread_team( tpd );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTaskCreate( location,
                                         timestamp,
                                         model,
                                         thread_team,
                                         threadId,
                                         generationNumber );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }
}


void
SCOREP_ThreadTaskSwitch( SCOREP_ThreadModel model,
                         uint32_t           threadId,
                         uint32_t           generationNumber )
{
    scorep_thread_private_data*      tpd         = scorep_thread_get_private_data();
    SCOREP_Location*                 location    = scorep_thread_get_location( tpd );
    uint64_t                         timestamp   = scorep_get_timestamp( location );
    SCOREP_InterimCommunicatorHandle thread_team = scorep_thread_get_thread_team( tpd );

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
                                         model,
                                         thread_team,
                                         threadId,
                                         generationNumber );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }
}


void
SCOREP_ThreadTaskBegin( SCOREP_ThreadModel  model,
                        SCOREP_RegionHandle regionHandle,
                        uint32_t            threadId,
                        uint32_t            generationNumber )
{
    scorep_thread_private_data*      tpd           = scorep_thread_get_private_data();
    SCOREP_Location*                 location      = scorep_thread_get_location( tpd );
    uint64_t                         timestamp     = scorep_get_timestamp( location );
    uint64_t*                        metric_values = SCOREP_Metric_Read( location );
    SCOREP_InterimCommunicatorHandle thread_team   = scorep_thread_get_thread_team( tpd );

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
                                         model,
                                         thread_team,
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
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }
}


void
SCOREP_ThreadTaskEnd( SCOREP_ThreadModel  model,
                      SCOREP_RegionHandle regionHandle,
                      uint32_t            threadId,
                      uint32_t            generationNumber )
{
    scorep_thread_private_data*      tpd           = scorep_thread_get_private_data();
    SCOREP_Location*                 location      = scorep_thread_get_location( tpd );
    uint64_t                         timestamp     = scorep_get_timestamp( location );
    uint64_t*                        metric_values = SCOREP_Metric_Read( location );
    SCOREP_InterimCommunicatorHandle thread_team   = scorep_thread_get_thread_team( tpd );

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
                                           model,
                                           thread_team,
                                           threadId,
                                           generationNumber );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }
}
