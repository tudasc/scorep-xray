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

#include <stdio.h>
#include <strings.h>
#include <mpi.h>

#include <SCOREP_Profile.h>
#include <SCOREP_Mpi.h>
#include <SCOREP_Profile_OAConsumer.h>

#include "scorep_oa_connection.h"
#include "scanner.h"
#include "scorep_profile_node.h"
#include "scorep_profile_definition.h"


static scorep_oa_mri_app_control_type appl_control = SCOREP_OA_MRI_STATUS_UNDEFINED;

static int32_t                        scorep_phase_fileid, scorep_phase_rfl;

typedef struct Measurement
{
    int     rank;       ///<MPI process
    int     thread;     ///<thread
    int     fileId;     ///<region fileId
    int     rfl;        ///<region first line number
    int     regionType; ///<regionType, e.g., loop, subroutine
    int     samples;    ///<number of measurements
    int     metric;     ///<metric, e.g., execution time
    int     ignore;     ///<number of measurements that could not be performed due to shortage of counters
    double  fpVal;      ///<value as floating point number
    int64_t intVal;     ///<value as integer number
} MeasurementType;

static void
scorep_oa_mri_restart_profiling
(
);

static void
scorep_profile_print_func
(
    scorep_profile_node* node,
    void*                param
);

static void
scorep_oa_mri_parse_subtree( scorep_profile_node* node,
                             uint32_t             level );

static void
scorep_oa_mri_dump_profile();

SCOREP_Error_Code
scorep_oa_mri_receive_and_process_requests
(
    int connection                              ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
)
{
    int  length, i;
    char buffer[ 2000 ];
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
    buffer[ 0 ] = 0;
    bzero( buffer, 2000 );

    while ( scorep_oa_mri_get_appl_control() != SCOREP_OA_MRI_STATUS_RUNNING_TO_END
            && scorep_oa_mri_get_appl_control() != SCOREP_OA_MRI_STATUS_RUNNING_TO_BEGINNING
            && scorep_oa_mri_get_appl_control() != SCOREP_OA_MRI_EXEC_REQUEST_TERMINATE )
    {
        bzero( buffer, 2000 );

        while ( ( length = scorep_oa_connection_read_string( connection, buffer, 2000 ) ) == 0 )
        {
        }

        for ( i = 0; i < length; i++ )
        {
            buffer[ i ] = toupper( buffer[ i ] );
        }
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Received from socket: %s\n", buffer );


        if ( scorep_oa_mri_parse( buffer ) != SCOREP_SUCCESS )
        {
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "ERROR in parsing MRI command\n" );
        }

        if ( scorep_oa_mri_get_appl_control() == SCOREP_OA_MRI_EXEC_REQUEST_TERMINATE )
        {
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Application Terminating!\n" );
            //cleanup_registry();
            //PMPI_Finalize();
            //exit(0);
        }
    }

    PMPI_Barrier( MPI_COMM_WORLD );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Leaving %s\n", __FUNCTION__ );
    return SCOREP_SUCCESS;
}

void
scorep_oa_mri_set_appl_control
(
    scorep_oa_mri_app_control_type command,
    uint8_t                        file_id,     ///@TODO get rid of line number and file ID, should be OA_Phase name instead or region handle
    uint8_t                        region_line
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
    appl_control = command;
    // appl_control_file=file_id;
    //appl_control_region=region_line;
}

scorep_oa_mri_app_control_type
scorep_oa_mri_get_appl_control
(
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
    return appl_control;
}

void
scorep_oa_mri_return_summary_data
(
    int connection                              ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __FUNCTION__ );

    //scorep_profile_dump();void

    scorep_oa_mri_dump_profile();
    SCOREP_OA_PeriscopeSummary* summary = SCOREP_Profile_GetPeriscopeSummary();




    MeasurementType* send_buffer = calloc( summary->measurement_size, sizeof( MeasurementType ) );
    int              i;
    for ( i = 0; i < summary->measurement_size; i++ )
    {
        send_buffer[ i ].rank       = summary->context_buffer[ summary->measurement_buffer[ i ].context_id ].rank;
        send_buffer[ i ].thread     = summary->context_buffer[ summary->measurement_buffer[ i ].context_id ].thread_id;
        send_buffer[ i ].fileId     = scorep_phase_fileid;
        send_buffer[ i ].rfl        = scorep_phase_rfl;
        send_buffer[ i ].regionType = 27;
        send_buffer[ i ].samples    = summary->measurement_buffer->count;
        send_buffer[ i ].metric     = 540;
        send_buffer[ i ].ignore     = 0;
        send_buffer[ i ].fpVal      = 0;
        send_buffer[ i ].intVal     = summary->measurement_buffer->sum;
    }


    scorep_oa_connection_send_string( connection, "SUMMARYDATA\n" );

    scorep_oa_connection_send_data( connection, send_buffer, summary->measurement_size, sizeof( MeasurementType ) );

    free( summary->context_buffer );
    free( summary->measurement_buffer );
    free( summary );
    free( send_buffer );
}

void
scorep_profile_print_func
(
    scorep_profile_node* node,
    void*                param
)
{
    //SCOREP_OA_PeriscopeSummary* summary_buffer = (SCOREP_OA_PeriscopeSummary*)param;
    if ( node == NULL )
    {
        return;
    }

    static char* type_name_map[] = {
        "regular region",
        "paramater string",
        "parameter integer",
        "thread root",
        "thread start"
    };
    if ( node->node_type == scorep_profile_node_regular_region )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "+ type: %s, time=%ld, count=%ld;", type_name_map[ node->node_type ], node->inclusive_time.sum, node->count );


        int i;
        for ( i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
        {
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, " dense_%i=%ld", i, node->dense_metrics[ i ].sum );
        }
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, ";" );
        int                               number     = 0;
        scorep_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
        while ( sparse_int != NULL )
        {
            SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, " sparse_int_%d=%ld", number, sparse_int->sum );
            number++;
            sparse_int = sparse_int->next_metric;
        }
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, ";\n" );
    }
}

void
scorep_oa_mri_parse_subtree( scorep_profile_node* node,
                             uint32_t             level )
{
    static char* type_name_map[] = {
        "regular region",
        "paramater string",
        "parameter integer",
        "thread root",
        "thread start"
    };

    if ( node == NULL )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "<end of the subtree>\n" );
        return;
    }

    int i;
    for ( i = 0; i < level; i++ )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "| " );
    }
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "+ type: %s, time=%ld, count=%ld, sparse double:", type_name_map[ node->node_type ], node->inclusive_time.sum, node->count );
    scorep_profile_sparse_metric_double* sparse_double = node->first_double_sparse;
    int                                  number        = 0;
    while ( sparse_double != NULL )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, " metric_%d=%f", number, sparse_double->sum );
        number++;
        sparse_double = sparse_double->next_metric;
    }
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "; sparse int:" );
    scorep_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
    number = 0;
    while ( sparse_int != NULL )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, " metric_%d=%ld", number, sparse_int->sum );
        number++;
        sparse_int = sparse_int->next_metric;
    }
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, ";\n" );

    if ( node->first_child != NULL )
    {
        scorep_oa_mri_parse_subtree( node->first_child, level + 1 );
    }
    if ( node->next_sibling != NULL )
    {
        scorep_oa_mri_parse_subtree( node->next_sibling, level );
    }
}

void
scorep_oa_mri_dump_profile()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Number of dense metrics is %d \n", scorep_profile.num_of_dense_metrics );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "<Root>:\n" );
    //scorep_oa_mri_parse_subtree( scorep_profile.first_root_node, 0 );
    scorep_profile_for_all( scorep_profile.first_root_node, &scorep_profile_print_func, NULL );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "<End of the tree.>\n" );
}


void
scorep_oa_mri_noop
(
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
}


SCOREP_Error_Code
scorep_oa_mri_parse
(
    char* buffer
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
    SCOREP_Error_Code return_status = SCOREP_SUCCESS;

    YY_BUFFER_STATE   my_string_buffer;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "parse_mri_cmd: %s\n", buffer );
    my_string_buffer = yy_scan_string( buffer );
    if ( yyparse() == 0 )
    {
        //TRF_create_globalRegionStructure (tid);
    }
    else
    {
        return_status = SCOREP_ERROR_MEMORY_OUT_OF_PAGES;                                 ///@TODO introduce my own error code here
    }
    yy_delete_buffer( my_string_buffer );

    return return_status;
}

void
scorep_oa_mri_set_mpiprofiling
(
    int value
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
    SCOREP_MPI_HOOKS_SET( value );
}

void
scorep_oa_mri_set_profiling
(
    int value
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s\n", __FUNCTION__ );
    //SCOREP_MPI_HOOKS_SET(value);
}

void
scorep_oa_mri_restart_profiling
(
)
{
    SCOREP_Profile_Finalize();
    SCOREP_Profile_Initialize( 0, NULL );
}

void
scorep_oa_mri_setphase
(
    int32_t file_id,
    int32_t rfl
)
{
    scorep_phase_fileid = file_id;
    scorep_phase_rfl    = rfl;
}
