/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       scorep_timer_rts_get_timebase.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * rts_get_timebase timer.
 */


#include <config.h>
#include "SCOREP_Timing.h"

#include <assert.h>
#include <bglpersonality.h>
#include <rts.h>
#include <stdbool.h>

static uint64_t scorep_ticks_per_sec = 0;

static bool     isInitialized = false;

void
SCOREP_Timer_Initialize()
{
    if ( isInitialized )
    {
        return;
    }

    BGLPersonality mybgl;
    rts_get_personality( &mybgl, sizeof( BGLPersonality ) );
    scorep_ticks_per_sec = ( uint64_t )BGLPersonality_clockHz( &mybgl );

    isInitialized = true;
}


uint64_t
SCOREP_GetClockTicks()
{
    return ( uint64_t )rts_get_timebase();
}


uint64_t
SCOREP_GetClockResolution()
{
    return scorep_ticks_per_sec;
}
