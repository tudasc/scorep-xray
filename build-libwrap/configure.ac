dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2017, 2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2018,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_PREREQ([2.69])
AC_INIT([Score-P], m4_esyscmd([../vendor/common/build-config/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

AFS_PACKAGE_BUILD_INIT([libwrap])
AFS_SUMMARY_INIT

AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../vendor/common/build-config/m4])

AC_CONFIG_HEADERS([../src/config-libwrap.h])

AC_DEFUN([SCOREP_LIBWRAP], [])

## require $afs_cross_compiling to be set to 'yes' or 'no'
AS_IF([test "x${afs_cross_compiling}" != "xyes" && \
       test "x${afs_cross_compiling}" != "xno"],
    [AC_MSG_ERROR([invalid value '$afs_cross_compiling' for variable \$afs_cross_compiling.], [1])])

AC_SCOREP_PRECIOUS_VARS_FOR_BUILD

AC_SCOREP_CHECK_COMPILER_VAR_SET([CC])
AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX])

# Compilers etc. are set according to LLVM installation (see below),
# so ignore environment variables set in the environment.
AS_UNSET([CFLAGS])
AS_UNSET([LDFLAGS])
AS_UNSET([LIBS])
AS_UNSET([CPPFLAGS])
AS_UNSET([LT_SYS_LIBRARY_PATH])
AS_UNSET([CPP])
AS_UNSET([CXXFLAGS])
AS_UNSET([CXXCPP])

AS_IF([test "x${afs_cross_compiling}" = "xyes"], [

    AC_SCOREP_CHECK_COMPILER_VAR_SET([CC_FOR_BUILD])
    AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX_FOR_BUILD])

    AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CC])
    AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CXX])
])

AC_ARG_VAR([LIBCLANG_CPPFLAGS], [Additional C preprocessor flags when compiling against libclang])
AC_ARG_VAR([LIBCLANG_CXXFLAGS], [Additional C++ compile flags when compiling against libclang])
AC_ARG_VAR([LIBCLANG_LDFLAGS],  [Additional linker flags when linking against libclang])
AC_ARG_VAR([LIBCLANG_LIBS],     [Additional libs when linking against libclang])

AC_ARG_WITH([llvm],
            [AS_HELP_STRING([--with-llvm[[=<llvm-bindir>]]],
                            [Use an already installed LLVM, including libclang as we need libclang for library-wrapping. Provide path to llvm-config. Auto-detected if already in $PATH.])],
            [with_llvm="${with_llvm%/}"], dnl yes, no, or <path>
            [with_llvm="not_given"])

rm -f library_wrapping_supported
reason=""

# AC_CHECK_PROGS does has no effect, if the $1 argument is already set
AS_UNSET([scorep_have_llvm_config])
AS_IF([test "x${with_llvm}" != "xno"],
      [AS_IF([test "x${with_llvm}" = "xyes" || test "x${with_llvm}" = "xnot_given"],
             [AC_CHECK_PROGS([scorep_have_llvm_config], [llvm-config llvm-config-4.0 llvm-config-3.9 llvm-config-3.8 llvm-config-3.7 llvm-config-3.6], ["no"])
              AS_IF([test "x${scorep_have_llvm_config}" != "xno"],
                    [scorep_llvm_config_bin="`which ${scorep_have_llvm_config}`"])],
             [# --with-llvm=<path>
              AC_CHECK_PROGS([scorep_have_llvm_config], [llvm-config llvm-config-4.0 llvm-config-3.9 llvm-config-3.8 llvm-config-3.7 llvm-config-3.6], [no], [${with_llvm}])
              AS_IF([test "x${scorep_have_llvm_config}" != "xno"],
                    [scorep_llvm_config_bin="${with_llvm}/${scorep_have_llvm_config}"],
                    [AS_UNSET([ac_cv_prog_scorep_have_llvm_config])
                     AS_UNSET([scorep_have_llvm_config])
                     AC_CHECK_PROGS([scorep_have_llvm_config], [llvm-config llvm-config-4.0 llvm-config-3.9 llvm-config-3.8 llvm-config-3.7 llvm-config-3.6], [no], ["${with_llvm}/bin"])
                     AS_IF([test "x${scorep_have_llvm_config}" != "xno"],
                           [scorep_llvm_config_bin="${with_llvm}/bin/${scorep_have_llvm_config}"])])
             ])],
      [scorep_have_llvm_config="no"
       reason="explicitly disabled via --without-llvm"])

AS_IF([test "x${scorep_have_llvm_config}" != "xno"],
      [# try executing llvm-config to rule out that it doesn't work due to not finding linked libs e.g.
       llvm_config_version='${scorep_llvm_config_bin} --version >&AS_MESSAGE_LOG_FD'
       AS_IF([_AC_DO_VAR([llvm_config_version])],
             [llvm_bindir=`${scorep_llvm_config_bin} --bindir`
              AS_IF([AS_EXECUTABLE_P([${llvm_bindir}/clang]) && AS_EXECUTABLE_P([${llvm_bindir}/clang++])],
                    [CC=${llvm_bindir}/clang
                     CXX=${llvm_bindir}/clang++],
                    [test "x${afs_cross_compiling}" = "xyes" && AS_EXECUTABLE_P([/usr/bin/gcc]) && AS_EXECUTABLE_P([/usr/bin/g++])],
                    [CC=/usr/bin/gcc
                     CXX=/usr/bin/g++],
                    [AS_EXECUTABLE_P([`which gcc`]) && AS_EXECUTABLE_P([`which g++`])],
                    [CC=`which gcc`
                     CXX=`which g++`],
                    [scorep_have_llvm_config=no
                     reason="no suitable compiler found, only Clang and GCC compiler supported"])],
             [scorep_have_llvm_config=no
              reason="${scorep_llvm_config_bin} crashes when executed"])],
      [AS_IF([test "x${with_llvm}" != "xno"],
             [reason="llvm-config not found"])])

AS_IF([test "x${scorep_have_llvm_config}" = "xno"],
      [# reset compilers to their original value, this way we avoid failures for the non-llvm related configure checks
       AS_IF([test "x${afs_cross_compiling}" = "xyes"],
             [AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CC])
              AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CXX])],
             [CC=${ac_cv_env_CC_value}
              CXX=${ac_cv_env_CXX_value}])])

AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR
AFS_MAINTAINER_MODE

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_PROG_CC
AC_SCOREP_PROG_CC_C99

AC_PROG_CXX
AC_LANG_PUSH([C++])
AX_COMPILER_VENDOR
AC_LANG_POP([C++])

AC_CXX_NAMESPACES
AC_CXX_HAVE_SSTREAM
AC_CXX_HAVE_STRSTREAM

LT_PREREQ([2.4.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

AFS_DEBUG_OPTION
AC_SCOREP_POSIX_FUNCTIONS

AS_IF([test "x${scorep_have_llvm_config}" != "xno"],
      [llvm_cppflags=$(${scorep_llvm_config_bin} --cppflags)
       llvm_ldflags=$(${scorep_llvm_config_bin} --ldflags)
       llvm_libdir=$(${scorep_llvm_config_bin} --libdir)

       libwrap_cppflags="${llvm_cppflags} ${LIBCLANG_CPPFLAGS}"
       # the CXXFLAGS from llvm-config may only be understood by clang++, but not by g++, thus ignore
       libwrap_cxxflags="${LIBCLANG_CXXFLAGS}"
       libwrap_ldflags="${llvm_ldflags} ${LIBCLANG_LDFLAGS}"
       libwrap_libs="-lclang ${LIBCLANG_LIBS}"

       AC_LANG_PUSH([C++])

       save_CPPFLAGS=$CPPFLAGS
       save_CXXFLAGS=$CXXFLAGS
       save_LDFLAGS=$LDFLAGS
       save_LIBS=$LIBS
       CPPFLAGS="$CPPFLAGS $libwrap_cppflags"
       CXXFLAGS="$CXXFLAGS $libwrap_cxxflags"
       LDFLAGS="$LDFLAGS $libwrap_ldflags"
       LIBS="$LIBS $libwrap_libs"

       # Check whether the header exists, this is a hint, that the dev packages
       # are not installed
       # than we can compile and link a simple program, and finally
       # whether the required functions are available.
       # These functions are provided since clang v3.6
       AC_CHECK_HEADER([clang-c/Index.h],
           [AC_MSG_CHECKING([linking against libclang])
            AC_LINK_IFELSE(
              [AC_LANG_PROGRAM([[#include <clang-c/Index.h>]], [[
    CXCursor cursor;
    clang_Cursor_isNull( cursor );]])],
              [AC_MSG_RESULT([yes])
               AC_MSG_CHECKING([for sufficient new libclang])
               AC_LINK_IFELSE(
                     [AC_LANG_PROGRAM([[#include <clang-c/Index.h>]], [[
    CXCursor cursor;
    clang_getCursorKind( cursor );
    clang_getCXXAccessSpecifier( cursor );
    clang_isCursorDefinition( cursor );
    clang_getCursorSemanticParent( cursor );
    clang_getCursorDisplayName( cursor );
    CXType type = clang_getCursorType( cursor );
    clang_getTypeSpelling( type );
    clang_isFunctionTypeVariadic( type );
    type = clang_getCursorResultType( cursor );
    clang_Cursor_getNumArguments( cursor );
    CXCursor arg = clang_Cursor_getArgument( cursor, 0 );
    clang_getCursorSpelling( arg );
    clang_Cursor_getMangling( cursor );]])],
                     [AC_MSG_RESULT([yes])
                      touch library_wrapping_supported
                      reason="using ${scorep_llvm_config_bin}"],
                     [AC_MSG_RESULT([no])
                      reason="libclang version not sufficient (minimum v3.6)"])],
              [AC_MSG_RESULT([no])
               reason="could not compile and link minimal program"])],
           [reason="missing <clang-c/Index.h>, please install developer packages for libclang"])

       CPPFLAGS=$save_CPPFLAGS
       CXXFLAGS=$save_CXXFLAGS
       LDFLAGS=$save_LDFLAGS
       LIBS=$save_LIBS
       AS_UNSET([save_CPPFLAGS])
       AS_UNSET([save_CXXFLAGS])
       AS_UNSET([save_LDFLAGS])
       AS_UNSET([save_LIBS])

       AC_LANG_POP([C++])])


AFS_AM_CONDITIONAL([HAVE_LIBRARY_WRAPPING_GENERATOR],
                   [test -e library_wrapping_supported],
                   [false])

AM_COND_IF([HAVE_LIBRARY_WRAPPING_GENERATOR],
           [scorep_have_libwrap_generator=yes
            AC_SUBST([SCOREP_LIBRARY_WRAPPING_GENERATOR_CPPFLAGS], ["${libwrap_cppflags}"])
            AC_SUBST([SCOREP_LIBRARY_WRAPPING_GENERATOR_CXXFLAGS], ["${libwrap_cxxflags}"])
            AC_SUBST([SCOREP_LIBRARY_WRAPPING_GENERATOR_LDFLAGS],  ["${libwrap_ldflags} -R${llvm_libdir}"])
            AC_SUBST([SCOREP_LIBRARY_WRAPPING_GENERATOR_LIBS],     ["${libwrap_libs}"])],
           [scorep_have_libwrap_generator=no])

AFS_SUMMARY([Library wrapper support], [${scorep_have_libwrap_generator}${reason:+, }${reason}])
AS_UNSET([reason])

AM_COND_IF([HAVE_LIBRARY_WRAPPING_GENERATOR],
      [AFS_SUMMARY([C compiler used],   [${CC}])
       AFS_SUMMARY([C++ compiler used], [${CXX}])])

AC_SCOREP_SVN_CONTROLLED

AC_SCOREP_ERROR_CODES

AFS_SET_SILENT_RULE_PREFIX([LIBWRAP ])
AFS_CONFIG_MAKEFILE

AC_OUTPUT
