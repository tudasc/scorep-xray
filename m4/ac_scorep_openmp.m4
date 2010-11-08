## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##


## file       ac_scorep_openmp.m4
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

AC_DEFUN([AC_SCOREP_OPENMP],
[
AC_LANG_PUSH([C])
AC_OPENMP
AC_LANG_POP([C])
AM_CONDITIONAL([OPENMP_SUPPORTED], 
               [test "x${ac_cv_prog_c_openmp}" != "xunsupported"])

if test "x${ac_cv_prog_c_openmp}" = "xunsupported"; then
  AC_MSG_WARN([Non suitbale OpenMP compilers found. SCOREP OpenMP and hybrid libraries will not be build.])
fi

AC_LANG_PUSH([C++])
AC_OPENMP
AC_LANG_POP([C++])

AC_LANG_PUSH([Fortran 77])
AC_OPENMP
AC_LANG_POP([Fortran 77])

AC_LANG_PUSH([Fortran])
AC_OPENMP
AC_LANG_POP([Fortran])
])
