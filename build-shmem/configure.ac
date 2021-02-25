dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013-2018, 2020,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2015-2016, 2018-2021,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_PREREQ([2.69])
AC_INIT([Score-P], m4_esyscmd([../build-config/common/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

AFS_PACKAGE_BUILD_INIT([SHMEM backend])
AFS_SUMMARY_INIT([../build-backend])

# Not all SHMEMs support executing SHMEM programs without a startup
# command like oshrun. Therefore prevent executing configure
# generated programs.
cross_compiling=yes
# suppress warnings about 'using cross tools not prefixed with host
# triplet'
ac_tool_warned=yes

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../build-config/common/generate-library-version.sh ../build-config/VERSION]))

AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../build-config/common/m4])

AC_DEFUN([AC_SCOREP_BACKEND_SHMEM], [])

AFS_PRECIOUS_VARS_SHMEM

AC_SCOREP_CHECK_COMPILER_VAR_SET([SHMEMCC])
AC_SCOREP_CHECK_COMPILER_VAR_SET([SHMEMCXX])
AC_SCOREP_CHECK_COMPILER_VAR_SET([SHMEMF77])
AC_SCOREP_CHECK_COMPILER_VAR_SET([SHMEMFC])

AFS_CONVERT_SHMEM_COMPILER([CC])
AFS_CONVERT_SHMEM_COMPILER([CXX])
AFS_CONVERT_SHMEM_COMPILER([F77])
AFS_CONVERT_SHMEM_COMPILER([FC])

AFS_CONVERT_SHMEM_FLAGS([CPPFLAGS])
AFS_CONVERT_SHMEM_FLAGS([CFLAGS])
AFS_CONVERT_SHMEM_FLAGS([CXXFLAGS])
AFS_CONVERT_SHMEM_FLAGS([FFLAGS])
AFS_CONVERT_SHMEM_FLAGS([FCFLAGS])
AFS_CONVERT_SHMEM_FLAGS([LDFLAGS])
AFS_CONVERT_SHMEM_FLAGS([LIBS])

AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR
AFS_MAINTAINER_MODE

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

SCOREP_COMPUTENODE_CC
SCOREP_COMPUTENODE_CXX
SCOREP_COMPUTENODE_F77
SCOREP_COMPUTENODE_FC

LT_PREREQ([2.4.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])
AFS_LINKMODE
AFS_SUMMARY([Link mode], [static=$enable_static, shared=$enable_shared])

AM_PROG_AS

AC_CONFIG_HEADERS([../src/config-backend-shmem.h])

## require $ac_scorep_cross_compiling to be set to 'yes' or 'no'
AS_IF([test "x${ac_scorep_cross_compiling}" != "xyes" && \
       test "x${ac_scorep_cross_compiling}" != "xno"],
    [AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])])

ac_scorep_backend=yes
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [ac_scorep_frontend="no"],
      [ac_scorep_frontend="yes"])

m4_include(../build-includes/computenode-shmem.m4)

AC_SCOREP_GIT_CONTROLLED

AM_CONDITIONAL([BUILD_TESTS], [test "x${ac_scorep_cross_compiling}" = "xno"])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])
AC_SCOREP_PLATFORM_SETTINGS
AC_SCOREP_PLATFORM_AND_MACHINE_NAMES

AC_LANG_PUSH([C])
SCOREP_SHMEM_WORKING(
    [scorep_shmem_c_supported="yes"
     scorep_shmem_c_summary_tail=", using $CC"],
    [scorep_shmem_c_supported="no"])
AC_SCOREP_CHECK_SIZEOF([void *])
AC_LANG_POP([C])
AFS_SUMMARY([C99 compiler works], [${scorep_shmem_c_supported}${scorep_shmem_c_summary_tail}])

# C++ SHMEM is needed for the jacobi C++ tests only.
AC_LANG_PUSH([C++])
SCOREP_SHMEM_WORKING(
    [scorep_shmem_cxx_supported="yes"
     scorep_shmem_cxx_summary_tail=", using $CXX"],
    [scorep_shmem_cxx_supported="no"])
AC_LANG_POP([C++])
AFS_SUMMARY([C++ compiler works], [${scorep_shmem_cxx_supported}${scorep_shmem_cxx_summary_tail}])

AC_LANG_PUSH([Fortran 77])
SCOREP_SHMEM_WORKING(
    [scorep_shmem_f77_supported="yes"
     scorep_shmem_f77_summary_tail=", using $F77"],
    [scorep_shmem_f77_supported="no"])
AC_LANG_POP([Fortran 77])
AFS_SUMMARY([F77 compiler works], [${scorep_shmem_f77_supported}${scorep_shmem_f77_summary_tail}])

AC_LANG_PUSH([Fortran])
SCOREP_SHMEM_WORKING(
    [scorep_shmem_f90_supported="yes"
     scorep_shmem_f90_summary_tail=", using $FC"],
    [scorep_shmem_f90_supported="no"])
AC_LANG_POP([Fortran])
AFS_SUMMARY([F90 compiler works], [${scorep_shmem_f90_supported}${scorep_shmem_f90_summary_tail}])

AS_IF([test "x$scorep_shmem_c_supported$scorep_shmem_cxx_supported$scorep_shmem_f77_supported$scorep_shmem_f90_supported" != "xnononono"], [
    AFS_SUMMARY([Library used for SHMEM], [$SHMEM_LIBS])
])

# <AC_SCOREP_COMMON_CHECKS>
AC_SCOREP_COMPILER_CHECKS
SCOREP_CHECK_FORTRAN_CHARLEN_TYPE

AC_C_BIGENDIAN

AC_CXX_NAMESPACES
AC_CXX_HAVE_SSTREAM
AC_CXX_HAVE_STRSTREAM

AFS_DEBUG_OPTION
AC_CUTEST_COLOR_TESTS
AC_SCOREP_POSIX_FUNCTIONS
AC_SCOREP_DEFINE_REVISIONS

AC_SCOREP_ERROR_CODES
AC_SCOREP_RUNTIME_MANAGEMENT_TIMINGS
# </AC_SCOREP_COMMON_CHECKS>

AC_SCOREP_BACKEND_TEST_RUNS
AC_SCOREP_FORTRAN_SUPPORT_ALLOCATABLE
SCOREP_THREAD_LOCAL_STORAGE
SCOREP_SAMPLING
SCOREP_METRICS_CHECK
SCOREP_DLFCN
SCOREP_OPENCL
SCOREP_IO_RECORDING

## Configure internal/external libraries
scorep_otf2_config_backend_arg=
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [scorep_otf2_config_backend_arg="--target=backend"
       AS_IF([test "x${ac_scorep_platform}" = "xmic"],
             [scorep_otf2_config_backend_arg="--target=mic"])
      ])
AS_IF([test -n "${scorep_otf2_bindir}"],
      [AC_SUBST([OTF2_CPPFLAGS], ["`${scorep_otf2_bindir}/otf2-config --cppflags ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_LIBS],     ["`${scorep_otf2_bindir}/otf2-config --libs ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_LDFLAGS],  ["`${scorep_otf2_bindir}/otf2-config --ldflags ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_BINDIR],   ["${scorep_otf2_bindir}"])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OTF2], [test 1 -eq 1])],
      [AC_SUBST([OTF2_CPPFLAGS], ['-I../vendor/otf2/include -I$(srcdir)/../vendor/otf2/include'])
       AC_SUBST([OTF2_LIBS],     [../vendor/otf2/build-backend/libotf2.la])
       AC_SUBST([OTF2_LDFLAGS],  [])
       AC_SUBST([OTF2_BINDIR],   [../vendor/otf2/build-backend])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OTF2], [test 1 -eq 0])])


AS_IF([test -n "${scorep_opari2_bindir}"],
      [AC_SUBST([OPARI2_CPPFLAGS], ["`${scorep_opari2_bindir}/opari2-config --cflags`"])],
      [AC_SUBST([OPARI2_CPPFLAGS], ['-I$(srcdir)/../vendor/opari2/include'])])

AC_SCOREP_BUILD_MODE
AC_SCOREP_CUDA
SCOREP_OPENACC
SCOREP_KOKKOS
SCOREP_CRAY_PMI
SCOREP_MNTENT_CHECK
SCOREP_LUSTREAPI_CHECK

# On crosscompile machines, install backend/mic stuff into
# $(bin|lid)dir/(backend|mic).
backend_suffix=""
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [backend_suffix="/backend"
       AS_IF([test "x${ac_scorep_platform}" = "xmic"],
             [backend_suffix="/mic"])
      ])
AC_SUBST([backend_suffix])

AC_SCOREP_OPENMP
AFS_SUMMARY_SECTION_BEGIN
SCOREP_OPENMP_TPD
SCOREP_OPENMP_ANCESTRY
AFS_SUMMARY_SECTION_END

SCOREP_SHMEM
AC_SCOREP_COMPILER_INSTRUMENTATION

#AC_SCOREP_ONLINE_ACCESS

SCOREP_MEMORY_SUPPORT
SCOREP_GCC_ATOMIC_BUILTINS

#AC_SCOREP_MPI_PROFILING_HEADERS
AC_SCOREP_PACKAGE_AND_LIBRARY_VERSION

AFS_SET_SILENT_RULE_PREFIX([SHMEM])
AFS_CONFIG_MAKEFILE

AC_CONFIG_FILES([../installcheck/instrumenter_checks/shmem/Makefile:../test/instrumenter_checks/shmem/Makefile.in
                 ../installcheck/instrumenter_checks/shmem_omp/Makefile:../test/instrumenter_checks/shmem_omp/Makefile.in
                 ../src/scorep_config_tool_shmem.h:../src/tools/config/scorep_config_tool_shmem.h.in])
##AC_CONFIG_FILES([../test/shmem_omp/run_metric_collection_test.sh], \
##                [chmod +x ../test/shmem_omp/run_metric_collection_test.sh])
##AC_CONFIG_FILES([../test/OA/MPI/C/run_oa_c_test_mpi.sh], \
##                [chmod +x ../test/OA/MPI/C/run_oa_c_test_mpi.sh])

SCOREP_LIBRARY_WRAPPING_GENERATOR(
    [AC_CONFIG_FILES([../test/tools/libwrap_init/shmem.sh])])

AM_COND_IF([HAVE_SHMEM_PROFILING_INTERFACE],
    [],
    [SCOREP_LD_WRAP_FILE([shmem])])

# On Cray, libunwind.la's dependecy_libs contain -lc which leads to a
# "multiple definition of `__lll_unlock_wake_private'" error. The patch
# below works around this issue. A better solution would be to fix the
# .la file or to provide a libunwind together with scorep as the
# side effects of this patch aren't investigated.
AS_CASE([${ac_scorep_platform}],
    [crayx*],
         [AC_CONFIG_COMMANDS([libtool-fix-cray-libunwind],
              [sed -e 's/for search_ext in \.la /for search_ext in /' \
               libtool > libtool.fix && mv libtool.fix libtool])
         ])

dnl Linux
SCOREP_CHECK_SYSCALL([SYS_gettid],
                     [],
                     [])

dnl macOS +10.5
SCOREP_CHECK_SYSCALL([SYS_thread_selfid],
                     [],
                     [])

AC_OUTPUT
