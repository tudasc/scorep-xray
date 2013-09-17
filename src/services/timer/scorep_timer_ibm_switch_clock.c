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
 * @file       scorep_timer_ibm_switch_clock.c
 *
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * ibm_switch_clock timer.
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
    return true;
}
