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

pushdef([UP], translit([$1], [a-z], [A-Z]))dnl
pushdef([DOWN], translit([$1], [A-Z], [a-z]))dnl

m4_ifval([$2], [], [
    _AC_SRCDIRS([.])
    AC_DEFINE_UNQUOTED([PACKAGE_SRCDIR],   ["$ac_abs_srcdir/.."], [Source dir])
    AC_DEFINE_UNQUOTED([PACKAGE_BUILDDIR], ["$ac_abs_builddir"],  [Build dir])

    AC_DEFINE_UNQUOTED(
        [PACKAGE_SYM],
        DOWN,
        [The package name usable as a symbol.])

    AC_DEFINE_UNQUOTED(
        [PACKAGE_SYM_CAPS],
        UP,
        [The package name usable as a symbol in all caps.])
])

AC_SUBST([PACKAGE_SYM], DOWN)
AC_SUBST([PACKAGE_SYM_CAPS], UP)

popdef([UP])
popdef([DOWN])
])
