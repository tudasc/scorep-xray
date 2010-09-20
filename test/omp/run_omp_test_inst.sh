#!/bin/bash

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

## file       run_omp_test_inst.sh
## maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>

echo "==================="
echo $PWD
echo "==================="


rm -f config.h omp_inst_test
make silc-config-tool-local
. ./silc_config.dat
echo "/* Dummy */" > config.h
cp $SRC_ROOT/test/omp/omp_test.c .
./silc --instrument -verbosity=1 $CC -I. -o omp_inst_test omp_test.c $OPENMP_CFLAGS
rm -f config.h omp_test.c omp_test.opari.c omp_inst_test.pomp_init.c omp_test.c.opari.inc
if [ ! -e omp_inst_test ]; then
    exit 1
fi
./silc --measure -verbosity=1 ./omp_inst_test
if [ $? -ne 0 ]; then
    rm -rf silc-measurement-tmp omp_inst_test
    exit 1
fi
rm -f omp_inst_test

exit 0
