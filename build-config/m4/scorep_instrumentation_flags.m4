dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013, 2015, 2020-2022,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2013-2015, 2019, 2021,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2016,
dnl Technische Universitaet Darmstadt, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_instrumentation_flags.m4


# SCOREP_INSTRUMENTATION_FLAGS()
# ------------------------------
# Provides additional flags as substitutions
# (SCOREP_INSTRUMENTATION_<LANG>FLAGS and
# SCOREP_INSTRUMENTATION_LDFLAGS) needed for general instrumentation
# (see SCOREP_COMPILER_INSTRUMENTATION_FLAGS for compiler
# instrumentation).
#
AC_DEFUN_ONCE([SCOREP_INSTRUMENTATION_FLAGS], [
AC_REQUIRE([SCOREP_COMPUTENODE_CC])dnl
AC_REQUIRE([SCOREP_COMPUTENODE_CXX])dnl
dnl AC_REQUIRE([SCOREP_COMPUTENODE_FC])dnl
dnl
# Since version 5 the GCC compiler performs 'Identical Code Folding' [1] on
# functions which have the exact same instructions. This "disturb unwind stacks",
# thus disable ICF when instrumenting application code via -fno-ipa-icf
# [1] https://gcc.gnu.org/onlinedocs/gcc-5.5.0/gcc//Optimize-Options.html#index-fipa-icf
dnl
# K Computer used to require -Ntl_notrt at link time to prevent
# linkage of Fujitsu's profiling tools. Newer Fujistu compilers might
# need -Nnofjprof (in traditional mode) or -ffj-no-fjprof (in clang
# mode) for C/C++ linkage. The Fortran option would be
# -Nnofjprof. More investigation needed.
dnl
# For BG/Q with GNU compilers we used to add -dynamic in conjunction
# with nm instrumentation. Both, BG/Q and nm instrumentation are gone.
dnl
AC_LANG_PUSH([C])
_CHECK_INSTRUMENTATION_FLAGS
AC_LANG_POP([C])
AS_IF([test x$afs_cv_prog_cxx_works = xyes], [
    AC_LANG_PUSH([C++])
    _CHECK_INSTRUMENTATION_FLAGS
    AC_LANG_POP([C++])])
AS_IF([test x$afs_cv_prog_fc_works = xyes], [
    AC_LANG_PUSH([Fortran])
    _CHECK_INSTRUMENTATION_FLAGS
    AC_LANG_POP([Fortran])])
dnl
AC_SUBST([SCOREP_INSTRUMENTATION_LDFLAGS], [])
])dnl SCOREP_INSTRUMENTATION_FLAGS


# _CHECK_INSTRUMENTATION_FLAGS()
# ------------------------------
#
m4_define([_CHECK_INSTRUMENTATION_FLAGS], [
AS_CASE([${ax_cv_[]_AC_LANG_ABBREV[]_compiler_vendor%/*}],
    [gnu], [_FLAG_TEST([scorep_instrumentation_[]_AC_LANG_ABBREV[]flags], [-fno-ipa-icf])],
    [])
dnl
AC_SUBST(SCOREP_INSTRUMENTATION_[]_AC_LANG_PREFIX[]FLAGS, ["${scorep_instrumentation_[]_AC_LANG_ABBREV[]flags}"])
AS_IF([test "x${scorep_instrumentation_[]_AC_LANG_ABBREV[]flags}" != x],
    [AC_MSG_NOTICE([using instrumentation []_AC_LANG_ABBREV[]flags: ${scorep_instrumentation_[]_AC_LANG_ABBREV[]flags}])])
])dnl _CHECK_INSTRUMENTATION_FLAGS


# _FLAG_TEST()
# ------------
# Check whether compiler accepts flag $2. Note that some compilers
# rarely choke on unknown flags, e.g., Intel compiler.  Append
# accepted flag to $1.
#
m4_define([_FLAG_TEST], [
_AC_LANG_PREFIX[]FLAGS_save="$_AC_LANG_PREFIX[]FLAGS"
_AC_LANG_PREFIX[]FLAGS="$_AC_LANG_PREFIX[]FLAGS $2"
dnl
AC_MSG_CHECKING([whether _AC_LANG compiler understands $2])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
    [AC_MSG_RESULT([yes])
     $1="$[]$1 $2"],
    [AC_MSG_RESULT([no])])
dnl
_AC_LANG_PREFIX[]FLAGS="$_AC_LANG_PREFIX[]FLAGS_save"
])dnl _FLAG_TEST
