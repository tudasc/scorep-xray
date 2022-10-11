## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2020, 2022,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##
##

AC_DEFUN([SCOREP_LUSTREAPI_CHECK], [

AFS_SUMMARY_PUSH

AC_LANG_PUSH([C])
AFS_EXTERNAL_LIB([lustreapi], [_LIBLUSTREAPI_CHECK], [lustre/lustreapi.h])dnl
AC_LANG_POP([C])

scorep_lustreapi_support=${have_liblustreapi}
AS_IF([test x"${scorep_lustreapi_support}" = x"yes"],
      [save_CPPFLAGS=$CPPFLAGS
       CPPFLAGS="$CPPFLAGS ${with_liblustreapi_cppflags}"
       AC_CHECK_DECLS([llapi_layout_get_by_path],
                      [], [], [[#include <lustre/lustreapi.h>]])
       CPPFLAGS=$save_CPPFLAGS],
      [scorep_lustreapi_support="no"])

AC_SCOREP_COND_HAVE([LUSTRE_API_SUPPORT],
                    [test x"${scorep_lustreapi_support}" = x"yes"],
                    [Defined if recording calls to Lustre-API is possible.])

AFS_SUMMARY_POP([Lustre stripe info support], [${scorep_lustreapi_support}${scorep_liblustreapi_summary_reason}])

])

AC_DEFUN([_LIBLUSTREAPI_CHECK], [
have_liblustreapi="no"
AS_IF([test "x${_afs_lib_prevent_check}" = xyes],
    [AS_IF([test "x${_afs_lib_prevent_check_reason}" = xdisabled],
        [scorep_liblustreapi_summary_reason=", explicitly disabled"],
        [test "x${_afs_lib_prevent_check_reason}" = xcrosscompile],
        [scorep_liblustreapi_summary_reason=", --with-_afs_lib_name needs path in cross-compile mode"],
        [AC_MSG_ERROR([Unknown _afs_lib_prevent_check_reason "${_afs_lib_prevent_check_reason}".])])],
    [CPPFLAGS=$_afs_lib_CPPFLAGS
     AC_CHECK_HEADER([lustre/lustreapi.h],
         [LTLDFLAGS=$_afs_lib_LDFLAGS
          LTLIBS=$_afs_lib_LIBS
          AFS_LTLINK_LA_IFELSE([_LIBLUSTREAPI_MAIN], [_LIBLUSTREAPI_LA],
              [have_liblustreapi="yes"
               scorep_liblustreapi_summary_reason="${_afs_lib_LDFLAGS:+, using $_afs_lib_LDFLAGS}${_afs_lib_CPPFLAGS:+ and $_afs_lib_CPPFLAGS}"],
              [scorep_liblustreapi_summary_reason=", cannot link against $_afs_lib_LIBS"])
         ],
         [scorep_liblustreapi_summary_reason=", missing lustre/lustreapi.h header"])])
])# _LIBLUSTREAPI_CHECK

# _LIBLUSTREAPI_LA()
# ---------------
# The source code for the libtool archive.
#
m4_define([_LIBLUSTREAPI_LA], [
AC_LANG_SOURCE([[
#include <lustre/lustreapi.h>
char test_lustre ()
{
    struct lov_user_md* lum_file = 0;
    return llapi_file_get_stripe( "file path", lum_file );
}
]])])#_LIBLUSTREAPI_LA


# _LIBLUSTREAPI_MAIN()
# -----------------
# The source code using the libtool archive.
#
m4_define([_LIBLUSTREAPI_MAIN], [
AC_LANG_PROGRAM(dnl
[[char test_lustre ();]],
[[test_lustre ();]]
)])#_LIBLUSTREAPI_MAIN
