## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2023,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


# AFS_COMPILE_IFELSE_WERROR(input, [action-if-true], [action-if-false])
# -------------------------------------------------------------------------------
# Convenient wrapper of AC_COMPILE_IFELSE macro which sets ac_[]_AC_LANG_ABBREV[]_werror_flag
# so that the compilation fails if there is any output on stderr.
#
# The remaining behavior matches the one found in AC_COMPILE_IFELSE.
#
AC_DEFUN([AFS_COMPILE_IFELSE_WERROR],[
    # Any non-null value works here
    afs_compile_ifelse_werror_[]_AC_LANG_ABBREV[]_werror_flag_save=${ac_[]_AC_LANG_ABBREV[]_werror_flag}
    ac_[]_AC_LANG_ABBREV[]_werror_flag=yes

    # Now call AC_COMPILE_IFELSE like one would normally do
    AC_COMPILE_IFELSE([$1], [$2], [$3])

    ac_[]_AC_LANG_ABBREV[]_werror_flag=${afs_compile_ifelse_werror_[]_AC_LANG_ABBREV[]_werror_flag_save}
])
