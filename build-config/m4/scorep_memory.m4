dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2016-2017,
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
AC_REQUIRE([SCOREP_CHECK_THREAD_LOCAL_STORAGE])dnl
AC_REQUIRE([SCOREP_LIBRARY_WRAPPING])dnl

AFS_SUMMARY_PUSH

scorep_memory_support="yes"
scorep_memory_summary_reason=

# check linktime wrapping support
AM_COND_IF([HAVE_LIBWRAP_LINKTIME_SUPPORT],
           [],
           [scorep_memory_support="no"
            scorep_memory_summary_reason+=", missing linktime library wrapping support"])

# check result of TLS
AS_IF([test "x${scorep_memory_support}" = "xyes"],
    [AM_COND_IF([HAVE_THREAD_LOCAL_STORAGE],
        [],
        [scorep_memory_support="no"
         scorep_memory_summary_reason+=", missing TLS support"])])

# check malloc.h header
AS_IF([test "x${scorep_memory_support}" = "xyes"],
    [AC_CHECK_HEADER(["malloc.h"],
        [],
        [scorep_memory_support="no"
         scorep_memory_summary_reason+=", missing malloc.h header"])])

# setting output variables/defines
AC_SCOREP_COND_HAVE([MEMORY_SUPPORT],
                    [test "x${scorep_memory_support}" = "xyes"],
                    [Define if memory tracking is supported.])

AFS_SUMMARY_POP([Memory tracking support], [${scorep_memory_support}${scorep_memory_summary_reason}])
AS_UNSET([scorep_memory_summary_reason])
])
