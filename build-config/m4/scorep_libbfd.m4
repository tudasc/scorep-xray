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
     obtain and use ]_afs_lib_name[ via external tarball.])])dnl
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
_afs_lib_PREFIX="$prefix/vendor/[]_afs_lib_name"
_afs_lib_MAKEFILE="Makefile.[]_afs_lib_name"
_afs_lib_LDFLAGS="-L$[]_afs_lib_PREFIX[]/lib -R$[]_afs_lib_PREFIX[]/lib"
_afs_lib_CPPFLAGS="-I$[]_afs_lib_PREFIX/include"
dnl
AFS_AM_CONDITIONAL(HAVE_[]_afs_lib_MAKEFILE, [test 0 -eq 0], [false])dnl
dnl binutils_package and binutils_base_url are sourced from build-config/downloads
libbfd_summary="yes, from downloaded $binutils_base_url/$binutils_package.tar.gz"
have_cplus_demangle=yes
dnl
m4_changecom([])
cat <<_SCOREPEOF > $[]_afs_lib_MAKEFILE
#
# $(pwd)/$_afs_lib_MAKEFILE
#
# Executing 'make -f $_afs_lib_MAKEFILE' downloads a binutils
# package and installs a shared _afs_lib_name into
# $prefix/vendor/[]_afs_lib_name
# using CC=gcc that was found in PATH.
#
# Usually, this process is triggered during Score-P's build-backend
# make. If _afs_lib_name's configure or make fail, or if there are
# failures in the subsequent build process of Score-P, consider
# modifying CC above to point to a compiler (gcc recommended) that is
# compatible with the compute node compiler that you are using for
# Score-P (i.e., $CC) and try (manually) again. Note that PGI/NVIDIA
# and well as non-clang-based Cray compilers fail to build
# _afs_lib_name.
#
# You can also modify the installation prefix if, e.g., you want to
# share the _afs_lib_name installation between several Score-P
# installations (which then need to be configured using
# --with-[]_afs_lib_name=<prefix>).
#
# Please report bugs to <support@score-p.org>.
#
THIS_FILE = $(pwd)/$_afs_lib_MAKEFILE
URL = $binutils_base_url
PACKAGE = $binutils_package
PREFIX = $[]_afs_lib_PREFIX
CC = gcc
all:
	@$AFS_LIB_DOWNLOAD_CMD \$(URL)/\$(PACKAGE).tar.gz \\
	&& tar xf \$(PACKAGE).tar.gz \\
	&& mkdir \$(PACKAGE)/_build \\
	&& cd \$(PACKAGE)/_build \\
	&& ../configure \\
	    --prefix=\$(PREFIX) \\
	    --libdir=\$(PREFIX)/lib \\
	    CC=\$(CC) \\
	    --enable-shared \\
	    --disable-static \\
	    --enable-silent-rules \\
	    --silent \\
	&& make -s all-bfd \\
	&& make -s install-bfd \\
	&& rm -f \$(PREFIX)/lib/libbfd.la
clean:
	@rm -rf \$(PACKAGE).tar.gz \$(PACKAGE)
uninstall:
	@rm -rf \$(PREFIX)
_SCOREPEOF
m4_changecom([#])
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
