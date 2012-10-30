/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @file       src/measurement/scorep_thread_serial.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include "scorep_thread.h"

#include "scorep_location.h"
#include <UTILS_Error.h>


static const bool       scorep_we_should_not_be_here_in_serial_mode = false;
static SCOREP_Location* scorep_thread_only_cpu_location;


void
SCOREP_Thread_Initialize()
{
    scorep_thread_only_cpu_location = SCOREP_Location_CreateCPULocation( NULL, "",  false /* deferNewLocationNotication */ );
    UTILS_ASSERT( scorep_thread_only_cpu_location );
}


void
SCOREP_Thread_Finalize()
{
}


void
SCOREP_Thread_OnThreadFork( size_t nRequestedThreads )
{
    UTILS_ASSERT( scorep_we_should_not_be_here_in_serial_mode );
}


void
SCOREP_Thread_OnThreadJoin()
{
    UTILS_ASSERT( scorep_we_should_not_be_here_in_serial_mode );
}


// temporary to get nesting running. nesting_level will be replaced by
// fork_count (in generic threading) to match parent- and child-threads.
uint32_t
scorep_thread_get_nesting_level()
{
    UTILS_ASSERT( scorep_we_should_not_be_here_in_serial_mode );
    return 1;
}


SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation()
{
    return scorep_thread_only_cpu_location;
}
