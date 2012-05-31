## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
##    RWTH Aachen University, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##


## check fo some usefull entropy sources for the trace id.

AC_DEFUN([AC_OTF2_ENTROPY_SOURCES],
[
AC_LANG_PUSH([C])
SCOREP_TIMER_CLOCK_GETTIME_AVAILABLE
SCOREP_TIMER_GETTIMEOFDAY_AVAILABLE

scorep_timer_lib=""
AS_IF([test "x${scorep_timer_clock_gettime_available}" = "xyes"], [scorep_timer_lib=${scorep_timer_librt}])
AC_SUBST([TIMER_LIB], ["$scorep_timer_lib"])

AH_TEMPLATE([HAVE_UNISTD_H],
            [Define to 1 if you have the <unistd.h> header file.])
AH_TEMPLATE([HAVE_GETPID],
            [Define to 1 if the getpid() function is available.])
AH_TEMPLATE([HAVE_SYS_SYSINFO_H],
            [Define to 1 if you have the <sys/sysinfo.h> header file.])
AH_TEMPLATE([HAVE_SYSINFO],
            [Define to 1 if the sysinfo() function is available.])

AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <unistd.h>
                                ]], [[
pid_t = getpid();
                                ]])], [
AC_DEFINE([HAVE_UNISTD_H])
AC_DEFINE([HAVE_GETPID])
                                ], [])

AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <sys/sysinfo.h>
                                ]], [[
struct sysinfo info;
sysinfo( &info );
                                ]])], [
AC_DEFINE([HAVE_SYS_SYSINFO_H])
AC_DEFINE([HAVE_SYSINFO])
                                ], [])

AC_LANG_POP([C])
]
)
