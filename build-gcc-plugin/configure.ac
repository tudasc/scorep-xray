dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2012-2015, 2018-2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2015, 2018,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_PREREQ([2.69])
AC_INIT([Score-P], m4_esyscmd([../vendor/common/build-config/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

# ignore --(enable|disable)-(shared|static) flags on the command line, we
# can only build the GCC plug-in if we have support for shared libraries
enable_shared=yes
enable_static=no

AFS_PACKAGE_BUILD_INIT([GCC plug-in])
AFS_SUMMARY_INIT

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../vendor/common/build-config/generate-library-version.sh ../build-config/VERSION]))

AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../vendor/common/build-config/m4])

AC_CONFIG_HEADERS([../src/config-gcc-plugin.h])

AC_DEFUN([AC_SCOREP_FRONTEND_GCC_PLUGIN], [])

scorep_enable_gcc_plugin=detect
AC_ARG_ENABLE([gcc-plugin],
    [AS_HELP_STRING([--disable-gcc-plugin],
        [Disable support for the GCC plug-in instrumentation. Default is to determine support automatically. This disables it by request and fails if support cannot be satisfied but was requested.])],
    [scorep_enable_gcc_plugin=${enableval}
    AS_CASE([${scorep_enable_gcc_plugin}],
        [yes|no], [:],
        [AC_MSG_ERROR([Invalid argument for --disable-gcc-plugin: ${scorep_enable_gcc_plugin}])])])

## require $ac_scorep_cross_compiling to be set to 'yes' or 'no'
AS_IF([test "x${ac_scorep_cross_compiling}" != "xyes" && \
       test "x${ac_scorep_cross_compiling}" != "xno"],
    [AC_MSG_ERROR([invalid value '$ac_scorep_cross_compiling' for variable \$ac_scorep_cross_compiling.], [1])])
cross_compiling=${ac_scorep_cross_compiling}

AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"], [
    # suppress warnings about 'using cross tools not prefixed with host triplet'
    ac_tool_warned=yes])

# we always need the backend compiler
AC_SCOREP_CHECK_COMPILER_VAR_SET([CC])
AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX])

# this is the target gcc for which we want to build the plug-in.
# this is always the initial value of $CC and the corresponding flags variables
GCC_PLUGIN_TARGET_CC=$CC
GCC_PLUGIN_TARGET_CPPFLAGS=$CPPFLAGS
GCC_PLUGIN_TARGET_CFLAGS=$CFLAGS
GCC_PLUGIN_TARGET_LDFLAGS=$LDFLAGS
GCC_PLUGIN_TARGET_LIBS=$LIBS

AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"], [

    AC_ARG_VAR([CC_FOR_BUILD], [C compiler command for the plug-in build])
    AC_ARG_VAR([CXX_FOR_BUILD], [C++ compiler command for the plug-in build])
    AC_ARG_VAR([CPPFLAGS_FOR_BUILD], [(Objective) C/C++ preprocessor flags for the plug-in build, e.g., -I<include dir> if you have headers in a nonstandard directory <include dir>])
    AC_ARG_VAR([CFLAGS_FOR_BUILD], [C compiler flags for the plug-in build])
    AC_ARG_VAR([CXXFLAGS_FOR_BUILD], [C++ compiler flags for the plug-in build])
    AC_ARG_VAR([LDFLAGS_FOR_BUILD], [linker flags for the plug-in build, e.g., -L<lib dir> if you have libraries in a nonstandard directory <lib dir>])
    AC_ARG_VAR([LIBS_FOR_BUILD], [libraries to pass to the linker for the plug-in build, e.g., -l<library>])

    AC_SCOREP_CHECK_COMPILER_VAR_SET([CC_FOR_BUILD])
    AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX_FOR_BUILD])

    AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CPPFLAGS])
    AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CFLAGS])
    AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CXXFLAGS])
    AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([LDFLAGS])
    AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([LIBS])

    AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CC])
    AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CXX])
])

AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR
AFS_MAINTAINER_MODE

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_SCOREP_PLATFORM_SETTINGS

AC_LANG_PUSH([C])
# get vendor for the target CC first
save_CC=$CC
save_CPPFLAGS=$CPPFLAGS
save_CFLAGS=$CFLAGS
CC=$GCC_PLUGIN_TARGET_CC
CPPFLAGS=$GCC_PLUGIN_TARGET_CPPFLAGS
CFLAGS=$GCC_PLUGIN_TARGET_CFLAGS

AX_COMPILER_VENDOR
scorep_gcc_plugin_target_vendor=${ax_cv_c_compiler_vendor}
AS_UNSET([ax_cv_c_compiler_vendor])
CC=$save_CC
CPPFLAGS=$save_CPPFLAGS
CFLAGS=$save_CFLAGS

# get vendor for the plug-in builder
AX_COMPILER_VENDOR
AC_LANG_POP([C])

# Checks for programs.
AC_PROG_CC
AC_SCOREP_PROG_CC_C99
AC_PROG_CXX
AM_PROG_CC_C_O

LT_PREREQ([2.4.6])
LT_INIT([shared disable-static])
AC_SUBST([LIBTOOL_DEPS])

adl_RECURSIVE_EVAL([${datadir}/${PACKAGE}], [PKGDATADIR])
AC_SUBST([PKGDATADIR])

rm -f gcc_plugin_supported

# Only if the platform supports shared libraries, try to check for plug-in
# support in GCC
AS_CASE([${ac_scorep_platform},${scorep_gcc_plugin_target_vendor},${enable_shared},${scorep_enable_gcc_plugin}],
    [*,*,*,no], dnl early avoid checks if the user disabled detection
        [AFS_SUMMARY([GCC plug-in support], [no, explicitly disabled via --disable-gcc-plugin])],
    [bg*,gnu,*,detect],
        [AFS_SUMMARY([GCC plug-in support], [no, currently not available on Blue Gene platforms])],
    [*,gnu,yes,*],
        [SCOREP_GCC_PLUGIN],
    [*,*,no,*],
        [AFS_SUMMARY([GCC plug-in support], [no, need support for shared libraries])],
    [*,*,*,*],
        [AFS_SUMMARY([GCC plug-in support], [no, only GNU compilers supported])])

AM_COND_IF([HAVE_GCC_PLUGIN_SUPPORT],
    [touch gcc_plugin_supported],
    [AS_IF([test "x${scorep_enable_gcc_plugin}" = "xyes"],
        [AC_MSG_ERROR([Request to enable the GCC plug-in could not be fulfilled. Run configure with --disable-gcc-plugin or run without --enable-gcc-plugin to see the reason.])])])

AC_SCOREP_ERROR_CODES

AFS_SET_SILENT_RULE_PREFIX([GCC PLUGIN ])
AFS_CONFIG_MAKEFILE

AC_OUTPUT
