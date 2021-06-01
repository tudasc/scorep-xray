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
## Copyright (c) 2009-2014, 2019, 2021,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2014, 2021,
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

AC_DEFUN([AC_SCOREP_COMPILER_INSTRUMENTATION], [
AC_REQUIRE([SCOREP_COMPUTENODE_CC])dnl
AC_REQUIRE([SCOREP_COMPILER_INSTRUMENTATION_FLAGS])dnl
AC_REQUIRE([SCOREP_LIBBFD])dnl

have_compiler_instrumentation=yes
AS_CASE([${ax_cv_c_compiler_vendor%/*}],
    [intel],    [],
    [ibm],      [],
    [portland], [],
    [gnu],      [],
    [cray],     [],
    [fujitsu],  [],
    [clang],    [],
    [have_compiler_instrumentation=no
     compiler_instrumentation_result="no, compiler vendor '${ax_cv_c_compiler_vendor}' not supported."])dnl


AS_IF([test "x${have_compiler_instrumentation}" = xyes],
    [AS_IF([test "x${scorep_compiler_instrumentation_needs_symbol_table}" = xyes],
         [pwd_save=`pwd`
          cd ${srcdir}/../
          scorep_abs_toplevel_srcdir=`pwd`
          cd ${pwd_save}
          AC_DEFINE_UNQUOTED([SCOREP_ABS_TOPLEVEL_SRCDIR],
              ["${scorep_abs_toplevel_srcdir}"],
              [Toplevel src directory])
          compiler_instrumentation_result="yes, using libbfd"
         ],
         [# compilers that don't need the symbol table
          AS_IF([test "x${scorep_compiler_gnu_with_plugin}" = "xyes"],
              [compiler_instrumentation_result="yes, using GCC plug-in with support for compile-time filtering"],
              [compiler_instrumentation_result="yes"])
         ])
    ])
AFS_SUMMARY([compiler instrumentation], [${compiler_instrumentation_result}])

AC_SCOREP_COND_HAVE([COMPILER_INSTRUMENTATION],
    [test "x${have_compiler_instrumentation}" = xyes],
    [Defined if compiler instrumentation is available.])

AC_SCOREP_COND_HAVE([COMPILER_INSTRUMENTATION_NEEDS_SYMBOL_TABLE],
    [test "x${have_compiler_instrumentation}" = xyes &&
     test "x${scorep_compiler_instrumentation_needs_symbol_table}" = "xyes"],
    [Define if the compiler instrumentation needs the symbol table.])

])

dnl ----------------------------------------------------------------------------
