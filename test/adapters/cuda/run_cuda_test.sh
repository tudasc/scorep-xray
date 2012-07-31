#!/bin/bash

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012, 
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

## file       run_cuda_test.sh
## maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>

echo "===== CUDA Compile and Run Test ====="

rm -f config.h cuda_test

# Get SRC_ROOT
make clean-local-scorep-config-tool
make scorep-config-tool-local
. ./scorep_config.dat

echo "/* Dummy */" > config.h

./scorep --build-check nvcc $SRC_ROOT/test/adapters/cuda/cuda_test.cu -o cuda_test -I.

SCOREP_CUDA_ENABLE=runtime,kernel,idle \
SCOREP_EXPERIMENT_DIRECTORY=scorep-serial-cuda \
./cuda_test

if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp
    exit 1
fi

rm -f config.h
echo "====================================="

exit 0
