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
## Copyright (c) 2009-2012, 2014, 2018,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013,
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

AC_DEFUN([SCOREP_FORTRAN_MANGLED], [
AC_REQUIRE([AX_COMPILER_VENDOR])dnl
AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [mangling=" [##] _"],
    [sun],      [mangling=" [##] _"],
    [ibm],      [mangling=],
    [portland], [mangling=" [##] _"],
    [gnu],      [mangling=" [##] _"],
    [cray],     [mangling=" [##] _"],
    [fujitsu],  [mangling=" [##] _"],
    [])dnl
AC_DEFINE_UNQUOTED([FORTRAN_MANGLED(var)],
    [var${mangling}],
    [Name of var after mangled by the Fortran compiler.])dnl
])
