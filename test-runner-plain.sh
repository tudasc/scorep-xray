#!/bin/sh

## -*- mode: sh -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## This script provides a simple test runner suitable for use with automake.
## It runs the given test executable with a default number of 4 OpenMP
## threads, if not specified otherwise by the OMP_NUM_THREADS environment
## variable. If the '--skip' option is passed, execution of the test is
## skipped and an appropriate exit code returned to automake's parallel-tests
## driver.


#--- Main program -----------------------------------------------------------

# Validate arguments
if test $# -gt 2; then
    echo "Usage: test-runner-plain.sh [<options>] <executable>" >&2
    echo >&2
    echo "Options:" >&2
    echo "  --skip    Skip test execution" >&2
    exit 1
fi


# Check whether test should by skipped
if test $# -eq 2; then
    if test "$1" = "--skip"; then
        echo "##EXEC## $2"
        exit 77
    else
        echo "ERROR: Unknown option '$1'!" >&2
        exit 1
    fi
fi

# If not specified, set default number of OpenMP threads
export OMP_NUM_THREADS=${OMP_NUM_THREADS:-4}

# Execute test
eval "$@"
