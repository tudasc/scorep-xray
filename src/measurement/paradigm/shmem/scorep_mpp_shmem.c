/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
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

#include <stdio.h>

#define SHMEM_LOCATION_GROUP_NAME_LENGTH 32

static char name[ SHMEM_LOCATION_GROUP_NAME_LENGTH ];


char*
SCOREP_Mpp_GetLocationGroupName( void )
{
    int rank = SCOREP_Ipc_GetRank();
    snprintf( name, SHMEM_LOCATION_GROUP_NAME_LENGTH, "SHMEM PE %d", rank );

    return name;
}

#undef SHMEM_LOCATION_GROUP_NAME_LENGTH
