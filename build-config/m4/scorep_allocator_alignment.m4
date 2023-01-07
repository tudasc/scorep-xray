dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2023,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_DEFUN([SCOREP_ALLOCATOR_ALIGNMENT],
[dnl
AC_SCOREP_CHECK_SIZEOF([void *])
AC_SCOREP_CHECK_SIZEOF([double])
AS_IF([test $ac_cv_sizeof_void_p -gt $ac_cv_sizeof_double],
      [scorep_allocator_alignment=$ac_cv_sizeof_void_p],
      [scorep_allocator_alignment=$ac_cv_sizeof_double])
AC_MSG_CHECKING([for aligment $scorep_allocator_alignment being a power-of-two])
AC_COMPILE_IFELSE(
    [AC_LANG_BOOL_COMPILE_TRY(
        [],
        [( $scorep_allocator_alignment & ( $scorep_allocator_alignment - 1 ) ) == 0])],
    [AC_MSG_RESULT([yes])],
    [AC_MSG_RESULT([no])
     AC_MSG_ERROR([Unsupported alignment, expected a power-of-two: $scorep_allocator_alignment])])

AC_DEFINE_UNQUOTED(
    [[SCOREP_ALLOCATOR_ALIGNMENT]],
    [$scorep_allocator_alignment],
    [First guess, use the maximum of sizeof(void*) and sizeof(double) as alignment for SCOREP_Allocator.])
])dnl
