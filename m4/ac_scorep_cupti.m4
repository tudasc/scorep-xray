## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       ac_scorep_cupti.m4
## maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>


AC_DEFUN([AC_SCOREP_CUPTI], [
              
AC_REQUIRE([AC_SCOREP_CUDA])

AS_IF([test "x$scorep_have_libcuda" = "xyes"],
      [AS_UNSET([cupti_root])
       AS_IF([test -n ${with_libcuda}],
             [AS_IF([test "x${with_libcuda}" != "xyes"],
                    [AS_IF([test "x${with_libcuda}" != "xnot_set"],
                           [cupti_root=${with_libcuda}/extras/CUPTI],
                           [])],
                    [])],
             [])
       AC_SCOREP_BACKEND_LIB([libcupti], [cupti.h], [${with_libcuda_cppflags}], [cupti_root])
       AS_IF([test "x${scorep_have_libcupti}" = "xyes"],
             [AM_CONDITIONAL([HAVE_CUPTI], [test "x${scorep_have_libcupti}" = "xyes"])],
             [AM_CONDITIONAL([HAVE_CUPTI], [test 1 -eq 0])
              AM_CONDITIONAL([HAVE_LIBCUPTI], [test 1 -eq 0]) ])],
      [AM_CONDITIONAL([HAVE_CUPTI], [test 1 -eq 0])
       AM_CONDITIONAL([HAVE_LIBCUPTI], [test 1 -eq 0])])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBCUPTI_LIB_CHECK], [
scorep_cupti_error="no"
scorep_cupti_lib_name="cupti"

dnl checking for CUPTI library
AS_IF([test "x$scorep_cupti_error" = "xno"],
      [AC_CHECK_LIB([$scorep_cupti_lib_name],
                    [cuptiGetVersion],
                    [],
                    [AC_MSG_NOTICE([error: no libcupti found; check path to CUPTI library ...])
                     scorep_cupti_error="yes" ])])
                     
dnl check the version of CUPTI
AS_IF([test "x$scorep_cupti_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cupti.h"]],
        [[
#ifndef CUPTI_API_VERSION
#  error "CUPTI_API_VERSION not defined"
#elif CUPTI_API_VERSION < 2
#  error "CUPTI_API_VERSION < 2"
#endif
         ]])],
         [],
         [AC_MSG_NOTICE([error: CUPTI version could not be determined and/or is 
                         incompatible (< 2)	See 'config.log' for more details.])
          scorep_cupti_error="yes" ])])

dnl final check for errors
if test "x${scorep_cupti_error}" = "xno"; then
    with_$1_lib_checks_successful="yes"
    with_$1_libs="-l${scorep_cupti_lib_name}"
else
    with_$1_lib_checks_successful="no"
    with_$1_libs=""
fi
])
