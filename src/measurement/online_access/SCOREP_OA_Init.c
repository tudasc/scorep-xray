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
 * @file SCOREP_OA_Init.c
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file contains the implementation of Online Access initialization and finalization functions
 */


#include <config.h>
#include <SCOREP_Config.h>
#include <SCOREP_RuntimeManagement.h>

#include "scorep_utility/SCOREP_Utils.h"
#include "SCOREP_Types.h"
#include "SCOREP_OA_Init.h"


#include "scorep_oa_connection.h"
#include "scorep_oa_mri_control.h"
#include "scorep_status.h"

#include <stdio.h>

static int8_t scorep_oa_is_initialized = 0;
bool          scorep_oa_is_requested   = false;


/**
   Configuration variable registration structures for the profiling system.
 */
static SCOREP_ConfigVariable scorep_oa_configs[] = {
    { "REG_PORT",
      SCOREP_CONFIG_TYPE_NUMBER,
      &silc_oa_registry_port,
      NULL,
      "50100",
      "Online access registry service port",
      "Online access registry service port" },
    { "ENABLE",
      SCOREP_CONFIG_TYPE_BOOL,
      &scorep_oa_is_requested,
      NULL,
      "false",
      "enable online access interface",
      "enable online access interface" },
    { "REG_HOST",
      SCOREP_CONFIG_TYPE_STRING,
      &silc_oa_registry_host,
      NULL,
      "localhost",
      "Online access registry service hostname",
      "Online access registry service hostname" },
    { "BASE_PORT",
      SCOREP_CONFIG_TYPE_NUMBER,
      &silc_oa_port,
      NULL,
      "50010",
      "Base port for online access server",
      "Base port for online access server" },
    { "APPL_NAME",
      SCOREP_CONFIG_TYPE_STRING,
      &silc_oa_app_name,
      NULL,
      "appl",
      "application name to be registered",
      "application name to be registered" },
    SCOREP_CONFIG_TERMINATOR
};

void
SCOREP_OA_Register()
{
    SCOREP_ConfigRegister( "ONLINEACCESS", scorep_oa_configs );
}

int8_t
SCOREP_OA_Init
(
)
{
    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __FUNCTION__ );

    //printf("ENV variables: %ld %ld %s\n",silc_oa_port,silc_oa_registry_port,silc_oa_registry_host);
    if ( scorep_oa_is_initialized == 0 && SCOREP_OA_IS_REQUESTED )
    {
        /* Set the intialization flag to indicate that the adapter is initialized */
        scorep_oa_is_initialized = 1;
        scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_SUSPENDED_INITIALIZATION, 0, 0 );
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

int8_t
SCOREP_OA_Finalize
(
)
{
    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __FUNCTION__ );
    if ( !SCOREP_IsOAEnabled() )
    {
        return 1;
    }
    if ( scorep_oa_is_initialized )
    {
        //scorep_oa_mri_send_app_status(SCOREP_OA_MRI_SUSPENDED_TERMINATION,0,0);

        //scorep_oa_mri_send_data(SCOREP_OA_MRI_SYMBOLS,void*,count)
    }
    return 1;
}
