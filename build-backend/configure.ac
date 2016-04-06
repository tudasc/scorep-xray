dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2016,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2016,
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

AC_PREREQ([2.69])
AC_INIT([Score-P], m4_esyscmd([../vendor/common/build-config/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

AFS_PACKAGE_BUILD_INIT([backend])
AFS_SUMMARY_INIT

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../vendor/common/build-config/generate-library-version.sh ../build-config/VERSION]))

AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../vendor/common/build-config/m4])

AC_DEFUN([AC_SCOREP_BACKEND], [])

## require $ac_scorep_cross_compiling to be set to 'yes' or 'no'
AS_IF([test "x${ac_scorep_cross_compiling}" != "xyes" && \
       test "x${ac_scorep_cross_compiling}" != "xno"],
    [AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])])
cross_compiling=${ac_scorep_cross_compiling}

AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"], [
    # suppress warnings about 'using cross tools not prefixed with host triplet'
    ac_tool_warned=yes])

AC_SCOREP_CHECK_COMPILER_VAR_SET([CC])
AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX])
AC_SCOREP_CHECK_COMPILER_VAR_SET([F77])
AC_SCOREP_CHECK_COMPILER_VAR_SET([FC])

AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR

LT_PREREQ([2.4.6])
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
        [crayx*], [FLIBS=`echo ${FLIBS} | sed -e 's/-ltcmalloc_minimal //g' -e 's/-ltcmalloc_minimal$//g'`])
AC_F77_WRAPPERS

AC_PROG_FC dnl needed for tests only
AC_PROG_FC_C_O
AC_FC_LIBRARY_LDFLAGS
AS_CASE([${ac_scorep_platform}],
        [crayx*], [FCLIBS=`echo ${FCLIBS} | sed -e 's/-ltcmalloc_minimal //g' -e 's/-ltcmalloc_minimal$//g'`])
AC_FC_WRAPPERS

AM_PROG_AS

AC_CONFIG_HEADERS([../src/config-backend.h])
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
    [AC_CONFIG_HEADERS([../src/config-backend-for-frontend.h])])

ac_scorep_backend="yes"
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
    [ac_scorep_frontend="no"],
    [ac_scorep_frontend="yes"])

# On crosscompile machines, install backend/mic stuff into
# $(bin|lid)dir/(backend|mic).
backend_suffix=""
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [backend_suffix="/backend"
       AS_IF([test "x${ac_scorep_platform}" = "xmic"],
             [backend_suffix="/mic"])
      ])
AC_SUBST([backend_suffix])

m4_include(../build-includes/compute-node.m4)
AS_IF([test "x${ac_scorep_cross_compiling}" = "xno"],
    [m4_include(../build-includes/login-node.m4)])

adl_RECURSIVE_EVAL([${includedir}], [INCLUDEDIR])
AC_SUBST([INCLUDEDIR])
adl_RECURSIVE_EVAL([${bindir}], [BINDIR])
AC_SUBST([BINDIR])
adl_RECURSIVE_EVAL([${libdir}/${PACKAGE}], [PKGLIBDIR])
AC_SUBST([PKGLIBDIR])
adl_RECURSIVE_EVAL([${libdir}${backend_suffix}/${PACKAGE}], [BACKEND_PKGLIBDIR])
AC_SUBST([BACKEND_PKGLIBDIR])
adl_RECURSIVE_EVAL([${datadir}/${PACKAGE}], [PKGDATADIR])
AC_SUBST([PKGDATADIR])
adl_RECURSIVE_EVAL([${docdir}], [DOCDIR])
AC_SUBST([DOCDIR])

AC_SCOREP_SVN_CONTROLLED

AM_CONDITIONAL([BUILD_TESTS], [test "x${ac_scorep_cross_compiling}" = "xno"])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])
AC_SCOREP_PLATFORM_SETTINGS
AC_SCOREP_PLATFORM_AND_MACHINE_NAMES

# <AC_SCOREP_COMMON_CHECKS>
AC_SCOREP_COMPILER_CHECKS
SCOREP_FORTRAN_MANGLED

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
# </AC_SCOREP_COMMON_CHECKS>

AS_IF([test "x${ac_scorep_platform}" = "xbgq"],
    [SCOREP_PAMI])
AC_SCOREP_RUNTIME_MANAGEMENT_TIMINGS
AC_SCOREP_BACKEND_TEST_RUNS
AC_SCOREP_FORTRAN_SUPPORT_ALLOCATABLE
SCOREP_THREAD_LOCAL_STORAGE
SCOREP_SAMPLING
SCOREP_METRICS_CHECK
SCOREP_DLFCN
SCOREP_OPENCL
AC_SCOREP_LINK_FLAGS_CHECK

AFS_SUMMARY_SECTION_BEGIN([OTF2 features])

scorep_otf2_config=
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
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OTF2], [test 1 -eq 1])
       scorep_otf2_config=${scorep_otf2_bindir}/otf2-config],
      [AC_SUBST([OTF2_CPPFLAGS], ['-I../vendor/otf2/include -I$(srcdir)/../vendor/otf2/include'])
       AC_SUBST([OTF2_LIBS],     [../vendor/otf2/build-backend/libotf2.la])
       AC_SUBST([OTF2_LDFLAGS],  [])
       AC_SUBST([OTF2_BINDIR],   [../vendor/otf2/build-backend])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OTF2], [test 1 -eq 0])
       scorep_otf2_config=../vendor/otf2/otf2-mini-config.sh])

scorep_otf2_have_sion=no
for substrate in `$scorep_otf2_config $scorep_otf2_config_backend_arg --features=substrates`
do
    AS_CASE([${substrate}],
        [sion], [scorep_otf2_have_sion=yes])
done
AS_IF([test "x$scorep_otf2_have_sion" = "xyes"], [
    AC_DEFINE([HAVE_OTF2_SUBSTRATE_SION], [1], [Define if the used OTF2 library has SIONlib support.])
])
AFS_SUMMARY([SIONlib support], [$scorep_otf2_have_sion])

scorep_otf2_have_zlib=no
for compression in `$scorep_otf2_config $scorep_otf2_config_backend_arg --features=compressions`
do
    AS_CASE([${compression}],
        [zlib], [scorep_otf2_have_zlib=yes])
done
AS_IF([test "x$scorep_otf2_have_zlib" = "xyes"], [
    AC_DEFINE([HAVE_OTF2_COMPRESSION_ZLIB], [1], [Define if the used OTF2 library has zlib compression support.])
])
AFS_SUMMARY([zlib support], [$scorep_otf2_have_zlib])

AFS_SUMMARY_SECTION_END # OTF2 features

opari2_fix_and_free_form_options=1
AS_IF([test -n "${scorep_opari2_bindir}"],
      [AC_SUBST([OPARI2_BINDIR],            ["${scorep_opari2_bindir}"])
       AC_SUBST([OPARI2_MAKE_CHECK_BINDIR], ["${scorep_opari2_bindir}"])
       AC_SUBST([OPARI2_CPPFLAGS],          ["`${scorep_opari2_bindir}/opari2-config --cflags`"])
       AC_DEFINE_UNQUOTED([HAVE_BACKEND_OPARI2_REVISION], [`${scorep_opari2_bindir}/opari2-config --opari2-revision`],
                 [OPARI2 revision used for version-dependent feature activation.])
       AC_SUBST([HAVE_EXTERNAL_OPARI2],     [1])
       AC_SUBST([OPARI2_REGION_INFO_SRC_DIR],["${scorep_opari2_bindir}/../share/opari2/devel"])
       AC_SUBST([OPARI2_REGION_INFO_INC_DIR],["${scorep_opari2_bindir}/../share/opari2/devel"])
       ${scorep_opari2_bindir}/opari2-config --fix-form > /dev/null 2>&1
       AS_IF([test $? -ne 0], [opari_fix_and_free_form_options=0])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OPARI2], [true])],
      [AC_SUBST([OPARI2_BINDIR],            ["${BINDIR}"])
       AC_SUBST([OPARI2_MAKE_CHECK_BINDIR], ["../vendor/opari2/build-frontend"])
       AC_SUBST([OPARI2_CPPFLAGS],          ['-I$(srcdir)/../vendor/opari2/include'])
       AC_SUBST([HAVE_EXTERNAL_OPARI2],     [0])
       AC_SUBST([OPARI2_REGION_INFO_SRC_DIR],["../vendor/opari2/src/opari-lib-dummy"])
       AC_SUBST([OPARI2_REGION_INFO_INC_DIR],['$(srcdir)/../vendor/opari2/src/opari-lib-dummy'])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OPARI2], [false])])
AC_SUBST([HAVE_OPARI2_FIX_AND_FREE_FORM_OPTIONS], [${opari2_fix_and_free_form_options}])

AS_IF([test -n "${scorep_cube_bindir}"],
      [scorep_cube_config_backend_arg=""
       AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
             [scorep_cube_config_backend_arg="--target=backend"
              AS_IF([test "x${ac_scorep_platform}" = "xmic"],
                    [scorep_cube_config_backend_arg="--target=mic"])
             ])
       AC_SUBST([CUBE_WRITER_CPPFLAGS], ["`${scorep_cube_bindir}/cube-config --writer-cppflags`"])
       AC_SUBST([CUBE_WRITER_LIBS],     ["`${scorep_cube_bindir}/cube-config --writer-libs`"])
       AC_SUBST([CUBE_WRITER_LDFLAGS],  ["`${scorep_cube_bindir}/cube-config --writer-ldflags ${scorep_cube_config_backend_arg}`"])
       AC_SUBST([CUBE_BINDIR],          ["${scorep_cube_bindir}"])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_CUBE], [test 1 -eq 1])
      ],
      [AC_SUBST([CUBE_WRITER_CPPFLAGS], ['-I$(srcdir)/../vendor/cube/src/cubew -I../vendor/cube/src'])
       AC_SUBST([CUBE_WRITER_LIBS],     ['../vendor/cube/build-backend/libcube4w.la ../vendor/cube/build-backend/libcubewriter4.la'])
       AC_SUBST([CUBE_WRITER_LDFLAGS],  [])
       AS_IF([test "${afs_cross_compiling}" = xyes],
             [AC_SUBST([CUBE_BINDIR], ["../vendor/cube/build-frontend"])],
             [AC_SUBST([CUBE_BINDIR], ["../vendor/cube/build-backend"])])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_CUBE], [test 1 -eq 0])
      ])

# does this work on cross-compile machines?
AC_CHECK_FUNCS_ONCE([readlink])

AC_SCOREP_BUILD_MODE
AC_SCOREP_CUDA
SCOREP_CRAY_PMI

AC_SCOREP_CHECK_SIZEOF([int64_t])
if (( ac_cv_sizeof_void_p > ac_cv_sizeof_int64_t )); then
    AC_MSG_ERROR([sizeof(void*) > sizeof(int64_t): internal thread handling will not work.])
fi

AC_SCOREP_OPENMP
AFS_SUMMARY_SECTION_BEGIN
SCOREP_OPENMP_TPD
SCOREP_OPENMP_ANCESTRY
AFS_SUMMARY_SECTION_END

SCOREP_PDT
AC_SCOREP_COBI

SCOREP_TIMER
AC_SCOREP_COMPILER_INSTRUMENTATION
SCOREP_CHECK_PREINCLUDES

AC_SCOREP_ONLINE_ACCESS

SCOREP_MEMORY_SUPPORT

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

# assuming that shmem configure already ran
AC_SCOREP_COND_HAVE([SHMEM_SUPPORT],
                    [test -f ../build-shmem/Makefile],
                    [Defined if SHMEM support is available.])
AC_SCOREP_COND_HAVE([SHMEM_PROFILING_INTERFACE],
                    [test -f ../build-shmem/scorep_have_pshmem_support.txt],
                    [Defined if SHMEM Profiling Interface support is available.])

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

AC_SUBST([SYS_LIB_DLSEARCH_PATH_SPEC], [${sys_lib_dlsearch_path_spec}])

scorep_list_of_wrappers=
additional_compiler_prefix_list=
AM_COND_IF([HAVE_MPI_SUPPORT], [
    AS_VAR_APPEND([additional_compiler_prefix_list], [" MPI"])
])
AM_COND_IF([HAVE_SHMEM_SUPPORT], [
    AS_VAR_APPEND([additional_compiler_prefix_list], [" SHMEM"])
])
for compiler_prefix in '' ${additional_compiler_prefix_list}; do
    for compiler_var in CC CXX F77 FC; do
        full_compiler_var=${compiler_prefix}${compiler_var}
        if eval test x\${${full_compiler_var}+set} != xset; then
            continue
        fi
        eval value=\$${full_compiler_var}

        # strip of compiler flags
        value="$( (set -- $value; echo $1) )"

        # strip of path elements
        value=${value##*/}

        # avoid duplicates
        case " ${scorep_list_of_wrappers} " in
        (*" scorep-$value "*)
            continue
        ;;
        esac

        AS_VAR_APPEND([scorep_list_of_wrappers], ["scorep-$value "])
    done
done

# add nvcc wrapper
AM_COND_IF([HAVE_CUDA_SUPPORT], [
    case " ${scorep_list_of_wrappers} " in
    (*" scorep-nvcc "*) : ignore duplicate ;;
    (*)
        AS_VAR_APPEND([scorep_list_of_wrappers], ["scorep-nvcc "])
    ;;
    esac
])

AC_CONFIG_FILES([scorep-wrapper:../src/tools/wrapper/scorep-wrapper.in],
                [chmod +x scorep-wrapper])
AC_SUBST([SCOREP_WRAPPER_NAMES], [${scorep_list_of_wrappers}])
AFS_SUMMARY([Wrappers], [${scorep_list_of_wrappers}])

AC_CONFIG_FILES([Makefile], [
if test "x${ac_scorep_cross_compiling}" = "xyes"; then
    sed -e 's/_0 = @echo "  \(YACC\|GEN\|LEX\)/&  /; t; s/_0 = @echo "  /&BE/' \
    Makefile >Makefile.be && mv -f Makefile.be Makefile || rm -f Makefile.be
fi
], [ac_scorep_cross_compiling='$ac_scorep_cross_compiling'])

AC_CONFIG_FILES([../installcheck/instrumenter_checks/Makefile.common_rules:../test/instrumenter_checks/Makefile.common_rules.in
                 ../installcheck/instrumenter_checks/Makefile.instrument_configurations:../test/instrumenter_checks/Makefile.instrument_configurations.in
                 ../installcheck/instrumenter_checks/serial/Makefile:../test/instrumenter_checks/serial/Makefile.in
                 ../installcheck/instrumenter_checks/omp/Makefile:../test/instrumenter_checks/omp/Makefile.in
                 ../src/scorep_config_tool_backend.h:../src/tools/config/scorep_config_tool_backend.h.in])
AM_COND_IF([HAVE_GNU_LINKER],
    [AM_COND_IF([HAVE_PTHREAD],
         [AC_CONFIG_FILES([../installcheck/instrumenter_checks/create_wait/pthread/Makefile:../test/instrumenter_checks/create_wait/pthread/Makefile.in])])])
AM_COND_IF([HAVE_MEMORY_SUPPORT],
    [AC_CONFIG_FILES([../installcheck/instrumenter_checks/memory/Makefile:../test/instrumenter_checks/memory/Makefile.in])])
AC_CONFIG_FILES([../test/adapters/opencl/run_opencl_test.sh], \
                [chmod +x ../test/adapters/opencl/run_opencl_test.sh])
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
AC_CONFIG_FILES([../installcheck/instrumenter_checks/instrumenter-configurations.sh:../test/instrumenter_checks/instrumenter-configurations.sh.in],
                [chmod +x ../installcheck/instrumenter_checks/instrumenter-configurations.sh])
AC_CONFIG_FILES([../installcheck/instrumenter_checks/check-instrumentation.sh:../test/instrumenter_checks/check-instrumentation.sh.in],
                [chmod +x ../installcheck/instrumenter_checks/check-instrumentation.sh])
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
AC_CONFIG_FILES([../installcheck/constructor_checks/bin/run_constructor_checks.sh:../test/constructor_checks/run_constructor_checks.sh.in],
                [chmod +x ../installcheck/constructor_checks/bin/run_constructor_checks.sh])
AC_CONFIG_FILES([../installcheck/constructor_checks/generate_makefile.sh:../test/constructor_checks/generate_makefile.sh.in],
                [chmod +x ../installcheck/constructor_checks/generate_makefile.sh])

AC_CONFIG_FILES([run_cct_tests.sh:../test/services/unwinding/run_cct_tests.sh.in],
                [chmod +x run_cct_tests.sh])
AC_CONFIG_FILES([../test/tools/wrapper/run_wrapper_checks.sh],
                [chmod +x ../test/tools/wrapper/run_wrapper_checks.sh])

#AFS_LIBTOOL_FIX_LINKER_PREFERENCE
AS_IF([test "x${enable_shared}" = xno],
    [AS_CASE([${ac_scorep_platform}],
         [bgq|crayx*|k|fx10*],
              [AC_CONFIG_COMMANDS([libtool-fix-linker-preference],
                   [sed -e '1,/prefer_static_libs=/ s/prefer_static_libs=.*$/prefer_static_libs=yes/' \
                    libtool > libtool.fix && mv libtool.fix libtool])
              ])
    ])

# On Cray, libunwind.la's dependecy_libs contain -lc which leads to a
# "multiple definition of `__lll_unlock_wake_private'" error. The patch
# below works around this issue. A better solution would be to fix the
# .la file or to provide a libunwind together with scorep as the
# side effects of this patch aren't investigated.
AS_CASE([${ac_scorep_platform}],
    [crayx*],
         [AC_CONFIG_COMMANDS([libtool-fix-cray-libunwind],
              [sed -i -e 's/for search_ext in \.la /for search_ext in /' libtool])
         ])

AC_CONFIG_FILES([../share/opencl.wrap:../share/opencl.wrap.in],
                [printf '\x2dwrap %s\n' $scorep_opencl_wrap_symbols >../share/opencl.wrap],
                [scorep_opencl_wrap_symbols='$scorep_opencl_wrap_symbols'])
AC_CONFIG_FILES([../share/opencl.nvcc.wrap:../share/opencl.wrap.in],
                [printf '\x2dXlinker \x2dwrap \x2dXlinker %s\n' $scorep_opencl_wrap_symbols >../share/opencl.nvcc.wrap \
                 && sed -i '0,/-Xlinker\ /s/-Xlinker\ //' ../share/opencl.nvcc.wrap],
                [scorep_opencl_wrap_symbols='$scorep_opencl_wrap_symbols'])

AC_OUTPUT
