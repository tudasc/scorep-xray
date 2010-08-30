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

/** @file silc_oa_mri_control.c
    @maintainer Yury Oleynik <oleynik@in.tum.de>
    @status     ALPHA

    This file contains implementation of OA measurement configuration and application execution control functions
 */


#include <config.h>

#include "silc_oa_mri_control.h"
#include "silc_oa_connection.h"
#include "scanner.h"
#include "silc_profile_node.h"
#include "silc_profile_definition.h"

#include "SILC_Profile.h"
#include "SILC_Mpi.h"
#include "SILC_Profile_OAConsumer.h"

#include <stdio.h>
#include <strings.h>
#include <mpi.h>

static silc_oa_mri_app_control_type appl_control = SILC_OA_MRI_STATUS_UNDEFINED;

static int32_t                      silc_phase_fileid, silc_phase_rfl;

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
silc_oa_mri_restart_profiling
(
);

static void
silc_profile_print_func
(
    silc_profile_node* node,
    void*              param
);

static void
silc_oa_mri_parse_subtree( silc_profile_node* node,
                           uint32_t           level );

static void
silc_oa_mri_dump_profile();

SILC_Error_Code
silc_oa_mri_receive_and_process_requests
(
    int connection                              ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
)
{
    int  length, i;
    char buffer[ 2000 ];
    printf( "Entering %s\n", __FUNCTION__ );
    buffer[ 0 ] = 0;
    bzero( buffer, 2000 );

    while ( silc_oa_mri_get_appl_control() != SILC_OA_MRI_STATUS_RUNNING_TO_END
            && silc_oa_mri_get_appl_control() != SILC_OA_MRI_STATUS_RUNNING_TO_BEGINNING
            && silc_oa_mri_get_appl_control() != SILC_OA_MRI_EXEC_REQUEST_TERMINATE )
    {
        bzero( buffer, 2000 );

        while ( ( length = silc_oa_connection_read_string( connection, buffer, 2000 ) ) == 0 )
        {
        }

        for ( i = 0; i < length; i++ )
        {
            buffer[ i ] = toupper( buffer[ i ] );
        }
        printf( "Received from socket: %s\n", buffer );


        if ( silc_oa_mri_parse( buffer ) != SILC_SUCCESS )
        {
            printf( "ERROR in parsing MRI command\n" );
        }

        if ( silc_oa_mri_get_appl_control() == SILC_OA_MRI_EXEC_REQUEST_TERMINATE )
        {
            printf( "Application Terminating!\n" );
            //cleanup_registry();
            //PMPI_Finalize();
            //exit(0);
        }
    }

    PMPI_Barrier( MPI_COMM_WORLD );
    printf( "Leaving %s\n", __FUNCTION__ );
    return SILC_SUCCESS;
}

void
silc_oa_mri_set_appl_control
(
    silc_oa_mri_app_control_type command,
    uint8_t                      file_id,       ///@TODO get rid of line number and file ID, should be OA_Phase name instead or region handle
    uint8_t                      region_line
)
{
    //printf("Entering %s\n",__FUNCTION__);
    appl_control = command;
    // appl_control_file=file_id;
    //appl_control_region=region_line;
}

silc_oa_mri_app_control_type
silc_oa_mri_get_appl_control
(
)
{
    //printf("Entering %s\n",__FUNCTION__);
    return appl_control;
}

void
silc_oa_mri_return_summary_data
(
    int connection                              ///@TODO switch to appropriate connection object which contains call backs to the chosen communication layer
)
{
    printf( "Entering %s\n", __FUNCTION__ );

    //silc_profile_dump();void

    silc_oa_mri_dump_profile();
    SILC_OA_PeriscopeSummary* summary = SILC_Profile_GetPeriscopeSummary();




    MeasurementType* send_buffer = calloc( summary->measurement_size, sizeof( MeasurementType ) );
    int              i;
    for ( i = 0; i < summary->measurement_size; i++ )
    {
        send_buffer[ i ].rank       = summary->context_buffer[ summary->measurement_buffer[ i ].context_id ].rank;
        send_buffer[ i ].thread     = summary->context_buffer[ summary->measurement_buffer[ i ].context_id ].thread_id;
        send_buffer[ i ].fileId     = silc_phase_fileid;
        send_buffer[ i ].rfl        = silc_phase_rfl;
        send_buffer[ i ].regionType = 27;
        send_buffer[ i ].samples    = summary->measurement_buffer->count;
        send_buffer[ i ].metric     = 540;
        send_buffer[ i ].ignore     = 0;
        send_buffer[ i ].fpVal      = 0;
        send_buffer[ i ].intVal     = summary->measurement_buffer->sum;
    }


    silc_oa_connection_send_string( connection, "SUMMARYDATA\n" );

    silc_oa_connection_send_data( connection, send_buffer, summary->measurement_size, sizeof( MeasurementType ) );

    free( summary->context_buffer );
    free( summary->measurement_buffer );
    free( summary );
    free( send_buffer );
}

void
silc_profile_print_func
(
    silc_profile_node* node,
    void*              param
)
{
    //SILC_OA_PeriscopeSummary* summary_buffer = (SILC_OA_PeriscopeSummary*)param;
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
    if ( node->node_type == silc_profile_node_regular_region )
    {
        printf( "+ type: %s, time=%ld, count=%ld;", type_name_map[ node->node_type ], node->inclusive_time.sum, node->count );


        int i;
        for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
        {
            printf( " dense_%i=%ld", i, node->dense_metrics[ i ].sum );
        }
        printf( ";" );
        int                             number     = 0;
        silc_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
        while ( sparse_int != NULL )
        {
            printf( " sparse_int_%d=%ld", number, sparse_int->sum );
            number++;
            sparse_int = sparse_int->next_metric;
        }
        printf( ";\n" );
    }
}

void
silc_oa_mri_parse_subtree( silc_profile_node* node,
                           uint32_t           level )
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
        printf( "<end of the subtree>\n" );
        return;
    }

    int i;
    for ( i = 0; i < level; i++ )
    {
        printf( "| " );
    }
    printf( "+ type: %s, time=%ld, count=%ld, sparse double:", type_name_map[ node->node_type ], node->inclusive_time.sum, node->count );
    silc_profile_sparse_metric_double* sparse_double = node->first_double_sparse;
    int                                number        = 0;
    while ( sparse_double != NULL )
    {
        printf( " metric_%d=%f", number, sparse_double->sum );
        number++;
        sparse_double = sparse_double->next_metric;
    }
    printf( "; sparse int:" );
    silc_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
    number = 0;
    while ( sparse_int != NULL )
    {
        printf( " metric_%d=%ld", number, sparse_int->sum );
        number++;
        sparse_int = sparse_int->next_metric;
    }
    printf( ";\n" );

    if ( node->first_child != NULL )
    {
        silc_oa_mri_parse_subtree( node->first_child, level + 1 );
    }
    if ( node->next_sibling != NULL )
    {
        silc_oa_mri_parse_subtree( node->next_sibling, level );
    }
}

void
silc_oa_mri_dump_profile()
{
    printf( "Number of dense metrics is %d \n", silc_profile.num_of_dense_metrics );
    printf( "<Root>:\n" );
    //silc_oa_mri_parse_subtree( silc_profile.first_root_node, 0 );
    silc_profile_for_all( silc_profile.first_root_node, &silc_profile_print_func, NULL );
    printf( "<End of the tree.>\n" );
}


void
silc_oa_mri_noop
(
)
{
    printf( "Entering %s\n", __FUNCTION__ );
}


SILC_Error_Code
silc_oa_mri_parse
(
    char* buffer
)
{
    printf( "Entering %s\n", __FUNCTION__ );
    SILC_Error_Code return_status = SILC_SUCCESS;

    YY_BUFFER_STATE my_string_buffer;

    printf( "parse_mri_cmd: %s\n", buffer );
    my_string_buffer = yy_scan_string( buffer );
    if ( yyparse() == 0 )
    {
        //TRF_create_globalRegionStructure (tid);
    }
    else
    {
        return_status = SILC_ERROR_MEMORY_OUT_OF_PAGES;                                 ///@TODO introduce my own error code here
    }
    yy_delete_buffer( my_string_buffer );

    return return_status;
}

void
silc_oa_mri_set_mpiprofiling
(
    int value
)
{
    printf( "Entering %s\n", __FUNCTION__ );
    SILC_MPI_HOOKS_SET( value );
}

void
silc_oa_mri_set_profiling
(
    int value
)
{
    printf( "Entering %s\n", __FUNCTION__ );
    //SILC_MPI_HOOKS_SET(value);
}

void
silc_oa_mri_restart_profiling
(
)
{
    SILC_Profile_Finalize();
    SILC_Profile_Initialize( 0, NULL );
}

void
silc_oa_mri_setphase
(
    int32_t file_id,
    int32_t rfl
)
{
    silc_phase_fileid = file_id;
    silc_phase_rfl    = rfl;
}
