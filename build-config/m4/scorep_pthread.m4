dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013-2014,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2013, 2017,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_pthread.m4


# --------------------------------------------------------------------

# SCOREP_CHECK_PTHREAD
# --------------
# Wrapper around AX_PTHREAD to provide AC_REQUIRE([SCOREP_CHECK_PTHREAD])
# functionality.
AC_DEFUN([SCOREP_CHECK_PTHREAD], [

dnl advertise the PTHREAD_CFLAGS and PTHREAD_LIBS environment variables
AC_ARG_VAR([PTHREAD_CFLAGS], [CFLAGS used to compile Pthread programs])
AC_ARG_VAR([PTHREAD_LIBS], [LIBS used to link Pthread programs])

AX_PTHREAD([scorep_have_pthread=1], [scorep_have_pthread=0])
])dnl SCOREP_CHECK_PTHREAD

# --------------------------------------------------------------------

# _SCOREP_PTHREAD_FEATURE
# -----------------------
# Helper macro for SCOREP_PTHREAD_* macros.
# $1: feature name (single word, must be usable as shell variable)
# $2: flags to loop over
# $3: program source to be used in AC_LANG_PROGRAM
# Output: SCOREP_HAVE_PTHREAD_[]m4_toupper($1) substitution
#         SCOREP_PTHREAD_[]m4_toupper($1)_CPPFLAGS substitution
#         HAVE_PTHREAD_[]m4_toupper($1) automake conditional
m4_define([_SCOREP_PTHREAD_FEATURE], [
AC_REQUIRE([SCOREP_CHECK_PTHREAD])dnl
scorep_pthread_$1=0
scorep_pthread_$1_flag=""
AS_IF([test "x${scorep_have_pthread}" = x1],
    [AC_LANG_PUSH([C])dnl
     AC_MSG_CHECKING([for pthread $1])
     for scorep_pthread_$1_flag in $2
     do
         save_LIBS="$LIBS"
         LIBS="$PTHREAD_LIBS"
         save_CFLAGS="$CFLAGS"
         CFLAGS="$PTHREAD_CFLAGS $scorep_pthread_$1_flag"
         save_CC="$CC"
         CC="$PTHREAD_CC"

         AC_LINK_IFELSE([AC_LANG_PROGRAM($3)],
             [scorep_pthread_$1=1],
             [scorep_pthread_$1=0])dnl

         LIBS="$save_LIBS"
         CFLAGS="$save_CFLAGS"
         CC="$save_CC"

         AS_IF([test "x$scorep_pthread_$1" = x1], [break])dnl
     done
     AS_IF([test "x$scorep_pthread_$1" = x0],
         [scorep_pthread_$1_flag=""
          AC_MSG_RESULT([no])],
         [AC_MSG_RESULT([yes])])
     AC_LANG_POP([C])dnl
    ])dnl

AC_SUBST(SCOREP_HAVE_PTHREAD_[]m4_toupper($1), [${scorep_pthread_$1}])dnl
AC_SUBST(SCOREP_PTHREAD_[]m4_toupper($1)_CPPFLAGS, [${scorep_pthread_$1_flag}])dnl
AFS_AM_CONDITIONAL(HAVE_PTHREAD_[]m4_toupper($1), [test "x$scorep_pthread_$1" = x1], [false])dnl
])

# --------------------------------------------------------------------

# SCOREP_PTHREAD_SPINLOCK
# -----------------------
# Check for Pthread spinlocks, provides:
#   SCOREP_HAVE_PTHREAD_SPINLOCK substitution
#   SCOREP_PTHREAD_SPINLOCK_CPPFLAGS substitution
#   HAVE_PTHREAD_SPINLOCK automake conditional
AC_DEFUN([SCOREP_PTHREAD_SPINLOCK], [
_SCOREP_PTHREAD_FEATURE([spinlock],
    ["" -D_XOPEN_SOURCE=600 -D_POSIX_C_SOURCE=200112L],
    [
     [[
     #include <pthread.h>
     ]],
     [[
     pthread_spinlock_t spinlock;
     pthread_spin_init(&spinlock, NULL);
     pthread_spin_lock (&spinlock);
     pthread_spin_unlock (&spinlock);
     pthread_spin_destroy(&spinlock);
     ]]
    ])
])dnl SCOREP_PTHREAD_SPINLOCK

# --------------------------------------------------------------------

# SCOREP_PTHREAD_MUTEX
# --------------------
# Check for Pthread mutexes, provides:
#   SCOREP_HAVE_PTHREAD_MUTEX substitution
#   SCOREP_PTHREAD_MUTEX_CPPFLAGS substitution
#   HAVE_PTHREAD_MUTEX automake conditional
AC_DEFUN([SCOREP_PTHREAD_MUTEX], [
_SCOREP_PTHREAD_FEATURE([mutex],
    ["" -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=199506L],
    [
     [[
     #include <pthread.h>
     ]],
     [[
     pthread_mutex_t mutex;
     pthread_mutex_init(&mutex, NULL);
     pthread_mutex_lock (&mutex);
     pthread_mutex_unlock (&mutex);
     pthread_mutex_destroy(&mutex);
     ]]
    ])
])dnl SCOREP_PTHREAD_MUTEX

# --------------------------------------------------------------------

# SCOREP_PTHREAD
# --------------
# Checks Pthread recording requirements
AC_DEFUN([SCOREP_PTHREAD], [
AC_REQUIRE([SCOREP_CHECK_PTHREAD])dnl
AC_REQUIRE([SCOREP_LIBRARY_WRAPPING])dnl
AC_REQUIRE([SCOREP_CHECK_THREAD_LOCAL_STORAGE])dnl

scorep_pthread_support=${scorep_have_pthread}
scorep_pthread_summary_reason=

AS_IF([test "x${scorep_pthread_support}" = x1],
    [AM_COND_IF([HAVE_LIBWRAP_LINKTIME_SUPPORT],
        [],
        [scorep_pthread_support=0
         scorep_pthread_summary_reason+=", missing linktime library wrapping support"])],
    [scorep_pthread_support=0
     scorep_pthread_summary_reason+=", missing pthread header or library"])

# check result of TLS
AS_IF([test "x${scorep_pthread_support}" = x1],
    [AM_COND_IF([HAVE_THREAD_LOCAL_STORAGE],
        [],
        [scorep_pthread_support=0
         scorep_pthread_summary_reason+=", missing TLS support"])])

AC_SUBST([SCOREP_HAVE_PTHREAD_SUPPORT], [${scorep_pthread_support}])
AFS_AM_CONDITIONAL([HAVE_PTHREAD_SUPPORT], [test "x${scorep_pthread_support}" = x1], [false])
AFS_AM_CONDITIONAL([HAVE_PTHREAD_WITHOUT_FLAGS],
    [test "x${PTHREAD_CFLAGS}" = x && test "x${PTHREAD_LIBS}" = x], [false])
AM_COND_IF([HAVE_PTHREAD_SUPPORT],
   [AFS_SUMMARY([Pthread support], [yes, using ${PTHREAD_CC} ${PTHREAD_CFLAGS} ${PTHREAD_LIBS}])],
   [AFS_SUMMARY([Pthread support], [no${scorep_pthread_summary_reason}])])
AS_UNSET([scorep_pthread_summary_reason])
])dnl SCOREP_PTHREAD
