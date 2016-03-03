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
#include "scorep_status.h"
#include "scorep_ipc.h"

#include <UTILS_Error.h>

#include <UTILS_Debug.h>
#include <scorep_runtime_management_timings.h>
#include <stdlib.h>


bool
SCOREP_Status_IsMpp( void )
{
    return true;
}


void
scorep_timing_reduce_runtime_management_timings( void )
{
    #if HAVE( SCOREP_RUNTIME_MANAGEMENT_TIMINGS )
    int size = SCOREP_Ipc_GetSize();
    SCOREP_Ipc_Reduce( scorep_timing_sendbuf_durations,
                       scorep_timing_recvbuf_durations_mean,
                       scorep_timing_num_entries,
                       SCOREP_IPC_DOUBLE,
                       SCOREP_IPC_SUM,
                       0 );
    for ( int i = 0; i < scorep_timing_num_entries; ++i )
    {
        scorep_timing_recvbuf_durations_mean[ i ] /= size;
    }
    SCOREP_Ipc_Reduce( scorep_timing_sendbuf_durations,
                       scorep_timing_recvbuf_durations_max,
                       scorep_timing_num_entries,
                       SCOREP_IPC_DOUBLE,
                       SCOREP_IPC_MAX,
                       0 );
    SCOREP_Ipc_Reduce( scorep_timing_sendbuf_durations,
                       scorep_timing_recvbuf_durations_min,
                       scorep_timing_num_entries,
                       SCOREP_IPC_DOUBLE,
                       SCOREP_IPC_MIN,
                       0 );
    #endif
}
