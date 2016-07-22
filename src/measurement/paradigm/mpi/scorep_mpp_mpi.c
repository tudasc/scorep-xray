/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 *
 */

#include <config.h>
#include "scorep_mpp.h"
#include "scorep_ipc.h"
#include <scorep_environment.h>

#include <stdio.h>

#define MPI_LOCATION_GROUP_NAME_LENGTH 32

static char name[ MPI_LOCATION_GROUP_NAME_LENGTH ];


char*
SCOREP_Mpp_GetLocationGroupName( void )
{
    // When using sequence definitions, we do no create individual names
    // in the definitions, but construct them from a base name and a
    // number when they are written to disk. Thus, return only the
    // base name in this case.
    if ( SCOREP_Env_UseSystemTreeSequence() )
    {
        return "MPI Rank";
    }
    int rank = SCOREP_Ipc_GetRank();
    snprintf( name, MPI_LOCATION_GROUP_NAME_LENGTH, "MPI Rank %d", rank );

    return name;
}

#undef MPI_LOCATION_GROUP_NAME_LENGTH
