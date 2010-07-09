#! /bin/sh

## 
## This file is part of the SILC project (http://www.silc.de)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       test/measurement/config/config_test_size.sh
## maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>

. "${0%/*}/test-framework.sh"

test_init 'SIZE config variable type'

test_expect_success 'setting size to 0' '
    result=$(SILC_size=0 ./test_silc_config_size) &&
    test $result = 0
'

test_expect_success 'setting size to 1' '
    result=$(SILC_size=1 ./test_silc_config_size) &&
    test $result = 1
'

test_expect_success 'setting size to 108 with whitespace at front' '
    result=$(SILC_size=" 108" ./test_silc_config_size) &&
    test $result = 108
'

test_expect_success 'setting size to 108 with whitespace at end' '
    result=$(SILC_size="108 " ./test_silc_config_size) &&
    test $result = 108
'

test_expect_success 'setting size to 108 with whitespace at front and end' '
    result=$(SILC_size=" 108 " ./test_silc_config_size) &&
    test $result = 108
'

test_expect_success 'setting size to INT_MAX' '
    result=$(SILC_size=2147483647 ./test_silc_config_size) &&
    test $result = 2147483647
'

test_expect_success 'setting size to UINT64_MAX' '
    result=$(SILC_size=18446744073709551615 ./test_silc_config_size) &&
    test $result = 18446744073709551615
'

test_expect_success 'setting size to UINT64_MAX+1 must fail' '
    export SILC_size=18446744073709551616
    test_must_fail ./test_silc_config_size
'

test_expect_success 'setting size to -1 must fail' '
    export SILC_size=-1
    test_must_fail ./test_silc_config_size
'

test_expect_success 'setting size to 1b' '
    result=$(SILC_size=1b ./test_silc_config_size) &&
    test $result = 1
'

test_expect_success 'setting size to 1B' '
    result=$(SILC_size=1B ./test_silc_config_size) &&
    test $result = 1
'

test_expect_success 'setting size to "1 b"' '
    result=$(SILC_size="1 b" ./test_silc_config_size) &&
    test $result = 1
'

test_expect_success 'setting size to 1k' '
    result=$(SILC_size=1k ./test_silc_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to 1K' '
    result=$(SILC_size=1K ./test_silc_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to 1Kb' '
    result=$(SILC_size=1Kb ./test_silc_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to 1KB' '
    result=$(SILC_size=1KB ./test_silc_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to "1 Kb"' '
    result=$(SILC_size="1 Kb" ./test_silc_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to "1 KB"' '
    result=$(SILC_size="1 KB" ./test_silc_config_size) &&
    test $result = 1024
'

test_expect_success 'setting size to "1K b" must fail' '
    export SILC_size="1K b"
    test_must_fail ./test_silc_config_size
'

test_expect_success 'setting size to "1 K b" must fail' '
    export SILC_size="1 K b"
    test_must_fail ./test_silc_config_size
'

test_expect_success 'setting size to 1M' '
    result=$(SILC_size=1M ./test_silc_config_size) &&
    test $result = 1048576
'

test_expect_success 'setting size to 1G' '
    result=$(SILC_size=1G ./test_silc_config_size) &&
    test $result = 1073741824
'

test_expect_success 'setting size to 1T' '
    result=$(SILC_size=1T ./test_silc_config_size) &&
    test $result = 1099511627776
'

test_expect_success 'setting size to 1P' '
    result=$(SILC_size=1P ./test_silc_config_size) &&
    test $result = 1125899906842624
'

test_expect_success 'setting size to 1E' '
    result=$(SILC_size=1E ./test_silc_config_size) &&
    test $result = 1152921504606846976
'

test_expect_success 'setting size to 1Z must fail' '
    export SILC_size=1Z
    test_must_fail ./test_silc_config_size
'

test_expect_success 'setting size to 1Ea must fail' '
    export SILC_size=1Ea
    test_must_fail ./test_silc_config_size
'

test_expect_success 'setting size to 1024M' '
    result=$(SILC_size=1024M ./test_silc_config_size) &&
    test $result = 1073741824
'

test_expect_success 'setting size to 1073741824T must fail' '
    export SILC_size=1073741824T
    test_must_fail ./test_silc_config_size
'

test_done
