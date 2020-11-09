dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2019-2020,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_gcc_atomic_builtins.m4


# SCOREP_CPU_INSTRUCTION_SETS
# ---------------------------
# Determine the instruction set the current compiler builds for.
# As a reasonable starting point, detected instruction sets are limited to
# ppc64, x86_64, and aarch64.
# Aborts if instruction set cannot be determined.
#
AC_DEFUN([SCOREP_CPU_INSTRUCTION_SETS], [
AC_MSG_CHECKING([for instruction set])
instruction_sets="ppc64:   __ppc64__,__powerpc64__,__PPC64__
                  x86_64:  __x86_64,__x86_64__,__amd64,_M_X64
                  aarch64: __aarch64__,__ARM64__,_M_ARM64
                  unknown: UNKNOWN"
AC_LANG_PUSH([C])
for instruction_set_test in ${instruction_sets}; do
    AS_CASE([${instruction_set_test}],
        [*:], [instruction_set=${instruction_set_test}; continue],
        [*], [instruction_set_cpp="defined("`echo ${instruction_set_test} | sed 's/,/) || defined(/g'`") "])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM(,[
    #if !( ${instruction_set_cpp})
      thisisanerror;
    #endif
    ])], [break])
done
AC_LANG_POP([C])
instruction_set=$(echo ${instruction_set} | cut -d: -f1)
AC_MSG_RESULT([${instruction_set}])
AS_IF([test "x${instruction_set}" = xunknown],
    [AC_MSG_ERROR([cannot determine instruction set. Please report this to <AC_PACKAGE_BUGREPORT>.])])
AC_SUBST([SCOREP_CPU_INSTRUCTION_SET], [${instruction_set}])
# AC_DEFINE_UNQUOTED([$var], ...) does not work with AC_CONFIG_HEADERS, thus define all possible instruction sets
AC_DEFINE_UNQUOTED([HAVE_SCOREP_CPU_INSTRUCTION_SET_PPC64], $(if test "x${instruction_set}" = xppc64; then echo 1; else echo 0; fi), [Instruction set ppc64])
AC_DEFINE_UNQUOTED([HAVE_SCOREP_CPU_INSTRUCTION_SET_X86_64], $(if test "x${instruction_set}" = xx86_64; then echo 1; else echo 0; fi), [Instruction set x86_64])
AC_DEFINE_UNQUOTED([HAVE_SCOREP_CPU_INSTRUCTION_SET_AARCH64], $(if test "x${instruction_set}" = xaarch64; then echo 1; else echo 0; fi), [Instruction set aarch_64])
]) # SCOREP_CPU_INSTRUCTION_SETS


# SCOREP_GCC_ATOMIC_BUILTINS
# --------------------------
# Check whether CC supports gcc atomic builtins see e.g.,
# https://gcc.gnu.org/onlinedocs/gcc-8.2.0/gcc/_005f_005fatomic-Builtins.html#g_t_005f_005fatomic-Builtins
# If gcc atomic builtins are available, define HAVE_SCOREP_GCC_ATOMIC_BUILTINS
# to 1 and set the automake conditional HAVE_SCOREP_GCC_ATOMIC_BUILTINS. If
# necessary, defines HAVE_SCOREP_GCC_ATOMIC_BUILTINS_NEEDS_CASTS. Determines
# and substitutes the instruction set CC builds for, see SCOREP_CPU_INSTRUCTION_SET.
# The substitution can be used for selecting precompiled instruction-set-dependent
# SOURCES.
#
# For additional documentation, see src/utils/atomic/SCOREP_Atomic.inc.c.
#
AC_DEFUN([SCOREP_GCC_ATOMIC_BUILTINS], [
AC_REQUIRE([SCOREP_CPU_INSTRUCTION_SETS])
AC_REQUIRE([AX_ASM_INLINE])
AC_LANG_PUSH([C])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
    [[#include "${srcdir}/../src/utils/atomic/SCOREP_Atomic.inc.c"]],
    [[]])],
    [scorep_have_gcc_atomic_builtins=1
     gcc_atomic_builtins_result=yes],
    [AC_LINK_IFELSE(
         [AC_LANG_PROGRAM(
         [[#define HAVE_SCOREP_GCC_ATOMIC_BUILTINS_NEEDS_CASTS 1
           #include "${srcdir}/../src/utils/atomic/SCOREP_Atomic.inc.c"]],
         [[]])],
         [scorep_have_gcc_atomic_builtins=1
          AC_DEFINE([HAVE_SCOREP_GCC_ATOMIC_BUILTINS_NEEDS_CASTS], [1],
              [Define to 1 if casting is needed to use gcc atomic builtins.])
          gcc_atomic_builtins_result="yes, needs pointer-to-int casts"],
         [scorep_have_gcc_atomic_builtins=0
          gcc_atomic_builtins_result="no, using precompiled ${SCOREP_CPU_INSTRUCTION_SET} version"])])
AC_LANG_POP([C])

AC_MSG_CHECKING([for gcc atomic builtins])
AC_MSG_RESULT([$gcc_atomic_builtins_result])
AFS_SUMMARY([GCC atomic builtins], [$gcc_atomic_builtins_result])

AM_CONDITIONAL([HAVE_SCOREP_GCC_ATOMIC_BUILTINS],
    [test "x$scorep_have_gcc_atomic_builtins" = x1])
AC_DEFINE_UNQUOTED([HAVE_SCOREP_GCC_ATOMIC_BUILTINS],
    [$scorep_have_gcc_atomic_builtins],
    [Define to 1 if the compiler supports gcc atomic builtins.])
]) # SCOREP_GCC_ATOMIC_BUILTINS
