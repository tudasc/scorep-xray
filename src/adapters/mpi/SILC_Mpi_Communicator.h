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
 * @file  SILC_Mpi_Communicator.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    MPI_Wrapper
 *
 * @brief Communicator, group and window management
 */

#ifndef SILC_MPI_COMMUNICATOR_H
#define SILC_MPI_COMMUNICATOR_H

/*
 *-----------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing Interface Kit)
 *
 *  - MPI communicator management
 *
 *-----------------------------------------------------------------------------
 */

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

#include <SILC_Types.h>
#include <SILC_DefinitionHandles.h>
#include <mpi.h>

/** Type of internal SILC group handles. Currently, Communicators and groups use
    the same realm and thus the handles are of the same type. Maybe tis changes later on.
    Thus, we already use two different internal names.
 */
typedef SILC_MPICommunicatorHandle SILC_Mpi_GroupHandle;

/** Type of the different colors for MPI epochs. Its a boolean flag which can have
    the values silc_mpi_exp_epoch or silc_mpi_acc_epoch.
 */
typedef uint8_t SILC_Mpi_Color;

/** Type of Ranks */
typedef int32_t SILC_Mpi_Rank;

/* Check MPI version. Disable RMA if MPI version 1 */
#if defined( MPI_VERSION ) && ( MPI_VERSION == 1 ) && !defined( SILC_MPI_NO_RMA )
  #define SILC_MPI_NO_RMA
#endif

/** @def SILC_MPI_INVALID_RANK
    Defines an invalid mpi rank.
 */
#define SILC_MPI_INVALID_RANK -1

/* Defines the value for SILC_Mpi_GroupHandle which marks an invalid group */
#define SILC_INVALID_MPI_GROUP SILC_INVALID_MPI_COMMUNICATOR

/** @internal Identifier for exposure epochs */
EXTERN const uint8_t silc_mpi_exp_epoch;

/** @internal Identifier for access epochs */
EXTERN const uint8_t silc_mpi_acc_epoch;

/** @internal
 *  @brief Initialize communicator management.
 *  Allocation and initialization of internal data structures. Registration of
 *  MPI_COMM_WORLD.
 */
EXTERN void
silc_mpi_comm_init();

/** @internal
 *  @brief Cleanup communicator management.
 */
EXTERN void
silc_mpi_comm_finalize();

/** @internal
 *  Create a bitvector resembling the ranks in an MPI group.
 *
 *  Creates a bitvector where nth bit is set, if rank n is part
 *  of the group. The bitvector is stored in a static internal
 *  variable. The output is written into the global variable
 *  silc_mpi_group_vector. This avoids reserving long arrays on
 *  each function call.
 *
 *  @param  group MPI group handle
 */
EXTERN void
silc_mpi_group_to_bitvector( MPI_Group group );

/** @internal
 *  Create an internal handle for an MPI group.
 *
 *  Creates an internal datastructure to enable tracking of this
 *  MPI group handle.
 *
 *  @param  group MPI group handle
 */
EXTERN void
silc_mpi_group_create( MPI_Group group );

/** @internal
 *  @brief Stop tracking of a given MPI group handle.
 *  Clears internal SILC handle.
 *  @param group MPI group handle
 */
EXTERN void
silc_mpi_group_free( MPI_Group group );

/** @internal
 *  @brief Retrieve the internal SILC handle for a givem MPI group handle.
 *  @param  group MPI group handle.
 *  @return Internal SILC handle for the given MPI group handle.
 */
EXTERN SILC_Mpi_GroupHandle
silc_mpi_group_id( MPI_Group group );

/** @internal
 *  @brief  Search internal data structures for the entry of a given MPI
 *          group handle.
 *  @param  group  MPI group handle
 *  @return Index of entry or -1 if entry could not be found.
 */
EXTERN int32_t
silc_mpi_group_search( MPI_Group group );

#ifndef SILC_MPI_NO_RMA

/** @internal
 *  Translate a rank to its global rank in reference to a window (and its
 *  communicator.
 *  @param  rank Local rank to be translated
 *  @param  win  Window that implicitely defines the communicator rank
 *               refers to.
 *  @return Global rank
 */
EXTERN SILC_Mpi_Rank
silc_mpi_win_rank_to_pe( SILC_Mpi_Rank rank,
                         MPI_Win       win );

/** @internal
 *  Return the internal SILC handle for a given MPI window handle.
 *  @param  win MPI window handle.
 *  @return Internal SILC handle for the given window.
 */
EXTERN SILC_MPIWindowHandle
silc_mpi_win_id( MPI_Win win );

/** @internal
 *  Create definition record and internal tracking handle.
 *  @param win MPI window handle of the window to be tracked.
 *  @param comm MPI communication handle the window is defined on.
 */
EXTERN void
silc_mpi_win_create( MPI_Win  win,
                     MPI_Comm comm );

/** @internal
 *  Free definition record and internal tracking handle.
 *  @param win MPI window handlte of the window to be freed.
 */
EXTERN void
silc_mpi_win_free( MPI_Win win );

/** @internal
 *  @brief Start tracking of epoch.
 *  @param win   MPI window handle of related window.
 *  @param group MPI group handle of related process group.
 *  @param color Type of epoch (exposure=0, access=1).
 */
EXTERN void
silc_mpi_winacc_start( MPI_Win        win,
                       MPI_Group      group,
                       SILC_Mpi_Color color );

/** @internal
 *  @brief End tracking of epoch.
 *  @param win   MPI window handle of related window.
 *  @param color Type of epoch (exposure=0, access=1).
 */
EXTERN void
silc_mpi_winacc_end( MPI_Win        win,
                     SILC_Mpi_Color color );

/** @internal
 *  @brief  Get internal group ID of process group related to an epoch.
 *  @param  win   MPI window handle of related window.
 *  @param  color Type of epoch (exposure=0, access=1).
 *  @return Internal process group handle.
 */
EXTERN SILC_Mpi_GroupHandle
silc_mpi_winacc_get_gid( MPI_Win        win,
                         SILC_Mpi_Color color );

#endif

/** @internal
 *  @brief Start tracking of a given MPI communcator.
 *  makes the definition of the given communicator to the measurement system.
 *  @param comm MPI communicator handle.
 */
EXTERN void
silc_mpi_comm_create( MPI_Comm comm );

/** @internal
 *  @brief  Stop tracking of an MPI communicator handle.
 *  @param  comm Still valid MPI communicator that is to be freed by
 *               \a MPI_Comm_free.
 */
EXTERN void
silc_mpi_comm_free( MPI_Comm comm );

/** @internal
 *  @brief  Retrieves the internal SILC handle of a given MPI communicator.
 *  @param  comm MPI communicator
 *  @return Internal SILC handle of MPI communicator %comm
 */
EXTERN SILC_MPICommunicatorHandle
silc_mpi_comm_id( MPI_Comm comm );

/** @internal
 *  Translate a rank within a given communicator to its global rank in
 *  \a MPI_COMM_WORLD.
 *  @param  rank Local rank to be translated.
 *  @param  comm Local communicator the rank refers to.
 *  @return Global rank.
 */
EXTERN SILC_Mpi_Rank
silc_mpi_rank_to_pe( SILC_Mpi_Rank rank,
                     MPI_Comm      comm );

/** @internal
 *  Structure to hold the \a MPI_COMM_WORLD definition.
 */
struct silc_mpi_world_type
{
    MPI_Group                  group;     /** Associated MPI group */
    int                        size;      /** Number of ranks */
    int                        size_grpv; /** Number of bytes used for the group vector */
    SILC_Mpi_Rank*             ranks;     /** Array which contains the rank numbers */
    SILC_MPICommunicatorHandle handle;    /** SILC handle */
};

/** Contains the data of the MPI_COMM_WORLD definition. */
extern struct silc_mpi_world_type silc_mpi_world;

EXTERN int8_t                     silc_mpi_comm_determination; /* whether comm/rank determined for events */

/** @def SILC_MPI_COMM_WORLD_HANDLE
    The SILC comminicator handle for MPI_COMM_WORLD.
 */
#define SILC_MPI_COMM_WORLD_HANDLE silc_mpi_world.handle

/* MPI communicator |-> SILC communicator id */
#define SILC_MPI_COMM_ID( c ) ( !silc_mpi_comm_determination ? SILC_INVALID_MPI_COMMUNICATOR : ( ( ( c ) == MPI_COMM_WORLD ) ? SILC_MPI_COMM_WORLD_HANDLE : silc_mpi_comm_id( c ) ) )

/* Rank with respect to arbitrary communicator |-> global rank */
#define SILC_MPI_RANK_TO_PE( r, c ) ( !silc_mpi_comm_determination ? SILC_MPI_INVALID_RANK : ( ( ( c ) == MPI_COMM_WORLD ) ? r : silc_mpi_rank_to_pe( r, c ) ) )

#endif // SILC_MPI_COMMUNICATOR_H
