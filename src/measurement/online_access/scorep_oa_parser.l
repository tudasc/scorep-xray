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

%{
#include <config.h>
#include "scorep_oa_scanner.h"
#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <SCOREP_ErrorCodes.h>
#include <UTILS_CStr.h>

%}

/* *INDENT-OFF* */
/* --- Definitions --- */

%e 2000  /* Raise the number of nodes */

Digit         [0-9]
Return        \n
Tabulator     \t
Delimiter     [ \t]
Number        {Digit}+
Letter        [a-zA-Z]
Word          {Letter}({Letter}|{Digit})+


%StartConditions LINE COMMENT1 COMMENT2

%%
  BEGIN LINE;

<LINE>"//"                   { BEGIN COMMENT1; }
<LINE>"/*"                   { BEGIN COMMENT2; }

<COMMENT1>.                  { ; }
<COMMENT1>"\n"               { BEGIN LINE; }

<COMMENT2>"\n"               { }
<COMMENT2>.                  { ; }
<COMMENT2>"*/"               { BEGIN LINE; }

<LINE>TERMINATE              { return T_TERMINATE; }
<LINE>RUNTOSTART             { return T_RUNTOSTART; }
<LINE>RUNTOEND               { return T_RUNTOEND; }
<LINE>GETSUMMARYDATA         { return T_GETSUMMARYDATA; }
<LINE>BEGINREQUESTS          { return T_BEGINREQUESTS; }
<LINE>ENDREQUESTS            { return T_ENDREQUESTS; }
<LINE>SETNUMITERATIONS       { return T_SETNUMITERATIONS; }

<LINE>REQUEST                { return T_REQUEST; }

<LINE>GLOBAL                 { return T_GLOBAL; }

<LINE>"MPI"                  { return T_MPI; }
<LINE>METRIC                 { return T_METRIC; }
<LINE>EXECUTION_TIME         { return T_EXECUTION_TIME; }

<LINE>PERISCOPE              { return T_PERISCOPE; }
<LINE>PAPI                   { return T_PAPI; }
<LINE>RUSAGE                 { return T_RUSAGE; }
<LINE>PERF                   { return T_PERF; }
<LINE>PLUGIN                 { return T_PLUGIN; }
<LINE>OTHER                  { return T_OTHER; }

<LINE>\*                     |
<LINE>\(                     |
<LINE>\)                     |
<LINE>\[                     |
<LINE>[;,=]                  |
<LINE>\]                     { return yytext[0]; }
<LINE>{Number}               {
                                 yylval.Number = atoi(yytext);
                                 return T_NUMBER;
                             }
<LINE>{Word}                 {
                                 yylval.String = UTILS_CStr_dup(yytext);
                                 return T_STRING;
                             }
<LINE>\"[^"]+\"              {
                                 //yytext string size including trailing 0
                                 int old_string_size = strlen(yytext) + 1;
                                 //string to be returned equal to the yytext but remove ""
                                 int new_string_size = old_string_size - 2;
                                 yylval.String = (char *)calloc( new_string_size, sizeof( char ) );
                                 UTILS_ASSERT( yylval.String );
                                 int i;
                                 for ( i = 1; i < old_string_size - 2; i++ ) {
                                     yylval.String[ i - 1 ] = yytext[ i ];
                                 }
                                 yylval.String[ new_string_size - 1 ] = '\0';
                                 return T_QSTRING;
                             }
<LINE>{Delimiter}*           {;}
<LINE>{Return}               {}

{Return}                     |
.                            {
                                 UTILS_ERROR( SCOREP_ERROR_OA_PARSE_MRI, "PARSING ERROR: (%c)\n", yytext[ 0 ] );
                                 return yytext[ 0 ];
                             }


%%

int
yywrap()
{
    return 1;
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

SCOREP_ErrorCode
scorep_oa_mri_parse( char* buffer )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );
    SCOREP_ErrorCode return_status = SCOREP_SUCCESS;

    YY_BUFFER_STATE my_string_buffer;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "parse_mri_cmd: %s", buffer );
    my_string_buffer = yy_scan_string( buffer );
    if ( yyparse() != 0 )
    {
        return_status = SCOREP_ERROR_OA_PARSE_MRI;
    }
    yy_delete_buffer( my_string_buffer );

    return return_status;
}
