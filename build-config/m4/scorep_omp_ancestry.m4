dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_omp_ancestry.m4


# SCOREP_OPENMP_ANCESTRY
# ----------------------
# Define automake conditional HAVE_SCOREP_OMP_ANCESTRY and
# substitution SCOREP_OMP_ANCESTRY if the OpenMP implementation
# supports the OpenMP 3.0 ancestry runtime library routines.
AC_DEFUN([SCOREP_OPENMP_ANCESTRY], [
AC_REQUIRE([AC_SCOREP_OPENMP])

AC_LANG_PUSH([C])
scorep_cflags_save=${CFLAGS}
CFLAGS="${OPENMP_CFLAGS}"

AC_MSG_CHECKING([for OpenMP ancestry runtime library routines])
AC_LINK_IFELSE([AC_LANG_PROGRAM(
[[
#include <omp.h>
]],
[[
    omp_get_max_active_levels();
    omp_get_level();
    omp_get_ancestor_thread_num(0);
    omp_get_team_size(0);
    omp_get_active_level();
]])],
    [scorep_have_omp_ancestry=1
     AC_MSG_RESULT([yes])
     AFS_SUMMARY([OpenMP ancestry], [yes])],
    [scorep_have_omp_ancestry=0
     AC_MSG_RESULT([no])
     AFS_SUMMARY([OpenMP ancestry], [no])])

AFS_AM_CONDITIONAL([HAVE_SCOREP_OMP_ANCESTRY],
    [test 1 -eq ${scorep_have_omp_ancestry}],
    [false])
AC_SUBST([SCOREP_OMP_ANCESTRY], [${scorep_have_omp_ancestry}])

CFLAGS=${scorep_cflags_save}
AC_LANG_POP([C])
])
