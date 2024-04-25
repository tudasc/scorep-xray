dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2021-2022,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2022,
dnl Deutsches Zentrum fuer Luft- und Raumfahrt, Germany
dnl
dnl Copyright (c) 2024,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file scorep_libbfd.m4


# _SCOREP_LIBBFD()
# --------------
# Provide configure options to use or download a libbfd installation
# or package. Check an existing installation whether it is usable, it
# needs to be either shared or PIC.  As libbfd is a hard requirement,
# abort if this check fails. The download option generates a Makefile
# that builds and install libbfd at make time.
#
AC_DEFUN([_SCOREP_LIBBFD], [
AC_LANG_PUSH([C])
AFS_EXTERNAL_LIB([bfd], [_LIBBFD_CHECK], [bfd.h], [_LIBBFD_DOWNLOAD], [dnl
AS_HELP_STRING([--with-]_afs_lib_name[@<:@=yes|download|<path to ]_afs_lib_name[ installation>@:>@],
    [A shared or PIC ]_afs_lib_name[ is  required. Option defaults to [yes]
     on non-cross-compile systems and expects library and headers
     to be found in system locations. Provide ]_afs_lib_name['s
     installation prefix [path] to override this default.
     --with-]_afs_lib_name[=<path> is a shorthand for
     --with-]_afs_lib_name[-include=<path/include> and
     --with-]_afs_lib_name[-lib=<path/(lib64|lib)>. If this
     not the case, use the explicit
     options directly or provide paths via ]_afs_lib_NAME[_LIB
     and ]_afs_lib_NAME[_INCLUDE. Use [download] to automatically
     obtain and use ]_afs_lib_name[ via external tarball. See
     --with-package-cache=<path> how to provide the tarball
     for an offline installation.])])dnl
AC_LANG_POP([C])
dnl
AS_IF([test "${libbfd_summary:+set}" != set],
    [AC_MSG_ERROR([internal: libbfd_summary not set])])
dnl
AS_IF([test ${have_cplus_demangle+set} != set],
    [AC_MSG_ERROR([internal: have_cplus_demangle not set])],
    [AS_IF([test "x${have_cplus_demangle}" = xyes],
         [AC_DEFINE([HAVE_SCOREP_DEMANGLE], [1], [Define to 1 if cplus_demangle is available via ]_afs_lib_NAME[_LDFLAGS, ]_afs_lib_NAME[_LIBS, and AM_CONDITIONAL HAVE_SCOREP_DEMANGLE.])
          AFS_AM_CONDITIONAL(HAVE_SCOREP_DEMANGLE, [test 0 -eq 0], [false])])])
])# _SCOREP_LIBBFD


# _LIBBFD_DOWNLOAD()
# ------------------
# Generate a Makefile.libbfd to download binutils and install libbfd
# at make time. In addition, set automake conditional
# HAVE_SCOREP_LIBBFD_MAKEFILE to trigger this process from
# build-backend/Makefile.
# Remove the installed libbfd.la as it cannot be relied upon.
# dependency_libs reference the build directory (which is OK for a
# static libbfd). On Fugaku, trying to link a shared libbfd failed as
# these directories were erroneously used. Other systems were OK
# though.
#
m4_define([_LIBBFD_DOWNLOAD], [
_afs_lib_PREFIX="${libdir}${backend_suffix}/${PACKAGE}/[]_afs_lib_name"
_afs_lib_MAKEFILE="Makefile.[]_afs_lib_name"
_afs_lib_LDFLAGS="-L$[]_afs_lib_PREFIX[]/lib -R$[]_afs_lib_PREFIX[]/lib"
_afs_lib_CPPFLAGS="-I$[]_afs_lib_PREFIX/include"
dnl
AFS_AM_CONDITIONAL(HAVE_[]_afs_lib_MAKEFILE, [test 0 -eq 0], [false])dnl
dnl binutils_* are sourced from build-config/downloads
libbfd_summary="yes, from downloaded $binutils_url"
have_cplus_demangle=yes
dnl
AC_SUBST([binutils_url])
AC_SUBST([binutils_package])
AC_CONFIG_FILES([Makefile.libbfd:../build-backend/Makefile.libbfd.in])
])# _LIBBFD_DOWNLOAD


# _LIBBFD_CHECK()
# ---------------
# Check for bfd.h and try to libtool-link a libbfd given via
# _afs_lib_CPPFLAGS and _afs_lib_LDFLAGS (might be unset) into a
# libtool archive and then into a binary.
#
m4_define([_LIBBFD_CHECK], [
AS_IF([test "x${_afs_lib_prevent_check}" = xyes], [
    AS_IF([test "x${_afs_lib_prevent_check_reason}" = xdisabled],
        [AC_MSG_ERROR([A working _afs_lib_name installation is required, --without-_afs_lib_name is not a valid option. See --with-_afs_lib_name in INSTALL.])],
	[test "x${_afs_lib_prevent_check_reason}" = xcrosscompile],
	[AC_MSG_ERROR([A working _afs_lib_name installation is required. Either provide a path or use the download option, see --with-_afs_lib_name in INSTALL.])],
	[AC_MSG_ERROR([Unknown _afs_lib_prevent_check_reason "${_afs_lib_prevent_check_reason}".])])])
dnl
CPPFLAGS=$_afs_lib_CPPFLAGS
AC_CHECK_HEADER([bfd.h],
    [LTLDFLAGS=$_afs_lib_LDFLAGS
     LTLIBS=$_afs_lib_LIBS
     AFS_LTLINK_LA_IFELSE([_LIBBFD_MAIN], [_LIBBFD_LA],
         [libbfd_summary="yes${_afs_lib_LDFLAGS:+, using $_afs_lib_LDFLAGS}${_afs_lib_CPPFLAGS:+ and $_afs_lib_CPPFLAGS}"
          CPPFLAGS="$CPPFLAGS -DCHECK_DEMANGLE"
          AFS_LTLINK_LA_IFELSE([_LIBBFD_MAIN], [_LIBBFD_LA],
              [libbfd_summary="${libbfd_summary}, has cplus_demangle"
               have_cplus_demangle=yes],
              [libbfd_summary="${libbfd_summary}, cplus_demangle not available"
               have_cplus_demangle=no])],
         [AC_MSG_FAILURE([Cannot link _afs_lib_name (and dependencies).])])],
    [AC_MSG_FAILURE([bfd.h required])])
])# _LIBBFD_CHECK


# _LIBBFD_LA()
# ------------
# The source code for the libtool archive.
#
m4_define([_LIBBFD_LA], [
AC_LANG_SOURCE([[
char bfd_check_format ();
char bfd_close ();
char bfd_init ();
char bfd_map_over_sections ();
char bfd_openr ();
char bfd_openr ();
#ifdef CHECK_DEMANGLE
char* cplus_demangle( const char* mangled, int options );
#endif
void check_bfd() {
    bfd_check_format ();
    bfd_close ();
    bfd_init ();
    bfd_map_over_sections ();
    bfd_openr ();
    bfd_openr ();
    #ifdef CHECK_DEMANGLE
    cplus_demangle("test", 27);
    #endif
}
]])])#_LIBBFD_LA


# _LIBBFD_MAIN()
# --------------
# The source code using the libtool archive.
#
m4_define([_LIBBFD_MAIN], [
AC_LANG_PROGRAM(dnl
[[char check_bfd ();]],
[[check_bfd ();]]
)])#_LIBBFD_MAIN
