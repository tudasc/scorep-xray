## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2013, 2021,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


# AFS_AM_CONDITIONAL(NAME, SHELL-CONDITION, DEFAULT)
# --------------------------------------------------
# Delegates to AM_CONDITIONAL but allows to be used conditionally
# as DEFAULT (either true or false) needs to be provided.
#
AC_DEFUN([AFS_AM_CONDITIONAL], [dnl
m4_case([$3],
    [true], [m4_divert_text([DEFAULTS], [$1_TRUE=])dnl
             m4_divert_text([DEFAULTS], [$1_FALSE='#'])],
    [false], [m4_divert_text([DEFAULTS], [$1_TRUE='#'])dnl
              m4_divert_text([DEFAULTS], [$1_FALSE=])],
    [m4_fatal([third argument to AFS_AM_CONDITIONAL needs to be either true or false])])dnl
AM_CONDITIONAL([$1], [$2])dnl
]) #AFS_AM_CONDITIONAL
