#! /bin/sh

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
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

## file       test/measurement/config/test_framework.sh

if [ -n "$TEST_VERBOSE" ]; then
    error_out=/dev/stderr
else
    name="${0##*/}"
    error_out="${name%.*}.$$.out"
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
