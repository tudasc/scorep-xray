dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013, 2022,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2017,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-includes/loginnode.m4

SCOREP_DETECT_MIC_BUILD

AC_LANG_PUSH([C++])
AFS_POSIX_OPENDIR
AFS_POSIX_READDIR
AFS_POSIX_CLOSEDIR
AS_IF([test "x${afs_cv_have_posix_opendir}" = "xyes" && \
       test "x${afs_cv_have_posix_readdir}" = "xyes" && \
       test "x${afs_cv_have_posix_closedir}" = "xyes"],
    [AC_DEFINE([HAVE_POSIX_DIRENTRY_FUNCS], [1],
        [Define to 1 if directory entries can be read via POSIX functions])])
AFS_POSIX_PCLOSE
AFS_POSIX_POPEN
AS_IF([test "x$afs_cv_have_posix_pclose" = "xyes" && \
       test "x$afs_cv_have_posix_popen" = "xyes"],
    [AC_DEFINE([HAVE_POSIX_PIPES], [1],
        [Define to 1 if POSIX pipes are supported])])
AC_LANG_POP([C++])
