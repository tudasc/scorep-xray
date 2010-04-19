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
 * @file       silc_timer_papi_real_usec.c
 * @author     Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 * This is the implementation of the SILC_Timing.h interface that uses the
 * papi_real_usec timer.
 */


#include "SILC_Timing.h"
#include "SILC_Utils.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <papi.h>

static bool isInitialized = false;

void
SILC_InitTimer()
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
            SILC_ERROR( SILC_ERROR_PAPI_INIT, "" );
            return;
        }
    }

    isInitialized = true;
}


uint64_t
SILC_GetClockTicks()
{
    return PAPI_get_real_usec();
}


uint64_t
SILC_GetClockResolution()
{
    return 1000000;
}
