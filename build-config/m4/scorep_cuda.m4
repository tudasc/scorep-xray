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
## Copyright (c) 2009-2012, 2014, 2020,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2012, 2020,
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

ac_scorep_cuda_safe_CPPFLAGS=$CPPFLAGS
ac_scorep_cuda_safe_LDFLAGS=$LDFLAGS
ac_scorep_cuda_safe_LIBS=$LIBS

AC_SCOREP_BACKEND_LIB([libcudart], [cuda.h cuda_runtime_api.h])
CPPFLAGS="$CPPFLAGS ${with_libcudart_cppflags}"
LDFLAGS="$LDFLAGS ${with_libcudart_ldflags}"
LIBS="$LIBS ${with_libcudart_libs}"

scorep_cuda_version="unknown"
AC_SCOREP_BACKEND_LIB([libcuda])
CPPFLAGS="$CPPFLAGS ${with_libcuda_cppflags}"
LDFLAGS="$LDFLAGS ${with_libcuda_ldflags}"
LIBS="$LIBS ${with_libcuda_libs}"
AC_SUBST([SCOREP_CUDA_VERSION], [${scorep_cuda_version}])

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
AC_SCOREP_BACKEND_LIB([libnvidia-ml], [nvml.h], [${with_libcudart_cppflags}])

CPPFLAGS=$ac_scorep_cuda_safe_CPPFLAGS
LDFLAGS=$ac_scorep_cuda_safe_LDFLAGS
LIBS=$ac_scorep_cuda_safe_LIBS

AS_IF([test "x${scorep_have_libcudart}" = "xyes" &&
       test "x${scorep_have_libcupti}"  = "xyes" &&
       test "x${scorep_have_libcuda}"   = "xyes"],
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

dnl cannot run CUDA checks, if only stubs-libraries are provided
scorep_have_cuda_tests=${scorep_have_cuda}
AC_MSG_CHECKING([whether libcuda is in a stubs-library])
AS_CASE(["/${with_libcuda_lib}/"],
        [*/stubs/*],
        [AS_UNSET([with_libcuda_rpathflag])
         scorep_have_cuda_tests=no
         AC_MSG_RESULT([yes])],
        [AC_MSG_RESULT([no])])
AC_MSG_CHECKING([whether libnvidia-ml is in a stubs-library])
AS_CASE(["/${with_libnvidia_ml_lib}/"],
        [*/stubs/*],
        [AS_UNSET([with_libnvidia_ml_rpathflag])
         scorep_have_cuda_tests=no
         AC_MSG_RESULT([yes])],
        [AC_MSG_RESULT([no])])

AC_SCOREP_COND_HAVE([CUDA_SUPPORT],
                    [test "x${scorep_have_cuda}" = "xyes"],
                    [Defined if cuda is available.],
                    [AC_SUBST(CUDA_CPPFLAGS, ["${with_libcudart_cppflags} ${with_libcupti_cppflags} ${with_libnvidia_ml_cppflags}"])
                     AC_SUBST(CUDA_LDFLAGS,  ["${with_libcuda_ldflags} ${with_libcudart_ldflags} ${with_libcupti_ldflags} ${with_libnvidia_ml_ldflags} ${with_libcuda_rpathflag} ${with_libcudart_rpathflag} ${with_libcupti_rpathflag} ${with_libnvidia_ml_rpathflag}"])
                     AC_SUBST(CUDA_LIBS,     ["${with_libcuda_libs} ${with_libcudart_libs} ${with_libcupti_libs} ${with_libnvidia_ml_libs}"])],
                    [AC_SUBST(CUDA_CPPFLAGS, [""])
                     AC_SUBST(CUDA_LDFLAGS,  [""])
                     AC_SUBST(CUDA_LIBS,     [""])])

AC_SCOREP_COND_HAVE([CUPTI_ASYNC_SUPPORT],
                    [test "x${scorep_have_cupti_activity_async}" = "xyes"],
                    [Defined if CUPTI activity asynchronous buffer handling is available.]
                   )

AC_SCOREP_COND_HAVE([NVML_SUPPORT],
                    [test "x${scorep_have_libnvidia_ml}" = "xyes"],
                    [Defined if NVIDIA NVML is available.])

AC_SCOREP_COND_HAVE([CUDA_TESTS],
    [test "x${scorep_have_cuda_tests}" = "xyes"],
    [Defined if CUDA tests will be run.], [], [])

dnl run_cuda_test.sh: test sources only available for scorep_cuda_version 5000, 5050, 6000, 6050
AM_CONDITIONAL([HAVE_CUDA_TESTS_HAVE_GOLD],
    [test "x${scorep_have_cuda_tests}" = "xyes" \
     && (test ${scorep_cuda_version} -eq 5000 \
         || test ${scorep_cuda_version} -eq 5050 \
         || test ${scorep_cuda_version} -eq 6000 \
         || test ${scorep_cuda_version} -eq 6050)])

AFS_SUMMARY([CUPTI async support], [${scorep_have_cupti_activity_async}])

AFS_SUMMARY([CUDA version], [${scorep_cuda_version}])

AFS_SUMMARY_POP([CUDA support], [${scorep_have_cuda}])

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

    # nvcc 10.x in conjunction with g++-8.4 and later on powerpc64le:
    # need to set '-std=c++11' or '-Xcompiler -mno-float128' to
    # prevent 'identifier "__ieee128" is undefined' errors. See #21.
    AC_LANG_PUSH([C++])
    CXX_save="${CXX}"
    CXX="nvcc -ccbin ${CXX}"
    CXXFLAGS_save="${CXXFLAGS}"
    ac_ext_save="${ac_ext}"
    ac_ext=cu
    for CXXFLAGS in "" "-Xcompiler=-mno-float128"; do
        AC_COMPILE_IFELSE([AC_LANG_SOURCE([[int main() {return 0;}]])],
            [nvcc_failure=no
             flag=$(echo $CXXFLAGS | cut -d '=' -f 2)
             AS_IF([test "x${flag}" != x],
                 [AS_IF([test "x${SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS}" = x],
                     [SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS="-Xcompiler=${flag}"],
                     [SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS="${SCOREP_INSTRUMENTER_CHECK_CUFLAGS_CXXFLAGS},${flag}"])])
             break],
            [nvcc_failure=yes])
    done
    AS_IF([test "x${nvcc_failure}" = xyes],
        [AC_MSG_WARN([nvcc compilation failed, make installcheck might fail.])])
    ac_ext="${ac_ext_save}"
    CXXFLAGS="${CXXFLAGS_save}"
    CXX="${CXX_save}"
    AC_LANG_POP([C++])

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
    with_[]lib_name[]_lib_checks_successful="yes"
    with_[]lib_name[]_libs="-l${scorep_cudart_lib_name}"
else
    with_[]lib_name[]_lib_checks_successful="no"
    with_[]lib_name[]_libs=""
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
    with_[]lib_name[]_lib_checks_successful="yes"
    with_[]lib_name[]_libs="-l${scorep_cupti_lib_name}"
else
    with_[]lib_name[]_lib_checks_successful="no"
    with_[]lib_name[]_libs=""
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

dnl determine cuda version
AS_IF([test "x$scorep_cuda_error" != xyes],
    [# Cannot run CUDA apps if only stubs are provided, thus switch to corss-compiling
    # to determine version
    scorep_cross_compiling_safe=$cross_compiling
    cross_compiling=yes
    AC_COMPUTE_INT([scorep_cuda_version],
        [CUDA_VERSION],
        [#include <cuda.h>],
        [scorep_cuda_error=yes])
    cross_compiling=$scorep_cross_compiling_safe
    AS_IF([test "x${scorep_cuda_error}" = xyes],
        [AC_MSG_WARN([CUDA driver API version could not be determined.])],
        [AS_IF([test ${scorep_cuda_version} -lt 4010],
            [AC_MSG_WARN([CUDA driver API version is incompatible (< 4010)])
             scorep_cuda_error=yes],
            [AC_MSG_NOTICE([CUDA driver API version is ${scorep_cuda_version}.])])])])

dnl final check for errors
if test "x${scorep_cuda_error}" = "xno"; then
    with_[]lib_name[]_lib_checks_successful="yes"
    with_[]lib_name[]_libs="-l${scorep_cuda_lib_name}"
else
    with_[]lib_name[]_lib_checks_successful="no"
    with_[]lib_name[]_libs=""
fi
])


dnl ----------------------------------------------------------------------------


AC_DEFUN([_AC_SCOREP_LIBNVIDIA_ML_LIB_CHECK], [
scorep_nvidia_ml_error="no"
scorep_nvidia_ml_lib_name="nvidia-ml"

dnl checking for NVML library
AS_IF([test "x$scorep_nvidia_ml_error" = "xno"],
      [scorep_nvidia_ml_save_libs="${LIBS}"
       AC_SEARCH_LIBS([nvmlInit],
                      [$scorep_nvidia_ml_lib_name],
                      [],
                      [AS_IF([test "x${with_libnvidia_ml}" != xnot_set || test "x${with_libnvidia_ml_lib}" != xnot_set],
                             [AC_MSG_NOTICE([no libnvidia-ml found; check path to NVML library ...])])
                       scorep_nvidia_ml_error="yes" ])
       LIBS="${scorep_nvidia_ml_save_libs}"])

dnl final check for errors
if test "x${scorep_nvidia_ml_error}" = "xno"; then
    with_[]lib_name[]_lib_checks_successful="yes"
    with_[]lib_name[]_libs="-l${scorep_nvidia_ml_lib_name}"
else
    with_[]lib_name[]_lib_checks_successful="no"
    with_[]lib_name[]_libs=""
fi
])
