/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
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

#include <SCOREP_Thread_Mgmt.h>
#include <scorep_location_management.h>
#include <scorep_subsystem_management.h>

#include <UTILS_Error.h>


static SCOREP_Location* scorep_thread_sole_cpu_location;


void
SCOREP_Thread_Initialize( void )
{
    UTILS_ASSERT( scorep_thread_sole_cpu_location == 0 );
    scorep_thread_sole_cpu_location =
        SCOREP_Location_CreateCPULocation( "Master thread" );
    UTILS_ASSERT( scorep_thread_sole_cpu_location );
}


void
SCOREP_Thread_ActivateLocation( struct SCOREP_Location* location,
                                struct SCOREP_Location* parent )
{
    scorep_subsystems_activate_cpu_location( location,
                                             parent,
                                             0 /* sequnce_count */,
                                             SCOREP_CPU_LOCATION_PHASE_MGMT );
}


void
SCOREP_Thread_Finalize( void )
{
}


bool
SCOREP_Thread_InParallel( void )
{
    return false;
}


SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation( void )
{
    return scorep_thread_sole_cpu_location;
}


bool
SCOREP_Thread_IsIntialThread( void )
{
    return true;
}
