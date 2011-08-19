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

# Execute test without OpenMP and compiler instrumentation (only user)
echo "/* Dummy */" > config.h
./scorep --verbosity=1 --config=scorep_config.dat --user --nocompiler $CXX -I. -o user_cxx_test $SRC_ROOT/test/adapters/user/C++/user_test.cxx
if [ ! -e user_cxx_test ]; then
    rm -f config.h
    exit 1
fi

./user_cxx_test
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp config.h user_cxx_test
    exit 1
fi
rm -f config.h user_cxx_test


# Execute test with OpenMP compiler and user instrumentation.
echo "/* Dummy */" > config.h
./scorep --verbosity=1 --config=scorep_config.dat --user $CXX $OPENMP_CFLAGS -I. -o user_cxx_test $SRC_ROOT/test/adapters/user/C++/user_test.cxx
if [ ! -e user_cxx_test ]; then
    rm -f config.h
    exit 1
fi

./user_cxx_test
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp config.h user_cxx_test
    exit 1
fi
rm -f config.h user_cxx_test

exit 0

