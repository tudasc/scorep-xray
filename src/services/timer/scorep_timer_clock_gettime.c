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

#define _POSIX_C_SOURCE 199309L
#include <config.h>
#include "SCOREP_Timing.h"

#include <assert.h>
#include <stdbool.h>
#include <time.h>

static bool isInitialized = false;

void
SCOREP_Timer_Initialize()
{
    if ( isInitialized )
    {
        return;
    }
    isInitialized = true;
}


uint64_t
SCOREP_GetClockTicks()
{
    struct timespec time;
    clock_gettime( CLOCK_REALTIME, &time );
    return ( uint64_t )time.tv_sec * ( uint64_t )1000000000 + time.tv_nsec;
}


uint64_t
SCOREP_GetClockResolution()
{
    return 1000000000;
}


bool
SCOREP_ClockIsGlobal()
{
    return false;
}
