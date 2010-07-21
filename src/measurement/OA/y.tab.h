/* *INDENT-OFF* */
/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
/* Put the tokens into the symbol table, so that GDB and other debuggers
   know about them.  */
enum yytokentype
{
    T_CALL                   = 258,
    T_CONFIG                 = 259,
    T_EXECUTION_TIME         = 260,
    T_GLOBAL                 = 261,
    T_INCLUDE                = 262,
    T_L1_CACHE_MISS          = 263,
    T_OFF                    = 264,
    T_ON                     = 265,
    T_ONLY                   = 266,
    T_REGION_SUMMARY         = 267,
    T_REQUEST                = 268,
    T_STRING                 = 269,
    T_USER                   = 270,
    T_ZAHL                   = 271,
    T_TERMINATE              = 272,
    T_RUNTOSTART             = 273,
    T_RUNTOEND               = 274,
    T_GETSUMMARYDATA         = 275,
    T_BEGINREQUESTS          = 276,
    T_ENDREQUESTS            = 277,
    T_SDDF_BUFFER_FLUSH_RATE = 278,
    T_MPI                    = 279,
    T_LOCAL                  = 280
};
#endif
/* Tokens.  */
#define T_CALL 258
#define T_CONFIG 259
#define T_EXECUTION_TIME 260
#define T_GLOBAL 261
#define T_INCLUDE 262
#define T_L1_CACHE_MISS 263
#define T_OFF 264
#define T_ON 265
#define T_ONLY 266
#define T_REGION_SUMMARY 267
#define T_REQUEST 268
#define T_STRING 269
#define T_USER 270
#define T_ZAHL 271
#define T_TERMINATE 272
#define T_RUNTOSTART 273
#define T_RUNTOEND 274
#define T_GETSUMMARYDATA 275
#define T_BEGINREQUESTS 276
#define T_ENDREQUESTS 277
#define T_SDDF_BUFFER_FLUSH_RATE 278
#define T_MPI 279
#define T_LOCAL 280




#if !defined ( YYSTYPE ) && !defined ( YYSTYPE_IS_DECLARED )
#line 45 "yacc.l"
typedef union YYSTYPE
{
    int   Zahl;
    char* String;
} YYSTYPE;
/* Line 1447 of yacc.c.  */
#line 93 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;
