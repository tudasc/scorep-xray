## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2009-2013,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2013,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2014,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2013,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013, 2022,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2013,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


AU_DEFUN([AC_SCOREP_POSIX_FUNCTIONS], [

    ##
    ## C
    ##

    AC_LANG_PUSH(C)
    _COMPAT_POSIX_CHECK([close])
    _COMPAT_POSIX_CHECK([fileno])
    _COMPAT_POSIX_CHECK([fseeko])
    _COMPAT_POSIX_CHECK([fseeko64])
    _COMPAT_POSIX_CHECK([getcwd])
    _COMPAT_POSIX_CHECK([gethostid])
    _COMPAT_POSIX_CHECK([gethostname])
    _COMPAT_POSIX_CHECK([getrlimit])
    _COMPAT_POSIX_CHECK([read])
    _COMPAT_POSIX_CHECK([realpath])
    AC_LANG_POP(C)

    ##
    ## CXX
    ##

    AFS_CXX_DECL_POSIX_CLOSE
    AFS_CXX_DECL_POSIX_FILENO
    AFS_CXX_DECL_POSIX_FSEEKO
    AFS_CXX_DECL_POSIX_FSEEKO64
    AFS_CXX_DECL_POSIX_GETCWD
    AFS_CXX_DECL_POSIX_GETHOSTID
    AFS_CXX_DECL_POSIX_GETHOSTNAME
    AFS_CXX_DECL_POSIX_READ

    AC_LANG_PUSH(C++)
    _COMPAT_POSIX_CHECK([pclose])
    _COMPAT_POSIX_CHECK([popen])
    AC_LANG_POP(C++)
], [
    The AC_SCOREP_POSIX_FUNCTIONS macro has been superseded by
    AFS_COMMON_UTILS to configure the common utils code and a
    set of AFS_POSIX_FOO checks for individual POSIX functions.

    !!! ATTENTION !!!
    The new macros use differently named configuration defines,
    Automake conditionals, and shell variables.  That is, code
    has to be adapted accordingly!
])


m4_define([_COMPAT_POSIX_CHECK], [
    AC_REQUIRE([AFS_POSIX_]m4_toupper($1))
    dnl Declaration
    ac_cv_have_decl_$1=afs_cv_have_[]_AC_LANG_ABBREV[]_decl_posix_$1
    AS_IF([test "x$afs_cv_have_[]_AC_LANG_ABBREV[]_decl_posix_$1" = "xyes"],
        [_afs_have_decl=1],
        [_afs_have_decl=0])
    AC_DEFINE_UNQUOTED([HAVE_DECL_]m4_toupper($1),
        [$_afs_have_decl],
        [Define to 1 if you have the declaration of `$1',
         and to 0 if you don't.])
    dnl Definition
    has_$1_func=$afs_cv_have_posix_$1
    AS_IF([test "x$afs_cv_have_posix_$1" = "xyes"],
        [AC_DEFINE([HAVE_]m4_toupper($1), 1,
            [Define to 1 if `$1' can be linked])])
    dnl Conditional
    AM_CONDITIONAL([HAVE_]m4_toupper($1), [test "x$afs_cv_have_posix_$1" = "xyes"])
])
