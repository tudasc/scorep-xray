## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen, Germany
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
## Copyright (c) 2009-2012,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_cuda.m4

dnl ----------------------------------------------------------------------------

dnl The Score-P CUDA adapter is dependent on the CUDA driver as well as
dnl the CUDA Toolkit (http://developer.nvidia.com/cuda/cuda-toolkit). From
dnl the driver we need the library libcuda and we need to check the
dnl drivers's API version. This version is encoded in the header cuda.h,
dnl which is unfortunately not installed with the driver but with the CUDA
dnl Toolkit. I.e. the user needs to take care of installing compatible
dnl versions of the driver (usually comes with the OS distribution) and
dnl the toolkit. From the toolkit we need two things. First, the cuda
dnl runtime library libcudart and the header cuda_runtime_api.h to check
dnl the runtime API version. Second, the library libcupti and the
dnl corresponding header cupti.h to check the cupti API version. Cupti
dnl comes with the toolkit and is located under <toolkit>/extras/CUPTI. As
dnl the driver and the toolkit can be installed separatly, we provide the
dnl user with the options --with-libcudart and --with-libcuda. There is no
dnl need for a --with-libcupti as cupti resides within the toolkit
dnl installation.

AC_DEFUN([AC_SCOREP_CUDA], [
AFS_SUMMARY_PUSH

scorep_have_cuda="no"
scorep_have_cupti4="no"
scorep_have_cupti_activity_async="no"
scorep_cuda_version="unknown"
scorep_cuda_version_greater_equal_60="no"

ac_scorep_cuda_safe_CPPFLAGS=$CPPFLAGS
ac_scorep_cuda_safe_LDFLAGS=$LDFLAGS
ac_scorep_cuda_safe_LIBS=$LIBS

AC_SCOREP_BACKEND_LIB([libcudart], [cuda.h cuda_runtime_api.h])
CPPFLAGS="$CPPFLAGS ${with_libcudart_cppflags}"
LDFLAGS="$LDFLAGS ${with_libcudart_ldflags}"
LIBS="$LIBS ${with_libcudart_libs}"

AC_SCOREP_BACKEND_LIB([libcuda])
CPPFLAGS="$CPPFLAGS ${with_libcuda_cppflags}"
LDFLAGS="$LDFLAGS ${with_libcuda_ldflags}"
LIBS="$LIBS ${with_libcuda_libs}"

AS_UNSET([cupti_root])
AS_IF([test "x${with_libcudart_lib}" = "xyes"],
      [for path in ${sys_lib_search_path_spec}; do
           AS_IF([test -e ${path}/libcudart.a || test -e ${path}/libcudart.so || test -e ${path}/libcudart.dylib],
                 [AS_IF([test -d "${path}/../extras/CUPTI"],
                        [cupti_root="${path}/../extras/CUPTI"],
                        [cupti_root="${path}"])
                  break])
       done],
      [AS_IF([test "x${with_libcudart}" != "xnot_set"],
             [cupti_root="${with_libcudart}/extras/CUPTI"])])

AC_SCOREP_BACKEND_LIB([libcupti], [cupti.h], [${with_libcudart_cppflags}], [${cupti_root}])

CPPFLAGS=$ac_scorep_cuda_safe_CPPFLAGS
LDFLAGS=$ac_scorep_cuda_safe_LDFLAGS
LIBS=$ac_scorep_cuda_safe_LIBS

AS_IF([test "x${scorep_have_libcudart}" = "xyes" && test "x${scorep_have_libcupti}"  = "xyes" && test "x${scorep_have_libcuda}"   = "xyes"],
      [scorep_have_cuda=yes])
AC_ARG_ENABLE([cuda],
              [AS_HELP_STRING([--enable-cuda],
                              [Enable or disable support for CUDA. Fails if support cannot be satisfied but was requested.])],
              [AS_CASE([$enableval,$scorep_have_cuda],
                       [yes,no],
                       [AC_MSG_ERROR([could not fulfill requested support for CUDA.])],
                       [no,yes],
                       [scorep_have_cuda="no (disabled by request)"],
                       [yes,yes|no,no],
                       [:],
                       [AC_MSG_ERROR([Invalid argument for --enable-cuda: $enableval])])])

AC_SCOREP_COND_HAVE([CUDA_SUPPORT],
                    [test "x${scorep_have_cuda}" = "xyes"],
                    [Defined if cuda is available.],
                    [AC_SUBST(CUDA_CPPFLAGS, ["${with_libcudart_cppflags} ${with_libcupti_cppflags}"])
                     AC_SUBST(CUDA_LDFLAGS,  ["${with_libcuda_ldflags} ${with_libcudart_ldflags} ${with_libcupti_ldflags} ${with_libcuda_rpathflag} ${with_libcudart_rpathflag} ${with_libcupti_rpathflag}"])
                     AC_SUBST(CUDA_LIBS,     ["${with_libcuda_libs} ${with_libcudart_libs} ${with_libcupti_libs}"])],
                    [AC_SUBST(CUDA_CPPFLAGS, [""])
                     AC_SUBST(CUDA_LDFLAGS,  [""])
                     AC_SUBST(CUDA_LIBS,     [""])])

AC_SCOREP_COND_HAVE([CUPTI_ASYNC_SUPPORT],
                    [test "x${scorep_have_cupti_activity_async}" = "xyes"],
                    [Defined if CUPTI activity asynchronous buffer handling is available.]
                   )

AC_SCOREP_COND_HAVE([CUDA_VERSION_GREATER_EQUAL_60],
                    [test "x${scorep_cuda_version_greater_equal_60}" = "xyes"],
                    [Defined if CUDA version is greater or equal 6.0.]
                   )

AFS_SUMMARY([CUPTI async support], [${scorep_have_cupti_activity_async}])

AFS_SUMMARY([CUDA version >= 6.0], [${scorep_cuda_version_greater_equal_60}])

AFS_SUMMARY_POP([CUDA support], [${scorep_have_cuda}])

AC_SUBST(SCOREP_CUDA_VERSION,      ["${scorep_cuda_version}"])

# prepare compiler variables for use with nvcc in instrumenter checks
# all flags are passed as comma-separated list after -Xcompiler
AM_COND_IF([HAVE_CUDA_SUPPORT], [
    SCOREP_INSTRUMENTER_CHECK_NVCC_CXX=${CXX%% *}
    SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS=${CXX#* }
    AS_IF([test "x${SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS}" = "x${CXX}"],
        [SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS=],
        [SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS=`
            echo $SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS |
                sed -e 's/ /,/g' -e 's/,,*/,/g' -e 's/^,//' -e 's/,$//'`
         SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS="${SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS:+-Xcompiler=}${SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS}"])
    AC_SUBST([SCOREP_INSTRUMENTER_CHECK_NVCC_CXX])
    AC_SUBST([SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS])
])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBCUDART_LIB_CHECK], [
scorep_cudart_error="no"
scorep_cudart_lib_name="cudart"

dnl checking for CUDA runtime library
AS_IF([test "x$scorep_cudart_error" = "xno"],
      [AC_SEARCH_LIBS([cudaRuntimeGetVersion],
                      [$scorep_cudart_lib_name],
                      [],
                      [AS_IF([test "x${with_libcudart}" != xnot_set || test "x${with_libcudart_lib}" != xnot_set],
                             [AC_MSG_NOTICE([no libcudart found; check path to CUDA runtime library ...])])
                       scorep_cudart_error="yes"])])

dnl check the version of the CUDA runtime API
AS_IF([test "x$scorep_cudart_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cuda_runtime_api.h"]],
        [[
#ifndef CUDART_VERSION
#  ups__cudart_version_not_defined
#elif CUDART_VERSION < 4010
#  ups__cudart_version_lt_4010
#endif
        ]])],
        [],
        [AC_MSG_NOTICE([CUDA runtime API version could not be determined and/or is
                        incompatible (< 4.1). See 'config.log' for more details.])
         scorep_cudart_error="yes" ])])


dnl final check for errors
if test "x${scorep_cudart_error}" = "xno"; then
    with_$1_lib_checks_successful="yes"
    with_$1_libs="-l${scorep_cudart_lib_name}"
else
    with_$1_lib_checks_successful="no"
    with_$1_libs=""
fi
])

dnl --------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBCUPTI_LIB_CHECK], [
scorep_cupti_error="no"
scorep_cupti_lib_name="cupti"
ldflags_save="${LDFLAGS}"
LDFLAGS="${LDFLAGS} ${with_libcudart_ldflags} -l${scorep_cudart_lib_name}"

dnl checking for CUPTI library
AS_IF([test "x$scorep_cupti_error" = "xno"],
      [AC_CHECK_LIB([$scorep_cupti_lib_name],
                    [cuptiGetVersion],
                    [],
                    [AS_IF([test "x${with_libcupti}" != xnot_set || test "x${with_libcupti_lib}" != xnot_set],
                           [AC_MSG_NOTICE([no libcupti found; check path to CUPTI library ...])])
                     scorep_cupti_error="yes"])])
LDFLAGS="${ldflags_save}"

dnl check the version of CUPTI
AS_IF([test "x$scorep_cupti_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cupti.h"]],
        [[
#ifndef CUPTI_API_VERSION
#  ups__cupti_api_version_not_defined
#elif CUPTI_API_VERSION < 2
#  ups__cupti_api_version_lt_2
#endif
         ]])],
         [
            AS_IF([test "x$scorep_cupti_error" = "xno"],
                  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cupti.h"]],
                    [[
#if CUPTI_API_VERSION < 4
#  ups__cupti_api_version_lt_4
#endif
                     ]])],
                     [AC_MSG_NOTICE([Check for CUPTI 4 was successful.])
                      scorep_have_cupti4="yes"],
                     [])])
         ],
         [AC_MSG_NOTICE([CUPTI version could not be determined and/or is
                         incompatible (< 2). See 'config.log' for more details.])
          scorep_cupti_error="yes" ])])

dnl check for CUPTI activity asynchronous buffer handling
AS_IF([test "x$scorep_have_cupti4" = "xyes"],
  [
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <cupti.h>
]],[[
CUpti_BuffersCallbackRequestFunc funcBufferRequested;
CUpti_BuffersCallbackCompleteFunc funcBufferCompleted;
cuptiActivityRegisterCallbacks(funcBufferRequested, funcBufferCompleted);
]])],
      [scorep_have_cupti_activity_async="yes"],
      [scorep_cupti_error="yes"])
  ],[])

dnl final check for errors
if test "x${scorep_cupti_error}" = "xno"; then
    with_$1_lib_checks_successful="yes"
    with_$1_libs="-l${scorep_cupti_lib_name}"
else
    with_$1_lib_checks_successful="no"
    with_$1_libs=""
fi
])

dnl --------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBCUDA_LIB_CHECK], [
scorep_cuda_lib_name="cuda"
scorep_cuda_error="no"

dnl checking for CUDA library
AS_IF([test "x$scorep_cuda_error" = "xno"],
      [AC_SEARCH_LIBS([cuInit],
                      [$scorep_cuda_lib_name],
                      [],
                      [AS_IF([test "x${with_libcuda}" != xnot_set || test "x${with_libcuda_lib}" != xnot_set],
                             [AC_MSG_NOTICE([no libcuda found; check path to CUDA library ...])])
                       scorep_cuda_error="yes"])])

dnl check the version of the CUDA Driver API
AS_IF([test "x$scorep_cuda_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cuda.h"]],
        [[
#ifndef CUDA_VERSION
#  ups__cuda_version_not_defined
#elif CUDA_VERSION < 4010
#  ups__cuda_version_lt_4010
#endif
        ]])],
        [],
        [AC_MSG_NOTICE([CUDA driver API version could not be determined and/or is
                        incompatible (< 4.1). See 'config.log' for more details.])
         scorep_cuda_error="yes" ])])

dnl check for CUDA version 5.0.x
AS_IF([test "x$scorep_cuda_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cuda.h"]],
        [[
#if ( CUDA_VERSION != 5000 )
#  ups__cuda_version_is_not_5_0_x
#endif
        ]])],
        [AC_MSG_NOTICE([CUDA driver API version is 5.0.])
         scorep_cuda_version="50"],
        [])])

dnl check for CUDA version 5.5.x
AS_IF([test "x$scorep_cuda_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cuda.h"]],
        [[
#if ( CUDA_VERSION != 5050 )
#  ups__cuda_version_is_not_5_5_x
#endif
        ]])],
        [AC_MSG_NOTICE([CUDA driver API version is 5.5.])
         scorep_cuda_version="55"],
        [])])

dnl check for CUDA version >= 6.0
AS_IF([test "x$scorep_cuda_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cuda.h"]],
        [[
#if CUDA_VERSION < 6000
#  ups__cuda_version_lt_6000
#endif
        ]])],
        [scorep_cuda_version_greater_equal_60="yes"],
        [AC_MSG_NOTICE([CUDA driver API version is less than 6.0.
                        See 'config.log' for more details.])
         scorep_cuda_version_greater_equal_60="no"])])

dnl check for CUDA version 6.0.x
AS_IF([test "x$scorep_cuda_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cuda.h"]],
        [[
#if ( CUDA_VERSION != 6000 )
#  ups__cuda_version_is_not_6_0_x
#endif
        ]])],
        [AC_MSG_NOTICE([CUDA driver API version is 6.0.])
         scorep_cuda_version="60"],
        [])])

dnl check for CUDA version 6.5.x
AS_IF([test "x$scorep_cuda_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cuda.h"]],
        [[
#if ( CUDA_VERSION != 6050 )
#  ups__cuda_version_is_not_6_5_x
#endif
        ]])],
        [AC_MSG_NOTICE([CUDA driver API version is 6.5.])
         scorep_cuda_version="65"],
        [])])

dnl final check for errors
if test "x${scorep_cuda_error}" = "xno"; then
    with_$1_lib_checks_successful="yes"
    with_$1_libs="-l${scorep_cuda_lib_name}"
else
    with_$1_lib_checks_successful="no"
    with_$1_libs=""
fi
])
