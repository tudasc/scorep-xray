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
 * Copyright (c) 2009-2011, 2014,
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
 * This file contains the implementation of Online Access initialization and finalization functions
 */


#include <config.h>

#include "SCOREP_OA_RuntimeManagement.h"
#include "scorep_oa_connection.h"
#include "scorep_oa_mri_control.h"

#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include "scorep_oa_confvars.inc.c"

static int8_t is_initialized = 0;

void
SCOREP_OA_Register( void )
{
    SCOREP_ConfigRegister( "onlineaccess", scorep_oa_confvars );
}

int8_t
SCOREP_OA_Init( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );

    //printf("ENV variables: %ld %ld %s\n",scorep_oa_port,scorep_oa_registry_port,scorep_oa_registry_host);
    if ( is_initialized == 0 && SCOREP_OA_IS_REQUESTED )
    {
        /* Set the initialization flag to indicate that the adapter is initialized */
        is_initialized = 1;
        scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_SUSPENDED_INITIALIZATION );
        scorep_oa_connection = scorep_oa_connection_connect();
        scorep_oa_mri_receive_and_process_requests( scorep_oa_connection );
        return 1;
    }
    return 0;
}

int8_t
SCOREP_OA_Initialized( void )
{
    return is_initialized;
}

void
SCOREP_OA_Finalize( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );
    if ( is_initialized
         && scorep_oa_mri_get_appl_control() != SCOREP_OA_MRI_EXEC_REQUEST_TERMINATE )
    {
        scorep_oa_connection_send_string( scorep_oa_connection, "SUSPENDEDATEND\n" );
        if ( scorep_oa_connection_disconnect() != SCOREP_SUCCESS )
        {
            UTILS_WARNING( "Unable to disconnect from server.\n" );
        }
    }
}
