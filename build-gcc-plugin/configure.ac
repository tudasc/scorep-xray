dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2012-2014,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_PREREQ([2.68])
AC_INIT([Score-P], m4_esyscmd([../vendor/common/build-config/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

# ignore --(enable|disable)-(shared|static) flags on the command line, we
# can only build the GCC plug-in if we have support for shared libraries
enable_shared=yes
enable_static=no

AFS_PACKAGE_BUILD_INIT([GCC plug-in])
AFS_SUMMARY_INIT

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../vendor/common/build-config/generate-library-version.sh ../build-config/VERSION]))

AC_CONFIG_AUX_DIR([../build-config])
AC_CONFIG_MACRO_DIR([../vendor/common/build-config/m4
                     ../build-config/m4])

AC_CONFIG_HEADERS([../src/config-gcc-plugin.h])

AC_DEFUN([AC_SCOREP_FRONTEND_GCC_PLUGIN], [])

## require $ac_scorep_cross_compiling to be set to 'yes' or 'no'
AS_IF([test "x${ac_scorep_cross_compiling}" != "xyes" && \
       test "x${ac_scorep_cross_compiling}" != "xno"],
    [AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])])
cross_compiling=${ac_scorep_cross_compiling}

AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"], [
    # suppress warnings about 'using cross tools not prefixed with host triplet'
    ac_tool_warned=yes])

AC_SCOREP_CHECK_COMPILER_VAR_SET([CC])
AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX])

AM_INIT_AUTOMAKE([foreign color-tests 1.11.1 -Wall tar-pax])
AM_SILENT_RULES([yes])

LT_PREREQ([2.4])
LT_INIT([shared disable-static])
AC_SUBST([LIBTOOL_DEPS])

AC_SCOREP_PLATFORM_SETTINGS

AC_LANG_PUSH([C])
AX_COMPILER_VENDOR
AC_LANG_POP([C])

# Checks for programs.
AC_PROG_CC
AC_SCOREP_PROG_CC_C99
AC_PROG_CXX
AM_PROG_CC_C_O

adl_RECURSIVE_EVAL([${datadir}/${PACKAGE}], [PKGDATADIR])
AC_SUBST([PKGDATADIR])

# Only if the platform supports shared libraries, try to check for plug-in
# support in GCC
AS_CASE([${cross_compiling},${ax_cv_c_compiler_vendor},${enable_shared}],
    [no,gnu,yes],
        [SCOREP_GCC_PLUGIN],
    [yes,*,*],
        [AFS_SUMMARY([GCC plug-in support], [no, disabled on cross build machines])],
    [*,*,no],
        [AFS_SUMMARY([GCC plug-in support], [no, need support for shared libraries])],
    [*,*,*],
        [AFS_SUMMARY([GCC plug-in support], [no, only GNU compilers supported])])


AC_SCOREP_ERROR_CODES

AC_CONFIG_FILES([Makefile], [[
sed -e 's/_0 = @echo "  \(YACC\|GEN\|LEX\)/&           /; t; s/_0 = @echo "  /&GCC PLUGIN /' \
Makefile >Makefile.gcc-plugin && mv -f Makefile.gcc-plugin Makefile || rm -f Makefile.gcc-plugin
]])

AC_OUTPUT
