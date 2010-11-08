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

## file       test/measurement/config/config_test_size.sh
## maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>

. "${0%/*}/test-framework.sh"

test_init 'SIZE config variable type'

test_expect_success 'setting size to 0' '
    result=$(SCOREP_size=0 ./test_scorep_config_size) &&
    test $result = 0
'

test_expect_success 'setting size to 1' '
    result=$(SCOREP_size=1 ./test_scorep_config_size) &&
    test $result = 1
'

test_expect_success 'setting size to 108 with whitespace at front' '
    result=$(SCOREP_size=" 108" ./test_scorep_config_size) &&
    test $result = 108
'

test_expect_success 'setting size to 108 with whitespace at end' '
    result=$(SCOREP_size="108 " ./test_scorep_config_size) &&
    test $result = 108
'

test_expect_success 'setting size to 108 with whitespace at front and end' '
    result=$(SCOREP_size=" 108 " ./test_scorep_config_size) &&
    test $result = 108
'

test_expect_success 'setting size to INT_MAX' '
    result=$(SCOREP_size=2147483647 ./test_scorep_config_size) &&
    test $result = 2147483647
'

test_expect_success 'setting size to UINT64_MAX' '
    result=$(SCOREP_size=18446744073709551615 ./test_scorep_config_size) &&
    test $result = 18446744073709551615
'

test_expect_success 'setting size to UINT64_MAX+1 must fail' '
    export SCOREP_size=18446744073709551616
    test_must_fail ./test_scorep_config_size
'

test_expect_success 'setting size to -1 must fail' '
    export SCOREP_size=-1
    test_must_fail ./test_scorep_config_size
'

test_expect_success 'setting size to 1b' '
    result=$(SCOREP_size=1b ./test_scorep_config_size) &&
    test $result = 1
'

test_expect_success 'setting size to 1B' '
    result=$(SCOREP_size=1B ./test_scorep_config_size) &&
    test $result = 1
'

test_expect_success 'setting size to "1 b"' '
    result=$(SCOREP_size="1 b" ./test_scorep_config_size) &&
    test $result = 1
'

test_expect_success 'setting size to 1k' '
    result=$(SCOREP_size=1k ./test_scorep_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to 1K' '
    result=$(SCOREP_size=1K ./test_scorep_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to 1Kb' '
    result=$(SCOREP_size=1Kb ./test_scorep_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to 1KB' '
    result=$(SCOREP_size=1KB ./test_scorep_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to "1 Kb"' '
    result=$(SCOREP_size="1 Kb" ./test_scorep_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to "1 KB"' '
    result=$(SCOREP_size="1 KB" ./test_scorep_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to "1K b" must fail' '
    export SCOREP_size="1K b"
    test_must_fail ./test_scorep_config_size
'

test_expect_success 'setting size to "1 K b" must fail' '
    export SCOREP_size="1 K b"
    test_must_fail ./test_scorep_config_size
'

test_expect_success 'setting size to 1M' '
    result=$(SCOREP_size=1M ./test_scorep_config_size) &&
    test $result = 1048576
'

test_expect_success 'setting size to 1G' '
    result=$(SCOREP_size=1G ./test_scorep_config_size) &&
    test $result = 1073741824
'

test_expect_success 'setting size to 1T' '
    result=$(SCOREP_size=1T ./test_scorep_config_size) &&
    test $result = 1099511627776
'

test_expect_success 'setting size to 1P' '
    result=$(SCOREP_size=1P ./test_scorep_config_size) &&
    test $result = 1125899906842624
'

test_expect_success 'setting size to 1E' '
    result=$(SCOREP_size=1E ./test_scorep_config_size) &&
    test $result = 1152921504606846976
'

test_expect_success 'setting size to 1Z must fail' '
    export SCOREP_size=1Z
    test_must_fail ./test_scorep_config_size
'

test_expect_success 'setting size to 1Ea must fail' '
    export SCOREP_size=1Ea
    test_must_fail ./test_scorep_config_size
'

test_expect_success 'setting size to 1024M' '
    result=$(SCOREP_size=1024M ./test_scorep_config_size) &&
    test $result = 1073741824
'

test_expect_success 'setting size to 1073741824T must fail' '
    export SCOREP_size=1073741824T
    test_must_fail ./test_scorep_config_size
'

test_done
