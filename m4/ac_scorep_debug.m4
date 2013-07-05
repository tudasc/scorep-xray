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
## Copyright (c) 2009-2013,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2011,
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

## file       ac_scorep_debug.m4
## maintainer Christian Roessel <c.roessel@fz-juelich.de>


AC_DEFUN([AC_SCOREP_DEBUG_OPTION],
[
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug], [activate internal debug output [no]])],
              [],
              [enable_debug=no])

ac_scorep_enable_debug="no"
if test "x${enable_debug}" = "xyes"; then
    ac_scorep_enable_debug="yes"
fi
])


AC_DEFUN([AC_SCOREP_ON_DEBUG_OPTION], [
    AS_IF([test  "x${ac_scorep_enable_debug}" = "xyes"], [
        AC_DEFINE([HAVE_]AFS_PACKAGE_SYM_CAPS[_DEBUG], [1],
            [Define to 1 to enable internal debug messages (like NDEBUG).])
    ], [test  "x${ac_scorep_enable_debug}" != "xno"], [
        AC_MSG_ERROR([ac_scorep_enable_debug neither "yes" nor "no" but "$ac_scorep_enable_debug", that's weird.])
    ])
    AC_DEFINE([HAVE_]AFS_PACKAGE_SYM_CAPS[_NO_ASSERT], [0],
        [Define to 1 to disable assertions (like NDEBUG).])
])
