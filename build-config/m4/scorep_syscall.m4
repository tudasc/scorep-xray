## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2013,
## RWTH Aachen, Germany
##
## Copyright (c) 2009-2013,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2015, 2020,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2013,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013, 2015,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2013,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_syscall.m4

## SCOREP_CHECK_SYSCALL_SUPPORT
## ----------------------------
##
## Basic check if syscall is available
##
AC_DEFUN([SCOREP_CHECK_SYSCALL_SUPPORT], [

scorep_have_syscall_support="yes"
AC_CHECK_HEADERS([unistd.h sys/syscall.h], [], [scorep_have_syscall_support="no"])
AS_IF([test "x${scorep_have_syscall_support}" = "xyes"],
      [AC_CHECK_DECLS([syscall],[],[],[[
#include <unistd.h>
#include <sys/syscall.h>]])])
AS_IF([test "x${scorep_have_syscall_support}" = "xyes"],
      [AC_MSG_CHECKING([for syscall])
       AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <unistd.h>
#include <sys/syscall.h>]], [return syscall(0)])],
                      [AC_MSG_RESULT([yes])],
                      [AC_MSG_RESULT([no])
                       scorep_have_syscall_support="no"])])

])

## SCOREP_CHECK_SYSCALL( SYSCALL, [ACTION-IF-AVAILABLE], [ACTION-IF-NOT-AVAILABLE] )
## ---------------------------------------------------------------------------------
##
## Check if a specific syscall number is available
## Performs a DECL_CHECKS on the SYSCALL, i.e., it results in a
## HAVE_DECL_<SYSCALL> AC_DEFINE.
##
AC_DEFUN([SCOREP_CHECK_SYSCALL], [
AC_REQUIRE([SCOREP_CHECK_SYSCALL_SUPPORT])
AS_IF([test "x${scorep_have_syscall_support}" = "xyes"],
      [AC_CHECK_DECLS([$1],
                      [: $1 found
                       $2],
                      [: $1 not found
                       $3], [[
#include <unistd.h>
#include <sys/syscall.h>]])
      ], [: no syscall support, no $1
         $3])
])
