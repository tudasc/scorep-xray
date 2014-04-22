dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013-2014,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_function_predef.m4

# SCOREP_CXX_FUNCTION_PREDEFINE
# -----------------------------
# Find the predefined macro that gets us the function name. Most C++
# compilers know __func__, but Fujitsu doesn't.  If a valid macro is
# found, create the preprocessor define
# HAVE_SCOREP_CXX_FUNCTION_PREDEF and the autoconf substitution
# SCOREP_CXX_FUNCTION_PREDEF.
AC_DEFUN([SCOREP_CXX_FUNCTION_PREDEFINE], [

AC_LANG_PUSH([C++])
for scorep_cxx_function_predef in __func__ __FUNCTION__; do
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [[
        #include <stdio.h>
        ]],
        [[
            printf("In function %s\n", ${scorep_cxx_function_predef});
        ]])],
        [scorep_have_cxx_function_predef=yes],
        [scorep_have_cxx_function_predef=no])
    AS_IF([test "x${scorep_have_cxx_function_predef}" = xyes], [break])
done
AC_LANG_POP([C++])

AC_MSG_CHECKING([for C++ __func__ equivalent])
AS_IF([test "x${scorep_have_cxx_function_predef}" = xyes],
    [AC_MSG_RESULT([${scorep_cxx_function_predef}])
     AC_DEFINE([HAVE_SCOREP_CXX_FUNCTION_PREDEF])
     AC_SUBST([SCOREP_CXX_FUNCTION_PREDEF],[${scorep_cxx_function_predef}])],
    [AC_MSG_RESULT([none])])
])
