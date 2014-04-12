dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013-2014
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_estimator.m4

AC_DEFUN([SCOREP_ESTIMATOR], [

AS_IF([test "x${scorep_otf2_bindir}" = x],
      [# internal OTF2
       adl_RECURSIVE_EVAL([${bindir}], [otf2_estimator_bindir])
       AC_DEFINE_UNQUOTED([OTF2_ESTIMATOR_INSTALL],
           ["${otf2_estimator_bindir}/otf2-estimator"],
           [The otf2-estimator tool])],
      [test -x "${scorep_otf2_bindir}/otf2-estimator"],
      [# external OTF2
       AC_DEFINE_UNQUOTED([OTF2_ESTIMATOR_INSTALL],
           ["${scorep_otf2_bindir}/otf2-estimator"],
           [The otf2-estimator tool])],
      [AC_MSG_ERROR([No otf2-estimator found. Need OTF2 version 1.4 or later.])
      ])

])
