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
 * @file       silc_timer_power_realtime.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * This is the implementation of the SILC_Timing.h interface that uses the
 * power_realtime timer.
 */


#include <config.h>
#include "SILC_Timing.h"

#include <assert.h>
#include <stdbool.h>

static bool isInitialized = false;

void
SILC_Timer_Initialize()
{
    assert( false ); // implement me
    if ( isInitialized )
    {
        return;
    }
    isInitialized = true;
}


uint64_t
SILC_GetClockTicks()
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
