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
 * @file       silc_timer_cray_rtclock.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 * This is the implementation of the SILC_Timing.h interface that uses the
 * cray_rtclock timer.
 */


#include "SILC_Timing.h"

#include <assert.h>
#include <stdbool.h>

static bool isInitialized = false;

void
SILC_InitTimer()
{
    assert( false ); // implement me
    if ( isInitialized )
    {
        return;
    }
    isInitialized = true;
}


uint64_t
SILC_GetWallClockTime()
{
    assert( false ); // implement me
    return 0;
}


uint64_t
SILC_GetClockResolution()
{
    assert( false ); // implement me
    return 0;
}
