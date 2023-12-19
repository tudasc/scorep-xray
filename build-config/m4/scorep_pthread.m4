dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013-2014, 2022-2024,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2013, 2017, 2023,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_pthread.m4


# --------------------------------------------------------------------

# SCOREP_CHECK_PTHREAD
# --------------
# Wrapper around AX_PTHREAD to provide AC_REQUIRE([SCOREP_CHECK_PTHREAD])
# functionality.
AC_DEFUN([SCOREP_CHECK_PTHREAD], [

dnl advertise the PTHREAD_CFLAGS and PTHREAD_LIBS environment variables
AC_ARG_VAR([PTHREAD_CFLAGS], [CFLAGS used to compile Pthread programs])
AC_ARG_VAR([PTHREAD_LIBS], [LIBS used to link Pthread programs])

AX_PTHREAD([scorep_have_pthread=1], [scorep_have_pthread=0])
])dnl SCOREP_CHECK_PTHREAD

# --------------------------------------------------------------------

# SCOREP_PTHREAD
# --------------
# Checks Pthread recording requirements
AC_DEFUN([SCOREP_PTHREAD], [
AC_REQUIRE([SCOREP_CHECK_PTHREAD])dnl
AC_REQUIRE([SCOREP_LIBRARY_WRAPPING])dnl
AC_REQUIRE([AFS_CHECK_THREAD_LOCAL_STORAGE])dnl

scorep_pthread_support=${scorep_have_pthread}
scorep_pthread_summary_reason=

AS_IF([test "x${scorep_pthread_support}" = x1],
    [AM_COND_IF([HAVE_LIBWRAP_LINKTIME_SUPPORT],
        [],
        [scorep_pthread_support=0
         AS_VAR_APPEND([scorep_pthread_summary_reason], [", missing linktime library wrapping support"])])],
    [scorep_pthread_support=0
     AS_VAR_APPEND([scorep_pthread_summary_reason], [", missing pthread header or library"])])

# check result of TLS
AS_IF([test "x${scorep_pthread_support}" = x1],
    [AM_COND_IF([HAVE_THREAD_LOCAL_STORAGE],
        [],
        [scorep_pthread_support=0
         AS_VAR_APPEND([scorep_pthread_summary_reason], [", missing TLS support"])])])

AC_SUBST([SCOREP_HAVE_PTHREAD_SUPPORT], [${scorep_pthread_support}])
AFS_AM_CONDITIONAL([HAVE_PTHREAD_SUPPORT], [test "x${scorep_pthread_support}" = x1], [false])
AFS_AM_CONDITIONAL([HAVE_PTHREAD_WITHOUT_FLAGS],
    [test "x${PTHREAD_CFLAGS}" = x && test "x${PTHREAD_LIBS}" = x], [false])
AM_COND_IF([HAVE_PTHREAD_SUPPORT],
   [AFS_SUMMARY([Pthread support], [yes, using ${PTHREAD_CC} ${PTHREAD_CFLAGS} ${PTHREAD_LIBS}])],
   [AFS_SUMMARY([Pthread support], [no${scorep_pthread_summary_reason}])])
AS_UNSET([scorep_pthread_summary_reason])
])dnl SCOREP_PTHREAD
