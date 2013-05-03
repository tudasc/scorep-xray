/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       src/measurement/paradigm/scorep_mpp_ipc.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */

#include <config.h>
#include "scorep_status.h"
#include "scorep_ipc.h"

#include <UTILS_Error.h>

#include <UTILS_Debug.h>
#include <scorep_runtime_management_timings.h>
#include <assert.h>
#include <stdlib.h>


bool
SCOREP_Status_IsMpp( void )
{
    return true;
}


bool
scorep_create_experiment_dir( void ( * createDir )( void ) )
{
    if ( !SCOREP_Status_IsMppInitialized() )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPI, "MPI not initialized, experiment directory creation deferred." );
        return false;
    }

    if ( SCOREP_Status_GetRank() == 0 )
    {
        createDir();
    }

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
