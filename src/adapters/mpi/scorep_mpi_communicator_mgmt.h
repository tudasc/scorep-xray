/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2018,
 * Technische Universitaet Dresden, Germany
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

extern uint64_t scorep_mpi_max_epochs;

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
 *  @def SCOREP_MPI_MAX_EPOCHS
 *  @internal
 *  Maximum amount of concurrently active access or exposure epochs per
 *  process.
 */
#define SCOREP_MPI_MAX_EPOCHS  scorep_mpi_max_epochs

/**
 * Type of internal SCOREP group handles.
 */
typedef SCOREP_GroupHandle SCOREP_Mpi_GroupHandle;

/**
 * Type of the different colors for MPI epochs. Its a boolean flag which can have
 * the values scorep_mpi_exp_epoch or scorep_mpi_acc_epoch.
 */
typedef uint8_t SCOREP_Mpi_EpochType;

/**
 *  @internal
 *  Structure to translate MPI window handles to internal SCOREP IDs.
 */
struct scorep_mpi_win_type
{
    MPI_Win                win;    /** MPI window handle */
    SCOREP_RmaWindowHandle handle; /** Internal SCOREP window handle */
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
struct scorep_mpi_epoch_info_type
{
    MPI_Win                win;        /* MPI window identifier */
    SCOREP_Mpi_GroupHandle handle;     /* SCOREP MPI group handle */
    SCOREP_Mpi_EpochType   epoch_type; /* flag to help distinguish accesses on same window */
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
    uint32_t comm_size;
    uint32_t local_rank;
    uint32_t global_root_rank;
    uint32_t root_id;
    uint32_t io_handle_counter;
} scorep_mpi_comm_definition_payload;

/**
 * Contains the data of the MPI_COMM_WORLD definition.
 */
extern struct scorep_mpi_world_type scorep_mpi_world;

/**
 *  @internal
 * structure for communicator tracking
 */
struct scorep_mpi_communicator_type
{
    MPI_Comm                         comm; /**< MPI Communicator handle */
    SCOREP_InterimCommunicatorHandle cid;  /**< Internal SCOREP Communicator handle */
};

/**
 * @internal
 * structure for group tracking
 */
struct scorep_mpi_group_type
{
    MPI_Group              group;  /**< MPI group handle */
    SCOREP_Mpi_GroupHandle handle; /**< Internal SCOREP group handle */
    int32_t                refcnt; /**< Number of references to this group */
};

extern uint64_t scorep_mpi_max_communicators;
/**
 *  @def SCOREP_MPI_MAX_COMM
 *  @internal
 *  Maximum amount of concurrently defined communicators per process.
 */
#define SCOREP_MPI_MAX_COMM    scorep_mpi_max_communicators

extern uint64_t scorep_mpi_max_groups;
/**
 *  @def SCOREP_MPI_MAX_GROUP
 *  @internal
 *  Maximum amount of concurrently defined groups per process.
 */
#define SCOREP_MPI_MAX_GROUP   scorep_mpi_max_groups

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

extern int32_t scorep_mpi_last_comm;

extern struct scorep_mpi_group_type* scorep_mpi_groups;

extern struct scorep_mpi_communicator_type* scorep_mpi_comms;

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
 * @brief Start tracking of a given MPI communicator.
 * makes the definition of the given communicator to the measurement system.
 * @param comm       MPI communicator handle.
 * @param parentComm Possible MPI communicator parent.
 */
extern void
scorep_mpi_comm_create( MPI_Comm comm,
                        MPI_Comm parentComm );

/**
 * @internal
 * @brief Finalize the creation of an MPI communicator to track.
 * @param comm             MPI communicator handle.
 * @param parentCommHandle Possible MPI communicator parent as Score-P handle.
 */
extern void
scorep_mpi_comm_create_finalize( MPI_Comm                         comm,
                                 SCOREP_InterimCommunicatorHandle parentCommHandle );

/**
 * @internal
 * @brief  Retrieves the internal SCOREP handle of a given MPI communicator.
 * @param  comm MPI communicator
 * @return Internal SCOREP handle of MPI communicator %comm
 */
extern SCOREP_InterimCommunicatorHandle
scorep_mpi_comm_handle( MPI_Comm comm );

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
 * Unifies the MPI communicators.
 *
 * This function lives under measurement/paradigm to have access to the
 * definition internals.
 */
void
scorep_mpi_unify_communicators( void );


#endif /* SCOREP_MPI_COMMUNICATOR_MGMT_H */
