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
dnl Macro will check for availability of $1-config. Intended to be
dnl called from scorep toplevel configure. If with_val=yes, search for
dnl $1-config in PATH, else search in with_val and with_val/bin. Sets
dnl output variables scorep_have_$1_config (yes/no) to be used in
dnl conditionals and scorep_$1_config_arg to pe passed to
dnl sub-configures.
AC_DEFUN([AC_SCOREP_HAVE_CONFIG_TOOL], [

AC_ARG_WITH([$1], 
            [AS_HELP_STRING([--with-$1], [Path to $1-config if not already in $PATH.])], 
            [with_$1="${with_$1%/}"], 
            [with_$1="no"
             AC_SCOREP_SUMMARY([$1 support], [yes, using internal.])])

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
              # add version checking here. if successful:
              AC_SCOREP_SUMMARY([$1 support], [yes, using external via ${scorep_$1_config_bin}.])],
             [AS_IF([test "x${with_$1}" = "xyes"],
                    [AC_MSG_ERROR([cannot detect $1-config although it was requested via --with-$1.])],
                    [AC_MSG_ERROR([cannot detect $1-config in ${with_$1} and ${with_$1}/bin.])])])],
      [scorep_have_$1_config="no"])
])
