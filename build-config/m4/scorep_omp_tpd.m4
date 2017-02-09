dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013-2017,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_omp_tpd.m4


# SCOREP_OPENMP_TPD
# -----------------
# Checks if C compiler supports '__attribute__((aligned (16)))'. This
# is necessary to reliably communicate via an instrumented 'int64_t
# pomp_tpd' variable between Fortran and C. If this communication is
# not possible, disable the OpenMP pomp_tpd implementation using the
# automake conditional 'HAVE_SCOREP_OMP_TPD' and the autoconf
# substitution SCOREP_OMP_TPD.
AC_DEFUN([SCOREP_OPENMP_TPD],[
AC_REQUIRE([AX_COMPILER_VENDOR])dnl

AC_LANG_PUSH([C])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[]],
    [[int __attribute__((aligned (16))) tpd;]])],
    [scorep_has_alignment_attribute="yes"],
    [scorep_has_alignment_attribute="no"]
)
AC_LANG_POP([C])

AS_IF([test "x${ac_scorep_platform}" = xk ||
       test "x${ac_scorep_platform}" = xfx10 ||
       test "x${ac_scorep_platform}" = xfx100],
    [# Can't get OPENMP_TPD working reliably on Fujitsu. Compiler options
     # -Xg -noansi needed for Pthreads, but they break OPENMP_TPD.
     # Using OpenMP_ANCESTRY as alternative.
     scorep_has_alignment_attribute="no"])

AS_CASE([${ac_scorep_platform}],
    [bg*], [# Switch of tpd on BlueGene systems because:
            # 1. On Juqueen with gfortan all OpenMP tpd installchecks fail,
            #    ancestry works though.
            # 2. On Juqueen with bgxlc V12.1 declaration and definition of
            #    pomp_tpd fails with a 'Identifier pomp_tpd has already been 
            #    defined' error. Reproducer:
            #      extern int64_t __attribute__((aligned (16))) pomp_tpd;
            #      _Pragma( "omp threadprivate( pomp_tpd )" )
            #      int64_t __attribute__((aligned (16))) pomp_tpd;
            #    Newer xlc (e.g. V13.1.4) work as expected.
            scorep_has_alignment_attribute="no"])

AS_IF([test "x${scorep_has_alignment_attribute}" = "xyes"],
    [AC_DEFINE([FORTRAN_ALIGNED],
         [__attribute__((aligned (16)))],
         [Makes C variable alignment consistent with Fortran])
     AC_SUBST([SCOREP_OMP_TPD], [1])
     AC_DEFINE_UNQUOTED([HAVE_SCOREP_OMP_TPD], [1],
         [Define to 1 if OpenMP tpd functionality is available.])
    ],
    [AC_SUBST([SCOREP_OMP_TPD], [0])])
AFS_AM_CONDITIONAL([HAVE_SCOREP_OMP_TPD], [test "x${scorep_has_alignment_attribute}" = "xyes"], [false])

AC_MSG_CHECKING([for alignment attribute])
AC_MSG_RESULT([${scorep_has_alignment_attribute}])
AFS_SUMMARY([OpenMP pomp_tpd], [${scorep_has_alignment_attribute}])
])
