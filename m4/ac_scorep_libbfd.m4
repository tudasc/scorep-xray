## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
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


AC_DEFUN([SCOREP_LIBBFD_TEST_PROGRAM], [
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


AC_DEFUN([SCOREP_LIBBFD_LINK_TEST], [
AC_LINK_IFELSE([SCOREP_LIBBFD_TEST_PROGRAM],
               [scorep_have_libbfd="yes"], 
               [scorep_libbfd_link_test_save_LIBS="${LIBS}"
                LIBS="${LIBS} -ldl"
                AC_LINK_IFELSE([SCOREP_LIBBFD_TEST_PROGRAM],
                               [scorep_have_libbfd="yes"], 
                               [LIBS="${scorep_libbfd_link_test_save_LIBS}"])])
])


AC_DEFUN([AC_SCOREP_LIBBFD], [
AC_REQUIRE([AC_SCOREP_COMPILER_CHECKS])

# What shall we do if there is a frontend and a backend libbfd? We need the
# backend version, but usually the frontend version will be found. One
# approach is to require user input on dual-architecture machines. But for now
# we will just abort (until this problem shows up in reality).
set -x
scorep_have_demangle="no"
scorep_have_libbfd="no"
scorep_have_nm="no"
scorep_bfd_libs=""
scorep_have_compiler_instrumentation="yes"

AS_IF([test "x${scorep_compiler_gnu}" = "xyes" || test "x${scorep_compiler_intel}" = "xyes"], 
      [AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
             [AC_MSG_ERROR([Can't reliably determine backend libbfd in cross compiling mode.])
              AC_MSG_ERROR([Can't reliably determine backend nm in cross compiling mode.])])

       AC_LANG_PUSH([C])
    
       AC_CHECK_HEADER([bfd.h])
    
       AC_MSG_CHECKING([for libbfd])
       scorep_libbfd_save_LIBS="${LIBS}"
       LIBS="-lbfd"

       SCOREP_LIBBFD_LINK_TEST
       AS_IF([test "x${scorep_have_libbfd}" = "xno"],
             [LIBS="${LIBS} -liberty"; 
              SCOREP_LIBBFD_LINK_TEST
              AS_IF([test "x${scorep_have_libbfd}" = "xno"],
                    [LIBS="${LIBS} -lz";
                     SCOREP_LIBBFD_LINK_TEST])])

       AS_IF([test "x${scorep_have_libbfd}" = "xyes"],
             [AC_MSG_RESULT([${LIBS}])
              # check for demangle
              AC_CHECK_HEADER([demangle.h])
              AC_MSG_CHECKING([for cplus_demangle])    
              AC_LINK_IFELSE([AC_LANG_PROGRAM([[char* cplus_demangle( const char* mangled, int options );]],
                                              [[cplus_demangle("test", 27)]])],
                             [scorep_have_demangle="yes"], 
                             [scorep_have_demangle="no"])
              AC_MSG_RESULT([${scorep_have_demangle}])],
             [AC_MSG_RESULT([no])])
    
       scorep_bfd_libs="${LIBS}"
       LIBS="${scorep_libbfd_save_LIBS}"

       # search for nm if bfd is not usable
       AS_IF([test "x${ac_cv_header_bfd_h}" = "xno" || test "x${scorep_have_libbfd}" = "xno"],
             [AC_MSG_WARN([libbfd not available. Trying compiler instrumentation via nm.])
              AC_CHECK_PROG([scorep_have_nm], [nm], ["yes"], ["no"])
              AS_IF([test "x${scorep_have_nm}" = "xno"],
                    [scorep_have_compiler_instrumentation="no"
                     AC_MSG_WARN([Neither libbfd nor nm are available. Compiler instrumentation will not work.])
                     AC_SCOREP_SUMMARY([compiler instrumentation], [no, neither libbfd nor nm are available.])])])

       AC_LANG_POP([C])])
set +x


# setting output variables/defines
AM_CONDITIONAL([HAVE_LIBBFD],                   
               [test "x${ac_cv_header_bfd_h}" = "xyes" && test "x${scorep_have_libbfd}" = "xyes"])
AM_CONDITIONAL([HAVE_DEMANGLE],                 
               [test "x${scorep_have_demangle}" = "xyes"])
AM_CONDITIONAL([HAVE_NM_AS_BFD_REPLACEMENT],    
               [test "x${scorep_have_nm}" = "xyes"])
AM_CONDITIONAL([HAVE_COMPILER_INSTRUMENTATION], 
               [test "x${scorep_have_compiler_instrumentation}" = "xyes"])

AC_SUBST([LIBBFD], ["${scorep_bfd_libs}"])

AS_IF([test "x${ac_cv_header_bfd_h}" = "xyes" && test "x${scorep_have_libbfd}" = "xyes"], 
      [AC_DEFINE([HAVE_LIBBFD], [1], [Define if libbfd is available.])
       AC_SCOREP_SUMMARY([compiler instrumentation], [yes, using libbfd (${scorep_bfd_libs})])])
AS_IF([test "x${scorep_have_demangle}" = "xyes"], 
      [AC_DEFINE([HAVE_DEMANGLE], [1], [Define if cplus_demangle is available.])])
AS_IF([test "x${scorep_have_nm}"       = "xyes"], 
      [AC_DEFINE([HAVE_NM], [1], [Define if nm is available.])
       AC_SCOREP_SUMMARY([compiler instrumentation], [yes, using nm])])
])
