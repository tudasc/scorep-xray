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
## Copyright (c) 2009-2013,
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

## file       ac_common_package.m4

# AFS_PACKAGE_INIT([BUILD-NAME[, TO-TOP]])
# ------------------------------------------------------
# Common AC_DEFINE's and AC_SUBST's for the package based on its name.
#
# List of defined autoconf macros:
#  `AFS_PACKAGE_SYM`::        The tarname of the package in lower case
#  `AFS_PACKAGE_SYM_CAPS`::   The tarname of the package in upper case
#  `AFS_PACKAGE_TOP_BUILD`::  The path to the top-level configure including a
#                             trailing slash  (empty for the top-level configure
#                             itself)
# List of provided automake substitutions:
#  `PACKAGE_SYM`::            The value of AFS_PACKAGE_SYM
#  `PACKAGE_SYM_CAPS`::       The value of AFS_PACKAGE_SYM_CAPS
#
# BUILD-NAME should be non-blank for sub-configures. The path to the top-level
# configure can be specified with TO-TOP, defaulting to `../`. In this case there
# will also be the following definitions:
#
# List of defined autoconf macros:
#  `AFS_PACKAGE_BUILD_NAME`:: The normalized name of the build (e.g. 'backend',
#                             'MPI backend')
#  `AFS_PACKAGE_build`::      The build name usable as a symbol in lower case
#                             (e.g. backend, mpi_backend)
#  `AFS_PACKAGE_BUILD`::      The build name usable as a symbol in upper case
#                             (e.g. BACKEND, MPI_BACKEND)
# List of provided automake substitutions:
#  'PACKAGE_build'            The value of AFS_PACKAGE_build
#  'PACKAGE_BUILD'            The value of AFS_PACKAGE_BUILD
# List of provided config header defines:
#  `PACKAGE_BUILD_NAME`::     The value of AFS_PACKAGE_BUILD_NAME as a string
#                             constant
#  `PACKAGE_BUILD`::          The value of AFS_PACKAGE_BUILD
#  `PACKAGE_SRCDIR`::         The absolute path of the source directory as
#                             string constant
#  `PACKAGE_BUILDDIR`::       The absolute path of the build directory as string
#                             constant
#  `PACKAGE_SYM`::            The value of AFS_PACKAGE_SYM
#  `PACKAGE_SYM_CAPS`::       The value of AFS_PACKAGE_SYM_CAPS
#
AC_DEFUN_ONCE([AFS_PACKAGE_INIT], [

m4_define([AFS_PACKAGE_SYM],      m4_tolower(AC_PACKAGE_TARNAME))dnl
m4_define([AFS_PACKAGE_SYM_CAPS], m4_toupper(AC_PACKAGE_TARNAME))dnl

m4_ifnblank([$1], [
    m4_define([AFS_PACKAGE_BUILD_NAME], m4_normalize($1))dnl
    AC_DEFINE_UNQUOTED([PACKAGE_BUILD_NAME], "AFS_PACKAGE_BUILD_NAME",
        [Name of the sub-build.])

    m4_define([AFS_PACKAGE_TOP_BUILD], m4_ifblank([$2], [../], $2))
    m4_if(m4_substr(AFS_PACKAGE_TOP_BUILD, decr(len(AFS_PACKAGE_TOP_BUILD))), [/],
        [], [m4_fatal([no trailing slash in second argument to AFS_PACKAGE_INIT: ]AFS_PACKAGE_TOP_BUILD)])

    m4_define([AFS_PACKAGE_build],
        m4_bpatsubst(m4_tolower(m4_normalize($1)), [[^a-z0-9]+], [_]))
    AC_SUBST([PACKAGE_build], AFS_PACKAGE_build)

    m4_define([AFS_PACKAGE_BUILD],
        m4_toupper(AFS_PACKAGE_build))
    AC_DEFINE_UNQUOTED([PACKAGE_BUILD], AFS_PACKAGE_BUILD,
        [Symbol name of the sub-build.])
    AC_SUBST([PACKAGE_BUILD], AFS_PACKAGE_BUILD)

    AS_CASE([$srcdir],
        [/*], [afs_package_srcdir=${srcdir}],
        [afs_package_srcdir="${ac_pwd}/${srcdir}"])dnl
    AC_DEFINE_UNQUOTED([PACKAGE_SRCDIR],
        ["${afs_package_srcdir}/]AFS_PACKAGE_TOP_BUILD["], [Source directory.])

    AC_DEFINE_UNQUOTED([PACKAGE_BUILDDIR],
        ["$ac_pwd"], [Build directory.])

    AC_DEFINE_UNQUOTED([PACKAGE_SYM], AFS_PACKAGE_SYM,
        [The package name usable as a symbol.])

    AC_DEFINE_UNQUOTED([PACKAGE_SYM_CAPS], AFS_PACKAGE_SYM_CAPS,
        [The package name usable as a symbol in all caps.])
], [
    m4_define([AFS_PACKAGE_TOP_BUILD], [])dnl
])

AC_SUBST([PACKAGE_SYM],      AFS_PACKAGE_SYM)
AC_SUBST([PACKAGE_SYM_CAPS], AFS_PACKAGE_SYM_CAPS)
])

# AC_SCOREP_DEFINE_HAVE(VARIABLE, VALUE[, DESCRIPTION])
# ------------------------------------------------------
# Like AC_DEFINE, but prepends the HAVE_ prefix and also defines the
# HAVE_'PACKAGE_BUILD'_ variant, if in a sub configure by utilizing the
# AFS_PACKAGE_BUILD macro.
#
AC_DEFUN([AC_SCOREP_DEFINE_HAVE], [
AC_DEFINE([HAVE_]$1, [$2], [$3])
m4_ifdef([AFS_PACKAGE_BUILD], [
    AC_DEFINE([HAVE_]AFS_PACKAGE_BUILD[_]$1, [$2], [$3])
])
])

# AC_SCOREP_COND_HAVE(VARIABLE, CONDITION[, DESCRIPTION[, COND_TRUE[, COND_FALSE]]])
# --------------------------------------------------
# Convenience macro to define a AM_CONDITIONAL and always a
# AC_SCOREP_DEFINE_HAVE at once. VARIABLE will be prefixed with HAVE_
#
AC_DEFUN([AC_SCOREP_COND_HAVE], [
AM_CONDITIONAL(HAVE_[]$1, [$2])
AM_COND_IF(HAVE_[]$1,
           [AC_SCOREP_DEFINE_HAVE([$1], [1], [$3])
            $4],
           [AC_SCOREP_DEFINE_HAVE([$1], [0], [$3])
            $5])
])
