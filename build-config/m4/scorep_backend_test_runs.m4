## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2019,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       scorep_backend_test_runs.m4

AC_DEFUN([SCOREP_BACKEND_TEST_RUNS], [
AC_SCOREP_BACKEND_TEST_RUNS
# enableval of --enable-backend-test-runs required to be available.
AS_IF([test "x${ac_scorep_enable_backend_test_runs}" = xyes \
       && test "x${enableval}" != xrecursive \
       && test "x${afs_externally_triggered}" != xyes],
      [AC_MSG_WARN([semantics of --enable-backend-test-runs changed in AC_PACKAGE_NAME 6.0. The new behavior is to run backend-test-runs for AC_PACKAGE_NAME only, but not for sub-packages (currently just otf2). To restore the old behavior, use `--enable-backend-test-runs=recursive`.])])
]) # SCOREP_BACKEND_TEST_RUNS
