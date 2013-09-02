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
 * @file       scorep_timer_gettimeofday.c
 *
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * gettimeofday timer.
 */

#include <config.h>
#include "SCOREP_Timing.h"

#include <sys/time.h>
#include <stdbool.h>


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
    struct timeval tp;
    gettimeofday( &tp, 0 );
    return ( tp.tv_sec * 1e6 ) + tp.tv_usec;
}


uint64_t
SCOREP_GetClockResolution( void )
{
    return 1e6;
}


bool
SCOREP_ClockIsGlobal( void )
{
    return false;
}
