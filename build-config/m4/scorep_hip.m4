## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2022,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_hip.m4

dnl ----------------------------------------------------------------------------

AC_DEFUN([SCOREP_HIP], [
AC_REQUIRE([SCOREP_COMPUTENODE_CC])dnl

AFS_SUMMARY_PUSH

# ROCM_PATH is a common variable used in ROCm related Makefiles, thus might be
# set by the user already. /opt/rocm is the default installation for AMD
# provided packages
AC_ARG_VAR([ROCM_PATH], [Base directory of the AMD ROCm installation])

AC_ARG_WITH([rocm],
            [AS_HELP_STRING([--with-rocm[=<path-to-ROCm-installation>]],
                            [Location of the ROCm installation (e.g., /opt/rocm)])],
            [with_rocm=$withval
             AS_CASE([$withval],
                     [no],  [:],
                     [yes], [scorep_hip_root=${ROCM_PATH-/opt/rocm}],
                     [/*],  [scorep_hip_root=$withval
                             with_rocm=yes],
                     [*],   [AC_MSG_ERROR([Invalid value for --with-rocm: $withval])])])

scorep_have_hip="${with_rocm:-no}"
scorep_have_rocm_smi="yes"

# hipcc is Clang-based, in contrast to nvcc which can use a different backend compiler.
# Thus Score-P needs to be compiled with a Clang-based compiler, so that instrumentation
# and compiler instrumentation flags match (GCC's `-fno-ipa-icf` is not supported by Clang,
# and hipcc only supports `-finstrument-functions*`)
AS_IF([test "x${scorep_have_hip}" = "xyes"],
      [AS_IF([test "x${ax_cv_c_compiler_vendor%/*}" != "xclang"],
             [scorep_have_hip="no, only a Clang-based compiler is supported, got ${ax_cv_c_compiler_vendor%/*}"])])

scorep_hip_safe_CPPFLAGS=$CPPFLAGS
scorep_hip_safe_LDFLAGS=$LDFLAGS
scorep_hip_safe_LIBS=$LIBS

scorep_hip_cppflags=""
CPPFLAGS="-D__HIP_PLATFORM_AMD__ ${scorep_hip_safe_CPPFLAGS}"
AC_SCOREP_BACKEND_LIB([libamdhip64], [hip/hip_runtime.h], [], [${scorep_hip_root}])
AS_IF([test "x${scorep_have_hip}" = "xyes"],
      [AS_IF([test "x${scorep_have_libamdhip64}" = "xyes"],
             [with_libamdhip64_cppflags="-D__HIP_PLATFORM_AMD__ ${with_libamdhip64_cppflags}"
              CPPFLAGS="${with_libamdhip64_cppflags} ${scorep_hip_safe_CPPFLAGS}"
              LDFLAGS="${with_libamdhip64_ldflags} $LDFLAGS"
              AC_CHECK_DECLS([hipDeviceGetUuid], [], [], [[
#include <hip/hip_runtime.h>
]])
              AC_CHECK_DECLS([hipGetDeviceForStream], [], [], [[
#include <hip/hip_runtime_api.h>
]])
              AC_CHECK_MEMBERS([hipPointerAttribute_t.memoryType], [], [], [[
#include <hip/hip_runtime_api.h>
]])],
             [scorep_have_hip="no, missing HIP runtime library"])])

AS_IF([test "x${with_libamdhip64}" != xnot_set],
      [scorep_hip_root=${with_libamdhip64}],
      [AS_CASE([${with_libamdhip64_include},${with_libamdhip64_lib}],
               [*/include,*],       [scorep_hip_root=${with_libamdhip64_include%/include}],
               [*,*/lib|*,*/lib64], [scorep_hip_root=${with_libamdhip64_lib%/lib*}])])

AC_SCOREP_BACKEND_LIB([libroctracer64], [roctracer/roctracer_hip.h], [], [${scorep_hip_root}])
AS_IF([test "x${scorep_have_hip}" = "xyes"],
      [AS_IF([test "x${scorep_have_libroctracer64}" = "xyes"],
             [scorep_hip_roctracer_safe_CPPFLAGS=$CPPFLAGS
              CPPFLAGS="${with_libroctracer64_cppflags} ${CPPFLAGS}"

              # check for deprecated APIs, might already be removed
              AC_CHECK_DECLS([HIP_API_ID_hipMallocHost], [], [], [[
#include <roctracer/roctracer_hip.h>
]])
              AC_CHECK_DECLS([HIP_API_ID_hipHostAlloc], [], [], [[
#include <roctracer/roctracer_hip.h>
]])
              AC_CHECK_DECLS([HIP_API_ID_hipFreeHost], [], [], [[
#include <roctracer/roctracer_hip.h>
]])
              AC_CHECK_TYPES([hip_op_id_t], [], [], [[
#include <roctracer/roctracer_hip.h>
]])
              CPPFLAGS=$scorep_hip_roctracer_safe_CPPFLAGS],
             [scorep_have_hip="no, missing ROCm tracer library"])])

AC_SCOREP_BACKEND_LIB([librocm_smi64], [rocm_smi/rocm_smi.h], [], [${scorep_hip_root}])
AS_IF([test "x${scorep_have_hip}" = "xyes"],
      [AS_IF([test "x${scorep_have_librocm_smi64}" = "xyes"],
             [AC_CHECK_TYPES([hipUUID], [], [scorep_have_hip="no, missing UUID support in HIP library"], [[
#include <hip/hip_runtime_api.h>
]])],
             [scorep_have_hip="no, missing ROCm SMI library"])])

# only reset CPPFLAGS
CPPFLAGS=$scorep_hip_safe_CPPFLAGS
LDFLAGS=$scorep_hip_safe_LDFLAGS
LIBS=$scorep_hip_safe_LIBS

AS_IF([test "x${scorep_have_hip}" = "xyes"],
      [AC_PATH_PROG([HIPCC], [hipcc], [:], [${scorep_hip_root}/bin])
       AS_IF([test "x$HIPCC" != "x:"],
             [AFS_AM_CONDITIONAL([HAVE_HIP_CHECK_SUPPORT], [:], [false])
              AC_SUBST([HIPCC])])])

AS_CASE([$with_rocm,$scorep_have_hip],
        [yes,no*],
        [AC_MSG_ERROR([[Could not fulfill requested support for HIP${scorep_have_hip#no}]])],
        [no,yes],
        [scorep_have_hip="no (disabled by request)"],
        [no,no*],
        [: ignore any configure result, the user disabled ROCm anyway
         scorep_have_hip="no"])

AC_SCOREP_COND_HAVE([HIP_SUPPORT],
                    [test "x${scorep_have_hip}" = "xyes"],
                    [Defined if hip is available.],
                    [AC_SUBST(HIP_CPPFLAGS, ["${with_libamdhip64_cppflags} ${with_libroctracer64_cppflags} ${with_librocm_smi64_cppflags}"])
                     AC_SUBST(HIP_LDFLAGS,  ["${with_libamdhip64_ldflags} ${with_libamdhip64_rpathflag} ${with_libroctracer64_ldflags} ${with_libroctracer64_rpathflag} ${with_librocm_smi64_ldflags} ${with_librocm_smi64_rpathflag}"])
                     AC_SUBST(HIP_LIBS,     ["${with_libamdhip64_libs} ${with_libroctracer64_libs} ${with_librocm_smi64_libs}"])],
                    [AC_SUBST(HIP_CPPFLAGS, [""])
                     AC_SUBST(HIP_LDFLAGS,  [""])
                     AC_SUBST(HIP_LIBS,     [""])])

AFS_SUMMARY_POP([HIP support], [${scorep_have_hip}])

AS_UNSET([scorep_hip_safe_CPPFLAGS])
AS_UNSET([scorep_hip_safe_LDFLAGS])
AS_UNSET([scorep_hip_safe_LIBS])
])

AC_DEFUN([_AC_SCOREP_LIBAMDHIP64_LIB_CHECK], [
scorep_[]lib_name[]_lib_name="amdhip64"
scorep_[]lib_name[]_error="no"

dnl checking for AMDHIP library
AS_IF([test "x${scorep_[]lib_name[]_error}" = "xno"],
      [AS_UNSET([ac_cv_search_hipGetDevice])
       AC_SEARCH_LIBS([hipGetDevice],
                      [${scorep_[]lib_name[]_lib_name}],
                      [],
                      [AS_IF([test "x${with_[]lib_name[]}" != xnot_set || test "x${with_[]lib_name[]_lib}" != xnot_set],
                             [AC_MSG_NOTICE([no []lib_name[] found; check path to AMD HIP library ...])])
                       scorep_[]lib_name[]_error="yes"])])

dnl final check for errors
if test "x${scorep_[]lib_name[]_error}" = "xno"; then
    with_[]lib_name[]_lib_checks_successful="yes"
    with_[]lib_name[]_libs="-l${scorep_[]lib_name[]_lib_name}"
else
    with_[]lib_name[]_lib_checks_successful="no"
    with_[]lib_name[]_libs=""
fi
])

AC_DEFUN([_AC_SCOREP_LIBROCTRACER64_LIB_CHECK], [
scorep_[]lib_name[]_lib_name="roctracer64"
scorep_[]lib_name[]_error="no"

dnl checking for ROCTRACER library
AS_IF([test "x${scorep_[]lib_name[]_error}" = "xno"],
      [AS_UNSET([ac_cv_search_roctracer_enable_op_callback])
       AC_SEARCH_LIBS([roctracer_enable_op_callback],
                      [${scorep_[]lib_name[]_lib_name}],
                      [],
                      [AS_IF([test "x${with_[]lib_name[]}" != xnot_set || test "x${with_[]lib_name[]_lib}" != xnot_set],
                             [AC_MSG_NOTICE([no []lib_name[] found; check path to ROCTRACER library ...])])
                       scorep_[]lib_name[]_error="yes"])])

dnl final check for errors
if test "x${scorep_[]lib_name[]_error}" = "xno"; then
    with_[]lib_name[]_lib_checks_successful="yes"
    with_[]lib_name[]_libs="-l${scorep_[]lib_name[]_lib_name}"
else
    with_[]lib_name[]_lib_checks_successful="no"
    with_[]lib_name[]_libs=""
fi
])

AC_DEFUN([_AC_SCOREP_LIBROCM_SMI64_LIB_CHECK], [
scorep_[]lib_name[]_lib_name="rocm_smi64"
scorep_[]lib_name[]_error="no"

dnl checking for ROCM-SMI library
AS_IF([test "x${scorep_[]lib_name[]_error}" = "xno"],
      [AS_UNSET([ac_cv_search_rsmi_init])
       AC_SEARCH_LIBS([rsmi_init],
                      [${scorep_[]lib_name[]_lib_name}],
                      [],
                      [AS_IF([test "x${with_[]lib_name[]}" != xnot_set || test "x${with_[]lib_name[]_lib}" != xnot_set],
                             [AC_MSG_NOTICE([no []lib_name[] found; check path to ROCM-SMI library ...])])
                       scorep_[]lib_name[]_error="yes"])])

dnl final check for errors
if test "x${scorep_[]lib_name[]_error}" = "xno"; then
    with_[]lib_name[]_lib_checks_successful="yes"
    with_[]lib_name[]_libs="-l${scorep_[]lib_name[]_lib_name}"
else
    with_[]lib_name[]_lib_checks_successful="no"
    with_[]lib_name[]_libs=""
fi
])
