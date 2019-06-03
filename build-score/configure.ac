dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2013, 2015, 2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2013, 2015-2016, 2018,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2014,
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Technische Universitaet Muenchen, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_PREREQ([2.69])
AC_INIT([Score-P], m4_esyscmd([../vendor/common/build-config/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

AFS_PACKAGE_BUILD_INIT([score])
AFS_SUMMARY_INIT

AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../vendor/common/build-config/m4])

AC_CONFIG_HEADERS([../src/config-score.h])

AC_DEFUN([SCOREP_SCORE], [])

## require $afs_cross_compiling to be set to 'yes' or 'no'
AS_IF([test "x${afs_cross_compiling}" != "xyes" && \
       test "x${afs_cross_compiling}" != "xno"],
    [AC_MSG_ERROR([invalid value '$afs_cross_compiling' for variable \$afs_cross_compiling.], [1])])

# Compilers etc. are set according to Cube installation (see below),
# so ignore environment variables set in the environment.
AS_UNSET([CC])
AS_UNSET([CFLAGS])
AS_UNSET([LDFLAGS])
AS_UNSET([LIBS])
AS_UNSET([CPPFLAGS])
AS_UNSET([LT_SYS_LIBRARY_PATH])
AS_UNSET([CPP])
AS_UNSET([CXX])
AS_UNSET([CXXFLAGS])
AS_UNSET([CXXCPP])

AC_ARG_VAR([CXXFLAGS_FOR_BUILD_SCORE],
    [C++ compiler flags for building scorep-score])
AS_IF([test "x${ac_cv_env_CXXFLAGS_FOR_BUILD_SCORE_set}" = xset],
    [CXXFLAGS="${ac_cv_env_CXXFLAGS_FOR_BUILD_SCORE_value}"])

AC_PROG_AWK
AC_PROG_GREP

# Get CXX that was used to build cube reader libraries.
AS_IF([test -n "${scorep_cubelib_bindir}"],
    [# Get from external cube installation
     CXX="`${scorep_cubelib_bindir}/cubelib-config --cxx | ${AWK} '{print $1}'`"
     AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_CUBELIB], [test 1 -eq 1])],
    [# Get from internal cube. cube's build-backend defines FRONTEND_CXX
     # in cross and non-cross mode.
     CXX="`GREP_OPTIONS= ${GREP} ^FRONTEND_CXX ../vendor/cubelib/build-frontend/config.log | awk -F\' '{print $2}'`"
     AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_CUBELIB], [test 1 -eq 0])])

AC_PROG_CXX
AC_LANG_PUSH([C++])
AX_COMPILER_VENDOR
AC_LANG_POP([C++])

AS_CASE([${ax_cv_cxx_compiler_vendor%/*}],
    [intel],    [CC=icc],
    [ibm],      [CC=xlc_r],
    [gnu],      [CC=gcc],
    [sun],      [CC=cc],
    [portland], [CC=pgcc], # TODO: that may have been 'pgc++ -Mllvm' from the default target, thus 'pgcc' may not be the LLVM variant
    [clang],    [CC=clang],
    [AC_MSG_WARN([invalid value of CXX vendor (${ax_cv_cxx_compiler_vendor}), cannot determine corresponding CC. Using gcc as fall-back.])
     CC=gcc
     fallback=" (fall-back)"])

# Use CXX arguments like -q64 on CC as well to prevent build errors relating
# to incompatible architectures if we are not using the fall-back.
CXX_ARGS="`echo ${CXX} | ${AWK} '{$1 = ""; print $0}'`"
AS_IF([test "x${fallback}" = x],
    [CC="${CC} ${CXX_ARGS}"])

AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR
AFS_MAINTAINER_MODE

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AC_PROG_CC
AC_SCOREP_PROG_CC_C99

LT_PREREQ([2.4.6])
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

AFS_SUMMARY([C compiler used],   [${CC}${fallback}])
AFS_SUMMARY([C++ compiler used], [${CXX}])

AC_SCOREP_SVN_CONTROLLED

SCOREP_CUBELIB
SCOREP_ESTIMATOR

AS_IF([test -n "${scorep_cubelib_bindir}"],
      [AC_SUBST([CUBELIB_BINDIR], [${scorep_cubelib_bindir}])],
      [AC_SUBST([CUBELIB_BINDIR], [../vendor/cubelib/build-frontend])
])

AC_SCOREP_ERROR_CODES

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([../test/profiling/run_profile_depth_limit_test.sh], \
                [chmod +x ../test/profiling/run_profile_depth_limit_test.sh])

AC_OUTPUT
