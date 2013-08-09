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
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

dnl ----------------------------------------------------------------------------

dnl The Score-P CUDA adapter is dependent on the CUDA driver as well as
dnl the CUDA Toolkit (http://developer.nvidia.com/cuda/cuda-toolkit). From
dnl the driver we need the library libcuda and we need to check the
dnl drivers's API version. This version is encoded in the header cuda.h,
dnl which is unfortunatly not installed with the driver but with the CUDA
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
scorep_have_cuda="no"

ac_scorep_cuda_safe_CPPFLAGS=$CPPFLAGS
ac_scorep_cuda_safe_LDFLAGS=$LDFLAGS
ac_scorep_cuda_safe_LIBS=$LIBS

AC_SCOREP_BACKEND_LIB([libcudart], [cuda.h cuda_runtime_api.h])
CPPFLAGS="$CPPFLAGS ${with_libcudart_cppflags}"
LDFLAGS="$LDFLAGS ${with_libcuda_ldflags} ${with_libcudart_ldflags}"
LIBS="$LIBS ${with_libcuda_libs} ${with_libcudart_libs}"

AC_SCOREP_BACKEND_LIB([libcuda])
CPPFLAGS="$CPPFLAGS ${with_libcuda_cppflags}"
LDFLAGS="$LDFLAGS ${with_libcuda_ldflags}"
LIBS="$LIBS ${with_libcuda_libs}"

AS_UNSET([cupti_root])
AS_IF([test "x${with_libcudart_lib}" = "xyes"],
      [for path in ${sys_lib_search_path_spec}; do 
           AS_IF([test -e ${path}/libcudart.a || test -e ${path}/libcudart.so || test -e ${path}/libcudart.dylib], 
                 [cupti_root="${path}"
                  break])
       done],
      [AS_IF([test "x${with_libcudart}" != "xnot_set"],
             [cupti_root="${with_libcudart}/extras/CUPTI"])])

AC_SCOREP_BACKEND_LIB([libcupti], [cupti.h], [${with_libcudart_cppflags}], [${cupti_root}])

CPPFLAGS=$ac_scorep_cuda_safe_CPPFLAGS
LDFLAGS=$ac_scorep_cuda_safe_LDFLAGS
LIBS=$ac_scorep_cuda_safe_LIBS

AC_SCOREP_COND_HAVE([CUDA],
                    [test "x${scorep_have_libcudart}" = "xyes" && test "x${scorep_have_libcupti}"  = "xyes" && test "x${scorep_have_libcuda}"   = "xyes"],
                    [Defined if cuda is available.],
                    [scorep_have_cuda="yes"
                     AC_SUBST(CUDA_CPPFLAGS, ["${with_libcudart_cppflags} ${with_libcupti_cppflags}"])
                     AC_SUBST(CUDA_LDFLAGS,  ["${with_libcuda_ldflags} ${with_libcudart_ldflags} ${with_libcupti_ldflags} ${with_libcuda_rpathflag} ${with_libcudart_rpathflag} ${with_libcupti_rpathflag}"])
                     AC_SUBST(CUDA_LIBS,     ["${with_libcuda_libs} ${with_libcudart_libs} ${with_libcupti_libs}"])],
                    [AC_SUBST(CUDA_CPPFLAGS, [""])
                     AC_SUBST(CUDA_LDFLAGS,  [""])
                     AC_SUBST(CUDA_LIBS,     [""])])

AFS_SUMMARY([cuda support], [${scorep_have_cuda}, see also libcudart, libcuda, and libcupti support])
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
AS_IF([test x"$scorep_cudart_error" = "xno"],
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "cuda_runtime_api.h"]],
        [[
#ifndef CUDART_VERSION
#  error "CUDART_VERSION not defined"
#elif CUDART_VERSION < 4010
#  error "CUDART_VERSION < 4010"
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
#  error "CUPTI_API_VERSION not defined"
#elif CUPTI_API_VERSION < 2
#  error "CUPTI_API_VERSION < 2"
#endif
         ]])],
         [],
         [AC_MSG_NOTICE([CUPTI version could not be determined and/or is
                         incompatible (< 2). See 'config.log' for more details.])
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
#  error "CUDA_VERSION not defined"
#elif CUDA_VERSION < 4010
#  error "CUDA_VERSION < 4010"
#endif
        ]])],
        [],
        [AC_MSG_NOTICE([CUDA driver API version could not be determined and/or is
                        incompatible (< 4.1). See 'config.log' for more details.])
         scorep_cuda_error="yes" ])])

dnl final check for errors
if test "x${scorep_cuda_error}" = "xno"; then
    with_$1_lib_checks_successful="yes"
    with_$1_libs="-l${scorep_cuda_lib_name}"
else
    with_$1_lib_checks_successful="no"
    with_$1_libs=""
fi
])
