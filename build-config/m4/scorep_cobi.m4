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


AC_DEFUN([AC_SCOREP_COBI],
[

dnl AC_ARG_WITH([cobi],
dnl             [AS_HELP_STRING([--with-cobi=(yes|no|<path-to-cobi-binary>)],
dnl                             [Whether to build with cobi binary instrumentation support. "yes" assumes it is in PATH [no].])],
dnl             # action-if-given
dnl             [AS_CASE([$withval],
dnl                      ["yes"], [scorep_with_cobi="yes"],
dnl                      ["no"],  [scorep_with_cobi="no"],
dnl                      [scorep_with_cobi="$withval"])],
dnl             # action-if-not-given
dnl             [scorep_with_cobi="no"])

have_cobi="no"
dnl AS_IF([test "x${scorep_with_cobi}" != "xno"],
dnl       [AS_IF([test "x${enable_shared}" = "xno"],
dnl              [AC_MSG_CHECKING([for cobi])
dnl               AC_MSG_RESULT([prerequisites missing])
dnl               AC_MSG_ERROR([Cobi support requires building shared libraries. Use --enable-shared to build shared libraries or omit the --with-cobi option.])])

dnl        AS_IF([test "x${scorep_with_cobi}" = "xyes"],
dnl              [AC_CHECK_PROG([COBI], [cobi], [`which cobi`], ["no"])],
dnl              [AC_CHECK_PROG([COBI], [cobi], [${scorep_with_cobi}/cobi], ["no"], [${scorep_with_cobi}])])
dnl        have_cobi="${COBI}"])

# The output of this macro
AM_CONDITIONAL([HAVE_COBI], [test "x${have_cobi}" != "xno"])

AS_IF([test "x${have_cobi}" != "xno"],
      [AC_DEFINE([HAVE_COBI], [1], [Defined if configured to use Cobi.])
       AC_DEFINE_UNQUOTED([SCOREP_COBI_PATH], ["${COBI}"], [Defines path to Cobi.])
       AFS_SUMMARY([Cobi support], [yes, using ${COBI}])],
      [AC_DEFINE([HAVE_COBI], [0], [Defined if configured to use Cobi.])
       AC_DEFINE_UNQUOTED([SCOREP_COBI_PATH], [""], [Defines path to Cobi.])
       AFS_SUMMARY([Cobi support], [no])])
])
