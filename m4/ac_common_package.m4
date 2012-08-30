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

m4_define([$1_UP], [m4_toupper($1)])
m4_define([$1_DOWN], [m4_tolower($1)])

m4_ifval([$2], [], [
    _AC_SRCDIRS([.])
    AC_DEFINE_UNQUOTED([PACKAGE_SRCDIR],   ["$ac_abs_srcdir/.."], [Source dir])
    AC_DEFINE_UNQUOTED([PACKAGE_BUILDDIR], ["$ac_abs_builddir"],  [Build dir])

    AC_DEFINE_UNQUOTED(
        [PACKAGE_SYM],
        $1_DOWN,
        [The package name usable as a symbol.])

    AC_DEFINE_UNQUOTED(
        [PACKAGE_SYM_CAPS],
        $1_UP,
        [The package name usable as a symbol in all caps.])
])

AC_SUBST([PACKAGE_SYM],      $1_DOWN)
AC_SUBST([PACKAGE_SYM_CAPS], $1_UP)
])

# AC_SCOREP_DEFINE_HAVE(VARIABLE, VALUE, [DESCRIPTION])
# ------------------------------------------------------
# Like AC_DEFINE, but prepends the HAVE_ prefix and also defines the
# HAVE_BACKEND_ variant, if in cross mode.
#
AC_DEFUN([AC_SCOREP_DEFINE_HAVE], [
AC_DEFINE(HAVE_[]$1, [$2], [$3])
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"], [
AC_DEFINE(HAVE_BACKEND_[]$1, [$2], [$3])
])
])
