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
 * @file SCOREP_OA_RuntimeManagement.c
 *
 * This file contains the implementation of Online Access initialization and finalization functions
 */


#include <config.h>
#include <UTILS_Debug.h>
#include <SCOREP_Config.h>
#include <SCOREP_RuntimeManagement.h>

#include "SCOREP_Types.h"
#include "SCOREP_OA_RuntimeManagement.h"


#include "scorep_oa_connection.h"
#include "scorep_oa_mri_control.h"
#include "scorep_status.h"

#include <stdio.h>

static int8_t scorep_oa_is_initialized = 0;

#include "scorep_oa_confvars.inc.c"

void
SCOREP_OA_Register()
{
    SCOREP_ConfigRegister( "onlineaccess", scorep_oa_configs );
}

int8_t
SCOREP_OA_Init
(
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );

    //printf("ENV variables: %ld %ld %s\n",silc_oa_port,silc_oa_registry_port,silc_oa_registry_host);
    if ( scorep_oa_is_initialized == 0 && SCOREP_OA_IS_REQUESTED )
    {
        /* Set the intialization flag to indicate that the adapter is initialized */
        scorep_oa_is_initialized = 1;
        scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_SUSPENDED_INITIALIZATION );
        connection = scorep_oa_connection_connect();
        scorep_oa_mri_receive_and_process_requests( connection );
        return 1;
    }
    return 0;
}

int8_t
SCOREP_OA_Initialized
(
)
{
    return scorep_oa_is_initialized;
}

void
SCOREP_OA_Finalize
(
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );
    if ( scorep_oa_is_initialized
         && scorep_oa_mri_get_appl_control() != SCOREP_OA_MRI_EXEC_REQUEST_TERMINATE )
    {
        scorep_oa_connection_send_string( connection, "SUSPENDEDATEND\n" );
        //scorep_oa_mri_receive_and_process_requests( connection );
    }
}
