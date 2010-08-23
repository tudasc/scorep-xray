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

make silc-config-tool-local
. silc_config.dat
./silc --instrument $CC -o test $SRC_ROOT/test/serial/serial_test.c
./test
if [ $? -ne 0 ]; then
    rm -rf silc-measurement-tmp test
    exit 1
fi
rm -rf test

exit 0
