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
## Copyright (c) 2009-2012, 2014-2017, 2020,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2012, 2021-2022,
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
AC_REQUIRE([SCOREP_ADDR2LINE])dnl
# check whether the compiler provides support for thread-local storage
# (TLS), the sampling check uses afs_have_thread_local_storage to
# determine whether TLS is supported or not
AC_REQUIRE([AFS_CHECK_THREAD_LOCAL_STORAGE])
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
AC_LANG_PUSH([C])
AFS_EXTERNAL_LIB([unwind], [_LIBUNWIND_CHECK], [libunwind.h], [_LIBUNWIND_DOWNLOAD])dnl
AC_LANG_POP([C])

# check that we have at least one interrupt generator
AS_IF([test "x${scorep_have_papi_support}" = "xyes" ||
       test "x${scorep_have_perf_support}" = "xyes" ||
       ( test "x${has_sampling_sigaction_sa_sigaction}" = "xyes" && test "x${has_sampling_siginfo_t}" = "xyes" )],
      [have_interrupt_generators=yes],
      [have_interrupt_generators=no])

scorep_unwinding_support=${have_libunwind}
scorep_unwinding_summary_reason=${scorep_libunwind_summary_reason}

AS_IF([test "x${afs_have_thread_local_storage}" != "xyes"],
      [scorep_unwinding_support=no
       scorep_unwinding_summary_reason+=", missing TLS support"])

SCOREP_CHECK_SYSCALL([SYS_gettid],
                     [],
                     [scorep_unwinding_support=no
                      scorep_unwinding_summary_reason+=", missing gettid() support"])

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
       CPPFLAGS="$CPPFLAGS ${SCOREP_LIBUNWIND_CPPFLAGS}"
       AC_CHECK_DECLS([unw_init_local2, unw_init_local_signal, unw_strerror],
                      [], [], [[#define UNW_LOCAL_ONLY
#include <libunwind.h>
]])
       CPPFLAGS=$save_CPPFLAGS])

AFS_SUMMARY_POP([Unwinding support], [${scorep_unwinding_support}${scorep_unwinding_summary_reason}])

# generating output
AC_SCOREP_COND_HAVE([UNWINDING_SUPPORT],
                    [test "x${scorep_unwinding_support}" = "xyes"],
                    [Defined if unwinding support is available.])

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

# _LIBUNWIND_DOWNLOAD()
# ---------------------
# Generate a Makefile.libunwind to download libunwind and install libunwind
# at make time. In addition, set automake conditional
# HAVE_SCOREP_LIBUNWIND_MAKEFILE to trigger this process from
# build-backend/Makefile.
#
m4_define([_LIBUNWIND_DOWNLOAD], [
_afs_lib_PREFIX="$prefix/vendor/[]_afs_lib_name"
_afs_lib_MAKEFILE="Makefile.[]_afs_lib_name"
_afs_lib_LDFLAGS="-L$[]_afs_lib_PREFIX[]/lib -R$[]_afs_lib_PREFIX[]/lib"
_afs_lib_CPPFLAGS="-I$[]_afs_lib_PREFIX/include -D_GNU_SOURCE"
dnl
AFS_AM_CONDITIONAL(HAVE_[]_afs_lib_MAKEFILE, [true], [false])dnl
dnl libunwind_package and libunwind_base_url are sourced from build-config/downloads
have_libunwind="yes"
scorep_libunwind_summary_reason=", from downloaded $libunwind_base_url/${libunwind_package}.tar.gz"
dnl check will fail, used version provides unw_init_local2
ac_cv_have_decl_unw_init_local2=yes
dnl check will fail, used version provides unw_strerror
ac_cv_have_decl_unw_strerror=yes
dnl
m4_changecom([])
cat <<_SCOREPEOF > $[]_afs_lib_MAKEFILE
#
# $(pwd)/$_afs_lib_MAKEFILE
#
# Executing 'make -f $_afs_lib_MAKEFILE' downloads a libunwind
# package and installs a shared _afs_lib_name into
# $prefix/vendor/[]_afs_lib_name
# using CC=gcc that was found in PATH.
#
# Usually, this process is triggered during Score-P's build-backend
# make. If _afs_lib_name's configure or make fail, or if there are
# failures in the subsequent build process of Score-P, consider
# modifying CC above to point to a compiler (gcc recommended) that is
# compatible with the compute node compiler that you are using for
# Score-P (i.e., $CC) and try (manually) again. Note that PGI/NVIDIA
# and well as non-clang-based Cray compilers fail to build
# _afs_lib_name.
#
# You can also modify the installation prefix if, e.g., you want to
# share the _afs_lib_name installation between several Score-P
# installations (which then need to be configured using
# --with-[]_afs_lib_name=<prefix>).
#
# Please report bugs to <support@score-p.org>.
#
THIS_FILE = $(pwd)/$_afs_lib_MAKEFILE
URL = $libunwind_base_url
PACKAGE = $libunwind_package
PREFIX = $[]_afs_lib_PREFIX
CC = gcc
all:
	@$AFS_LIB_DOWNLOAD_CMD \$(URL)/\$(PACKAGE).tar.gz
	@tar xf \$(PACKAGE).tar.gz
	@mkdir \$(PACKAGE)/_build
	@cd \$(PACKAGE)/_build && \\
	    ../configure \\
	        --silent \\
	        --enable-silent-rules \\
	        --prefix=\$(PREFIX) \\
	        --libdir=\$(PREFIX)/lib \\
	        CC=\$(CC) \\
	        --enable-shared \\
	        --disable-static \\
	        --disable-ptrace \\
	        --disable-coredump \\
	        --disable-setjmp \\
	        --disable-weak-backtrace \\
	        --disable-documentation \\
	        --disable-tests \\
	        --disable-per-thread-cache && \\
	    make -s install
	@rm -f \$(PREFIX)/lib/libunwind.la
clean:
	@rm -rf \$(PACKAGE).tar.gz \$(PACKAGE)
uninstall:
	@rm -rf \$(PREFIX)
_SCOREPEOF
m4_changecom([#])
])# _LIBUNWIND_DOWNLOAD

dnl ----------------------------------------------------------------------------

AC_DEFUN([_LIBUNWIND_CHECK], [
have_libunwind="no"
AS_IF([test "x${_afs_lib_prevent_check}" = xyes],
    [AS_IF([test "x${_afs_lib_prevent_check_reason}" = xdisabled],
        [scorep_libunwind_summary_reason=", explicitly disabled"],
        [test "x${_afs_lib_prevent_check_reason}" = xcrosscompile],
        [scorep_libunwind_summary_reason=", --with-_afs_lib_name needs path or download in cross-compile mode"],
        [AC_MSG_ERROR([Unknown _afs_lib_prevent_check_reason "${_afs_lib_prevent_check_reason}".])])],
    [CPPFLAGS="$CPPFLAGS $_afs_lib_CPPFLAGS"
     AC_CHECK_HEADER([libunwind.h],
         [LTLDFLAGS=$_afs_lib_LDFLAGS
          LTLIBS=$_afs_lib_LIBS
          AFS_LTLINK_LA_IFELSE([_LIBUNWIND_MAIN], [_LIBUNWIND_LA],
              [have_libunwind="yes"
               scorep_libunwind_summary_reason="${_afs_lib_LDFLAGS:+, using $_afs_lib_LDFLAGS}${_afs_lib_CPPFLAGS:+ and $_afs_lib_CPPFLAGS}"],
              [scorep_libunwind_summary_reason=", cannot link against $_afs_lib_LIBS"])
         ],
         [scorep_libunwind_summary_reason=", missing libunwind.h header, try --with-_afs_lib_name[]=download"])])
])# _LIBUNWIND_CHECK

# _LIBUNWIND_LA()
# ---------------
# The source code for the libtool archive.
#
m4_define([_LIBUNWIND_LA], [
AC_LANG_SOURCE([[
/* see man libunwind */
#define UNW_LOCAL_ONLY
#include <libunwind.h>
char test_backtrace ()
{
    unw_cursor_t cursor;
    unw_context_t uc;
    unw_word_t ip, sp;

    unw_getcontext(&uc);
    unw_init_local(&cursor, &uc);

    while (unw_step(&cursor) > 0) {
        unw_get_reg(&cursor, UNW_REG_IP, &ip);
        unw_get_reg(&cursor, UNW_REG_SP, &sp);
    }
}
]])])#_LIBUNWIND_LA


# _LIBUNWIND_MAIN()
# -----------------
# The source code using the libtool archive.
#
m4_define([_LIBUNWIND_MAIN], [
AC_LANG_PROGRAM(dnl
[[char test_backtrace ();]],
[[test_backtrace ();]]
)])#_LIBUNWIND_MAIN
