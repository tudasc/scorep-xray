## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2017,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2018, 2022,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


# POSIX access
# ------------
# Check whether the POSIX 'access' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_ACCESS], [
_AFS_CHECK_POSIX_DECL([C], [access], [#include <unistd.h>])
]) # AFS_C_DECL_POSIX_ACCESS

AC_DEFUN([AFS_CXX_DECL_POSIX_ACCESS], [
_AFS_CHECK_POSIX_DECL([C++], [access], [#include <unistd.h>])
]) # AFS_CXX_DECL_POSIX_ACCESS

AC_DEFUN([AFS_POSIX_ACCESS], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_ACCESS])
_AFS_CHECK_POSIX_SYMBOL([access],
    [#include <unistd.h>],
    [int access( const char* pathname, int mode );],
    [
int result = access( "/proc/self/exe", X_OK );
    ])
]) # AFS_POSIX_ACCESS


# POSIX closedir
# --------------
# Check whether the POSIX 'closedir' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_CLOSEDIR], [
_AFS_CHECK_POSIX_DECL([C], [closedir], [#include <dirent.h>])
]) # AFS_C_DECL_POSIX_CLOSEDIR

AC_DEFUN([AFS_CXX_DECL_POSIX_CLOSEDIR], [
_AFS_CHECK_POSIX_DECL([C++], [closedir], [#include <dirent.h>])
]) # AFS_CXX_DECL_POSIX_CLOSEDIR

AC_DEFUN([AFS_POSIX_CLOSEDIR], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_CLOSEDIR])
_AFS_CHECK_POSIX_SYMBOL([closedir],
    [#include <dirent.h>],
    [int closedir( DIR* dirp );],
    [
DIR* dir;
int result = closedir( dir );
    ])
]) # AFS_POSIX_CLOSEDIR


# POSIX opendir
# -------------
# Check whether the POSIX 'opendir' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_OPENDIR], [
_AFS_CHECK_POSIX_DECL([C], [opendir], [#include <dirent.h>])
]) # AFS_C_DECL_POSIX_OPENDIR

AC_DEFUN([AFS_CXX_DECL_POSIX_OPENDIR], [
_AFS_CHECK_POSIX_DECL([C++], [opendir], [#include <dirent.h>])
]) # AFS_CXX_DECL_POSIX_OPENDIR

AC_DEFUN([AFS_POSIX_OPENDIR], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_OPENDIR])
_AFS_CHECK_POSIX_SYMBOL([opendir],
    [#include <dirent.h>],
    [DIR* opendir( const char* dirname );],
    [
DIR* dir = opendir(".");
    ])
]) # AFS_POSIX_OPENDIR


# POSIX readdir
# -------------
# Check whether the POSIX 'readdir' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_READDIR], [
_AFS_CHECK_POSIX_DECL([C], [readdir], [#include <dirent.h>])
]) # AFS_C_DECL_POSIX_READDIR

AC_DEFUN([AFS_CXX_DECL_POSIX_READDIR], [
_AFS_CHECK_POSIX_DECL([C++], [readdir], [#include <dirent.h>])
]) # AFS_CXX_DECL_POSIX_READDIR

AC_DEFUN([AFS_POSIX_READDIR], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_READDIR])
_AFS_CHECK_POSIX_SYMBOL([readdir],
    [#include <dirent.h>],
    [struct dirent* readdir( DIR* dirp );],
    [
DIR* dir;
struct dirent* ent;
while ( ( ent = readdir( dir ) ) )
{
    const char* name = ent->d_name;
}
    ])
]) # AFS_POSIX_READDIR


# POSIX readlink
# --------------
# Check whether the POSIX 'readlink' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_READLINK], [
_AFS_CHECK_POSIX_DECL([C], [readlink], [#include <unistd.h>])
]) # AFS_C_DECL_POSIX_READLINK

AC_DEFUN([AFS_CXX_DECL_POSIX_READLINK], [
_AFS_CHECK_POSIX_DECL([C++], [readlink], [#include <unistd.h>])
]) # AFS_CXX_DECL_POSIX_READLINK

AC_DEFUN([AFS_POSIX_READLINK], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_READLINK])
_AFS_CHECK_POSIX_SYMBOL([readlink],
    [#include <unistd.h>],
    [ssize_t readlink( const char* pathname, char* buf, size_t bufsiz );],
    [
size_t bufsiz = 256;
char buf[bufsiz];
ssize_t result = readlink( "/proc/self/exe", buf, bufsiz );
    ])
]) # AFS_POSIX_READLINK


# _AFS_CHECK_POSIX_DECL(LANG, SYMBOL, INCLUDES)
# ---------------------------------------------
# Check whether SYMBOL (a function, variable or constant) is declared by
# INCLUDES in LANG.
#
# List of provided config header defines:
#   HAVE_<LANG>_DECL_POSIX_<SYMBOL>:: Set to '1' if SYMBOL is declared in LANG,
#                                     '0' otherwise
#
# List of (cached) configure variables set:
#   afs_cv_have_<lang>_decl_posix_<symbol>:: Set to 'yes' if SYMBOL is declared
#                                            in LANG, 'no' otherwise
#
m4_define([_AFS_CHECK_POSIX_DECL], [
AC_LANG_PUSH([$1])
AS_VAR_PUSHDEF([afs_CacheVar], [afs_cv_have_[]_AC_LANG_ABBREV[]_decl_posix_$2])
AC_CACHE_CHECK([whether $2 is declared ([]_AC_LANG[])],
    [afs_CacheVar],
    [AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[$3]], [[(void)$2;]])],
        [AS_VAR_SET([afs_CacheVar], [yes])],
        [AS_VAR_SET([afs_CacheVar], [no])])])
AS_VAR_IF([afs_CacheVar], [yes],
    [_afs_have_decl=1],
    [_afs_have_decl=0])
AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_[]_AC_LANG_PREFIX[]_DECL_POSIX_$2]),
    [$_afs_have_decl],
    [Define to 1 if you have the declaration of `$2' in ]_AC_LANG[,
     and to 0 if you don't.])
AS_VAR_POPDEF([afs_CacheVar])
AC_LANG_POP([$1])
]) # _AFS_CHECK_DECL


# _AFS_CHECK_POSIX_SYMBOL(SYMBOL, INCLUDES, PROTOTYPE, CONFTEST-BODY)
# -------------------------------------------------------------------
# Check whether SYMBOL can be linked, using a test program in the currently
# selected language (C/C++) consisting of INCLUDES, the function PROTOYPE (if
# needed, considering the result of a previous _AFS_CHECK_DECL check), and
# CONFTEST-BODY.
#
# List of provided config header defines:
#   HAVE_POSIX_<SYMBOL>:: Set to '1' if SYMBOL can be linked, unset otherwise
#
# List of (cached) configure variables set:
#   afs_cv_have_posix_<symbol>:: Set to 'yes' if SYMBOL can be linked, 'no'
#                                otherwise
#
m4_define([_AFS_CHECK_POSIX_SYMBOL], [
AS_VAR_PUSHDEF([afs_CacheVar], [afs_cv_have_posix_$1])
AC_CACHE_CHECK([whether $1 can be linked],
    [afs_CacheVar],
    [AC_LINK_IFELSE(
        [AC_LANG_PROGRAM([[
$2
#if !]]AS_TR_CPP([HAVE_[]_AC_LANG_PREFIX[]_DECL_POSIX_$1])[[
    #ifdef __cplusplus
    extern "C"
    #endif
    $3
#endif
            ]], [[
$4
            ]])],
        [AS_VAR_SET([afs_CacheVar], [yes])],
        [AS_VAR_SET([afs_CacheVar], [no])])])
AS_VAR_IF([afs_CacheVar], [yes],
    [AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_POSIX_$1]), 1,
        [Define to 1 if `$1' function can be linked])])
AS_VAR_POPDEF([afs_CacheVar])
]) # _AFS_CHECK_SYMBOL
