## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file build-config/m4/scorep_runtime_management_timings.m4


AC_DEFUN([AC_SCOREP_RUNTIME_MANAGEMENT_TIMINGS], [

AC_ARG_VAR([RUNTIME_MANAGEMENT_TIMINGS],
           [Whether to activate time measurements for Score-P's SCOREP_InitMeasurement() and scorep_finalize() functions. Activation values are '1', 'yes', and 'true'. For developer use.])

AS_IF([test "x${RUNTIME_MANAGEMENT_TIMINGS}" = "x1"   ||
       test "x${RUNTIME_MANAGEMENT_TIMINGS}" = "xyes" ||
       test "x${RUNTIME_MANAGEMENT_TIMINGS}" = "xtrue"],
      [AC_DEFINE([HAVE_SCOREP_RUNTIME_MANAGEMENT_TIMINGS], [1] , [If set, time measurements for Score-P's SCOREP_InitMeasurement() and scorep_finalize() functions are performed.])])

])
