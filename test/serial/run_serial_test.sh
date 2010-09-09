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

## file       run_serial_test.sh
## maintainer Christian Roessel <c.roessel@fz-juelich.de>


./serial_test
if [ $? -ne 0 ]; then
    rm -rf silc-measurement-tmp
    exit 1
fi

rm -rf config.h serial_inst_test
make silc-config-tool-local
. silc_config.dat
echo "/* Dummy */" > config.h
./silc --instrument -verbosity=1 $CC -I. -o serial_inst_test $SRC_ROOT/test/serial/serial_test.c
if [ ! -e serial_inst_test ]; then
    exit 1
fi
./silc --measure -verbosity=1 ./serial_inst_test
if [ $? -ne 0 ]; then
    rm -rf silc-measurement-tmp
    exit 1
fi
rm -rf config.h serial_inst_test

exit 0
