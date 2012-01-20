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

## file       run_selective_test.sh
## maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>

# Get SRC_ROOT and OTF2_PRINT
make clean-local-scorep-config-tool
make scorep-config-tool-local
. ./scorep_config.dat

RESULT_DIR=scorep-selective-f-test-dir
rm -rf $RESULT_DIR

# Execute selective test
SCOREP_EXPERIMENT_DIRECTORY=$RESULT_DIR SCOREP_ENABLE_PROFILING=false SCOREP_ENABLE_TRACING=true SCOREP_SELECTIVE_CONFIG_FILE=$SRC_ROOT/test/adapters/user/Fortran/selective.cfg ./user_f_test
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp start_ls.log
    exit 1
fi

# Check output
$OTF2_PRINT $RESULT_DIR/traces.otf2 | LC_ALL=C grep '^\(ENTER\|LEAVE\) ' > trace.txt
if [ x`grep -c Region1 trace.txt` = x4 ]; then
  if [ x`grep -v Region1 trace.txt | grep -v main | grep -v Region2` = x ]; then
    rm -rf trace.txt $RESULT_DIR
    exit 0;
  else
    echo "Unexpected events detected in trace:"
    echo "`grep -v Region1 trace.txt | grep -v main`"
    rm trace.txt
    exit 1
  fi
fi

echo "Uncorrect number of events for 'Region1'."
echo "4 expected but `grep -c Region1 trace.txt` occured"
rm trace.txt
exit 1


