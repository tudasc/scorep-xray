#!/bin/bash

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2011, 2024,
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

## file       run_serial_ack_test.sh

SCOREP_EXPERIMENT_DIRECTORY=scorep-test_serial_ack
export SCOREP_EXPERIMENT_DIRECTORY

cleanup()
{
    rm -rf $SCOREP_EXPERIMENT_DIRECTORY
}
${KEEP_TEST_OUTPUT:+: }trap cleanup EXIT

SCOREP_VERBOSE=true \
    SCOREP_ENABLE_PROFILING=false \
    SCOREP_ENABLE_TRACING=true \
    ./serial_ack_test
