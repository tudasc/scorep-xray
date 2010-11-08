/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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


scorep_status status = {
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
    status.is_profiling_enabled = SCOREP_Env_DoProfiling();
    status.is_tracing_enabled   = SCOREP_Env_DoTracing();
}


void
scorep_status_initialize_mpi()
{
    // nothing to to here
}


void
scorep_status_initialize_non_mpi()
{
    status.mpi_rank            = 0;
    status.mpi_rank_is_set     = true;
    status.mpi_is_initialized  = true;
    status.mpi_is_finalized    = true;
    status.mpi_comm_world_size = 1;
}


void
SCOREP_OnPMPI_Init()
{
    assert( !status.mpi_is_initialized );
    assert( !status.mpi_is_finalized );
    status.mpi_is_initialized = true;

    SCOREP_Mpi_DuplicateCommWorld(); // call before SCOREP_Mpi_CalculateCommWorldSize()

    assert( status.mpi_comm_world_size == 0 );
    status.mpi_comm_world_size = SCOREP_Mpi_CalculateCommWorldSize();
    assert( status.mpi_comm_world_size > 0 );
}


void
SCOREP_OnPMPI_Finalize()
{
    assert( status.mpi_is_initialized );
    assert( !status.mpi_is_finalized );
    status.mpi_is_finalized = true;
}


void
SCOREP_Mpi_SetRankTo( int rank )
{
    assert( !status.mpi_rank_is_set );
    assert( status.mpi_is_initialized );
    assert( !status.mpi_is_finalized );
    assert( rank >= 0 );
    status.mpi_rank        = rank;
    status.mpi_rank_is_set = true;
}


int
SCOREP_Mpi_GetRank()
{
    assert( status.mpi_rank_is_set );
    return status.mpi_rank;
}


bool
SCOREP_Mpi_IsInitialized()
{
    return status.mpi_is_initialized;
}


bool
SCOREP_Mpi_IsFinalized()
{
    return status.mpi_is_finalized;
}


int
SCOREP_Mpi_GetCommWorldSize()
{
    assert( status.mpi_is_initialized );
    return status.mpi_comm_world_size;
}


bool
SCOREP_IsTracingEnabled()
{
    return status.is_tracing_enabled;
}


bool
SCOREP_IsProfilingEnabled()
{
    return status.is_profiling_enabled;
}

bool
SCOREP_IsOAEnabled()
{
    return status.is_oa_enabled;
}

void
SCOREP_Otf2_OnFlush()
{
    status.otf2_has_flushed = true;
}


bool
SCOREP_Otf2_HasFlushed()
{
    return status.otf2_has_flushed;
}


bool
SCOREP_IsExperimentDirCreated()
{
    return status.is_experiment_dir_created;
}


void
SCOREP_OnExperimentDirCreation()
{
    assert( !status.is_experiment_dir_created );
    status.is_experiment_dir_created = true;
}
