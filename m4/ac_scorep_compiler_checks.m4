## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2012, 2019, 2021,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013, 2021-2022,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012, 2014,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

AC_DEFUN([SCOREP_COMPILER_COND_AND_SUBST],[
dnl FIX REQUIRE: Needs AFS_PROG_CC

# The SCOREP_COMPILER_* automake conditionals are exclusively used by
# scorep. Thus, define only the ones that are used.
# Later on, move from common to scorep.
AS_CASE([${ax_cv_c_compiler_vendor%/*}],
    [intel],    [],
    [ibm],      [],
    [nvhpc],    [AFS_AM_CONDITIONAL([SCOREP_COMPILER_CC_PGI],     [test 1 -eq 1], [false])],
    [portland], [AFS_AM_CONDITIONAL([SCOREP_COMPILER_CC_PGI],     [test 1 -eq 1], [false])],
    [gnu],      [AFS_AM_CONDITIONAL([SCOREP_COMPILER_CC_GNU],     [test 1 -eq 1], [false])],
    [clang],    [AFS_AM_CONDITIONAL([SCOREP_COMPILER_CC_CLANG],   [test 1 -eq 1], [false])],
    [cray],     [],
    [fujitsu],  [],
    [unknown],  [AC_MSG_WARN([Could not determine C compiler vendor. AC_PACKAGE_NAME might not function properly.])],
    [AC_MSG_WARN([C compiler vendor '${ax_cv_c_compiler_vendor}' unsupported. AC_PACKAGE_NAME might not function properly.])])dnl

AS_IF([test x$afs_cv_prog_cxx_works = xyes], [
    AS_CASE([${ax_cv_cxx_compiler_vendor%/*}],
        [intel],    [],
        [ibm],      [],
        [nvhpc],    [],
        [portland], [],
        [gnu],      [],
        [clang],    [],
        [cray],     [],
        [fujitsu],  [],
        [unknown],  [AC_MSG_WARN([Could not determine C++ compiler vendor. AC_PACKAGE_NAME might not function properly.])],
        [AC_MSG_WARN([C++ compiler vendor '${ax_cv_cxx_compiler_vendor}' unsupported. AC_PACKAGE_NAME might not function properly.])])])

AS_IF([test x$afs_cv_prog_fc_works = xyes], [
    AS_CASE([${ax_cv_fc_compiler_vendor%/*}],
        [intel],       [],
        [ibm],         [AFS_AM_CONDITIONAL([SCOREP_COMPILER_FC_IBM],     [test 1 -eq 1], [false])],
        [nvhpc],       [AFS_AM_CONDITIONAL([SCOREP_COMPILER_FC_PGI],     [test 1 -eq 1], [false])],
        [portland],    [AFS_AM_CONDITIONAL([SCOREP_COMPILER_FC_PGI],     [test 1 -eq 1], [false])],
        [gnu],         [AFS_AM_CONDITIONAL([SCOREP_COMPILER_FC_GNU],     [test 1 -eq 1], [false])],
        [clang|flang], [],
        [cray],        [AFS_AM_CONDITIONAL([SCOREP_COMPILER_FC_CRAY],    [test 1 -eq 1], [false])],
        [fujitsu],     [AFS_AM_CONDITIONAL([SCOREP_COMPILER_FC_FUJITSU], [test 1 -eq 1], [false])],
        [unknown],     [AC_MSG_WARN([Could not determine Fortran compiler vendor. AC_PACKAGE_NAME might not function properly.])],
        [AC_MSG_WARN([Fortran compiler vendor '${ax_cv_fc_compiler_vendor}' unsupported. AC_PACKAGE_NAME might not function properly.])])])

cc_compiler="${ax_cv_c_compiler_vendor%/*}"
AC_SUBST([SCOREP_COMPILER_CC_INTEL],   $(if test "x${cc_compiler}" = xintel; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CC_IBM],     $(if test "x${cc_compiler}" = xibm; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CC_PGI],     $(if test "x${cc_compiler}" = xnvhpc || test "x${cc_compiler}" = xpgi; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CC_GNU],     $(if test "x${cc_compiler}" = xgnu; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CC_CLANG],   $(if test "x${cc_compiler}" = xclang; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CC_CRAY],    $(if test "x${cc_compiler}" = xcray; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CC_FUJITSU], $(if test "x${cc_compiler}" = xfujitsu; then echo 1; else echo 0; fi))

cxx_compiler="${ax_cv_cxx_compiler_vendor%/*}"
AC_SUBST([SCOREP_COMPILER_CXX_INTEL],   $(if test "x${cxx_compiler}" = xintel; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CXX_IBM],     $(if test "x${cxx_compiler}" = xibm; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CXX_PGI],     $(if test "x${cxx_compiler}" = xnvhpc || test "x${cxx_compiler}" = xpgi; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CXX_GNU],     $(if test "x${cxx_compiler}" = xgnu; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CXX_CLANG],   $(if test "x${cxx_compiler}" = xclang; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CXX_CRAY],    $(if test "x${cxx_compiler}" = xcray; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_CXX_FUJITSU], $(if test "x${cxx_compiler}" = xfujitsu; then echo 1; else echo 0; fi))

fc_compiler="${ax_cv_fc_compiler_vendor%/*}"
AC_SUBST([SCOREP_COMPILER_FC_INTEL],   $(if test "x${fc_compiler}" = xintel; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_FC_IBM],     $(if test "x${fc_compiler}" = xibm; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_FC_PGI],     $(if test "x${fc_compiler}" = xnvhpc || test "x${fc_compiler}" = xpgi; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_FC_GNU],     $(if test "x${fc_compiler}" = xgnu; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_FC_CLANG],   $(if test "x${fc_compiler}" = xclang || test "x${fc_compiler}" = xflang; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_FC_CRAY],    $(if test "x${fc_compiler}" = xcray; then echo 1; else echo 0; fi))
AC_SUBST([SCOREP_COMPILER_FC_FUJITSU], $(if test "x${fc_compiler}" = xfujitsu; then echo 1; else echo 0; fi))

# Keep -mmic support for a while. Assumption: for HAVE( PLATFORM_MIC )
# || HAVE( MIC_SUPPORT ), if intel is used, then it is used for all
# languages. If this is not the case, the instrumenter will not provide
# MIC support. As # MIC will be removed in scorep-9, we are not going to
# make this rock-solid.
AC_SUBST([SCOREP_COMPILER_MIC], $(if test "x${cc_compiler}" = xintel && test "x${cxx_compiler}" = xintel && test "x${fc_compiler}" = xintel; then echo 1; else echo 0; fi))

dnl strip epoch (Borland only)
_scorep_compiler_version=${ax_cv_c_compiler_version##*:}
dnl extract major
afs_compiler_cc_version_major=${_scorep_compiler_version%%.*}
dnl fallback to 0
: ${afs_compiler_version_major:=0}
_scorep_compiler_version=${_scorep_compiler_version#*.}
dnl extract minor
afs_compiler_cc_version_minor=${_scorep_compiler_version%%.*}
dnl fallback to 0
: ${afs_compiler_version_minor:=0}
AS_UNSET([_scorep_compiler_version])
])dnl
