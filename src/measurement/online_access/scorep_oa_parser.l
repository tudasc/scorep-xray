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
 * Copyright (c) 2009-2011,
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
  * @brief   automatically generated file for the MRI string parser
  *
  */

%{
#include <config.h>
#include "scorep_oa_scanner.h"
#include <UTILS_Debug.h>
#include <SCOREP_ErrorCodes.h>
%}

/* *INDENT-OFF* */

/******************************************************************************/
/* lex.l:                                               Version vom 19.11.1999 */
/* ========================================================================== */
/******************************************************************************/

/* --- Definitionen --- */

%e 2000  /* Knotenanzahl erhoehen */

Ziffer        [0-9]
Return        \n
Tabulator     \t
Leer          [ \t]
Zahl          {Ziffer}+
A             [Aa]
B             [Bb]
C             [Cc]
D             [Dd]
E             [Ee]
F             [Ff]
G             [Gg]
H             [Hh]
I             [Ii]
J             [Jj]
K             [Kk]
L             [Ll]
M             [Mm]
N             [Nn]
O             [Oo]
P             [Pp]
Q             [Qq]
R             [Rr]
S             [Ss]
T             [Tt]
U             [Uu]
V             [Vv]
W             [Ww]
X             [Xx]
Y             [Yy]
Z             [Zz]


%StartConditions ZEILE COMMENT1 COMMENT2

%%
  BEGIN ZEILE;

<ZEILE>"//"                              {BEGIN COMMENT1;}
<ZEILE>"/*"                              {BEGIN COMMENT2;}

<COMMENT1>.                              {;}
<COMMENT1>"\n"                       {BEGIN ZEILE;}

<COMMENT2>"\n"                       {}
<COMMENT2>.                              {;}
<COMMENT2>"*/"                           {BEGIN ZEILE;}

<ZEILE>CONFIG               {return (T_CONFIG);}
<ZEILE>INCLUDE             {return (T_INCLUDE);}
<ZEILE>REQUEST             {return (T_REQUEST);}

<ZEILE>SDDF_BUFFER_FLUSH_RATE {return(T_SDDF_BUFFER_FLUSH_RATE);}

<ZEILE>REGION_SUMMARY {return (T_REGION_SUMMARY);}
<ZEILE>ON                            {return (T_ON);}
<ZEILE>ONLY                      {return (T_ONLY);}
<ZEILE>OFF                         {return (T_OFF);}


<ZEILE>GLOBAL             {return (T_GLOBAL);}
<ZEILE>LOCAL                 {return (T_LOCAL);}

<ZEILE>CALL                    {return (T_CALL);}
<ZEILE>USER                    {return (T_USER);}


<ZEILE>EXECUTION_TIME          {return (T_EXECUTION_TIME);}
<ZEILE>"L1_CACHE_MISS"         {return (T_L1_CACHE_MISS);}

<ZEILE>TERMINATE               {return (T_TERMINATE);}
<ZEILE>RUNTOSTART              {return (T_RUNTOSTART);}
<ZEILE>RUNTOEND                {return (T_RUNTOEND);}
<ZEILE>GETSUMMARYDATA          {return (T_GETSUMMARYDATA);}
<ZEILE>BEGINREQUESTS           {return (T_BEGINREQUESTS);}
<ZEILE>ENDREQUESTS             {return (T_ENDREQUESTS);}

<ZEILE>"MPI"                	            {return (T_MPI);}
<ZEILE>METRIC 								{return (T_METRIC);}
<ZEILE>PERISCOPE							{return (T_PERISCOPE);}
<ZEILE>PAPI									{return (T_PAPI);}
<ZEILE>RUSAGE								{return (T_RUSAGE);}
<ZEILE>OTHER								{return (T_OTHER);}

<ZEILE>\*                                |
<ZEILE>\(                                |
<ZEILE>\)                                |
<ZEILE>\[                                |
<ZEILE>[;,=]                             |
<ZEILE>\]                                {return (yytext[0]);}
<ZEILE>[0-9]+                            {yylval.Zahl = atoi(yytext); return (T_ZAHL);}
<ZEILE>\"[^ ";\t\n]+\"                   {
                                           //yytext string size including trailing 0
                                           int old_string_size= strlen(yytext) + 1;
                                           //string to be returned equal to the yytext but remove ""
                                           int new_string_size= old_string_size -2;
                                           yylval.String = (char *) calloc(new_string_size ,sizeof(char));
                                           int i;
                                           for(i=1;i<old_string_size-2;i++)
                                           		yylval.String[i-1]=yytext[i];
                                           yylval.String[new_string_size-1]= '\0';
                                           return (T_STRING);
                                         }
<ZEILE>[ \t]*                           {;}
<ZEILE>{Return}                              {}

{Return}                                 |
.                                        {
       printf ("PARSING ERROR: (%c)\n", yytext[0] ); return (yytext[0]);}


%%

int yywrap ()
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
scorep_oa_mri_parse( char* buffer)
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Entering %s", __func__ );
    SCOREP_ErrorCode return_status = SCOREP_SUCCESS;

    YY_BUFFER_STATE my_string_buffer;

    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "parse_mri_cmd: %s", buffer );
    my_string_buffer = yy_scan_string( buffer );
    if ( yyparse() != 0 )
    {
        return_status = SCOREP_ERROR_OA_PARSE_MRI;
    }
    yy_delete_buffer( my_string_buffer );

    return return_status;
}

