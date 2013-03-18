/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       src/measurement/scorep_status.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include "scorep_status.h"
#include "scorep_ipc.h"
#include <SCOREP_Config.h>
#include "scorep_environment.h"
#include "scorep_runtime_management_timings.h"

#include <limits.h>
#include <assert.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


typedef struct scorep_status scorep_status;
struct scorep_status
{
    int  mpp_rank;
    bool mpp_rank_is_set;
    bool mpp_is_initialized;
    bool mpp_is_finalized;
    int  mpp_comm_world_size;
    bool is_experiment_dir_created;
    bool is_profiling_enabled;
    bool is_tracing_enabled;
    bool is_oa_enabled;
    bool otf2_has_flushed;
};


static scorep_status scorep_process_local_status = {
    .mpp_rank                  = INT_MAX,
    .mpp_rank_is_set           = false,
    .mpp_is_initialized        = false,
    .mpp_is_finalized          = false,
    .mpp_comm_world_size       = 0,
    .is_experiment_dir_created = false,
    .is_profiling_enabled      = true,
    .is_tracing_enabled        = true,
    .is_oa_enabled             = true,
    .otf2_has_flushed          = false
};


void
SCOREP_Status_Initialize( void )
{
    scorep_process_local_status.is_profiling_enabled = SCOREP_Env_DoProfiling();
    scorep_process_local_status.is_tracing_enabled   = SCOREP_Env_DoTracing();

    // Lets see if we have an IPC, ie multi program paradigm
    if ( !SCOREP_Status_IsMpp() )
    {
        scorep_process_local_status.mpp_rank            = 0;
        scorep_process_local_status.mpp_rank_is_set     = true;
        scorep_process_local_status.mpp_is_initialized  = true;
        scorep_process_local_status.mpp_is_finalized    = true;
        scorep_process_local_status.mpp_comm_world_size = 1;
    }
}


void
SCOREP_Status_Finalize()
{
    SCOREP_Ipc_Finalize();
}


void
SCOREP_Status_OnMppInit( void )
{
    assert( !scorep_process_local_status.mpp_is_initialized );
    assert( !scorep_process_local_status.mpp_is_finalized );
    scorep_process_local_status.mpp_is_initialized = true;

    SCOREP_Ipc_Init();

    assert( scorep_process_local_status.mpp_comm_world_size == 0 );
    scorep_process_local_status.mpp_comm_world_size = SCOREP_Ipc_GetSize();
    assert( scorep_process_local_status.mpp_comm_world_size > 0 );

    assert( !scorep_process_local_status.mpp_rank_is_set );
    scorep_process_local_status.mpp_rank = SCOREP_Ipc_GetRank();
    assert( scorep_process_local_status.mpp_rank >= 0 );
    assert( scorep_process_local_status.mpp_rank < scorep_process_local_status.mpp_comm_world_size );
    scorep_process_local_status.mpp_rank_is_set = true;
}


void
SCOREP_Status_OnMppFinalize( void )
{
    scorep_timing_reduce_runtime_management_timings();

    assert( scorep_process_local_status.mpp_is_initialized );
    assert( !scorep_process_local_status.mpp_is_finalized );
    scorep_process_local_status.mpp_is_finalized = true;
}


int
SCOREP_Status_GetSize( void )
{
    assert( scorep_process_local_status.mpp_is_initialized );
    return scorep_process_local_status.mpp_comm_world_size;
}


int
SCOREP_Status_GetRank( void )
{
    assert( scorep_process_local_status.mpp_rank_is_set );
    return scorep_process_local_status.mpp_rank;
}


bool
SCOREP_Status_IsMppInitialized( void )
{
    return scorep_process_local_status.mpp_is_initialized;
}


bool
SCOREP_Status_IsMppFinalized( void )
{
    return scorep_process_local_status.mpp_is_finalized;
}


bool
SCOREP_IsTracingEnabled( void )
{
    return scorep_process_local_status.is_tracing_enabled;
}


bool
SCOREP_IsProfilingEnabled( void )
{
    return scorep_process_local_status.is_profiling_enabled;
}

bool
SCOREP_IsOAEnabled( void )
{
    return scorep_process_local_status.is_oa_enabled;
}

void
SCOREP_Status_OnOtf2Flush( void )
{
    scorep_process_local_status.otf2_has_flushed = true;
}


bool
SCOREP_Status_HasOtf2Flushed( void )
{
    return scorep_process_local_status.otf2_has_flushed;
}


bool
SCOREP_Status_IsExperimentDirCreated( void )
{
    return scorep_process_local_status.is_experiment_dir_created;
}


void
SCOREP_OnExperimentDirCreation( void )
{
    assert( !scorep_process_local_status.is_experiment_dir_created );
    scorep_process_local_status.is_experiment_dir_created = true;
}
