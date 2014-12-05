dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2014
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_check_preincludes.m4

AC_DEFUN([SCOREP_CHECK_PREINCLUDES], [
AC_REQUIRE([AC_SCOREP_OPENMP])

scorep_no_preinclude_flag=""
AS_IF([test "x${ax_cv_c_compiler_vendor}" = xportland],
[
    AS_IF([${CXX} ${CXXFLAGS} ${OPENMP_CXXFLAGS} -show 2>&1 | GREP_OPTIONS= grep -q preinclude],
    [
        AS_IF([${CXX} ${CXXFLAGS} ${OPENMP_CXXFLAGS} -show --no_preincludes >/dev/null 2>&1],
        [scorep_no_preinclude_flag="--no_preincludes"])
    ])
])

AC_SUBST([SCOREP_NO_PREINCLUDE_FLAG], [${scorep_no_preinclude_flag}])
])
