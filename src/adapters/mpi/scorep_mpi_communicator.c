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
 * @file       scorep_mpi_communicator.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    MPI_Wrapper
 *
 * @brief Internal functions for communicator, group and window management.
 */

#include <config.h>
#include <scorep_mpi_communicator.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Mpi.h>

#include <UTILS_Error.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

/*
 * ----------------------------------------------------------------------------
 *
 * External declarations
 *
 * ----------------------------------------------------------------------------
 */
extern uint64_t scorep_mpi_max_communicators;
extern uint64_t scorep_mpi_max_windows;
extern uint64_t scorep_mpi_max_access_epochs;
extern uint64_t scorep_mpi_max_groups;

/*
 * ----------------------------------------------------------------------------
 *
 * Internal definitions
 *
 * ----------------------------------------------------------------------------
 */

/**
 *  @def SCOREP_MPI_MAX_COMM
 *  @internal
 *  Maximum amount of concurrently defined communicators per process.
 */
#define SCOREP_MPI_MAX_COMM    scorep_mpi_max_communicators

/**
 *  @def SCOREP_MPI_MAX_GROUP
 *  @internal
 *  Maximum amount of concurrently defined groups per process.
 */
#define SCOREP_MPI_MAX_GROUP   scorep_mpi_max_groups

/**
 *  @def SCOREP_MPI_MAX_WIN
 *  @internal
 *  Maximum amount of concurrently defined windows per process
 */
#define SCOREP_MPI_MAX_WIN    scorep_mpi_max_windows

/**
 *  @def SCOREP_MPI_MAX_WINACC
 *  @internal
 *  Maximum amount of concurrently active access or exposure epochs per
 *  process.
 */
#define SCOREP_MPI_MAX_WINACC  scorep_mpi_max_access_epochs

/**
 *  Contains the data of the MPI_COMM_WORLD definition.
 */
struct scorep_mpi_world_type scorep_mpi_world;

/* ------------------------------------------------ Definitions for MPI Window handling */
#ifndef SCOREP_MPI_NO_RMA

/**
 *  @internal
 *  Structure to translate MPI window handles to internal SCOREP IDs.
 */
struct scorep_mpi_win_type
{
    MPI_Win                win; /** MPI window handle */
    SCOREP_RmaWindowHandle wid; /** Internal SCOREP window handle */
};

/**
 *  @internal
 *  Index in the scorep_mpi_windows array of last valid entry in window tracking data
 *  structure.
 */
static int32_t scorep_mpi_last_window = 0;

/**
 *  @internal
 *  Window tracking array
 */
static struct scorep_mpi_win_type* scorep_mpi_windows = NULL;

/**
 *  @internal
 *  Mutex for mpi window definitions.
 */
static SCOREP_Mutex scorep_mpi_window_mutex;

#endif // SCOREP_MPI_NO_RMA

/* ------------------------------------------- Definitions for communicators and groups */

typedef uint32_t SCOREP_CommunicatorId;

/**
 *  @internal
 * structure for communicator tracking
 */
struct scorep_mpi_communicator_type
{
    MPI_Comm                          comm; /**< MPI Communicator handle */
    SCOREP_LocalMPICommunicatorHandle cid;  /**< Internal SCOREP Communicator handle */
};

/**
 * @internal
 * structure for group tracking
 */
struct scorep_mpi_group_type
{
    MPI_Group              group;  /**< MPI group handle */
    SCOREP_Mpi_GroupHandle gid;    /**< Internal SCOREP group handle */
    int32_t                refcnt; /**< Number of references to this group */
};

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
 *  Index into the scorep_mpi_comms array to the last entry.
 */
static int32_t scorep_mpi_last_comm = 0;

/**
 *  @internal
 *  Index into the scorep_mpi_groups array to the last entry.
 */
static int32_t scorep_mpi_last_group = 0;

/**
 *  @internal
 *  Communicator tracking data structure. Array of created communicators' handles.
 */
static struct scorep_mpi_communicator_type* scorep_mpi_comms = NULL;

/**
 *  @internal
 *  Group tracking data structure. Array of created groups' handles.
 */
static struct scorep_mpi_group_type* scorep_mpi_groups = NULL;

/**
 *  @internal
 *  Internal array used for rank translation.
 */
static SCOREP_MpiRank* scorep_mpi_ranks;

/**
 *  @internal
 *  Internal flag to indicate communicator initialization. It is set o non-zero if the
 *  communicator management is initialized. This happens when the function
 *  scorep_mpi_comm_init() is called.
 */
static int scorep_mpi_comm_initialized = 0;
static int scorep_mpi_comm_finalized   = 0;

/**
 *  @internal
 *  Mutex for communicator definition.
 */
static SCOREP_Mutex scorep_mpi_communicator_mutex;

/**
   MPI datatype for ID-ROOT exchange
 */
static MPI_Datatype scorep_mpi_id_root_type = MPI_DATATYPE_NULL;

/**
   Rank of local process in esd_comm_world
 */
static int scorep_mpi_my_global_rank = SCOREP_INVALID_ROOT_RANK;

/**
   Local communicator counters
 */
uint32_t scorep_mpi_number_of_self_comms = 0;
uint32_t scorep_mpi_number_of_root_comms = 0;

/* ------------------------------------------------ Definition for one sided operations */
#ifndef SCOREP_MPI_NO_RMA

/**
 *  @internal exposure epoch
 */
const SCOREP_Mpi_Color scorep_mpi_exp_epoch = 0;

/**
 *  @internal access epoch
 */
const SCOREP_Mpi_Color scorep_mpi_acc_epoch = 1;

/**
 * @internal
 *  Entry data structure to track GATS epochs
 */
struct scorep_mpi_winacc_type
{
    MPI_Win                win;   /* MPI window identifier */
    SCOREP_Mpi_GroupHandle gid;   /* SCOREP MPI group handle */
    SCOREP_Mpi_Color       color; /* byte to help distinguish accesses on same window */
};

/**
 *  @internal
 *  Data structure to track active GATS epochs.
 */
static struct scorep_mpi_winacc_type* scorep_mpi_winaccs = NULL;

/**
 *  @internal
 *  Index of last valid entry in the scorep_mpi_winaccs array.
 */
static int scorep_mpi_last_winacc = 0;

#endif // SCOREP_MPI_NO_RMA

/*
 * -----------------------------------------------------------------------------
 *
 * Communicator management
 *
 * -----------------------------------------------------------------------------
 */

/* -- rank translation -- */

SCOREP_MpiRank
scorep_mpi_rank_to_pe( SCOREP_MpiRank rank,
                       MPI_Comm       comm )
{
    MPI_Group      group;
    SCOREP_MpiRank global_rank;
    int32_t        inter;

    /* inter-communicators need different call than intra-communicators */
    PMPI_Comm_test_inter( comm, &inter );
    if ( inter )
    {
        PMPI_Comm_remote_group( comm, &group );
    }
    else
    {
        PMPI_Comm_group( comm, &group );
    }

    /* translate rank with respect to \a MPI_COMM_WORLD */
    PMPI_Group_translate_ranks( group, 1, &rank, scorep_mpi_world.group, &global_rank );
    /* free internal group of input communicator */
    PMPI_Group_free( &group );

    return global_rank;
}

/* -------------------------------------------------------------------- window handling */
void
scorep_mpi_win_init( void )
{
#ifndef SCOREP_MPI_NO_RMA
    SCOREP_MutexCreate( &scorep_mpi_window_mutex );

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_RMA ) )
    {
        if ( SCOREP_MPI_MAX_WIN == 0 )
        {
            fprintf( stderr, "Environment variable SCOREP_MPI_MAX_WINDOWS was set to 0, "
                     "thus, one-sided communication can not be recorded and is disabled. "
                     "To avoid this warning you can disable one sided communications, "
                     "by disabling RMA via SCOREP_MPI_ENABLE_GROUPS." );
            SCOREP_MPI_DISABLE_GROUP( SCOREP_MPI_ENABLED_RMA );
        }

        if ( SCOREP_MPI_MAX_WINACC == 0 )
        {
            fprintf( stderr, "Environment variable SCOREP_MPI_MAX_ACCESS_EPOCHS was set "
                     "to 0, thus, one-sided communication can not be recorded and is "
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
                         "One-sided communication can not be recoreded.\n"
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
                         "One-sided communication can not be recoreded.\n"
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

#ifndef SCOREP_MPI_NO_RMA

SCOREP_MpiRank
scorep_mpi_win_rank_to_pe( SCOREP_MpiRank rank,
                           MPI_Win        win )
{
    MPI_Group      group;
    SCOREP_MpiRank global_rank;

    /* get group of communicator associated with input window */
    PMPI_Win_get_group( win, &group );
    /* translate rank with respect to \a MPI_COMM_WORLD */
    PMPI_Group_translate_ranks( group, 1, &rank, scorep_mpi_world.group, &global_rank );
    /* free internal group of input window */
    PMPI_Group_free( &group );

    return global_rank;
}

SCOREP_RmaWindowHandle
scorep_mpi_win_id( MPI_Win win )
{
    int i = 0;

    SCOREP_MutexLock( scorep_mpi_window_mutex );
    while ( i < scorep_mpi_last_window && scorep_mpi_windows[ i ].win != win )
    {
        i++;
    }

    if ( i <= scorep_mpi_last_window )
    {
        SCOREP_MutexUnlock( scorep_mpi_window_mutex );
        return scorep_mpi_windows[ i ].wid;
    }
    else
    {
        SCOREP_MutexUnlock( scorep_mpi_window_mutex );
        UTILS_ERROR( SCOREP_ERROR_MPI_NO_WINDOW,
                     "Please tell me what you were trying to do!" );
        return SCOREP_INVALID_RMA_WINDOW;
    }
}

void
scorep_mpi_win_create( MPI_Win  win,
                       MPI_Comm comm )
{
    SCOREP_RmaWindowHandle handle = SCOREP_INVALID_RMA_WINDOW;

    SCOREP_MutexLock( scorep_mpi_window_mutex );
    if ( scorep_mpi_last_window >= SCOREP_MPI_MAX_WIN )
    {
        UTILS_ERROR( SCOREP_ERROR_MPI_TOO_MANY_WINDOWS,
                     "Hint: Increase SCOREP_MPI_MAX_WINDOWS configuration variable." );
    }

    /* register mpi window definition */
    /* NOTE: MPI_COMM_WORLD is _not_ present in the internal structures,
     * and _must not_ be queried by scorep_mpi_comm_handle */
    handle = SCOREP_DefineRmaWindow( "",
                                     comm == MPI_COMM_WORLD ? SCOREP_MPI_COMM_WORLD_HANDLE : scorep_mpi_comm_handle( comm ) );

    /* enter win in scorep_mpi_windows[] array */
    scorep_mpi_windows[ scorep_mpi_last_window ].win = win;
    scorep_mpi_windows[ scorep_mpi_last_window ].wid = handle;

    scorep_mpi_last_window++;
    SCOREP_MutexUnlock( scorep_mpi_window_mutex );
}

void
scorep_mpi_win_free( MPI_Win win )
{
    SCOREP_MutexLock( scorep_mpi_window_mutex );
    if ( scorep_mpi_last_window == 1 && scorep_mpi_windows[ 0 ].win == win )
    {
        scorep_mpi_last_window = 0;
    }
    else if ( scorep_mpi_last_window > 1 )
    {
        int i = 0;

        while ( i < scorep_mpi_last_window && scorep_mpi_windows[ i ].win != win )
        {
            i++;
        }

        if ( i < scorep_mpi_last_window-- )
        {
            scorep_mpi_windows[ i ] = scorep_mpi_windows[ scorep_mpi_last_window ];
        }
        else
        {
            UTILS_ERROR( SCOREP_ERROR_MPI_NO_WINDOW, "" );
        }
    }
    else
    {
        UTILS_ERROR( SCOREP_ERROR_MPI_NO_WINDOW, "" );
    }
    SCOREP_MutexUnlock( scorep_mpi_window_mutex );
}
#endif

/* -------------------------------------------------------------- communicator handling */

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

    /* allocate translation buffers */
    scorep_mpi_ranks = calloc( scorep_mpi_world.size,
                               sizeof( SCOREP_MpiRank ) );
    assert( scorep_mpi_ranks );

    /* create a derived datatype for distributed communicator
     * definition handling */
#if HAVE( MPI_GET_ADDRESS )
    PMPI_Get_address( &pair.id, &( disp[ 0 ] ) );
    PMPI_Get_address( &pair.root, &( disp[ 1 ] ) );
#else
    PMPI_Address( &pair.id, &( disp[ 0 ] ) );
    PMPI_Address( &pair.root, &( disp[ 1 ] ) );
#endif
    for ( i = 1; i >= 0; --i )
    {
        disp[ i ] -= disp[ 0 ];
    }

#if HAVE( MPI_TYPE_CREATE_STRUCT )
    PMPI_Type_create_struct( 2, lengths, disp, types, &scorep_mpi_id_root_type );
#else
    PMPI_Type_struct( 2, lengths, disp, types, &scorep_mpi_id_root_type );
#endif
    PMPI_Type_commit( &scorep_mpi_id_root_type );

    /* initialize global rank variable */
    PMPI_Comm_rank( MPI_COMM_WORLD, &scorep_mpi_my_global_rank );

    /* initialize MPI_COMM_WORLD */
    scorep_mpi_comm_definition_payload* comm_payload;
    scorep_mpi_world.handle =
        SCOREP_DefineLocalMPICommunicator( SCOREP_INVALID_LOCAL_MPI_COMMUNICATOR,
                                           SCOREP_ADAPTER_MPI,
                                           sizeof( *comm_payload ),
                                           ( void** )&comm_payload );
    comm_payload->is_self_like     = scorep_mpi_world.size == 1;
    comm_payload->local_rank       = scorep_mpi_my_global_rank;
    comm_payload->global_root_rank = 0;
    comm_payload->root_id          = 0;

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

int32_t
scorep_mpi_group_translate_ranks( MPI_Group group )
{
    int32_t size;

    /*
     * Determine the world rank of each process in group.
     *
     * Parameter #3 is world.ranks here, as we need an array of integers
     * initialized with 0 to n-1, which world.ranks happens to be.
     */
    PMPI_Group_size( group, &size );
    PMPI_Group_translate_ranks( group,
                                size,
                                scorep_mpi_world.ranks,
                                scorep_mpi_world.group,
                                scorep_mpi_ranks );

    return size;
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
scorep_mpi_comm_create( MPI_Comm comm, MPI_Comm parent_comm )
{
    SCOREP_CommunicatorId             id;         /* identifier unique to root */
    SCOREP_MpiRank                    root;       /* global rank of rank 0 */
    int                               local_rank; /* local rank in this communicator */
    int                               size;       /* size of communicator */
    SCOREP_LocalMPICommunicatorHandle handle;     /* Score-P handle for the communicator */

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

    SCOREP_LocalMPICommunicatorHandle parent_handle = SCOREP_INVALID_LOCAL_MPI_COMMUNICATOR;
    int                               inter;
    PMPI_Comm_test_inter( comm, &inter );
    if ( !inter && parent_comm != MPI_COMM_NULL )
    {
        /* SCOREP_MPI_COMM_HANDLE() also takes the scorep_mpi_communicator_mutex
         * mutex, thus resolve parent_comm outside of the comm mutex
         */
        parent_handle = SCOREP_MPI_COMM_HANDLE( parent_comm );
    }

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
    handle =  SCOREP_DefineLocalMPICommunicator( parent_handle,
                                                 SCOREP_ADAPTER_MPI,
                                                 sizeof( *comm_payload ),
                                                 ( void** )&comm_payload );
    comm_payload->is_self_like     = size == 1;
    comm_payload->local_rank       = local_rank;
    comm_payload->global_root_rank = root;
    comm_payload->root_id          = id;

    /* enter comm in scorep_mpi_comms[] array */
    scorep_mpi_comms[ scorep_mpi_last_comm ].comm = comm;
    scorep_mpi_comms[ scorep_mpi_last_comm ].cid  = handle;
    scorep_mpi_last_comm++;

    /* clean up */
    SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
}

void
scorep_mpi_comm_free( MPI_Comm comm )
{
    const char* message = "You are trying to free a communicator that was "
                          "not tracked. Maybe you used a non-standard "
                          "MPI function call to create it.";

    /* check if comm handling is initialized (see scorep_mpi_comm_create comment) */
    if ( !scorep_mpi_comm_initialized )
    {
        UTILS_WARNING( "Skipping attempt to free communicator "
                       "outside init->finalize scope" );
        return;
    }

    /* Lock communicator definition */
    SCOREP_MutexLock( scorep_mpi_communicator_mutex );

    /* if only one communicator exists, we just need to decrease \a
     * scorep_mpi_last_comm */
    if ( scorep_mpi_last_comm == 1 && scorep_mpi_comms[ 0 ].comm == comm )
    {
        scorep_mpi_last_comm = 0;
    }
    /* if more than one communicator exists, we need to search for the
     * entry */
    else if ( scorep_mpi_last_comm > 1 )
    {
        int i = 0;

        while ( i < scorep_mpi_last_comm && scorep_mpi_comms[ i ].comm != comm )
        {
            i++;
        }

        if ( i < scorep_mpi_last_comm-- )
        {
            /* swap deletion candidate with last entry in the list */
            scorep_mpi_comms[ i ] = scorep_mpi_comms[ scorep_mpi_last_comm ];
        }
        else
        {
            UTILS_ERROR( SCOREP_ERROR_MPI_NO_COMM, "scorep_mpi_comm_free1 %s", message );
        }
    }
    else
    {
        UTILS_ERROR( SCOREP_ERROR_MPI_NO_COMM, "scorep_mpi_comm_free2 %s", message );
    }

    /* Unlock communicator definition */
    SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
}

SCOREP_LocalMPICommunicatorHandle
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
            return SCOREP_INVALID_LOCAL_MPI_COMMUNICATOR;
        }
    }
}

void
scorep_mpi_comm_set_name( MPI_Comm comm, const char* name )
{
    if ( !name )
    {
        return;
    }

    SCOREP_LocalMPICommunicatorHandle   comm_handle  = SCOREP_MPI_COMM_HANDLE( comm );
    scorep_mpi_comm_definition_payload* comm_payload =
        SCOREP_LocalMPICommunicatorGetPayload( comm_handle );

    SCOREP_MutexLock( scorep_mpi_communicator_mutex );

    /*
     * Set the name only for the root rank in the communicator and
     * for non-SELF-like communicators
     */
    if ( 0 == comm_payload->local_rank && !comm_payload->is_self_like )
    {
        /*
         * Does set the name only the first time
         */
        SCOREP_LocalMPICommunicatorSetName( comm_handle, name );
    }

    SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
}

/*
 * -----------------------------------------------------------------------------
 *
 * Group management
 *
 * -----------------------------------------------------------------------------
 */

void
scorep_mpi_group_create( MPI_Group group )
{
    int32_t            i;
    SCOREP_GroupHandle handle;

    /* Check if communicator handling has been initialized.
     * Prevents crashes with broken MPI implementations (e.g. mvapich-0.9.x)
     * that use MPI_ calls instead of PMPI_ calls to create some
     * internal communicators.
     */
    if ( !scorep_mpi_comm_initialized )
    {
        UTILS_WARNING( "Skipping attempt to create group "
                       "outside init->finalize scope" );
        return;
    }

    /* Lock communicator definition */
    SCOREP_MutexLock( scorep_mpi_communicator_mutex );

    if ( scorep_mpi_last_group >= SCOREP_MPI_MAX_GROUP )
    {
        UTILS_ERROR( SCOREP_ERROR_MPI_TOO_MANY_GROUPS,
                     "Hint: Increase SCOREP_MPI_MAX_GROUPS configuration variable." );
        return;
    }

    /* check if group already exists */
    if ( ( i = scorep_mpi_group_search( group ) ) == -1 )
    {
        /* create group entry in scorep_mpi_ranks */
        int32_t size = scorep_mpi_group_translate_ranks( group );

        /* register mpi group definition (as communicator) */
        handle = SCOREP_DefineMPIGroup( size, scorep_mpi_ranks );

        /* enter group in scorep_mpi_groups[] array */
        scorep_mpi_groups[ scorep_mpi_last_group ].group  = group;
        scorep_mpi_groups[ scorep_mpi_last_group ].gid    = handle;
        scorep_mpi_groups[ scorep_mpi_last_group ].refcnt = 1;
        scorep_mpi_last_group++;
    }
    else
    {
        /* count additional reference on group */
        scorep_mpi_groups[ i ].refcnt++;
    }

    /* Unlock communicator definition */
    SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
}

void
scorep_mpi_group_free( MPI_Group group )
{
    /* Check if communicator handling has been initialized.
     * Prevents crashes with broken MPI implementations (e.g. mvapich-0.9.x)
     * that use MPI_ calls instead of PMPI_ calls to create some
     * internal communicators.
     */
    if ( !scorep_mpi_comm_initialized )
    {
        UTILS_WARNING( "Skipping attempt to free group "
                       "outside init->finalize scope" );
        return;
    }

    SCOREP_MutexLock( scorep_mpi_communicator_mutex );

    if ( scorep_mpi_last_group == 1 && scorep_mpi_groups[ 0 ].group == group )
    {
        scorep_mpi_groups[ 0 ].refcnt--;

        if ( scorep_mpi_groups[ 0 ].refcnt == 0 )
        {
            scorep_mpi_last_group--;
        }
    }
    else if ( scorep_mpi_last_group > 1 )
    {
        int32_t i;

        if ( ( i = scorep_mpi_group_search( group ) ) != -1 )
        {
            /* decrease reference count on entry */
            scorep_mpi_groups[ i ].refcnt--;

            /* check if entry can be deleted */
            if ( scorep_mpi_groups[ i ].refcnt == 0 )
            {
                scorep_mpi_groups[ i ] = scorep_mpi_groups[ --scorep_mpi_last_group ];
            }
        }
        else
        {
            UTILS_ERROR( SCOREP_ERROR_MPI_NO_GROUP, "" );
        }
    }
    else
    {
        UTILS_ERROR( SCOREP_ERROR_MPI_NO_GROUP, "" );
    }

    SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
}

SCOREP_Mpi_GroupHandle
scorep_mpi_group_id( MPI_Group group )
{
    int32_t i = 0;

    SCOREP_MutexLock( scorep_mpi_communicator_mutex );
    while ( ( i < scorep_mpi_last_group ) && ( scorep_mpi_groups[ i ].group != group ) )
    {
        i++;
    }

    if ( i != scorep_mpi_last_group )
    {
        SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
        return scorep_mpi_groups[ i ].gid;
    }
    else
    {
        SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
        UTILS_ERROR( SCOREP_ERROR_MPI_NO_GROUP, "" );
        return SCOREP_INVALID_MPI_GROUP;
    }
}

int32_t
scorep_mpi_group_search( MPI_Group group )
{
    int32_t i = 0;

    SCOREP_MutexLock( scorep_mpi_communicator_mutex );
    while ( ( i < scorep_mpi_last_group ) && ( scorep_mpi_groups[ i ].group != group ) )
    {
        i++;
    }

    if ( i != scorep_mpi_last_group )
    {
        SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
        return i;
    }
    else
    {
        SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
        return -1;
    }
}

void
scorep_mpi_unify_define_mpi_locations( void )
{
    if ( scorep_mpi_my_global_rank != 0 )
    {
        return;
    }

    /*
     * Define the list of locations which are MPI ranks.
     *
     * If we support MPI_THREADED_FUNNELED, this needs to be the
     * location, wich has called MPI_Init/MPI_Thread_init.
     * For the moment, the location and rank ids match.
     *
     * This needs to be called early, so that the resulting definition
     * is before any other group definition of type SCOREP_GROUP_MPI_GROUP.
     */
    SCOREP_DefineUnifiedMPIGroup( SCOREP_GROUP_MPI_LOCATIONS,
                                  scorep_mpi_world.size,
                                  scorep_mpi_world.ranks );
}

/*
 * -----------------------------------------------------------------------------
 *
 * Window Access Groups -- which window is accessed by what group
 *
 * -----------------------------------------------------------------------------
 */

#ifndef SCOREP_MPI_NO_RMA

void
scorep_mpi_winacc_start( MPI_Win          win,
                         MPI_Group        group,
                         SCOREP_Mpi_Color color )
{
    if ( scorep_mpi_last_winacc >= SCOREP_MPI_MAX_WINACC )
    {
        UTILS_ERROR( SCOREP_ERROR_MPI_TOO_MANY_WINACCS,
                     "Hint: Increase SCOREP_MPI_MAX_ACCESS_EPOCHS "
                     "configuration variable." );
    }

    scorep_mpi_winaccs[ scorep_mpi_last_winacc ].win   = win;
    scorep_mpi_winaccs[ scorep_mpi_last_winacc ].gid   = scorep_mpi_group_id( group );
    scorep_mpi_winaccs[ scorep_mpi_last_winacc ].color = color;

    scorep_mpi_last_winacc++;
}

void
scorep_mpi_winacc_end( MPI_Win          win,
                       SCOREP_Mpi_Color color )
{
    int i = 0;
    /* only one window inside wingrp */
    if ( scorep_mpi_last_winacc == 1
         && scorep_mpi_winaccs[ 0 ].win   == win
         && scorep_mpi_winaccs[ 0 ].color == color )
    {
        scorep_mpi_last_winacc--;
    }
    else
    {
        while ( ( i <= scorep_mpi_last_winacc ) &&
                ( ( scorep_mpi_winaccs[ i ].win != win ) || ( scorep_mpi_winaccs[ i ].color != color ) ) )
        {
            i++;
        }

        if ( i != scorep_mpi_last_winacc )
        {
            scorep_mpi_last_winacc--;
            scorep_mpi_winaccs[ i ].win   = scorep_mpi_winaccs[ scorep_mpi_last_winacc ].win;
            scorep_mpi_winaccs[ i ].gid   = scorep_mpi_winaccs[ scorep_mpi_last_winacc ].gid;
            scorep_mpi_winaccs[ i ].color = scorep_mpi_winaccs[ scorep_mpi_last_winacc ].color;
        }
        else
        {
            UTILS_ERROR( SCOREP_ERROR_MPI_NO_WINACC, "" );
        }
    }
}

SCOREP_Mpi_GroupHandle
scorep_mpi_winacc_get_gid( MPI_Win          win,
                           SCOREP_Mpi_Color color )
{
    int i = 0;

    while ( ( i <= scorep_mpi_last_winacc ) &&
            ( ( scorep_mpi_winaccs[ i ].win != win ) || ( scorep_mpi_winaccs[ i ].color != color ) ) )
    {
        i++;
    }

    if ( i != scorep_mpi_last_winacc )
    {
        return scorep_mpi_winaccs[ i ].gid;
    }
    else
    {
        UTILS_ERROR( SCOREP_ERROR_MPI_NO_WINACC, "" );
        return SCOREP_INVALID_MPI_GROUP;
    }
}
#endif
