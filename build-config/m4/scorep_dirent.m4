dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2017,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_dirent.m4


AC_DEFUN([SCOREP_DIRENT], [

AC_LANG_PUSH([C])

have_dirent_header="yes"
AC_CHECK_HEADERS([sys/types.h dirent.h],
                 [],
                 [have_dirent_header="no"])

have_dirent="no"
AS_IF([test "x${have_dirent_header}" = "xyes"],
    [AC_MSG_CHECKING([for opendir/readdir/closedir])
    AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
            [[
            #include <sys/types.h>
            #include <dirent.h>
            ]],
            [[
            DIR* dir = opendir(".");
            struct dirent* ent;
            while ( ( ent = readdir( dir ) ) )
            {
                const char* name = ent->d_name;
            }
            closedir( dir );
            ]])
        ],
        [have_dirent="yes"])
    AC_MSG_RESULT([${have_dirent}])])

AC_LANG_POP([C])

AS_IF([test "x${have_dirent}" = "xyes"],
    [AC_DEFINE([HAVE_DIRENT], [1], [Defined if opendir/readdir/closedir is supported.])])

AS_UNSET([have_dirent_header])
AS_UNSET([have_dirent])
])
