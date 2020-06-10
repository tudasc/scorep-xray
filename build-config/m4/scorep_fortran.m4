## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2012, 2014, 2018-2019,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013, 2020,
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

## file build-config/m4/scorep_fortran.m4

dnl ------------------------------------------------------------------

#  Checks whether the F77 compiler actually is functional, OpenMPI can install empty shells for the compiler wrappers
dnl Do not use AC_DEFUN to prevent AC_F77_LIBRARY_LDFLAGS was expanded before it was required warnings
m4_define([SCOREP_FORTRAN_F77],[
    SCOREP_FORTRAN_F77_WORKS
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

AC_DEFUN([SCOREP_FORTRAN_F77_WORKS], [
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

#  Checks whether the FC compiler actually is functional, OpenMPI can install empty shells for the compiler wrappers
dnl Do not use AC_DEFUN to prevent AC_FC_LIBRARY_LDFLAGS was expanded before it was required warnings
m4_define([SCOREP_FORTRAN_FC],[
    SCOREP_FORTRAN_FC_WORKS
    dnl do not use AS_IF here, as this epands AC_F77_LIBRARY_LDFLAGS before AS_IF,
    dnl which renders the if ineffective
    if test "x${scorep_cv_fc_works}" = "xyes"; then
        AC_PROG_FC_C_O
        AC_FC_LIBRARY_LDFLAGS
        AS_CASE([${ac_scorep_platform}],
            [crayx*], [FCLIBS=`echo ${FCLIBS} | sed -e 's/-ltcmalloc_minimal //g' -e 's/-ltcmalloc_minimal$//g'`])
        AC_FC_WRAPPERS
    else
        AC_DEFINE([FC_FUNC(name,NAME)], [name])
        AC_DEFINE([FC_FUNC_(name,NAME)], [name])
    fi
])

AC_DEFUN([SCOREP_FORTRAN_FC_WORKS], [
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
