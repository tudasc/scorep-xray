## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
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
## Copyright (c) 2009-2013,
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


## file       ac_scorep_backend_test_runs.m4

AC_DEFUN([AC_SCOREP_BACKEND_TEST_RUNS], [
AC_ARG_ENABLE([backend-test-runs],
              [AS_HELP_STRING([--enable-backend-test-runs],
                              [Enable execution of tests during 'make check' [no] (does not affect building of tests, though). If disabled, the files 'check-file-*' and/or 'skipped_tests' listing the tests are generated in the corresponding build directory.])],
              [ac_scorep_enable_backend_test_runs=$enableval],
              [ac_scorep_enable_backend_test_runs="no"])

AM_CONDITIONAL([BACKEND_TEST_RUNS], [test "x$ac_scorep_enable_backend_test_runs" = "xyes"])
])
