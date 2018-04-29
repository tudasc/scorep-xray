/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief Declaration of MPI profiling functions used in libscorep_adapter_mpi_mgmt
 */

#ifndef SCOREP_MPI_OA_PROFILE_MGMT_H
#define SCOREP_MPI_OA_PROFILE_MGMT_H

#include <mpi.h>

#define _WITH_PREALLOCATION_OF_TIME_PACKS

/**
 * @internal
 * Structure to hold the \a MPI_COMM_WORLD duplication used in mpi profiling.
 */
typedef struct scorep_mpiprofile_world_comm_dup_struct
{
    MPI_Group group;                            /** Associated MPI group */
    MPI_Comm  comm;
} scorep_mpiprofile_world_comm_dup;


/**
 * Contains the data of the MPI_COMM_WORLD definition.
 */
extern scorep_mpiprofile_world_comm_dup scorep_mpiprofiling_world_comm_dup;

extern int scorep_mpiprofiling_initialized;

extern int scorep_mpiprofiling_myrank;

void
scorep_mpiprofile_init( void );

#endif // SCOREP_MPI_OA_PROFILE_MGMT_H
