dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2012,
dnl RWTH Aachen, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2012, 2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2012, 2017, 2019,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl Copyright (c) 2009-2012, 2016,
dnl Technische Universitaet Darmstadt, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_online_access.m4

AC_DEFUN([AC_SCOREP_ONLINE_ACCESS],
[
AC_REQUIRE([AC_PROG_SED])dnl
AC_REQUIRE([AC_PROG_LEX])dnl
AC_REQUIRE([AC_PROG_YACC])dnl

ac_scorep_have_online_access="yes"

scorep_online_access_reason=
AC_CHECK_HEADERS([stdio.h strings.h ctype.h netdb.h sys/types.h sys/socket.h arpa/inet.h netinet/in.h unistd.h string.h],
                 [],
                 [ac_scorep_have_online_access="no"
                  scorep_online_access_reason=", missing $ac_header header"])

AS_IF([test "x${ac_scorep_have_online_access}" = "xyes"], [
AS_CASE([${build_os}],
         [aix*],
         [AC_CHECK_DECL([getaddrinfo],
	                     [],
	                     [ac_scorep_have_online_access="no"
	                      scorep_online_access_reason=", missing getaddrinfo"],
	                     [[
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <netdb.h>
	                     ]])],

         [AC_CHECK_DECL([getaddrinfo],
	                     [],
	                     [ac_scorep_have_online_access="no"
	                      scorep_online_access_reason=", missing getaddrinfo"],
	                     [[
            #define _POSIX_C_SOURCE 200112L
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <netdb.h>
		              	 ]])]
	   )
])


AS_UNSET([scorep_online_access_hint])
AS_IF([test "x${ac_scorep_have_online_access}" = "xyes"], [
    # For the version check we need flex, not lex
    AS_IF([test "x${LEX}" = "xflex"],
        [AC_MSG_CHECKING([for a suitable version of flex])
         flex_version_full=`${LEX} -V | ${SED} 's/[[a-zA-Z]]//g'`
         flex_version=`echo "${flex_version_full}" | ${SED} 's/\.//g'`
         AS_IF([test "${flex_version}" -gt 254],
             [AC_MSG_RESULT([${flex_version_full}])],
             [scorep_online_access_hint="flex >= 2.5.4 instead of ${flex_version_full}"
              AC_MSG_RESULT([none (${flex_version_full}, need > 2.5.4)])])],
        [scorep_online_access_hint="flex"
         LEX=${am_missing_run}flex])

    AS_IF([test "x${YACC}" = xyacc],
        [AC_CHECK_PROG([scorep_yacc_exists], [yacc], [yes], [no])
         AS_IF([test "x${scorep_yacc_exists}" = xno],
             [scorep_online_access_hint="${scorep_online_access_hint:+${scorep_online_access_hint} and }yacc"
              YACC=${am_missing_run}yacc
              AS_UNSET([scorep_yacc_exists])])])])
AS_IF([test "x${scorep_online_access_hint}" != x ],
    [scorep_online_access_hint=" (note: development requires ${scorep_online_access_hint})"])

AC_ARG_ENABLE([online-access],
              [AS_HELP_STRING([--disable-online-access],
                              [Enable or disable Online Access. Fails if support cannot be satisfied but was requested.])],
              [AS_CASE([$enableval,$ac_scorep_have_online_access,${ac_scorep_platform}],
                       [*,*,bgp|*,*,bgq],
                       [ac_scorep_have_online_access="no"
                        scorep_online_access_reason=", unsupported on Blue Gene/P and Q"],
                       [yes,no,*],
                       [AC_MSG_ERROR([could not fulfill requested support for Online Access.])],
                       [no,yes,*],
                       [ac_scorep_have_online_access="no"
                        scorep_online_access_reason=", explicitly disabled via --disable-online-access"],
                       [yes,yes,*|no,no,*],
                       [:],
                       [AC_MSG_ERROR([invalid argument for --online-access: $enableval])])])


dnl case ${build_os} in
dnl             aix*)
dnl                 ac_scorep_have_online_access="no"
dnl             ;;
dnl esac

AC_SCOREP_COND_HAVE([ONLINE_ACCESS],
                    [test "x${ac_scorep_have_online_access}" = "xyes" ],
                    [Defined if online access is possible.])
AC_MSG_CHECKING([for online access possible])
AC_MSG_RESULT([${ac_scorep_have_online_access}])
AFS_SUMMARY([Online access support], [${ac_scorep_have_online_access}${scorep_online_access_reason}${scorep_online_access_hint}])
AS_UNSET([scorep_online_access_hint])
AS_UNSET([scorep_online_access_reason])

# for OA tests only
AC_CHECK_PROG([GDB], [gdb], [$(which gdb)], [no])
AM_CONDITIONAL([HAVE_GDB], [test "x${GDB}" != xno])
])
