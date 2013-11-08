/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen, Germany
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
 * @file
 *
 *
 */

#include <config.h>
#include <scorep_clock_synchronization.h>

#include <definitions/SCOREP_Definitions.h>
#include <SCOREP_Timing.h>
#include <scorep_ipc.h>

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
        master_send_time[ i ] = SCOREP_GetClockTicks();
        SCOREP_Ipc_Send( NULL, 0, SCOREP_IPC_INT, worker );
        SCOREP_Ipc_Recv( NULL, 0, SCOREP_IPC_INT, worker );
        master_recv_time[ i ] = SCOREP_GetClockTicks();
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
SCOREP_SynchronizeClocks()
{
    if ( SCOREP_ClockIsGlobal() )
    {
        SCOREP_AddClockOffset( SCOREP_GetClockTicks(), 0, 0 );
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

        offset_time = SCOREP_GetClockTicks();
        offset      = 0;
    }
    else
    {
        uint64_t worker_time[ N_PINGPONGS ];
        int      master = 0;
        for ( int i = 0; i < N_PINGPONGS; ++i )
        {
            SCOREP_Ipc_Recv( NULL, 0, SCOREP_IPC_INT, master );
            worker_time[ i ] = SCOREP_GetClockTicks();
            SCOREP_Ipc_Send( NULL, 0, SCOREP_IPC_INT, master );
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
