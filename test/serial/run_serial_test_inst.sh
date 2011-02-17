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

## file       run_serial_test.sh
## maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>

rm -f config.h serial_inst_test
make scorep-config-tool-local
. ./scorep_config.dat
echo "/* Dummy */" > config.h
./scorep --instrument --verbosity=1 --config=scorep_config.dat $CC -I. -o serial_inst_test $SRC_ROOT/test/serial/serial_test.c
if [ ! -e serial_inst_test ]; then
    rm -f config.h
    exit 1
fi
./scorep --measure --verbosity=1 ./serial_inst_test
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp config.h
    exit 1
fi
rm -f config.h serial_inst_test

exit 0
