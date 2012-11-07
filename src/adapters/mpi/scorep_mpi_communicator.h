/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       scorep_mpi_communicator.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    MPI_Wrapper
 *
 * @brief Communicator, group and window management
 */

#ifndef SCOREP_MPI_COMMUNICATOR_H
#define SCOREP_MPI_COMMUNICATOR_H

/*
 * -----------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing Interface Kit)
 *
 *  - MPI communicator management
 *
 * -----------------------------------------------------------------------------
 */

#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>
#include <mpi.h>

/**
 * Type of internal SCOREP group handles.
 */
typedef SCOREP_GroupHandle SCOREP_Mpi_GroupHandle;

/**
 * Type of the different colors for MPI epochs. Its a boolean flag which can have
 * the values scorep_mpi_exp_epoch or scorep_mpi_acc_epoch.
 */
typedef uint8_t SCOREP_Mpi_Color;


/* Check MPI version. Disable RMA if MPI version 1 */
#if defined( MPI_VERSION ) && ( MPI_VERSION == 1 ) && !defined( SCOREP_MPI_NO_RMA )
  #define SCOREP_MPI_NO_RMA
#endif


/**
 * Defines the value for SCOREP_Mpi_GroupHandle which marks an invalid group
 */
#define SCOREP_INVALID_MPI_GROUP SCOREP_INVALID_GROUP

/**
 * @internal
 * @brief Initialize communicator management.
 * Allocation and initialization of internal data structures. Registration of
 * MPI_COMM_WORLD.
 */
extern void
scorep_mpi_comm_init( void );

/**
 * @internal
 * @brief Cleanup communicator management.
 */
extern void
scorep_mpi_comm_finalize( void );

/**
 *  @internal
 * Translates ranks in the MPI Group @group into MPI_COMM_WORLD ranks.
 *
 * @param  group MPI group handle
 *
 * @return the size of group @group
 */
extern int32_t
scorep_mpi_group_translate_ranks( MPI_Group group );

/**
 *  @internal
 * Create an internal handle for an MPI group.
 *
 * Creates an internal datastructure to enable tracking of this
 * MPI group handle.
 *
 * @param  group MPI group handle
 */
extern void
scorep_mpi_group_create( MPI_Group group );

/**
 * @internal
 * @brief Stop tracking of a given MPI group handle.
 * Clears internal SCOREP handle.
 * @param group MPI group handle
 */
extern void
scorep_mpi_group_free( MPI_Group group );

/**
 * @internal
 * @brief Retrieve the internal SCOREP handle for a given MPI group handle.
 * @param  group MPI group handle.
 * @return Internal SCOREP handle for the given MPI group handle.
 */
extern SCOREP_Mpi_GroupHandle
scorep_mpi_group_id( MPI_Group group );

/**
 * @internal
 * @brief  Search internal data structures for the entry of a given MPI
 *         group handle.
 * @param  group  MPI group handle
 * @return Index of entry or -1 if entry could not be found.
 */
extern int32_t
scorep_mpi_group_search( MPI_Group group );

/**
 * Initializes the window handling specific data structures.
 */
extern void
scorep_mpi_win_init( void );

/**
 * Finalizes the window handling specific data structures.
 */
extern void
scorep_mpi_win_finalize( void );

#ifndef SCOREP_MPI_NO_RMA

/**
 * @internal
 * Translate a rank to its global rank in reference to a window (and its
 * communicator.
 * @param  rank Local rank to be translated
 * @param  win  Window that implicitly defines the communicator rank
 *              refers to.
 * @return Global rank
 */
extern SCOREP_MpiRank
scorep_mpi_win_rank_to_pe( SCOREP_MpiRank rank,
                           MPI_Win        win );

/**
 * @internal
 * Return the internal SCOREP handle for a given MPI window handle.
 * @param  win MPI window handle.
 * @return Internal SCOREP handle for the given window.
 */
extern SCOREP_MPIWindowHandle
scorep_mpi_win_id( MPI_Win win );

/**
 * @internal
 * Create definition record and internal tracking handle.
 * @param win MPI window handle of the window to be tracked.
 * @param comm MPI communication handle the window is defined on.
 */
extern void
scorep_mpi_win_create( MPI_Win  win,
                       MPI_Comm comm );

/**
 * @internal
 * Free definition record and internal tracking handle.
 * @param win MPI window handle of the window to be freed.
 */
extern void
scorep_mpi_win_free( MPI_Win win );

/**
 * @internal
 * @brief Start tracking of epoch.
 * @param win   MPI window handle of related window.
 * @param group MPI group handle of related process group.
 * @param color Type of epoch (exposure=0, access=1).
 */
extern void
scorep_mpi_winacc_start( MPI_Win          win,
                         MPI_Group        group,
                         SCOREP_Mpi_Color color );

/**
 * @internal
 * @brief End tracking of epoch.
 * @param win   MPI window handle of related window.
 * @param color Type of epoch (exposure=0, access=1).
 */
extern void
scorep_mpi_winacc_end( MPI_Win          win,
                       SCOREP_Mpi_Color color );

/**
 * @internal
 * @brief  Get internal group ID of process group related to an epoch.
 * @param  win   MPI window handle of related window.
 * @param  color Type of epoch (exposure=0, access=1).
 * @return Internal process group handle.
 */
extern SCOREP_Mpi_GroupHandle
scorep_mpi_winacc_get_gid( MPI_Win          win,
                           SCOREP_Mpi_Color color );

#endif

/**
 * @internal
 * @brief Start tracking of a given MPI communicator.
 * makes the definition of the given communicator to the measurement system.
 * @param comm MPI communicator handle.
 */
extern void
scorep_mpi_comm_create( MPI_Comm comm,
                        MPI_Comm parent_comm );

/**
 * @internal
 * @brief  Stop tracking of an MPI communicator handle.
 * @param  comm Still valid MPI communicator that is to be freed by
 *              \a MPI_Comm_free.
 */
extern void
scorep_mpi_comm_free( MPI_Comm comm );

/**
 * @internal
 * @brief  Retrieves the internal SCOREP handle of a given MPI communicator.
 * @param  comm MPI communicator
 * @return Internal SCOREP handle of MPI communicator %comm
 */
extern SCOREP_LocalMPICommunicatorHandle
scorep_mpi_comm_handle( MPI_Comm comm );

/**
 * @internal
 * @brief  Sets the name of the communicator.
 * @param  comm MPI communicator
 * @param  name The name for the MPI communicator
 */
extern void
scorep_mpi_comm_set_name( MPI_Comm    comm,
                          const char* name );

/**
 * @internal
 * Translate a rank within a given communicator to its global rank in
 * \a MPI_COMM_WORLD.
 * @param  rank Local rank to be translated.
 * @param  comm Local communicator the rank refers to.
 * @return Global rank.
 */
extern SCOREP_MpiRank
scorep_mpi_rank_to_pe( SCOREP_MpiRank rank,
                       MPI_Comm       comm );

/**
 * @internal
 * Structure to hold the \a MPI_COMM_WORLD definition.
 */
struct scorep_mpi_world_type
{
    MPI_Group                         group;    /** Associated MPI group */
    int                               size;     /** Number of ranks */
    SCOREP_MpiRank*                   ranks;    /** Array which contains the rank numbers */
    SCOREP_LocalMPICommunicatorHandle handle;   /** SCOREP handle */
};

/**
 * Contains the data of the MPI_COMM_WORLD definition.
 */
extern struct scorep_mpi_world_type scorep_mpi_world;

/**
 * Flag that indicates whether comm/rank is determined for events
 */
//extern int8_t scorep_mpi_comm_determination;

/**
 * @def SCOREP_MPI_COMM_WORLD_HANDLE
 * The SCOREP communicator handle for MPI_COMM_WORLD.
 */
#define SCOREP_MPI_COMM_WORLD_HANDLE scorep_mpi_world.handle

/**
 * @def SCOREP_MPI_COMM_HANDLE
 * Translates a MPI communicator to the SCOREP communicator handle
 */
#define SCOREP_MPI_COMM_HANDLE( c ) ( ( c ) == MPI_COMM_WORLD ? SCOREP_MPI_COMM_WORLD_HANDLE : scorep_mpi_comm_handle( c ) )

#endif // SCOREP_MPI_COMMUNICATOR_H
