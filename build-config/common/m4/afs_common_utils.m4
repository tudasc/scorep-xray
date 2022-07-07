## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2012, 2022,
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


# AFS_COMMON_UTILS(build|use-only, error header, include path)
# ------------------------------------------------------------
# Convenience macro to configure the common utils code.
#
# The first argument set to 'build' indicates that the common utils code
# will be both built and used by the corresponding build directory.  If
# set to 'use-only', it will only use and link a utils library built
# elsewhere.
#
# The second argument defines the name of the error codes header to be
# used.
#
# The third argument specifies the (relative) path where the error
# codes header can be found.
#
# NOTE:
#   The error codes header has to be generated outside of this macro.
#   This usually happens using `AC_CONFIG_HEADER` in the top-level
#   `configure.ac`.
#
AC_DEFUN([AFS_COMMON_UTILS], [
m4_case([$1],
    [build],
        [AC_REQUIRE([AM_PROG_AS])
         AC_REQUIRE([AC_SCOREP_PLATFORM_SETTINGS])
         _AFS_COMMON_UTILS_USE
         _AFS_COMMON_UTILS_BUILD],
    [use-only],
        [_AFS_COMMON_UTILS_USE],
    [m4_fatal([missing/invalid first argument `$1' to $0])])
m4_ifblank([$2], [m4_fatal([missing second argument to $0])])
m4_ifblank([$3], [m4_fatal([missing third argument to $0])])
dnl
AC_SUBST([PACKAGE_ERROR_CODES_INCDIR], [$3])
AC_DEFINE([PACKAGE_ERROR_CODES_HEADER], [$2],
    [The #include argument used to include this packages error codes header.])
]) # AFS_COMMON_UTILS


# _AFS_COMMON_UTILS_BUILD
# -----------------------
# Collection of configuration macros required to build the common
# utils code.
#
m4_define([_AFS_COMMON_UTILS_BUILD], [
AC_LANG_PUSH([C])
AFS_POSIX_GETCWD
AFS_POSIX_GETHOSTNAME
AC_LANG_POP([C])
AFS_THREAD_LOCAL_STORAGE
]) # _AFS_COMMON_UTILS_BUILD


# _AFS_COMMON_UTILS_USE
# ---------------------
# Collection of configuration macros required to use the common
# utils code.
#
m4_define([_AFS_COMMON_UTILS_USE], [
AFS_DEBUG_OPTION
AFS_GCC_ATOMIC_BUILTINS
]) # _AFS_COMMON_UTILS_USE
