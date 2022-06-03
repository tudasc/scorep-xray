## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2014,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##


AC_DEFUN([AFS_GNU_LINKER], [

AC_SCOREP_COND_HAVE([GNU_LINKER],
                    [test "x${with_gnu_ld}" = "xyes"],
                    [Defined if the linker is GNU ld.],
                    [afs_have_gnu_linker="yes"],
                    [afs_have_gnu_linker="no"])

])
