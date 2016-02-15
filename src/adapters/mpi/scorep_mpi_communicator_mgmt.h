/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef SCOREP_MPI_COMMUNICATOR_MGMT_H
#define SCOREP_MPI_COMMUNICATOR_MGMT_H

/**
 * @file
 */

#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Types.h>

#include <mpi.h>
#include <stdbool.h>

extern uint64_t scorep_mpi_max_access_epochs;

/**
 * @internal
 * Counts the number of communicators which have this rank as root and
 * are not equal to \a MPI_COMM_SELF
 */
extern uint32_t scorep_mpi_number_of_root_comms;

/**
 * @internal
 * Counts the number of communicators at this process which are equal to
 * \a MPI_COMM_SELF.
 */
extern uint32_t scorep_mpi_number_of_self_comms;

/**
 *  @def SCOREP_MPI_MAX_WINACC
 *  @internal
 *  Maximum amount of concurrently active access or exposure epochs per
 *  process.
 */
#define SCOREP_MPI_MAX_WINACC  scorep_mpi_max_access_epochs

/**
 * Type of internal SCOREP group handles.
 */
typedef SCOREP_GroupHandle SCOREP_Mpi_GroupHandle;

/**
 * Type of the different colors for MPI epochs. Its a boolean flag which can have
 * the values scorep_mpi_exp_epoch or scorep_mpi_acc_epoch.
 */
typedef uint8_t SCOREP_Mpi_Color;

/**
 *  @internal
 *  Structure to translate MPI window handles to internal SCOREP IDs.
 */
struct scorep_mpi_win_type
{
    MPI_Win                       win; /** MPI window handle */
    SCOREP_InterimRmaWindowHandle wid; /** Internal SCOREP window handle */
};

/**
 *  @internal
 *  Array which maps all MPI windows to Score-P internal handles.
 */
extern struct scorep_mpi_win_type* scorep_mpi_windows;

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
 * @internal
 * Structure to hold the \a MPI_COMM_WORLD definition.
 */
struct scorep_mpi_world_type
{
    MPI_Group                        group;    /** Associated MPI group */
    int                              size;     /** Number of ranks */
    SCOREP_MpiRank*                  ranks;    /** Array which contains the rank numbers */
    SCOREP_InterimCommunicatorHandle handle;   /** SCOREP handle */
};

typedef struct scorep_mpi_comm_definition_payload
{
    bool     is_self_like;
    uint32_t local_rank;
    uint32_t global_root_rank;
    uint32_t root_id;
} scorep_mpi_comm_definition_payload;

/**
 * Contains the data of the MPI_COMM_WORLD definition.
 */
extern struct scorep_mpi_world_type scorep_mpi_world;

/**
 * Initializes the window handling specific data structures.
 */
void
scorep_mpi_win_init( void );

/**
 * Finalizes the window handling specific data structures.
 */
void
scorep_mpi_win_finalize( void );


/**
 * @internal
 * @brief Cleanup communicator management.
 */
void
scorep_mpi_comm_finalize( void );

/**
 * @internal
 * @brief  Defines the group of MPI locations.
 */
void
scorep_mpi_unify_define_mpi_locations( void );

/**
 * @internal
 * Unifies the MPI communicators.
 *
 * This function lives under measurement/paradigm to have access to the
 * definition internals.
 */
void
scorep_mpi_unify_communicators( void );


#endif /* SCOREP_MPI_COMMUNICATOR_MGMT_H */
