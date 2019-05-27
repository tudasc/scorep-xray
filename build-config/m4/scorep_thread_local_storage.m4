## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2014-2015, 2017, 2019,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

# SCOREP_CHECK_THREAD_LOCAL_STORAGE
# -----------------
# Checks whether the compiler supports thread-local storage.
# Sets scorep_have_thread_local_storage to yes if the compiler does,
# otherwise scorep_have_thread_local_storage is set to no.
# Sets scorep_thread_local_storage_specifier to the supported specifier.
# Suitable for AC_REQUIRE.
AC_DEFUN([SCOREP_CHECK_THREAD_LOCAL_STORAGE], [

scorep_have_thread_local_storage="no"
scorep_thread_local_storage_specifier=""

dnl There is a bug in gcc < 4.1.2 involving TLS and -fPIC on x86:
dnl http://gcc.gnu.org/ml/gcc-bugs/2006-09/msg02275.html
dnl
dnl And mingw also does compile __thread but resultant code actually
dnl fails to work correctly at least in some not so ancient version:
dnl http://mingw-users.1079350.n2.nabble.com/gcc-4-4-multi-threaded-exception-handling-amp-thread-specifier-not-working-td3440749.html
dnl
dnl Also it was reported that earlier gcc versions for mips compile
dnl __thread but it does not really work
_SCOREP_CHECK_TLS_SPECIFIER([__thread], [[
#if defined(__GNUC__) && ((__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 1) || (__GNUC__ == 4 && __GNUC_MINOR__ == 1 && __GNUC_PATCHLEVEL__ < 2))
#ups_unsupported_gcc_version gcc has this bug: http://gcc.gnu.org/ml/gcc-bugs/2006-09/msg02275.html
#elif defined(__MINGW32__)
#ups_unsupported_mingw_version mingw doesnt really support thread local storage
#endif
]], [
    scorep_have_thread_local_storage="yes"
    scorep_thread_local_storage_specifier="__thread"
])

AS_IF([test "x${scorep_have_thread_local_storage}" = "xno"], [
    _SCOREP_CHECK_TLS_SPECIFIER([_Thread_local], [], [
        scorep_have_thread_local_storage="yes"
        scorep_thread_local_storage_specifier="_Thread_local"
    ])
])

AS_IF([test "x${scorep_have_thread_local_storage}" = "xno"], [
    scorep_tls_cflags="-c11"
    _SCOREP_CHECK_TLS_SPECIFIER([_Thread_local], [], [
        scorep_have_thread_local_storage="yes"
        scorep_thread_local_storage_specifier="_Thread_local"
        CC="$CC $scorep_tls_cflags"
    ], [],
    [$scorep_tls_cflags])
    AS_UNSET([scorep_tls_cflags])
])

AC_SCOREP_COND_HAVE([THREAD_LOCAL_STORAGE],
                    [test "x${scorep_have_thread_local_storage}" = "xyes"],
                    [Defined if thread local storage support is available.],
                    [AC_DEFINE_UNQUOTED([SCOREP_THREAD_LOCAL_STORAGE_SPECIFIER],
                                        [$scorep_thread_local_storage_specifier],
                                        [Set specifier to mark a variable as thread-local storage (TLS)])])
])

# SCOREP_THREAD_LOCAL_STORAGE
AC_DEFUN([SCOREP_THREAD_LOCAL_STORAGE], [
AC_REQUIRE([SCOREP_CHECK_THREAD_LOCAL_STORAGE])dnl

scorep_tls_reason=""
scorep_tls_model="/* not supported tls_model_arg */"
AM_COND_IF([HAVE_THREAD_LOCAL_STORAGE], [
    AS_VAR_APPEND([scorep_tls_reason], [", using $scorep_thread_local_storage_specifier"])
    AS_IF([test "x${enable_shared}" = "xyes"], [
        _SCOREP_CHECK_TLS_MODEL([initial-exec], [
            scorep_tls_model="__attribute__(( tls_model( tls_model_arg ) ))"
            AS_VAR_APPEND([scorep_tls_reason], [" and the initial-exec model"])
        ], [
            AS_VAR_APPEND([scorep_tls_reason], [" and the default model"])
        ])
    ])
])
AC_DEFINE_UNQUOTED([SCOREP_THREAD_LOCAL_STORAGE_MODEL( tls_model_arg )],
                   [$scorep_tls_model],
                   [Variable attribute to select a specific TLS model.])
AS_UNSET([scorep_tls_model])

AFS_SUMMARY([TLS support], [${scorep_have_thread_local_storage}${scorep_tls_reason}])
AS_UNSET([scorep_tls_reason])
])


# _SCOREP_CHECK_TLS_SPECIFIER( TLS_SPECIFIER,
#                              PROLOG,
#                              ACTION_FOUND,
#                              ACTION_NOT_FOUND,
#                              [ADDITIONAL_CFLAGS] )
# --------------------------------------------------
# Performs checks whether the compiler supports TLS_SPECIFIER.
AC_DEFUN([_SCOREP_CHECK_TLS_SPECIFIER], [
AC_REQUIRE([LT_OUTPUT])

AC_LANG_PUSH([C])

cflags_save="$CFLAGS"
CFLAGS="$CFLAGS $5"

dnl check the static case (i.e., linking all into the executable)
AC_MSG_CHECKING([for $1 (static)])
scorep_check_tls_result=yes
AC_LINK_IFELSE([AC_LANG_PROGRAM([
$2
$1 volatile int global_thread_private_var = 1;
], [
    static $1 volatile int my_thread_private_var = 0;
    global_thread_private_var++;
    --my_thread_private_var;
])],
   [AC_MSG_RESULT([yes])],
   [AC_MSG_RESULT([no])
    scorep_check_tls_result=no])

dnl check the dynamic library case (i.e,. tls is accessed from dynamic lib)
AS_IF([test "x${scorep_check_tls_result}" = "xyes" && test "x${enable_shared}" = "xyes"], [

AC_LANG_CONFTEST([
    AC_LANG_SOURCE([[
extern $1 volatile int confvar;

void conffunc(void)
{
    confvar++;
    --confvar;
}
]])])

tls_check_compile='$SHELL ./libtool --mode=compile --tag=_AC_CC [$]_AC_CC $CPPFLAGS [$]_AC_LANG_PREFIX[FLAGS] -c -o conftest.lo conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
tls_check_link='$SHELL ./libtool --mode=link --tag=_AC_CC [$]_AC_CC [$]_AC_LANG_PREFIX[FLAGS] -rpath $PWD/lib -o libconftest.la conftest.lo >&AS_MESSAGE_LOG_FD'
tls_check_clean='$SHELL ./libtool --mode=clean $RM conftest.lo libconftest.la >&AS_MESSAGE_LOG_FD'

AC_MSG_CHECKING([for $1 (shared)])
AS_IF([_AC_DO_VAR([tls_check_compile]) &&
       _AC_DO_VAR([tls_check_link])],
   [AC_MSG_RESULT([yes])],
   [_AC_MSG_LOG_CONFTEST
    AC_MSG_RESULT([no])
    scorep_check_tls_result=no])

_AC_DO_VAR([tls_check_clean])
$RM conftest.$ac_ext

AS_UNSET([tls_check_compile])
AS_UNSET([tls_check_link])
AS_UNSET([tls_check_compile])

])

CFLAGS="$cflags_save"

AS_IF([test "x${scorep_check_tls_result}" = "xyes"],
   [$3
    :],
   [$4
    :])

AC_LANG_POP([C])

])


# _SCOREP_CHECK_TLS_MODEL( TLS_MODEL,
#                          ACTION_FOUND,
#                          ACTION_NOT_FOUND )
# -----------------------------------
# Performs checks whether the compiler supports '__attribute__(( tls_model( "TLS_MODEL" ) ))'.
AC_DEFUN([_SCOREP_CHECK_TLS_MODEL], [
AC_REQUIRE([LT_OUTPUT])

AC_LANG_PUSH([C])

AC_LANG_CONFTEST([
    AC_LANG_SOURCE([[
extern SCOREP_THREAD_LOCAL_STORAGE_SPECIFIER volatile int confvar __attribute__(( tls_model( "$1" ) ));

void conffunc(void)
{
    confvar++;
    --confvar;
}
]])])

tls_model_compile='$SHELL ./libtool --mode=compile --tag=_AC_CC [$]_AC_CC $CPPFLAGS [$]_AC_LANG_PREFIX[FLAGS] -c -o conftest.lo conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
tls_model_link='$SHELL ./libtool --mode=link --tag=_AC_CC [$]_AC_CC [$]_AC_LANG_PREFIX[FLAGS] -rpath $PWD/lib -o libconftest.la conftest.lo >&AS_MESSAGE_LOG_FD'
tls_model_clean='$SHELL ./libtool --mode=clean $RM conftest.lo libconftest.la >&AS_MESSAGE_LOG_FD'

AC_MSG_CHECKING([for tls_model("$1")])
AS_IF([_AC_DO_VAR([tls_model_compile]) &&
       _AC_DO_VAR([tls_model_link])],
   [AC_MSG_RESULT([yes])
    $2
    :],
   [_AC_MSG_LOG_CONFTEST
    AC_MSG_RESULT([no])
    $3
    :])

_AC_DO_VAR([tls_model_clean])
$RM conftest.$ac_ext

AS_UNSET([tls_model_compile])
AS_UNSET([tls_model_link])
AS_UNSET([tls_model_compile])

AC_LANG_POP([C])

])
