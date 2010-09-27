/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 * @file       scorep_timer_gettimeofday_test.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * Test of the implementation of the SCOREP_Timing.h interface that uses the
 * gettimeofday timer.
 */

#include <config.h>

#include <SCOREP_Timing.h>

#include <assert.h>
#include <unistd.h>
#include <stdio.h>

int
main()
{
    SCOREP_Timer_Initialize();
    uint64_t startTime = SCOREP_GetClockTicks();
    uint64_t stopTime  = SCOREP_GetClockTicks();
    assert( stopTime > startTime );
    startTime = SCOREP_GetClockTicks();
    usleep( 100000 );
    stopTime = SCOREP_GetClockTicks();
    double diff = ( ( double )( stopTime - startTime ) ) / ( ( double )SCOREP_GetClockResolution() );
    printf( "Check clock accuracy. Expected difference 0.1.\n" );
    printf( "Measured difference: %f\n", diff );
    assert( ( diff > 0.08 ) && ( diff < 0.15 ) );
    return 0;
}
