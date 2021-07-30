## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2020,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_align.m4

dnl ----------------------------------------------------------------------------


# SCOREP_ALIGN
# ------------
# Convenience macro for alignas and cachelinesize
#
AC_DEFUN([SCOREP_ALIGN], [
_SCOREP_ALIGNAS
_SCOREP_CACHELINESIZE
])#SCOREP_ALIGN


dnl ----------------------------------------------------------------------------

# _SCOREP_ALIGNAS
# --------------
# Determine if macro alignas(x) is available. If yes, define
# HAVE_DECL_ALIGNAS and define SCOREP_ALIGNAS(x) to alignas(x),
# otherwise define it empty.
# alignas needs stdalign.h. If this ia available, define
# HAVE_STDALIGN_H to 1
#
# usage:
# #if HAVE( STDALIGN_H )
# #include <stdalign.h>
# #endif
# ...
# /* if HAVE_DECL_ALIGNAS, objects of foo will be 64-bit aligned: */
# struct foo { SCOREP_ALIGNAS(64) int a; ... };
#
AC_DEFUN([_SCOREP_ALIGNAS], [
AS_UNSET([alignas])
AC_CHECK_HEADERS([stdalign.h],
    [AC_CHECK_DECLS([alignas],
        [alignas="alignas(x)"],
        [],
        [#include <stdalign.h>])])
AC_DEFINE_UNQUOTED([SCOREP_ALIGNAS(x)], [$alignas], [Provide alignas macro])
])#_SCOREP_ALIGNAS

dnl ----------------------------------------------------------------------------

# _SCOREP_CACHELINESIZE
# ---------------------
# Provide macro SCOREP_CACHELINESIZE. It defaults to 64. We can't
# query it programatically as the architecture configure is run on
# might differ from the one an instrumented application runs on. We
# know that A64FX has a cachelinesize of 256.
#
AC_DEFUN([_SCOREP_CACHELINESIZE], [
cachelinesize=64
# Once we know how to detect fugaku/a64fx
##AS_IF(ac_scorep_platform == a64fx, cachelinesize=256)
AC_DEFINE_UNQUOTED([SCOREP_CACHELINESIZE], [$cachelinesize], [Provide cachelinesize])
])#_SCOREP_CACHELINESIZE
