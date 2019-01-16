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

dnl file scorep_iquote.m4

# SCOREP_IQUOTE_SUPPORT
# ---------------------
# Test whether the CC supports the -iquote <dir> directory option.
# This flag adds <dir> to the front of the include file search
# path for files included with quotes but not brackets.
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

# Try to compile a file that includes "foo.h" where foo.h resides in
# $subdir.
CPPFLAGS_save=${CPPFLAGS}
CPPFLAGS="-iquote ${subdir} ${CPPFLAGS}"
AC_LANG_PUSH([C])
AC_MSG_CHECKING([whether $CC supports -iquote])
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
         [[#include "foo.h"]],
         [[]])],
    [AC_MSG_RESULT([yes])
     scorep_have_iquote_support=1],
    [AC_MSG_RESULT([no])
     ls -l $subdir
     scorep_have_iquote_support=0])
AC_LANG_POP([C])
CPPFLAGS=${CPPFLAGS_save}
rm -rf "${subdir}"

AC_SUBST([HAVE_SCOREP_IQUOTE_SUPPORT], [${scorep_have_iquote_support}])
]) # SCOREP_IQUOTE_SUPPORT
