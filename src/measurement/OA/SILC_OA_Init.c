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
 * @file SILC_OA_Init.c
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file contains the implementation of Online Access initialization and finalization functions
 */


#include <config.h>

#include "silc_utility/SILC_Utils.h"
#include "SILC_Types.h"
#include "SILC_OA_Init.h"

#include "silc_oa_connection.h"
#include "silc_oa_mri_control.h"
#include "silc_status.h"

#include <stdio.h>

static int8_t silc_oa_is_initialized = 0;

SILC_Error_Code
SILC_OA_Init
(
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
    printf( "%d \n", 1 << 14 );
    if ( !SILC_IsOAEnabled() )
    {
        return SILC_SUCCESS;
    }

    if ( silc_oa_is_initialized == 0 )
    {
        /* Set the intialization flag to indicate that the adapter is initialized */
        silc_oa_is_initialized = 1;
        silc_oa_mri_set_appl_control( SILC_OA_MRI_STATUS_SUSPENDED_INITIALIZATION, 0, 0 );
        connection = silc_oa_connection_connect();
        silc_oa_mri_receive_and_process_requests( connection );
    }
    return SILC_SUCCESS;
}

int8_t
SILC_OA_Initialized
(
)
{
    return silc_oa_is_initialized;
}

SILC_Error_Code
SILC_OA_Finalize
(
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
    if ( !SILC_IsOAEnabled() )
    {
        return SILC_SUCCESS;
    }
    if ( silc_oa_is_initialized )
    {
        //silc_oa_mri_send_app_status(SILC_OA_MRI_SUSPENDED_TERMINATION,0,0);

        //silc_oa_mri_send_data(SILC_OA_MRI_SYMBOLS,void*,count)
    }
    return SILC_SUCCESS;
}
