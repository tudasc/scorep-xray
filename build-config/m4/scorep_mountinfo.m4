## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013, 2015,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2015,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##
##

AC_DEFUN([SCOREP_MNTENT_CHECK], [

AC_CHECK_HEADER([mntent.h],
                [ac_scorep_mntent_header="yes"],
                [ac_scorep_mntent_header="no"])

AC_MSG_CHECKING([for setmntent])
AC_LINK_IFELSE([AC_LANG_CALL([], [setmntent])],
                [ac_scorep_mntent_have_set="yes"],
                [ac_scorep_mntent_have_set="no"])
AC_MSG_RESULT([${ac_scorep_mntent_have_set}])

AC_MSG_CHECKING([for getmntent])
AC_LINK_IFELSE([AC_LANG_CALL([], [getmntent])],
                [ac_scorep_mntent_have_get="yes"],
                [ac_scorep_mntent_have_get="no"])
AC_MSG_RESULT([${ac_scorep_mntent_have_get}])

AC_MSG_CHECKING([for endmntent])
AC_LINK_IFELSE([AC_LANG_CALL([], [endmntent])],
                [ac_scorep_mntent_have_end="yes"],
                [ac_scorep_mntent_have_end="no"])
AC_MSG_RESULT([${ac_scorep_mntent_have_end}])

AS_IF([test "x${ac_scorep_mntent_header}"   = "xyes" && \
       test "x${ac_scorep_mntent_have_set}" = "xyes" && \
       test "x${ac_scorep_mntent_have_get}" = "xyes" && \
       test "x${ac_scorep_mntent_have_end}" = "xyes"],
      [ac_scorep_have_mount_info="yes"],
      [])

AM_CONDITIONAL([MOUNT_INFO_SUPPORT], [test "x${ac_scorep_have_mount_info}" = "xyes"])


AFS_SUMMARY([Mount point extraction], [${ac_scorep_have_mount_info}])

])
