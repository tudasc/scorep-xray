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
 * @file       scorep_timer_KINDOFTIMER.c
 *
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * KINDOFTIMER.
 */


#include <config.h>
#include "SCOREP_Timing.h"

#include <assert.h>
#include <stdbool.h>

static bool isInitialized = false;

void
SCOREP_Timer_Initialize( void )
{
    assert( false ); // implement me
    if ( isInitialized )
    {
        return;
    }
    isInitialized = true;
}


uint64_t
SCOREP_GetClockTicks( void )
{
    assert( false ); // implement me
    return 0;
}


uint64_t
SCOREP_GetClockResolution( void )
{
    assert( false ); // implement me
    return 0;
}


bool
SCOREP_ClockIsGlobal( void )
{
    return false;
}
