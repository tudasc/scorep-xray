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

## file       test/measurement/config/config_test_number.sh

. "${0%/*}/test-framework.sh"

test_init 'NUMBER config variable type'

test_expect_success 'setting number to 0' '
    result=$(SCOREP_NUMBER=0 ./test_scorep_config_number) &&
    test $result = 0
'

test_expect_success 'setting number to 1' '
    result=$(SCOREP_NUMBER=1 ./test_scorep_config_number) &&
    test $result = 1
'

test_expect_success 'setting number to 108 with whitespace at front' '
    result=$(SCOREP_NUMBER=" 108" ./test_scorep_config_number) &&
    test $result = 108
'

test_expect_success 'setting number to 108 with whitespace at end' '
    result=$(SCOREP_NUMBER="108 " ./test_scorep_config_number) &&
    test $result = 108
'

test_expect_success 'setting number to 108 with whitespace at front and end' '
    result=$(SCOREP_NUMBER=" 108 " ./test_scorep_config_number) &&
    test $result = 108
'

test_expect_success 'setting number to INT_MAX' '
    result=$(SCOREP_NUMBER=2147483647 ./test_scorep_config_number) &&
    test $result = 2147483647
'

test_expect_success 'setting number to UINT64_MAX' '
    result=$(SCOREP_NUMBER=18446744073709551615 ./test_scorep_config_number) &&
    test $result = 18446744073709551615
'

test_expect_success 'setting number to UINT64_MAX+1 must fail' '
    export SCOREP_NUMBER=18446744073709551616
    test_must_fail ./test_scorep_config_number
'

test_expect_success 'setting number to -1 must fail' '
    export SCOREP_NUMBER=-1
    test_must_fail ./test_scorep_config_number
'

test_expect_success 'garbage after number must fail' '
    export SCOREP_NUMBER="1foo"
    test_must_fail ./test_scorep_config_number
'

test_expect_success 'garbage with space after number must fail' '
    export SCOREP_NUMBER="1 foo"
    test_must_fail ./test_scorep_config_number
'

test_done
