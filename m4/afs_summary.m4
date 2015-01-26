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
## Copyright (c) 2009-2013,
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

# AFS_SUMMARY_INIT
# ----------------
# Initializes the summary system and adds the pacakge header (possibly
# including the sub-build name) to it. It removes config.summary files
# from previous configure runs recursively, therefore you need to call
# AFS_SUMMARY_INIT before any sub-configures.
# The sub-build name is used from the `AFS_PACKAGE_BUILD` variable
# set by the AFS_PACAKGE_INIT macro.
AC_DEFUN([AFS_SUMMARY_INIT], [
rm -f AC_PACKAGE_TARNAME.summary
LC_ALL=C find . -name config.summary -exec rm -f '{}' \;
cat >config.summary <<_ACEOF
AS_HELP_STRING(AC_PACKAGE_NAME[]m4_ifdef([AFS_PACKAGE_BUILD], [ (]AFS_PACKAGE_BUILD[):]), [], 32, 128)
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

AC_DEFUN([_AFS_SUMMARY_SHOW], [
    AS_ECHO([""])
    cat AC_PACKAGE_TARNAME.summary
])

# AFS_SUMMARY_COLLECT([SHOW-COND])
# --------------------------------
# Collectes the summary of all configures recusivly into the file
# $PACKAGE.summary. If SHOW-COND is not given, or the expression is
# evaluates to true in an AS_IF the summary is also printed to stdout.
# Should be called after AC_OUTPUT.
AC_DEFUN([AFS_SUMMARY_COLLECT], [
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
        AS_CASE([$arg],
        [*\'*], [arg="`$as_echo "$arg" | sed "s/'/'\\\\\\\\''/g"`"])
        AS_ECHO_N(["$sep'$arg'"])
        sep=" \\$as_nl$padding"
    ])
    AS_ECHO([""])

    AS_ECHO([""])
    sep="Configuration summary:"
    LC_ALL=C find . -name config.summary |
        LC_ALL=C $AWK -F "config.summary" '{print $[]1}' |
        LC_ALL=C sort |
        LC_ALL=C $AWK '{print $[]0 "config.summary"}' |
        while read summary
    do
        AS_ECHO(["$sep"])
        cat $summary
        sep=""
    done
    ) >AC_PACKAGE_TARNAME.summary
    m4_ifblank($1,
              [_AFS_SUMMARY_SHOW],
              [AS_IF([$1], [_AFS_SUMMARY_SHOW])])
])
