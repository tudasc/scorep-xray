dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2012,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2012, 2014, 2018-2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2013, 2020-2021,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2012, 2014,
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl Technische Universitaet Muenchen, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

AC_DEFUN([SCOREP_COMPUTENODE_CC],[
AC_PROG_CC dnl required
AC_SCOREP_PROG_CC_C99([], [AC_MSG_ERROR([No ISO C99 support in C compiler.])])
AFS_SUMMARY([C99 compiler used], [$CC])
AM_PROG_CC_C_O

AC_LANG_PUSH([C])
AX_COMPILER_VENDOR
AX_COMPILER_VERSION
AC_LANG_POP([C])
])



AC_DEFUN([SCOREP_COMPUTENODE_CXX],[
AC_REQUIRE([AC_PROG_CXX]) dnl in cross mode, needed just for tests; in non-cross
                          dnl mode also for frontend tools

AC_LANG_PUSH([C++])
AX_COMPILER_VENDOR
AX_COMPILER_VERSION
AC_LANG_POP([C++])
])


#  Checks whether the F77 compiler actually is functional, OpenMPI can
#  install empty shells for the compiler wrappers
dnl Unfortunatly we cannot use AC_DEFUN here, i.e.,
dnl SCOREP_COMPUTENODE_F77 cannot be AC_REQUIREd. The Fortran macros
dnl come with complex dependencies, AC_DEFUN leads to configure failures
dnl if F77 is defunct. This might be fixable by patching autoconf.
m4_define([SCOREP_COMPUTENODE_F77],[
    SCOREP_COMPUTENODE_F77_WORKS
    dnl do not use AS_IF here, as this epands AC_F77_LIBRARY_LDFLAGS before AS_IF,
    dnl which renders the if ineffective
    if test "x${scorep_cv_f77_works}" = "xyes"; then
        AC_PROG_F77_C_O
        # AC_F*_LIBRARY_LDFLAGS should not be needed as we link the
        # libscorep_* libs with the fortran compiler. Users of libscorep_* use
        # the appropriate compiler anyhow.  Well , these macros are implicitly
        # called by AC_F*_WRAPPERS. On Cray calls to AC_F*_WRAPPERS produce
        # linker errors that can be fixed by removing "-ltcmalloc_minimal"
        # from FLIBS and FCLIBS BEFORE calling AC_F*_WRAPPERS macros.
        AC_F77_LIBRARY_LDFLAGS
        AS_CASE([${ac_scorep_platform}],
            [crayx*], [FLIBS=`echo ${FLIBS} | sed -e 's/-ltcmalloc_minimal //g' -e 's/-ltcmalloc_minimal$//g'`])
        AC_F77_WRAPPERS
    else
        AC_DEFINE([F77_FUNC(name,NAME)], [name])
        AC_DEFINE([F77_FUNC_(name,NAME)], [name])
    fi
])

AC_DEFUN([SCOREP_COMPUTENODE_F77_WORKS], [
    AC_REQUIRE([AC_PROG_F77]) dnl needed for linking if we build mpi fortran wrappers, also for tests
    AC_CACHE_CHECK([whether the Fortran 77 compiler works],
               [scorep_cv_f77_works],
               [AC_LANG_PUSH([Fortran 77])
                AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
                               [scorep_cv_f77_works=yes],
                               [scorep_cv_f77_works=no
                                F77=no])
                AC_LANG_POP([Fortran 77])])
    AM_CONDITIONAL([SCOREP_HAVE_F77], [test "x${scorep_cv_f77_works}" = "xyes"])
])

#  Checks whether the FC compiler actually is functional, OpenMPI can
#  install empty shells for the compiler wrappers
dnl Unfortunatly we cannot use AC_DEFUN here, i.e.,
dnl SCOREP_COMPUTENODE_FC cannot be AC_REQUIREd. The Fortran macros
dnl come with complex dependencies, AC_DEFUN leads to configure failures
dnl if FC is defunct. This might be fixable by patching autoconf.
m4_define([SCOREP_COMPUTENODE_FC],[
    SCOREP_COMPUTENODE_FC_WORKS
    afs_compiler_fc_cray=0
    dnl do not use AS_IF here, as this epands AC_F77_LIBRARY_LDFLAGS before AS_IF,
    dnl which renders the if ineffective
    if test "x${scorep_cv_fc_works}" = "xyes"; then
        AC_LANG_PUSH([Fortran])
        AC_FC_PP_SRCEXT([F])
        AX_COMPILER_VENDOR
        AC_LANG_POP([Fortran])
        AC_PROG_FC_C_O
        AC_FC_LIBRARY_LDFLAGS
        AS_CASE([${ac_scorep_platform}],
            [crayx*], [FCLIBS=`echo ${FCLIBS} | sed -e 's/-ltcmalloc_minimal //g' -e 's/-ltcmalloc_minimal$//g'`])
        AC_FC_WRAPPERS
        # Provide SCOREP_COMPILER_FC_CRAY to resurrect PrgEnv-cray. This temporary solution will be replaced with #16.
        AS_IF([test "x$ax_cv_fc_compiler_vendor" = xcray],
            [afs_compiler_fc_cray=1])
    else
        AC_DEFINE([FC_FUNC(name,NAME)], [name])
        AC_DEFINE([FC_FUNC_(name,NAME)], [name])
    fi
    AC_SUBST([SCOREP_COMPILER_FC_CRAY], [${afs_compiler_fc_cray}])
])

AC_DEFUN([SCOREP_COMPUTENODE_FC_WORKS], [
    AC_REQUIRE([AC_PROG_FC]) dnl needed for tests only
    AC_CACHE_CHECK([whether the Fortran compiler works (FC)],
               [scorep_cv_fc_works],
               [AC_LANG_PUSH([Fortran])
                AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
                               [scorep_cv_fc_works=yes],
                               [scorep_cv_fc_works=no
                                FC=no])
                AC_LANG_POP([Fortran])])
    AM_CONDITIONAL([SCOREP_HAVE_FC], [test "x${scorep_cv_fc_works}" = "xyes"])
])
