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
## Copyright (c) 2009-2012, 2014, 2018-2019,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012, 2014,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_fortran.m4

dnl ------------------------------------------------------------------

AC_DEFUN([SCOREP_FORTRAN_MANGLED], [
AC_REQUIRE([AX_COMPILER_VENDOR])dnl
mangling=
AS_IF([test "x${scorep_cv_f77_works}" = "xyes"], [
    AS_CASE([${ax_cv_c_compiler_vendor%/*}],
        [intel],    [mangling=" [##] _"],
        [sun],      [mangling=" [##] _"],
        [ibm],      [mangling=],
        [portland], [mangling=" [##] _"],
        [gnu],      [mangling=" [##] _"],
        [cray],     [mangling=" [##] _"],
        [fujitsu],  [mangling=" [##] _"],
        [])dnl
])
AC_DEFINE_UNQUOTED([FORTRAN_MANGLED(var)],
    [var${mangling}],
    [Name of var after mangled by the Fortran compiler.])dnl
])


AC_DEFUN([SCOREP_FORTRAN_F77_WORKS],[
    AC_REQUIRE([AC_PROG_F77])
    AC_CACHE_CHECK([whether the Fortran 77 compiler works],
               [scorep_cv_f77_works],
               [AC_LANG_PUSH([Fortran 77])
                AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
                               [scorep_cv_f77_works=yes],
                               [scorep_cv_f77_works=no
                                F77=no])
                AC_LANG_POP([Fortran 77])])
    AM_CONDITIONAL([SCOREP_HAVE_F77], [test "x${scorep_cv_f77_works}" = "xyes"])
])

AC_DEFUN([SCOREP_FORTRAN_FC_WORKS],[
    AC_REQUIRE([AC_PROG_FC])
    AC_CACHE_CHECK([whether the Fortran compiler works (FC)],
               [scorep_cv_fc_works],
               [AC_LANG_PUSH([Fortran])
                AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
                               [scorep_cv_fc_works=yes],
                               [scorep_cv_fc_works=no
                                FC=no])
                AC_LANG_POP([Fortran])])
    AM_CONDITIONAL([SCOREP_HAVE_FC], [test "x${scorep_cv_fc_works}" = "xyes"])
])
