/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2017,
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
 *
 */

/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief Internal functions for communicator, group and window management.
 */

#include <config.h>
#include "scorep_mpi_communicator.h"
#include "scorep_mpi_communicator_mgmt.h"
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>
#include "SCOREP_Mpi.h"
#include <SCOREP_Definitions.h>

#include <UTILS_Error.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

/*
 * ----------------------------------------------------------------------------
 *
 * External declarations
 *
 * ----------------------------------------------------------------------------
 */

/*
 * ----------------------------------------------------------------------------
 *
 * Internal definitions
 *
 * ----------------------------------------------------------------------------
 */

/* ------------------------------------------------ Definitions for MPI Window handling */
#ifndef SCOREP_MPI_NO_RMA

/**
 *  @internal
 *  Index in the scorep_mpi_windows array of last valid entry in window tracking data
 *  structure.
 */
static int32_t scorep_mpi_last_window = 0;

/**
 *  @internal
 *  Mutex for mpi window definitions.
 */
extern SCOREP_Mutex scorep_mpi_window_mutex;

#endif // !SCOREP_MPI_NO_RMA

/* ------------------------------------------- Definitions for communicators and groups */

/**
 *  @internal
 *  Index into the scorep_mpi_groups array to the last entry.
 */
static int32_t scorep_mpi_last_group = 0;

/**
 *  @internal
 *  Internal array used for rank translation.
 */
extern SCOREP_MpiRank* scorep_mpi_ranks;

/**
 *  @internal
 *  Internal flag to indicate communicator initialization. It is set o non-zero if the
 *  communicator management is initialized. This happens when the function
 *  scorep_mpi_comm_init() is called.
 */
extern int scorep_mpi_comm_initialized;
extern int scorep_mpi_comm_finalized;

/**
 *  @internal
 *  Mutex for communicator definition.
 */
extern SCOREP_Mutex scorep_mpi_communicator_mutex;

/**
   MPI datatype for ID-ROOT exchange
 */
extern MPI_Datatype scorep_mpi_id_root_type;

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
 *  @internal
 *  Data structure to track active GATS epochs.
 */
extern struct scorep_mpi_winacc_type* scorep_mpi_winaccs;

/**
 *  @internal
 *  Index of last valid entry in the scorep_mpi_winaccs array.
 */
static int scorep_mpi_last_winacc = 0;

#endif // !SCOREP_MPI_NO_RMA

/*
 * -----------------------------------------------------------------------------
 *
 * Communicator management
 *
 * -----------------------------------------------------------------------------
 */

/* -------------------------------------------------------------------- window handling */

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

#endif // !SCOREP_MPI_NO_RMA

/* -------------------------------------------------------------- communicator handling */

/**
 *  @internal
 * Translates ranks in the MPI Group @group into MPI_COMM_WORLD ranks.
 *
 * @param  group MPI group handle
 *
 * @return the size of group @group
 */
static int32_t
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

void
scorep_mpi_comm_set_name( MPI_Comm comm, const char* name )
{
    if ( !name )
    {
        return;
    }

    SCOREP_InterimCommunicatorHandle    comm_handle  = SCOREP_MPI_COMM_HANDLE( comm );
    scorep_mpi_comm_definition_payload* comm_payload =
        SCOREP_InterimCommunicatorHandle_GetPayload( comm_handle );

    SCOREP_MutexLock( scorep_mpi_communicator_mutex );

    /*
     * Set the name only for the root rank in the communicator and
     * for non-SELF-like communicators
     */
    if ( 0 == comm_payload->local_rank && comm_payload->comm_size != 1 )
    {
        /*
         * Does set the name only the first time
         */
        SCOREP_InterimCommunicatorHandle_SetName( comm_handle, name );
    }

    SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
}

void
scorep_mpi_comm_set_default_names( void )
{
    scorep_mpi_comm_set_name( MPI_COMM_WORLD, "MPI_COMM_WORLD" );

    /*
     * 'scorep_mpi_comm_set_name' prevents from changing the name for self-like
     * comms, though MPI_COMM_SELF exists in all ranks, and thus we can
     * give him his name.
     */
    SCOREP_InterimCommunicatorHandle_SetName(
        scorep_mpi_comm_handle( MPI_COMM_SELF ), "MPI_COMM_SELF" );
}

/*
 * -----------------------------------------------------------------------------
 *
 * Group management
 *
 * -----------------------------------------------------------------------------
 */

/**
 * @internal
 * @brief  Search internal data structures for the entry of a given MPI
 *         group handle.
 * @param  group  MPI group handle
 * @return Index of entry or -1 if entry could not be found.
 */
static int32_t
scorep_mpi_group_search( MPI_Group group );

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

    /* check if group already exists */
    if ( ( i = scorep_mpi_group_search( group ) ) == -1 )
    {
        if ( scorep_mpi_last_group >= SCOREP_MPI_MAX_GROUP )
        {
            UTILS_ERROR( SCOREP_ERROR_MPI_TOO_MANY_GROUPS,
                         "Hint: Increase SCOREP_MPI_MAX_GROUPS configuration variable." );
            SCOREP_MutexUnlock( scorep_mpi_communicator_mutex );
            return;
        }

        /* create group entry in scorep_mpi_ranks */
        int32_t size = scorep_mpi_group_translate_ranks( group );

        /* register mpi group definition (as communicator) */
        handle = SCOREP_Definitions_NewGroupFrom32(
            SCOREP_GROUP_MPI_GROUP,
            "",
            size,
            ( const uint32_t* )scorep_mpi_ranks );

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

static SCOREP_Mpi_GroupHandle
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

static int32_t
scorep_mpi_group_search( MPI_Group group )
{
    int32_t i = 0;

    while ( ( i < scorep_mpi_last_group ) && ( scorep_mpi_groups[ i ].group != group ) )
    {
        i++;
    }

    if ( i != scorep_mpi_last_group )
    {
        return i;
    }
    else
    {
        return -1;
    }
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
#endif // !SCOREP_MPI_NO_RMA
