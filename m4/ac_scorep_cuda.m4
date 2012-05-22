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

dnl Don't check for CUDA on the frontend.
AS_IF([test "x$ac_scorep_backend" = xno], 
      [AC_MSG_ERROR([cannot check for CUDA on frontend.])])

scorep_cuda_error="no"
scorep_have_cuda="no"

AC_ARG_WITH([cuda],
            [AS_HELP_STRING([--with-cuda=<path-to-cuda-toolkit-directory>], 
                            [If CUDA toolkit is not installed in the default location, specify the directory where it can be found.])],
            [
                AS_IF([test x"${withval}" != xyes -a x"${withval}" != xno], 
                      [scorep_cuda_dir="${withval}"])
            ] # action-if-given
)

AC_ARG_WITH([cuda-include],
            [AS_HELP_STRING([--with-cuda-include=<path-to-cuda-toolkit-include-directory>], 
                            [If the CUDA headers are not installed in the default location ($CUDA_ROOT/include), specify the directory where it can be found.])],
            [scorep_cuda_inc_dir="${withval}"], # action-if-given.
            [
                AS_IF([test x"$scorep_cuda_dir" != x], 
                      [scorep_cuda_inc_dir="$scorep_cuda_dir"/include])
            ] # action-if-not-given
)

AC_ARG_WITH([cuda-lib],
            [AS_HELP_STRING([--with-cuda-lib=<path-to-cuda-toolkit-library-directory>], 
                            [If libcudart.* is not installed in the default location ($CUDA_ROOT/lib64), specify the directory where it can be found.])],
            [scorep_cuda_lib_dir="${withval}"], # action-if-given
            [
                AS_IF([test x"$scorep_cuda_dir" != x], 
                      [scorep_cuda_lib_dir="$scorep_cuda_dir"/lib64])
            ] # action-if-not-given
)

AS_IF([test "x$with_cuda" != xno],
[
dnl use C as configure check language
  AC_LANG_PUSH([C])

dnl save CPPFLAGS and LIBS and set path, if available
  cppflags_save="$CPPFLAGS"
  AS_IF([test x"$scorep_cuda_inc_dir" != x], 
        [CPPFLAGS="-I$scorep_cuda_inc_dir $CPPFLAGS"])
  libs_save="$LIBS"
  AS_IF([test x"$scorep_cuda_lib_dir" != x], 
        [LIBS="$LIBS -L$scorep_cuda_lib_dir"])

dnl check the available CUDA header
  AC_CHECK_HEADER([cuda.h], [], [
    AC_MSG_NOTICE([error: no cuda.h found; check path to CUDA headers ...])
    scorep_cuda_error="yes"
  ])

dnl checking for CUDA library
  AS_IF([test x"$scorep_cuda_error" = "xno"], [
    scorep_cuda_lib_name="cuda"
    AC_SEARCH_LIBS([cuInit], [$scorep_cuda_lib_name], [], [
      AC_MSG_NOTICE([error: no libcuda found; check path to CUDA library ...])
      scorep_cuda_error="yes"
    ])
  ])

dnl check the version of the CUDA Driver API
  AS_IF([test x"$scorep_cuda_error" = "xno"], [
    AC_COMPILE_IFELSE([
      AC_LANG_PROGRAM(
        [[#include "cuda.h"]],
        [[
#ifndef CUDA_VERSION
#  error "CUDA_VERSION not defined"
#elif CUDA_VERSION < 4010
#  error "CUDA_VERSION < 4010"
#endif
        ]])],[],
        [
          AC_MSG_NOTICE([error: CUDA driver API version could not be determined and/or is 
                        incompatible (< 4.1)	See 'config.log' for more details.])
          scorep_cuda_error="yes"
    ])
  ])

dnl checking for CUDA runtime library
  AS_IF([test x"$scorep_cuda_error" = "xno"], [
    scorep_cudart_lib_name="cudart"
    AC_SEARCH_LIBS([cudaRuntimeGetVersion], [$scorep_cudart_lib_name], [], [
      AC_MSG_NOTICE([error: no libcuda found; check path to CUDA runtime library ...])
      scorep_cuda_error="yes"
    ])
  ])

dnl check the version of the CUDA runtime API
  AS_IF([test x"$scorep_cuda_error" = "xno"], [
    AC_COMPILE_IFELSE([
      AC_LANG_PROGRAM(
        [[#include "cuda_runtime_api.h"]],
        [[
#ifndef CUDART_VERSION
#  error "CUDART_VERSION not defined"
#elif CUDART_VERSION < 4010
#  error "CUDART_VERSION < 4010"
#endif
        ]])],[],
        [
          AC_MSG_NOTICE([error: CUDA runtime API version could not be determined and/or is 
                        incompatible (< 4.1)	See 'config.log' for more details.])
          scorep_cuda_error="yes"
    ])
  ])

dnl restore CPPFLAGS and LIBS
  LIBS="$libs_save"
  CPPFLAGS="$cppflags_save"
  AC_LANG_POP([C])

dnl final check for errors
  if test "x${scorep_cuda_error}" = "xno"; then
    scorep_have_cuda="yes"
  fi

dnl generating output
  AC_MSG_CHECKING([for CUDA support])
  AC_MSG_RESULT([$scorep_have_cuda])

dnl generating results
  if test "x${scorep_have_cuda}" = "xyes"; then
      AC_DEFINE([HAVE_CUDA], [1],     [Defined if CUDA is available.])
      AS_IF([test x"$scorep_cuda_lib_dir" != x],
      [
        AC_SUBST([SCOREP_CUDA_LDFLAGS], [-L${scorep_cuda_lib_dir}])
      ],[
        AC_SUBST([SCOREP_CUDA_LDFLAGS], [""])
      ])
      AC_SUBST([SCOREP_CUDA_LIBS],    ["-l${scorep_cuda_lib_name} -l${scorep_cudart_lib_name}"])
  else
      AC_SUBST([SCOREP_CUDA_LDFLAGS], [""])
      AC_SUBST([SCOREP_CUDA_LIBS],    [""])
  fi

  AC_SUBST([SCOREP_CUDA_CPPFLAGS],[$scorep_cuda_cppflags])
  AC_SUBST([SCOREP_CUDA_DIR],  [$scorep_cuda_dir])
  AC_SUBST([SCOREP_CUDA_LIBDIR],  [$scorep_cuda_lib_dir])
  AC_SUBST([SCOREP_CUDA_INCDIR],  [$scorep_cuda_inc_dir])
])
AM_CONDITIONAL([HAVE_CUDA],     [test "x${scorep_have_cuda}" = "xyes"])
AC_SUBST([SCOREP_CUDA_ENABLE],  [$scorep_have_cuda])
])
