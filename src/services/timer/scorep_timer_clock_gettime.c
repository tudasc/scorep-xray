/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 *
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * clock_gettime timer. Needs librt.
 */

#include <config.h>

#ifdef _POSIX_C_SOURCE
#  if _POSIX_C_SOURCE < 199309L
#    undef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 199309L
#  endif
#else
#  define _POSIX_C_SOURCE 199309L
#endif

#include "SCOREP_Timing.h"

#include <UTILS_Error.h>
#include <stdbool.h>
#include <time.h>

/* clock_gettime fills a timespec struct that gives us seconds and
 * nanoseconds. In SCOREP_GetClockTicks we return nanoseconds, so
 * the resolution is always nanoseconds.
 */
#define SCOREP_CLOCK_GETTIME_FREQUENCY 1000000000

static bool isInitialized = false;

void
SCOREP_Timer_Initialize( void )
{
    if ( isInitialized )
    {
        return;
    }
    isInitialized = true;
}


uint64_t
SCOREP_GetClockTicks( void )
{
    struct timespec time;
    int             result = clock_gettime( SCOREP_CLOCK_GETTIME_CLOCK, &time );
    UTILS_ASSERT( result == 0 );
    return ( uint64_t )time.tv_sec * ( uint64_t )SCOREP_CLOCK_GETTIME_FREQUENCY + time.tv_nsec;
}


uint64_t
SCOREP_GetClockResolution( void )
{
    return SCOREP_CLOCK_GETTIME_FREQUENCY;
}


bool
SCOREP_ClockIsGlobal( void )
{
    return false;
}
