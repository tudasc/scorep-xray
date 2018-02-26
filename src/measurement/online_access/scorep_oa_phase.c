/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014,
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
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * This file contains the implementation of Online Access phase enter and exit functions
 */

#include <config.h>
#include <stdio.h>
#include <inttypes.h>

#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include "SCOREP_Types.h"

#include "scorep_oa_phase.h"
#include "scorep_oa_mri_control.h"
#include "scorep_oa_connection.h"
#include "scorep_selective_region.h"

void
scorep_oa_phase_enter( const SCOREP_RegionHandle handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );

    UTILS_ASSERT( handle != SCOREP_INVALID_REGION );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "OA phase enter: region handle %" PRIu32, handle );

    if ( scorep_oa_mri_get_appl_control() == SCOREP_OA_MRI_STATUS_RUNNING_TO_BEGINNING )
    {
        scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_SUSPENDED_BEGINNING );
        scorep_oa_mri_set_phase( handle );
        scorep_oa_connection_send_string( scorep_oa_connection, "SUSPENDED\n" );
        scorep_oa_mri_receive_and_process_requests( scorep_oa_connection );
    }
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Leaving %s", __func__ );
}

void
scorep_oa_phase_exit( const SCOREP_RegionHandle handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __func__ );

    UTILS_ASSERT( handle != SCOREP_INVALID_REGION );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "OA phase exit: region handle %" PRIu32, handle );

    if ( scorep_oa_mri_get_appl_control() == SCOREP_OA_MRI_STATUS_RUNNING_TO_END )
    {
        scorep_oa_mri_set_phase( handle );
        scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_SUSPENDED_END );
        scorep_oa_connection_send_string( scorep_oa_connection, "SUSPENDED\n" );
        scorep_oa_mri_receive_and_process_requests( scorep_oa_connection );
    }
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Leaving %s", __func__ );
}
