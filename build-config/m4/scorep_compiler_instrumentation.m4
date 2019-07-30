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
## Copyright (c) 2009-2014, 2019,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2014,
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

AC_DEFUN([SCOREP_LIBBFD], [
scorep_have_demangle="no"
AS_CASE([${ac_scorep_platform}],
    [k|fx10|fx100], [_FUJITSU_LIBBFD_CHECK_WORKAROUND],
    [AC_SCOREP_BACKEND_LIB([libbfd], [bfd.h])])
AC_SCOREP_COND_HAVE([LIBBFD],
    [test "x${scorep_have_libbfd}" = "xyes"],
    [Define if libbfd is available.])

AC_SCOREP_COND_HAVE([DEMANGLE],
    [test "x${scorep_have_demangle}" = "xyes"],
    [Define if cplus_demangle is available.])
])

AC_DEFUN([AC_SCOREP_COMPILER_INSTRUMENTATION], [
AC_REQUIRE([AX_COMPILER_VENDOR])dnl
AC_REQUIRE([SCOREP_COMPILER_INSTRUMENTATION_FLAGS])dnl
AC_REQUIRE([SCOREP_LIBBFD])dnl

have_compiler_instrumentation=yes
AS_CASE([${ax_cv_c_compiler_vendor%/*}],
    [intel],    [],
    [sun],      [],
    [ibm],      [],
    [portland], [],
    [gnu],      [],
    [cray],     [],
    [fujitsu],  [],
    [clang],    [],
    [have_compiler_instrumentation=no
     compiler_instrumentation_result="no, compiler vendor '${ax_cv_c_compiler_vendor}' not supported."])dnl


dnl `which nm` is the correct one for BG and Cray, but
dnl wrong for NEC-SX, see opari2:ticket:54 and silc:ticket:620.
AC_CHECK_PROG([scorep_have_nm], [nm], ["`which nm`"], ["no"])
AC_SCOREP_COND_HAVE([NM],
    [test "x${scorep_have_nm}" != "xno"],
    [Define if nm is available.])

AS_IF([test "x${have_compiler_instrumentation}" = xyes],
    [AS_IF([test "x${scorep_compiler_instrumentation_needs_symbol_table}" = xyes],
         [pwd_save=`pwd`
          cd ${srcdir}/../
          scorep_abs_toplevel_srcdir=`pwd`
          cd ${pwd_save}
          AC_DEFINE_UNQUOTED([SCOREP_ABS_TOPLEVEL_SRCDIR],
              ["${scorep_abs_toplevel_srcdir}"],
              [Toplevel src directory])

          AM_COND_IF([HAVE_LIBBFD],
             [compiler_instrumentation_result="yes, using libbfd"],
             [# try nm if bfd is not available
              AM_COND_IF([HAVE_NM],
                  [compiler_instrumentation_result="yes, using nm"
                   AC_DEFINE_UNQUOTED([SCOREP_BACKEND_NM], ["${scorep_have_nm}"], [Backend nm.])],
                  [have_compiler_instrumentation="no"
                   compiler_instrumentation_result="no, neither libbfd nor nm are available"])])
          AS_IF([test "x${have_compiler_instrumentation}" = xyes && test "x${ax_cv_c_compiler_vendor%/*}" = xgnu],
	      [AC_MSG_WARN([Superior instrumentation mechanism for GNU compilers exists. We highly recommend installing files for GNU GCC plugin development, see INSTALL.])])
         ],
         [# compilers which do not need the symbol table
          AS_IF([test "x${ax_cv_c_compiler_vendor}" = xsun],
              [compiler_instrumentation_result="partially, studio compiler supports Fortran only."],
              [test "x${scorep_compiler_gnu_with_plugin}" = "xyes"],
              [compiler_instrumentation_result="yes, using GCC plug-in with support for compile-time filtering"],
              [compiler_instrumentation_result="yes"])
         ])
    ])
AFS_SUMMARY([compiler instrumentation], [${compiler_instrumentation_result}])

AC_SCOREP_COND_HAVE([COMPILER_INSTRUMENTATION],
    [test "x${have_compiler_instrumentation}" = xyes],
    [Defined if compiler instrumentation is available.])

AC_SCOREP_COND_HAVE([COMPILER_INSTRUMENTATION_NEEDS_SYMBOL_TABLE],
    [test "x${have_compiler_instrumentation}" = xyes &&
     test "x${scorep_compiler_instrumentation_needs_symbol_table}" = "xyes"],
    [Define if the compiler instrumentation needs the symbol table.])

])

dnl ----------------------------------------------------------------------------

# _FUJITSU_LIBBFD_CHECK_WORKAROUND
# Fujitsu are cross-compile machines, i.e. we explicitly need to specify the
# path to bfd. This path (which contains system stuff) is used in a CPPFLAGS.
# This breaks compilation. Therefore, work around the usual compiler
# instrumentation's AC_SCOREP_BACKEND_LIB check.
# -----------------------------------------------------------------------------
m4_define([_FUJITSU_LIBBFD_CHECK_WORKAROUND], [
BYPASS_GENERIC_LIB_CHECK_ON_FUJITSU([libbfd], [-lbfd -liberty])
AC_CHECK_HEADER([demangle.h])
AC_MSG_CHECKING([for cplus_demangle])
save_libs=${LIBS}
LIBS="${LIBBFD_LIBS} ${LIBS}"
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([[char* cplus_demangle( const char* mangled, int options );]],
                     [[cplus_demangle("test", 27)]])],
    [scorep_have_demangle=yes],
    [scorep_have_demangle=no])
LIBS=${save_libs}
AC_MSG_RESULT([${scorep_have_demangle}])
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
