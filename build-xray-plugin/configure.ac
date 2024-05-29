# copied from llvm-plugin/configure.ac

AC_PREREQ([2.69])
AC_INIT([Score-P], m4_esyscmd([../build-config/common/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

#TODO: Check whether this is also true for xray plugin, depends on whether it will be a llvm plugin architecture
# ignore --(enable|disable)-(shared|static) flags on the command line, we
# can only build the XRAY plug-in if we have support for shared libraries
enable_shared=yes
enable_static=no

AFS_PACKAGE_BUILD_INIT([XRAY plug-in])
AFS_SUMMARY_INIT

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../build-config/common/generate-library-version.sh ../build-config/VERSION]))

AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../build-config/common/m4])

AC_CONFIG_HEADERS([../src/config-xray-plugin.h])

AC_DEFUN([AC_SCOREP_FRONTEND_XRAY_PLUGIN], [])

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

# Provide CXXFLAGS, CPPFLAGS, LDFLAGS and LIBS for build-xray-plugin via
# *FLAGS_FOR_BUILD_XRAY_PLUGIN environment variables. If not set,
# use autoconf's defaults.
# TODO: Check if passing flags is correct depending on architecture
AC_ARG_VAR([CXXFLAGS_FOR_BUILD_XRAY_PLUGIN],
    [C++ compiler flags for building XRAY plugin. Will be amended with 'llvm-config --cxxflags', if 'llvm-config' is found.])
AS_IF([test "x${ac_cv_env_CXXFLAGS_FOR_BUILD_XRAY_PLUGIN_set}" != xset],
    [AS_UNSET([CXXFLAGS])],
    [CXXFLAGS="${ac_cv_env_CXXFLAGS_FOR_BUILD_XRAY_PLUGIN_value}"])

AC_ARG_VAR([CPPFLAGS_FOR_BUILD_XRAY_PLUGIN],
    [C preprocesser flags for building XRAY plugin. Will be amended with 'llvm-config --cppflags', if 'llvm-config' is found.])
AS_IF([test "x${ac_cv_env_CPPFLAGS_FOR_BUILD_XRAY_PLUGIN_set}" != xset],
    [AS_UNSET([CPPFLAGS])],
    [CXXFLAGS="${ac_cv_env_CPPFLAGS_FOR_BUILD_XRAY_PLUGIN_value}"])

AC_ARG_VAR([LDFLAGS_FOR_BUILD_XRAY_PLUGIN],
    [Linker flags for building XRAY plugin. Will be amended with 'llvm-config --ldflags', if 'llvm-config' is found.])
AS_IF([test "x${ac_cv_env_LDFLAGS_FOR_BUILD_XRAY_PLUGIN_set}" != xset],
    [AS_UNSET([LDFLAGS])],
    [LDFLAGS="${ac_cv_env_LDFLAGS_FOR_BUILD_XRAY_PLUGIN_value}"])

AC_ARG_VAR([LIBS_FOR_BUILD_XRAY_PLUGIN],
    [Libraries for building XRAY plugin. Will be amended with 'llvm-config --libs demangle support', if 'llvm-config' is found.])
AS_IF([test "x${ac_cv_env_LIBS_FOR_BUILD_XRAY_PLUGIN_set}" != xset],
    [AS_UNSET([LIBS])],
    [LIBS="${ac_cv_env_LIBS_FOR_BUILD_XRAY_PLUGIN_value}"])

# this is the target clang, clang++ for which we want to build the plug-in.
# this is always the initial value of $CC etc. and the corresponding flags variables
XRAY_PLUGIN_TARGET_CC=$CC
XRAY_PLUGIN_TARGET_CXX=$CXX
XRAY_PLUGIN_TARGET_FC=$FC
XRAY_PLUGIN_TARGET_CXXFLAGS=$CXXFLAGS
XRAY_PLUGIN_TARGET_CPPFLAGS=$CPPFLAGS
XRAY_PLUGIN_TARGET_LDFLAGS=$LDFLAGS
XRAY_PLUGIN_TARGET_LIBS=$LIBS

AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR
AFS_MAINTAINER_MODE

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_SCOREP_PLATFORM_SETTINGS

# Checks for programs.
AFS_PROG_CC([99])
# Since LLVM 16.0, only C++17 or newer is supported inside the code base
# This yields issues in the LLVM IR headers when compiling with C++11
# Therefore, try to use C++17 instead. Set C++ compiler as optional
# to allow configure to run through when a compiler doesn't support
# C++17.
AFS_PROG_CXX([17], [noext], [optional])
AFS_PROG_FC([optional])

LT_PREREQ([2.4.6])
LT_INIT([shared disable-static])
AC_SUBST([LIBTOOL_DEPS])

adl_RECURSIVE_EVAL([${datadir}/${PACKAGE}], [PKGDATADIR])
AC_SUBST([PKGDATADIR])

# TODO: Check where these directories come from and remove them accordingly
rm -f xray_plugin_supported_c
rm -f xray_plugin_supported_cxx
rm -f xray_plugin_supported_fc

SCOREP_XRAY_PLUGIN_SUMMARY
AC_CONFIG_FILES([../src/scorep_config_tool_xray_plugin.h:../src/tools/config/scorep_config_tool_xray_plugin.h.in])

AFS_SET_SILENT_RULE_PREFIX([XRAY PLUGIN ])
AFS_CONFIG_MAKEFILE

AC_OUTPUT
