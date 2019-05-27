## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2018-2019,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


## file build-config/m4/scorep_fortran_charlen_type.m4

# SCOREP_CHECK_FORTRAN_CHARLEN_TYPE
# -----------------------------------------------------------
# Determine the type for Fortran character lengths
AC_DEFUN([SCOREP_CHECK_FORTRAN_CHARLEN_TYPE],
[AS_IF([test "x${scorep_cv_fc_works}" = "xyes"], [
AC_LANG_PUSH([Fortran])
# $ac_ext is the current value of $ac_fc_srcext, or his default
# $ac_fc_srcext is used with parameter expansion for a default value:
# ${ac_fc_srcext-f}, but that also honors '' as a value, and would not select
# the default value, thus the usual 'save=$var; : use/modify $var; var=$save'
# does not work, as it changes the value to '', even though it was 'f' before
# reported upstream: http://lists.gnu.org/archive/html/bug-autoconf/2018-12/msg00000.html
scorep_ac_ext_save=$ac_ext
scorep_ac_fcflags_srcext_save=$ac_fcflags_srcext
AC_FC_PP_SRCEXT([F])
AC_MSG_CHECKING([for Fortran character length type])
scorep_FCFLAGS_save=$FCFLAGS
FCFLAGS="${FCFLAGS_F:+$FCFLAGS_F }$FCFLAGS"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [[
#if defined( __GFORTRAN__ ) && __GNUC__ >= 8
       choke me
#endif
]])], [
    scorep_fortran_charlen_type=int
], [
    scorep_fortran_charlen_type=size_t
])
FCFLAGS=$scorep_FCFLAGS_save
AS_UNSET([scorep_FCFLAGS_save])
AC_DEFINE_UNQUOTED([scorep_fortran_charlen_t], [$scorep_fortran_charlen_type], [Type to use for implicit character length arguments.])
AC_MSG_RESULT([$scorep_fortran_charlen_type])
ac_fc_srcext=$scorep_ac_ext_save
ac_fcflags_srcext_save=$scorep_ac_fcflags_srcext
AS_UNSET([scorep_ac_ext_save])
AC_LANG_POP([Fortran])
])
])# SCOREP_CHECK_FORTRAN_CHARLEN_TYPE
