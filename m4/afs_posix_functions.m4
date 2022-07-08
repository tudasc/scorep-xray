## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2009-2013,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2013,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2014, 2017,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2013,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013, 2018, 2022,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2013,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Muenchen, Germany
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


# POSIX close
# -----------
# Check whether the POSIX 'close' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_CLOSE], [
_AFS_CHECK_POSIX_DECL([C], [close], [#include <unistd.h>])
]) # AFS_C_DECL_POSIX_CLOSE

AC_DEFUN([AFS_CXX_DECL_POSIX_CLOSE], [
_AFS_CHECK_POSIX_DECL([C++], [close], [#include <unistd.h>])
]) # AFS_CXX_DECL_POSIX_CLOSE

AC_DEFUN([AFS_POSIX_CLOSE], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_CLOSE])
_AFS_CHECK_POSIX_SYMBOL([close],
    [#include <unistd.h>],
    [int close( int fd );],
    [
int result = close( 0 );
    ])
]) # AFS_POSIX_CLOSE


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


# POSIX fileno
# ------------
# Check whether the POSIX 'fileno' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_FILENO], [
_AFS_CHECK_POSIX_DECL([C], [fileno], [#include <stdio.h>])
]) # AFS_C_DECL_POSIX_FILENO

AC_DEFUN([AFS_CXX_DECL_POSIX_FILENO], [
_AFS_CHECK_POSIX_DECL([C++], [fileno], [#include <stdio.h>])
]) # AFS_CXX_DECL_POSIX_FILENO

AC_DEFUN([AFS_POSIX_FILENO], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_FILENO])
_AFS_CHECK_POSIX_SYMBOL([fileno],
    [#include <stdio.h>],
    [int fileno( FILE* stream );],
    [
FILE* stream;
int fno = fileno( stream );
    ])
]) # AFS_POSIX_FILENO


# POSIX fseeko
# ------------
# Check whether the POSIX 'fseeko' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_FSEEKO], [
_AFS_CHECK_POSIX_DECL([C], [fseeko], [#include <stdio.h>])
]) # AFS_C_DECL_POSIX_FSEEKO

AC_DEFUN([AFS_CXX_DECL_POSIX_FSEEKO], [
_AFS_CHECK_POSIX_DECL([C++], [fseeko], [#include <stdio.h>])
]) # AFS_CXX_DECL_POSIX_FSEEKO

AC_DEFUN([AFS_POSIX_FSEEKO], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_FSEEKO])
_AFS_CHECK_POSIX_SYMBOL([fseeko],
    [#include <stdio.h>],
    [int fseeko( FILE* stream, off_t offset, int whence );],
    [
FILE* stream;
int result = fseeko( stream, 256, SEEK_SET );
    ])
]) # AFS_POSIX_FSEEKO


# LFS fseeko64
# ------------
# Check whether the Large File Support 'fseeko64' function is declared in
# C or C++, respectively, and whether it can be linked.  The link check
# uses the current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
# NOTE:
#   While not a POSIX function, we consider it as such for convenience.
#
AC_DEFUN([AFS_C_DECL_POSIX_FSEEKO64], [
_AFS_CHECK_POSIX_DECL([C], [fseeko64], [#include <stdio.h>])
]) # AFS_C_DECL_POSIX_FSEEKO64

AC_DEFUN([AFS_CXX_DECL_POSIX_FSEEKO64], [
_AFS_CHECK_POSIX_DECL([C++], [fseeko64], [#include <stdio.h>])
]) # AFS_CXX_DECL_POSIX_FSEEKO64

AC_DEFUN([AFS_POSIX_FSEEKO64], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_FSEEKO64])
_AFS_CHECK_POSIX_SYMBOL([fseeko64],
    [#include <stdio.h>],
    [int fseeko64( FILE* stream, off64_t offset, int whence );],
    [
FILE* stream;
int result = fseeko64( stream, 256UL, SEEK_SET );
    ])
]) # AFS_POSIX_FSEEKO64


# POSIX getcwd
# ------------
# Check whether the POSIX 'getcwd' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_GETCWD], [
_AFS_CHECK_POSIX_DECL([C], [getcwd], [#include <unistd.h>])
]) # AFS_C_DECL_POSIX_GETCWD

AC_DEFUN([AFS_CXX_DECL_POSIX_GETCWD], [
_AFS_CHECK_POSIX_DECL([C++], [getcwd], [#include <unistd.h>])
]) # AFS_CXX_DECL_POSIX_GETCWD

AC_DEFUN([AFS_POSIX_GETCWD], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_GETCWD])
_AFS_CHECK_POSIX_SYMBOL([getcwd],
    [#include <unistd.h>],
    [char* getcwd( char* buf, size_t size );],
    [
char* cwd = getcwd( NULL, 0 );
    ])
]) # AFS_POSIX_GETCWD


# POSIX gethostid
# ---------------
# Check whether the POSIX 'gethostid' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_GETHOSTID], [
_AFS_CHECK_POSIX_DECL([C], [gethostid], [#include <unistd.h>])
]) # AFS_C_DECL_POSIX_GETHOSTID

AC_DEFUN([AFS_CXX_DECL_POSIX_GETHOSTID], [
_AFS_CHECK_POSIX_DECL([C++], [gethostid], [#include <unistd.h>])
]) # AFS_CXX_DECL_POSIX_GETHOSTID

AC_DEFUN([AFS_POSIX_GETHOSTID], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_GETHOSTID])
_AFS_CHECK_POSIX_SYMBOL([gethostid],
    [#include <unistd.h>],
    [long gethostid( void );],
    [
long host_id = gethostid();
    ])
]) # AFS_POSIX_GETHOSTID


# POSIX gethostname
# -----------------
# Check whether the POSIX 'gethostname' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_GETHOSTNAME], [
_AFS_CHECK_POSIX_DECL([C], [gethostname], [#include <unistd.h>])
]) # AFS_C_DECL_POSIX_GETHOSTNAME

AC_DEFUN([AFS_CXX_DECL_POSIX_GETHOSTNAME], [
_AFS_CHECK_POSIX_DECL([C++], [gethostname], [#include <unistd.h>])
]) # AFS_CXX_DECL_POSIX_GETHOSTNAME

AC_DEFUN([AFS_POSIX_GETHOSTNAME], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_GETHOSTNAME])
_AFS_CHECK_POSIX_SYMBOL([gethostname],
    [#include <unistd.h>],
    [int gethostname( char* name, size_t len );],
    [
char* name;
int result = gethostname( name, 256 );
    ])
]) # AFS_POSIX_GETHOSTNAME


# POSIX getrlimit
# ---------------
# Check whether the POSIX 'getrlimit' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_GETRLIMIT], [
_AFS_CHECK_POSIX_DECL([C], [getrlimit], [#include <sys/resource.h>])
]) # AFS_C_DECL_POSIX_GETRLIMIT

AC_DEFUN([AFS_CXX_DECL_POSIX_GETRLIMIT], [
_AFS_CHECK_POSIX_DECL([C++], [getrlimit], [#include <sys/resource.h>])
]) # AFS_CXX_DECL_POSIX_GETRLIMIT

AC_DEFUN([AFS_POSIX_GETRLIMIT], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_GETRLIMIT])
_AFS_CHECK_POSIX_SYMBOL([getrlimit],
    [#include <sys/resource.h>],
    [int getrlimit( int resource, struct rlimit* rlp );],
    [
struct rlimit res_nofile;
int result = getrlimit( RLIMIT_NOFILE, &res_nofile );
    ])
]) # AFS_POSIX_GETRLIMIT


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


# POSIX pclose
# ------------
# Check whether the POSIX 'pclose' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_PCLOSE], [
_AFS_CHECK_POSIX_DECL([C], [pclose], [#include <stdio.h>])
]) # AFS_C_DECL_POSIX_PCLOSE

AC_DEFUN([AFS_CXX_DECL_POSIX_PCLOSE], [
_AFS_CHECK_POSIX_DECL([C++], [pclose], [#include <stdio.h>])
]) # AFS_CXX_DECL_POSIX_PCLOSE

AC_DEFUN([AFS_POSIX_PCLOSE], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_PCLOSE])
_AFS_CHECK_POSIX_SYMBOL([pclose],
    [#include <stdio.h>],
    [int pclose( FILE* stream );],
    [
FILE* pipe;
int result = pclose( pipe );
    ])
]) # AFS_POSIX_PCLOSE


# POSIX popen
# -----------
# Check whether the POSIX 'popen' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_POPEN], [
_AFS_CHECK_POSIX_DECL([C], [popen], [#include <stdio.h>])
]) # AFS_C_DECL_POSIX_POPEN

AC_DEFUN([AFS_CXX_DECL_POSIX_POPEN], [
_AFS_CHECK_POSIX_DECL([C++], [popen], [#include <stdio.h>])
]) # AFS_CXX_DECL_POSIX_POPEN

AC_DEFUN([AFS_POSIX_POPEN], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_POPEN])
_AFS_CHECK_POSIX_SYMBOL([popen],
    [#include <stdio.h>],
    [FILE* popen( const char* command, const char* type );],
    [
FILE* pipe = popen( "echo test", "r" );
    ])
]) # AFS_POSIX_POPEN


# POSIX read
# ----------
# Check whether the POSIX 'read' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_READ], [
_AFS_CHECK_POSIX_DECL([C], [read], [#include <unistd.h>])
]) # AFS_C_DECL_POSIX_READ

AC_DEFUN([AFS_CXX_DECL_POSIX_READ], [
_AFS_CHECK_POSIX_DECL([C++], [read], [#include <unistd.h>])
]) # AFS_CXX_DECL_POSIX_READ

AC_DEFUN([AFS_POSIX_READ], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_READ])
_AFS_CHECK_POSIX_SYMBOL([read],
    [#include <unistd.h>],
    [ssize_t read( int fd, void* buf, size_t count );],
    [
ssize_t result = read( 0, NULL, 0 );
    ])
]) # AFS_POSIX_READ


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


# POSIX realpath
# --------------
# Check whether the POSIX 'realpath' function is declared in C or C++,
# respectively, and whether it can be linked.  The link check uses the
# current language and implies a corresponding declaration check.
#
# See _AFS_CHECK_POSIX_DECL and _AFS_CHECK_POSIX_SYMBOL for details on
# config header defines and configure variables.
#
AC_DEFUN([AFS_C_DECL_POSIX_REALPATH], [
_AFS_CHECK_POSIX_DECL([C], [realpath], [#include <stdlib.h>])
]) # AFS_C_DECL_POSIX_REALPATH

AC_DEFUN([AFS_CXX_DECL_POSIX_REALPATH], [
_AFS_CHECK_POSIX_DECL([C++], [realpath], [#include <stdlib.h>])
]) # AFS_CXX_DECL_POSIX_REALPATH

AC_DEFUN([AFS_POSIX_REALPATH], [
AC_REQUIRE([AFS_]_AC_LANG_PREFIX[_DECL_POSIX_REALPATH])
_AFS_CHECK_POSIX_SYMBOL([realpath],
    [#include <stdlib.h>],
    [char* realpath( const char* file_name, char* resolved_name );],
    [
char* resolved_path = realpath( "/tmp" , NULL );
free( resolved_path );
    ])
]) # AFS_POSIX_REALPATH


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
    [AC_DEFINE(AS_TR_CPP([HAVE_POSIX_$1]), 1,
        [Define to 1 if `$1' function can be linked])])
AS_VAR_POPDEF([afs_CacheVar])
]) # _AFS_CHECK_SYMBOL
