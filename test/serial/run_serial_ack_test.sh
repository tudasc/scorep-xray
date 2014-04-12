#!/bin/bash

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

## file       run_serial_ack_test.sh

SCOREP_EXPERIMENT_DIRECTORY=scorep-test_serial_ack
export SCOREP_EXPERIMENT_DIRECTORY

cleanup()
{
    rm -rf $SCOREP_EXPERIMENT_DIRECTORY
}
trap cleanup EXIT

SCOREP_VERBOSE=true \
    SCOREP_ENABLE_PROFILING=false \
    SCOREP_ENABLE_TRACING=true \
    ./serial_ack_test
