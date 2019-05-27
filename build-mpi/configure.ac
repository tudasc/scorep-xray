dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2016, 2018,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Technische Universitaet Muenchen, Germany
dnl
dnl Copyright (c) 2016,
dnl Technische Universitaet Darmstadt, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_PREREQ([2.69])
AC_INIT([Score-P], m4_esyscmd([../vendor/common/build-config/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

AFS_PACKAGE_BUILD_INIT([MPI backend])
AFS_SUMMARY_INIT

## require $ac_scorep_cross_compiling to be set to 'yes' or 'no'
AS_IF([test "x${ac_scorep_cross_compiling}" != "xyes" && \
       test "x${ac_scorep_cross_compiling}" != "xno"],
    [AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])])
# Not all MPIs support executing MPI programs without a startup
# command like mpiexec. Therefore prevent executing configure
# generated programs.
cross_compiling=yes
# suppress warnings about 'using cross tools not prefixed with host
# triplet'
ac_tool_warned=yes

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../vendor/common/build-config/generate-library-version.sh ../build-config/VERSION]))

AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../vendor/common/build-config/m4])

AC_DEFUN([AC_SCOREP_BACKEND_MPI], [])

AC_SCOREP_PRECIOUS_VARS_MPI

AC_SCOREP_CHECK_COMPILER_VAR_SET([MPICC])
AC_SCOREP_CHECK_COMPILER_VAR_SET([MPICXX])
AC_SCOREP_CHECK_COMPILER_VAR_SET([MPIF77])
AC_SCOREP_CHECK_COMPILER_VAR_SET([MPIFC])

AC_SCOREP_CONVERT_MPI_COMPILER([CC])
AC_SCOREP_CONVERT_MPI_COMPILER([CXX])
AC_SCOREP_CONVERT_MPI_COMPILER([F77])
AC_SCOREP_CONVERT_MPI_COMPILER([FC])

AC_SCOREP_CONVERT_MPI_FLAGS([CPPFLAGS])
AC_SCOREP_CONVERT_MPI_FLAGS([CFLAGS])
AC_SCOREP_CONVERT_MPI_FLAGS([CXXFLAGS])
AC_SCOREP_CONVERT_MPI_FLAGS([FFLAGS])
AC_SCOREP_CONVERT_MPI_FLAGS([FCFLAGS])
AC_SCOREP_CONVERT_MPI_FLAGS([LDFLAGS])
AC_SCOREP_CONVERT_MPI_FLAGS([LIBS])

AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR
AFS_MAINTAINER_MODE

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_PROG_CC dnl required
AC_SCOREP_PROG_CC_C99([], [AC_MSG_ERROR([No ISO C99 support in C compiler.])])
AM_PROG_CC_C_O

AC_PROG_CXX dnl needed for tests only

AC_PROG_F77 dnl needed to build mpi fortran wrappers only
SCOREP_FORTRAN_F77_WORKS dnl checks whether the Fortran 77 compiler actually is functional, OpenMPI can install empty shells for the compiler wrappers
dnl do not use AS_IF here, as this epands AC_F77_LIBRARY_LDFLAGS before AS_IF,
dnl which renders the if ineffective
if test "x${scorep_cv_f77_works}" = "xyes"; then
    AC_PROG_F77_C_O
    # AC_F*_LIBRARY_LDFLAGS should not be needed as we link the
    # libscorep_* libs with the fortran compiler. Users of libscorep_* use
    # the appropriate compiler anyhow.  Well , these macros are implicitly
    # called by AC_F*_WRAPPERS. On Cray calls to AC_F*_WRAPPERS produce
    # linker errors that can be fixed by removing "-ltcmalloc_minimal"
    # from FLIBS and FCLIBS BEFORE calling AC_F*_WRAPPERS macros.
    AC_F77_LIBRARY_LDFLAGS
    AS_CASE([${ac_scorep_platform}],
            [crayx*], [FLIBS=`echo ${FLIBS} | sed -e 's/-ltcmalloc_minimal //g' -e 's/-ltcmalloc_minimal$//g'`])
    AC_F77_WRAPPERS
fi

AC_PROG_FC dnl needed for tests only
SCOREP_FORTRAN_FC_WORKS dnl checks whether the Fortran 77 compiler actually is functional, OpenMPI can install empty shells for the compiler wrappers
dnl do not use AS_IF here, as this epands AC_F77_LIBRARY_LDFLAGS before AS_IF,
dnl which renders the if ineffective
if test "x${scorep_cv_fc_works}" = "xyes"; then
    AC_PROG_FC_C_O
    AC_FC_LIBRARY_LDFLAGS
    AS_CASE([${ac_scorep_platform}],
            [crayx*], [FCLIBS=`echo ${FCLIBS} | sed -e 's/-ltcmalloc_minimal //g' -e 's/-ltcmalloc_minimal$//g'`])
    AC_FC_WRAPPERS
fi

LT_PREREQ([2.4.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])
AFS_LINKMODE
AFS_SUMMARY([Link mode], [static=$enable_static, shared=$enable_shared])

AM_PROG_AS

AC_CONFIG_HEADERS([../src/config-backend-mpi.h])

ac_scorep_backend=yes
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [ac_scorep_frontend="no"],
      [ac_scorep_frontend="yes"])

m4_include(../build-includes/compute-node-mpi.m4)

AC_SCOREP_SVN_CONTROLLED

AM_CONDITIONAL([BUILD_TESTS], [test "x${ac_scorep_cross_compiling}" = "xno"])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])
AC_SCOREP_PLATFORM_SETTINGS
AC_SCOREP_PLATFORM_AND_MACHINE_NAMES

AC_LANG_PUSH([C])
AC_SCOREP_MPI_WORKING(
    [scorep_mpi_c_supported="yes"
     scorep_mpi_c_summary_tail=", using $CC"],
    [AC_MSG_FAILURE([MPI C compiler does not work. If you intended to build Score-P without support for the MPI programming model, please reconfigure using --without-mpi.])])
AC_SCOREP_CHECK_SIZEOF([void *])
AC_LANG_POP([C])
AFS_SUMMARY([C99 compiler works], [${scorep_mpi_c_supported}${scorep_mpi_c_summary_tail}])

# C++ MPI is needed for the jacobi C++ tests only.
AC_LANG_PUSH([C++])
AC_SCOREP_MPI_WORKING(
    [scorep_mpi_cxx_supported="yes"
     scorep_mpi_cxx_summary_tail=", using $CXX"],
    [scorep_mpi_cxx_supported="no"])
AC_LANG_POP([C++])
AFS_SUMMARY([C++ compiler works], [${scorep_mpi_cxx_supported}${scorep_mpi_cxx_summary_tail}])

AC_LANG_PUSH([Fortran 77])
AC_SCOREP_MPI_WORKING(
    [scorep_mpi_f77_supported="yes"
     scorep_mpi_f77_summary_tail=", using $F77"],
    [scorep_mpi_f77_supported="no"])
AC_LANG_POP([Fortran 77])
AFS_SUMMARY([F77 compiler works], [${scorep_mpi_f77_supported}${scorep_mpi_f77_summary_tail}])

AC_LANG_PUSH([Fortran])
AC_SCOREP_MPI_WORKING(
    [scorep_mpi_f90_supported="yes"
     scorep_mpi_f90_summary_tail=", using $FC"],
    [scorep_mpi_f90_supported="no"])
AC_LANG_POP([Fortran])
AFS_SUMMARY([F90 compiler works], [${scorep_mpi_f90_supported}${scorep_mpi_f90_summary_tail}])

AS_IF([test "x$scorep_mpi_c_supported$scorep_mpi_cxx_supported$scorep_mpi_f77_supported$scorep_mpi_f90_supported" != "xnononono"], [
    AFS_SUMMARY([Library used for MPI], [$MPI_LIBS])
])

# <AC_SCOREP_COMMON_CHECKS>
AC_SCOREP_COMPILER_CHECKS
SCOREP_FORTRAN_MANGLED
SCOREP_CHECK_FORTRAN_CHARLEN_TYPE

AC_C_BIGENDIAN

AC_CXX_NAMESPACES
AC_CXX_HAVE_SSTREAM
AC_CXX_HAVE_STRSTREAM

AFS_DEBUG_OPTION
AC_CUTEST_COLOR_TESTS
AC_SCOREP_POSIX_FUNCTIONS
AC_SCOREP_DEFINE_REVISIONS

AC_SCOREP_CHECK_SIZEOF([void *])
AC_SCOREP_CHECK_SIZEOF([double])
if (( ac_cv_sizeof_void_p > ac_cv_sizeof_double )); then
    AC_DEFINE_UNQUOTED(
        [SCOREP_ALLOCATOR_ALIGNMENT],
        [$ac_cv_sizeof_void_p],
        [First guess, use the maximum of size(void*) and sizeof(double) as alignment for SCOREP_Allocator.])
else
    AC_DEFINE_UNQUOTED(
        [SCOREP_ALLOCATOR_ALIGNMENT],
        [$ac_cv_sizeof_double],
        [First guess, use the maximum of size(void*) and sizeof(double) as alignment for SCOREP_Allocator.])
fi

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

# we need an openmpi conditional because of a bug in the pgi-openmpi
# configuration. We might extend this hack for other MPI
# implementations.
$CC -showme:version 2>&1 | grep -q "Open MPI" > /dev/null
AM_CONDITIONAL([SCOREP_MPI_OPENMPI], [test $? -eq 0])


AC_SCOREP_BUILD_MODE
AC_SCOREP_CUDA
SCOREP_OPENACC
SCOREP_CRAY_PMI
SCOREP_MNTENT_CHECK

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

AC_SCOREP_MPI
AC_SUBST([MPILIB_LDFLAGS], ["${MPILIB_LDFLAGS}"])
# Need timer defines and substitution for mpi_unification_test
SCOREP_TIMER
AC_SCOREP_COMPILER_INSTRUMENTATION

AC_SCOREP_ONLINE_ACCESS

SCOREP_MEMORY_SUPPORT

AC_SCOREP_MPI_PROFILING_HEADERS
AC_SCOREP_PACKAGE_AND_LIBRARY_VERSION

AFS_SET_SILENT_RULE_PREFIX([MPI])
AFS_CONFIG_MAKEFILE

AC_CONFIG_FILES([run_otf2_speed_test.sh \
                 ../installcheck/instrumenter_checks/mpi/Makefile:../test/instrumenter_checks/mpi/Makefile.in
                 ../installcheck/instrumenter_checks/mpi_omp/Makefile:../test/instrumenter_checks/mpi_omp/Makefile.in
                 ../src/scorep_config_tool_mpi.h:../src/tools/config/scorep_config_tool_mpi.h.in])
AC_CONFIG_FILES([../test/mpi_omp/run_metric_collection_test.sh], \
                [chmod +x ../test/mpi_omp/run_metric_collection_test.sh])
AC_CONFIG_FILES([../test/mpi_omp/run_mpi_omp_sequence_definition_test.sh], \
                [chmod +x ../test/mpi_omp/run_mpi_omp_sequence_definition_test.sh])
AC_CONFIG_FILES([../test/OA/MPI/C/run_oa_c_test_mpi.sh], \
                [chmod +x ../test/OA/MPI/C/run_oa_c_test_mpi.sh])
AC_CONFIG_FILES([run_io_mpi_test.sh:../test/io/mpi/run_io_mpi_test.sh.in],
                [chmod +x run_io_mpi_test.sh])

SCOREP_LIBRARY_WRAPPING_GENERATOR(
    [AC_CONFIG_FILES([../test/tools/libwrap_init/mpi.sh])])

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

AC_OUTPUT
