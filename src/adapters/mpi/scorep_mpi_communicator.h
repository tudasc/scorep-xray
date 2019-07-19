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

#include <SCOREP_DefinitionHandles.h>

#include "scorep_mpi_communicator_mgmt.h"

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
 * Epoch type for tracking general active target synchronization
 */
enum SCOREP_Mpi_EpochType_enum
{
    SCOREP_MPI_RMA_ACCESS_EPOCH,
    SCOREP_MPI_RMA_EXPOSURE_EPOCH,
};

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
 * @brief Get group SCOREP handle for given MPI group
 * @param group MPI group handle
 * @return SCOREP group handle
 */
SCOREP_Mpi_GroupHandle
scorep_mpi_group_handle( MPI_Group group );

#ifndef SCOREP_MPI_NO_RMA

/**
 * @internal
 * Create window handle
 * @param name Name for window handle
 * @param win MPI window handle
 * @param comm MPI communicatior handle
 * @return SCOREP RMA window handle
 */
SCOREP_RmaWindowHandle
scorep_mpi_win_create( const char* name,
                       MPI_Win     win,
                       MPI_Comm    comm );

/**
 * @internal
 * Free window handle
 * @param win MPI window handle
 */
void
scorep_mpi_win_free( MPI_Win win );

/**
 * @internal
 * Return the internal SCOREP handle for a given MPI window handle.
 * @param  win MPI window handle.
 * @return Internal SCOREP handle for the given window.
 */
extern SCOREP_RmaWindowHandle
scorep_mpi_win_handle( MPI_Win win );


/**
 * @internal
 * @brief Set the name of the window
 * @param win   MPI window handle
 * @param name  New name to be set
 */
extern void
scorep_mpi_win_set_name( MPI_Win     win,
                         const char* name );

/**
 * @internal
 * @brief Start tracking of epoch.
 * @param win        MPI window handle of related window.
 * @param group      MPI group handle of related process group.
 * @param epoch_type Epoch Type (access vs. exposure)
 */
extern void
scorep_mpi_epoch_start( MPI_Win              win,
                        MPI_Group            group,
                        SCOREP_Mpi_EpochType epochType );

/**
 * @internal
 * @brief End tracking of epoch.
 * @param win   MPI window handle of related window.
 * @param epoch_type Epoch Type (access vs. exposure)
 */
extern void
scorep_mpi_epoch_end( MPI_Win              win,
                      SCOREP_Mpi_EpochType epochType );

/**
 * @internal
 * @brief  Get internal group ID of process group related to an epoch.
 * @param  win   MPI window handle of related window.
 * @param epoch_type Epoch Type (access vs. exposure)
 * @return Internal process group handle.
 */
extern SCOREP_Mpi_GroupHandle
scorep_mpi_epoch_get_group_handle( MPI_Win              win,
                                   SCOREP_Mpi_EpochType epochType );

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
