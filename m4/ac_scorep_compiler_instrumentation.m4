## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012, 
##    RWTH Aachen University, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##


AC_DEFUN([AC_SCOREP_COMPILER_INSTRUMENTATION], [
AC_REQUIRE([AC_SCOREP_COMPILER_CHECKS])

have_compiler_instrumentation="yes"
scorep_have_demangle="no"

AS_IF([test "x${ac_scorep_compiler_gnu}" = "xyes" || test "x${ac_scorep_compiler_intel}" = "xyes"],
      [AC_SCOREP_BACKEND_LIB([libbfd], [bfd.h])
       AS_IF([test "x${scorep_have_libbfd}" = "xyes"],
             [result=${libbfd_result}],
             [# search for nm if bfd is not usable
              AC_MSG_WARN([libbfd not available. Trying compiler instrumentation via nm.])
              AC_CHECK_PROG([scorep_have_nm], [nm], ["yes"], ["no"])
              AS_IF([test "x${scorep_have_nm}" = "xno"],
                    [have_compiler_instrumentation="no"
                     AC_MSG_WARN([Neither libbfd nor nm are available. Compiler instrumentation will not work.])
                     result="no, neither libbfd nor nm are available."],
                    [result="yes, using nm"])])],
      [# non-gnu, non-intel compilers
       AM_CONDITIONAL(HAVE_LIBBFD, [test 1 -eq 0])
       AS_IF([test "x${ac_scorep_compiler_sun}" = "xyes"],
             [result="partially, studio compiler supports just Fortran"],
             [result="yes"])])

AC_SCOREP_SUMMARY([compiler instrumentation], [${result}])

# setting output variables/defines, also set:
#  - automake conditional HAVE_LIBBFD
#  - CPP define HAVE_LIBBFD
#  - Makefile substitutions LIBBFD_(CPPFLAGS|LDFLAGS|LIBS)
AM_CONDITIONAL([HAVE_DEMANGLE],                 
               [test "x${scorep_have_demangle}" = "xyes"])
AM_CONDITIONAL([HAVE_NM_AS_BFD_REPLACEMENT],    
               [test "x${scorep_have_libbfd}" = "xno" && test "x${scorep_have_nm}" = "xyes"])
AM_CONDITIONAL([HAVE_COMPILER_INSTRUMENTATION], 
               [test "x${have_compiler_instrumentation}" = "xyes"])

AS_IF([test "x${scorep_have_demangle}" = "xyes"], 
      [AC_DEFINE([HAVE_DEMANGLE], [1], [Define if cplus_demangle is available.])])
AS_IF([test "x${scorep_have_libbfd}" = "xno" && test "x${scorep_have_nm}" = "xyes"],
      [AC_DEFINE([HAVE_NM_AS_BFD_REPLACEMENT], [1], [Define if nm is available as a libbfd replacement.])])

])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBBFD_LIB_CHECK], [
have_libbfd="no"
LIBS="-lbfd"

_AC_SCOREP_LIBBFD_LINK_TEST
AS_IF([test "x${have_libbfd}" = "xno"],
      [LIBS="${LIBS} -liberty"; 
       _AC_SCOREP_LIBBFD_LINK_TEST
       AS_IF([test "x${have_libbfd}" = "xno"],
             [LIBS="${LIBS} -lz";
              _AC_SCOREP_LIBBFD_LINK_TEST])])

AS_IF([test "x${have_libbfd}" = "xyes"],
      [# check for demangle, libbfd CPPFLAGS|LDFLAGS|LIBS apply
       AC_CHECK_HEADER([demangle.h])
       AC_MSG_CHECKING([for cplus_demangle])
       AC_LINK_IFELSE([AC_LANG_PROGRAM([[char* cplus_demangle( const char* mangled, int options );]],
                                       [[cplus_demangle("test", 27)]])],
                      [scorep_have_demangle="yes"
                       libbfd_success_post_result=" and cplus_demangle"],
                      [scorep_have_demangle="no"])
       AC_MSG_RESULT([${scorep_have_demangle}])])

with_$1_lib_checks_successful=${have_libbfd}
with_$1_libs=${LIBS}
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBBFD_LINK_TEST], [
AC_LINK_IFELSE([_AC_SCOREP_LIBBFD_TEST_PROGRAM],
               [have_libbfd="yes"], 
               [libbfd_link_test_save_LIBS="${LIBS}"
                LIBS="${LIBS} -ldl"
                AC_LINK_IFELSE([_AC_SCOREP_LIBBFD_TEST_PROGRAM],
                               [have_libbfd="yes"], 
                               [LIBS="${libbfd_link_test_save_LIBS}"])])
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_LIBBFD_TEST_PROGRAM], [
AC_LANG_PROGRAM([[
char bfd_init ();
char bfd_openr ();
char bfd_check_format ();
char bfd_close ();]],
                [[
bfd_init ();
bfd_openr ();
bfd_check_format ();
bfd_close ();]])
])
