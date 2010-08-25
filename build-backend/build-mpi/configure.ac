AC_PREREQ([2.65])
AC_INIT([silc-backend-mpi], [0])

## require $ac_silc_cross_compiling to be set to 'yes' or 'no'
if test "x${ac_silc_cross_compiling}" != "xyes" -a "x${ac_silc_cross_compiling}" != "xno"; then
    AC_MSG_ERROR([invalid value '$ac_silc_cross_compiling' for variable \$ac_silc_cross_compiling.], [1])
fi

AC_SILC_CHECK_COMPILER_VAR_SET([CC])
AC_SILC_CHECK_COMPILER_VAR_SET([CXX])
AC_SILC_CHECK_COMPILER_VAR_SET([F77])
AC_SILC_CHECK_COMPILER_VAR_SET([FC])

AC_LANG_PUSH([C])
AC_SILC_MPI_WORKING([silc_mpi_c_supported="yes"],[silc_mpi_c_supported="no"])
AC_LANG_POP([C])
#echo "silc_mpi_c_supported=$silc_mpi_c_supported"

# C++ MPI is needed for the jacobi C++ tests only.
AC_LANG_PUSH([C++])
AC_SILC_MPI_WORKING([silc_mpi_cxx_supported="yes"],[silc_mpi_cxx_supported="no"])
AC_LANG_POP([C++])
#echo "silc_mpi_cxx_supported=$silc_mpi_cxx_supported"

AC_LANG_PUSH([Fortran 77])
AC_SILC_MPI_WORKING([silc_mpi_f77_supported="yes"],[silc_mpi_f77_supported="no"])
AC_LANG_POP([Fortran 77])
#echo "silc_mpi_f77_supported=$silc_mpi_f77_supported"

AC_LANG_PUSH([Fortran])
AC_SILC_MPI_WORKING([silc_mpi_f90_supported="yes"],[silc_mpi_f90_supported="no"])
AC_LANG_POP([Fortran])
#echo "silc_mpi_f90_supported=$silc_mpi_f90_supported"

#echo "CC =$CC"
#echo "CXX=$CXX"
#echo "F77=$F77"
#echo "FC =$FC"
#echo "MPILIBS=$MPILIBS"

AC_SILC_SVN_CONTROLLED
AH_SILC_CONFIG_HEADER_CUSTOM
AC_CONFIG_HEADERS([../../src/config-backend-mpi.h])


AC_CONFIG_AUX_DIR([../../build-config])
AC_CONFIG_MACRO_DIR([../../build-config/m4])

AM_INIT_AUTOMAKE([foreign silent-rules color-tests 1.11 -Wall])
AM_CONDITIONAL([BUILD_TESTS], [test "x${ac_silc_cross_compiling}" = "xno"])
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_silc_cross_compiling}" = "xyes"])
AC_SILC_PLATFORM_SETTINGS   

AC_SILC_COMMON_CHECKS
AM_PROG_AS

LT_PREREQ([2.2.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

# don't use ${exec_prefix} here, evaluates to 'NONE' but use it in Makefile.am
backend_bin_dir="/bin"
backend_lib_dir="/lib"
if test "x${ac_silc_cross_compiling}" = "xyes"; then
    backend_bin_dir="/bin/backend"
    backend_lib_dir="/lib/backend"
fi
AC_SUBST(backend_bin_dir)
AC_SUBST(backend_lib_dir)

AC_CHECK_SIZEOF([void *])

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
AC_F77_LIBRARY_LDFLAGS
AC_FC_LIBRARY_LDFLAGS

AC_SILC_OPENMP
AC_SILC_COMPILER_CHECKS
AC_SILC_MPI

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
