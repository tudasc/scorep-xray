dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013, 2016-2017,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2019,
dnl Technische Universitaet Dresden, Germany
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
AC_REQUIRE([AC_SCOREP_OPENMP])dnl
AC_REQUIRE([AX_COMPILER_VENDOR])dnl

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
    [scorep_have_omp_ancestry=1],
    [scorep_have_omp_ancestry=0])

# PGI's implementation of omp_get_ancestor_thread_num returns
# 'Error: omp_get_ancestor_thread_num: not implemented'.
AS_IF([test "x${ax_cv_c_compiler_vendor%/*}" = xportland &&
       test ${afs_compiler_version_major} -lt 18],
    [scorep_have_omp_ancestry=1])

AS_IF([test 1 -eq ${scorep_have_omp_ancestry}], 
    [AC_MSG_RESULT([yes])
     AFS_SUMMARY([OpenMP ancestry], [yes])], 
    [AC_MSG_RESULT([no])
     AFS_SUMMARY([OpenMP ancestry], [no])])

AFS_AM_CONDITIONAL([HAVE_SCOREP_OMP_ANCESTRY],
    [test 1 -eq ${scorep_have_omp_ancestry}],
    [false])
AC_SUBST([SCOREP_OMP_ANCESTRY], [${scorep_have_omp_ancestry}])
AC_DEFINE_UNQUOTED([HAVE_SCOREP_OMP_ANCESTRY],
    [${scorep_have_omp_ancestry}],
    [Define to 1 if OpenMP ancestry functionality is available.])

CFLAGS=${scorep_cflags_save}
AC_LANG_POP([C])
])
