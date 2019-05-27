dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file scorep_macos.m4

# SCOREP_MACOS_GETEXEC
# --------------------
# Check for macOS _NSGetExecutablePath
# Defines HAVE_MACOS_GETEXEC.
AC_DEFUN([SCOREP_MACOS_GETEXEC], [
AC_LANG_PUSH([C])
AC_MSG_CHECKING([for _NSGetExecutablePath])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([[
#include <sys/param.h>
#include <mach-o/dyld.h>]],
[[char path[ MAXPATHLEN ];
uint32_t size = sizeof( path );
int ret = _NSGetExecutablePath( path, &size );
]])],
    [AC_MSG_RESULT([yes])
     AC_DEFINE([HAVE_MACOS_GETEXEC], [1], [Can link macOS _NSGetExecutablePath])],
    [AC_MSG_RESULT([no])])
AC_LANG_POP([C])
]) # SCOREP_MACOS_GETEXEC

# SCOREP_MACOS_LIBPROC
# --------------------
# Check for macOS proc_pidpath
# Defines HAVE_MACOS_LIBPROC.
AC_DEFUN([SCOREP_MACOS_LIBPROC], [
AC_LANG_PUSH([C])
AC_MSG_CHECKING([for proc_pidpath])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([[
#include <unistd.h>
#include <libproc.h>]],
[[char path[ PROC_PIDPATHINFO_MAXSIZE ];
int ret = proc_pidpath( getpid(), path, sizeof( path ) );
]])],
    [AC_MSG_RESULT([yes])
     AC_DEFINE([HAVE_MACOS_LIBPROC], [1], [Can link macOS proc_pidpath])],
    [AC_MSG_RESULT([no])])
AC_LANG_POP([C])
]) # SCOREP_MACOS_LIBPROC
