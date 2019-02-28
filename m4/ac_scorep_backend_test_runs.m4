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
## Copyright (c) 2009-2013, 2019,
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
              [AS_HELP_STRING([--enable-backend-test-runs[[=recursive]]],
                              [Enable execution of tests during 'make check' [no]
                              (does not affect building of tests, though).
                               If disabled, the files 'check-file-*' and/or 
                               'skipped_tests', listing the tests, are generated
                               in the corresponding build directory. If 'recursive'
                               is given, execution of tests is enabled for
                               sub-packages as well.])],
              [ac_scorep_enable_backend_test_runs=no
               # Don't enable backend test runs for sub-packages 
               # unless --enable-backend-test-runs=recursive was provided to
               # the toplevel configure. Requires that the toplevel configure
               # passes afs_externally_triggered=yes to sub-packages.
               # The intention is to skip (long-running) tests of sub-packages;
               # these packages are usually tested on their own.
               AS_IF([test "x$enableval" = xyes && test "x$afs_externally_triggered" != xyes ],
                         [ac_scorep_enable_backend_test_runs=yes],
                     [test "x$enableval" = xrecursive],
                         [ac_scorep_enable_backend_test_runs=yes])],
              [ac_scorep_enable_backend_test_runs="no"])

AM_CONDITIONAL([BACKEND_TEST_RUNS], [test "x$ac_scorep_enable_backend_test_runs" = "xyes"])
])
