/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
 * @file  SILC_Mpi_Communicator.h
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

#include "SILC_Types.h"

#include "mpi.h"

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

#ifdef HAS_MPI2_1SIDED

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

EXTERN int8_t silc_mpi_comm_determination;  /* whether comm/rank determined for events */

/* MPI communicator |-> EPIK communicator id */
#define SILC_MPI_COMM_ID( c ) ( !silc_mpi_comm_determination ? SILC_MPI_INVALID_RANK : ( ( ( c ) == MPI_COMM_WORLD ) ? 0 : silc_mpi_comm_id( c ) ) )

/* Rank with respect to arbitrary communicator |-> global rank */
#define SILC_MPI_RANK_TO_PE( r, c ) ( !silc_mpi_comm_determination ? SILC_MPI_INVALID_RANK : ( ( ( c ) == MPI_COMM_WORLD ) ? r : silc_mpi_rank_to_pe( r, c ) ) )

#endif // SILC_MPI_COMMUNICATOR_H
