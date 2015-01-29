dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2013,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2013,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2013, 2015,
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

AC_PREREQ([2.68])
AC_INIT([Score-P], m4_esyscmd([../vendor/common/build-config/generate-package-version.sh ../build-config/VERSION]), [support@score-p.org], [scorep])

AFS_PACKAGE_BUILD_INIT([score])
AFS_SUMMARY_INIT

AC_CONFIG_AUX_DIR([../build-config])
AC_CONFIG_MACRO_DIR([../vendor/common/build-config/m4
                     ../build-config/m4])

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

AM_INIT_AUTOMAKE([foreign color-tests 1.11.1 -Wall tar-pax])
AM_SILENT_RULES([yes])

LT_PREREQ([2.4])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

AC_PROG_CC
AC_SCOREP_PROG_CC_C99

AC_PROG_CXX

AC_SCOREP_SVN_CONTROLLED

SCOREP_CUBE_READER
SCOREP_ESTIMATOR

AC_SCOREP_ERROR_CODES

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
