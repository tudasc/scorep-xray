## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2014, 2017,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_library_wrapping.m4


AC_DEFUN([SCOREP_LIBRARY_WRAPPING], [
AC_REQUIRE([AFS_GNU_LINKER])dnl
AC_REQUIRE([SCOREP_CHECK_DLFCN])dnl

dnl Do not check for prerequisite of library wrapping on the frontend.
AS_IF([test "x$ac_scorep_backend" = xno], [AC_MSG_ERROR([cannot check for library wrapping support on frontend.])])

scorep_libwrap_support="no"
scorep_libwrap_default_mode=""

AC_MSG_CHECKING([for linktime wrapping support])
AM_COND_IF([HAVE_GNU_LINKER],
           [scorep_libwrap_support="yes"
            scorep_libwrap_linktime_support="yes"
            : ${scorep_libwrap_default_mode:=linktime}],
           [scorep_libwrap_linktime_support="no"])
AC_MSG_RESULT([${scorep_libwrap_linktime_support}])

AC_MSG_CHECKING([for runtime wrapping support])
AM_COND_IF([HAVE_DLFCN_SUPPORT],
           [scorep_libwrap_support="yes"
            scorep_libwrap_runtime_support="yes"
            : ${scorep_libwrap_default_mode:=runtime}],
           [scorep_libwrap_runtime_support="no"])
AC_MSG_RESULT([${scorep_libwrap_runtime_support}])

AC_MSG_CHECKING([default wrapping mode])
AC_MSG_RESULT([${scorep_libwrap_default_mode}])

AC_SCOREP_COND_HAVE([LIBWRAP_SUPPORT],
                    [test "x${scorep_libwrap_support}" = "xyes"],
                    [Define if library wrapping is possible at all.])

AC_SCOREP_COND_HAVE([LIBWRAP_LINKTIME_SUPPORT],
                    [test "x${scorep_libwrap_linktime_support}" = "xyes"],
                    [Define if linktime library wrapping is possible.])

AC_SCOREP_COND_HAVE([LIBWRAP_RUNTIME_SUPPORT],
                    [test "x${scorep_libwrap_runtime_support}" = "xyes"],
                    [Define if runtime library wrapping is possible.])

AC_SUBST([SCOREP_LIBWRAP_DEFAULT_MODE], [$scorep_libwrap_default_mode])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([SCOREP_LIBRARY_WRAPPING_GENERATOR], [

AFS_AM_CONDITIONAL([HAVE_LIBRARY_WRAPPING_GENERATOR],
                   [test -e ../build-libwrap/library_wrapping_supported],
                   [false])

AFS_SUMMARY_PUSH

AM_COND_IF([HAVE_LIBRARY_WRAPPING_GENERATOR],
           [scorep_have_libwrap_generator=yes
            $1],
           [scorep_have_libwrap_generator=no
            $2])

AFS_SUMMARY_POP([User library wrappers support], [${scorep_have_libwrap_generator}])
])
