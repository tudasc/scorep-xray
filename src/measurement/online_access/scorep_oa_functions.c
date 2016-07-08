/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * This file contains the implementation of Online Access Phase functions
 */

#include <config.h>
#include <SCOREP_OA_Functions.h>

#include <SCOREP_RuntimeManagement.h>
#include <stdio.h>

#include <scorep_profile_definition.h>
#include "SCOREP_OA_RuntimeManagement.h"
#include "SCOREP_Types.h"
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include "scorep_oa_phase.h"
#include <scorep_status.h>

int        scorep_oa_iterations_per_phase = 1;
static int remaining_iterations           = 0;
static int in_oa_phase                    = 0;

void
SCOREP_OA_PhaseBegin( const SCOREP_RegionHandle handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );
    if ( in_oa_phase )
    {
        return;
    }

    remaining_iterations = scorep_oa_iterations_per_phase;
    in_oa_phase          = 1;

    if ( !SCOREP_IsInitialized() )
    {
        SCOREP_InitMeasurement();
    }

    if ( !SCOREP_IsOAEnabled() || !SCOREP_OA_IS_REQUESTED )
    {
        return;
    }

    if ( !scorep_profile.is_initialized )
    {
        UTILS_WARNING( "Online Access works only in Profiling mode. Online Access will be deactivated." );
        scorep_oa_is_requested = false;
        return;
    }

    if ( !SCOREP_OA_Initialized() )
    {
        SCOREP_OA_Init();
    }

    scorep_oa_phase_enter( handle );
}


void
SCOREP_OA_PhaseEnd( const SCOREP_RegionHandle handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );

    if ( --remaining_iterations > 0 )
    {
        return;
    }

    in_oa_phase = 0;

    if ( !SCOREP_IsOAEnabled() || !SCOREP_OA_IS_REQUESTED )
    {
        return;
    }

    if ( !scorep_profile.is_initialized )
    {
        UTILS_WARNING( "Online Access works only in Profiling mode. Online Access will be deactivated." );
        scorep_oa_is_requested = false;
        return;
    }

    if ( !SCOREP_OA_Initialized() )
    {
        return;
    }

    scorep_oa_phase_exit( handle );
}
