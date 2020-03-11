## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2020,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_kokkos.m4

dnl ----------------------------------------------------------------------------

dnl Kokkos needs a shared build of Score-P. No external dependencies.

AC_DEFUN([SCOREP_KOKKOS], [

AFS_SUMMARY_PUSH

scorep_have_kokkos=""
scorep_have_kokkos_reason=""

AS_IF([test "x$enable_shared" = xyes],
      [scorep_have_kokkos="yes"],
      [scorep_have_kokkos="no",
       scorep_have_kokkos_reason=", configure with --enable-shared"])

AC_SCOREP_COND_HAVE([KOKKOS_SUPPORT],
                    [test "x${scorep_have_kokkos}" = "xyes"],
                    [Defined if Kokkos is supported.])

AFS_SUMMARY_POP([Kokkos support], [${scorep_have_kokkos}${scorep_have_kokkos_reason}])

AS_UNSET([scorep_have_kokkos_reason])

])
