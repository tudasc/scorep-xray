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
 * @file       silc_timer_rts_get_timebase.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 * This is the implementation of the SILC_Timing.h interface that uses the
 * rts_get_timebase timer.
 */


#include "SILC_Timing.h"

#include <assert.h>
#include <bglpersonality.h>
#include <rts.h>
#include <stdbool.h>

static uint64_t silc_ticks_per_sec = 0;

static bool     isInitialized = false;

void
SILC_InitTimer()
{
    if ( isInitialized )
    {
        return;
    }

    BGLPersonality mybgl;
    rts_get_personality( &mybgl, sizeof( BGLPersonality ) );
    silc_ticks_per_sec = ( uint64_t )BGLPersonality_clockHz( &mybgl );

    isInitialized = true;
}


uint64_t
SILC_GetWallClockTime()
{
    return ( uint64_t )rts_get_timebase();
}


uint64_t
SILC_GetClockResolution()
{
    return silc_ticks_per_sec;
}
