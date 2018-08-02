## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2018,
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
[AC_LANG_PUSH([Fortran])
scorep_fc_pp_define_srcext_save=$ac_fc_srcext
AC_FC_PP_SRCEXT([F])
AC_MSG_CHECKING([for Fortran character length type])
FCFLAGS_save=$FCFLAGS
FCFLAGS="$FCFLAGS_F $FCFLAGS"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [[
#if defined( __GFORTRAN__ ) && __GNUC__ >= 8
    choke me
#endif
]])], [
    scorep_fortran_charlen_type=int
], [
    scorep_fortran_charlen_type=size_t
])
AC_DEFINE_UNQUOTED([scorep_fortran_charlen_t], [$scorep_fortran_charlen_type], [Type to use for implicit character length arguments.])
AC_MSG_RESULT([$scorep_fortran_charlen_type])
ac_fc_srcext=$scorep_fc_pp_define_srcext_save
AC_LANG_POP([Fortran])
])# SCOREP_CHECK_FORTRAN_CHARLEN_TYPE
