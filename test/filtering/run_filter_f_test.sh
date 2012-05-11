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

## file       run_filter_test.sh
## maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>

# Get SRC_ROOT and OTF2_PRINT
make clean-local-scorep-config-tool
make scorep-config-tool-local
. ./scorep_config.dat

RESULT_DIR=scorep-filter-f-test-dir
rm -rf $RESULT_DIR

#-------------------------------------------------------------
#                                          Fortran Filter test
#-------------------------------------------------------------

# Execute filter test
SCOREP_EXPERIMENT_DIRECTORY=$RESULT_DIR \
SCOREP_ENABLE_PROFILING=false \
SCOREP_ENABLE_TRACING=true \
SCOREP_FILTERING_FILE=$SRC_ROOT/test/filtering/filter_f.cfg \
gdb -batch -x $SRC_ROOT/test/filtering/filter_f.gdb ./filter_f_test
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp start_ls.log
    exit 1
fi

$OTF2_PRINT -A $RESULT_DIR/traces.otf2

# Check output
$OTF2_PRINT $RESULT_DIR/traces.otf2 | LC_ALL=C grep '^\(ENTER\|LEAVE\) ' > trace.txt

if [ "`grep foo trace.txt`" ]; then
    echo "The following events should be filtered:"
    grep foo trace.txt
    rm trace.txt
    exit 1
fi

if [ "`grep FOO trace.txt`" ]; then
    echo "The following events should be filtered:"
    grep FOO trace.txt
    rm trace.txt
    exit 1
fi

if [ "`grep bar trace.txt`" ]; then
    echo "The following events should be filtered:"
    grep bar trace.txt
    rm trace.txt
    exit 1
fi

if [ "`grep BAR trace.txt`" ]; then
    echo "The following events should be filtered:"
    grep BAR trace.txt
    rm trace.txt
    exit 1
fi

if [ "`grep baz trace.txt`" ]; then
    echo "The following events should be filtered:"
    grep baz trace.txt
    rm trace.txt
    exit 1
fi

if [ "`grep BAZ trace.txt`" ]; then
    echo "The following events should be filtered:"
    grep BAZ trace.txt
    rm trace.txt
    exit 1
fi

rm -rf $RESULT_DIR
exit 0

