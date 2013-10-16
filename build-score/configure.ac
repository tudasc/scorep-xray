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

AFS_PACKAGE_INIT([score])
AFS_SUMMARY_INIT

AC_CONFIG_AUX_DIR([../build-config])
AC_CONFIG_MACRO_DIR([../vendor/common/build-config/m4
                     ../build-config/m4])

AC_CONFIG_HEADERS([../src/config-score.h])

AC_DEFUN([AC_SCOREP_SCORE], [])

AC_SCOREP_CHECK_COMPILER_VAR_SET([CXX])

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

AM_INIT_AUTOMAKE([foreign color-tests 1.11.1 -Wall tar-pax])
AM_SILENT_RULES([yes])

LT_PREREQ([2.4])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

AC_PROG_CC
AC_SCOREP_PROG_CC_C99

AC_PROG_CXX

AC_SCOREP_SVN_CONTROLLED

AC_SCOREP_CUBE_READER

AC_SCOREP_ERROR_CODES

AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
    [ac_scorep_frontend="frontend"],
    [ac_scorep_frontend="backend"])

AS_IF([test -n "${scorep_otf2_bindir}"],
      [AC_SUBST([OTF2_CPPFLAGS], ["`${scorep_otf2_bindir}/otf2-config --cppflags`"])
       AC_SUBST([OTF2_LIBS],     ["`${scorep_otf2_bindir}/otf2-config --libs`"])
       AC_SUBST([OTF2_LDFLAGS],  ["`${scorep_otf2_bindir}/otf2-config --ldflags`"])
       AC_SUBST([OTF2_BINDIR],   ["${scorep_otf2_bindir}"])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OTF2], [test 1 -eq 1])
       scorep_otf2_config=${scorep_otf2_bindir}/otf2-config],
      [AC_SUBST([OTF2_CPPFLAGS], ['-I../vendor/otf2/include -I$(srcdir)/../vendor/otf2/include'])
       AC_SUBST([OTF2_LIBS],     [../vendor/otf2/build-${ac_scorep_frontend}/libotf2.la])
       AC_SUBST([OTF2_LDFLAGS],  [])
       AC_SUBST([OTF2_BINDIR],   [../vendor/otf2/build-${ac_scorep_frontend}])
       AM_CONDITIONAL([HAVE_SCOREP_EXTERNAL_OTF2], [test 1 -eq 0])
       scorep_otf2_config=../vendor/otf2/otf2-mini-config.sh])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
