/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file
 *
 *
 */

#include <config.h>

#include <SCOREP_ThreadForkJoin_Mgmt.h>
#include <scorep_location.h>
#include <SCOREP_Subsystem.h>

#include <UTILS_Error.h>


static SCOREP_Location* scorep_thread_sole_cpu_location;


void
SCOREP_ThreadForkJoin_Initialize()
{
    UTILS_ASSERT( scorep_thread_sole_cpu_location == 0 );
    scorep_thread_sole_cpu_location = SCOREP_Location_CreateCPULocation( NULL, "Master thread",  /* deferNewLocationNotification = */ false );
    UTILS_ASSERT( scorep_thread_sole_cpu_location );
}


void
SCOREP_ThreadForkJoin_Finalize()
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


const SCOREP_Subsystem SCOREP_Subsystem_ThreadForkJoin =
{
    .subsystem_name              = "THREAD FORK JOIN",
    .subsystem_register          = NULL,
    .subsystem_init              = NULL,
    .subsystem_init_location     = NULL,
    .subsystem_finalize_location = NULL,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = NULL,
    .subsystem_deregister        = NULL,
    .subsystem_control           = NULL
};
