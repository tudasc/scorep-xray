/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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
 * @file        yacc.l
 * @maintainer  Yury Olenyik <oleynik@in.tum.de>
 *
 * @brief   automatically generated file for the MRI string parser
 *
 * @status alpha
 *
 */

/* *INDENT-OFF* */
/******************************************************************************/
/* yacc.l:                                 Version vom 19.11.1999 */
/* ========================================================================== */
/******************************************************************************/
%{

#include "scorep_oa_mri_control.h"
#include "scorep_oa_connection.h"
#include "SCOREP_OA_Init.h"
#include "SCOREP_Utils.h"
#include <stdio.h>

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

%start Command

%%

/******************************************************************************/
/* Es folgten die yacc-Regeln.                                                */
/******************************************************************************/

Command          : ConfigCommand  ';' {scorep_oa_connection_send_string(connection,"OK\n");} 
                 | RequestCommand ';' {scorep_oa_connection_send_string(connection,"OK\n");}
                 | ControlCommand ';'  


ControlCommand   : T_TERMINATE {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Termination request received\n");scorep_oa_mri_set_appl_control(SCOREP_OA_MRI_EXEC_REQUEST_TERMINATE,0,0);scorep_oa_connection_send_string(connection,"OK\n");}
                 | T_RUNTOSTART '(' T_ZAHL ',' T_ZAHL ')' {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Execution request run_to_beginning received\n");scorep_oa_mri_set_appl_control(SCOREP_OA_MRI_STATUS_RUNNING_TO_BEGINNING,$3,$5);}
                 | T_RUNTOEND '(' T_ZAHL ',' T_ZAHL ')' {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Execution request run_to_end received\n");scorep_oa_mri_set_appl_control(SCOREP_OA_MRI_STATUS_RUNNING_TO_END,$3,$5);}
                 | T_GETSUMMARYDATA {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Measured data requested\n");scorep_oa_mri_return_summary_data(connection);}
                 | T_BEGINREQUESTS {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Standby for requests...\n");scorep_oa_connection_send_string(connection,"OK\n");}
                 | T_ENDREQUESTS {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Requests submitted\n");scorep_oa_connection_send_string(connection,"OK\n");}

ConfigCommand    : T_CONFIG ConfigSpecification

ConfigSpecification : T_SDDF_BUFFER_FLUSH_RATE  '=' T_ZAHL   {
                                                        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Not supported and never be\n");}
                    | T_REGION_SUMMARY          '=' SummaryStatus 

SummaryStatus : T_OFF   {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Not supported and never be\n");}
              | T_ON    {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Not supported and never be\n");} 
              | T_ONLY  {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Not supported and never be\n");} 

RequestCommand   : T_REQUEST {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Measurements are requested\n");} '[' NodeList ']' RequestKind

RequestKind      : T_GLOBAL GlobalRequestList 
                 | T_LOCAL RegionSpecifier '=' LocalRequestList {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Local requests are not supported yet\n");scorep_oa_connection_send_string(connection,"Local requests are not supported yet\n");}

NodeList         : NodeNrs                       
                 | '*'                           {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Node lists are ignored\n");}

NodeNrs          : T_ZAHL                        {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Node lists are ignored\n");}

GlobalRequestList : GlobalRequest ',' GlobalRequestList
                  | GlobalRequest


GlobalRequest    : T_MPI {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Global MPI request was received\n");scorep_oa_mri_set_profiling(1);scorep_oa_mri_set_mpiprofiling(1);}
				 | T_EXECUTION_TIME {SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,"Global Execution time request was received\n");scorep_oa_mri_set_profiling(1);}
 

RegionSpecifier  : '(' T_ZAHL ',' Regiontyp ',' T_ZAHL ')' {scorep_oa_mri_setphase($2,$6);}
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
#include "lex.yy.c" 




void yyerror(char *s)
{
  fprintf (stderr, "Unknown request '(%c,%i)'", yychar, yychar);
  //exit(1);
} /* void yyerror () */ 








void LOKAL_set_RegionPos  (int FileId, int RegionFirstLineNr)
{
  
}  /* void LOKAL_set_RegionPosAndTID () */









