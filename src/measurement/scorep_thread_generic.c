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
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include "scorep_thread.h"

#include <SCOREP_Mutex.h>
#include <SCOREP_Properties.h>

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
        "" /* name */,
        true /* deferNewLocationNotication */ );

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
    UTILS_ASSERT( tpd->parent );
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
SCOREP_ThreadTeamBegin( uint32_t forkSequenceCount, uint32_t threadId, SCOREP_ThreadModel model )
{
    UTILS_ASSERT( model < SCOREP_INVALID_THREAD_MODEL );
    scorep_thread_private_data* parent_tpd          = 0;
    scorep_thread_private_data* current_tpd         = 0;
    char*                       current_name        = 0;
    bool                        location_is_created = false;

    scorep_thread_on_team_begin( &parent_tpd,
                                 &current_tpd,
                                 current_name,
                                 &forkSequenceCount,
                                 model,
                                 &location_is_created );
    UTILS_ASSERT( parent_tpd );
    UTILS_ASSERT( current_tpd );
    if ( forkSequenceCount == SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_UNIQUE_FORK_SEQUENCE_COUNTS );
    }

    SCOREP_Location* parent_location  = scorep_thread_get_location( parent_tpd );
    SCOREP_Location* current_location = scorep_thread_get_location( current_tpd );
    uint64_t         timestamp        = scorep_get_timestamp( current_location );

    if ( location_is_created )
    {
        SCOREP_Location_CallSubstratesOnNewLocation( current_location,
                                                     current_name,
                                                     parent_location );
    }
    SCOREP_Location_CallSubstratesOnActivation( current_location,
                                                parent_location,
                                                forkSequenceCount );

    if ( scorep_tracing_consume_event() )
    {
        //SCOREP_Tracing_ThreadTeamBegin( current_location, timestamp, forkSequenceCount, threadId, model );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        //SCOREP_Profiling_ThreadTeamBegin( current_location, timestamp, forkSequenceCount, threadId, model );
    }
}


void
SCOREP_ThreadEnd( uint32_t forkSequenceCount, SCOREP_ThreadModel model )
{
    UTILS_ASSERT( model < SCOREP_INVALID_THREAD_MODEL );
    scorep_thread_private_data* tpd       = scorep_thread_get_private_data();
    scorep_thread_private_data* parent    = 0;
    SCOREP_Location*            location  = scorep_thread_get_location( tpd );
    uint64_t                    timestamp = scorep_get_timestamp( location );

    scorep_thread_on_end( tpd, &parent, &forkSequenceCount, model );
    UTILS_ASSERT( parent );
    if ( forkSequenceCount == SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_UNIQUE_FORK_SEQUENCE_COUNTS );
    }

    SCOREP_Location_CallSubstratesOnDeactivation(
        location,
        scorep_thread_get_location( parent ) );

    if ( scorep_tracing_consume_event() )
    {
        //SCOREP_Tracing_ThreadEnd( location, timestamp, forkSequenceCount, model );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        //SCOREP_Profiling_ThreadEnd( location, timestamp, forkSequenceCount, model );
    }
}
