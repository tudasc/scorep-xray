/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
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
