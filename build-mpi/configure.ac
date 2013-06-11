AC_PREREQ([2.68])
AC_INIT([Score-P], m4_esyscmd([../vendor/common/build-config/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../vendor/common/build-config/generate-library-version.sh ../build-config/VERSION]))

AC_CONFIG_AUX_DIR([../build-config])
AC_CONFIG_MACRO_DIR([../vendor/common/build-config/m4])

AC_CONFIG_HEADERS([../src/config-backend-mpi.h])

AC_DEFUN([AC_SCOREP_BACKEND_MPI], [])

AC_SCOREP_SUMMARY_INIT([MPI backend])

## require $ac_scorep_cross_compiling to be set to 'yes' or 'no'
if test "x${ac_scorep_cross_compiling}" != "xyes" -a "x${ac_scorep_cross_compiling}" != "xno"; then
    AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])
fi

cross_compiling=${ac_scorep_cross_compiling}

ac_scorep_backend=yes
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [ac_scorep_frontend="no"
       scorep_otf2_config_backend_arg="--backend"],
      [ac_scorep_frontend="yes"])

#if test "x${ac_scorep_platform}" = "xaix"; then
    cross_compiling=yes
    # on power6 systems with poe, e.g. JUMP, we cannot execute a MPI
    # program on the login node. By setting cross_compiling temporarily
    # to "yes", we allow configure to succeed. Wrap all macros that might
    # execute a program in a cross_compiling=yes block, e.g. AC_SCOREP_CHECK_SIZEOF
#fi

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

AC_LANG_PUSH([C])
AC_SCOREP_MPI_WORKING([scorep_mpi_c_supported="yes"],[scorep_mpi_c_supported="no"])
AC_SCOREP_CHECK_SIZEOF([void *])
AC_LANG_POP([C])
cross_compiling=${ac_scorep_cross_compiling}
AC_SCOREP_SUMMARY([MPI support for C], [$scorep_mpi_c_supported])
AS_IF([test "x$scorep_mpi_c_supported" = "xyes"], [
    AC_SCOREP_SUMMARY([C compiler used for MPI], [$CC])
])

# C++ MPI is needed for the jacobi C++ tests only.
AC_LANG_PUSH([C++])
AC_SCOREP_MPI_WORKING([scorep_mpi_cxx_supported="yes"],[scorep_mpi_cxx_supported="no"])
AC_LANG_POP([C++])
AC_SCOREP_SUMMARY([MPI support C++], [$scorep_mpi_cxx_supported])
AS_IF([test "x$scorep_mpi_cxx_supported" = "xyes"], [
    AC_SCOREP_SUMMARY([C++ compiler used for MPI], [$CXX])
])

AC_LANG_PUSH([Fortran 77])
AC_SCOREP_MPI_WORKING([scorep_mpi_f77_supported="yes"],[scorep_mpi_f77_supported="no"])
AC_LANG_POP([Fortran 77])
AC_SCOREP_SUMMARY([MPI support F77], [$scorep_mpi_f77_supported])
AS_IF([test "x$scorep_mpi_f77_supported" = "xyes"], [
    AC_SCOREP_SUMMARY([F77 compiler used for MPI], [$F77])
])

AC_LANG_PUSH([Fortran])
AC_SCOREP_MPI_WORKING([scorep_mpi_f90_supported="yes"],[scorep_mpi_f90_supported="no"])
AC_LANG_POP([Fortran])
AC_SCOREP_SUMMARY([MPI support F90], [$scorep_mpi_f90_supported])
AS_IF([test "x$scorep_mpi_f90_supported" = "xyes"], [
    AC_SCOREP_SUMMARY([F90 compiler used for MPI], [$F90])
])

AS_IF([test "x$scorep_mpi_c_supported$scorep_mpi_cxx_supported$scorep_mpi_f77_supported$scorep_mpi_f90_supported" != "xnononono"], [
    AC_SCOREP_SUMMARY([Library used for MPI], [$MPILIBS])
])


AC_SCOREP_SVN_CONTROLLED

AM_INIT_AUTOMAKE([foreign color-tests 1.11.1 -Wall tar-pax])
AM_SILENT_RULES([yes])

AC_SCOREP_COMMON_PACKAGE([scorep])

AM_CONDITIONAL([BUILD_TESTS], [test "x${ac_scorep_cross_compiling}" = "xno"])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])
AC_SCOREP_PLATFORM_SETTINGS   

AC_SCOREP_COMMON_CHECKS
AC_SCOREP_COMPILER_INSTRUMENTATION_FLAGS
AC_SCOREP_BACKEND_TEST_RUNS
AC_SCOREP_ATTRIBUTE_ALIGNMENT
AC_SCOREP_FORTRAN_SUPPORT_ALLOCATABLE
AC_SCOREP_LIBSION([MPI])
AC_SCOREP_LIBPAPI
AC_SCOREP_RUSAGE
AC_SCOREP_METRIC_PLUGINS
AC_SCOREP_SAMPLING
AC_SCOREP_LIBUNWIND

AS_IF([test -n "${scorep_otf2_bindir}"],
      [AC_SUBST([OTF2_CPPFLAGS], ["`${scorep_otf2_bindir}/otf2-config --cppflags ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_LIBS],     ["`${scorep_otf2_bindir}/otf2-config --libs ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_LDFLAGS],  ["`${scorep_otf2_bindir}/otf2-config --ldflags ${scorep_otf2_config_backend_arg}`"])
       AC_SUBST([OTF2_BINDIR],   ["${scorep_otf2_bindir}"])],
      [AC_SUBST([OTF2_CPPFLAGS], ['-I../vendor/otf2/include -I$(srcdir)/../vendor/otf2/include'])
       AC_SUBST([OTF2_LIBS],     [../vendor/otf2/build-backend/libotf2.la])
       AC_SUBST([OTF2_LDFLAGS],  [])
       AC_SUBST([OTF2_BINDIR],   [../vendor/otf2/build-backend])])


AS_IF([test -n "${scorep_opari2_bindir}"],
      [AC_SUBST([OPARI2_CPPFLAGS], ["`${scorep_opari2_bindir}/opari2-config --cflags`"])],
      [AC_SUBST([OPARI2_CPPFLAGS], ['-I$(srcdir)/../vendor/opari2/include'])])

# we need an openmpi conditional because of a bug in the pgi-openmpi
# configuration. We might extend this hack for other MPI
# implementations.
$CC -showme:version 2>&1 | grep -q "Open MPI" > /dev/null
AM_CONDITIONAL([SCOREP_MPI_OPENMPI], [test $? -eq 0])

AM_PROG_AS

LT_PREREQ([2.4])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

AC_SCOREP_BUILD_MODE
AC_SCOREP_CUDA

# On crosscompile machines, install backend stuff into $(bin|lid)dir/backend.
backend_suffix=""
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [backend_suffix="/backend"])
AC_SUBST([backend_suffix])

## Determine a Fortran 77 compiler to use. If F77 is not already set in the
## environment, then check for g77 and f77, and then some other names. Set the
## output variable F77 to the name of the compiler found.
##
## This macro may, however, be invoked with an optional first argument which,
## if specified, must be a blank-separated list of Fortran 77 compilers to
## search for. This just gives the user an opportunity to specify an
## alternative search list for the Fortran 77 compiler. For example, if you
## didn't like the default order, then you could invoke AC_PROG_F77 like this:
##          AC_PROG_F77([fl32 f77 fort77 xlf g77 f90 xlf90])
## If using g77 (the GNU Fortran 77 compiler), then set the shell variable G77
## to 'yes'. If the output variable FFLAGS was not already set in the
## environment, then set it to -g -02 for g77 (or -O2 where g77 does not
## accept -g). Otherwise, set FFLAGS to -g for all other Fortran 77 compilers.
AC_PROG_F77

## Determine a Fortran compiler to use. If FC is not already set in the
## environment, then dialect is a hint to indicate what Fortran dialect to
## search for; the default is to search for the newest available dialect. Set
## the output variable FC to the name of the compiler found.
##
## By default, newer dialects are preferred over older dialects, but if
## dialect is specified then older dialects are preferred starting with the
## specified dialect. dialect can currently be one of Fortran 77, Fortran 90,
## or Fortran 95. However, this is only a hint of which compiler name to
## prefer (e.g., f90 or f95), and no attempt is made to guarantee that a
## particular language standard is actually supported. Thus, it is preferable
## that you avoid the dialect option, and use AC_PROG_FC only for code
## compatible with the latest Fortran standard.
##
## This macro may, alternatively, be invoked with an optional first argument
## which, if specified, must be a blank-separated list of Fortran compilers to
## search for, just as in AC_PROG_F77.
##
## If the output variable FCFLAGS was not already set in the environment, then
## set it to -g -02 for GNU g77 (or -O2 where g77 does not accept
## -g). Otherwise, set FCFLAGS to -g for all other Fortran compilers.
AC_PROG_FC


## Test whether the Fortran compiler accepts the options -c and -o
## simultaneously, and define F77_NO_MINUS_C_MINUS_O or FC_NO_MINUS_C_MINUS_O,
## respectively, if it does not.
AC_PROG_F77_C_O
AC_PROG_FC_C_O


## Determine the linker flags (e.g., -L and -l) for the Fortran intrinsic and
## runtime libraries that are required to successfully link a Fortran program
## or shared library. The output variable FLIBS or FCLIBS is set to these
## flags (which should be included after LIBS when linking).
##
## This macro is intended to be used in those situations when it is necessary
## to mix, e.g., C++ and Fortran source code in a single program or shared
## library (see Mixing Fortran 77 With C and C++).
##
## For example, if object files from a C++ and Fortran compiler must be linked
## together, then the C++ compiler/linker must be used for linking (since
## special C++-ish things need to happen at link time like calling global
## constructors, instantiating templates, enabling exception support, etc.).
##
## However, the Fortran intrinsic and runtime libraries must be linked in as
## well, but the C++ compiler/linker doesn't know by default how to add these
## Fortran 77 libraries. Hence, this macro was created to determine these
## Fortran libraries.
##
## The macros AC_F77_DUMMY_MAIN and AC_FC_DUMMY_MAIN or AC_F77_MAIN and
## AC_FC_MAIN are probably also necessary to link C/C++ with Fortran; see
## the autoconf manual.

# Should not be needed as we link the libscorep_* libs with the fortran 
# compiler. Users of libscorep_* use the appropriate compiler anyhow.
# Well , these macros are implicitly called by AC_F7*_WRAPPERS. On Cray
# calls to AC_F7*_WRAPPERS produce linker errors that can be fixed by 
# removing "-ltcmalloc_minimal" from FLIBS and FCLIBS BEFORE calling
# AC_F7*_WRAPPERS macros. 
AC_F77_LIBRARY_LDFLAGS
AC_FC_LIBRARY_LDFLAGS
AS_IF([test "x${ac_scorep_platform}" = "xcrayxt"],
      [FLIBS=`echo ${FLIBS} | sed -e 's/-ltcmalloc_minimal //g'`
       FCLIBS=`echo ${FCLIBS} | sed -e 's/-ltcmalloc_minimal //g'`])
AC_F77_WRAPPERS
AC_FC_WRAPPERS

AC_SCOREP_OPENMP
AC_SCOREP_MPI
AC_SCOREP_COMPILER_INSTRUMENTATION

AC_SCOREP_ONLINE_ACCESS

AC_SCOREP_MPI_PROFILING_HEADERS
AC_SCOREP_PACKAGE_AND_LIBRARY_VERSION([../])

AC_CONFIG_FILES([Makefile], [
sed -e 's/_0 = @echo "  \(YACC\|GEN\|LEX\)/&   /; t; s/_0 = @echo "  /&MPI/' \
Makefile >Makefile.mpi && mv -f Makefile.mpi Makefile || rm -f Makefile.mpi
])
AC_CONFIG_FILES([run_otf2_speed_test.sh \
                 ../test/jacobi/MPI/C/Makefile \
                 ../test/jacobi/hybrid/C/Makefile \
                 ../installcheck/mpi/Makefile:../test/installcheck/mpi/Makefile.in
                 ../installcheck/mpi_omp/Makefile:../test/installcheck/mpi_omp/Makefile.in
                 ../src/scorep_config_tool_mpi.h:../src/tools/config/scorep_config_tool_mpi.h.in])
AC_CONFIG_FILES([../test/mpi_omp/run_metric_collection_test.sh], \
                [chmod +x ../test/mpi_omp/run_metric_collection_test.sh])
AC_CONFIG_FILES([../test/OA/MPI/C/run_oa_c_test_mpi.sh], \
                [chmod +x ../test/OA/MPI/C/run_oa_c_test_mpi.sh])

AC_OUTPUT
