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
 * @file       silc_timer_gettimeofday_test.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 * Test of the implementation of the SILC_Timing.h interface that uses the
 * gettimeofday timer.
 */

#include <SILC_Timing.h>

#include <assert.h>

int
main()
{
    SILC_InitTimer();
    uint64_t startTime = SILC_GetClockTicks();
    uint64_t stopTime  = SILC_GetClockTicks();
    assert( stopTime > startTime );
    return 0;
}
