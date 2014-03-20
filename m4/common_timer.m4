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
## Copyright (c) 2009-2013,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2012,
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


dnl Calls to the timer macros below might be m4-generated. In this
dnl case you get a 'possibly undefined macro' error durung autoreconf. To
dnl fix this, AC_REQUIRE COMMON_TIMER_SUCK_FILE. Ugly, but works. See
dnl e.g., AC_SCOREP_TIMER_CHECK.
AC_DEFUN([COMMON_TIMER_SUCK_FILE], [])

###############################################################################

AC_DEFUN([AC_SCOREP_TIMER_CLOCK_GETTIME_AVAILABLE],[
ac_scorep_timer_clock_gettime_available="no"
ac_scorep_timer_clock_gettime_clock=""

ac_scorep_timer_save_LIBS="$LIBS"
AC_SEARCH_LIBS([clock_gettime], [rt],
               [AS_IF([test "x${ac_cv_search_clock_gettime}" != "xnone required"],
                      [ac_scorep_timer_librt="$ac_cv_search_clock_gettime"])])

m4_foreach([clock],
          [[CLOCK_REALTIME], [CLOCK_MONOTONIC], [CLOCK_MONOTONIC_RAW]],
          [AC_MSG_CHECKING([for clock])
           AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
#ifdef _POSIX_C_SOURCE
#  if _POSIX_C_SOURCE < 199309L
#    undef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 199309L
#  endif
#else
#  define _POSIX_C_SOURCE 199309L
#endif
#include <time.h>
                                                      ],
                                                      [
    struct timespec tp;
    clock_getres(  clock, &tp );
    clock_gettime( clock, &tp );
                                                      ])],
                                     [ac_scorep_timer_clock_gettime_clock="clock"
                                      AC_MSG_RESULT([yes])],
                                     [AC_MSG_RESULT([no])])
])dnl

# perform a final link test
AS_IF([test "x${ac_scorep_timer_clock_gettime_clock}" != "x"],
      [AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#ifdef _POSIX_C_SOURCE
#  if _POSIX_C_SOURCE < 199309L
#    undef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 199309L
#  endif
#else
#  define _POSIX_C_SOURCE 199309L
#endif
#include <time.h>]], [[
    struct timespec tp;
    clock_getres( $ac_scorep_timer_clock_gettime_clock , &tp );
    clock_gettime( $ac_scorep_timer_clock_gettime_clock, &tp );
]])], [ac_scorep_timer_clock_gettime_available="yes"
       AC_DEFINE([HAVE_CLOCK_GETTIME], [1],
                 [Defined to 1 if the clock_gettime() function is available.])
       AC_DEFINE_UNQUOTED([SCOREP_CLOCK_GETTIME_CLOCK],
                          [${ac_scorep_timer_clock_gettime_clock}],
                          [The clock used in clock_gettime calls.])])])
AC_MSG_CHECKING([for clock_gettime timer])
AC_MSG_RESULT([$ac_scorep_timer_clock_gettime_available])

LIBS="$ac_scorep_timer_save_LIBS"

])

###############################################################################

AC_DEFUN([AC_SCOREP_TIMER_GETTIMEOFDAY_AVAILABLE],[
ac_scorep_timer_gettimeofday_available="no"
AH_TEMPLATE([HAVE_GETTIMEOFDAY],
            [Define to 1 if the gettimeofday() function is available.])
AC_MSG_CHECKING([for gettimeofday timer])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <sys/time.h>
                                ]], [[
struct timeval tp; gettimeofday( &tp, 0 );
                                ]])], [
ac_scorep_timer_gettimeofday_available="yes"
AC_DEFINE([HAVE_GETTIMEOFDAY])
                                ], [])
AC_MSG_RESULT([$ac_scorep_timer_gettimeofday_available])
])

###############################################################################
