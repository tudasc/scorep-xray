/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
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


#include <UTILS_Error.h>


#include <UTILS_Debug.h>


#include "scorep_ipc.h"
#include "scorep_mpp.h"
#include "scorep_status.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define LOCATION_GROUP_NAME_LENGTH 9

static char name[ LOCATION_GROUP_NAME_LENGTH ];


bool
SCOREP_Status_IsMpp( void )
{
    return false;
}

char*
SCOREP_Mpp_GetLocationGroupName( void )
{
    snprintf( name, LOCATION_GROUP_NAME_LENGTH, "Process" );

    return name;
}

void
scorep_timing_reduce_runtime_management_timings( void )
{
}

#if HAVE( PLATFORM_K )
/* On K a MPI call is used to gather coordinate information.
   In serial mode the call is not called or needed, however the platform
   files are MPI unaware and therefore the symbol is needed to avoid
   undefined references.
 */
int
FJMPI_Topology_sys_rank2xyzabc( int  rank,
                                int* x,
                                int* y,
                                int* z,
                                int* a,
                                int* b,
                                int* c )
{
    return 0;
}
#endif
