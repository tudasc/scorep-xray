## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

AC_DEFUN([AC_SCOREP_COMPILER_INSTRUMENTATION], [
AC_REQUIRE([AX_COMPILER_VENDOR])dnl
AC_REQUIRE([AC_SCOREP_COMPILER_INSTRUMENTATION_FLAGS])

have_compiler_instrumentation=yes
AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [],
    [sun],      [],
    [ibm],      [],
    [portland], [],
    [gnu],      [],
    [cray],     [],
    [have_compiler_instrumentation=no
     result="no, compiler vendor '${ax_cv_c_compiler_vendor}' not supported."])dnl

AS_IF([test "x${have_compiler_instrumentation}" = xyes],
    [scorep_have_demangle="no"
     AS_IF([test "x${ax_cv_c_compiler_vendor}" = xgnu || \
            test "x${ax_cv_c_compiler_vendor}" = xcray ],
         [AC_SCOREP_BACKEND_LIB([libbfd], [bfd.h])
          AS_IF([test "x${scorep_have_libbfd}" = "xyes"],
             [result=${libbfd_result}],
             [# search for nm if bfd is not usable
              AC_MSG_WARN([libbfd not available. Trying compiler instrumentation via nm.])
              AC_CHECK_PROG([scorep_have_nm], [nm], ["`which nm`"], ["no"])
              AS_IF([test "x${scorep_have_nm}" = "xno"],
                  [have_compiler_instrumentation="no"
                   AC_MSG_WARN([Neither libbfd nor nm are available. Compiler instrumentation will not work.])
                   result="no, neither libbfd nor nm are available."],
                  [result="yes, using nm."])])
         ],
         [# non-gnu, non-cray compilers
          AM_CONDITIONAL(HAVE_LIBBFD, [test 1 -eq 0])
          AS_IF([test "x${ax_cv_c_compiler_vendor}" = xsun],
              [result="partially, studio compiler supports Fortran only."],
              [result="yes"])
         ])
    ])
AFS_SUMMARY([compiler instrumentation], [${result}])

AM_CONDITIONAL([HAVE_COMPILER_INSTRUMENTATION], 
               [test "x${have_compiler_instrumentation}" = xyes])

# The following just deals with bfd, demangle, and nm
AM_CONDITIONAL([HAVE_DEMANGLE],                 
               [test "x${scorep_have_demangle}" = "xyes"])
AM_COND_IF([HAVE_DEMANGLE],
           [AC_DEFINE([HAVE_DEMANGLE], [1], [Define if cplus_demangle is available.])])

AS_IF([test "x${scorep_have_libbfd}" = "xno" && test "x${scorep_have_nm}" != "xno"],
      [AM_CONDITIONAL([HAVE_NM_AS_BFD_REPLACEMENT], [test 1 -eq 1])
       AC_DEFINE([HAVE_NM_AS_BFD_REPLACEMENT], [1], [Define if nm is available as a libbfd replacement.])
       dnl scorep_have_nm=`which nm` is the correct one for BG and Cray, but 
       dnl wrong for NEC-SX, see opari2:ticket:54 and silc:ticket:620.
       AC_DEFINE_UNQUOTED([SCOREP_BACKEND_NM], ["${scorep_have_nm}"], [Backend nm as bfd replacement])],
      [AM_CONDITIONAL([HAVE_NM_AS_BFD_REPLACEMENT], [test 1 -eq 0])])
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
