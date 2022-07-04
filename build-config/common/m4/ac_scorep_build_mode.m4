## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2011,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2011,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


# needs to be called after LT_INIT, otherwise enable_(static|shared) are not set
AC_DEFUN([AC_SCOREP_BUILD_MODE], [

AM_CONDITIONAL([BUILD_SHARED_LIBRARIES], [test "x${enable_shared}" = "xyes"])
AM_CONDITIONAL([BUILD_STATIC_LIBRARIES], [test "x${enable_static}" = "xyes"])

if test "x${enable_shared}" = "xyes"; then
    AC_DEFINE([SCOREP_SHARED_BUILD], [], [Defined if we are building shared libraries. See also SCOREP_STATIC_BUILD])
fi

if test "x${enable_static}" = "xyes"; then
    AC_DEFINE([SCOREP_STATIC_BUILD], [], [Defined if we are building static libraries. See also SCOREP_SHARED_BUILD])
fi

])
