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
dnl Copyright (c) 2009-2013,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2013,
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

AC_CONFIG_AUX_DIR([../build-config])
AC_CONFIG_MACRO_DIR([../vendor/common/build-config/m4
                     ../build-config/m4])

AC_CONFIG_HEADERS([../src/config-score.h])

AC_DEFUN([AC_SCOREP_SCORE], [])

AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX])

AC_SCOREP_SVN_CONTROLLED

AM_INIT_AUTOMAKE([foreign color-tests 1.11.1 -Wall tar-pax])
AM_SILENT_RULES([yes])

AFS_PACKAGE_INIT([score])
AFS_SUMMARY_INIT

# CXX given, find corresponding CC for utilities/filter                                                             
# Do this before calls to AC_PROG_*                                                                                 
CXX_STRIPPED="`echo ${CXX} | awk '{printf $1}'`"
AS_CASE([${CXX_STRIPPED}],
        [g++],         [CC=gcc],   # gnu
        [icpc],        [CC=icc],   # intel
        [CC],          [CC=cc],    # studio
        [xlC*|xlc++*], [CC=xlc_r], # ibm
        [pgCC],        [CC=pgcc],  # pgi
        [AC_MSG_ERROR([invalid value of CXX (${CXX_STRIPPED}), cannot determine corresponding CC])])

AC_PROG_CC
AC_SCOREP_PROG_CC_C99
AC_PROG_CXX

LT_PREREQ([2.4])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

AC_SCOREP_CUBE_READER

AC_SCOREP_ERROR_CODES

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
