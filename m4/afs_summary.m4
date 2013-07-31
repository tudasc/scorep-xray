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

## file       afs_summary.m4
## maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>

# AFS_SUMMARY_INIT
# ----------------
# Initializes the summary system and adds the pacakge header (possibly
# including the sub-build name) to it.
# The sub-build name is used from the `AFS_PACKAGE_BUILD_NAME` variable
# set by the AFS_PACAKGE_INIT macro.
AC_DEFUN([AFS_SUMMARY_INIT], [
AC_BEFORE([AFS_PACAKGE_INIT], [$0])dnl
rm -f AC_PACKAGE_TARNAME.summary
cat >config.summary <<_ACEOF
AS_HELP_STRING(AC_PACKAGE_NAME[]m4_ifdef([AFS_PACKAGE_BUILD_NAME], [ (]AFS_PACKAGE_BUILD_NAME[):]), [], 32, 128)
_ACEOF
])

AC_DEFUN([AFS_SUMMARY_SECTION], [
    AC_REQUIRE([AFS_SUMMARY_INIT])
cat >>config.summary <<_ACEOF
AS_HELP_STRING([ $1:], [], 32, 128)
_ACEOF
])

AC_DEFUN([AFS_SUMMARY], [
    AC_REQUIRE([AFS_SUMMARY_INIT])
cat >>config.summary <<_ACEOF
AS_HELP_STRING([  $1:], [$2], 32, 128)
_ACEOF
])

# additional output if ./configure was called with --verbose
AC_DEFUN([AFS_SUMMARY_VERBOSE], [
    AS_IF([test "x${verbose}" = "xyes"], [
        AFS_SUMMARY([$1], [$2])
    ])
])

# should be called after AC_OUTPUT
AC_DEFUN([AFS_SUMMARY_COLLECT], [
    AS_ECHO([""])
    (
    AS_ECHO(["Configure command:"])
    prefix="  $as_myself "
    printf "%-32s" "$prefix"
    padding="                                "
    AS_IF([test ${#prefix} -gt 32], [
        sep="\\$as_nl$padding"
    ], [
        sep=""
    ])

    eval "set x $ac_configure_args"
    shift
    AS_FOR([ARG], [arg], [], [
        case $arg in
        *\'*) arg="`$as_echo "$arg" | sed "s/'/'\\\\\\\\''/g"`" ;;
        esac
        AS_ECHO_N(["$sep'$arg'"])
        sep=" \\$as_nl$padding"
    ])
    AS_ECHO([""])

    AS_ECHO([""])
    sep="Configuration summary:"
    LC_ALL=C find . -name config.summary |
        LC_ALL=C $AWK -F "/" '{print NF, $[]0}' |
        LC_ALL=C sed -e 's/^. /0&/' |
        LC_ALL=C sort |
        while read level summary
    do
        AS_ECHO(["$sep"])
        cat $summary
        sep=""
    done
    ) | tee AC_PACKAGE_TARNAME.summary
])
