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
 * @file       silc_timer_cycle_counter.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 * This is the implementation of the SILC_Timing.h interface that uses the
 * cycle_counter timer.
 */


#include "SILC_Timing.h"

#include <assert.h>

void
SILC_InitTimer()
{
    assert( false ); // implement me
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
