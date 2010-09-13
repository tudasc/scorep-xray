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
 * @file       silc_status.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include "silc_status.h"
#include "silc_mpi.h"
#include <SILC_Config.h>
#include "silc_environment.h"

#include <limits.h>
#include <assert.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


typedef struct silc_status silc_status;
struct silc_status
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


silc_status status = {
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
silc_status_initialize_common()
{
    // environment variables already processed. can we check this somehow?

    // These two variables may become public in future due to performance reasons.
    status.is_profiling_enabled = SILC_Env_DoProfiling();
    status.is_tracing_enabled   = SILC_Env_DoTracing();
}


void
silc_status_initialize_mpi()
{
    // nothing to to here
}


void
silc_status_initialize_non_mpi()
{
    status.mpi_rank            = 0;
    status.mpi_rank_is_set     = true;
    status.mpi_is_initialized  = true;
    status.mpi_is_finalized    = true;
    status.mpi_comm_world_size = 1;
}


void
SILC_OnPMPI_Init()
{
    assert( !status.mpi_is_initialized );
    assert( !status.mpi_is_finalized );
    status.mpi_is_initialized = true;

    SILC_Mpi_DuplicateCommWorld(); // call before SILC_Mpi_CalculateCommWorldSize()

    assert( status.mpi_comm_world_size == 0 );
    status.mpi_comm_world_size = SILC_Mpi_CalculateCommWorldSize();
    assert( status.mpi_comm_world_size > 0 );
}


void
SILC_OnPMPI_Finalize()
{
    assert( status.mpi_is_initialized );
    assert( !status.mpi_is_finalized );
    status.mpi_is_finalized = true;
}


void
SILC_Mpi_SetRankTo( int rank )
{
    assert( !status.mpi_rank_is_set );
    assert( status.mpi_is_initialized );
    assert( !status.mpi_is_finalized );
    assert( rank >= 0 );
    status.mpi_rank        = rank;
    status.mpi_rank_is_set = true;
}


int
SILC_Mpi_GetRank()
{
    assert( status.mpi_rank_is_set );
    return status.mpi_rank;
}


bool
SILC_Mpi_IsInitialized()
{
    return status.mpi_is_initialized;
}


bool
SILC_Mpi_IsFinalized()
{
    return status.mpi_is_finalized;
}


int
SILC_Mpi_GetCommWorldSize()
{
    assert( status.mpi_is_initialized );
    return status.mpi_comm_world_size;
}


bool
SILC_IsTracingEnabled()
{
    return status.is_tracing_enabled;
}


bool
SILC_IsProfilingEnabled()
{
    return status.is_profiling_enabled;
}

bool
SILC_IsOAEnabled()
{
    return status.is_oa_enabled;
}

void
SILC_Otf2_OnFlush()
{
    status.otf2_has_flushed = true;
}


bool
SILC_Otf2_HasFlushed()
{
    return status.otf2_has_flushed;
}


bool
SILC_IsExperimentDirCreated()
{
    return status.is_experiment_dir_created;
}


void
SILC_OnExperimentDirCreation()
{
    assert( !status.is_experiment_dir_created );
    status.is_experiment_dir_created = true;
}
