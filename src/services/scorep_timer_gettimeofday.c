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
 * @file       scorep_timer_gettimeofday.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * gettimeofday timer.
 */

#include <config.h>
#include "SCOREP_Timing.h"

#include <sys/time.h>
#include <stdbool.h>


static time_t scorep_time_base = 0;
static bool   isInitialized    = false;


void
SCOREP_Timer_Initialize()
{
    if ( isInitialized )
    {
        return;
    }

    struct timeval tp;
    gettimeofday( &tp, 0 );
    scorep_time_base = tp.tv_sec - ( tp.tv_sec & 0xFFFF );
    isInitialized    = true;
}


uint64_t
SCOREP_GetClockTicks()
{
    struct timeval tp;
    gettimeofday( &tp, 0 );
    return ( ( tp.tv_sec - scorep_time_base ) * 1e6 ) + tp.tv_usec;
}


uint64_t
SCOREP_GetClockResolution()
{
    return 1e6;
}
