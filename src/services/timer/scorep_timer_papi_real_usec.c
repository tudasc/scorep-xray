/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       scorep_timer_papi_real_usec.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * This is the implementation of the SCOREP_Timing.h interface that uses the
 * papi_real_usec timer.
 */


#include <config.h>
#include "SCOREP_Timing.h"
#include <UTILS_Error.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <papi.h>

static bool isInitialized = false;

void
SCOREP_Timer_Initialize()
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
            UTILS_ERROR( SCOREP_ERROR_PAPI_INIT );
            return;
        }
    }

    isInitialized = true;
}


uint64_t
SCOREP_GetClockTicks()
{
    return PAPI_get_real_usec();
}


uint64_t
SCOREP_GetClockResolution()
{
    return 1000000;
}


bool
SCOREP_ClockIsGlobal()
{
    return false;
}
