/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen, Germany
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
 * @file       src/measurement/scorep_thread_mockup.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include "scorep_thread.h"

#include "scorep_location.h"
#include <UTILS_Error.h>


static SCOREP_Location* scorep_thread_sole_cpu_location;


void
SCOREP_Thread_Initialize()
{
    UTILS_ASSERT( scorep_thread_sole_cpu_location == 0 );
    scorep_thread_sole_cpu_location = SCOREP_Location_CreateCPULocation( NULL, "Master thread",  /* deferNewLocationNotification = */ false );
    UTILS_ASSERT( scorep_thread_sole_cpu_location );
}


void
SCOREP_Thread_Finalize()
{
}


bool
SCOREP_Thread_InParallel()
{
    return false;
}


SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation()
{
    return scorep_thread_sole_cpu_location;
}
