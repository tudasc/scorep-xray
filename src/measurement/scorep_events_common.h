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
 * @file       src/measurement/scorep_events_common.h
 *
 *
 */

#ifndef SCOREP_EVENTS_COMMON_H_
#define SCOREP_EVENTS_COMMON_H_


#include <stdbool.h>
#include <SCOREP_Timing.h>
#include "scorep_status.h"
#include <SCOREP_RuntimeManagement.h>


static inline uint64_t
scorep_get_timestamp( SCOREP_Location* location )
{
    uint64_t timestamp = SCOREP_GetClockTicks();
    SCOREP_Location_SetLastTimestamp( location, timestamp );
    return timestamp;
}


/* Use this predicate to decide, whether to record an event in the trace. */
static inline bool
scorep_tracing_consume_event( void )
{
    return SCOREP_IsTracingEnabled() && SCOREP_RecordingEnabled();
}


static inline bool
scorep_profiling_consume_event( void )
{
    return SCOREP_IsProfilingEnabled() && SCOREP_RecordingEnabled();
}


#endif /* SCOREP_EVENTS_COMMON_H_ */
