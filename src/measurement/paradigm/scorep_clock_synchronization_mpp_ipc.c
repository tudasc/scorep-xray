/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2017,
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
 */


/**
 * @file
 *
 *
 */

#include <config.h>
#include <scorep_clock_synchronization.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Timer_Utils.h>
#include <scorep_ipc.h>
#include <scorep_environment.h>

#include <stddef.h>
#include <assert.h>

#define N_PINGPONGS 10

/* *INDENT-OFF* */
extern void scorep_interpolate_epoch(uint64_t* epochBegin, uint64_t* epochEnd);
/* *INDENT-ON*  */


static uint64_t
synchronize_with_worker( int worker, int* min_index )
{
    uint64_t master_send_time[ N_PINGPONGS ];
    uint64_t master_recv_time[ N_PINGPONGS ];
    for ( int i = 0; i < N_PINGPONGS; ++i )
    {
        /*
         * Some SHMEM implementations don't support zero-count put/get
         * operations and/or NULL buffers. That's why we send a dummy
         * integer value.
         */
        int dummy = 0;
        master_send_time[ i ] = SCOREP_Timer_GetClockTicks();
        SCOREP_Ipc_Send( &dummy, 1, SCOREP_IPC_INT, worker );
        SCOREP_Ipc_Recv( &dummy, 1, SCOREP_IPC_INT, worker );
        master_recv_time[ i ] = SCOREP_Timer_GetClockTicks();
    }

    uint64_t ping_pong_time = UINT64_MAX;
    for ( int i = 0; i < N_PINGPONGS; ++i )
    {
        uint64_t time_diff = master_recv_time[ i ] - master_send_time[ i ];
        if ( time_diff < ping_pong_time )
        {
            ping_pong_time = time_diff;
            *min_index     = i;
        }
    }

    return master_send_time[ *min_index ] + ping_pong_time / 2;
}


void
SCOREP_SynchronizeClocks( void )
{
    if ( SCOREP_Timer_ClockIsGlobal() || !SCOREP_Env_DoTracing() )
    {
        SCOREP_AddClockOffset( SCOREP_Timer_GetClockTicks(), 0, 0 );
        return;
    }

    int size = SCOREP_Ipc_GetSize();
    int rank = SCOREP_Ipc_GetRank();

    uint64_t offset_time;
    int64_t  offset;

    if ( rank == 0 )
    {
        for ( int worker = 1; worker < size; ++worker )
        {
            int      min_index;
            uint64_t sync_time = synchronize_with_worker( worker, &min_index );

            SCOREP_Ipc_Send( &sync_time, 1, SCOREP_IPC_UINT64_T, worker );
            SCOREP_Ipc_Send( &min_index, 1, SCOREP_IPC_INT, worker );
        }

        offset_time = SCOREP_Timer_GetClockTicks();
        offset      = 0;
    }
    else
    {
        uint64_t worker_time[ N_PINGPONGS ];
        int      master = 0;
        for ( int i = 0; i < N_PINGPONGS; ++i )
        {
            /*
             * Some SHMEM implementations don't support zero-count put/get
             * operations and/or NULL buffers. That's why we send a dummy
             * integer value.
             */
            int dummy = 0;
            SCOREP_Ipc_Recv( &dummy, 1, SCOREP_IPC_INT, master );
            worker_time[ i ] = SCOREP_Timer_GetClockTicks();
            SCOREP_Ipc_Send( &dummy, 1, SCOREP_IPC_INT, master );
        }

        uint64_t sync_time;
        int      min_index;
        SCOREP_Ipc_Recv( &sync_time, 1, SCOREP_IPC_UINT64_T, master );
        SCOREP_Ipc_Recv( &min_index, 1, SCOREP_IPC_INT, master );

        offset_time = worker_time[ min_index ];
        offset      = sync_time - offset_time;
    }

    double stddev_not_calculated_yet = 0;
    SCOREP_AddClockOffset( offset_time, offset, stddev_not_calculated_yet );
}


void
SCOREP_GetGlobalEpoch( uint64_t* globalEpochBegin, uint64_t* globalEpochEnd )
{
    uint64_t local_epoch_begin;
    uint64_t local_epoch_end;
    scorep_interpolate_epoch( &local_epoch_begin, &local_epoch_end );

    SCOREP_Ipc_Reduce( &local_epoch_begin,
                       globalEpochBegin,
                       1,
                       SCOREP_IPC_UINT64_T,
                       SCOREP_IPC_MIN,
                       0 );

    SCOREP_Ipc_Reduce( &local_epoch_end,
                       globalEpochEnd,
                       1,
                       SCOREP_IPC_UINT64_T,
                       SCOREP_IPC_MAX,
                       0 );
    if ( SCOREP_Ipc_GetRank() == 0 )
    {
        assert( *globalEpochEnd > *globalEpochBegin );
    }
}
