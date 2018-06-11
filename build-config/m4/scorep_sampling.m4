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
## Copyright (c) 2009-2012, 2014-2017,
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

## file build-config/m4/scorep_sampling.m4


AC_DEFUN([SCOREP_SAMPLING], [
# check whether the compiler provides support for thread-local storage
# (TLS), the sampling check uses scorep_have_thread_local_storage to
# determine whether TLS is supported or not
AC_REQUIRE([SCOREP_CHECK_THREAD_LOCAL_STORAGE])
# Check whether we can use PAPI as interrupt generator
AC_REQUIRE([_SCOREP_METRICS_CHECK_LIBPAPI])
# Check whether we can use perf as interrupt generator
AC_REQUIRE([_SCOREP_METRICS_CHECK_PERF])

#check sampling prerequisites
AC_LANG_PUSH([C])

has_sampling_headers="yes"
AC_CHECK_HEADERS([sys/mman.h stdlib.h signal.h],
                 [],
                 [has_sampling_headers=no])

has_sampling_functions="yes"
AC_CHECK_FUNCS([sigaction],
               [],
               [has_sampling_functions="no"])

cppflags_save=${CPPFLAGS}
sampling_cppflags="-D_GNU_SOURCE"
CPPFLAGS="${sampling_cppflags} ${CPPFLAGS}"
AC_CHECK_MEMBER([struct sigaction.sa_handler],
                [has_sampling_sigaction_sa_handler="yes"],
                [has_sampling_sigaction_sa_handler="no"],
                [[#include <signal.h>]])

AC_CHECK_MEMBER([struct sigaction.sa_sigaction],
                [has_sampling_sigaction_sa_sigaction="yes"],
                [has_sampling_sigaction_sa_sigaction="no"],
                [[#include <signal.h>]])

AC_CHECK_TYPE([siginfo_t],
              [has_sampling_siginfo_t="yes"],
              [has_sampling_siginfo_t="no"],
              [[#include <signal.h>]])
CPPFLAGS="${cppflags_save}"

AC_LANG_POP([C])

AFS_SUMMARY_PUSH

# check for libunwind
AC_SCOREP_BACKEND_LIB([libunwind], [libunwind.h], [-D_GNU_SOURCE])

# check that we have at least one interrupt generator
AS_IF([test "x${ac_scorep_have_papi}" = "xyes" ||
       test "x${ac_scorep_have_perf}" = "xyes" ||
       ( test "x${has_sampling_sigaction_sa_sigaction}" = "xyes" && test "x${has_sampling_siginfo_t}" = "xyes" )],
      [have_interrupt_generators=yes],
      [have_interrupt_generators=no])

scorep_unwinding_support=yes
scorep_unwinding_summary_reason=
AS_IF([test "x${scorep_have_libunwind}" != "xyes"],
      [scorep_unwinding_support=no
       scorep_unwinding_summary_reason+=", missing libunwind support"])

AS_IF([test "x${scorep_have_thread_local_storage}" != "xyes"],
      [scorep_unwinding_support=no
       scorep_unwinding_summary_reason+=", missing TLS support"])

# Covers: GNU, Intel, IBM, Cray, Clang
scorep_return_address=0
AS_IF([test "x${scorep_unwinding_support}" = "xyes"],
      [AC_MSG_CHECKING([for __builtin_extract_return_addr/__builtin_return_address])
       AC_LINK_IFELSE([AC_LANG_PROGRAM([], [__builtin_extract_return_addr( __builtin_return_address( 0 ) );])],
                      [AC_MSG_RESULT([yes])
                       scorep_return_address="__builtin_extract_return_addr( __builtin_return_address( 0 ) )"],
                      [AC_MSG_RESULT([no])
                       AC_MSG_CHECKING([for __builtin_return_address])
                       AC_LINK_IFELSE([AC_LANG_PROGRAM([], [__builtin_return_address( 0 );])],
                                      [AC_MSG_RESULT([yes])
                                       scorep_return_address="__builtin_return_address( 0 )"],
                                      [AC_MSG_RESULT([no])])])])
AC_DEFINE_UNQUOTED([SCOREP_RETURN_ADDRESS()],
                   [$scorep_return_address],
                   [Provides the return address from the current function.])
AS_IF([test "x${scorep_return_address}" != x0], [
    AC_DEFINE([HAVE_RETURN_ADDRESS], [1], [Defined if SCOREP_RETURN_ADDRESS() is functional.])
])

AS_IF([test "x${scorep_unwinding_support}" = "xyes"],
      [save_CPPFLAGS=$CPPFLAGS
       CPPFLAGS="$CPPFLAGS ${with_libunwind_cppflags}"
       AC_CHECK_DECLS([unw_init_local2, unw_init_local_signal],
                      [], [], [[#define UNW_LOCAL_ONLY
#include <libunwind.h>
]])
       CPPFLAGS=$save_CPPFLAGS])

AFS_SUMMARY_POP([Unwinding support], [${scorep_unwinding_support}${scorep_unwinding_summary_reason}])

# generating output
AC_SCOREP_COND_HAVE([UNWINDING_SUPPORT],
                    [test "x${scorep_unwinding_support}" = "xyes"],
                    [Defined if unwinding support is available.],
                    [AC_SUBST([LIBUNWIND_CPPFLAGS], ["${with_libunwind_cppflags}"])
                     AC_SUBST([LIBUNWIND_LDFLAGS],  ["${with_libunwind_ldflags} ${with_libunwind_rpathflag}"])
                     AC_SUBST([LIBUNWIND_LIBS],     ["${with_libunwind_libs}"])])

AC_SCOREP_COND_HAVE([SAMPLING_SUPPORT],
                    [test "x${scorep_unwinding_support}" = "xyes" &&
                     test "x${has_sampling_headers}" = "xyes" &&
                     test "x${has_sampling_functions}" = "xyes" &&
                     test "x${has_sampling_sigaction_sa_handler}" = "xyes" &&
                     test "x${have_interrupt_generators}" = "xyes"],
                    [Defined if sampling support is available.],
                    [has_sampling="yes"
                     sampling_summary="yes, using ${sampling_cppflags}"
                     AC_SUBST([SAMPLING_CPPFLAGS], ["${sampling_cppflags}"])
                     AS_IF([test "x${has_sampling_sigaction_sa_sigaction}" = "xyes" &&
                            test "x${has_sampling_siginfo_t}" = "xyes"],
                           [AC_DEFINE([HAVE_SAMPLING_SIGACTION], [1],
                                      [Defined if struct member sigaction.sa_sigaction and type siginfo_t are available.])
                            sampling_summary+=", sa_sigaction"])],
                    [has_sampling="no"
                     AS_IF([test "x${have_interrupt_generators}" = "xno"],
                           [sampling_summary="no, cannot find any interrupt generator"],
                           [sampling_summary="no"])
                     AC_SUBST([SAMPLING_CPPFLAGS], [""])])

AFS_SUMMARY([Sampling support], [${sampling_summary}])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBUNWIND_LIB_CHECK], [
have_libunwind="no"
LIBS="-lunwind"

_AC_SCOREP_LIBUNWIND_LINK_TEST
AS_IF([test "x${have_libunwind}" = "xno"],
      [LIBS="${LIBS} -llzma";
       _AC_SCOREP_LIBUNWIND_LINK_TEST])

with_$1_lib_checks_successful=${have_libunwind}
with_$1_libs=${LIBS}
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBUNWIND_LINK_TEST], [
AC_LINK_IFELSE([_AC_SCOREP_LIBUNWIND_TEST_PROGRAM],
               [have_libunwind="yes"])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBUNWIND_TEST_PROGRAM], [
AC_LANG_PROGRAM([[
/* see man libunwind */
#define UNW_LOCAL_ONLY
#include <libunwind.h>]],
                [[
unw_cursor_t cursor;
unw_context_t uc;
unw_word_t ip, sp;

unw_getcontext(&uc);
unw_init_local(&cursor, &uc);

while (unw_step(&cursor) > 0) {
    unw_get_reg(&cursor, UNW_REG_IP, &ip);
    unw_get_reg(&cursor, UNW_REG_SP, &sp);
}]])
])
