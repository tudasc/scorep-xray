/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2018,
 * Technische Universitaet Dresden, Germany
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

#include "scorep_mpi_rma_request.h"

#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>

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
struct scorep_mpi_epoch_info_type* scorep_mpi_epochs = NULL;

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

/**
 *  @internal
 *  Internal flag to indicate window initialization. It is set o non-zero if the
 *  communicator management is initialized. This happens when the function
 *  scorep_mpi_win_init() is called.
 */
static int mpi_win_initialized = 0;


typedef uint32_t SCOREP_CommunicatorId;

/**
 * @brief Structure to exchange id and root value
 */
struct scorep_mpi_id_root_pair
{
    unsigned int id;      /**< identifier of communicator */
    int          root;    /**< global rank of id-providing process */
};

/**
 *  @internal
 *  Communicator tracking data structure. Array of created communicators' handles.
 */
struct scorep_mpi_communicator_type* scorep_mpi_comms = NULL;

/**
 *  @internal
 *  Group tracking data structure. Array of created groups' handles.
 */
struct scorep_mpi_group_type* scorep_mpi_groups = NULL;

/**
   Rank of local process in esd_comm_world
 */
static int scorep_mpi_my_global_rank = SCOREP_INVALID_ROOT_RANK;

/**
 *  @internal
 *  Index into the scorep_mpi_comms array to the last entry.
 */
int32_t scorep_mpi_last_comm = 0;

void
scorep_mpi_win_init( void )
{
#ifndef SCOREP_MPI_NO_RMA
    SCOREP_MutexCreate( &scorep_mpi_window_mutex );

    if ( !mpi_win_initialized )
    {
        if ( SCOREP_MPI_MAX_WIN == 0 )
        {
            UTILS_WARN_ONCE( "Environment variable SCOREP_MPI_MAX_WINDOWS was set to 0, "
                             "thus, one-sided communication cannot be recorded and is disabled. "
                             "To avoid this warning you can disable one sided communications, "
                             "by disabling RMA via SCOREP_MPI_ENABLE_GROUPS." );
            SCOREP_MPI_DISABLE_GROUP( SCOREP_MPI_ENABLED_RMA );
        }

        if ( SCOREP_MPI_MAX_EPOCHS == 0 )
        {
            UTILS_WARN_ONCE( "Environment variable SCOREP_MPI_MAX_EPOCHS was set "
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

        scorep_mpi_epochs = ( struct scorep_mpi_epoch_info_type* )SCOREP_Memory_AllocForMisc
                                ( sizeof( struct scorep_mpi_epoch_info_type ) * SCOREP_MPI_MAX_EPOCHS );

        if ( scorep_mpi_epochs == NULL )
        {
            UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                         "Failed to allocate memory for access epoch tracking.\n"
                         "One-sided communication cannot be recoreded.\n"
                         "Space for %" PRIu64 " access epochs was requested.\n"
                         "You can change this number via environment variable "
                         "SCOREP_MPI_MAX_EPOCHS.",
                         SCOREP_MPI_MAX_EPOCHS );
            SCOREP_MPI_DISABLE_GROUP( SCOREP_MPI_ENABLED_RMA );
        }

        // Initialize RMA request tracking
        scorep_mpi_rma_request_init();

        mpi_win_initialized = 1;
    }
#endif
}

void
scorep_mpi_win_finalize( void )
{
#ifndef SCOREP_MPI_NO_RMA
    SCOREP_MutexDestroy( &scorep_mpi_window_mutex );

    scorep_mpi_rma_request_finalize();

    mpi_win_initialized = 0;
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

/**
 * @brief Determine id and root for communicator
 * @param comm Communicator to be tracked
 * @param id   Id returned from rank 0
 * @param root Global rank of id-providing rank (rank 0)
 */
static void
scorep_mpi_comm_create_id( MPI_Comm               comm,
                           int                    size,
                           int                    local_rank,
                           SCOREP_MpiRank*        root,
                           SCOREP_CommunicatorId* id )
{
    struct scorep_mpi_id_root_pair pair;

    if ( size == 1 )
    {
        *id   = scorep_mpi_number_of_self_comms++;
        *root = scorep_mpi_my_global_rank;
    }
    else
    {
        pair.id   = scorep_mpi_number_of_root_comms;
        pair.root = scorep_mpi_my_global_rank;

        /* root determines the id used by all processes */
        PMPI_Bcast( &pair, 1,  scorep_mpi_id_root_type, 0, comm );
        *id   = pair.id;
        *root = pair.root;

        /* increase local communicator id counter, if this
         * process is root in the new communicator */
        if ( local_rank == 0 )
        {
            ++scorep_mpi_number_of_root_comms;
        }
    }
}

void
scorep_mpi_comm_create_finalize( MPI_Comm                         comm,
                                 SCOREP_InterimCommunicatorHandle parentCommHandle )
{
    SCOREP_CommunicatorId            id;         /* identifier unique to root */
    SCOREP_MpiRank                   root;       /* global rank of rank 0 */
    int                              local_rank; /* local rank in this communicator */
    int                              size;       /* size of communicator */
    SCOREP_InterimCommunicatorHandle handle;     /* Score-P handle for the communicator */

    /* Lock communicator definition */
    SCOREP_MutexLock( scorep_mpi_communicator_mutex );

    /* is storage available */
    if ( scorep_mpi_last_comm >= SCOREP_MPI_MAX_COMM )
    {
        SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
        UTILS_ERROR( SCOREP_ERROR_MPI_TOO_MANY_COMMS,
                     "Hint: Increase SCOREP_MPI_MAX_COMMUNICATORS "
                     "configuration variable" );
        return;
    }

    /* fill in local data */
    PMPI_Comm_rank( comm, &local_rank );
    PMPI_Comm_size( comm, &size );

    /* determine id and root for communicator definition */
    scorep_mpi_comm_create_id( comm, size, local_rank, &root, &id );

    /* create definition in measurement system */
    scorep_mpi_comm_definition_payload* comm_payload;
    handle = SCOREP_Definitions_NewInterimCommunicator( parentCommHandle,
                                                        SCOREP_PARADIGM_MPI,
                                                        sizeof( *comm_payload ),
                                                        ( void** )&comm_payload );
    comm_payload->comm_size         = size;
    comm_payload->local_rank        = local_rank;
    comm_payload->global_root_rank  = root;
    comm_payload->root_id           = id;
    comm_payload->io_handle_counter = 0;

    /* enter comm in scorep_mpi_comms[] array */
    scorep_mpi_comms[ scorep_mpi_last_comm ].comm = comm;
    scorep_mpi_comms[ scorep_mpi_last_comm ].cid  = handle;
    scorep_mpi_last_comm++;

    /* clean up */
    SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
}

void
scorep_mpi_comm_create( MPI_Comm comm, MPI_Comm parentComm )
{
    /* Check if communicator handling has been initialized.
     * Prevents crashes with broken MPI implementations (e.g. mvapich-0.9.x)
     * that use MPI_ calls instead of PMPI_ calls to create some
     * internal communicators.
     * Also applies to scorep_mpi_comm_free and scorep_mpi_group_(create|free).
     *
     * After finalization communicator tracking is not possible anymore.
     * However, if Score-P uses SION, SION will create some communicators using
     * MPI functions. In this case, we do not want to have error messages, but
     * simply ignore those communicators.
     */
    if ( !scorep_mpi_comm_initialized || scorep_mpi_comm_finalized )
    {
        if ( !scorep_mpi_comm_finalized )
        {
            UTILS_WARNING( "Skipping attempt to create communicator "
                           "outside init->finalize scope" );
        }
        return;
    }

    SCOREP_InterimCommunicatorHandle parent_handle = SCOREP_INVALID_INTERIM_COMMUNICATOR;
    int                              inter;
    PMPI_Comm_test_inter( comm, &inter );
    if ( !inter && parentComm != MPI_COMM_NULL )
    {
        /* SCOREP_MPI_COMM_HANDLE() also takes the scorep_mpi_communicator_mutex
         * mutex, thus resolve parentComm outside of the comm mutex
         */
        parent_handle = SCOREP_MPI_COMM_HANDLE( parentComm );
    }

    scorep_mpi_comm_create_finalize( comm, parent_handle );
}

/* no static, because this is called from the mpi unification test */
void
scorep_mpi_setup_world( void )
{
    assert( scorep_mpi_comm_initialized == 0 );

    int                            i;
    MPI_Datatype                   types[ 2 ]   = { MPI_UNSIGNED, MPI_INT };
    int                            lengths[ 2 ] = { 1, 1 };
    MPI_Aint                       disp[ 2 ];
    struct scorep_mpi_id_root_pair pair;

    /* get group of \a MPI_COMM_WORLD */
    PMPI_Comm_group( MPI_COMM_WORLD, &scorep_mpi_world.group );

    /* determine the number of MPI processes */
    PMPI_Group_size( scorep_mpi_world.group, &scorep_mpi_world.size );

    /* initialize translation data structure for \a MPI_COMM_WORLD */
    scorep_mpi_world.ranks = calloc( scorep_mpi_world.size,
                                     sizeof( SCOREP_MpiRank ) );
    assert( scorep_mpi_world.ranks );
    for ( i = 0; i < scorep_mpi_world.size; i++ )
    {
        scorep_mpi_world.ranks[ i ] = i;
    }

    /*
     * Define the list of locations which are MPI ranks.
     *
     * If we support MPI_THREAD_FUNNELED, this needs to be the
     * location, which has called MPI_Init/MPI_Thread_init.
     * For the moment, the location and rank ids match.
     *
     * This needs to be called early, so that the resulting definition
     * is before any other group definition of type SCOREP_GROUP_MPI_GROUP.
     */
    SCOREP_Definitions_NewGroupFrom32( SCOREP_GROUP_MPI_LOCATIONS,
                                       "",
                                       scorep_mpi_world.size,
                                       ( const uint32_t* )scorep_mpi_world.ranks );

    /* allocate translation buffers */
    scorep_mpi_ranks = calloc( scorep_mpi_world.size,
                               sizeof( SCOREP_MpiRank ) );
    assert( scorep_mpi_ranks );

    /* create a derived datatype for distributed communicator
     * definition handling */
#if HAVE( DECL_PMPI_GET_ADDRESS )
    PMPI_Get_address( &pair.id, &( disp[ 0 ] ) );
    PMPI_Get_address( &pair.root, &( disp[ 1 ] ) );
#else // !HAVE( DECL_PMPI_GET_ADDRESS )
    PMPI_Address( &pair.id, &( disp[ 0 ] ) );
    PMPI_Address( &pair.root, &( disp[ 1 ] ) );
#endif // !HAVE( DECL_PMPI_GET_ADDRESS )
    for ( i = 1; i >= 0; --i )
    {
        disp[ i ] -= disp[ 0 ];
    }

#if HAVE( DECL_PMPI_TYPE_CREATE_STRUCT )
    PMPI_Type_create_struct( 2, lengths, disp, types, &scorep_mpi_id_root_type );
#else // !HAVE( DECL_PMPI_TYPE_CREATE_STRUCT )
    PMPI_Type_struct( 2, lengths, disp, types, &scorep_mpi_id_root_type );
#endif // !HAVE( DECL_PMPI_TYPE_CREATE_STRUCT )
    PMPI_Type_commit( &scorep_mpi_id_root_type );

    /* initialize global rank variable */
    PMPI_Comm_rank( MPI_COMM_WORLD, &scorep_mpi_my_global_rank );

    /* initialize MPI_COMM_WORLD */
    scorep_mpi_comm_definition_payload* comm_payload;
    scorep_mpi_world.handle =
        SCOREP_Definitions_NewInterimCommunicator( SCOREP_INVALID_INTERIM_COMMUNICATOR,
                                                   SCOREP_PARADIGM_MPI,
                                                   sizeof( *comm_payload ),
                                                   ( void** )&comm_payload );
    comm_payload->comm_size         = scorep_mpi_world.size;
    comm_payload->local_rank        = scorep_mpi_my_global_rank;
    comm_payload->global_root_rank  = 0;
    comm_payload->root_id           = 0;
    comm_payload->io_handle_counter = 0;

    if ( scorep_mpi_my_global_rank == 0 )
    {
        if ( scorep_mpi_world.size > 1 )
        {
            scorep_mpi_number_of_root_comms++;
        }
        else
        {
            scorep_mpi_number_of_self_comms++;
        }
    }
}

void
scorep_mpi_comm_init( void )
{
    SCOREP_MutexCreate( &scorep_mpi_communicator_mutex );

    /* check, if we already initialized the data structures */
    if ( !scorep_mpi_comm_initialized )
    {
        /* Create tracking structures */
        scorep_mpi_comms = ( struct scorep_mpi_communicator_type* )SCOREP_Memory_AllocForMisc
                               ( sizeof( struct scorep_mpi_communicator_type ) * SCOREP_MPI_MAX_COMM );

        if ( scorep_mpi_comms == NULL )
        {
            UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                         "Failed to allocate memory for communicator tracking.\n"
                         "Space for %" PRIu64 " communicators was requested.\n"
                         "You can change this number via the environment variable "
                         "SCOREP_MPI_MAX_COMMUNICATORS.", SCOREP_MPI_MAX_COMM );
        }

        scorep_mpi_groups = ( struct scorep_mpi_group_type* )SCOREP_Memory_AllocForMisc
                                ( sizeof( struct scorep_mpi_group_type ) *  SCOREP_MPI_MAX_GROUP );
        if ( scorep_mpi_groups == NULL )
        {
            UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                         "Failed to allocate memory for MPI group tracking.\n"
                         "Space for %" PRIu64 " groups was requested.\n"
                         "You can change this number via the environment variable "
                         "SCOREP_MPI_MAX_GROUPS.", SCOREP_MPI_MAX_GROUP );
        }

        scorep_mpi_setup_world();

        /* The initialization is done, flag that */
        scorep_mpi_comm_initialized = 1;

        /* create MPI_COMM_SELF definition */
        scorep_mpi_comm_create( MPI_COMM_SELF, MPI_COMM_NULL );
    }
    else
    {
        UTILS_WARNING( "Duplicate call to communicator initialization ignored!" );
    }
}

SCOREP_InterimCommunicatorHandle
scorep_mpi_comm_handle( MPI_Comm comm )
{
    int i = 0;

    /* Lock communicator definition */
    SCOREP_MutexLock( scorep_mpi_communicator_mutex );

    while ( i < scorep_mpi_last_comm && scorep_mpi_comms[ i ].comm != comm )
    {
        i++;
    }

    if ( i != scorep_mpi_last_comm )
    {
        /* Unlock communicator definition */
        SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );

        return scorep_mpi_comms[ i ].cid;
    }
    else
    {
        /* Unlock communicator definition */
        SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );

        if ( comm == MPI_COMM_WORLD )
        {
            UTILS_WARNING( "This function SHOULD NOT be called with MPI_COMM_WORLD" );
            return SCOREP_MPI_COMM_WORLD_HANDLE;
        }
        else
        {
            UTILS_ERROR( SCOREP_ERROR_MPI_NO_COMM,
                         "You are using a communicator that was "
                         "not tracked. Please contact the Score-P support team." );
            return SCOREP_INVALID_INTERIM_COMMUNICATOR;
        }
    }
}

/* *INDENT-OFF* */
/* *INDENT-ON*  */
