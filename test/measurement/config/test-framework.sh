#! /bin/sh

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2011, 2015,
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

## file       test/measurement/config/test_framework.sh

if test -n "$TEST_VERBOSE"
then
    error_out=/dev/stderr
else
    name="${0##*/}"
    error_out="$(pwd)/${name%.*}.$$.out"
fi
: >"$error_out"

test_init()
{
    test=0
    test_failed=0
    printf "test: %s\n" "$1"
}

test_expect_success()
{
    name="$1"
    to_run="$2"
    test=$(($test + 1))
    (eval "$to_run") 2>>"$error_out"
    rc=$?
    if test $rc -ne 0
    then
        printf " FAIL %d: %s\n" $test "$name"
        test_failed=$(($test_failed + 1))
    else
        printf "   ok %d: %s\n" $test  "$name"
    fi
}

test_must_fail()
{
    "$@"
    rc=$?
    if test $rc -eq 0
    then
        return 1
    else
        return 0
    fi
}

test_compare()
{
    diff -u $1 $2 >"$error_out" 2>&1
}

test_done()
{
    if test $test_failed -ne 0
    then
        printf "Failed %d of %d tests\n" $test_failed $test
        exit 1
    else
        printf "All %d tests OK\n" $test
        exit 0
    fi
}
