## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2012, 2015,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


AC_DEFUN([SCOREP_PDT], [
AC_REQUIRE([AC_PROG_AWK])

## Evalute parameters
AC_ARG_WITH([pdt],
    [AS_HELP_STRING([--with-pdt=<path-to-binaries>],
        [Specifies the path to the program database toolkit (PDT) binaries, e.g., cparse.])],
    [pdt_search_path=$withval$PATH_SEPARATOR$PATH],
    [pdt_search_path=$PATH])

## Check for pdt programs
for pdt_prog in cparse cxxparse gfparse tau_instrumentor; do
    AC_PATH_PROG([scorep_have_pdt],
        [${pdt_prog}],
        [no],
        [${pdt_search_path}])
    AS_IF([test "x${scorep_have_pdt}" = xno],
        [AS_UNSET([pdt_bin_path])
        break],
        [pdt_bin_path=`AS_DIRNAME([${scorep_have_pdt}])`
        # reset vars for next loop iteration
        AS_UNSET([scorep_have_pdt])
        AS_UNSET([ac_cv_path_scorep_have_pdt])])
done
AS_IF([test -n "${pdt_bin_path}"], [scorep_have_pdt=yes])

AS_IF([test "x${scorep_have_pdt}" = xyes],
    [## Try to obtain pdt version from README file
    AS_IF([test -f ${pdt_bin_path}/../../README],
        [pdt_version=`${AWK} '{if (NR==2) print $[]2}' ${pdt_bin_path}/../../README`
        # expect pdt_version to be major.minor
        pdt_major_version=${pdt_version%.*}
        pdt_minor_version=${pdt_version#*.}
        AC_MSG_CHECKING([for PDT version])
        AC_MSG_RESULT([${pdt_major_version}.${pdt_minor_version}])
        pdt_info_msg=", version ${pdt_major_version}.${pdt_minor_version}"])
    ## Blue Gene handling
    AS_CASE([${ac_scorep_platform}],
        [bg*],
        [AS_IF([test -n ${pdt_major_version} && test -n ${pdt_minor_version}],
            [AS_IF([test ${pdt_major_version} -eq 3 && test ${pdt_minor_version} -gt 18],
                [],
                [## Version 3.18 and earlier(?) broken
                AC_MSG_NOTICE([PDT version ${pdt_major_version}.${pdt_minor_version} not supported on Blue Gene systems])
                pdt_info_msg=", version ${pdt_major_version}.${pdt_minor_version} not supported on Blue Gene systems"
                AS_UNSET([pdt_bin_path])
                scorep_have_pdt=no])
            ],
            [## Version couldn't be obtained
            AC_MSG_NOTICE([PDT version unknown, not supported on Blue Gene systems])
            pdt_info_msg="PDT version unknown, not supported on Blue Gene systems"
            AS_UNSET([pdt_bin_path])
            scorep_have_pdt=no
            ])])])

AC_MSG_CHECKING([for PDT])
AC_MSG_RESULT([${scorep_have_pdt}])

## Create output
AC_SUBST([SCOREP_PDT_PATH], ["${pdt_bin_path}"])
AS_IF([test "x${scorep_have_pdt}" = xyes],
    [AC_DEFINE([HAVE_SCOREP_PDT], [1], [Define to 1 if PDT support available])],
    [AC_DEFINE([HAVE_SCOREP_PDT], [0], [Define to 1 if PDT support available])])
AM_CONDITIONAL([HAVE_SCOREP_PDT], [test "x${scorep_have_pdt}" = xyes])
AFS_SUMMARY([PDT support], [${scorep_have_pdt}${pdt_info_msg}])
])
