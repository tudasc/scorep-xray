## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       ac_scorep_external_components.m4
## maintainer Christian Roessel <c.roessel@fz-juelich.de>


dnl $1: name of the component, e.g., otf2, opari2.
dnl $2: minimal current of current:revision:age that is supported.
dnl $3: minimal age of current:revision:age that is supported.
dnl Note that the external current:revision:age is accepted 
dnl if (current-age) == ($2-$3) && age >= $3
dnl Macro will check for availability of $1-config. Intended to be
dnl called from scorep toplevel configure. If with_val=yes, search for
dnl $1-config in PATH, else search in with_val and with_val/bin. Sets
dnl output variables scorep_have_$1_config (yes/no) to be used in
dnl conditionals and scorep_$1_config_arg to pe passed to
dnl sub-configures.
AC_DEFUN([AC_SCOREP_HAVE_CONFIG_TOOL], [

AC_ARG_WITH([$1], 
            [AS_HELP_STRING([--with-$1=(yes|<$1-bindir>)], [Use an already installed $1. Provide path to $1-config if not already in $PATH.])], 
            [with_$1="${with_$1%/}"], 
            [with_$1="no"
             AC_SCOREP_SUMMARY([$1 support], [yes, using internal])])

AS_UNSET([scorep_have_$1_config])
AS_UNSET([scorep_$1_config_bin])
AS_UNSET([scorep_$1_config_arg])
AS_IF([test "x${with_$1}" != "xno"], 
      [AS_IF([test "x${with_$1}" = "xyes"], 
             [AC_CHECK_PROG([scorep_have_$1_config], [$1-config], ["yes"], ["no"])
              AS_IF([test "x${scorep_have_$1_config}" = "xyes"],
                    [scorep_$1_config_bin="`which $1-config`"])],
             [AC_CHECK_PROG([scorep_have_$1_config], [$1-config], ["yes"], ["no"], [${with_$1}])
              AS_IF([test "x${scorep_have_$1_config}" = "xyes"],
                    [scorep_$1_config_bin="${with_$1}/$1-config"],
                    [AS_UNSET([ac_cv_prog_scorep_have_$1_config])
                     AS_UNSET([scorep_have_$1_config])
                     AC_CHECK_PROG([scorep_have_$1_config], [$1-config], ["yes"], ["no"], ["${with_$1}/bin"])
                     AS_IF([test "x${scorep_have_$1_config}" = "xyes"],
                           [scorep_$1_config_bin="${with_$1}/bin/$1-config"])])])

       AS_IF([test "x${scorep_have_$1_config}" = "xyes"], 
             [scorep_$1_config_arg="scorep_$1_bindir=`dirname ${scorep_$1_config_bin}`"
              # version checking, see http://www.gnu.org/software/libtool/manual/libtool.html#Versioning
              interface_version=`${scorep_$1_config_bin} --interface-version 2> /dev/null`
              AS_IF([test $? -eq 0 && test "x${interface_version}" != "x"], 
                    [$1_current=`echo ${interface_version} | awk -F ":" '{print $[]1}'`
                     $1_age=`echo ${interface_version} | awk -F ":" '{print $[]3}'`
                     AS_IF([test ${$1_current} -eq 0 && test ${$1_age} -eq 0],
                           [AC_MSG_WARN([external $1 built from trunk, version checks disabled, might produce compile and link errors.])
                            AC_SCOREP_SUMMARY([$1 support], [yes, using external via ${scorep_$1_config_bin} (built from trunk, version checks disabled, might produce compile and link errors.)])],
                           [AS_VAR_ARITH([$1_current_minus_age], [${$1_current} - ${$1_age}])
                            AS_VAR_ARITH([scorep_current_minus_age], [$2 - $3])
                            AS_IF([test ${$1_current_minus_age} -lt 0], 
                                  [AC_MSG_ERROR([invalid data returned from $1-config --interface-version: ${$1_current}:x:${$1_age}.])])
                            AS_IF([test ${$1_current_minus_age} -eq ${scorep_current_minus_age} && test ${$1_age} -ge $3],
                                  [AC_SCOREP_SUMMARY([$1 support], [yes, using external via ${scorep_$1_config_bin}])],
                                  [AC_MSG_ERROR([interface version ${$1_current}:x:${$1_age} of $1 not supported by Score-P, provide $2:x:$3 or compatible.])])])],
                    [AC_MSG_ERROR([required option --interface-version not supported by $1-config.])])],
       
             [AS_IF([test "x${with_$1}" = "xyes"],
                    [AC_MSG_ERROR([cannot detect $1-config although it was requested via --with-$1.])],
                    [AC_MSG_ERROR([cannot detect $1-config in ${with_$1} and ${with_$1}/bin.])])])],
      [scorep_have_$1_config="no"])
])
