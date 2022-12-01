dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2012,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2012, 2014, 2018-2019, 2022,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2013, 2020-2022,
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
AFS_PROG_CC([99])
])



AC_DEFUN([SCOREP_COMPUTENODE_CXX],[
AFS_PROG_CXX([11], [noext]) dnl in cross mode, needed just for tests; in non-cross
                            dnl mode also for frontend tools
])


#  Checks whether the F77 compiler actually is functional, OpenMPI can
#  install empty shells for the compiler wrappers
dnl Unfortunatly we cannot use AC_DEFUN here, i.e.,
dnl SCOREP_COMPUTENODE_F77 cannot be AC_REQUIREd. The Fortran macros
dnl come with complex dependencies, AC_DEFUN leads to configure failures
dnl if F77 is defunct. This might be fixable by patching autoconf.
m4_define([SCOREP_COMPUTENODE_F77],[
    AFS_PROG_F77([optional])
    AM_CONDITIONAL([SCOREP_HAVE_F77], [test "x${afs_cv_prog_f77_works}" = "xyes"])
    dnl do not use AS_IF here, as this epands AC_F77_LIBRARY_LDFLAGS before AS_IF,
    dnl which renders the if ineffective
    if test "x${afs_cv_prog_f77_works}" = "xyes"; then
        # AC_F*_LIBRARY_LDFLAGS should not be needed as we link the
        # libscorep_* libs with the fortran compiler. Users of libscorep_* use
        # the appropriate compiler anyhow.  Well , these macros are implicitly
        # called by AC_F*_WRAPPERS. On Cray calls to AC_F*_WRAPPERS produce
        # linker errors that can be fixed by removing "-ltcmalloc_minimal"
        # from FLIBS and FCLIBS BEFORE calling AC_F*_WRAPPERS macros.
        # Note that that the Fortran option -h tcmalloc was deprecated in CCE-15.
        AC_F77_LIBRARY_LDFLAGS
        AS_CASE([${ac_scorep_platform}],
            [crayx*], [FLIBS=`echo ${FLIBS} | sed -e 's/-ltcmalloc_minimal //g' -e 's/-ltcmalloc_minimal$//g'`])
        AC_F77_WRAPPERS
    else
        AC_DEFINE([F77_FUNC(name,NAME)], [name])
        AC_DEFINE([F77_FUNC_(name,NAME)], [name])
    fi
])

#  Checks whether the FC compiler actually is functional, OpenMPI can
#  install empty shells for the compiler wrappers
dnl Unfortunatly we cannot use AC_DEFUN here, i.e.,
dnl SCOREP_COMPUTENODE_FC cannot be AC_REQUIREd. The Fortran macros
dnl come with complex dependencies, AC_DEFUN leads to configure failures
dnl if FC is defunct. This might be fixable by patching autoconf.
m4_define([SCOREP_COMPUTENODE_FC],[
    AFS_PROG_FC([optional])
    AM_CONDITIONAL([SCOREP_HAVE_FC], [test "x${afs_cv_prog_fc_works}" = "xyes"])
    AC_SUBST([SCOREP_HAVE_FC], $(if test "x${afs_cv_prog_fc_works}" = "xyes"; then echo 1; else echo 0; fi))
    dnl do not use AS_IF here, as this epands AC_F77_LIBRARY_LDFLAGS before AS_IF,
    dnl which renders the if ineffective
    if test "x${afs_cv_prog_fc_works}" = "xyes"; then
        AC_FC_LIBRARY_LDFLAGS
        AS_CASE([${ac_scorep_platform}],
            [crayx*], [FCLIBS=`echo ${FCLIBS} | sed -e 's/-ltcmalloc_minimal //g' -e 's/-ltcmalloc_minimal$//g'`])
        AC_FC_WRAPPERS
    else
        AC_DEFINE([FC_FUNC(name,NAME)], [name])
        AC_DEFINE([FC_FUNC_(name,NAME)], [name])
    fi
])
