#!/bin/bash

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2011, 2018,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2011,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       run_omp_tasks.sh


SCOREP_ENABLE_PROFILING=false SCOREP_ENABLE_TRACING=true ./omp_tasks
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp
    exit 1
fi

if [ -f omp_tasks_untied ]; then
    SCOREP_ENABLE_PROFILING=false SCOREP_ENABLE_TRACING=true ./omp_tasks_untied
    if [ $? -ne 0 ]; then
        rm -rf scorep-measurement-tmp
        exit 1
    fi
fi
exit 0
