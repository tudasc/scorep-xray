## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2019-2020, 2022-2023,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


# AFS_CPU_INSTRUCTION_SETS
# ------------------------
# Determine the instruction set the current compiler builds for.
# As a reasonable starting point, detected instruction sets are limited to
# ppc64, x86_64, and aarch64.
# Aborts if instruction set cannot be determined.
#
AC_DEFUN([AFS_CPU_INSTRUCTION_SETS], [
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
    [AC_MSG_WARN([cannot determine instruction set.])])
AC_SUBST([CPU_INSTRUCTION_SET], [${instruction_set}])
# AC_DEFINE_UNQUOTED([$var], ...) does not work with AC_CONFIG_HEADERS, thus define all possible instruction sets
AC_DEFINE_UNQUOTED([HAVE_CPU_INSTRUCTION_SET_PPC64], $(if test "x${instruction_set}" = xppc64; then echo 1; else echo 0; fi), [Instruction set ppc64])
AC_DEFINE_UNQUOTED([HAVE_CPU_INSTRUCTION_SET_X86_64], $(if test "x${instruction_set}" = xx86_64; then echo 1; else echo 0; fi), [Instruction set x86_64])
AC_DEFINE_UNQUOTED([HAVE_CPU_INSTRUCTION_SET_AARCH64], $(if test "x${instruction_set}" = xaarch64; then echo 1; else echo 0; fi), [Instruction set aarch64])
]) # AFS_CPU_INSTRUCTION_SETS


# AFS_GCC_ATOMIC_BUILTINS
# -----------------------
# Check whether CC supports gcc atomic builtins see e.g.,
# https://gcc.gnu.org/onlinedocs/gcc-8.2.0/gcc/_005f_005fatomic-Builtins.html#g_t_005f_005fatomic-Builtins
# If gcc atomic builtins are available, define HAVE_GCC_ATOMIC_BUILTINS
# to 1 and set the automake conditional HAVE_GCC_ATOMIC_BUILTINS. If
# necessary, defines HAVE_GCC_ATOMIC_BUILTINS_NEEDS_CASTS. Determines
# and substitutes the instruction set CC builds for, see CPU_INSTRUCTION_SET.
# The substitution can be used for selecting precompiled instruction-set-dependent
# SOURCES.
#
# For additional documentation, see common/utils/src/atomic/UTILS_Atomic.inc.c.
#
AC_DEFUN([AFS_GCC_ATOMIC_BUILTINS], [
AC_REQUIRE([AFS_CPU_INSTRUCTION_SETS])
AC_REQUIRE([AX_ASM_INLINE])
AC_LANG_PUSH([C])
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
    [[#include "${srcdir}/../common/utils/src/atomic/UTILS_Atomic.inc.c"]],
    [[]])],
    [afs_have_gcc_atomic_builtins=1
     gcc_atomic_builtins_result=yes],
    [AC_LINK_IFELSE(
         [AC_LANG_PROGRAM(
         [[#define HAVE_GCC_ATOMIC_BUILTINS_NEEDS_CASTS 1
           #include "${srcdir}/../common/utils/src/atomic/UTILS_Atomic.inc.c"]],
         [[]])],
         [afs_have_gcc_atomic_builtins=1
          AC_DEFINE([HAVE_GCC_ATOMIC_BUILTINS_NEEDS_CASTS], [1],
              [Define to 1 if casting is needed to use gcc atomic builtins.])
          gcc_atomic_builtins_result="yes, needs pointer-to-int casts"],
         [afs_have_gcc_atomic_builtins=0
          AS_IF([test "x${CPU_INSTRUCTION_SET}" = xunknown],
              [AC_MSG_ERROR([Cannot provide precompiled GCC atomic builtin wrappers when instruction set is unknown.])],
              [gcc_atomic_builtins_result="no, using precompiled ${CPU_INSTRUCTION_SET} version"])])])
AC_LANG_POP([C])

AC_MSG_CHECKING([for gcc atomic builtins])
AC_MSG_RESULT([$gcc_atomic_builtins_result])
AFS_SUMMARY([GCC atomic builtins], [$gcc_atomic_builtins_result])

# Using "true" as the default for the HAVE_GCC_ATOMIC_BUILTINS conditional
# seems counter-intuitive, but is required to prevent compilation of the
# atomics/mutex fall-back convenience libraries in case the configuration
# macro is not executed.  In this case, the implementation is considered
# "header-only" -- and may fail when used.
AFS_AM_CONDITIONAL([HAVE_GCC_ATOMIC_BUILTINS],
    [test "x$afs_have_gcc_atomic_builtins" = x1], [true])
AC_DEFINE_UNQUOTED([HAVE_GCC_ATOMIC_BUILTINS],
    [$afs_have_gcc_atomic_builtins],
    [Define to 1 if the compiler supports gcc atomic builtins.])
# Only prepare the required assembly file and only if needed.  Note that the
# file may be (over)written multiple times from different build directories,
# though with identical contents.
AS_IF([test "x$afs_have_gcc_atomic_builtins" = x0],
    [AC_CONFIG_FILES([../common/utils/src/atomic/UTILS_Atomic.inc.${CPU_INSTRUCTION_SET}.s])])
]) # AFS_GCC_ATOMIC_BUILTINS
