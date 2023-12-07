## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2023,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

# AFS_LT_RESTRICTIONS()
# ---------------------
#
# Ensure that libtool libraries are either built shared or static.
# This makes development life easier w.r.t. to the instrumenter
# and the config tools. The linking-information (<pkg>-config --libs
# and --ldflags) for non-libtool users depends on the libraries' link
# mode.
# Intended to be used after LT_INIT. In LT_INIT, enable_shared and
# enable_static are set to the LT_INIT default, if not explicitly
# provided as --(en|dis)able-(static|shared) on the commandline.
#
# Provides AM_CONDITIONALS and #defines:
#     HAVE_BUILD_SHARED_LT_LIBRARIES
#     HAVE_BUILD_STATIC_LT_LIBRARIES
#
AC_DEFUN([AFS_LT_RESTRICTIONS],
[_AFS_LT_RESTRICTIONS([enable_shared], [enable_static])]dnl
) # AFS_LT_RESTRICTIONS

# AFS_LT_RESTRICTIONS_USER( SHARED_VAR, STATIC_VAR )
# --------------------------------------------------
#
# Like AFS_LT_RESTRICTIONS, but use SHARED_VAR and STATIC_VAR instead
# of enable_shared and enable_static
#
AC_DEFUN([AFS_LT_RESTRICTIONS_USER],
[m4_ifblank([$1], [m4_fatal([Macro requires at least two arguments])])]dnl
[m4_ifblank([$2], [m4_fatal([Macro requires at least two arguments])])]dnl
[_AFS_LT_RESTRICTIONS($1, $2)]dnl
) # AFS_LT_RESTRICTIONS_USER

# _AFS_LT_RESTRICTIONS( SHARED_VAR, STATIC_VAR )
# ----------------------------------------------
#
# Implementation of AFS_LT_RESTRICTIONS and AFS_LT_RESTRICTIONS_USER.
#
m4_define([_AFS_LT_RESTRICTIONS], [
AS_IF([test "x${$1}" = xyes && test "x${$2}" = xyes],
    [AC_MSG_ERROR(AFS_PACKAGE_name[ libraries are restricted to be either shared (default) or static (--disable-shared --enable-static).])])
AC_SCOREP_COND_HAVE([BUILD_SHARED_LT_LIBRARIES],
    [test "x${$1}" = xyes],
    [Whether libtool libraries are built shared])
AC_SCOREP_COND_HAVE([BUILD_STATIC_LT_LIBRARIES],
    [test "x${$2}" = xyes],
    [Whether libtool libraries are built static])
]) # _AFS_LT_RESTRICTIONS
