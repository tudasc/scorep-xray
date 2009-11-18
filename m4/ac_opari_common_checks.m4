AC_DEFUN([AC_OPARI_COMMON_CHECKS],
[
## Determine a C++ compiler to use. Check whether the environment variable CXX 
## or CCC (in that order) is set; if so, then set output variable CXX to its 
## value.
## 
## Otherwise, if the macro is invoked without an argument, then search for a
## C++ compiler under the likely names (first g++ and c++ then other
## names). If none of those checks succeed, then as a last resort set CXX to
## g++.
## 
## This macro may, however, be invoked with an optional first argument which,
## if specified, must be a blank-separated list of C++ compilers to search
## for.  This just gives the user an opportunity to specify an alternative
## search list for the C++ compiler. For example, if you didn't like the
## default order, then you could invoke AC_PROG_CXX like this:
## AC_PROG_CXX([gcc cl KCC CC cxx cc++ xlC aCC c++ g++])
AC_PROG_CXX

## Enable using per-target flags or subdir-objects with C sources
AM_PROG_CC_C_O

## If the current language is C, the macro AC_OPENMP sets the variable
## OPENMP_CFLAGS to the C compiler flags needed for supporting
## OpenMP. OPENMP_CFLAGS is set to empty if the compiler already supports
## OpenMP, if it has no way to activate OpenMP support, or if the user rejects
## OpenMP support by invoking ‘configure’ with the ‘--disable-openmp’
## option.
##
## OPENMP_CFLAGS needs to be used when compiling programs, when preprocessing
## program source, and when linking programs. Therefore you need to add
## $(OPENMP_CFLAGS) to the CFLAGS of C programs that use OpenMP. If you
## preprocess OpenMP-specific C code, you also need to add $(OPENMP_CFLAGS) to
## CPPFLAGS. The presence of OpenMP support is revealed at compile time by the
## preprocessor macro _OPENMP.
##
## Linking a program with OPENMP_CFLAGS typically adds one more shared library
## to the program's dependencies, so its use is recommended only on programs
## that actually require OpenMP.
##
## If the current language is C++, AC_OPENMP sets the variable
## OPENMP_CXXFLAGS, suitably for the C++ compiler. The same remarks hold as
## for C.
##
## If the current language is Fortran 77 or Fortran, AC_OPENMP sets the
## variable OPENMP_FFLAGS or OPENMP_FCFLAGS, respectively. Similar remarks as
## for C hold, except that CPPFLAGS is not used for Fortran, and no
## preprocessor macro signals OpenMP support.
AC_OPENMP


## Determine a Fortran 77 compiler to use. If F77 is not already set in the
## environment, then check for g77 and f77, and then some other names. Set the
## output variable F77 to the name of the compiler found.
##
## This macro may, however, be invoked with an optional first argument which,
## if specified, must be a blank-separated list of Fortran 77 compilers to
## search for. This just gives the user an opportunity to specify an
## alternative search list for the Fortran 77 compiler. For example, if you
## didn't like the default order, then you could invoke AC_PROG_F77 like this:
##
##          AC_PROG_F77([fl32 f77 fort77 xlf g77 f90 xlf90])
##
## If using g77 (the GNU Fortran 77 compiler), then set the shell variable G77
## to ‘yes’. If the output variable FFLAGS was not already set in the
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
#AC_PROG_FC ([compiler-search-list], [dialect])

#AC_CXX_NAMESPACES
#AC_CXX_HAVE_SSTREAM
#AC_CXX_HAVE_STRSTREAM

AC_PROG_RANLIB
])
