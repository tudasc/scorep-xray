dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2019,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file scorep_iquote_support.m4

m4_define([_SCOREP_CHECK_IQUOTE], [
# Try to compile a file that includes "foo.h" where foo.h resides in
# $subdir.
CPPFLAGS_save=${CPPFLAGS}
CPPFLAGS="-iquote ${subdir} ${CPPFLAGS}"
AC_MSG_CHECKING([whether _AC_LANG compiler supports -iquote])
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
         [[#include "foo.h"]],
         [[]])],
    [AC_MSG_RESULT([yes])
     scorep_have_iquote_support=1],
    [AC_MSG_RESULT([no])
     scorep_have_iquote_support=0])
CPPFLAGS=${CPPFLAGS_save}
]) # _SCOREP_CHECK_IQUOTE


# SCOREP_IQUOTE_SUPPORT
# ---------------------
# Test whether the CC supports the -iquote <dir> directory option.
# This flag adds <dir> to the front of the include file search
# path for files included with quotes but not brackets.
# The instrumenter uses -iquote instead of -I to deal with cases where
# a system header is included via a local header of the same name and
# where scorep moves the file to be compiled out of the src tree
# (opari2).
#
AC_DEFUN([SCOREP_IQUOTE_SUPPORT],[
# create subdir and header file
{
    subdir=`(umask 077 && mktemp -d "./iquote-XXXXXX") 2>/dev/null` &&
    test -d "${subdir}"
} || {
    subdir=./iquote-$$$RANDOM
    (umask 077 && mkdir "${subdir}")
} || exit $?
touch "${subdir}"/foo.h

AC_LANG_PUSH([C])
_SCOREP_CHECK_IQUOTE
AC_SUBST([HAVE_SCOREP_C_IQUOTE_SUPPORT], [${scorep_have_iquote_support}])
AC_LANG_POP([C])

AC_LANG_PUSH([C++])
_SCOREP_CHECK_IQUOTE
AC_SUBST([HAVE_SCOREP_CXX_IQUOTE_SUPPORT], [${scorep_have_iquote_support}])
AC_LANG_POP([C++])

# The scenario where a system header is included via a local header of
# the same name does not apply to Fortran.

rm -rf "${subdir}"
]) # SCOREP_IQUOTE_SUPPORT
