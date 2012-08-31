## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen University, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       ac_scorep_summary.m4
## maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>

# AC_SCOREP_SUMMARY_INIT([SHORT-DESCRIPTION])
# ___________________________________________
AC_DEFUN([AC_SCOREP_SUMMARY_INIT], [
cat >config.summary <<_ACEOF
AS_HELP_STRING(m4_ifval([$1], [$PACKAGE_NAME ($1):], [$PACKAGE_NAME:]), [], 32, 128)
_ACEOF
])

AC_DEFUN([AC_SCOREP_SUMMARY_SECTION], [
    AC_REQUIRE([AC_SCOREP_SUMMARY_INIT])
cat >>config.summary <<_ACEOF
AS_HELP_STRING([ $1:], [], 32, 128)
_ACEOF
])

AC_DEFUN([AC_SCOREP_SUMMARY], [
    AC_REQUIRE([AC_SCOREP_SUMMARY_INIT])
cat >>config.summary <<_ACEOF
AS_HELP_STRING([  $1:], [$2], 32, 128)
_ACEOF
])

# additional output if ./configure was called with --verbose
AC_DEFUN([AC_SCOREP_SUMMARY_VERBOSE], [
    AS_IF([test "x${verbose}" = "xyes"], [
        AC_SCOREP_SUMMARY(["$1"], ["$2"])
    ])
])

# should be called after AC_OUTPUT
AC_DEFUN([AC_SCOREP_SUMMARY_COLLECT], [
    (
    AS_ECHO(["Configuration summary:"])
    AS_IF([test -f config.summary], [
        cat config.summary
    ])
    find */ -depth -name config.summary | while read summary; do
        AS_ECHO([""])
        cat $summary
    done
    ) >$PACKAGE.summary
    AS_ECHO([""])
    cat $PACKAGE.summary
])
