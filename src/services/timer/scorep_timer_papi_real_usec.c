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
 * papi_real_usec timer.
 */


#include <config.h>
#include "SCOREP_Timing.h"
#include <UTILS_Error.h>

#include <stdbool.h>
#include <stddef.h>
#include <papi.h>

static bool isInitialized = false;

void
SCOREP_Timer_Initialize( void )
{
    int retval;

    if ( isInitialized )
    {
        return;
    }

    retval = PAPI_is_initialized();
    if ( retval != PAPI_LOW_LEVEL_INITED )
    {
        retval = PAPI_library_init( PAPI_VER_CURRENT );
        if ( retval != PAPI_VER_CURRENT && retval > 0 )
        {
            UTILS_ERROR( SCOREP_ERROR_PAPI_INIT,
                         "Coudn't init PAPI with version %d, got %d",
                         PAPI_VER_CURRENT, retval );
            return;
        }
    }

    isInitialized = true;
}


uint64_t
SCOREP_GetClockTicks( void )
{
    return PAPI_get_real_usec();
}


uint64_t
SCOREP_GetClockResolution( void )
{
    return 1000000;
}


bool
SCOREP_ClockIsGlobal( void )
{
    return false;
}
