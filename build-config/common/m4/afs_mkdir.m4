dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2014,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2012,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file afs_mkdir.m4


# AFS_PORTABLE_MKDIR
# ------------------
# Check if mkdir accepts a mode_t as second parameter. If yes, define
# <AFS_PACKAGE_NAME>_MKDIR( path, mode ) to 'mkdir( path, mode )',
# else omit the second parameter and define it to 'mkdir( path )'.
# This is necessary as on some platforms, e.g. MinGW, the mode_t
# argument is not supported. On MinGW, include io.h conditionally to
# get rid of the 'implicit declaration' warning.
AC_DEFUN([AFS_PORTABLE_MKDIR], [

AC_LANG_PUSH([C])
AC_CHECK_HEADERS([io.h]) dnl MinGW declares mkdir here instead
                         dnl of POSIX sys/stat.h
AC_MSG_CHECKING([whether mkdir accepts a mode_t argument])
AC_COMPILE_IFELSE([
    AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <sys/stat.h>
#if HAVE_IO_H
#include <io.h>
#endif
        ]], [[
mode_t bar = 777;
mkdir("./foo", bar);
        ]])
    ],
    [AC_MSG_RESULT([yes])
     AFS_PACKAGE_name[]_mkdir="mkdir( path, mode )"],
    [AC_MSG_RESULT([no, omitting mode_t argument])
     AFS_PACKAGE_name[]_mkdir="mkdir( path )"])
AC_DEFINE_UNQUOTED([AFS_PACKAGE_NAME[]_MKDIR( path, mode )],
    [${AFS_PACKAGE_name[]_mkdir}],
    [Portable mkdir that handles implementations which do not have the mode_t argument])
AC_LANG_POP([C])

])
