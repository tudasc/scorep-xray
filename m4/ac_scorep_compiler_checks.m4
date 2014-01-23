## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/ac_scorep_compiler_checks.m4

dnl ------------------------------------------------------------------

# used for pomp_tpd in scorep and opari2
AC_DEFUN([AC_SCOREP_ATTRIBUTE_ALIGNMENT],[
AC_LANG_PUSH([C])
AC_MSG_CHECKING([for alignment attribute])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[int __attribute__((aligned (16))) tpd;]])],
                [ac_scorep_has_alignment_attribute="yes"],
                [ac_scorep_has_alignment_attribute="no"]
)

if test "x${ac_scorep_has_alignment_attribute}" = "xyes"; then
  AC_DEFINE([FORTRAN_ALIGNED],[__attribute__((aligned (16)))],[Makes C variable alignment consistent with Fortran])
else
  AC_DEFINE([FORTRAN_ALIGNED],[],[Alignment attribute not supported])
fi

AC_MSG_RESULT([$ac_scorep_has_alignment_attribute])
AC_LANG_POP([C])
])

dnl ------------------------------------------------------------------

AC_DEFUN([AC_SCOREP_COMPILER_CHECKS],[
AC_LANG_PUSH([C])
AX_COMPILER_VENDOR
AC_LANG_POP([C])

# Disable default OpenMP support for the Cray compilers 
AS_IF([test "x${ax_cv_c_compiler_vendor}" = xcray],
    [CC="${CC} -hnoomp -O2"
     CXX="${CXX} -hnoomp -O2"
     F77="${F77} -hnoomp -O2"
     FC="${FC} -hnoomp -O2"])dnl

AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [AFS_AM_CONDITIONAL([SCOREP_COMPILER_INTEL], [test 1 -eq 1], [false])],
    [sun],      [AFS_AM_CONDITIONAL([SCOREP_COMPILER_SUN],   [test 1 -eq 1], [false])],
    [ibm],      [AFS_AM_CONDITIONAL([SCOREP_COMPILER_IBM],   [test 1 -eq 1], [false])],
    [portland], [AFS_AM_CONDITIONAL([SCOREP_COMPILER_PGI],   [test 1 -eq 1], [false])],
    [gnu],      [AFS_AM_CONDITIONAL([SCOREP_COMPILER_GNU],   [test 1 -eq 1], [false])],
    [cray],     [AFS_AM_CONDITIONAL([SCOREP_COMPILER_CRAY],  [test 1 -eq 1], [false])],
    [unknown],  [AC_MSG_WARN([Could not determine compiler vendor. Score-P might not function properly.])],
    [AC_MSG_WARN([Compiler vendor '${ax_cv_c_compiler_vendor}' unsupported. Score-P might not function properly.])])dnl

afs_compiler_intel=0
afs_compiler_sun=0
afs_compiler_ibm=0
afs_compiler_portland=0
afs_compiler_gnu=0
afs_compiler_cray=0
AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [afs_compiler_intel=1],
    [sun],      [afs_compiler_sun=1],
    [ibm],      [afs_compiler_ibm=1],
    [portland], [afs_compiler_portland=1],
    [gnu],      [afs_compiler_gnu=1],
    [cray],     [afs_compiler_cray=1],
    [])
AC_SUBST([SCOREP_COMPILER_INTEL], [${afs_compiler_intel}])dnl
AC_SUBST([SCOREP_COMPILER_SUN],   [${afs_compiler_sun}])dnl
AC_SUBST([SCOREP_COMPILER_IBM],   [${afs_compiler_ibm}])dnl
AC_SUBST([SCOREP_COMPILER_PGI],   [${afs_compiler_portland}])dnl
AC_SUBST([SCOREP_COMPILER_GNU],   [${afs_compiler_gnu}])dnl
AC_SUBST([SCOREP_COMPILER_CRAY],  [${afs_compiler_cray}])dnl
])dnl

dnl ------------------------------------------------------------------

AC_DEFUN([AC_SCOREP_COMPILER_INSTRUMENTATION_FLAGS],[
AC_REQUIRE([AX_COMPILER_VENDOR])dnl

dnl Is there a use case for extra-instrumentation-flags?
AC_ARG_WITH([extra-instrumentation-flags],
            [AS_HELP_STRING([--with-extra-instrumentation-flags=flags],
                            [Add additional instrumentation flags.])],
            [ac_scorep_with_extra_instrumentation_cppflags=$withval],
            [ac_scorep_with_extra_instrumentation_cppflags=""])dnl

AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [ac_scorep_compiler_instrumentation_cppflags="-tcollect"],
    [sun],      [ac_scorep_compiler_instrumentation_cppflags="-O -Qoption f90comp -phat"],
    [ibm],      [ac_scorep_compiler_instrumentation_cppflags="-qdebug=function_trace"],
    [portland], [ac_scorep_compiler_instrumentation_cppflags="-Mprof=func"],
    [gnu],      [ac_scorep_compiler_instrumentation_cppflags="-finstrument-functions"],
    [cray],     [ac_scorep_compiler_instrumentation_cppflags="-hfunc_trace"
                 ac_scorep_compiler_instrumentation_ldflags="-Wl,-u,__pat_tp_func_entry,-u,__pat_tp_func_return"],
    [])dnl

AS_IF([test "x${ac_scorep_with_extra_instrumentation_cppflags}" != x || \
       test "x${ac_scorep_compiler_instrumentation_cppflags}" != x],
    [AC_MSG_NOTICE([using compiler instrumentation cppflags: ${ac_scorep_compiler_instrumentation_cppflags} ${ac_scorep_with_extra_instrumentation_cppflags}])
     AFS_SUMMARY_VERBOSE([instrumentation cppflags], [${ac_scorep_compiler_instrumentation_cppflags} ${ac_scorep_with_extra_instrumentation_cppflags}])
     AS_IF([test "x${ac_scorep_compiler_instrumentation_ldflags}" != x],
         [AC_MSG_NOTICE([using compiler instrumentation ldflags: ${ac_scorep_compiler_instrumentation_ldflags}])
          AFS_SUMMARY_VERBOSE([instrumentation ldflags], [${ac_scorep_compiler_instrumentation_ldflags}])
         ])
    ])

AC_SUBST([COMPILER_INSTRUMENTATION_CPPFLAGS], 
    ["${ac_scorep_compiler_instrumentation_cppflags} ${ac_scorep_with_extra_instrumentation_cppflags}"])
AC_SUBST([COMPILER_INSTRUMENTATION_LDFLAGS], 
    ["${ac_scorep_compiler_instrumentation_ldflags}"])
])

dnl ------------------------------------------------------------------

AC_DEFUN([SCOREP_FORTRAN_MANGLED], [
AC_REQUIRE([AX_COMPILER_VENDOR])dnl
AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [mangling=" [##] _"],
    [sun],      [mangling=" [##] _"],
    [ibm],      [mangling=],
    [portland], [mangling=" [##] _"],
    [gnu],      [mangling=" [##] _"],
    [cray],     [mangling=" [##] _"],
    [])dnl
AC_DEFINE_UNQUOTED([FORTRAN_MANGLED(var)], 
    [var${mangling}], 
    [Name of var after mangled by the Fortran compiler.])dnl
])
