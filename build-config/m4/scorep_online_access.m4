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
dnl Copyright (c) 2009-2012,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2012, 2017,
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
ac_scorep_have_online_access="no"
ac_scorep_have_online_access_flex="no"
ac_scorep_have_online_access_yacc="no"
ac_scorep_have_online_access_headers="yes"
ac_scorep_have_online_access_getaddrinfo="no"

AC_CHECK_HEADERS([stdio.h strings.h ctype.h netdb.h sys/types.h sys/socket.h arpa/inet.h netinet/in.h unistd.h string.h],
                 [],
                 [ac_scorep_have_online_access_headers="no"])
AS_CASE([${build_os}],
         [aix*],
         [AC_CHECK_DECL([getaddrinfo],
	                     [ac_scorep_have_online_access_getaddrinfo="yes"],
	                     [ac_scorep_have_online_access_getaddrinfo="no"],
	                     [[
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <netdb.h>
	                     ]])],

         [AC_CHECK_DECL([getaddrinfo],
	                     [ac_scorep_have_online_access_getaddrinfo="yes"],
	                     [ac_scorep_have_online_access_getaddrinfo="no"],
	                     [[
            #define _POSIX_C_SOURCE 200112L
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <netdb.h>
		              	 ]])]
	   )


AC_PROG_LEX
AS_IF([test "x${LEX}" != "x:"],
    [AC_MSG_CHECKING([for a suitable version of flex])
     flex_version_full=`${LEX} -V | ${SED} 's/[[a-zA-Z]]//g'`
     flex_version=`echo "${flex_version_full}" | ${SED} 's/\.//g'`
     AS_IF([test "${flex_version}" -gt 254],
         [ac_scorep_have_online_access_flex=yes
          AC_MSG_RESULT([${flex_version_full}])],
         [AC_MSG_RESULT([none (${flex_version_full}, need > 2.5.4)])])])
dnl remaining AM_PROG_LEX part that is not covered by AC_PROG_LEX
AS_IF([test "x$LEX" = "x:"],
    [LEX=${am_missing_run}flex])

AC_PROG_YACC
AS_IF([test "x${YACC}" != x],
      [ac_scorep_have_online_access_yacc=yes])

AS_IF([test "x${ac_scorep_have_online_access_headers}" = "xyes" && \
       test "x${ac_scorep_have_online_access_flex}" = "xyes" && \
       test "x${ac_scorep_have_online_access_getaddrinfo}" = "xyes" && \
       test "x${ac_scorep_have_online_access_yacc}" = "xyes"],
      [ac_scorep_have_online_access="yes"])

AS_IF([test "x${ac_scorep_platform}" = "xbgp" || \
       test "x${ac_scorep_platform}" = "xbgl"],
      [ac_scorep_have_online_access="no"])

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
AFS_SUMMARY([Online access support], [${ac_scorep_have_online_access}])
])
