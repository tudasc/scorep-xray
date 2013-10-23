## -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

AC_DEFUN([_OTF2_PTHREAD_BARRIER], [

otf2_pthread_barrier_cflags=
for flag in "" -D_XOPEN_SOURCE=600 -D_POSIX_C_SOURCE=200112L
do
    save_LIBS="$LIBS"
    LIBS="$PTHREAD_LIBS $LIBS"
    save_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS $PTHREAD_CFLAGS $flag"
    save_CC="$CC"
    CC="$PTHREAD_CC"

    AS_UNSET([ac_cv_have_decl_PTHREAD_BARRIER_SERIAL_THREAD])
    AC_CHECK_DECL([PTHREAD_BARRIER_SERIAL_THREAD],
        [otf2_pthread_barrier_serial_thread="yes"
         otf2_pthread_barrier_cflags=$flag],
        ,
        [[#include <pthread.h>]])

    LIBS="$save_LIBS"
    CFLAGS="$save_CFLAGS"
    CC="$save_CC"

    AS_IF([test "x$otf2_pthread_barrier_serial_thread" = "xyes"], [
        break
    ])
done

AS_IF([test x"$otf2_pthread_barrier_serial_thread" = xyes], [
    :
    $1
], [
    :
    $2
])

]) dnl _OTF2_PTHREAD_BARRIER

AC_DEFUN([OTF2_PTHREAD], [

otf2_pthread_support=no
otf2_pthread_barrier_support=no
AS_IF([test "x$ac_scorep_frontend" = "xyes"], [
    AX_PTHREAD([
        otf2_pthread_support=yes
        _OTF2_PTHREAD_BARRIER([otf2_pthread_barrier_support=yes],
                              [otf2_pthread_support=no])])])
AM_CONDITIONAL([HAVE_PTHREAD], [test "x$otf2_pthread_support" = "xyes"])
AM_CONDITIONAL([HAVE_PTHREAD_BARRIER], [test "x$otf2_pthread_barrier_support" = "xyes"])
AS_IF([test "x${otf2_pthread_barrier_support}" = "xyes"], [
    PTHREAD_CFLAGS="$PTHREAD_CFLAGS $otf2_pthread_barrier_cflags"
    AC_DEFINE([HAVE_PTHREAD_BARRIER], [1], [Support for pthread_barrier_t.])
])
AS_IF([test "x$ac_scorep_frontend" = "xyes"], [
    AFS_SUMMARY([Pthread support for tests], [${otf2_pthread_support}])
])

]) dnl OTF2_PTHREAD
