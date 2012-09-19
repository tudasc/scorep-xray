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
 * @file SCOREP_OA_Functions.c
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file contains the implementation of Online Access Phase functions
 */

#include <config.h>
#include <SCOREP_RuntimeManagement.h>
#include <stdio.h>

#include "scorep/SCOREP_User_Functions.h"
#include "SCOREP_OA_Functions.h"
#include "scorep_profile_definition.h"
#include "SCOREP_OA_Init.h"
#include "SCOREP_Types.h"
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include "scorep_oa_phase.h"
#include "scorep_status.h"

void
SCOREP_OA_PhaseBegin
(
    SCOREP_User_RegionHandle*    handle,
    const char**                 lastFileName,
    SCOREP_SourceFileHandle*     lastFile,
    const char*                  name,
    const SCOREP_User_RegionType regionType,
    const char*                  fileName,
    const uint32_t               lineNo
)
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA );

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

    SCOREP_User_RegionInit( \
        handle, lastFileName, lastFile, name, \
        regionType, fileName, lineNo );

    scorep_oa_phase_enter( *handle );

    SCOREP_User_RegionEnter( *handle );
}


void
SCOREP_OA_PhaseEnd
(
    const SCOREP_User_RegionHandle handle
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __func__ );

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

    SCOREP_User_RegionEnd( handle );

    scorep_oa_phase_exit( handle );
}
