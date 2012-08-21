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

## file       ac_scorep_cuda.m4
## maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>


AC_DEFUN([AC_SCOREP_CUDA], [
AC_SCOREP_BACKEND_LIB([libcuda], [cuda.h])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBCUDA_LIB_CHECK], [
scorep_cuda_lib_name="cuda"
scorep_cudart_lib_name="cudart"
scorep_cuda_error="no"

dnl checking for CUDA library
AS_IF([test "x$scorep_cuda_error" = "xno"],
      [AC_SEARCH_LIBS([cuInit],
                      [$scorep_cuda_lib_name],
                      [],
                      [AC_MSG_NOTICE([error: no libcuda found; check path to CUDA library ...])
                       scorep_cuda_error="yes" ])])

dnl check the version of the CUDA Driver API
AS_IF([test "x$scorep_cuda_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cuda.h"]],
        [[
#ifndef CUDA_VERSION
#  error "CUDA_VERSION not defined"
#elif CUDA_VERSION < 4010
#  error "CUDA_VERSION < 4010"
#endif
        ]])],
        [],
        [AC_MSG_NOTICE([error: CUDA driver API version could not be determined and/or is 
                        incompatible (< 4.1)	See 'config.log' for more details.])
         scorep_cuda_error="yes" ])])

dnl checking for CUDA runtime library
AS_IF([test "x$scorep_cuda_error" = "xno"],
      [AC_SEARCH_LIBS([cudaRuntimeGetVersion],
                      [$scorep_cudart_lib_name],
                      [],
                      [AC_MSG_NOTICE([error: no libcuda found; check path to CUDA runtime library ...])
                       scorep_cuda_error="yes" ])])

dnl check the version of the CUDA runtime API
AS_IF([test x"$scorep_cuda_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cuda_runtime_api.h"]],
        [[
#ifndef CUDART_VERSION
#  error "CUDART_VERSION not defined"
#elif CUDART_VERSION < 4010
#  error "CUDART_VERSION < 4010"
#endif
        ]])],
        [],
        [AC_MSG_NOTICE([error: CUDA runtime API version could not be determined and/or is 
                        incompatible (< 4.1)	See 'config.log' for more details.])
         scorep_cuda_error="yes" ])])

dnl final check for errors
if test "x${scorep_cuda_error}" = "xno"; then
    with_$1_lib_checks_successful="yes"
    with_$1_libs="-l${scorep_cuda_lib_name} -l${scorep_cudart_lib_name}"
else
    with_$1_lib_checks_successful="no"
    with_$1_libs=""
fi
])
