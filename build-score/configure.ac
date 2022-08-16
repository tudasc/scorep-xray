dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2013, 2015, 2019, 2022,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2013, 2015-2016, 2018, 2020-2022,
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
AC_INIT([Score-P], m4_esyscmd([../build-config/common/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

AFS_PACKAGE_BUILD_INIT([score])
AFS_SUMMARY_INIT

AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../build-config/common/m4])

AC_CONFIG_HEADERS([../src/config-score.h])

AC_DEFUN([SCOREP_SCORE], [])

## require $afs_cross_compiling to be set to 'yes' or 'no'
AS_IF([test "x${afs_cross_compiling}" != "xyes" && \
       test "x${afs_cross_compiling}" != "xno"],
    [AC_MSG_ERROR([invalid value '$afs_cross_compiling' for variable \$afs_cross_compiling.], [1])])

# Compilers etc. are set according to cubelib installation (see
# below), so intentionally ignore environment variables set in the
# environment. For CFLAGS, CXXFLAGS, and LDFLAGS see
# *FLAGS_FOR_BUILD_SCORE below.
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

AC_PROG_AWK
AC_PROG_GREP
AC_PROG_SED

# Get CC and CXX (without leading whitespace) that was used to build
# cubelib. Might contain language and architecture options like
# '-std=c++11' or '-q64'.
AS_IF([test -n "${scorep_cubelib_bindir}"],
    [# Get from external cube installation
     CC="`${scorep_cubelib_bindir}/cubelib-config --cc | ${SED} 's/^[ \t]*//'`"
     CXX="`${scorep_cubelib_bindir}/cubelib-config --cxx | ${SED} 's/^[ \t]*//'`"
     AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_CUBELIB], [test 1 -eq 1])],
    [# Get from internal cubelib
     CC="`GREP_OPTIONS= ${GREP} ^CC= ../vendor/cubelib/build-frontend/config.log | ${AWK} -F\' '{gsub(/^ +/,"",$2); print $2}'`"
     CXX="`GREP_OPTIONS= ${GREP} ^CXX= ../vendor/cubelib/build-frontend/config.log | ${AWK} -F\' '{gsub(/^ +/,"",$2); print $2}'`"
     AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_CUBELIB], [test 1 -eq 0])])

# Provide CFLAGS, CXXFLAGS, ands LDFLAGS for build-score via
# *FLAGS_FOR_BUILD_SCORE environment variables. If not set,
# use autoconf's defaults.
AC_ARG_VAR([CFLAGS_FOR_BUILD_SCORE],
    [C compiler flags for building scorep-score. Please note that CC comes from cubelib.])
AS_IF([test "x${ac_cv_env_CFLAGS_FOR_BUILD_SCORE_set}" != xset],
    [AS_UNSET([CFLAGS])],
    [CFLAGS="${ac_cv_env_CFLAGS_FOR_BUILD_SCORE_value}"])
AC_ARG_VAR([CXXFLAGS_FOR_BUILD_SCORE],
    [C++ compiler flags for building scorep-score. Please note that CXX comes from cubelib.])
AS_IF([test "x${ac_cv_env_CXXFLAGS_FOR_BUILD_SCORE_set}" != xset],
    [AS_UNSET([CXXFLAGS])],
    [CXXFLAGS="${ac_cv_env_CXXFLAGS_FOR_BUILD_SCORE_value}"])
AC_ARG_VAR([LDFLAGS_FOR_BUILD_SCORE],
    [Linker flags for building scorep-score.])
AS_IF([test "x${ac_cv_env_LDFLAGS_FOR_BUILD_SCORE_set}" != xset],
    [AS_UNSET([LDFLAGS])],
    [LDFLAGS="${ac_cv_env_LDFLAGS_FOR_BUILD_SCORE_value}"])

# Don't use any options to AFS_PROG_* but use the dialect provided
# (from 4.7 on) by cubelib-config.
AFS_PROG_CC
AFS_PROG_CXX

AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR
AFS_MAINTAINER_MODE

AC_CANONICAL_BUILD
AC_CANONICAL_HOST

LT_PREREQ([2.4.6])
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

AFS_PROG_CC_SUMMARY
AFS_PROG_CXX_SUMMARY

AC_SCOREP_GIT_CONTROLLED

SCOREP_CUBELIB
SCOREP_ESTIMATOR

AS_IF([test -n "${scorep_cubelib_bindir}"],
      [AC_SUBST([CUBELIB_BINDIR], [${scorep_cubelib_bindir}])],
      [AC_SUBST([CUBELIB_BINDIR], [../vendor/cubelib/build-frontend])
])

AFS_COMMON_UTILS([build], [<SCOREP_ErrorCodes.h>], [../src/utils/include])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([../test/profiling/run_profile_depth_limit_test.sh], \
                [chmod +x ../test/profiling/run_profile_depth_limit_test.sh])

AC_OUTPUT
