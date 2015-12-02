## -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2015,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file scorep_compiler_constructor.m4

AC_DEFUN([SCOREP_COMPILER_CONSTRUCTOR], [
AC_REQUIRE([AX_COMPILER_VENDOR])dnl

scorep_compiler_constructor_mode=
AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [scorep_compiler_constructor_mode=attribute
                 scorep_compiler_constructor_cflags=""],
    [sun],      [scorep_compiler_constructor_mode=pragma
                 scorep_compiler_constructor_cflags=""],
    [ibm],      [scorep_compiler_constructor_mode=attribute
                 scorep_compiler_constructor_cflags=""],
    [portland], [scorep_compiler_constructor_mode=pragma
                 scorep_compiler_constructor_cflags=""],
    [gnu],      [scorep_compiler_constructor_mode=attribute
                 scorep_compiler_constructor_cflags=""],
    [cray],     [scorep_compiler_constructor_mode=attribute
                 scorep_compiler_constructor_cflags="-hgnu"],
    [])dnl

## define possible values for SCOREP_COMPILER_CONSTRUCTOR_MODE
AC_DEFINE([SCOREP_COMPILER_CONSTRUCTOR_MODE_ATTRIBUTE], [0],
          [Attribute mode for compiler constructor in C.])
AC_DEFINE([SCOREP_COMPILER_CONSTRUCTOR_MODE_PRAGMA], [1],
          [Pragma mode for compiler constructor in C.])

AS_CASE([${scorep_compiler_constructor_mode}],
    [attribute], [
        AC_MSG_CHECKING([whether compiler accepts constructor attribute])
        scorep_compiler_constructor_safe_CFLAGS=$CFLAGS
        CFLAGS="$CFLAGS $scorep_compiler_constructor_cflags"
        AC_LANG_PUSH([C])
        AC_LINK_IFELSE([
            AC_LANG_PROGRAM(
[[void
__attribute__((constructor))
checkconstructor(void);

void
checkconstructor(void)
{
}]],
[])
        ], [
            AC_MSG_RESULT([yes])
            AC_DEFINE([SCOREP_COMPILER_CONSTRUCTOR_MODE],
                      [SCOREP_COMPILER_CONSTRUCTOR_MODE_ATTRIBUTE],
                      [Specifies how the compiler supports a constructor in C.])
            scorep_compiler_constructor_summary="yes, using attribute syntax"
        ], [
            AC_MSG_RESULT([no])
            scorep_compiler_constructor_summary="no, compiler rejects constructor attribute"
            scorep_compiler_constructor_mode=
        ])
        AC_LANG_POP([C])
        CFLAGS=$scorep_compiler_constructor_safe_CFLAGS
    ],
    [pragma], [
        AC_MSG_CHECKING([whether compiler accepts @%:@pragma init syntax])
        scorep_compiler_constructor_safe_CFLAGS=$CFLAGS
        CFLAGS="$CFLAGS $scorep_compiler_constructor_cflags"
        AC_LANG_PUSH([C])
        AC_LINK_IFELSE([
            AC_LANG_PROGRAM(
[[void
checkconstructor(void);

#pragma init(checkconstructor)

void
checkconstructor(void)
{
}]],
[])
        ], [
            AC_MSG_RESULT([yes])
            AC_DEFINE([SCOREP_COMPILER_CONSTRUCTOR_MODE],
                      [SCOREP_COMPILER_CONSTRUCTOR_MODE_PRAGMA],
                      [Specifies how the compiler supports a constructor in C.])
            scorep_compiler_constructor_summary="yes, using @%:@pragma init syntax"
        ], [
            AC_MSG_RESULT([no])
            scorep_compiler_constructor_summary="no, compiler rejects @%:@pragma init syntax"
            scorep_compiler_constructor_mode=
        ])
        AC_LANG_POP([C])
        CFLAGS=$scorep_compiler_constructor_safe_CFLAGS
    ],
    [scorep_compiler_constructor_summary="no, compiler vendor '${ax_cv_c_compiler_vendor}' not supported"])dnl

AC_SUBST([SCOREP_COMPILER_CONSTRUCTOR_CFLAGS],
         ["${scorep_compiler_constructor_cflags}"])
AC_SCOREP_COND_HAVE([COMPILER_CONSTRUCTOR_SUPPORT],
                    [test "x${scorep_compiler_constructor_mode}" != x],
                    [Compiler constructor support])

AFS_SUMMARY([compiler constructor],
            [${scorep_compiler_constructor_summary}])
])
