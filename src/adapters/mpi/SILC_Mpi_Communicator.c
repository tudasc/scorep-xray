/*
 * This file is part of the SILC project (http://www.silc.de)
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
   @file       SILC_Mpi_Communicator.c
   @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
   @status     ALPHA
   @ingroup    MPI_Wrapper

   @brief Internal funcions for communicator, group and window management.
 */

#include "SILC_Mpi_Communicator.h"
#include "SILC_Error.h"
#include "SILC_Debug.h"
#include "SILC_Definitions.h"
#include "SILC_DefinitionLocking.h"

#include <stdlib.h>
#include <string.h>

/*
 *-----------------------------------------------------------------------------
 *
 * Internal definitions
 *
 *-----------------------------------------------------------------------------
 */

/** @def SILC_MPI_MAX_COMM
 *  @internal
 *  Maximum amount of concurrently defined communicators per process.
 */
#define SILC_MPI_MAX_COMM    50

/** @def SILC_MPI_MAX_GROUP
 *  @internal
 *  Maximum amount of concurrently defines groups per process.
 */
#define SILC_MPI_MAX_GROUP   50

/** @def SILC_MPI_MAX_WIN
 *  @internal
 *  Maximum amount of concurrently defined windows per process
 */
#define SILC_MPI_MAX_WIN     50

/** @def SILC_MPI_MAX_WINACC
 *  @internal
 *  Maximum amount of concurrently active access or exposure epochs per
 *  process.
 */
#define SILC_MPI_MAX_WINACC  50

/** Contains the data of the MPI_COMM_WORLD definition. */
struct silc_mpi_world_type silc_mpi_world;

/** A switch if global ranks are calculated or not. If set to zero no global ranks are
    calculated. If set to non-zero global values are calculated. The current default
    are global ranks.
 */
int8_t silc_mpi_comm_determination = 1;

/* ------------------------------------------------ Definitions for MPI Window handling */
#ifndef SILC_MPI_NO_RMA

/** @internal
 *  Structure to translate MPI window handles to internal SILC IDs.
 */
struct silc_mpi_win_type
{
    MPI_Win              win; /** MPI window handle */
    SILC_MPIWindowHandle wid; /** Internal SILC window handle */
};

/** @internal
 *  Index in the silc_mpi_windows array of last valid entry in window tracking data
    structure.
 */
static int32_t silc_mpi_last_window = 0;

/** @internal
 *  Window tracking array */
static struct silc_mpi_win_type silc_mpi_windows[ SILC_MPI_MAX_WIN ];

#endif // SILC_MPI_NO_RMA

/* ------------------------------------------- Definitions for communicators and groups */

/** @internal
 * structure for communicator tracking
 */
struct silc_mpi_communicator_type
{
    MPI_Comm                   comm; /** MPI Communicator handle */
    SILC_MPICommunicatorHandle cid;  /** Internal SILC Communicator handle */
};

/** @internal
 * structure for group tracking
 */
struct silc_mpi_group_type
{
    MPI_Group            group;  /** MPI group handle */
    SILC_Mpi_GroupHandle gid;    /** Internal SILC group handle */
    int32_t              refcnt; /** Number of references to this group */
};

/** @internal
 *  Index into the silc_mpi_comms array to the last entry.
 */
static int32_t silc_mpi_last_comm = 0;

/** @internal
 *  Index into the silc_mpi_groups array to the last entry.
 */
static int32_t silc_mpi_last_group = 0;

/** @internal
 *  Communicator tracking data structure. Array of created communicators' handles.
 */
static struct silc_mpi_communicator_type silc_mpi_comms[ SILC_MPI_MAX_COMM ];

/** @internal
 *  Group tracking data structure. Array of created groups' handles.
 */
static struct silc_mpi_group_type silc_mpi_groups[ SILC_MPI_MAX_GROUP ];

/** @internal
 *  Internal array used for rank translation.
 */
static SILC_Mpi_Rank* silc_mpi_ranks;

/** @internal
 *  Internal flag to indicate communicator initialization. It is set o non-zero if the
 *  communicator managment is initialzed. This happens when the function
 *  silc_mpi_comm_init() is called.
 */
static int silc_mpi_comm_initialized = 0;

/* ------------------------------------------------ Definition for one sided operations */
#ifndef SILC_MPI_NO_RMA

/*
 *  exposure epoch
 */
const SILC_Mpi_Color silc_mpi_exp_epoch = 0;

/*
 *  access epoch
 */
const SILC_Mpi_Color silc_mpi_acc_epoch = 1;

/** @internal
 *  Entry data structure to track GATS epochs
 */
struct silc_mpi_winacc_type
{
    MPI_Win              win;   /* MPI window identifier */
    SILC_Mpi_GroupHandle gid;   /* SILC MPI group handle */
    SILC_Mpi_Color       color; /* byte to help distiguish accesses on same window */
};

/** @internal
 *  Data structure to track active GATS epochs.
 */
static struct silc_mpi_winacc_type silc_mpi_winaccs[ SILC_MPI_MAX_WINACC ];

/** @internal
 *  Index of last valid entry in the silc_mpi_winaccs array.
 */
static int silc_mpi_last_winacc = 0;

#endif // SILC_MPI_NO_RMA

/*
 *-----------------------------------------------------------------------------
 *
 * Communicator management
 *
 *-----------------------------------------------------------------------------
 */

/* -- rank translation -- */

SILC_Mpi_Rank
silc_mpi_rank_to_pe( SILC_Mpi_Rank rank,
                     MPI_Comm      comm )
{
    MPI_Group     group;
    SILC_Mpi_Rank global_rank;
    int32_t       inter;

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
    PMPI_Group_translate_ranks( group, 1, &rank, silc_mpi_world.group, &global_rank );
    /* free internal group of input communicator */
    PMPI_Group_free( &group );

    return global_rank;
}

#ifndef SILC_MPI_NO_RMA

SILC_Mpi_Rank
silc_mpi_win_rank_to_pe( SILC_Mpi_Rank rank,
                         MPI_Win       win )
{
    MPI_Group     group;
    SILC_Mpi_Rank global_rank;

    /* get group of communicator associated with input window */
    PMPI_Win_get_group( win, &group );
    /* translate rank with respect to \a MPI_COMM_WORLD */
    PMPI_Group_translate_ranks( group, 1, &rank, silc_mpi_world.group, &global_rank );
    /* free internal group of input window */
    PMPI_Group_free( &group );

    return global_rank;
}

/* -------------------------------------------------------------------- window handling */

SILC_MPIWindowHandle
silc_mpi_win_id( MPI_Win win )
{
    int i = 0;

    SILC_LockMPIWindowDefinition();
    while ( i < silc_mpi_last_window && silc_mpi_windows[ i ].win != win )
    {
        i++;
    }

    if ( i <= silc_mpi_last_window )
    {
        SILC_UnlockMPIWindowDefinition();
        return silc_mpi_windows[ i ].wid;
    }
    else
    {
        SILC_UnlockMPIWindowDefinition();
        SILC_ERROR( SILC_ERROR_MPI_NO_WINDOW, "" );
        return SILC_INVALID_MPI_WINDOW;
    }
}

void
silc_mpi_win_create( MPI_Win  win,
                     MPI_Comm comm )
{
    SILC_MPIWindowHandle handle = SILC_INVALID_MPI_WINDOW;

    SILC_LockMPIWindowDefinition();
    if ( silc_mpi_last_window >= SILC_MPI_MAX_WIN )
    {
        SILC_ERROR( SILC_ERROR_MPI_TOO_MANY_WINDOWS, "" );
    }

    /* register mpi window definition */
    /* NOTE: MPI_COMM_WORLD is _not_ present in the internal structures,
     * and _must not_ be queried by silc_mpi_comm_id */
    handle = SILC_DefineMPIWindow(
        comm == MPI_COMM_WORLD ? SILC_MPI_COMM_WORLD_HANDLE : silc_mpi_comm_id( comm ) );

    /* enter win in silc_mpi_windows[] arrray */
    silc_mpi_windows[ silc_mpi_last_window ].win = win;
    silc_mpi_windows[ silc_mpi_last_window ].wid = handle;

    silc_mpi_last_window++;
    SILC_UnlockMPIWindowDefinition();
}

void
silc_mpi_win_free( MPI_Win win )
{
    SILC_LockMPIWindowDefinition();
    if ( silc_mpi_last_window == 1 && silc_mpi_windows[ 0 ].win == win )
    {
        silc_mpi_last_window = 0;
    }
    else if ( silc_mpi_last_window > 1 )
    {
        int i = 0;

        while ( i < silc_mpi_last_window && silc_mpi_windows[ i ].win != win )
        {
            i++;
        }

        if ( i < silc_mpi_last_window-- )
        {
            silc_mpi_windows[ i ] = silc_mpi_windows[ silc_mpi_last_window ];
        }
        else
        {
            SILC_ERROR( SILC_ERROR_MPI_NO_WINDOW, "" );
        }
    }
    else
    {
        SILC_ERROR( SILC_ERROR_MPI_NO_WINDOW, "" );
    }
    SILC_UnlockMPIWindowDefinition();
}
#endif

/* -------------------------------------------------------------- communicator handling */

void
silc_mpi_comm_init()
{
    int i;

    /* check, if we already initialized the data structures */
    if ( !silc_mpi_comm_initialized )
    {
        /* Set the flag the communicator management is initialized */
        silc_mpi_comm_initialized = 1;

        /* get group of \a MPI_COMM_WORLD */
        PMPI_Comm_group( MPI_COMM_WORLD, &silc_mpi_world.group );

        /* determine the number of MPI processes */
        PMPI_Group_size( silc_mpi_world.group, &silc_mpi_world.size );

        /* initialize translation data structure for \a MPI_COMM_WORLD */
        silc_mpi_world.ranks = calloc( silc_mpi_world.size, sizeof( SILC_Mpi_Rank ) );
        for ( i = 0; i < silc_mpi_world.size; i++ )
        {
            silc_mpi_world.ranks[ i ] = i;
        }

        /* allocate translation buffers */
        silc_mpi_ranks = calloc( silc_mpi_world.size, sizeof( SILC_Mpi_Rank ) );

        silc_mpi_world.handle =
            SILC_DefineMPICommunicator( silc_mpi_world.size,
                                        silc_mpi_world.ranks );
    }
    else
    {
        SILC_DEBUG_PRINTF( SILC_WARNING | SILC_DEBUG_MPI, "Duplicate call to communicator initialization ignored!\n" );
    }
}

void
silc_mpi_comm_finalize()
{
    /* free MPI group held internally */
    PMPI_Group_free( &silc_mpi_world.group );

    /* free local translation buffers */
    free( silc_mpi_world.ranks );
    free( silc_mpi_ranks );

    /* reset initialization flag
     * (needed to prevent crashes with broken MPI implementations) */
    silc_mpi_comm_initialized = 0;
}

int32_t
silc_mpi_group_translate_ranks( MPI_Group group )
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
                                silc_mpi_world.ranks,
                                silc_mpi_world.group,
                                silc_mpi_ranks );

    return size;
}

void
silc_mpi_comm_create( MPI_Comm comm )
{
    MPI_Group                  group;
    SILC_MPICommunicatorHandle handle;

    /* Check if communicator handling has been initialized.
     * Prevents crashes with broken MPI implementations (e.g. mvapich-0.9.x)
     * that use MPI_ calls instead of PMPI_ calls to create some
     * internal communicators.
     * Also applies to silc_mpi_comm_free and silc_mpi_group_(create|free).
     */
    if ( !silc_mpi_comm_initialized )
    {
        SILC_DEBUG_PRINTF( SILC_WARNING,
                           "Skipping attempt to create communicator "
                           "outside init->finalize scope" );
        return;
    }

    /* Lock communicator definition */
    SILC_LockMPICommunicatorDefinition();

    /* is storage available */
    if ( silc_mpi_last_comm >= SILC_MPI_MAX_COMM )
    {
        SILC_UnlockMPICommunicatorDefinition();
        SILC_ERROR( SILC_ERROR_MPI_TOO_MANY_COMMS, "" );
        return;
    }

    /* get group of this communicator */
    PMPI_Comm_group( comm, &group );

    /* create group entry in silc_mpi_ranks */
    int32_t size = silc_mpi_group_translate_ranks( group );

    /* register mpi communicator definition */
    handle = SILC_DefineMPICommunicator( size, silc_mpi_ranks );

    /* enter comm in silc_mpi_comms[] arrray */
    silc_mpi_comms[ silc_mpi_last_comm ].comm = comm;
    silc_mpi_comms[ silc_mpi_last_comm ].cid  = handle;
    silc_mpi_last_comm++;

    /* clean up */
    PMPI_Group_free( &group );
    SILC_UnlockMPICommunicatorDefinition();
}

void
silc_mpi_comm_free( MPI_Comm comm )
{
    const char* message = "You are trying to free a communicator that was "
                          "not tracked. Maybe you used a non-standard "
                          "MPI function call to create it.";

    /* check if comm handling is initialized (see silc_mpi_comm_create comment) */
    if ( !silc_mpi_comm_initialized )
    {
        SILC_DEBUG_PRINTF( SILC_WARNING,
                           "Skipping attempt to create communicator "
                           "outside init->finalize scope" );
        return;
    }

    /* Lock communicator definition */
    SILC_LockMPICommunicatorDefinition();

    /* if only one communicator exists, we just need to decrease \a
     * silc_mpi_last_comm */
    if ( silc_mpi_last_comm == 1 && silc_mpi_comms[ 0 ].comm == comm )
    {
        silc_mpi_last_comm = 0;
    }
    /* if more than one communicator exists, we need to search for the
     * entry */
    else if ( silc_mpi_last_comm > 1 )
    {
        int i = 0;

        while ( i < silc_mpi_last_comm && silc_mpi_comms[ i ].comm != comm )
        {
            i++;
        }

        if ( i < silc_mpi_last_comm-- )
        {
            /* swap deletion candidate with last entry in the list */
            silc_mpi_comms[ i ] = silc_mpi_comms[ silc_mpi_last_comm ];
        }
        else
        {
            SILC_ERROR( SILC_ERROR_MPI_NO_COMM, "silc_mpi_comm_free1 %s", message );
        }
    }
    else
    {
        SILC_ERROR( SILC_ERROR_MPI_NO_COMM, "silc_mpi_comm_free2 %s", message );
    }

    /* Unlock communicator definition */
    SILC_UnlockMPICommunicatorDefinition();
}

SILC_MPICommunicatorHandle
silc_mpi_comm_id( MPI_Comm comm )
{
    int i = 0;

    /* Lock communicator definition */
    SILC_LockMPICommunicatorDefinition();

    while ( i < silc_mpi_last_comm && silc_mpi_comms[ i ].comm != comm )
    {
        i++;
    }

    if ( i != silc_mpi_last_comm )
    {
        /* Unlock communicator definition */
        SILC_UnlockMPICommunicatorDefinition();

        return silc_mpi_comms[ i ].cid;
    }
    else
    {
        /* Unlock communicator definition */
        SILC_UnlockMPICommunicatorDefinition();

        if ( comm == MPI_COMM_WORLD )
        {
            SILC_DEBUG_PRINTF( SILC_WARNING | SILC_DEBUG_MPI, "This function SHOULD NOT be called with MPI_COMM_WORLD" );
            return SILC_MPI_COMM_WORLD_HANDLE;
        }
        else
        {
            SILC_ERROR( SILC_ERROR_MPI_NO_COMM,
                        "epk_comm_id: You are using a communicator that was "
                        "not tracked. Maybe you used a non-standard "
                        "MPI function call to create it." );
            return SILC_INVALID_MPI_COMMUNICATOR;
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
silc_mpi_group_create( MPI_Group group )
{
    int32_t                    i;
    SILC_MPICommunicatorHandle handle;

    /* Check if communicator handling has been initialized.
     * Prevents crashes with broken MPI implementations (e.g. mvapich-0.9.x)
     * that use MPI_ calls instead of PMPI_ calls to create some
     * internal communicators.
     */
    if ( !silc_mpi_comm_initialized )
    {
        SILC_DEBUG_PRINTF( SILC_WARNING,
                           "Skipping attempt to create communicator "
                           "outside init->finalize scope" );
        return;
    }

    /* Lock communicator definition */
    SILC_LockMPICommunicatorDefinition();

    if ( silc_mpi_last_group >= SILC_MPI_MAX_GROUP )
    {
        SILC_ERROR( SILC_ERROR_MPI_TOO_MANY_GROUPS, "" );
    }

    /* check if group already exists */
    if ( ( i = silc_mpi_group_search( group ) ) == -1 )
    {
        /* create group entry in silc_mpi_ranks */
        int32_t size = silc_mpi_group_translate_ranks( group );

        /* register mpi group definition (as communicator) */
        handle = SILC_DefineMPICommunicator( size, silc_mpi_ranks );

        /* enter group in silc_mpi_groups[] arrray */
        silc_mpi_groups[ silc_mpi_last_group ].group  = group;
        silc_mpi_groups[ silc_mpi_last_group ].gid    = handle;
        silc_mpi_groups[ silc_mpi_last_group ].refcnt = 1;
        silc_mpi_last_group++;
    }
    else
    {
        /* count additional reference on group */
        silc_mpi_groups[ i ].refcnt++;
    }

    /* Unlock communicator definition */
    SILC_UnlockMPICommunicatorDefinition();
}

void
silc_mpi_group_free( MPI_Group group )
{
    /* Check if communicator handling has been initialized.
     * Prevents crashes with broken MPI implementations (e.g. mvapich-0.9.x)
     * that use MPI_ calls instead of PMPI_ calls to create some
     * internal communicators.
     */
    if ( !silc_mpi_comm_initialized )
    {
        SILC_DEBUG_PRINTF( SILC_WARNING,
                           "Skipping attempt to create communicator "
                           "outside init->finalize scope" );
        return;
    }

    SILC_LockMPICommunicatorDefinition();

    if ( silc_mpi_last_group == 1 && silc_mpi_groups[ 0 ].group == group )
    {
        silc_mpi_groups[ 0 ].refcnt--;

        if ( silc_mpi_groups[ 0 ].refcnt == 0 )
        {
            silc_mpi_last_group--;
        }
    }
    else if ( silc_mpi_last_group > 1 )
    {
        int32_t i;

        if ( ( i = silc_mpi_group_search( group ) ) != -1 )
        {
            /* decrease reference count on entry */
            silc_mpi_groups[ i ].refcnt--;

            /* check if entry can be deleted */
            if ( silc_mpi_groups[ i ].refcnt == 0 )
            {
                silc_mpi_groups[ i ] = silc_mpi_groups[ --silc_mpi_last_group ];
            }
        }
        else
        {
            SILC_ERROR( SILC_ERROR_MPI_NO_GROUP, "" );
        }
    }
    else
    {
        SILC_ERROR( SILC_ERROR_MPI_NO_GROUP, "" );
    }

    SILC_UnlockMPICommunicatorDefinition();
}

SILC_Mpi_GroupHandle
silc_mpi_group_id( MPI_Group group )
{
    int32_t i = 0;

    SILC_LockMPICommunicatorDefinition();
    while ( ( i < silc_mpi_last_group ) && ( silc_mpi_groups[ i ].group != group ) )
    {
        i++;
    }

    if ( i != silc_mpi_last_group )
    {
        SILC_UnlockMPICommunicatorDefinition();
        return silc_mpi_groups[ i ].gid;
    }
    else
    {
        SILC_UnlockMPICommunicatorDefinition();
        SILC_ERROR( SILC_ERROR_MPI_NO_GROUP, "" );
        return SILC_INVALID_MPI_GROUP;
    }
}

int32_t
silc_mpi_group_search( MPI_Group group )
{
    int32_t i = 0;

    SILC_LockMPICommunicatorDefinition();
    while ( ( i < silc_mpi_last_group ) && ( silc_mpi_groups[ i ].group != group ) )
    {
        i++;
    }

    if ( i != silc_mpi_last_group )
    {
        SILC_UnlockMPICommunicatorDefinition();
        return i;
    }
    else
    {
        SILC_UnlockMPICommunicatorDefinition();
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

#ifndef SILC_MPI_NO_RMA

void
silc_mpi_winacc_start( MPI_Win        win,
                       MPI_Group      group,
                       SILC_Mpi_Color color )
{
    if ( silc_mpi_last_winacc >= SILC_MPI_MAX_WINACC )
    {
        SILC_ERROR( SILC_ERROR_MPI_TOO_MANY_WINACCS, "" );
    }

    silc_mpi_winaccs[ silc_mpi_last_winacc ].win   = win;
    silc_mpi_winaccs[ silc_mpi_last_winacc ].gid   = silc_mpi_group_id( group );
    silc_mpi_winaccs[ silc_mpi_last_winacc ].color = color;

    silc_mpi_last_winacc++;
}

void
silc_mpi_winacc_end( MPI_Win        win,
                     SILC_Mpi_Color color )
{
    int i = 0;
    /* only one window inside wingrp */
    if ( silc_mpi_last_winacc == 1
         && silc_mpi_winaccs[ 0 ].win   == win
         && silc_mpi_winaccs[ 0 ].color == color )
    {
        silc_mpi_last_winacc--;
    }
    else
    {
        while ( ( i <= silc_mpi_last_winacc ) &&
                ( ( silc_mpi_winaccs[ i ].win != win ) || ( silc_mpi_winaccs[ i ].color != color ) ) )
        {
            i++;
        }

        if ( i != silc_mpi_last_winacc )
        {
            silc_mpi_last_winacc--;
            silc_mpi_winaccs[ i ].win   = silc_mpi_winaccs[ silc_mpi_last_winacc ].win;
            silc_mpi_winaccs[ i ].gid   = silc_mpi_winaccs[ silc_mpi_last_winacc ].gid;
            silc_mpi_winaccs[ i ].color = silc_mpi_winaccs[ silc_mpi_last_winacc ].color;
        }
        else
        {
            SILC_ERROR( SILC_ERROR_MPI_NO_WINACC, "" );
        }
    }
}

SILC_Mpi_GroupHandle
silc_mpi_winacc_get_gid( MPI_Win        win,
                         SILC_Mpi_Color color )
{
    int i = 0;

    while ( ( i <= silc_mpi_last_winacc ) &&
            ( ( silc_mpi_winaccs[ i ].win != win ) || ( silc_mpi_winaccs[ i ].color != color ) ) )
    {
        i++;
    }

    if ( i != silc_mpi_last_winacc )
    {
        return silc_mpi_winaccs[ i ].gid;
    }
    else
    {
        SILC_ERROR( SILC_ERROR_MPI_NO_WINACC, "" );
        return SILC_INVALID_MPI_GROUP;
    }
}
#endif
