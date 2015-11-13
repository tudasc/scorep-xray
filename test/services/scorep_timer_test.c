/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 * Test of the implementation of the SCOREP_Timer_*.h interface using
 * the default timer.
 */

#include <config.h>

#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Timer_Utils.h>

#include <assert.h>
#include <unistd.h>
#include <stdio.h>

int
main()
{
    SCOREP_Timer_Initialize();
    uint64_t startTime = SCOREP_Timer_GetClockTicks();
    uint64_t stopTime  = SCOREP_Timer_GetClockTicks();
    assert( stopTime > startTime );
    startTime = SCOREP_Timer_GetClockTicks();
    sleep( 1 );
    stopTime = SCOREP_Timer_GetClockTicks();
    double diff = ( ( double )( stopTime - startTime ) ) / ( ( double )SCOREP_Timer_GetClockResolution() );
    printf( "Check clock accuracy. Expected difference 1.\n" );
    printf( "Measured difference: %f\n", diff );
    assert( ( diff > 0.8 ) && ( diff < 1.5 ) );
    return 0;
}
