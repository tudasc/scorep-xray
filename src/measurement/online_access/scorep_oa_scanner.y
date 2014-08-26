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
 * Copyright (c) 2009-2011,
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
/******************************************************************************/
/* yacc.l:                                 Version vom 19.11.1999 */
/* ========================================================================== */
/******************************************************************************/
%{

#include <config.h>
#include "scorep_oa_mri_control.h"
#include "SCOREP_OA_Request.h"
#include "scorep_oa_connection.h"
#include "SCOREP_OA_RuntimeManagement.h"
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>
#include <stdio.h>
#include <string.h>

void yyerror(char *s);
int yylex (void);
/*----------------------------------------------------------------------------*/
/* Laden der System Interface Dateien.                                        */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Laden der eigenen Interface Dateien.                                       */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Definiere Makros                                                           */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* TYPE-Deklarationen                                                         */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* VAR-Deklaration:                                                           */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* FORWARD-Deklaration von Prozeduren/Funktionen:                             */
/*----------------------------------------------------------------------------*/

%}

/******************************************************************************/
/******************************************************************************/
/* Es folgt der Definitionsteil der yacc Spezifikation                        */
/******************************************************************************/
/******************************************************************************/
%union
{
  int   Zahl;
  char *String;
}

%token T_CALL
%token T_CONFIG
%token T_EXECUTION_TIME
%token T_GLOBAL
%token T_INCLUDE
%token T_L1_CACHE_MISS
%token T_OFF
%token T_ON
%token T_ONLY
%token T_REGION_SUMMARY
%token T_REQUEST
%token <String> T_STRING
%token T_USER
%token <Zahl>   T_ZAHL
%token T_TERMINATE
%token T_RUNTOSTART
%token T_RUNTOEND
%token T_GETSUMMARYDATA
%token T_BEGINREQUESTS
%token T_ENDREQUESTS
%token T_SDDF_BUFFER_FLUSH_RATE
%token T_MPI
%token T_LOCAL
%token T_METRIC
%token T_PERISCOPE
%token T_PAPI
%token T_RUSAGE
%token T_OTHER


%start Command

%%

/******************************************************************************/
/* Es folgten die yacc-Regeln.                                                */
/******************************************************************************/

Command          : RequestCommand ';' {scorep_oa_connection_send_string(scorep_oa_connection,"OK\n");}
                 | ControlCommand ';'


ControlCommand   : T_TERMINATE {UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Termination request received\n");scorep_oa_mri_set_appl_control(SCOREP_OA_MRI_EXEC_REQUEST_TERMINATE);scorep_oa_connection_send_string(scorep_oa_connection,"OK\n");}
                 | T_RUNTOSTART '(' T_ZAHL ',' T_ZAHL ')' {UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Execution request run_to_beginning received\n");scorep_oa_mri_set_appl_control(SCOREP_OA_MRI_STATUS_RUNNING_TO_BEGINNING);}
                 | T_RUNTOEND '(' T_ZAHL ',' T_ZAHL ')' {UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Execution request run_to_end received\n");scorep_oa_mri_set_appl_control(SCOREP_OA_MRI_STATUS_RUNNING_TO_END);}
                 | T_GETSUMMARYDATA {UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Measured data requested\n");scorep_oa_mri_return_summary_data(scorep_oa_connection);}
                 | T_BEGINREQUESTS {UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Standby for requests...\n");scorep_oa_mri_begin_request();scorep_oa_connection_send_string(scorep_oa_connection,"OK\n");}
                 | T_ENDREQUESTS {UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Requests submitted\n");scorep_oa_mri_end_request();scorep_oa_connection_send_string(scorep_oa_connection,"OK\n");}

RequestCommand   : T_REQUEST {UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Measurements are requested\n");} '[' NodeList ']' RequestKind

RequestKind      : T_GLOBAL GlobalRequestList
                 | T_LOCAL RegionSpecifier '=' LocalRequestList {UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Local requests are not supported yet\n");scorep_oa_connection_send_string(scorep_oa_connection,"Local requests are not supported yet\n");}

NodeList         : NodeNrs
                 | '*'                           {UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Node lists are ignored\n");}

NodeNrs          : T_ZAHL                        {UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Node lists are ignored\n");}

GlobalRequestList : GlobalRequest ',' GlobalRequestList
                  | GlobalRequest

GlobalRequest    : T_MPI {scorep_oa_mri_set_mpiprofiling(1);SCOREP_OA_RequestsAddMetricByName(UTILS_CStr_dup("late_send"),SCOREP_METRIC_SOURCE_MPI_PROFILING);SCOREP_OA_RequestsAddMetricByName(UTILS_CStr_dup("late_receive"),SCOREP_METRIC_SOURCE_MPI_PROFILING);}
				 | T_METRIC MetricSpecifier
				 | T_EXECUTION_TIME {SCOREP_OA_RequestsAddMetricByName(UTILS_CStr_dup("execution_time"),SCOREP_METRIC_TIMER);}

MetricSpecifier	 : T_PERISCOPE T_ZAHL {SCOREP_OA_RequestsAddPeriscopeMetric($2);}
				 | T_PAPI T_STRING {SCOREP_OA_RequestsAddMetricByName($2,SCOREP_METRIC_SOURCE_PAPI);}
				 | T_RUSAGE T_STRING {SCOREP_OA_RequestsAddMetricByName($2,SCOREP_METRIC_SOURCE_RUSAGE);}
				 | T_OTHER T_STRING {SCOREP_OA_RequestsAddMetricByName($2,SCOREP_METRIC_OTHER);}

RegionSpecifier  : '(' T_ZAHL ',' Regiontyp ',' T_ZAHL ')' {}
                 | '(' T_ZAHL ',' Regiontyp ',' '*'    ')' {}
                 | '(' '*'    ',' Regiontyp ',' '*'    ')' {}

Regiontyp        : T_USER            {}
                 | T_CALL            {}

LocalRequestList : LocalRequest ',' LocalRequestList
                 | LocalRequest

LocalRequest     : T_EXECUTION_TIME     {}
                 | T_L1_CACHE_MISS      {}



%%

/******************************************************************************/
/* Es folgt der C-Code                                                        */
/******************************************************************************/
#include <string.h>

void yyerror(char *s)
{
  //fprintf (stderr, "Unknown request '(%d,%i)'", yychar, yychar);
  //exit(1);
} /* void yyerror () */










