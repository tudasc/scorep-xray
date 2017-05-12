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
 * Copyright (c) 2009-2013, 2017,
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


#include "scorep_mpi_communicator_mgmt.h"
#include <SCOREP_DefinitionHandles.h>
#include <mpi.h>


/* Check MPI version. Disable RMA if MPI version 1 */
#if defined( MPI_VERSION ) && ( MPI_VERSION == 1 ) && !defined( SCOREP_MPI_NO_RMA )
  #define SCOREP_MPI_NO_RMA
#endif


/**
 * Defines the value for SCOREP_Mpi_GroupHandle which marks an invalid group
 */
#define SCOREP_INVALID_MPI_GROUP SCOREP_INVALID_GROUP


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
extern SCOREP_RmaWindowHandle
scorep_mpi_win_id( MPI_Win win );

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
 * @brief  Stop tracking of an MPI communicator handle.
 * @param  comm Still valid MPI communicator that is to be freed by
 *              \a MPI_Comm_free.
 */
extern void
scorep_mpi_comm_free( MPI_Comm comm );

/**
 * @internal
 * @brief  Sets the name of the communicator.
 * @param  comm MPI communicator
 * @param  name The name for the MPI communicator
 */
extern void
scorep_mpi_comm_set_name( MPI_Comm    comm,
                          const char* name );

extern void
scorep_mpi_comm_set_default_names( void );

#endif /* SCOREP_MPI_COMMUNICATOR_H */
