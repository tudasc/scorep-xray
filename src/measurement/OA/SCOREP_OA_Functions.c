/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file SCOREP_OA_Functions.c
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file contains the implementation of Online Access Phase functions
 */

#include <config.h>

#include "SCOREP_OA_Functions.h"
#include "SCOREP_OA_Init.h"

#include "scorep_utility/SCOREP_Utils.h"

#include "SCOREP_Types.h"
#include "scorep_oa_mri_control.h"
#include "scorep_oa_connection.h"
#include "scorep_status.h"

#include <stdio.h>



void
SCOREP_OA_PhaseBegin
(
    const SCOREP_RegionHandle* handle
)
{
    printf( "Entering %s\n", __FUNCTION__ );
    if ( !SCOREP_IsOAEnabled() || !SCOREP_OA_Initialized() )
    {
        return;
    }
    if ( scorep_oa_mri_get_appl_control() == SCOREP_OA_MRI_STATUS_RUNNING_TO_BEGINNING )
    {
        scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_SUSPENDED_BEGINNING, 0, 0 );
        scorep_oa_connection_send_string( connection, "SUSPENDED\n" );
        scorep_oa_mri_receive_and_process_requests( connection );
    }
    printf( "Leaving %s\n", __FUNCTION__ );
}

void
SCOREP_OA_PhaseEnd
(
    const SCOREP_RegionHandle* handle
)
{
    printf( "Entering %s\n", __FUNCTION__ );
    if ( !SCOREP_IsOAEnabled() || !SCOREP_OA_Initialized() )
    {
        return;
    }
    if ( scorep_oa_mri_get_appl_control() == SCOREP_OA_MRI_STATUS_RUNNING_TO_END )
    {
        scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_SUSPENDED_END, 0, 0 );
        scorep_oa_connection_send_string( connection, "SUSPENDED\n" );
        scorep_oa_mri_receive_and_process_requests( connection );
    }
    printf( "Leaving %s\n", __FUNCTION__ );
}
