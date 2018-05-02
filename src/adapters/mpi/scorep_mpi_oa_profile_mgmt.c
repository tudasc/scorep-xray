/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
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
 * @brief Definitions of MPI profiling functions used in libscorep_adapter_mpi_mgmt
 */

#include <config.h>

#include "scorep_mpi_oa_profile.h"
#include "scorep_mpi_oa_profile_mgmt.h"

#include <stdlib.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME MPIPROFILING
#include <UTILS_Debug.h>


int                              scorep_mpiprofiling_initialized = 0;
static int                       scorep_mpiprofiling_numprocs;
int                              scorep_mpiprofiling_myrank;
scorep_mpiprofile_world_comm_dup scorep_mpiprofiling_world_comm_dup;

void* scorep_mpiprofiling_remote_time_packs        = NULL;
void* scorep_mpiprofiling_local_time_pack          = NULL;
void* scorep_mpiprofiling_remote_time_pack         = NULL;
int   scorep_mpiprofiling_remote_time_packs_in_use = 0;
int   scorep_mpiprofiling_local_time_pack_in_use   = 0;
int   scorep_mpiprofiling_remote_time_pack_in_use  = 0;

void**       scorep_mpiprofiling_send_timepack_pool = 0;
MPI_Request* scorep_mpiprofiling_timepack_requests  = 0;
int          scorep_mpiprofiling_timepack_pool_size = 0;

static void
scorep_mpiprofile_free_timepack_pool( void )
{
    int i;
    for ( i = 0; i < scorep_mpiprofiling_timepack_pool_size; i++ )
    {
        free( scorep_mpiprofiling_send_timepack_pool[ i ] );
    }
    free( scorep_mpiprofiling_send_timepack_pool );
    free( scorep_mpiprofiling_timepack_requests );
}

/**
 * Initializes MPI profiling module
 */
void
scorep_mpiprofile_init( void )
{
    if ( scorep_mpiprofiling_initialized )
    {
        return;
    }
    UTILS_DEBUG_ENTRY();
    /* -- duplicate MPI_COMM_WORLD to be used for mpi profiling messages -- */
    if ( PMPI_Comm_dup( MPI_COMM_WORLD, &( scorep_mpiprofiling_world_comm_dup.comm ) ) != MPI_SUCCESS )
    {
        UTILS_DEBUG( "Could not duplicate MPI_COMM_WORLD" );
        return;
    }
    if ( PMPI_Comm_group( scorep_mpiprofiling_world_comm_dup.comm, &( scorep_mpiprofiling_world_comm_dup.group ) ) != MPI_SUCCESS )
    {
        UTILS_DEBUG( "Could not get the group of the MPI_COMM_WORLD duplicate" );
        return;
    }

    PMPI_Comm_size( scorep_mpiprofiling_world_comm_dup.comm, &scorep_mpiprofiling_numprocs );
    PMPI_Comm_rank( scorep_mpiprofiling_world_comm_dup.comm, &scorep_mpiprofiling_myrank );
    UTILS_DEBUG( "myrank = %d, numprocs = %d", scorep_mpiprofiling_myrank, scorep_mpiprofiling_numprocs );

    /* -- allocate timepack buffers -- */
    #ifdef _WITH_PREALLOCATION_OF_TIME_PACKS
    scorep_mpiprofiling_local_time_pack   = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    scorep_mpiprofiling_remote_time_pack  = malloc( MPIPROFILER_TIMEPACK_BUFSIZE );
    scorep_mpiprofiling_remote_time_packs = malloc( scorep_mpiprofiling_numprocs * MPIPROFILER_TIMEPACK_BUFSIZE );
    if ( scorep_mpiprofiling_remote_time_packs == NULL
         || scorep_mpiprofiling_local_time_pack == NULL
         || scorep_mpiprofiling_remote_time_pack == NULL )
    {
        UTILS_FATAL( "Memory allocation for time packs failed." );
    }
    #endif // _WITH_PREALLOCATION_OF_TIME_PACKS

    scorep_mpiprofiling_initialized = 1;
}

/**
 * Finalizes MPI profiling module
 */
void
scorep_mpiprofile_finalize( void )
{
    if ( !scorep_mpiprofiling_initialized )
    {
        return;
    }
    UTILS_DEBUG_ENTRY();
    scorep_mpiprofiling_initialized = 0;
    if ( scorep_mpiprofiling_remote_time_packs_in_use )
    {
        UTILS_DEBUG( "remote_time_packs_in_use is still in use" );
    }
    if ( scorep_mpiprofiling_local_time_pack_in_use )
    {
        UTILS_DEBUG( "scorep_mpiprofiling_local_time_pack_in_use is still in use" );
    }
    if ( scorep_mpiprofiling_remote_time_pack_in_use )
    {
        UTILS_DEBUG( "scorep_mpiprofiling_remote_time_pack_in_use is still in use" );
    }
    MPI_Status statuses[ scorep_mpiprofiling_timepack_pool_size ];
    int        flag = 0;
    PMPI_Testall( scorep_mpiprofiling_timepack_pool_size, scorep_mpiprofiling_timepack_requests, &flag, statuses );
    if ( !flag )
    {
        UTILS_DEBUG( "at least one timepack buffer in the pool is busy" );
    }
    scorep_mpiprofile_free_timepack_pool();
    free( scorep_mpiprofiling_local_time_pack );
    free( scorep_mpiprofiling_remote_time_pack );
    free( scorep_mpiprofiling_remote_time_packs );
    UTILS_DEBUG_EXIT();
}
