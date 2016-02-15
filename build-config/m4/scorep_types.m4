## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2013,
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


## file build-config/m4/scorep_types.m4

# AC_SCOREP_CHECK_SIZEOF(TYPE, [unused], [INCLUDES = DEFAULT-INCLUDES])
# -----------------------------------------------------------
# Wraps AC_CHECK_SIZEOF, but always execute them in cross_compiling mode.
AC_DEFUN([AC_SCOREP_CHECK_SIZEOF],
[ac_scorep_cross_compiling_safe=$cross_compiling
cross_compiling=yes
AC_CHECK_SIZEOF([$1], [], ifelse([$3], , , [$3]))
cross_compiling=$ac_scorep_cross_compiling_safe
])# AC_SCOREP_CHECK_SIZEOF
