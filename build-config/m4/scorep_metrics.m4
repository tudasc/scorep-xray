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

## file build-config/m4/scorep_metrics.m4

AC_DEFUN([SCOREP_METRICS_CHECK], [
    _SCOREP_METRICS_CHECK_LIBPAPI
    _SCOREP_METRICS_CHECK_RUSAGE
    _SCOREP_METRICS_CHECK_PERF
]) # AC_DEFUN(SCOREP_METRICS_CHECK)

AC_DEFUN_ONCE([_SCOREP_METRICS_CHECK_LIBPAPI], [

dnl Don't check for PAPI on the frontend.
AS_IF([test "x$ac_scorep_backend" = xno], [AC_MSG_ERROR([cannot check for PAPI on frontend.])])

# advertise the $PAPI_INC and $PAPI_LIB variables in the --help output
AC_ARG_VAR([PAPI_INC], [Include path to the papi.h header.])
AC_ARG_VAR([PAPI_LIB], [Library path to the papi library.])

dnl checking for the header
AC_ARG_WITH([papi-header],
            [AS_HELP_STRING([--with-papi-header=<path-to-papi.h>],
                            [If papi.h is not installed in the default location, specify the dirname where it can be found.])],
            [scorep_papi_inc_dir="${withval}"],  # action-if-given.
            [scorep_papi_inc_dir="${PAPI_INC-}"] # action-if-not-given
)
# The -DC99 is a necessary gcc workaround for a
# bug in papi 4.1.2.1. It might be compiler dependent.
dc99_fix="-DC99"
AS_IF([test "x$scorep_papi_inc_dir" != "x"], [
    scorep_papi_cppflags="-I$scorep_papi_inc_dir $dc99_fix"
], [
    scorep_papi_cppflags="$dc99_fix"
])

AC_LANG_PUSH([C])
cppflags_save="$CPPFLAGS"

CPPFLAGS="$scorep_papi_cppflags $CPPFLAGS"
AC_CHECK_HEADER([papi.h],
                [has_papi_header="yes"],
                [has_papi_header="no"])
CPPFLAGS="$cppflags_save"
AC_LANG_POP([C])


dnl checking for the library
AC_ARG_WITH([papi-lib],
            [AS_HELP_STRING([--with-papi-lib=<path-to-libpapi.*>],
                            [If libpapi.* is not installed in the default location, specify the dirname where it can be found.])],
            [scorep_papi_lib_dir="${withval}"],  # action-if-given
            [scorep_papi_lib_dir="${PAPI_LIB-}"] # action-if-not-given
)
AS_IF([test "x$scorep_papi_lib_dir" != "x"], [
    scorep_papi_ldflags="-L$scorep_papi_lib_dir"
], [
    scorep_papi_ldflags=""
])

AC_LANG_PUSH([C])
ldflags_save="$LDFLAGS"
LDFLAGS="$scorep_papi_ldflags $LDFLAGS"

# To use -papi or -papi64 we need to add additional libraries on several systems
AS_CASE([${ac_scorep_platform}],
    [aix],    [scorep_papi_extra_libs="-lpmapi"],
    [bgq],    [scorep_papi_extra_libs="-lstdc++ -lrt"],
    [mic],    [scorep_papi_extra_libs="-lpfm"],
    [crayx*], [scorep_papi_extra_libs="-ldl"])

for scorep_papi_lib_name in papi papi64; do
    for scorep_papi_additional_libs in "" "${scorep_papi_extra_libs}"; do
        AS_UNSET([ac_cv_lib_${scorep_papi_lib_name}___PAPI_library_init])
        AC_CHECK_LIB([$scorep_papi_lib_name],
            [PAPI_library_init],
            [has_papi_library=yes],
            [has_papi_library=no],
            [${scorep_papi_additional_libs}])
        AS_IF([test "x${has_papi_library}" = xyes],
            [break])
    done
    AS_IF([test "x${has_papi_library}" = xyes],
        [break])
done

LDFLAGS="$ldflags_save"
AC_LANG_POP([C])


dnl generating results/output/summary
scorep_have_papi_support="no"
if test "x${has_papi_header}" = "xyes" && test "x${has_papi_library}" = "xyes"; then
    scorep_have_papi_support="yes"
fi
AC_MSG_CHECKING([for papi support])
AC_MSG_RESULT([$scorep_have_papi_support])
AC_SCOREP_COND_HAVE([PAPI],
                    [test "x${scorep_have_papi_support}" = "xyes"],
                    [Defined if libpapi is available.],
                    [AS_IF([test "x${scorep_papi_lib_dir}" != "x"],
                         [AC_SUBST([SCOREP_PAPI_LDFLAGS], ["-L${scorep_papi_lib_dir} -R${scorep_papi_lib_dir}"])],
                         [AC_SUBST([SCOREP_PAPI_LDFLAGS], [""])])
                     AC_SUBST([SCOREP_PAPI_LIBS],    ["-l${scorep_papi_lib_name} ${scorep_papi_additional_libs}"])],
                    [AC_SUBST([SCOREP_PAPI_LDFLAGS], [""])
                     AC_SUBST([SCOREP_PAPI_LIBS],    [""])])
AC_SUBST([SCOREP_PAPI_CPPFLAGS],    [$scorep_papi_cppflags])
AC_SUBST([SCOREP_PAPI_LIBDIR],      [$scorep_papi_lib_dir])
AFS_SUMMARY([PAPI support],   [${scorep_have_papi_support}])
AS_IF([test "x${scorep_have_papi_support}" = "xyes"], [
    AFS_SUMMARY_VERBOSE([PAPI include directory], [$scorep_papi_inc_dir])
    AFS_SUMMARY_VERBOSE([PAPI library directory], [$scorep_papi_lib_dir])
    AFS_SUMMARY_VERBOSE([PAPI libraries],         [-l${scorep_papi_lib_name} ${scorep_papi_additional_libs}])
])
AS_UNSET([has_papi_header])
AS_UNSET([has_papi_library])
AS_UNSET([scorep_papi_inc_dir])
AS_UNSET([scorep_papi_cppflags])
AS_UNSET([scorep_papi_lib_dir])
AS_UNSET([scorep_papi_ldflags])
AS_UNSET([scorep_papi_lib_name])
AS_UNSET([scorep_papi_additional_libs])
])



AC_DEFUN([_SCOREP_METRICS_CHECK_RUSAGE], [

AS_IF([test "x${ac_scorep_platform}" = "xbgq"],
      [scorep_have_getrusage_support="no, not supported on BG/Q"
       has_rusage_thread="no, not supported on BG/Q"
       scorep_rusage_cppflags=""],
      [dnl Check for getrusage function
       AC_LANG_PUSH([C])
       AC_CHECK_DECL([getrusage],
                     [scorep_have_getrusage_support="yes"],
                     [scorep_have_getrusage_support="no"],
                     [[
#include <sys/time.h>
#include <sys/resource.h>
                     ]])

       dnl Check for availability of RUSAGE_THREAD
       scorep_rusage_cppflags=""
       AC_CHECK_DECL([RUSAGE_THREAD],
                     [has_rusage_thread="yes"],
                     [has_rusage_thread="no"],
                     [[
#include <sys/time.h>
#include <sys/resource.h>
                     ]])
       AS_IF([test "x$has_rusage_thread" = "xno"],
             [AS_UNSET([ac_cv_have_decl_RUSAGE_THREAD])
              cppflags_save="$CPPFLAGS"
              dnl For the affects of _GNU_SOURCE see /usr/include/features.h. Without
              dnl -D_GNU_SOURCE it seems that we don't get rusage per thread (RUSAGE_THREAD)
              dnl but per process only.
              scorep_rusage_cppflags="-D_GNU_SOURCE"
              CPPFLAGS="${scorep_rusage_cppflags} $CPPFLAGS"
              AC_CHECK_DECL([RUSAGE_THREAD],
                            [has_rusage_thread="yes"],
                            [has_rusage_thread="no"],
                            [[
#include <sys/time.h>
#include <sys/resource.h>
                            ]])
              CPPFLAGS="$cppflags_save"])
       AC_LANG_POP([C])])

dnl generating results/output/summary
AC_SCOREP_COND_HAVE([GETRUSAGE],
                    [test "x${scorep_have_getrusage_support}" = "xyes"],
                    [Defined if getrusage() is available.])
AS_IF([test "x${has_rusage_thread}" = "xyes"],
      [AC_DEFINE([HAVE_RUSAGE_THREAD], [1], [Defined if RUSAGE_THREAD is available.])
       AC_DEFINE([SCOREP_RUSAGE_SCOPE], [RUSAGE_THREAD], [Defined to RUSAGE_THREAD, if it is available, else to RUSAGE_SELF.])],
      [AC_DEFINE([SCOREP_RUSAGE_SCOPE], [RUSAGE_SELF],   [Defined to RUSAGE_THREAD, if it is available, else to RUSAGE_SELF.])])
AC_SUBST([SCOREP_RUSAGE_CPPFLAGS], [$scorep_rusage_cppflags])
AFS_SUMMARY([getrusage support], [${scorep_have_getrusage_support}])
AS_IF([test "x${has_rusage_thread}" = "xyes"],
      [AFS_SUMMARY([RUSAGE_THREAD support], [${has_rusage_thread}, using ${scorep_rusage_cppflags}])],
      [AFS_SUMMARY([RUSAGE_THREAD support], [${has_rusage_thread}])])
AS_UNSET([scorep_rusage_cppflags])
AS_UNSET([has_rusage_thread])
])



AC_DEFUN_ONCE([_SCOREP_METRICS_CHECK_PERF], [
AC_REQUIRE([AC_SCOREP_POSIX_FUNCTIONS])

dnl Do not check for prerequisite of metric perf on the frontend.
AS_IF([test "x$ac_scorep_backend" = xno], [AC_MSG_ERROR([cannot check for metric perf on frontend.])])

AC_LANG_PUSH([C])

##
## Check for perf headers and selected metrics
##
has_metric_perf_headers="yes"
AC_CHECK_HEADERS([linux/perf_event.h],
                 [],
                 [has_metric_perf_headers="no"])
AC_CHECK_DECLS([PERF_COUNT_HW_STALLED_CYCLES_FRONTEND,
                PERF_COUNT_HW_STALLED_CYCLES_BACKEND,
                PERF_COUNT_HW_REF_CPU_CYCLES,
                PERF_COUNT_SW_ALIGNMENT_FAULTS,
                PERF_COUNT_SW_EMULATION_FAULTS],
               [], [], [[#include <linux/perf_event.h>]])

##
## Check for syscall perf_event_open
##
SCOREP_CHECK_SYSCALL([__NR_perf_event_open],
                     [has_syscall_perf_event_open="yes"],
                     [has_syscall_perf_event_open="no"])

##
## Check for ioctl
##
## The manpage says:
## int ioctl(int d, int request, ...);
## sys/ioctl.h says:
## int ioctl(int d, unsigned long int request, ...);
has_ioctl_support="yes"
AC_CHECK_HEADER([sys/ioctl.h],
                [],
                [has_ioctl_support="no"])
AS_IF([test "x${has_ioctl_support}" = "xyes"],
      [AC_CHECK_FUNC([ioctl],
                     [],
                     [has_ioctl_support="no"])])
AS_IF([test "x${has_ioctl_support}" = "xyes"],
      [AC_LINK_IFELSE([AC_LANG_SOURCE([
                        #include <sys/ioctl.h>

                        int main()
                        {
                            int result = ioctl(0, 0);
                            return 0;
                        }
                      ])],
                      [],
                      [has_ioctl_support="no"])])

AC_LANG_POP([C])

## Check availability of POSIX read and close functions,
## corresponding AM_CONDITIONALs provided by AC_SCOREP_POSIX_FUNCTIONS
has_posix_functions="yes"
AM_COND_IF([HAVE_READ],
           [],
           [has_posix_functions="no"])
AM_COND_IF([HAVE_CLOSE],
           [],
           [has_posix_functions="no"])

scorep_have_perf_support="no"
metric_perf_summary_reason=
AS_IF([test "x${has_metric_perf_headers}"     = "xyes" &&
       test "x${has_syscall_perf_event_open}" = "xyes" &&
       test "x${has_ioctl_support}"           = "xyes" &&
       test "x${has_posix_functions}"         = "xyes" ], [
    AS_IF([test "x${ac_scorep_platform}" = "xbgq"],
          [metric_perf_summary_reason=", not supported on BG/Q"],
          [scorep_have_perf_support="yes"])
])
AS_UNSET([has_metric_perf_headers])
AS_UNSET([has_syscall_perf_event_open])
AS_UNSET([has_ioctl_support])
AS_UNSET([has_posix_functions])

AC_SCOREP_COND_HAVE([METRIC_PERF],
                    [test "x${scorep_have_perf_support}" = "xyes"],
                    [Defined if metric perf support is available.])

AFS_SUMMARY([metric perf support], [${scorep_have_perf_support}${metric_perf_summary_reason}])
AS_UNSET([metric_perf_summary_reason])
])
