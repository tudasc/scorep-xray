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


#-------------------------------------------------------------
#                                                C Filter test
#-------------------------------------------------------------


# Remember current content of directory ro figure out the result dir
ls > start_ls.log

# Execute filter test
SCOREP_ENABLE_PROFILING=false SCOREP_ENABLE_TRACING=true SCOREP_FILTERING_FILE=$SRC_ROOT/test/filtering/filter.cfg ./filter_test
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp start_ls.log
    exit 1
fi

# Figure out the result dir
ls > end_ls.log
RESULT_DIR=`diff end_ls.log start_ls.log | grep scorep- | sed 's!< !!g'`
rm end_ls.log start_ls.log
if [ "x$RESULT_DIR" = "x" ]; then
    echo "Can not identify output directory. Skip evaluation of filter test"
    exit 0
fi
$OTF2_PRINT -A $RESULT_DIR/traces.otf2

# Check output
$OTF2_PRINT $RESULT_DIR/traces.otf2 | grep region > trace.txt

if [ "`grep filtered trace.txt`" ]; then
    echo "The following events should be filtered:"
    grep filtered trace.txt
    rm trace.txt
    exit 1
fi

if [ "x`grep main trace.txt`" = "x" ]; then
    echo "The main function should not be filtered."
    rm trace.txt
    exit 1
fi

if [ "x`grep filter_not1 trace.txt`" = "x" ]; then
    echo "The filter_not1 function should not be filtered."
    rm trace.txt
    exit 1
fi

if [ "x`grep filter_not2 trace.txt`" = "x" ]; then
    echo "The filter_not2 function should not be filtered."
    rm trace.txt
    exit 1
fi


#-------------------------------------------------------------
#                                          Fortran Filter test
#-------------------------------------------------------------

# Remember current content of directory ro figure out the result dir
ls > start_ls.log

# Execute filter test
SCOREP_ENABLE_PROFILING=false SCOREP_ENABLE_TRACING=true SCOREP_FILTERING_FILE=$SRC_ROOT/test/filtering/filter_f.cfg ./filter_f_test
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp start_ls.log
    exit 1
fi

# Figure out the result dir
ls > end_ls.log
RESULT_DIR=`diff end_ls.log start_ls.log | grep scorep- | sed 's!< !!g'`
rm end_ls.log start_ls.log
if [ "x$RESULT_DIR" = "x" ]; then
    echo "Can not identify output directory. Skip evaluation of filter test"
    exit 0
fi
$OTF2_PRINT -A $RESULT_DIR/traces.otf2

# Check output
$OTF2_PRINT $RESULT_DIR/traces.otf2 | grep region > trace.txt

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

exit 0

