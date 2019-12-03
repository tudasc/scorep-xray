dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2018,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file afs_maintainer_mode.m4

# AFS_MAINTAINER_MODE
# -------------------
# Enable maintainer mode only when building from repository sources.
# We want automatic regeneration of files when our build environemnt
# is available, i.e., when correct and patched versions of autotools
# are available. Otherwise we want to prevent automatic regeneration
# of files.
AC_DEFUN([AFS_MAINTAINER_MODE],
[AC_REQUIRE([AC_SCOREP_GIT_CONTROLLED])
AS_IF([test "x${ac_scorep_git_controlled}" = xyes],
    [AM_MAINTAINER_MODE([enable])],
    [AM_MAINTAINER_MODE([])])dnl
]) # AFS_MAINTAINER_MODE
