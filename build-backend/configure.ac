dnl 
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2013,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Technische Universitaet Muenchen, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_PREREQ([2.68])
AC_INIT([Score-P], m4_esyscmd([../vendor/common/build-config/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

AFS_PACKAGE_INIT([backend])
AFS_SUMMARY_INIT

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../vendor/common/build-config/generate-library-version.sh ../build-config/VERSION]))

AC_CONFIG_AUX_DIR([../build-config])
AC_CONFIG_MACRO_DIR([../vendor/common/build-config/m4
                     ../build-config/m4])

AC_DEFUN([AC_SCOREP_BACKEND], [])

AC_SCOREP_CHECK_COMPILER_VAR_SET([CC])
AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX])
AC_SCOREP_CHECK_COMPILER_VAR_SET([F77])
AC_SCOREP_CHECK_COMPILER_VAR_SET([FC])

AM_INIT_AUTOMAKE([foreign color-tests 1.11.1 -Wall tar-pax])
AM_SILENT_RULES([yes])

LT_PREREQ([2.4])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

AC_PROG_CC dnl required
AC_SCOREP_PROG_CC_C99([], [AC_MSG_ERROR([No ISO C99 support in C compiler.])])
AFS_SUMMARY([C99 compiler used], [$CC])
AM_PROG_CC_C_O

AC_PROG_CXX dnl in cross mode, needed just for tests; in non-cross
            dnl mode also for frontend tools

AC_PROG_F77 dnl needed for linking if we build mpi fortran wrappers,
            dnl also for tests
AC_PROG_F77_C_O
# AC_F*_LIBRARY_LDFLAGS should not be needed as we link the
# libscorep_* libs with the fortran compiler. Users of libscorep_* use
# the appropriate compiler anyhow.  Well , these macros are implicitly
# called by AC_F*_WRAPPERS. On Cray calls to AC_F*_WRAPPERS produce
# linker errors that can be fixed by removing "-ltcmalloc_minimal"
# from FLIBS and FCLIBS BEFORE calling AC_F*_WRAPPERS macros.
AC_F77_LIBRARY_LDFLAGS
AS_CASE([${ac_scorep_platform}],
        [crayx*], [FLIBS=`echo ${FLIBS} | sed -e 's/-ltcmalloc_minimal //g'`])
AC_F77_WRAPPERS

AC_PROG_FC dnl needed for tests only
AC_PROG_FC_C_O
AC_FC_LIBRARY_LDFLAGS
AS_CASE([${ac_scorep_platform}],
        [crayx*], [FCLIBS=`echo ${FCLIBS} | sed -e 's/-ltcmalloc_minimal //g'`])
AC_FC_WRAPPERS

AM_PROG_AS

AC_CONFIG_HEADERS([../src/config-backend.h])
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
    [AC_CONFIG_HEADERS([../src/config-backend-for-frontend.h])])

## require $ac_scorep_cross_compiling to be set to 'yes' or 'no'
AS_IF([test "x${ac_scorep_cross_compiling}" != "xyes" && \
       test "x${ac_scorep_cross_compiling}" != "xno"],
    [AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])])
cross_compiling=${ac_scorep_cross_compiling}

ac_scorep_backend="yes"
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
    [ac_scorep_frontend="no"],
    [ac_scorep_frontend="yes"])

AC_SCOREP_SVN_CONTROLLED

AM_CONDITIONAL([BUILD_TESTS], [test "x${ac_scorep_cross_compiling}" = "xno"])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])
AC_SCOREP_PLATFORM_SETTINGS
AC_SCOREP_PLATFORM_AND_MACHINE_NAMES

# <AC_SCOREP_COMMON_CHECKS>
AC_SCOREP_COMPILER_CHECKS

AC_C_BIGENDIAN

AC_CXX_NAMESPACES
AC_CXX_HAVE_SSTREAM
AC_CXX_HAVE_STRSTREAM

AC_PROG_RANLIB

AC_SCOREP_DEBUG_OPTION
AC_SCOREP_ON_DEBUG_OPTION
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
# </AC_SCOREP_COMMON_CHECKS>

AS_IF([test "x${ac_scorep_platform}" = "xbgq"],
    [SCOREP_PAMI])
AC_SCOREP_RUNTIME_MANAGEMENT_TIMINGS 
AC_SCOREP_COMPILER_INSTRUMENTATION_FLAGS
AC_SCOREP_BACKEND_TEST_RUNS
AC_SCOREP_ATTRIBUTE_ALIGNMENT
AC_SCOREP_FORTRAN_SUPPORT_ALLOCATABLE
AC_SCOREP_LIBPAPI
AC_SCOREP_RUSAGE
AC_SCOREP_METRIC_PLUGINS
AC_SCOREP_LINK_FLAGS_CHECK

AS_IF([test -n "${scorep_otf2_bindir}"],
      [AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
           [scorep_otf2_config_backend_arg="--backend"])
       AC_SUBST([OTF2_CPPFLAGS], ["`${scorep_otf2_bindir}/otf2-config --cppflags ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_LIBS],     ["`${scorep_otf2_bindir}/otf2-config --libs ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_LDFLAGS],  ["`${scorep_otf2_bindir}/otf2-config --ldflags ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_BINDIR],   ["${scorep_otf2_bindir}"])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OTF2], [test 1 -eq 1])],
      [AC_SUBST([OTF2_CPPFLAGS], ['-I../vendor/otf2/include -I$(srcdir)/../vendor/otf2/include'])
       AC_SUBST([OTF2_LIBS],     [../vendor/otf2/build-backend/libotf2.la])
       AC_SUBST([OTF2_LDFLAGS],  [])
       AC_SUBST([OTF2_BINDIR],   [../vendor/otf2/build-backend])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OTF2], [test 1 -eq 0])])

opari2_fix_and_free_form_options=1
AS_IF([test -n "${scorep_opari2_bindir}"],
      [AC_SUBST([OPARI2_BINDIR],            ["${scorep_opari2_bindir}"])
       AC_SUBST([OPARI2_MAKE_CHECK_BINDIR], ["${scorep_opari2_bindir}"])
       AC_SUBST([OPARI2_CPPFLAGS],          ["`${scorep_opari2_bindir}/opari2-config --cflags`"])
       AC_DEFINE_UNQUOTED([HAVE_BACKEND_OPARI2_REVISION], [`${scorep_opari2_bindir}/opari2-config --opari2-revision`],
                 [OPARI2 revision used for version-dependent feature activation.])
       AC_SUBST([HAVE_EXTERNAL_OPARI2],     [1])
       AC_SUBST([POMP2_REGION_INFO_SRC_DIR],["${scorep_opari2_bindir}/../share/opari2/devel"])
       AC_SUBST([POMP2_REGION_INFO_INC_DIR],["${scorep_opari2_bindir}/../share/opari2/devel"])
       ${scorep_opari2_bindir}/opari2-config --fix-form > /dev/null 2>&1
       AS_IF([test $? -ne 0], [opari_fix_and_free_form_options=0])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OPARI2], [true])],
      [AC_SUBST([OPARI2_BINDIR],            ["${prefix}/bin"])
       AC_SUBST([OPARI2_MAKE_CHECK_BINDIR], ["../vendor/opari2/build-frontend"])
       AC_SUBST([OPARI2_CPPFLAGS],          ['-I$(srcdir)/../vendor/opari2/include'])
       AC_SUBST([HAVE_EXTERNAL_OPARI2],     [0])
       AC_SUBST([POMP2_REGION_INFO_SRC_DIR],["../vendor/opari2/src/pomp-lib-dummy"])
       AC_SUBST([POMP2_REGION_INFO_INC_DIR],['$(srcdir)/../vendor/opari2/src/pomp-lib-dummy'])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OPARI2], [false])])
AC_SUBST([HAVE_OPARI2_FIX_AND_FREE_FORM_OPTIONS], [${opari2_fix_and_free_form_options}])

AS_IF([test -n "${scorep_cube_bindir}"],
      [AC_SUBST([CUBE_WRITER_CPPFLAGS],      ["`${scorep_cube_bindir}/cube-config --writer-cppflags`"])
       AC_SUBST([CUBE_WRITER_LIBS],          ["`${scorep_cube_bindir}/cube-config --writer-libs`"])
       AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
           [scorep_cube_config_backend_arg="--backend"])
       AC_SUBST([CUBE_WRITER_LDFLAGS],       ["`${scorep_cube_bindir}/cube-config --writer-ldflags ${scorep_cube_config_backend_arg}`"])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_CUBE_WRITER], [test 1 -eq 1])
      ],
      [AC_SUBST([CUBE_WRITER_CPPFLAGS],      ['-I$(srcdir)/../src/measurement/cube_writer'])
       AC_SUBST([CUBE_WRITER_LIBS],          [libscorep_cube_writer.la])
       AC_SUBST([CUBE_WRITER_LDFLAGS],       [])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_CUBE_WRITER], [test 1 -eq 0])
      ])

AS_IF([test "x${ac_scorep_cross_compiling}" = "xno" && test "x${scorep_have_score_build}" = "xno"], 
      [AC_SCOREP_CUBE_READER],
      [AM_CONDITIONAL([HAVE_SCORE], [test 1 -eq 0])])

# does this work on cross-compile machines?
AC_CHECK_FUNCS_ONCE([readlink])

AC_SCOREP_BUILD_MODE
AC_SCOREP_CUDA
SCOREP_CRAY_PMI

# On crosscompile machines, install backend stuff into $(bin|lid)dir/backend.
backend_suffix=""
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [backend_suffix="/backend"])
AC_SUBST([backend_suffix])

AC_SCOREP_CHECK_SIZEOF([int64_t])
if (( ac_cv_sizeof_void_p > ac_cv_sizeof_int64_t )); then
    AC_MSG_ERROR([sizeof(void*) > sizeof(int64_t): internal thread handling will not work.])
fi

AC_SCOREP_OPENMP
AC_SCOREP_PDT
AC_SCOREP_COBI

AC_SCOREP_TIMER
AC_SCOREP_COMPILER_INSTRUMENTATION

AC_SCOREP_ONLINE_ACCESS

## trunc and ceil needed by the cube4 writer
AC_SEARCH_LIBS([trunc], [m])
AC_SEARCH_LIBS([ceil], [m])

# We get cube via svn:externals and build it ourselves with compression 
# support disabled. This solves difficulties with library detection on 
# cross compile systems. The following check can be omitted.
#AC_SCOREP_CUBE4

# assuming that mpi configure already ran
AC_SCOREP_COND_HAVE([MPI_SUPPORT],
                    [test -f ../build-mpi/Makefile],
                    [Defined if MPI support is available.])

AC_SCOREP_PACKAGE_AND_LIBRARY_VERSION

dnl Predicate to use HARDCODE_LIBDIR_FLAG_* only if linker needs it. 
AS_IF([test "x${hardcode_into_libs}" = "xyes"], 
      [ac_scorep_hardcode_into_libs=1],
      [ac_scorep_hardcode_into_libs=0])
AC_SUBST([HARDCODE_INTO_LIBS], [${ac_scorep_hardcode_into_libs}])
dnl Flag to hardcode libdir into a binary during linking, C version.
AC_SUBST([HARDCODE_LIBDIR_FLAG_CC], [${hardcode_libdir_flag_spec}])
dnl Flag to hardcode libdir into a binary during linking, C++ version.
AC_SUBST([HARDCODE_LIBDIR_FLAG_CXX], [${hardcode_libdir_flag_spec_CXX}])
dnl Flag to hardcode libdir into a binary during linking, F77 version.
AC_SUBST([HARDCODE_LIBDIR_FLAG_F77], [${hardcode_libdir_flag_spec_F77}])
dnl Flag to hardcode libdir into a binary during linking, Fortran version.
AC_SUBST([HARDCODE_LIBDIR_FLAG_FC], [${hardcode_libdir_flag_spec_FC}])
dnl The ${wl} part of the HARDCODE_LIBDIR_FLAG_* defines.
AC_SUBST([HARDCODE_LIBDIR_WL], [${wl}])
dnl The ${aix_libpath} part of the HARDCODE_LIBDIR_FLAG_* defines. Only available on AIX.
AC_SUBST([HARDCODE_LIBDIR_AIX_LIBPATH], [${aix_libpath}])

AC_SUBST([DATAROOTDIR_SCOREP], [`eval echo ${datarootdir}/${PACKAGE}`])

AC_SUBST([SYS_LIB_DLSEARCH_PATH_SPEC], [${sys_lib_dlsearch_path_spec}]) 

AC_CONFIG_FILES([Makefile], [[
test "x${ac_scorep_cross_compiling}" = "xyes" && {
sed -e 's/_0 = @echo "  \(YACC\|GEN\|LEX\)/&  /; t; s/_0 = @echo "  /&BE/' \
Makefile >Makefile.be && mv -f Makefile.be Makefile || rm -f Makefile.be
}
]], [ac_scorep_cross_compiling='$ac_scorep_cross_compiling'])
AC_CONFIG_FILES([../installcheck/Makefile.common_rules:../test/installcheck/Makefile.common_rules.in
                 ../installcheck/Makefile.instrument_configurations:../test/installcheck/Makefile.instrument_configurations.in
                 ../installcheck/serial/Makefile:../test/installcheck/serial/Makefile.in
                 ../installcheck/omp/Makefile:../test/installcheck/omp/Makefile.in
                 ../src/scorep_config_tool_backend.h:../src/tools/config/scorep_config_tool_backend.h.in])
AC_CONFIG_FILES([../test/adapters/cuda/run_cuda_test.sh], \
                [chmod +x ../test/adapters/cuda/run_cuda_test.sh])
AC_CONFIG_FILES([../test/adapters/user/C/run_phase_test.sh], \
                [chmod +x ../test/adapters/user/C/run_phase_test.sh])
AC_CONFIG_FILES([../test/adapters/user/C/run_selective_test.sh], \
                [chmod +x ../test/adapters/user/C/run_selective_test.sh])
AC_CONFIG_FILES([../test/adapters/user/C++/run_user_cxx_test.sh], \
                [chmod +x ../test/adapters/user/C++/run_user_cxx_test.sh])
AC_CONFIG_FILES([../test/adapters/user/Fortran/run_selective_test.sh], \
                [chmod +x ../test/adapters/user/Fortran/run_selective_test.sh])
AC_CONFIG_FILES([../test/omp_tasks/run_fibonacci_test.sh], \
                [chmod +x ../test/omp_tasks/run_fibonacci_test.sh])
AC_CONFIG_FILES([../test/omp_tasks/run_single_loop_test.sh], \
                [chmod +x ../test/omp_tasks/run_single_loop_test.sh])
AC_CONFIG_FILES([../installcheck/instrumenter-configurations.sh:../test/installcheck/instrumenter-configurations.sh.in],
                [chmod +x ../installcheck/instrumenter-configurations.sh])
AC_CONFIG_FILES([../test/filtering/run_filter_test.sh], \
                [chmod +x ../test/filtering/run_filter_test.sh])
AC_CONFIG_FILES([../test/filtering/run_filter_f_test.sh], \
                [chmod +x ../test/filtering/run_filter_f_test.sh])
AC_CONFIG_FILES([../test/apart_test_suite/run_apart_test_suite_with_oa_test.sh], \
                [chmod +x ../test/apart_test_suite/run_apart_test_suite_with_oa_test.sh])
AC_CONFIG_FILES([../test/services/metric/run_rusage_serial_metric_test.sh], \
                [chmod +x ../test/services/metric/run_rusage_serial_metric_test.sh])
AC_CONFIG_FILES([../test/services/metric/run_rusage_openmp_metric_test.sh], \
                [chmod +x ../test/services/metric/run_rusage_openmp_metric_test.sh])
AC_CONFIG_FILES([../test/services/metric/run_rusage_openmp_per_process_metric_test.sh], \
                [chmod +x ../test/services/metric/run_rusage_openmp_per_process_metric_test.sh])
AC_CONFIG_FILES([../test/services/metric/run_papi_serial_metric_test.sh], \
                [chmod +x ../test/services/metric/run_papi_serial_metric_test.sh])
AC_CONFIG_FILES([../test/services/metric/run_papi_openmp_metric_test.sh], \
                [chmod +x ../test/services/metric/run_papi_openmp_metric_test.sh])
AC_CONFIG_FILES([../test/services/metric/run_papi_openmp_per_process_metric_test.sh], \
                [chmod +x ../test/services/metric/run_papi_openmp_per_process_metric_test.sh])
AC_CONFIG_FILES([../test/rewind/run_rewind_test.sh], \
                [chmod +x ../test/rewind/run_rewind_test.sh])
AC_CONFIG_FILES([../test/OA/OMP/run_oa_omp_test.sh], \
                [chmod +x ../test/OA/OMP/run_oa_omp_test.sh])
AC_CONFIG_FILES([../test/OA/serial/C/run_oa_c_test.sh], \
                [chmod +x ../test/OA/serial/C/run_oa_c_test.sh])
AC_CONFIG_FILES([../test/OA/serial/Fortran/run_oa_f_test.sh], \
                [chmod +x ../test/OA/serial/Fortran/run_oa_f_test.sh])
AC_OUTPUT
