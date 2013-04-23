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
 * @file       src/measurement/scorep_thread_generic_fork_join.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include "scorep_thread.h"

#include <tracing/SCOREP_Tracing_Events.h>
#include <SCOREP_Properties.h>

#include "scorep_events_common.h"

#include <UTILS_Error.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


uint32_t
SCOREP_ThreadFork( uint32_t nRequestedThreads, SCOREP_ThreadModel model )
{
    UTILS_ASSERT( model < SCOREP_INVALID_THREAD_MODEL );
    scorep_thread_private_data* tpd       = scorep_thread_get_private_data();
    SCOREP_Location*            location  = scorep_thread_get_location( tpd );
    uint64_t                    timestamp = scorep_get_timestamp( location );

    uint32_t fork_sequence_count = scorep_thread_get_next_fork_sequence_count();

    scorep_thread_on_fork( nRequestedThreads,
                           fork_sequence_count,
                           model,
                           scorep_thread_get_model_data( tpd ),
                           location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadFork( location, timestamp, nRequestedThreads, fork_sequence_count, model );
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
        SCOREP_Profile_ThreadFork( location, nRequestedThreads, fork_sequence_count );
    }

    return fork_sequence_count;
}


void
SCOREP_ThreadJoin( uint32_t forkSequenceCount, SCOREP_ThreadModel model )
{
    UTILS_ASSERT( model < SCOREP_INVALID_THREAD_MODEL );
    scorep_thread_private_data* tpd             = scorep_thread_get_private_data();
    scorep_thread_private_data* tpd_from_now_on = 0;

    scorep_thread_on_join( tpd,
                           scorep_thread_get_parent( tpd ),
                           &tpd_from_now_on,
                           &forkSequenceCount,
                           model );
    UTILS_BUG_ON( tpd_from_now_on == 0 );
    UTILS_ASSERT( tpd_from_now_on == scorep_thread_get_private_data() );
    if ( forkSequenceCount == SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_UNIQUE_FORK_SEQUENCE_COUNTS );
    }

    SCOREP_Location* location  = scorep_thread_get_location( tpd_from_now_on );
    uint64_t         timestamp = scorep_get_timestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadJoin( location, timestamp, forkSequenceCount, model );
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
        SCOREP_Profile_ThreadJoin( location );
    }
}