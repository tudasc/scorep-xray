## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2013,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2013,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2013,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2013,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/ac_scorep_external_components.m4


dnl $1: name of the component, e.g., otf2, opari2.
dnl $2: requested interface version to be provided by external $1 to build this package.
dnl Note that the external's current:revision:age is
dnl accepted if current >= $2 && $2 >= (current - age)
dnl Macro will check for availability of $1-config. Intended to be
dnl called from a toplevel configure. If with_val=yes, search for
dnl $1-config in PATH, else search in with_val and with_val/bin. Sets
dnl output variables scorep_have_$1_config (yes/no) to be used in
dnl conditionals and scorep_$1_config_arg to pe passed to
dnl sub-configures.
AC_DEFUN([AC_SCOREP_HAVE_CONFIG_TOOL], [

AC_ARG_WITH([$1],
    [AS_HELP_STRING([--with-$1[[=<$1-bindir>]]],
        [Use an already installed $1. Provide path to $1-config. Auto-detected if already in $PATH.])],
    [with_$1="${with_$1%/}"], dnl yes, no, or <path>
    [with_$1="not_given"])

AS_UNSET([scorep_have_$1_config])
AS_UNSET([scorep_$1_config_bin])
AS_UNSET([scorep_$1_config_arg])
AS_IF([test "x${with_$1}" != "xno"],
    [AS_IF([test "x${with_$1}" = "xyes" || test "x${with_$1}" = "xnot_given"],
        [AC_CHECK_PROG([scorep_have_$1_config], [$1-config], ["yes"], ["no"])
         AS_IF([test "x${scorep_have_$1_config}" = "xyes"],
             [scorep_$1_config_bin="`which $1-config`"])],
        [# --with-$1=<path>
         AC_CHECK_PROG([scorep_have_$1_config], [$1-config], ["yes"], ["no"], [${with_$1}])
         AS_IF([test "x${scorep_have_$1_config}" = "xyes"],
             [scorep_$1_config_bin="${with_$1}/$1-config"],
             [AS_UNSET([ac_cv_prog_scorep_have_$1_config])
              AS_UNSET([scorep_have_$1_config])
              AC_CHECK_PROG([scorep_have_$1_config], [$1-config], ["yes"], ["no"], ["${with_$1}/bin"])
              AS_IF([test "x${scorep_have_$1_config}" = "xyes"],
                  [scorep_$1_config_bin="${with_$1}/bin/$1-config"])])
        ])
     AS_IF([test "x${scorep_have_$1_config}" = "xyes"],
         [scorep_$1_config_arg="scorep_$1_bindir=`echo "$(cd "$(dirname "${scorep_$1_config_bin}")" && pwd)"`"
          # version checking, see http://www.gnu.org/software/libtool/manual/libtool.html#Versioning
          interface_version=`${scorep_$1_config_bin} --interface-version 2> /dev/null`
          AS_IF([test $? -eq 0 && test "x${interface_version}" != "x"],
              [# get 'current'
               $1_max_provided_interface_version=`echo ${interface_version} | awk -F ":" '{print $[]1}'`
               # get 'age'
               $1_provided_age=`echo ${interface_version} | awk -F ":" '{print $[]3}'`
              AS_IF([test ${$1_max_provided_interface_version} -eq 0 && test ${$1_provided_age} -eq 0],
                  [# by convention, trunk is 0:0:0
                   AC_MSG_WARN([external $1 built from trunk, version checks disabled, might produce compile and link errors.])
                   AFS_SUMMARY([$1 support], [yes, using external via ${scorep_$1_config_bin} (built from trunk, version checks disabled, might produce compile and link errors.)])],
                  [# calc 'current - age'
                   AS_VAR_ARITH([$1_min_provided_interface_version], [${$1_max_provided_interface_version} - ${$1_provided_age}])
                   # this is the version check:
                   AS_IF([test ${$1_max_provided_interface_version} -ge $2 && \
                          test $2 -ge ${$1_min_provided_interface_version}],
                       [AFS_SUMMARY([$1 support], [yes, using external via ${scorep_$1_config_bin}])],
                       [AS_IF([test ${$1_provided_age} -eq 0],
                           [AC_MSG_ERROR([provided interface version '${$1_max_provided_interface_version}' of $1 not sufficient for AC_PACKAGE_NAME, provide '$2' or compatible.])],
                           [AC_MSG_ERROR([provided interface versions [[${$1_min_provided_interface_version},${$1_max_provided_interface_version}]] of $1 not sufficient for AC_PACKAGE_NAME, provide '$2' or compatible.])
])])])
              ],
              [AC_MSG_ERROR([required option --interface-version not supported by $1-config.])])
         ],
         [# scorep_have_$1_config = no
          AS_IF([test "x${with_$1}" = "xnot_given"],
              [AFS_SUMMARY([$1 support], [yes, using internal])],
              [test "x${with_$1}" = "xyes"],
              [AC_MSG_ERROR([cannot detect $1-config although it was requested via --with-$1.])],
              [AC_MSG_ERROR([cannot detect $1-config in ${with_$1} and ${with_$1}/bin.])
              ])
         ])
    ],
    [# --without-$1
     AS_IF([test ! -d ${srcdir}/vendor/$1],
         [AC_MSG_ERROR([$1 is required. Opting out an external $1 via --without-$1 is only an option if an internal $1 is available, which isn't the case here. Please provide an external $1.])])
     scorep_have_$1_config="no"
     AFS_SUMMARY([$1 support], [yes, using internal])])
])
