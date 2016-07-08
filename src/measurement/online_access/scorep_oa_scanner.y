/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

 /**
 * @file
 *
 * @brief   automatically generated file for the MRI string parser
 *
 */

/* *INDENT-OFF* */
%{

/*----------------------------------------------------------------------------*/
/* Loads system interface modules.                                            */
/*----------------------------------------------------------------------------*/
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*----------------------------------------------------------------------------*/
/* Loads own interface modules.                                               */
/*----------------------------------------------------------------------------*/
#include "scorep_oa_mri_control.h"
#include "SCOREP_OA_Request.h"
#include "scorep_oa_connection.h"
#include "SCOREP_OA_RuntimeManagement.h"
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>

/*----------------------------------------------------------------------------*/
/* Defines Macros                                                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Declaration of types                                                       */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Variable declarations:                                                     */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Declaration of Procedures/Functions prototypes:                            */
/*----------------------------------------------------------------------------*/
void yyerror(char *s);
int yylex (void);

%}

/******************************************************************************/
/******************************************************************************/
/* Section with Yacc specification follows                                    */
/******************************************************************************/
/******************************************************************************/
%union
{
  int   Number;
  char *String;
}

%token T_BEGINREQUESTS
%token T_ENDREQUESTS
%token T_SETNUMITERATIONS
%token T_RUNTOSTART
%token T_RUNTOEND
%token T_GETSUMMARYDATA
%token T_TERMINATE
%token T_REQUEST
%token <Number> T_NUMBER
%token <String> T_QSTRING
%token <String> T_STRING
%token T_GLOBAL
%token T_MPI
%token T_METRIC
%token T_EXECUTION_TIME

%token T_PERISCOPE
%token T_PAPI
%token T_PERF
%token T_RUSAGE
%token T_PLUGIN
%token T_OTHER

%start Command

%%

/******************************************************************************/
/* Section with Yacc rules follows                                            */
/******************************************************************************/

Command               : ControlCommand  ';'
                      | RequestCommand  ';' {
                                                scorep_oa_connection_send_string( scorep_oa_connection, "OK\n" );
                                            }

ControlCommand        : T_BEGINREQUESTS                            {
                                                                       UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Standby for requests..." );
                                                                       scorep_oa_requests_begin();
                                                                       scorep_oa_connection_send_string( scorep_oa_connection, "OK\n" );
                                                                   }
                      | T_ENDREQUESTS                              {
                                                                       UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Requests submitted" );
                                                                       scorep_oa_requests_end();
                                                                       scorep_oa_connection_send_string( scorep_oa_connection, "OK\n" );
                                                                   }
                      | T_SETNUMITERATIONS T_NUMBER                {
                                                                       UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Request to set number of iterations" );
                                                                       scorep_oa_mri_set_num_iterations( $2 );
                                                                   }
                      | T_RUNTOSTART '(' T_NUMBER ',' T_NUMBER ')' {
                                                                       UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Execution request run_to_beginning received" );
                                                                       scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_RUNNING_TO_BEGINNING );
                                                                   }
                      | T_RUNTOEND   '(' T_NUMBER ',' T_NUMBER ')' {
                                                                       UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Execution request run_to_end received" );
                                                                       scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_STATUS_RUNNING_TO_END );
                                                                   }
                      | T_GETSUMMARYDATA                           {
                                                                       UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Measured data requested" );
                                                                       scorep_oa_mri_return_summary_data( scorep_oa_connection );
                                                                   }
                      | T_TERMINATE                                {
                                                                       UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Termination request received" );
                                                                       scorep_oa_mri_set_appl_control( SCOREP_OA_MRI_EXEC_REQUEST_TERMINATE );
                                                                       scorep_oa_connection_send_string( scorep_oa_connection, "OK\n" );
                                                                   }

RequestCommand        : T_REQUEST '[' NodeList ']' RequestKind     {
                                                                       UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Measurements are requested" );
                                                                   }

NodeList              : NodeNrs
                      | '*'                                        {
                                                                       UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Node lists are ignored" );
                                                                   }

NodeNrs               : T_NUMBER                                   {
                                                                       UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Node lists are ignored" );
                                                                   }

RequestKind           : T_GLOBAL GlobalRequestList

GlobalRequestList     : GlobalRequest ',' GlobalRequestList
                      | GlobalRequest

GlobalRequest         : T_MPI                        {
                                                         scorep_oa_mri_set_mpiprofiling( 1 );
                                                         scorep_oa_requests_add_metric_by_name( UTILS_CStr_dup( "late_send" ), NULL, SCOREP_METRIC_SOURCE_MPI_PROFILING );
                                                         scorep_oa_requests_add_metric_by_name( UTILS_CStr_dup( "late_receive" ), NULL, SCOREP_METRIC_SOURCE_MPI_PROFILING );
                                                     }
                      | T_METRIC MetricSpecifier
                      | T_EXECUTION_TIME             {
                                                         scorep_oa_requests_add_metric_by_name( UTILS_CStr_dup( "execution_time" ), NULL, SCOREP_METRIC_TIMER );
                                                     }

MetricSpecifier       : T_PAPI T_QSTRING             {
                                                         scorep_oa_requests_add_metric_by_name( $2, NULL, SCOREP_METRIC_SOURCE_PAPI );
                                                     }
                      | T_PERF T_QSTRING             {
                                                         scorep_oa_requests_add_metric_by_name( $2, NULL, SCOREP_METRIC_SOURCE_PERF );
                                                     }
                      | T_RUSAGE T_QSTRING           {
                                                         scorep_oa_requests_add_metric_by_name( $2, NULL, SCOREP_METRIC_SOURCE_RUSAGE );
                                                     }
                      | T_PLUGIN T_QSTRING T_QSTRING {
                                                         scorep_oa_requests_add_metric_by_name( $3, $2, SCOREP_METRIC_SOURCE_PLUGIN );
                                                     }
                      | T_OTHER T_QSTRING            {
                                                         scorep_oa_requests_add_metric_by_name( $2, NULL, SCOREP_METRIC_OTHER );
                                                     }
%%


/******************************************************************************/
/* Error handling                                                             */
/******************************************************************************/
void yyerror( char* s )
{
    fprintf( stderr, "Unknown online access request: %s\n", s );
    fprintf( stderr, "Aborting...\n" );
    abort();
}
