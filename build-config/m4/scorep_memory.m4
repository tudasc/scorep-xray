dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2016,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_memory.m4


# --------------------------------------------------------------------

# SCOREP_MEMORY_SUPPORT
# --------------
AC_DEFUN([SCOREP_MEMORY_SUPPORT], [
AC_REQUIRE([SCOREP_THREAD_LOCAL_STORAGE])
# get compiler vendor, PGI and CRAY are not supported yet
AC_REQUIRE([AX_COMPILER_VENDOR])

AC_REQUIRE([AFS_GNU_LINKER])

AFS_SUMMARY_PUSH

# we require the GNU linker, thus start with this as value
scorep_have_memory_support="${afs_have_gnu_linker}"

# check results of TLS
AS_IF([test "x${scorep_have_memory_support}" = "xyes"], [
    scorep_have_memory_support=${scorep_have_thread_local_storage}])

AS_IF([test "x${scorep_have_memory_support}" = "xyes"], [
    AC_CHECK_HEADER(["malloc.h"], [], [scorep_have_memory_support="no"])
])

# setting output variables/defines
AC_SCOREP_COND_HAVE([MEMORY_SUPPORT],
                    [test "x${scorep_have_memory_support}" = "xyes"],
                    [Define if memory tracking is supported.])

AFS_SUMMARY_POP([Memory tracking support], [${scorep_have_memory_support}])
])
