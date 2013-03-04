## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2013, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       ac_scorep_metrics.m4
## maintainer Christian Roessel <c.roessel@fz-juelich.de>


AC_DEFUN([AC_SCOREP_LIBPAPI], [

dnl Don't check for PAPI on the frontend.
AS_IF([test "x$ac_scorep_backend" = xno], [AC_MSG_ERROR([cannot check for PAPI on frontend.])])

# advertise the $PAPI_INC and $PAPI_LIB variables in the --help output
AC_ARG_VAR([PAPI_INC], [Include path to the papi.h header.])
AC_ARG_VAR([PAPI_LIB], [Library path to the papi library.])

dnl checking for the header
AC_ARG_WITH([papi-header],
            [AS_HELP_STRING([--with-papi-header=<path-to-papi.h>], 
                            [If papi.h is not installed in the default location, specify the dirname where it can be found.])],
            [ac_scorep_papi_inc_dir="${withval}"],  # action-if-given.
            [ac_scorep_papi_inc_dir="${PAPI_INC-}"] # action-if-not-given
)
# The -DC99 is a necessary gcc workaround for a
# bug in papi 4.1.2.1. It might be compiler dependent.
dc99_fix="-DC99"
AS_IF([test "x$ac_scorep_papi_inc_dir" != "x"], [
    ac_scorep_papi_cppflags="-I$ac_scorep_papi_inc_dir $dc99_fix"
], [
    ac_scorep_papi_cppflags="$dc99_fix"
])

AC_LANG_PUSH([C])
cppflags_save="$CPPFLAGS"

CPPFLAGS="$ac_scorep_papi_cppflags $CPPFLAGS"
AC_CHECK_HEADER([papi.h],
                [ac_scorep_papi_header="yes"],
                [ac_scorep_papi_header="no"])
CPPFLAGS="$cppflags_save"
AC_LANG_POP([C])


dnl checking for the library
AC_ARG_WITH([papi-lib],
            [AS_HELP_STRING([--with-papi-lib=<path-to-libpapi.*>], 
                            [If libpapi.* is not installed in the default location, specify the dirname where it can be found.])],
            [ac_scorep_papi_lib_dir="${withval}"],  # action-if-given
            [ac_scorep_papi_lib_dir="${PAPI_LIB-}"] # action-if-not-given
)
AS_IF([test "x$ac_scorep_papi_lib_dir" != "x"], [
    ac_scorep_papi_ldflags="-L$ac_scorep_papi_lib_dir"
], [
    ac_scorep_papi_ldflags=""
])

AC_LANG_PUSH([C])
ldflags_save="$LDFLAGS"
LDFLAGS="$ac_scorep_papi_lib_dir $LDFLAGS"
# To use PAPI on IBM systems you have to link to
# their performance monitor library (-lpmapi)
if test "x${ac_scorep_platform}" = "xaix"; then
    ac_scorep_papi_additional_libs="-lpmapi"
elif test "x${ac_scorep_platform}" = "xbgq"; then
    ac_scorep_papi_additional_libs="-lstdc++ -lrt"
else
    ac_scorep_papi_additional_libs=""
fi
ac_scorep_papi_lib_name="papi"
AC_CHECK_LIB([$ac_scorep_papi_lib_name], [PAPI_library_init],
             [ac_scorep_papi_library="yes"],  # action-if-found
             [ac_scorep_papi_library="no"],
             [$ac_scorep_papi_additional_libs]
)
if test "x${ac_scorep_papi_library}" = "xno"; then
    ac_scorep_papi_lib_name="papi64"
    AC_CHECK_LIB([$ac_scorep_papi_lib_name], [PAPI_library_init],
                 [ac_scorep_papi_library="yes"],  # action-if-found
                 [ac_scorep_papi_library="no"],
                 [$ac_scorep_papi_additional_libs]
    )
fi
LDFLAGS="$ldflags_save"
AC_LANG_POP([C])


dnl generating results/output/summary
ac_scorep_have_papi="no"
if test "x${ac_scorep_papi_header}" = "xyes" && test "x${ac_scorep_papi_library}" = "xyes"; then
    ac_scorep_have_papi="yes"
fi
AC_MSG_CHECKING([for papi support])
AC_MSG_RESULT([$ac_scorep_have_papi])
AC_SCOREP_COND_HAVE([PAPI],
                    [test "x${ac_scorep_have_papi}" = "xyes"],
                    [Defined if libpapi is available.],
                    [AC_SUBST([SCOREP_PAPI_LDFLAGS], ["-L${ac_scorep_papi_lib_dir} -R${ac_scorep_papi_lib_dir}"])
                     AC_SUBST([SCOREP_PAPI_LIBS],    ["-l${ac_scorep_papi_lib_name} ${ac_scorep_papi_additional_libs}"])],
                    [AC_SUBST([SCOREP_PAPI_LDFLAGS], [""])
                     AC_SUBST([SCOREP_PAPI_LIBS],    [""])])
AC_SUBST([SCOREP_PAPI_CPPFLAGS],    [$ac_scorep_papi_cppflags])
AC_SUBST([SCOREP_PAPI_LIBDIR],      [$ac_scorep_papi_lib_dir])
AC_SCOREP_SUMMARY([PAPI support],   [${ac_scorep_have_papi}])
AS_IF([test "x${ac_scorep_have_papi}" = "xyes"], [
    AC_SCOREP_SUMMARY_VERBOSE([PAPI include directory], [$ac_scorep_papi_inc_dir])
    AC_SCOREP_SUMMARY_VERBOSE([PAPI library directory], [$ac_scorep_papi_lib_dir])
    AC_SCOREP_SUMMARY_VERBOSE([PAPI libraries],         [-l${ac_scorep_papi_lib_name} ${ac_scorep_papi_additional_libs}])
])
])



AC_DEFUN([AC_SCOREP_RUSAGE], [

AS_IF([test "x${ac_scorep_platform}" = "xbgq"],
      [ac_scorep_getrusage="no, not supported on BG/Q"
       ac_scorep_rusage_thread="no, not supported on BG/Q"
       ac_scorep_rusage_cppflags=""],
      [dnl Check for getrusage function
       AC_LANG_PUSH([C])
       AC_CHECK_DECL([getrusage], 
                     [ac_scorep_getrusage="yes"], 
                     [ac_scorep_getrusage="no"], 
                     [[
#include <sys/time.h>
#include <sys/resource.h>
                     ]])

       dnl Check for availability of RUSAGE_THREAD
       ac_scorep_rusage_cppflags=""
       AC_CHECK_DECL([RUSAGE_THREAD], 
                     [ac_scorep_rusage_thread="yes"], 
                     [ac_scorep_rusage_thread="no"], 
                     [[
#include <sys/time.h>
#include <sys/resource.h>
                     ]])
       AS_IF([test "x$ac_scorep_rusage_thread" = "xno"],
             [unset ac_cv_have_decl_RUSAGE_THREAD
              cppflags_save="$CPPFLAGS"
              dnl For the affects of _GNU_SOURCE see /usr/include/features.h. Without
              dnl -D_GNU_SOURCE it seems that we don't get rusage per thread (RUSAGE_THREAD)
              dnl but per process only.
              ac_scorep_rusage_cppflags="-D_GNU_SOURCE"
              CPPFLAGS="${ac_scorep_rusage_cppflags} $CPPFLAGS"
              AC_CHECK_DECL([RUSAGE_THREAD], 
                            [ac_scorep_rusage_thread="yes"], 
                            [ac_scorep_rusage_thread="no"], 
                            [[
#include <sys/time.h>
#include <sys/resource.h>
                            ]])
              CPPFLAGS="$cppflags_save"])
       AC_LANG_POP([C])])

dnl generating results/output/summary
AC_SCOREP_COND_HAVE([GETRUSAGE],
                    [test "x${ac_scorep_getrusage}" = "xyes"],
                    [Defined if getrusage() is available.])
AS_IF([test "x${ac_scorep_rusage_thread}" = "xyes"],
      [AC_DEFINE([HAVE_RUSAGE_THREAD], [1], [Defined if RUSAGE_THREAD is available.])
       AC_DEFINE([SCOREP_RUSAGE_SCOPE], [RUSAGE_THREAD], [Defined to RUSAGE_THREAD, if it is available, else to RUSAGE_SELF.])],
      [AC_DEFINE([SCOREP_RUSAGE_SCOPE], [RUSAGE_SELF],   [Defined to RUSAGE_THREAD, if it is available, else to RUSAGE_SELF.])])
AC_SUBST([SCOREP_RUSAGE_CPPFLAGS], [$ac_scorep_rusage_cppflags])
AC_SCOREP_SUMMARY([getrusage support], [${ac_scorep_getrusage}])
AS_IF([test "x${ac_scorep_rusage_thread}" = "xyes"],
      [AC_SCOREP_SUMMARY([RUSAGE_THREAD support], [${ac_scorep_rusage_thread}, using ${ac_scorep_rusage_cppflags}])],
      [AC_SCOREP_SUMMARY([RUSAGE_THREAD support], [${ac_scorep_rusage_thread}])])
])
