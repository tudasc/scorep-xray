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
## Copyright (c) 2009-2013, 2021-2023,
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

m4_define(_SCOREP_COMPILER_SUBST, [
AS_CASE(${ax_cv_[]_AC_LANG_ABBREV[]_compiler_vendor},
    [intel],              SCOREP_COMPILER_[]_AC_CC[]_INTEL=1,
    [intel/oneapi],       SCOREP_COMPILER_[]_AC_CC[]_INTEL_ONEAPI=1,
    [ibm],                SCOREP_COMPILER_[]_AC_CC[]_IBM=1,
    [nvhpc|pgi|pgi/llvm], SCOREP_COMPILER_[]_AC_CC[]_PGI=1,
    [gnu],                SCOREP_COMPILER_[]_AC_CC[]_GNU=1,
    [clang|flang|flang/classic],
                          SCOREP_COMPILER_[]_AC_CC[]_CLANG=1,
    [cray],               SCOREP_COMPILER_[]_AC_CC[]_CRAY=1,
    [fujitsu],            SCOREP_COMPILER_[]_AC_CC[]_FUJITSU=1)

AC_SUBST(SCOREP_COMPILER_[]_AC_CC[]_INTEL,        ${SCOREP_COMPILER_[]_AC_CC[]_INTEL-0})
AC_SUBST(SCOREP_COMPILER_[]_AC_CC[]_INTEL_ONEAPI, ${SCOREP_COMPILER_[]_AC_CC[]_INTEL_ONEAPI-0})
AC_DEFINE_UNQUOTED([HAVE_SCOREP_COMPILER_[]_AC_CC[]_INTEL_ONEAPI],
    [${SCOREP_COMPILER_[]_AC_CC[]_INTEL_ONEAPI}],
    [Defined to 1 if oneAPI compiler is used])
AC_SUBST(SCOREP_COMPILER_[]_AC_CC[]_IBM,          ${SCOREP_COMPILER_[]_AC_CC[]_IBM-0})
AC_SUBST(SCOREP_COMPILER_[]_AC_CC[]_PGI,          ${SCOREP_COMPILER_[]_AC_CC[]_PGI-0})
AC_SUBST(SCOREP_COMPILER_[]_AC_CC[]_GNU,          ${SCOREP_COMPILER_[]_AC_CC[]_GNU-0})
AC_SUBST(SCOREP_COMPILER_[]_AC_CC[]_CLANG,        ${SCOREP_COMPILER_[]_AC_CC[]_CLANG-0})
AC_SUBST(SCOREP_COMPILER_[]_AC_CC[]_CRAY,         ${SCOREP_COMPILER_[]_AC_CC[]_CRAY-0})
AC_SUBST(SCOREP_COMPILER_[]_AC_CC[]_FUJITSU,      ${SCOREP_COMPILER_[]_AC_CC[]_FUJITSU-0})
])

AC_DEFUN([SCOREP_COMPILER_COND_AND_SUBST],[
dnl Cannot easily require AFS_PROG_CC|CXX|FC

# The SCOREP_COMPILER_* automake conditionals are exclusively used by
# scorep. Thus, define only the ones that are used.
AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [],
    [intel/oneapi],
                [AFS_AM_CONDITIONAL([SCOREP_COMPILER_CC_CLANG],   [test 1 -eq 1], [false])],
    [ibm],      [],
    [nvhpc],    [AFS_AM_CONDITIONAL([SCOREP_COMPILER_CC_PGI],     [test 1 -eq 1], [false])],
    [portland|portland/*],
                [AFS_AM_CONDITIONAL([SCOREP_COMPILER_CC_PGI],     [test 1 -eq 1], [false])],
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

AC_LANG_PUSH([C])
_SCOREP_COMPILER_SUBST
AC_LANG_POP([C])

AC_LANG_PUSH([C++])
_SCOREP_COMPILER_SUBST
AC_LANG_POP([C++])

AC_LANG_PUSH([Fortran])
_SCOREP_COMPILER_SUBST
AC_LANG_POP([Fortran])

# Keep -mmic support for a while. Assumption: for HAVE( PLATFORM_MIC )
# || HAVE( MIC_SUPPORT ), if intel is used, then it is used for all
# languages. If this is not the case, the instrumenter will not provide
# MIC support. As # MIC will be removed in scorep-9, we are not going to
# make this rock-solid.
AC_SUBST([SCOREP_COMPILER_MIC], $(if test "x${ax_cv_c_compiler_vendor}" = xintel && test "x${ax_cv_cxx_compiler_vendor}" = xintel && test "x${ax_cv_fc_compiler_vendor}" = xintel; then echo 1; else echo 0; fi))

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
