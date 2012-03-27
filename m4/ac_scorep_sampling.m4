## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012, 
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

## file       ac_scorep_sampling.m4
## maintainer Christian Roessel <c.roessel@fz-juelich.de>


AC_DEFUN([AC_SCOREP_LIBUNWIND], [
AC_SCOREP_BACKEND_LIB([libunwind], [libunwind.h], [-D_XOPEN_SOURCE=500])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBUNWIND_LIB_CHECK], [
LIBS="-lunwind"

AC_LINK_IFELSE([AC_LANG_PROGRAM([[
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
    /* printf ("ip = %lx, sp = %lx\n", (long) ip, (long) sp); */
}]])],
               [with_libunwind_lib_checks_successful="yes"], 
               [with_libunwind_lib_checks_successful="no"])

with_libunwind_libs="-lunwind"
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([AC_SCOREP_SAMPLING], [

AC_LANG_PUSH([C])

has_sampling_headers="yes"
AC_CHECK_HEADERS([sys/mman.h stdlib.h signal.h],
                 [],
                 [has_sampling_headers=no])

has_sampling_functions="yes"
AC_CHECK_FUNCS([mprotect posix_memalign sigaction],
               [],
               [has_sampling_functions="no"])

AC_CHECK_TYPE([sig_atomic_t], 
              [has_sampling_sig_atomic_t="yes"], 
              [has_sampling_sig_atomic_t="no"], 
              [[#include <signal.h>]])

cppflags_save=${CPPFLAGS}
sampling_cppflags="-D_XOPEN_SOURCE=500"
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

# generating output
AS_IF([   test "x${has_sampling_headers}" = "xyes"              \
       && test "x${has_sampling_functions}" = "xyes"            \
       && test "x${has_sampling_sig_atomic_t}" = "xyes"         \
       && test "x${has_sampling_sigaction_sa_handler}" = "xyes"],
      [has_sampling="yes"
       sampling_summary="yes, using ${sampling_cppflags}"
       AC_SUBST([SAMPLING_CPPFLAGS], ["${sampling_cppflags}"])
       AS_IF([   test "x${has_sampling_sigaction_sa_sigaction}" = "xyes" \
              && test "x${has_sampling_siginfo_t}" = "xyes"], 
             [AC_DEFINE([HAVE_SAMPLING_SIGACTION], [1], 
                        [Defined if struct member sigaction.sa_sigaction and type siginfo_t are available.])
              sampling_summary="yes, using ${sampling_cppflags} and sa_sigaction"])],
      [has_sampling="no"
       sampling_summary="no"
       AC_SUBST([SAMPLING_CPPFLAGS], [""])])

AM_CONDITIONAL([HAVE_SAMPLING], [test "x${has_sampling}" = "xyes"])

AC_SCOREP_SUMMARY([sampling support], [${sampling_summary}])
])
