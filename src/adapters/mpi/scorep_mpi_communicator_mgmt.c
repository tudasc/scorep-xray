/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 * @ingroup MPI_Wrapper
 */


#include <config.h>
#include "scorep_mpi_communicator_mgmt.h"

#include "SCOREP_Mpi.h"

#include <SCOREP_Mutex.h>
#include <UTILS_Error.h>
#include <SCOREP_Memory.h>

#include <inttypes.h>
#include <stdlib.h>


extern uint64_t scorep_mpi_max_windows;

/**
   Local communicator counters
 */
uint32_t scorep_mpi_number_of_self_comms = 0;
uint32_t scorep_mpi_number_of_root_comms = 0;

/**
 *  @def SCOREP_MPI_MAX_WIN
 *  @internal
 *  Maximum amount of concurrently defined windows per process
 */
#define SCOREP_MPI_MAX_WIN    scorep_mpi_max_windows

/**
 *  @internal
 *  Mutex for mpi window definitions.
 */
SCOREP_Mutex scorep_mpi_window_mutex = SCOREP_INVALID_MUTEX;

/**
 *  @internal
 *  Window tracking array
 */
struct scorep_mpi_win_type* scorep_mpi_windows = NULL;

/**
 *  @internal
 *  Data structure to track active GATS epochs.
 */
struct scorep_mpi_winacc_type* scorep_mpi_winaccs = NULL;

/**
 *  Contains the data of the MPI_COMM_WORLD definition.
 */
struct scorep_mpi_world_type scorep_mpi_world;

/**
 *  @internal
 *  Internal array used for rank translation.
 */
SCOREP_MpiRank* scorep_mpi_ranks;

/**
   MPI datatype for ID-ROOT exchange
 */
MPI_Datatype scorep_mpi_id_root_type = MPI_DATATYPE_NULL;

/**
 *  @internal
 *  Mutex for communicator definition.
 */
SCOREP_Mutex scorep_mpi_communicator_mutex = SCOREP_INVALID_MUTEX;

/**
 *  @internal
 *  Internal flag to indicate communicator initialization. It is set o non-zero if the
 *  communicator management is initialized. This happens when the function
 *  scorep_mpi_comm_init() is called.
 */
int scorep_mpi_comm_initialized = 0;
int scorep_mpi_comm_finalized   = 0;

void
scorep_mpi_win_init( void )
{
#ifndef SCOREP_MPI_NO_RMA
    SCOREP_MutexCreate( &scorep_mpi_window_mutex );

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_RMA ) )
    {
        if ( SCOREP_MPI_MAX_WIN == 0 )
        {
            UTILS_WARN_ONCE( "Environment variable SCOREP_MPI_MAX_WINDOWS was set to 0, "
                             "thus, one-sided communication cannot be recorded and is disabled. "
                             "To avoid this warning you can disable one sided communications, "
                             "by disabling RMA via SCOREP_MPI_ENABLE_GROUPS." );
            SCOREP_MPI_DISABLE_GROUP( SCOREP_MPI_ENABLED_RMA );
        }

        if ( SCOREP_MPI_MAX_WINACC == 0 )
        {
            UTILS_WARN_ONCE( "Environment variable SCOREP_MPI_MAX_ACCESS_EPOCHS was set "
                             "to 0, thus, one-sided communication cannot be recorded and is "
                             "disabled. To avoid this warning you can disable one sided "
                             "communications, by disabling RMA via SCOREP_MPI_ENABLE_GROUPS." );
            SCOREP_MPI_DISABLE_GROUP( SCOREP_MPI_ENABLED_RMA );
        }


        scorep_mpi_windows = ( struct scorep_mpi_win_type* )SCOREP_Memory_AllocForMisc
                                 ( sizeof( struct scorep_mpi_win_type ) * SCOREP_MPI_MAX_WIN );
        if ( scorep_mpi_windows == NULL )
        {
            UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                         "Failed to allocate memory for MPI window tracking.\n"
                         "One-sided communication cannot be recoreded.\n"
                         "Space for %" PRIu64 " windows was requested.\n"
                         "You can change this number via the environment variable "
                         "SCOREP_MPI_MAX_WINDOWS.", SCOREP_MPI_MAX_WIN );
            SCOREP_MPI_DISABLE_GROUP( SCOREP_MPI_ENABLED_RMA );
        }

        scorep_mpi_winaccs = ( struct scorep_mpi_winacc_type* )SCOREP_Memory_AllocForMisc
                                 ( sizeof( struct scorep_mpi_winacc_type ) * SCOREP_MPI_MAX_WINACC );

        if ( scorep_mpi_winaccs == NULL )
        {
            UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                         "Failed to allocate memory for access epoch tracking.\n"
                         "One-sided communication cannot be recoreded.\n"
                         "Space for %" PRIu64 " access epochs was requested.\n"
                         "You can change this number via environment variable "
                         "SCOREP_MPI_MAX_ACCESS_EPOCHS.",
                         SCOREP_MPI_MAX_WINACC );
            SCOREP_MPI_DISABLE_GROUP( SCOREP_MPI_ENABLED_RMA );
        }
    }
#endif
}

void
scorep_mpi_win_finalize( void )
{
#ifndef SCOREP_MPI_NO_RMA
    SCOREP_MutexDestroy( &scorep_mpi_window_mutex );
#endif
}

void
scorep_mpi_comm_finalize( void )
{
    /* reset initialization flag
     * (needed to prevent crashes with broken MPI implementations) */
    scorep_mpi_comm_initialized = 0;
    scorep_mpi_comm_finalized   = 1;

    /* free MPI group held internally */
    PMPI_Group_free( &scorep_mpi_world.group );

    /* free local translation buffers */
    free( scorep_mpi_world.ranks );
    free( scorep_mpi_ranks );

    /* free created MPI data types */
    PMPI_Type_free( &scorep_mpi_id_root_type );

    SCOREP_MutexDestroy( &scorep_mpi_communicator_mutex );
}

/* *INDENT-OFF* */
/* *INDENT-ON*  */
