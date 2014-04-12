## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
##    RWTH Aachen University, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

AC_DEFUN([AC_SCOREP_ERROR_CODES], [
AC_SUBST([PACKAGE_ERROR_CODES_INCDIR], [../src/utils/include])
AC_DEFINE_UNQUOTED(
    [PACKAGE_ERROR_CODES_HEADER],
    [<SCOREP_ErrorCodes.h>],
    [The #include argument used to include this packages error codes header.]
)
])
