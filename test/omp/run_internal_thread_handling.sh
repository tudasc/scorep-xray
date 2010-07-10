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

## file       run_omp_test.sh
## maintainer Christian Roessel <c.roessel@fz-juelich.de>


./internal_thread_handling
if [ $? -ne 0 ]; then
    rm -rf silc-measurement-tmp
    exit 1
fi
exit 0
