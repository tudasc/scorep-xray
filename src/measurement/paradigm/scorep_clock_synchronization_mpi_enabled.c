/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       src/measurement/paradigm/scorep_clock_synchronization_mpi_enabled.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <scorep_clock_synchronization.h>

#include <scorep_definitions.h>
#include <SCOREP_Timing.h>
#include <scorep_mpi.h>

#include <mpi.h>
#include <stddef.h>

#define N_PINGPONGS 10

uint64_t master_send_time[ N_PINGPONGS ];
uint64_t master_recv_time[ N_PINGPONGS ];

/* *INDENT-OFF* */
extern void scorep_interpolate_epoch(uint64_t* epochBegin, uint64_t* epochEnd);
static void scorep_synchronize_master( int worker, uint64_t* ping_pong_time, int* min_index );
/* *INDENT-ON*  */


void
SCOREP_SynchronizeClocks()
{
    if ( SCOREP_ClockIsGlobal() )
    {
        SCOREP_AddClockOffset( SCOREP_GetClockTicks(), 0, 0 );
        return;
    }

    int64_t  offset;
    uint64_t offset_time;

    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        SCOREP_Mpi_Status status;
        for ( int worker = 1; worker < SCOREP_Mpi_GetCommWorldSize(); ++worker )
        {
            uint64_t ping_pong_time;
            int      min_index = 0;
            scorep_synchronize_master( worker, &ping_pong_time, &min_index );
            uint64_t sync_time = master_send_time[ min_index ] + ping_pong_time / 2;

            SCOREP_Mpi_Send( &sync_time, 1, SCOREP_MPI_UNSIGNED_LONG_LONG, worker );
            SCOREP_Mpi_Send( &min_index, 1, SCOREP_MPI_INT, worker );
        }

        offset      = 0;
        offset_time = SCOREP_GetClockTicks();
    }
    else
    {
        uint64_t   worker_time[ N_PINGPONGS ];
        MPI_Status status;
        int        master = 0;
        for ( int i = 0; i < N_PINGPONGS; ++i )
        {
            //SCOREP_Mpi_Recv( NULL, 0, SCOREP_MPI_INT, master, &status );
            PMPI_Recv( NULL, 0, MPI_INT, master, 1, MPI_COMM_WORLD, &status );
            worker_time[ i ] = SCOREP_GetClockTicks();
            //SCOREP_Mpi_Send( NULL, 0, SCOREP_MPI_INT, master );
            PMPI_Send( NULL, 0, MPI_INT, master, 2, MPI_COMM_WORLD );
        }

        uint64_t sync_time;
        int      min_index;
        SCOREP_Mpi_Recv( &sync_time, 1, SCOREP_MPI_UNSIGNED_LONG_LONG, master, &status );
        SCOREP_Mpi_Recv( &min_index, 1, SCOREP_MPI_INT, master, &status );

        offset      = sync_time - worker_time[ min_index ];
        offset_time = worker_time[ min_index ];
    }

    uint64_t stddev_not_calculated_yet = 0;
    SCOREP_AddClockOffset( offset_time, offset, stddev_not_calculated_yet );
}


static void
scorep_synchronize_master( int worker, uint64_t* ping_pong_time, int* min_index )
{
    MPI_Status status;
    int        foox;
    for ( int i = 0; i < N_PINGPONGS; ++i )
    {
        master_send_time[ i ] = SCOREP_GetClockTicks();
        //SCOREP_Mpi_Send( NULL, 0, SCOREP_MPI_INT, worker );
        //SCOREP_Mpi_Recv( NULL, 0, SCOREP_MPI_INT, worker, &status );
        PMPI_Send( NULL, 0, MPI_INT, worker, 1, MPI_COMM_WORLD );
        PMPI_Recv( NULL, 0, MPI_INT, worker, 2, MPI_COMM_WORLD, &status );
        master_recv_time[ i ] = SCOREP_GetClockTicks();
    }

    *ping_pong_time = master_recv_time[ 0 ] - master_send_time[ 0 ];
    for ( int i = 1; i < N_PINGPONGS; ++i )
    {
        if ( master_recv_time[ i ] - master_send_time[ i ] < *ping_pong_time )
        {
            *ping_pong_time = master_recv_time[ i ] - master_send_time[ i ];
            *min_index      = i;
        }
    }
}


void
SCOREP_GetGlobalEpoch( uint64_t* globalEpochBegin, uint64_t* globalEpochEnd )
{
    uint64_t local_epoch_begin;
    uint64_t local_epoch_end;
    scorep_interpolate_epoch( &local_epoch_begin, &local_epoch_end );

    SCOREP_Mpi_Reduce( &local_epoch_begin,
                       globalEpochBegin,
                       1,
                       SCOREP_MPI_UNSIGNED_LONG_LONG,
                       SCOREP_MPI_MIN,
                       0 );

    SCOREP_Mpi_Reduce( &local_epoch_end,
                       globalEpochEnd,
                       1,
                       SCOREP_MPI_UNSIGNED_LONG_LONG,
                       SCOREP_MPI_MAX,
                       0 );
}
