## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2014-2015, 2017,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


# SCOREP_DETECT_MIC_BUILD
# -----------------------
# Guesstimates whether a native MIC build already exists in the given install
# locations. The decision is based on the existance of a 'scorep-config-mic'
# tool in '${libexecdir}/scorep', which is of course only a very crude
# heuristic.
#
# List of configure variables set:
#  `scorep_have_mic_support`:: Set to 'yes' if a native MIC build was detected,
#                              'no' otherwise.
#
# List of provided config header defines:
#  `HAVE_MIC_SUPPORT`:: Defined if a native MIC build was detected
#
AC_DEFUN([SCOREP_DETECT_MIC_BUILD], [
AC_REQUIRE([AC_SCOREP_DETECT_PLATFORMS])
# search for scorep-config-mic only during non-mic configure runs, see #1095.
AS_IF([test "x${ac_scorep_platform}" != xmic], [
    AC_MSG_CHECKING([for native Intel MIC build])
        scorep_have_mic_support=no
        adl_RECURSIVE_EVAL([${libexecdir}], [LIBEXECDIR])
        AS_IF([AS_EXECUTABLE_P([${LIBEXECDIR}/scorep/scorep-config-mic])],
                [scorep_have_mic_support=yes
                 AC_DEFINE([HAVE_MIC_SUPPORT],
                           [1],
                           [Defined to 1 if native MIC build exists])
                 AFS_SUMMARY([MIC target support], [$scorep_have_mic_support])
                ])
        AC_MSG_RESULT([$scorep_have_mic_support])])
])# SCOREP_DETECT_MIC_BUILD
