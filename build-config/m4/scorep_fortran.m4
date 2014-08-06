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
## Copyright (c) 2009-2012, 2014,
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
AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [mangling=" [##] _"],
    [sun],      [mangling=" [##] _"],
    [ibm],      [mangling=],
    [portland], [mangling=" [##] _"],
    [gnu],      [mangling=" [##] _"],
    [cray],     [mangling=" [##] _"],
    [fujitsu],  [mangling=" [##] _"],
    [])dnl
AC_DEFINE_UNQUOTED([FORTRAN_MANGLED(var)],
    [var${mangling}],
    [Name of var after mangled by the Fortran compiler.])dnl
])


AC_DEFUN([SCOREP_FORTRAN_DEFAULT_INTEGER_SIZE],[
    scorep_fortran_integer_default_size="8"
    AC_REQUIRE([AC_PROG_F77])
    AC_MSG_CHECKING([the fortran default integer size])
    AC_LANG_PUSH([Fortran 77])
    scorep_fortran_default_integer_size_fflags_save=${FFLAGS}
    FFLAGS="$FFLAGS -Werror -Wall"
    AC_COMPILE_IFELSE([
      program hello
      implicit none
      integer         :: var
      integer(kind=8) :: var8
      var8 = 77
      var  = var8
      end
                   ],
                   [],
                   [scorep_fortran_integer_default_size="4"])
    FFLAGS=${scorep_fortran_default_integer_size_fflags_save}
    AC_LANG_POP([Fortran 77])
    AM_CONDITIONAL([SCOREP_HAVE_FORTRAN_4BYTE_INTEGER_DEFAULT_SIZE], [test "x${scorep_fortran_integer_default_size}" = "x4"])
    AM_CONDITIONAL([SCOREP_HAVE_FORTRAN_8BYTE_INTEGER_DEFAULT_SIZE], [test "x${scorep_fortran_integer_default_size}" = "x8"])
    AC_MSG_RESULT([${scorep_fortran_integer_default_size}])
])


AC_DEFUN([SCOREP_FORTRAN_DEFAULT_REAL_SIZE],[
    scorep_fortran_real_default_size="8"
    AC_REQUIRE([AC_PROG_F77])
    AC_MSG_CHECKING([the fortran default real size])
    AC_LANG_PUSH([Fortran 77])
    scorep_fortran_default_real_size_fflags_save=${FFLAGS}
    FFLAGS="$FFLAGS -Werror -Wall"
    AC_COMPILE_IFELSE([
      program hello
      implicit none
      real         :: var
      real(kind=8) :: var8
      var8 = 77
      var  = var8
      end
                   ],
                   [],
                   [scorep_fortran_real_default_size="4"])
    FFLAGS=${scorep_fortran_default_real_size_fflags_save}
    AC_LANG_POP([Fortran 77])
    AM_CONDITIONAL([SCOREP_HAVE_FORTRAN_4BYTE_REAL_DEFAULT_SIZE], [test "x${scorep_fortran_real_default_size}" = "x4"])
    AM_CONDITIONAL([SCOREP_HAVE_FORTRAN_8BYTE_REAL_DEFAULT_SIZE], [test "x${scorep_fortran_real_default_size}" = "x8"])
    AC_MSG_RESULT([${scorep_fortran_real_default_size}])
])


AC_DEFUN([SCOREP_FORTRAN_DEFAULT_DOUBLE_SIZE],[
    scorep_fortran_double_default_size="8"
    AC_REQUIRE([AC_PROG_F77])
    AC_MSG_CHECKING([the fortran default double size])
    AC_LANG_PUSH([Fortran 77])
    scorep_fortran_default_double_size_fflags_save=${FFLAGS}
    FFLAGS="$FFLAGS -Werror -Wall"
    AC_COMPILE_IFELSE([
      program hello
      double precision :: var
      real(kind=8)     :: var8
      var  = 77
      var8 = var
      end
                   ],
                   [],
                   [scorep_fortran_double_default_size="16"])
    FFLAGS=${scorep_fortran_default_double_size_fflags_save}
    AC_LANG_POP([Fortran 77])
    AM_CONDITIONAL([SCOREP_HAVE_FORTRAN_4BYTE_DOUBLE_DEFAULT_SIZE], [test "x${scorep_fortran_double_default_size}" = "x8"])
    AM_CONDITIONAL([SCOREP_HAVE_FORTRAN_8BYTE_DOUBLE_DEFAULT_SIZE], [test "x${scorep_fortran_double_default_size}" = "x16"])
    AC_MSG_RESULT([${scorep_fortran_double_default_size}])
])
