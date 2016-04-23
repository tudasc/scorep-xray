dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2013, 2015,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2013, 2015-2016,
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

AFS_PACKAGE_BUILD_INIT([score])
AFS_SUMMARY_INIT

AC_CONFIG_AUX_DIR([../build-config])
## See also ACLOCAL_AMFLAGS in Makefile.am.
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../vendor/common/build-config/m4])

AC_CONFIG_HEADERS([../src/config-score.h])

AC_DEFUN([SCOREP_SCORE], [])

AC_PROG_AWK
AC_PROG_GREP

# Get CXX that was used to build cube reader libraries.
AS_IF([test -n "${scorep_cube_bindir}"],
    [# Get from external cube installation
     CXX="`${scorep_cube_bindir}/cube-config --reader-cxx | ${AWK} '{print $1}'`"
     AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_CUBE], [test 1 -eq 1])],
    [# Get from internal cube. cube's build-backend defines FRONTEND_CXX
     # in cross and non-cross mode.
     CXX="`GREP_OPTIONS= ${GREP} ^FRONTEND_CXX ../vendor/cube/build-backend/config.log | awk -F\' '{print $2}'`"
     AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_CUBE], [test 1 -eq 0])])

# With CXX given, find corresponding CC for utilities/filter.
# Do this before calls to AC_PROG_*
CXX_STRIPPED="`echo ${CXX} | ${AWK} '{printf $1}'`"
AS_CASE([`basename ${CXX_STRIPPED}`],
        [g++],         [CC=gcc],   # gnu
        [icpc],        [CC=icc],   # intel
        [CC],          [CC=cc],    # studio
        [xlC*|xlc++*], [CC=xlc_r], # ibm
        [pgCC|pgc++],  [CC=pgcc],  # pgi
        [AC_MSG_ERROR([invalid value of CXX (${CXX_STRIPPED}), cannot determine corresponding CC])])
# Use CXX arguments like -q64 on CC as well to prevent build errors relating
# to incompatible architectures.
CXX_ARGS="`echo ${CXX} | ${AWK} '{$1 = ""; print $0}'`"
CC="${CC} ${CXX_ARGS}"

AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR

LT_PREREQ([2.4.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

AC_PROG_CC
AC_SCOREP_PROG_CC_C99

AC_PROG_CXX

AC_SCOREP_SVN_CONTROLLED

SCOREP_CUBE_READER
SCOREP_ESTIMATOR

AS_IF([test -n "${scorep_cube_bindir}"],
      [AC_SUBST([CUBE_BINDIR], [${scorep_cube_bindir}])],
      [AS_IF([test "${afs_cross_compiling}" = xyes],
             [AC_SUBST([CUBE_BINDIR], [../vendor/cube/build-frontend])],
             [AC_SUBST([CUBE_BINDIR], [../vendor/cube/build-backend])
      ])
])

AC_SCOREP_ERROR_CODES

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([../test/profiling/run_profile_depth_limit_test.sh], \
                [chmod +x ../test/profiling/run_profile_depth_limit_test.sh])

AC_OUTPUT
