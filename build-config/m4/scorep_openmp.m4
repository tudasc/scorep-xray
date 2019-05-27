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
## Copyright (c) 2009-2011, 2019,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2011, 2013,
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

## file build-config/m4/scorep_openmp.m4


AC_DEFUN([AC_SCOREP_OPENMP],
[
# set *FLAGS temporarily to "" because the cray compiler ignores
# the OpenMP flags if -g is set (which is done per default by configure)

AC_LANG_PUSH([C])
scorep_cflags_save=${CFLAGS}
CFLAGS=""
AC_OPENMP
CFLAGS=${scorep_cflags_save}
AC_LANG_POP([C])
AM_CONDITIONAL([HAVE_OPENMP_SUPPORT],
               [test "x${ac_cv_prog_c_openmp}" != "xunsupported" && test "x${enable_openmp}" != "xno"])

if test "x${enable_openmp}" != "xno"; then
  if test "x${ac_cv_prog_c_openmp}" = "xunsupported"; then
    AC_MSG_WARN([no suitable OpenMP compilers found. OpenMP and hybrid libraries will not be build.])
    AFS_SUMMARY([OpenMP support], [no, no suitable compilers found])
  else
    AFS_SUMMARY([OpenMP support], [yes, using ${ac_cv_prog_c_openmp}])
  fi
else
  AFS_SUMMARY([OpenMP support], [disabled])
fi


# On Hermit/CRAYXT, the C++ OpenMP check fails because
# of a broken CC. Set OPENMP_CXXFLAGS manually. See
# also silc:#687.
AS_IF([(test "x${afs_platform_cray}" = "xyes" && \
       test "x${ax_cv_c_compiler_vendor}" = xportland)],
      [AC_SUBST([OPENMP_CXXFLAGS], [-mp])],
      [AC_LANG_PUSH([C++])
       scorep_cxxflags_save=${CXXFLAGS}
       CXXFLAGS=""
       AC_OPENMP
       CXXFLAGS=${scorep_cxxflags_save}
       AC_LANG_POP([C++])])

AC_LANG_PUSH([Fortran 77])
scorep_fflags_save=${FFLAGS}
FFLAGS=""
AC_OPENMP
FFLAGS=${scorep_fflags_save}
AC_LANG_POP([Fortran 77])

AC_LANG_PUSH([Fortran])
scorep_fcflags_save=${FCFLAGS}
FCFLAGS=""
AC_OPENMP
FCFLAGS=${scorep_fflags_save}
AC_LANG_POP([Fortran])
])
