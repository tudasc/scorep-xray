/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014,
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
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * bgq_mftb timer.
 */


#include <config.h>
#include "SCOREP_Timing.h"

#include <stdbool.h>
#include <firmware/include/personality.h>

static uint64_t scorep_ticks_per_sec = 0;
static bool     isInitialized        = false;

void
SCOREP_Timer_Initialize( void )
{
    if ( isInitialized )
    {
        return;
    }
    scorep_ticks_per_sec = ( uint64_t )( DEFAULT_FREQ_MHZ * 1e6 );

    isInitialized = true;
}


uint64_t
SCOREP_GetClockTicks( void )
{
    return ( uint64_t )__mftb();
}


uint64_t
SCOREP_GetClockResolution( void )
{
    return scorep_ticks_per_sec;
}


bool
SCOREP_ClockIsGlobal( void )
{
    return true;
}
