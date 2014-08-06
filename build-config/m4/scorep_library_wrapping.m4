## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2014,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_library_wrapping.m4


AC_DEFUN([SCOREP_LIBRARY_WRAPPING], [
AC_REQUIRE([AFS_GNU_LINKER])

dnl Don't check for prerequisite of library wrapping on the frontend.
AS_IF([test "x$ac_scorep_backend" = xno], [AC_MSG_ERROR([cannot check for library wrapping support on frontend.])])
])
