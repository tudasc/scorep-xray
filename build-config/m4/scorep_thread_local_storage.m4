## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2014
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

AC_DEFUN([SCOREP_THREAD_LOCAL_STORAGE], [

dnl Do not check for prerequisite of thread local storage support on the frontend.
AS_IF([test "x$ac_scorep_backend" = xno], [AC_MSG_ERROR([cannot check for thread local storage support on frontend.])])

AC_LANG_PUSH([C])

dnl There is a bug in gcc < 4.1.2 involving TLS and -fPIC on x86:
dnl http://gcc.gnu.org/ml/gcc-bugs/2006-09/msg02275.html
dnl
dnl And mingw also does compile __thread but resultant code actually
dnl fails to work correctly at least in some not so ancient version:
dnl http://mingw-users.1079350.n2.nabble.com/gcc-4-4-multi-threaded-exception-handling-amp-thread-specifier-not-working-td3440749.html
dnl
dnl Also it was reported that earlier gcc versions for mips compile
dnl __thread but it does not really work

AC_MSG_CHECKING([for __thread])
AC_LINK_IFELSE([AC_LANG_PROGRAM([#if defined(__GNUC__) && ((__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 1) || (__GNUC__ == 4 && __GNUC_MINOR__ == 1 && __GNUC_PATCHLEVEL__ < 2))
#ups_unsupported_gcc_version gcc has this bug: http://gcc.gnu.org/ml/gcc-bugs/2006-09/msg02275.html
#elif defined(__MINGW32__)
#ups_unsupported_mingw_version mingw doesnt really support thread local storage
#elif defined(__APPLE__)
#ups_unsupported_platform OSX __thread support is known to call malloc which makes it unsafe to use from malloc replacement
#endif

__thread int global_thread_private_var = 1;
], [static __thread int my_thread_private_var = 0])],
   [have_thread_local_storage="yes"],
   [have_thread_local_storaeg="no"])
AC_MSG_RESULT([${have_thread_local_storage}])

AC_LANG_POP([C])

AC_SCOREP_COND_HAVE([THREAD_LOCAL_STORAGE],
                    [test "x${have_thread_local_storage}" = "xyes"],
                    [Defined if thread local storage support is available.])
])
