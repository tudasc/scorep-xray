## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
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

AC_DEFUN([AC_COMMON_PACKAGE], [
AC_REQUIRE([AM_INIT_AUTOMAKE])
_AC_SRCDIRS([.])
AC_DEFINE_UNQUOTED([PACKAGE_SRCDIR],   ["$ac_abs_srcdir/.."], [Source dir])
AC_DEFINE_UNQUOTED([PACKAGE_BUILDDIR], ["$ac_abs_builddir"],  [Build dir])
AC_DEFINE_UNQUOTED(
    [PACKAGE_SYM],
    [$PACKAGE],
    [The package name usable as a symbol.])
AC_DEFINE_UNQUOTED(
    [PACKAGE_SYM_CAPS],
    $(echo "$PACKAGE" | LC_ALL=C tr '[a-z]' '[A-Z]'),
    [The package name usable as a symbol in all caps.])
])
