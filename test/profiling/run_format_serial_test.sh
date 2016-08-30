#!/bin/bash

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2015,
## Technische Universität Darmstadt, Darmstadt, Germany
##
## Copyright (c) 2013,
## Forschungszentrum Juelich GmbH, Juelich, Germany
##
## Copyright (c) 2014,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

formats="TAU_SNAPSHOT CUBE_TUPLE THREAD_SUM THREAD_TUPLE KEY_THREADS CLUSTER_THREADS"

if test -f ./jacobi_serial_c; then
    for format in $formats; do
	echo "Testing $format format in serial mode ..."
        SCOREP_PROFILING_FORMAT=$format ./jacobi_serial_c
    done
fi
