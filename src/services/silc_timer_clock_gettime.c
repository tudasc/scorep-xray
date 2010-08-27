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
 * @file       silc_timer_clock_gettime.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * This is the implementation of the SILC_Timing.h interface that uses the
 * clock_gettime timer. Needs librt.
 */

#include <config.h>
#include "SILC_Timing.h"

#include <assert.h>
#include <stdbool.h>
#include <time.h>

static bool            isInitialized = false;

static struct timespec silc_timer_start;

void
SILC_Timer_Initialize()
{
    if ( isInitialized )
    {
        return;
    }
    clock_gettime( CLOCK_REALTIME, &silc_timer_start );
    isInitialized = true;
}


uint64_t
SILC_GetClockTicks()
{
    struct timespec time;
    clock_gettime( CLOCK_REALTIME, &time );
    return ( time.tv_sec - silc_timer_start.tv_sec ) * 1000000000
           + time.tv_nsec - silc_timer_start.tv_nsec;
}


uint64_t
SILC_GetClockResolution()
{
    return 1000000000;
}
