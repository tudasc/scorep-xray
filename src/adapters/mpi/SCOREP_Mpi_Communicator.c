/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SCOREP_Mpi_Communicator.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    MPI_Wrapper
 *
 * @brief Internal funcions for communicator, group and window management.
 */

#include <config.h>
#include <stdlib.h>
#include <string.h>

#include "SCOREP_Mpi_Communicator.h"
#include "scorep_utility/SCOREP_Error.h"
#include "scorep_utility/SCOREP_Debug.h"
#include "SCOREP_Definitions.h"
#include "SCOREP_DefinitionLocking.h"

/*
 *-----------------------------------------------------------------------------
 *
 * Internal definitions
 *
 *-----------------------------------------------------------------------------
 */

/**
 *  @def SCOREP_MPI_MAX_COMM
 *  @internal
 *  Maximum amount of concurrently defined communicators per process.
 */
#define SCOREP_MPI_MAX_COMM    50

/**
 *  @def SCOREP_MPI_MAX_GROUP
 *  @internal
 *  Maximum amount of concurrently defines groups per process.
 */
#define SCOREP_MPI_MAX_GROUP   50

/**
 *  @def SCOREP_MPI_MAX_WIN
 *  @internal
 *  Maximum amount of concurrently defined windows per process
 */
#define SCOREP_MPI_MAX_WIN     50

/**
 *  @def SCOREP_MPI_MAX_WINACC
 *  @internal
 *  Maximum amount of concurrently active access or exposure epochs per
 *  process.
 */
#define SCOREP_MPI_MAX_WINACC  50

/**
 *  Contains the data of the MPI_COMM_WORLD definition.
 */
struct scorep_mpi_world_type scorep_mpi_world;

/**
 *  A switch if global ranks are calculated or not. If set to zero no global ranks are
 *  calculated. If set to non-zero global values are calculated. The current default
 *  are global ranks.
 */
int8_t scorep_mpi_comm_determination = 1;

/* ------------------------------------------------ Definitions for MPI Window handling */
#ifndef SCOREP_MPI_NO_RMA

/**
 *  @internal
 *  Structure to translate MPI window handles to internal SCOREP IDs.
 */
struct scorep_mpi_win_type
{
    MPI_Win                win; /** MPI window handle */
    SCOREP_MPIWindowHandle wid; /** Internal SCOREP window handle */
};

/**
 *  @internal
 *  Index in the scorep_mpi_windows array of last valid entry in window tracking data
 *  structure.
 */
static int32_t scorep_mpi_last_window = 0;

/**
 *  @internal
 *  Window tracking array */
static struct scorep_mpi_win_type scorep_mpi_windows[ SCOREP_MPI_MAX_WIN ];

#endif // SCOREP_MPI_NO_RMA

/* ------------------------------------------- Definitions for communicators and groups */

/**
 *  @internal
 * structure for communicator tracking
 */
struct scorep_mpi_communicator_type
{
    MPI_Comm                     comm; /** MPI Communicator handle */
    SCOREP_MPICommunicatorHandle cid;  /** Internal SCOREP Communicator handle */
};

/**
 * @internal
 * structure for group tracking
 */
struct scorep_mpi_group_type
{
    MPI_Group              group;  /** MPI group handle */
    SCOREP_Mpi_GroupHandle gid;    /** Internal SCOREP group handle */
    int32_t                refcnt; /** Number of references to this group */
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
static struct scorep_mpi_communicator_type scorep_mpi_comms[ SCOREP_MPI_MAX_COMM ];

/**
 *  @internal
 *  Group tracking data structure. Array of created groups' handles.
 */
static struct scorep_mpi_group_type scorep_mpi_groups[ SCOREP_MPI_MAX_GROUP ];

/**
 *  @internal
 *  Internal array used for rank translation.
 */
static SCOREP_Mpi_Rank* scorep_mpi_ranks;

/**
 *  @internal
 *  Internal flag to indicate communicator initialization. It is set o non-zero if the
 *  communicator managment is initialzed. This happens when the function
 *  scorep_mpi_comm_init() is called.
 */
static int scorep_mpi_comm_initialized = 0;

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
    SCOREP_Mpi_Color       color; /* byte to help distiguish accesses on same window */
};

/**
 *  @internal
 *  Data structure to track active GATS epochs.
 */
static struct scorep_mpi_winacc_type scorep_mpi_winaccs[ SCOREP_MPI_MAX_WINACC ];

/**
 *  @internal
 *  Index of last valid entry in the scorep_mpi_winaccs array.
 */
static int scorep_mpi_last_winacc = 0;

#endif // SCOREP_MPI_NO_RMA

/*
 *-----------------------------------------------------------------------------
 *
 * Communicator management
 *
 *-----------------------------------------------------------------------------
 */

/* -- rank translation -- */

SCOREP_Mpi_Rank
scorep_mpi_rank_to_pe( SCOREP_Mpi_Rank rank,
                       MPI_Comm        comm )
{
    MPI_Group       group;
    SCOREP_Mpi_Rank global_rank;
    int32_t         inter;

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

#ifndef SCOREP_MPI_NO_RMA

SCOREP_Mpi_Rank
scorep_mpi_win_rank_to_pe( SCOREP_Mpi_Rank rank,
                           MPI_Win         win )
{
    MPI_Group       group;
    SCOREP_Mpi_Rank global_rank;

    /* get group of communicator associated with input window */
    PMPI_Win_get_group( win, &group );
    /* translate rank with respect to \a MPI_COMM_WORLD */
    PMPI_Group_translate_ranks( group, 1, &rank, scorep_mpi_world.group, &global_rank );
    /* free internal group of input window */
    PMPI_Group_free( &group );

    return global_rank;
}

/* -------------------------------------------------------------------- window handling */

SCOREP_MPIWindowHandle
scorep_mpi_win_id( MPI_Win win )
{
    int i = 0;

    SCOREP_LockMPIWindowDefinition();
    while ( i < scorep_mpi_last_window && scorep_mpi_windows[ i ].win != win )
    {
        i++;
    }

    if ( i <= scorep_mpi_last_window )
    {
        SCOREP_UnlockMPIWindowDefinition();
        return scorep_mpi_windows[ i ].wid;
    }
    else
    {
        SCOREP_UnlockMPIWindowDefinition();
        SCOREP_ERROR( SCOREP_ERROR_MPI_NO_WINDOW, "" );
        return SCOREP_INVALID_MPI_WINDOW;
    }
}

void
scorep_mpi_win_create( MPI_Win  win,
                       MPI_Comm comm )
{
    SCOREP_MPIWindowHandle handle = SCOREP_INVALID_MPI_WINDOW;

    SCOREP_LockMPIWindowDefinition();
    if ( scorep_mpi_last_window >= SCOREP_MPI_MAX_WIN )
    {
        SCOREP_ERROR( SCOREP_ERROR_MPI_TOO_MANY_WINDOWS, "" );
    }

    /* register mpi window definition */
    /* NOTE: MPI_COMM_WORLD is _not_ present in the internal structures,
     * and _must not_ be queried by scorep_mpi_comm_id */
    handle = SCOREP_DefineMPIWindow(
        comm == MPI_COMM_WORLD ? SCOREP_MPI_COMM_WORLD_HANDLE : scorep_mpi_comm_id( comm ) );

    /* enter win in scorep_mpi_windows[] arrray */
    scorep_mpi_windows[ scorep_mpi_last_window ].win = win;
    scorep_mpi_windows[ scorep_mpi_last_window ].wid = handle;

    scorep_mpi_last_window++;
    SCOREP_UnlockMPIWindowDefinition();
}

void
scorep_mpi_win_free( MPI_Win win )
{
    SCOREP_LockMPIWindowDefinition();
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
            SCOREP_ERROR( SCOREP_ERROR_MPI_NO_WINDOW, "" );
        }
    }
    else
    {
        SCOREP_ERROR( SCOREP_ERROR_MPI_NO_WINDOW, "" );
    }
    SCOREP_UnlockMPIWindowDefinition();
}
#endif

/* -------------------------------------------------------------- communicator handling */

void
scorep_mpi_comm_init()
{
    int i;

    /* check, if we already initialized the data structures */
    if ( !scorep_mpi_comm_initialized )
    {
        /* Set the flag the communicator management is initialized */
        scorep_mpi_comm_initialized = 1;

        /* get group of \a MPI_COMM_WORLD */
        PMPI_Comm_group( MPI_COMM_WORLD, &scorep_mpi_world.group );

        /* determine the number of MPI processes */
        PMPI_Group_size( scorep_mpi_world.group, &scorep_mpi_world.size );

        /* initialize translation data structure for \a MPI_COMM_WORLD */
        scorep_mpi_world.ranks = calloc( scorep_mpi_world.size, sizeof( SCOREP_Mpi_Rank ) );
        for ( i = 0; i < scorep_mpi_world.size; i++ )
        {
            scorep_mpi_world.ranks[ i ] = i;
        }

        /* allocate translation buffers */
        scorep_mpi_ranks = calloc( scorep_mpi_world.size, sizeof( SCOREP_Mpi_Rank ) );

        scorep_mpi_world.handle =
            SCOREP_DefineMPICommunicator( scorep_mpi_world.size,
                                          scorep_mpi_world.ranks,
                                          "MPI_COMM_WORLD" );
    }
    else
    {
        SCOREP_DEBUG_PRINTF( SCOREP_WARNING | SCOREP_DEBUG_MPI, "Duplicate call to communicator initialization ignored!\n" );
    }
}

void
scorep_mpi_comm_finalize()
{
    /* free MPI group held internally */
    PMPI_Group_free( &scorep_mpi_world.group );

    /* free local translation buffers */
    free( scorep_mpi_world.ranks );
    free( scorep_mpi_ranks );

    /* reset initialization flag
     * (needed to prevent crashes with broken MPI implementations) */
    scorep_mpi_comm_initialized = 0;
}

int32_t
scorep_mpi_group_translate_ranks( MPI_Group group )
{
    int32_t i, size;

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

void
scorep_mpi_comm_create( MPI_Comm comm )
{
    MPI_Group                    group;
    SCOREP_MPICommunicatorHandle handle;

    /* Check if communicator handling has been initialized.
     * Prevents crashes with broken MPI implementations (e.g. mvapich-0.9.x)
     * that use MPI_ calls instead of PMPI_ calls to create some
     * internal communicators.
     * Also applies to scorep_mpi_comm_free and scorep_mpi_group_(create|free).
     */
    if ( !scorep_mpi_comm_initialized )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_WARNING,
                             "Skipping attempt to create communicator "
                             "outside init->finalize scope" );
        return;
    }

    /* Lock communicator definition */
    SCOREP_LockMPICommunicatorDefinition();

    /* is storage available */
    if ( scorep_mpi_last_comm >= SCOREP_MPI_MAX_COMM )
    {
        SCOREP_UnlockMPICommunicatorDefinition();
        SCOREP_ERROR( SCOREP_ERROR_MPI_TOO_MANY_COMMS, "" );
        return;
    }

    /* get group of this communicator */
    PMPI_Comm_group( comm, &group );

    /* create group entry in scorep_mpi_ranks */
    int32_t size = scorep_mpi_group_translate_ranks( group );

    /* register mpi communicator definition */
    handle = SCOREP_DefineMPICommunicator( size, scorep_mpi_ranks, "" );

    /* enter comm in scorep_mpi_comms[] arrray */
    scorep_mpi_comms[ scorep_mpi_last_comm ].comm = comm;
    scorep_mpi_comms[ scorep_mpi_last_comm ].cid  = handle;
    scorep_mpi_last_comm++;

    /* clean up */
    PMPI_Group_free( &group );
    SCOREP_UnlockMPICommunicatorDefinition();
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
        SCOREP_DEBUG_PRINTF( SCOREP_WARNING,
                             "Skipping attempt to create communicator "
                             "outside init->finalize scope" );
        return;
    }

    /* Lock communicator definition */
    SCOREP_LockMPICommunicatorDefinition();

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
            SCOREP_ERROR( SCOREP_ERROR_MPI_NO_COMM, "scorep_mpi_comm_free1 %s", message );
        }
    }
    else
    {
        SCOREP_ERROR( SCOREP_ERROR_MPI_NO_COMM, "scorep_mpi_comm_free2 %s", message );
    }

    /* Unlock communicator definition */
    SCOREP_UnlockMPICommunicatorDefinition();
}

SCOREP_MPICommunicatorHandle
scorep_mpi_comm_id( MPI_Comm comm )
{
    int i = 0;

    /* Lock communicator definition */
    SCOREP_LockMPICommunicatorDefinition();

    while ( i < scorep_mpi_last_comm && scorep_mpi_comms[ i ].comm != comm )
    {
        i++;
    }

    if ( i != scorep_mpi_last_comm )
    {
        /* Unlock communicator definition */
        SCOREP_UnlockMPICommunicatorDefinition();

        return scorep_mpi_comms[ i ].cid;
    }
    else
    {
        /* Unlock communicator definition */
        SCOREP_UnlockMPICommunicatorDefinition();

        if ( comm == MPI_COMM_WORLD )
        {
            SCOREP_DEBUG_PRINTF( SCOREP_WARNING | SCOREP_DEBUG_MPI, "This function SHOULD NOT be called with MPI_COMM_WORLD" );
            return SCOREP_MPI_COMM_WORLD_HANDLE;
        }
        else
        {
            SCOREP_ERROR( SCOREP_ERROR_MPI_NO_COMM,
                          "epk_comm_id: You are using a communicator that was "
                          "not tracked. Maybe you used a non-standard "
                          "MPI function call to create it." );
            return SCOREP_INVALID_MPI_COMMUNICATOR;
        }
    }
}

/*
 *-----------------------------------------------------------------------------
 *
 * Group management
 *
 *-----------------------------------------------------------------------------
 */

void
scorep_mpi_group_create( MPI_Group group )
{
    int32_t                      i;
    SCOREP_MPICommunicatorHandle handle;

    /* Check if communicator handling has been initialized.
     * Prevents crashes with broken MPI implementations (e.g. mvapich-0.9.x)
     * that use MPI_ calls instead of PMPI_ calls to create some
     * internal communicators.
     */
    if ( !scorep_mpi_comm_initialized )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_WARNING,
                             "Skipping attempt to create communicator "
                             "outside init->finalize scope" );
        return;
    }

    /* Lock communicator definition */
    SCOREP_LockMPICommunicatorDefinition();

    if ( scorep_mpi_last_group >= SCOREP_MPI_MAX_GROUP )
    {
        SCOREP_ERROR( SCOREP_ERROR_MPI_TOO_MANY_GROUPS, "" );
    }

    /* check if group already exists */
    if ( ( i = scorep_mpi_group_search( group ) ) == -1 )
    {
        /* create group entry in scorep_mpi_ranks */
        int32_t size = scorep_mpi_group_translate_ranks( group );

        /* register mpi group definition (as communicator) */
        handle = SCOREP_DefineMPICommunicator( size, scorep_mpi_ranks, "" );

        /* enter group in scorep_mpi_groups[] arrray */
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
    SCOREP_UnlockMPICommunicatorDefinition();
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
        SCOREP_DEBUG_PRINTF( SCOREP_WARNING,
                             "Skipping attempt to create communicator "
                             "outside init->finalize scope" );
        return;
    }

    SCOREP_LockMPICommunicatorDefinition();

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
            SCOREP_ERROR( SCOREP_ERROR_MPI_NO_GROUP, "" );
        }
    }
    else
    {
        SCOREP_ERROR( SCOREP_ERROR_MPI_NO_GROUP, "" );
    }

    SCOREP_UnlockMPICommunicatorDefinition();
}

SCOREP_Mpi_GroupHandle
scorep_mpi_group_id( MPI_Group group )
{
    int32_t i = 0;

    SCOREP_LockMPICommunicatorDefinition();
    while ( ( i < scorep_mpi_last_group ) && ( scorep_mpi_groups[ i ].group != group ) )
    {
        i++;
    }

    if ( i != scorep_mpi_last_group )
    {
        SCOREP_UnlockMPICommunicatorDefinition();
        return scorep_mpi_groups[ i ].gid;
    }
    else
    {
        SCOREP_UnlockMPICommunicatorDefinition();
        SCOREP_ERROR( SCOREP_ERROR_MPI_NO_GROUP, "" );
        return SCOREP_INVALID_MPI_GROUP;
    }
}

int32_t
scorep_mpi_group_search( MPI_Group group )
{
    int32_t i = 0;

    SCOREP_LockMPICommunicatorDefinition();
    while ( ( i < scorep_mpi_last_group ) && ( scorep_mpi_groups[ i ].group != group ) )
    {
        i++;
    }

    if ( i != scorep_mpi_last_group )
    {
        SCOREP_UnlockMPICommunicatorDefinition();
        return i;
    }
    else
    {
        SCOREP_UnlockMPICommunicatorDefinition();
        return -1;
    }
}

/*
 *-----------------------------------------------------------------------------
 *
 * Window Access Groups -- which window is accessed by what group
 *
 *-----------------------------------------------------------------------------
 */

#ifndef SCOREP_MPI_NO_RMA

void
scorep_mpi_winacc_start( MPI_Win          win,
                         MPI_Group        group,
                         SCOREP_Mpi_Color color )
{
    if ( scorep_mpi_last_winacc >= SCOREP_MPI_MAX_WINACC )
    {
        SCOREP_ERROR( SCOREP_ERROR_MPI_TOO_MANY_WINACCS, "" );
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
            SCOREP_ERROR( SCOREP_ERROR_MPI_NO_WINACC, "" );
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
        SCOREP_ERROR( SCOREP_ERROR_MPI_NO_WINACC, "" );
        return SCOREP_INVALID_MPI_GROUP;
    }
}
#endif
