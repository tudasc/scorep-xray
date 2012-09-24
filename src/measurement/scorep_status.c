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
 * @file       scorep_status.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include "scorep_status.h"
#include "scorep_mpi.h"
#include <SCOREP_Config.h>
#include "scorep_environment.h"

#include <limits.h>
#include <assert.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


typedef struct scorep_status scorep_status;
struct scorep_status
{
    int  mpi_rank;
    bool mpi_rank_is_set;
    bool mpi_is_initialized;
    bool mpi_is_finalized;
    int  mpi_comm_world_size;
    bool is_experiment_dir_created;
    bool is_profiling_enabled;
    bool is_tracing_enabled;
    bool is_oa_enabled;
    bool otf2_has_flushed;
};


static scorep_status scorep_process_local_status = {
    INT_MAX,                              // mpi_rank
    false,                                // mpi_rank_is_set
    false,                                // mpi_is_initialized
    false,                                // mpi_is_finalized
    0,                                    // mpi_comm_world_size
    false,                                // is_experiment_dir_created
    true,                                 // is_profiling_enabled
    true,                                 // is_tracing_enabled
    true,                                 // is_oa_enabled
    false                                 // otf2_has_flushed
};


void
scorep_status_initialize_common()
{
    // environment variables already processed. can we check this somehow?

    // These two variables may become public in future due to performance reasons.
    scorep_process_local_status.is_profiling_enabled = SCOREP_Env_DoProfiling();
    scorep_process_local_status.is_tracing_enabled   = SCOREP_Env_DoTracing();
}


void
scorep_status_initialize_mpi()
{
    // nothing to to here
}


void
scorep_status_initialize_non_mpi()
{
    scorep_process_local_status.mpi_rank            = 0;
    scorep_process_local_status.mpi_rank_is_set     = true;
    scorep_process_local_status.mpi_is_initialized  = true;
    scorep_process_local_status.mpi_is_finalized    = true;
    scorep_process_local_status.mpi_comm_world_size = 1;
}


void
SCOREP_OnPMPI_Init()
{
    assert( !scorep_process_local_status.mpi_is_initialized );
    assert( !scorep_process_local_status.mpi_is_finalized );
    scorep_process_local_status.mpi_is_initialized = true;

    SCOREP_Mpi_DuplicateCommWorld(); // call before SCOREP_Mpi_CalculateCommWorldSize()

    assert( scorep_process_local_status.mpi_comm_world_size == 0 );
    scorep_process_local_status.mpi_comm_world_size = SCOREP_Mpi_CalculateCommWorldSize();
    assert( scorep_process_local_status.mpi_comm_world_size > 0 );
}


void
SCOREP_OnPMPI_Finalize()
{
    assert( scorep_process_local_status.mpi_is_initialized );
    assert( !scorep_process_local_status.mpi_is_finalized );
    scorep_process_local_status.mpi_is_finalized = true;
}


void
SCOREP_Mpi_SetRankTo( int rank )
{
    assert( !scorep_process_local_status.mpi_rank_is_set );
    assert( scorep_process_local_status.mpi_is_initialized );
    assert( !scorep_process_local_status.mpi_is_finalized );
    assert( rank >= 0 );
    scorep_process_local_status.mpi_rank        = rank;
    scorep_process_local_status.mpi_rank_is_set = true;
}


int
SCOREP_Mpi_GetRank()
{
    assert( scorep_process_local_status.mpi_rank_is_set );
    return scorep_process_local_status.mpi_rank;
}


bool
SCOREP_Mpi_IsInitialized()
{
    return scorep_process_local_status.mpi_is_initialized;
}


bool
SCOREP_Mpi_IsFinalized()
{
    return scorep_process_local_status.mpi_is_finalized;
}


int
SCOREP_Mpi_GetCommWorldSize()
{
    assert( scorep_process_local_status.mpi_is_initialized );
    return scorep_process_local_status.mpi_comm_world_size;
}


bool
SCOREP_IsTracingEnabled()
{
    return scorep_process_local_status.is_tracing_enabled;
}


bool
SCOREP_IsProfilingEnabled()
{
    return scorep_process_local_status.is_profiling_enabled;
}

bool
SCOREP_IsOAEnabled()
{
    return scorep_process_local_status.is_oa_enabled;
}

void
SCOREP_Otf2_OnFlush()
{
    scorep_process_local_status.otf2_has_flushed = true;
}


bool
SCOREP_Otf2_HasFlushed()
{
    return scorep_process_local_status.otf2_has_flushed;
}


bool
SCOREP_IsExperimentDirCreated()
{
    return scorep_process_local_status.is_experiment_dir_created;
}


void
SCOREP_OnExperimentDirCreation()
{
    assert( !scorep_process_local_status.is_experiment_dir_created );
    scorep_process_local_status.is_experiment_dir_created = true;
}
