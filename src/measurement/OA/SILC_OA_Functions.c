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
 * @file SILC_OA_Functions.c
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file contains the implementation of Online Access Phase functions
 */

#include <config.h>

#include "SILC_OA_Functions.h"
#include "SILC_OA_Init.h"

#include "silc_utility/SILC_Utils.h"

#include "SILC_Types.h"
#include "silc_oa_mri_control.h"
#include "silc_oa_connection.h"
#include "silc_status.h"

#include <stdio.h>



void
SILC_OA_PhaseBegin
(
    const SILC_RegionHandle* handle
)
{
    printf( "Entering %s\n", __FUNCTION__ );
    if ( !SILC_IsOAEnabled() || !SILC_OA_Initialized() )
    {
        return;
    }
    if ( silc_oa_mri_get_appl_control() == SILC_OA_MRI_STATUS_RUNNING_TO_BEGINNING )
    {
        silc_oa_mri_set_appl_control( SILC_OA_MRI_STATUS_SUSPENDED_BEGINNING, 0, 0 );
        silc_oa_connection_send_string( connection, "SUSPENDED\n" );
        silc_oa_mri_receive_and_process_requests( connection );
    }
    printf( "Leaving %s\n", __FUNCTION__ );
}

void
SILC_OA_PhaseEnd
(
    const SILC_RegionHandle* handle
)
{
    printf( "Entering %s\n", __FUNCTION__ );
    if ( !SILC_IsOAEnabled() || !SILC_OA_Initialized() )
    {
        return;
    }
    if ( silc_oa_mri_get_appl_control() == SILC_OA_MRI_STATUS_RUNNING_TO_END )
    {
        silc_oa_mri_set_appl_control( SILC_OA_MRI_STATUS_SUSPENDED_END, 0, 0 );
        silc_oa_connection_send_string( connection, "SUSPENDED\n" );
        silc_oa_mri_receive_and_process_requests( connection );
    }
    printf( "Leaving %s\n", __FUNCTION__ );
}
