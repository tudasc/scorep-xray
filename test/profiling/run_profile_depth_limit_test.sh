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

## file       run_profile_depth_limit_test.sh
## maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>

# Remember current content of directory ro figure out the result dir
ls > start_ls.log

# Run test
SCOREP_PROFILING_MAX_CALLPATH_DEPTH=5 SCOREP_ENABLE_PROFILING=true SCOREP_ENABLE_TRACING=false ./profile_depth_limit_test

# Figure out the result dir
ls > end_ls.log
RESULT_DIR=`diff end_ls.log start_ls.log | grep scorep- | sed 's!< !!g'`
rm end_ls.log start_ls.log
if [ "x$RESULT_DIR" = "x" ]; then
    echo "Can not identify output directory. Skip evaluation of filter test"
    exit 0
fi

echo "Output is located at $PWD/$RESULT_DIR"

# Check output
if [ ! -e $RESULT_DIR/profile.cubex ]; then
  echo "Error: No profile generated."
  exit 1
fi

NUM_CNODES=`grep -c "<cnode id=" $RESULT_DIR/profile.cubex`
if [ ! x$NUM_CNODES = x6 ]; then
  echo "Expected 6 calpath definitions, but found $NUM_CNODES"
  exit 1
fi

exit 0
