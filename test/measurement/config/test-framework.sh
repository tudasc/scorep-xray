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
## Copyright (c) 2009-2011, 2015, 2017,
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

# ### setup color support ###
color_enabled=no
if [ "$AM_COLOR_TESTS" = no ]; then
    color_enabled=no
elif [ "$AM_COLOR_TESTS" = always ]; then
    color_enabled=yes
elif [ "$TERM" != dumb ] && [ -t 1 ]; then
    color_enabled=yes
fi

if test $color_enabled = yes; then
    # e for ascii escape sequence. Add some prefix to avoid name clashes when being sourced.
    e_r='\033[0;31m'
    e_g='\033[0;32m'
    e_b='\033[1m'
    e_rb="$e_r$e_b"
    e_gb="$e_g$e_b"
    e_n='\033[0m'
else
    e_r=''
    e_g=''
    e_b=''
    e_rb=''
    e_gb=''
    e_n=''
fi

# ### determine output files ###
if [ -n "$TEST_VERBOSE" ]; then
    error_out=/dev/stderr
else
    name="${0##*/}"
    error_out="$(pwd)/${name%.*}.$$.out"
fi
: >"$error_out"

error_out_tmp="$(pwd)/${name%.*}.$$.out.tmp"

# ### functions ###

test_init()
{
    test=0
    tests_failed=0
    printf "${e_b}Test: %s${e_n}\n" "$1"
}

test_expect_success()
{
    name="$1"
    to_run="$2"
    test=$(($test + 1))
    test -n "$TEST_VERBOSE" && echo "$to_run"
    (eval "$to_run") 2>>"$error_out"
    rc=$?
    if test $rc -ne 0
    then
        printf " ${e_rb}fail${e_n} %d: %s\n" $test "$name"
        tests_failed=$(($tests_failed + 1))
    else
        printf " ${e_gb}ok  ${e_n} %d: %s\n" $test  "$name"
    fi
}

# Similar to test_expect_success() if you have a chain of test && test && ...
# Note:
#  * Every argument is executed and is expected to succeed
#  * If one fails, its output will be shown by default and the first failed argument/command is provided as well
#  * In contrast to test_expect_success() it provides all output, not only stderr output
test_expect_success_all()
{
    name="$1"
    shift
    test=$(($test + 1))
    test_orig_pwd=$PWD

    first_failed_run=""
    while [ $# -gt 0 ]; do
        run="$1"
        shift
        test -n "$TEST_VERBOSE" && echo "$run"
        eval "$run" >$error_out_tmp 2>&1
        rc=$?
        cat "$error_out_tmp" >>"$error_out"
        if [ $rc -ne 0 ] && [ -z "$first_failed_run" ]; then
            first_failed_run="$run"
            if [ -s "$error_out_tmp" ]; then
                cat "$error_out_tmp"
            fi
        fi
    done

    if [ -n "$first_failed_run" ]; then
        printf " ${e_rb}fail${e_n} %d: ${e_b}%s${e_n}: %s\n" $test "$name" "$first_failed_run"
        tests_failed=$(($tests_failed + 1))
    else
        printf " ${e_gb}ok${e_n}   %d: %s\n" $test  "$name"
    fi
    cd "$test_orig_pwd"
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
    if test $tests_failed -ne 0
    then
        printf "${e_rb}Failed${e_n} %d of %d tests\n" $tests_failed $test
        exit 1
    else
        printf "All %d tests ${e_gb}OK${e_n}\n" $test
        exit 0
    fi
}

# Portability work-around
test_grep()
{
    GREP_OPTIONS= grep "$@"
}

# Convenience function
# Assumes the last argument is the file
# Assumes file.tmp is not used. Will be scrambled.
test_sed_i()
{
    file=
    for a in "$@"; do file="$a"; done

    sed "$@" > $file.tmp
    mv -f $file.tmp $file
}
