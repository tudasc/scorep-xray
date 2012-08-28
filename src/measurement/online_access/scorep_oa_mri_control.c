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
 * @file scorep_oa_mri_control.c
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file contains implementation of OA measurement configuration and application execution control functions
 */

#include <config.h>

#include "scorep_oa_mri_control.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#ifdef WITH_MPI
        #include <mpi.h>
        #include <SCOREP_Mpi.h>
#endif
#include <ctype.h>

#include <SCOREP_Profile.h>
#include <SCOREP_Profile_OAConsumer.h>
#include <SCOREP_RuntimeManagement.h>

#include "scorep_oa_connection.h"
#include "scorep_profile_node.h"
#include "scorep_profile_definition.h"
#include "scorep_oa_request.h"


/**
 * Stores current application execution control status
 */
static scorep_oa_mri_app_control_type appl_control = SCOREP_OA_MRI_STATUS_UNDEFINED;

/**
 * Stores the currently used Online Access phase region handle
 */
static SCOREP_RegionHandle phase_handle = SCOREP_INVALID_REGION;

/**
 * Helper function to call a parser
 *
 * @param buffer string buffer to parse
 */
SCOREP_Error_Code
scorep_oa_mri_parse
(
    char* buffer
);

/**
 * Declaration of the parser function generated by flex
 */
int
yyparse
(
    void
);

void
scorep_oa_mri_receive_and_process_requests
(
    int connection
)
{
    int  length, i;
    char buffer[ 2000 ];
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __FUNCTION__ );
    buffer[ 0 ] = 0;
    memset( buffer, '\0', 2000 );

    while ( scorep_oa_mri_get_appl_control() != SCOREP_OA_MRI_STATUS_RUNNING_TO_END
            && scorep_oa_mri_get_appl_control() != SCOREP_OA_MRI_STATUS_RUNNING_TO_BEGINNING
            && scorep_oa_mri_get_appl_control() != SCOREP_OA_MRI_EXEC_REQUEST_TERMINATE )
    {
        memset( buffer, '\0', 2000 );

        while ( ( length = scorep_oa_connection_read_string( connection, buffer, 2000 ) ) == 0 )
        {
        }

        for ( i = 0; i < length; i++ )
        {
            buffer[ i ] = toupper( buffer[ i ] );
        }
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Received from socket: %s", buffer );

        if ( scorep_oa_mri_parse( buffer ) != SCOREP_SUCCESS )
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "ERROR in parsing MRI command" );
        }
        if ( scorep_oa_mri_get_appl_control() == SCOREP_OA_MRI_EXEC_REQUEST_TERMINATE )
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Terminating application!" );
            SCOREP_FinalizeMeasurement();
            _Exit( EXIT_SUCCESS );
        }
    }
#ifdef WITH_MPI
    PMPI_Barrier( MPI_COMM_WORLD );
#endif
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Leaving %s", __FUNCTION__ );
}

scorep_oa_mri_app_control_type
scorep_oa_mri_get_appl_control
(
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __FUNCTION__ );
    return appl_control;
}

void
scorep_oa_mri_set_appl_control
(
    scorep_oa_mri_app_control_type command
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __FUNCTION__ );
    appl_control = command;
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Execution control is SET TO: %d", ( int )command );
}

void
scorep_oa_mri_set_phase
(
    SCOREP_RegionHandle handle
)
{
    phase_handle = handle;
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Phase set to region (handle = %ld )\n", phase_handle );
}

void
scorep_oa_mri_noop
(
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __FUNCTION__ );
}

void
scorep_oa_mri_set_mpiprofiling
(
    int value
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __FUNCTION__ );
#ifdef WITH_MPI
    if ( value )
    {
        SCOREP_MPI_HOOKS_ON;
        scorep_mpiprofile_reinit_metrics();
    }
    else
    {
        SCOREP_MPI_HOOKS_OFF;
    }
#else
    scorep_oa_connection_send_string( connection, "this is serial version of Score-P, no MPI available\n" );
#endif
}

void
scorep_oa_mri_add_metric_by_code
(
    int metric_code
)
{
    SCOREP_OA_RequestsAddPeriscopeMetric( metric_code );
}


void
scorep_oa_mri_begin_request
(
)
{
    SCOREP_OA_RequestBegin();
}

void
scorep_oa_mri_end_request
(
)
{
    SCOREP_OA_RequestsSubmit();
}

void
scorep_oa_mri_return_summary_data
(
    int connection
)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __FUNCTION__ );

    /** Initialize OA Consumer interface and index Profile data */
    SCOREP_OAConsumer_Initialize( phase_handle );

    /** Get number of merged regions definitions*/
    int region_defs_size = ( int )SCOREP_OAConsumer_GetDataSize( MERGED_REGION_DEFINITIONS );
    /** Generate merged regions definitions buffer*/
    SCOREP_OA_CallPathRegionDef* region_defs = ( SCOREP_OA_CallPathRegionDef* )SCOREP_OAConsumer_GetData(
        MERGED_REGION_DEFINITIONS );

    /** Send merged region definitions to the agent*/
    //printf( "Sending MERGED_REGION_DEFINITIONS size: %d elements of size %d\n", region_defs_size, sizeof( SCOREP_OA_CallPathRegionDef ) );
    scorep_oa_connection_send_string( connection, "MERGED_REGION_DEFINITIONS\n" );
    scorep_oa_connection_send_data( connection, region_defs, region_defs_size, sizeof( SCOREP_OA_CallPathRegionDef ) );

    /** Get number of static profile records*/
    int static_profile_size = ( int )SCOREP_OAConsumer_GetDataSize( FLAT_PROFILE );
    /** Get static profile buffer*/
    SCOREP_OA_FlatProfileMeasurement* static_profile = ( SCOREP_OA_FlatProfileMeasurement* )SCOREP_OAConsumer_GetData(
        FLAT_PROFILE );
    /** Send static profile to the agent*/
    //printf( "Sending STATIC_PROFILE size: %d elements of size %d\n", static_profile_size, sizeof( SCOREP_OA_FlatProfileMeasurement ) );
    scorep_oa_connection_send_string( connection, "FLAT_PROFILE\n" );
    scorep_oa_connection_send_data( connection, static_profile, static_profile_size, sizeof( SCOREP_OA_FlatProfileMeasurement ) );

    /** Get number of metric definitions */
    int metric_def_size = ( int )SCOREP_OAConsumer_GetDataSize( COUNTER_DEFINITIONS );
    /** Get metric definitions */
    SCOREP_OA_CallPathCounterDef* metric_def = ( SCOREP_OA_CallPathCounterDef* )SCOREP_OAConsumer_GetData(
        COUNTER_DEFINITIONS );
    /** Send metric definitions */
    scorep_oa_connection_send_string( connection, "METRIC_DEFINITIONS\n" );
    scorep_oa_connection_send_data( connection, metric_def, metric_def_size, sizeof( SCOREP_OA_CallPathCounterDef ) );

    /** Dissmiss the data*/
    SCOREP_OAConsumer_DismissData();

    SCOREP_OA_RequestsDismiss();
}
