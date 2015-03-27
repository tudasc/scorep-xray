## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2014,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2011, 2015,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2011,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file afs_debug_option.m4


# AFS_DEBUG_OPTION
# ----------------
# Defines and handles the configure option '--enable-debug'.  Ensures
# that only valid arguments (yes|no) are passed and sets a number of
# debugging-related config header defines.
#
# List of configure variables set:
#  `enable_debug`::  'yes' if debug mode is enabled, 'no' otherwise
#
# List of provided config header defines:
#  `HAVE_<AFS_PACKAGE>_DEBUG`::     Set to '1' if debugging code is enabled,
#                                   unset otherwise
#  `HAVE_<AFS_PACKAGE>_NO_ASSERT::  Always set to '0'
#
AC_DEFUN([AFS_DEBUG_OPTION],
[
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable additional debug output and checks [no]])],
    [],
    [enable_debug=no])
AS_CASE([$enable_debug],
    [yes],
        [AC_DEFINE([HAVE_]AFS_PACKAGE_NAME[_DEBUG], [1],
         [Define to 1 to enable additional debug output and checks.])],
    [no],
        [],
    [AC_MSG_ERROR([invalid value "$enable_debug" passed to --enable-debug])])
AC_DEFINE([HAVE_]AFS_PACKAGE_NAME[_NO_ASSERT], [0],
    [Define to 1 to disable assertions (like NDEBUG).])
])


# Backward compatibility macros.  These macros issue a warning during
# bootstrap and can be removed once packages have been updated to use
# AFS_DEBUG_OPTION.
#
AU_ALIAS([AC_SCOREP_DEBUG_OPTION], [AFS_DEBUG_OPTION])
AU_DEFUN([AC_SCOREP_ON_DEBUG_OPTION], [])
