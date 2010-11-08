/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
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
 * @file       scorep_timer_clock_gettime.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * clock_gettime timer. Needs librt.
 */

#include <config.h>
#include "SCOREP_Timing.h"

#include <assert.h>
#include <stdbool.h>
#include <time.h>

static bool            isInitialized = false;

static struct timespec scorep_timer_start;

void
SCOREP_Timer_Initialize()
{
    if ( isInitialized )
    {
        return;
    }
    clock_gettime( CLOCK_REALTIME, &scorep_timer_start );
    isInitialized = true;
}


uint64_t
SCOREP_GetClockTicks()
{
    struct timespec time;
    clock_gettime( CLOCK_REALTIME, &time );
    return ( time.tv_sec - scorep_timer_start.tv_sec ) * 1000000000
           + time.tv_nsec - scorep_timer_start.tv_nsec;
}


uint64_t
SCOREP_GetClockResolution()
{
    return 1000000000;
}
