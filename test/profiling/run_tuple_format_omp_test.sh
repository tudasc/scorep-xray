#!/bin/bash

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

if test -f ./jacobi_omp_c; then
    SCOREP_PROFILING_FORMAT=CUBE_TUPLE ./jacobi_omp_c
fi
