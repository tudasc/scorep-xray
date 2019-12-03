## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2018,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

# AFS_LINKMODE
# ------------
# Determines additional platform-specific libtool link flags, i.e.:
#
#   - On Cray, force static linking of compute-node executables if the
#     environment variable CRAYPE_LINK_TYPE is set to 'static' or unset
#     (default: static).
#
#   - On IBM Blue Gene/Q and Fujitsu K/FX10/FX100, force static linking
#     of compute-node executables.
#
# This macro has to be called after LT_INIT and requires that
# `ac_scorep_platform` is set appropriately.  For each compute-node
# executable `foo`, use
#
#   foo_LDFLAGS = $(AM_LDFLAGS) $(LINKMODE_FLAGS)
#
# and add
#
#   LINKMODE_FLAGS = @AFS_LINKMODE_LDFLAGS@
#
# to build-{backend,mpi,shmem}/Makefile.am for the link flags to take
# effect.
#
#
# List of configure variables set:
#   `afs_linkmode_LDFLAGS`::  Additional link flags
#
# List of provided automake substitutions:
#   `AFS_LINKMODE_LDFLAGS`::  Additional link flags
#
AC_DEFUN([AFS_LINKMODE], [
AC_MSG_CHECKING([for platform-specific libtool link flags])
AS_ECHO(["library link mode: static=$enable_static, shared=$enable_shared"]) >&AS_MESSAGE_LOG_FD
AS_CASE([$ac_scorep_platform],
    [crayx*],
        [AS_ECHO(["executable link mode: ${CRAYPE_LINK_TYPE:-static} (CRAYPE_LINK_TYPE)"]) >&AS_MESSAGE_LOG_FD
         AS_IF([test "x$CRAYPE_LINK_TYPE" = x || test "x$CRAYPE_LINK_TYPE" = xstatic],
            [AS_IF([test "x$enable_shared" = xyes && test "x$enable_static" = xno],
                [AC_MSG_FAILURE([static linking with only shared libraries not possible])])
             afs_linkmode_LDFLAGS="-all-static"])],
    [bgq|k|fx10*],
        [AS_ECHO(["executable link mode: static"]) >&AS_MESSAGE_LOG_FD
         AS_IF([test "x$enable_shared" = xyes && test "x$enable_static" = xno],
            [AC_MSG_FAILURE([static linking with only shared libraries not possible])])
         afs_linkmode_LDFLAGS="-all-static"],
    [AS_ECHO(["executable link mode: default"]) >&AS_MESSAGE_LOG_FD])
AC_MSG_RESULT([${afs_linkmode_LDFLAGS:-none}])
AC_SUBST([AFS_LINKMODE_LDFLAGS], [$afs_linkmode_LDFLAGS])
])dnl
