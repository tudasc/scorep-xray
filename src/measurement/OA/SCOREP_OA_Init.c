/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 * @file SCOREP_OA_Init.c
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file contains the implementation of Online Access initialization and finalization functions
 */


#include <config.h>

#include "scorep_utility/SCOREP_Utils.h"
#include "SCOREP_Types.h"
#include "SCOREP_OA_Init.h"

#include "scorep_oa_connection.h"
#include "scorep_oa_mri_control.h"
#include "scorep_status.h"

#include <stdio.h>

static int8_t scorep_oa_is_initialized = 0;

SCOREP_Error_Code
SCOREP_OA_Init
(
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
    printf( "%d \n", 1 << 14 );
    if ( !SCOREP_IsOAEnabled() )
    {
        return SCOREP_SUCCESS;
    }

    if ( scorep_oa_is_initialized == 0 )
    {
        /* Set the intialization flag to indicate that the adapter is initialized */
        scorep_oa_is_initialized = 1;
        scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_SUSPENDED_INITIALIZATION, 0, 0 );
        connection = scorep_oa_connection_connect();
        scorep_oa_mri_receive_and_process_requests( connection );
    }
    return SCOREP_SUCCESS;
}

int8_t
SCOREP_OA_Initialized
(
)
{
    return scorep_oa_is_initialized;
}

SCOREP_Error_Code
SCOREP_OA_Finalize
(
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
    if ( !SCOREP_IsOAEnabled() )
    {
        return SCOREP_SUCCESS;
    }
    if ( scorep_oa_is_initialized )
    {
        //scorep_oa_mri_send_app_status(SCOREP_OA_MRI_SUSPENDED_TERMINATION,0,0);

        //scorep_oa_mri_send_data(SCOREP_OA_MRI_SYMBOLS,void*,count)
    }
    return SCOREP_SUCCESS;
}
