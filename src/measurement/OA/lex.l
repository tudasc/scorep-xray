/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 
 /**
  * @file        lex.l
  * @maintainer  Yury Olenyik <oleynik@in.tum.de>
  *
  * @brief   automatically generated file for the MRI string parser
  *
  * @status alpha
  *
  */
 
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

<ZEILE>\*                                |
<ZEILE>\(                                |
<ZEILE>\)                                |
<ZEILE>\[                                |
<ZEILE>[;,=]                             |
<ZEILE>\]                                {return (yytext[0]);}
<ZEILE>[0-9]+                            {yylval.Zahl = atoi(yytext); return (T_ZAHL);}
<ZEILE>\"[^ ";\t\n]+\"                   { 
                                           //yytext[yyleng-1] = '\0';
                                           //yylval.String = &(yytext[1]);
                                           return (T_STRING);
                                         }
<ZEILE>[ \t]*                           {;}
<ZEILE>{Return}                              {}

{Return}                                 |
.                                        {
       printf ("Fehler: (%c)\n", yytext[0] ); return (yytext[0]);}


%%

int yywrap ()
{
  
  return 1;
}




