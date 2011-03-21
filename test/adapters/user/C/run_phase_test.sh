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

## file       run_phase_test.sh
## maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>

echo "==================="
echo $PWD
echo "==================="

make scorep-config-tool-local
. ./scorep_config.dat
echo "/* Dummy */" > config.h

cp  $SRC_ROOT/test/adapters/user/C/scorep_phase_test.c scorep_phase_test.c
./scorep --instrument --user --verbosity=1 --config=scorep_config.dat $CC -I. -o scorep_phase_test scorep_phase_test.c $OPENMP_CFLAGS
rm -f config.h scorep_config.dat scorep_phase_test.pomp_init.c  scorep_phase_test.opari.c  scorep_phase_test.c.opari.inc scorep_phase_test.c
if [ ! -e scorep_phase_test ]; then
    exit 1
fi
./scorep --measure --verbosity=1 ./scorep_phase_test
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp scorep_phase_test
    exit 1
fi
rm -f scorep_phase_test

exit 0
