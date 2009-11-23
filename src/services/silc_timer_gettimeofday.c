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
 * @file       silc_timer_gettimeofday.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 * This is the implementation of the SILC_Timing.h interface that uses the
 * gettimeofday timer.
 */

#include "SILC_Timing.h"

#include <sys/time.h>
#include <stdbool.h>


static time_t silc_time_base = 0;
static bool   isInitialized  = false;


void
SILC_InitTimer()
{
    if ( isInitialized )
    {
        return;
    }

    struct timeval tp;
    gettimeofday( &tp, 0 );
    silc_time_base = tp.tv_sec - ( tp.tv_sec & 0xFFFF );
    isInitialized  = true;
}


uint64_t
SILC_GetWallClockTime()
{
    struct timeval tp;
    gettimeofday( &tp, 0 );
    return ( ( tp.tv_sec - silc_time_base ) * 1e6 ) + tp.tv_usec;
}


uint64_t
SILC_GetClockResolution()
{
    return 1e6;
}
