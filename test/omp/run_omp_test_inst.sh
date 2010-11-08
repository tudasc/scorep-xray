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

## file       run_omp_test_inst.sh
## maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>

echo "==================="
echo $PWD
echo "==================="


rm -f config.h omp_inst_test
make scorep-config-tool-local
. ./scorep_config.dat
echo "/* Dummy */" > config.h
cp $SRC_ROOT/test/omp/omp_test.c .
./scorep --instrument -verbosity=1 -config=scorep_config.dat $CC -I. -o omp_inst_test omp_test.c $OPENMP_CFLAGS
rm -f config.h omp_test.c omp_test.opari.c omp_inst_test.pomp_init.c omp_test.c.opari.inc
if [ ! -e omp_inst_test ]; then
    exit 1
fi
./scorep --measure -verbosity=1 ./omp_inst_test
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp omp_inst_test
    exit 1
fi
rm -f omp_inst_test

exit 0
