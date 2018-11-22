dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2018
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file scorep_posix_functions.m4

# SCOREP_POSIX_READLINK
# ---------------------
# Check for posix readlink
# Defines HAVE_POSIX_READLINK.
AC_DEFUN([SCOREP_POSIX_READLINK], [
AC_LANG_PUSH([C])
AC_MSG_CHECKING([for posix readlink])
AC_LINK_IFELSE(
    [AC_LANG_SOURCE([[
#include <unistd.h>
#include <stddef.h>
int main()
{
     size_t bufsiz = 256;
     char buf[bufsiz];
     ssize_t r = readlink("/proc/self/exe", buf, bufsiz);
     return 0;
}
        ]])],
    [AC_MSG_RESULT(yes);
     AC_DEFINE(HAVE_POSIX_READLINK, 1, [Can link posix readlink function])],
    [AC_MSG_RESULT(no)
    ])
AC_LANG_POP([C])
]) # SCOREP_POSIX_READLINK


# SCOREP_POSIX_ACCESS
# -------------------
# Check for posix access.
# Defines HAVE_POSIX_ACCESS. The source shows the intended
# usage (error handling disabled).
AC_DEFUN([SCOREP_POSIX_ACCESS], [
AC_LANG_PUSH([C])
AC_MSG_CHECKING([for posix access])
AC_LINK_IFELSE(
    [AC_LANG_SOURCE([[
#include <unistd.h>
int main()
{
    if ( access( "/proc/self/exe", X_OK ) != -1 )
    {
        int file_is_executable = 1;
    }
    return 0;
}
        ]])],
    [AC_MSG_RESULT(yes);
     AC_DEFINE(HAVE_POSIX_ACCESS, 1, [Can link posix access function])],
    [AC_MSG_RESULT(no)
    ])
AC_LANG_POP([C])
]) # SCOREP_POSIX_ACCESS
