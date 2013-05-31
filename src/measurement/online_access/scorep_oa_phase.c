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
 * @file scorep_oa_phases.c
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file contains the implementation of Online Access phase enter and exit functions
 */

#include <config.h>
#include <stdio.h>
#include <assert.h>

#include <UTILS_Debug.h>

#include "SCOREP_Types.h"

#include "scorep_oa_phase.h"
#include "scorep_oa_mri_control.h"
#include "scorep_oa_connection.h"
#include "scorep_selective_region.h"

void
scorep_oa_phase_enter
(
    const SCOREP_RegionHandle ms_handle
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __func__ );

    assert( ms_handle != SCOREP_INVALID_REGION );
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "OA phase enter: region handle %ld\n", ms_handle );

    if ( scorep_oa_mri_get_appl_control() == SCOREP_OA_MRI_STATUS_RUNNING_TO_BEGINNING )
    {
        scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_SUSPENDED_BEGINNING );
        scorep_oa_mri_set_phase( ms_handle );
        scorep_oa_connection_send_string( connection, "SUSPENDED\n" );
        scorep_oa_mri_receive_and_process_requests( connection );
    }
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Leaving %s\n", __func__ );
}

void
scorep_oa_phase_exit
(
    const SCOREP_RegionHandle ms_handle
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __func__ );

    assert( ms_handle != SCOREP_INVALID_REGION );
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "OA phase exit: region handle %ld\n", ms_handle );

    if ( scorep_oa_mri_get_appl_control() == SCOREP_OA_MRI_STATUS_RUNNING_TO_END )
    {
        scorep_oa_mri_set_phase( ms_handle );
        scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_SUSPENDED_END );
        scorep_oa_connection_send_string( connection, "SUSPENDED\n" );
        scorep_oa_mri_receive_and_process_requests( connection );
    }
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Leaving %s\n", __func__ );
}
