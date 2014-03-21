dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2012,
dnl    RWTH Aachen, Germany
dnl    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl    Technische Universitaet Dresden, Germany
dnl    University of Oregon, Eugene, USA
dnl    Forschungszentrum Juelich GmbH, Germany
dnl    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl    Technische Universitaet Muenchen, Germany
dnl
dnl See the COPYING file in the package base directory for details.
dnl

dnl file build-config/m4/scorep_online_access.m4

AC_DEFUN([AC_SCOREP_ONLINE_ACCESS],
[
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
	    #define _POSIX_SOURCE
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <netdb.h>
		              	 ]])]
	   )


AM_PROG_LEX
AC_MSG_CHECKING([for a suitable version of flex])
[flex_version=`${LEX} -V | sed 's/[a-z,\.]//g'`]
AS_IF([test "x${LEX}" != "x:" && test ${flex_version} -gt 254],
      [ac_scorep_have_online_access_flex="yes"],
      [])
AC_MSG_RESULT([${ac_scorep_have_online_access_flex}])

AC_PROG_YACC
AS_IF([test "${YACC}" != "yacc"],
      [ac_scorep_have_online_access_yacc="yes"],
      [])

AS_IF([test "x${ac_scorep_have_online_access_headers}" = "xyes" && \
       test "x${ac_scorep_have_online_access_flex}" = "xyes" && \
       test "x${ac_scorep_have_online_access_getaddrinfo}" = "xyes" && \
       test "x${ac_scorep_have_online_access_yacc}" = "xyes"],
      [ac_scorep_have_online_access="yes"],
      [])

AS_IF([test "x${ac_scorep_platform}" = "xbgp" || \
       test "x${ac_scorep_platform}" = "xbgl"],
      [ac_scorep_have_online_access="no"],
      [])

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
