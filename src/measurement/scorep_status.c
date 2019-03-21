/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */



/**
 * @file
 *
 *
 */


#include <config.h>

#include "scorep_status.h"
#include "scorep_ipc.h"
#include <SCOREP_Config.h>
#include <SCOREP_Platform.h>
#include "scorep_environment.h"
#include "scorep_runtime_management_timings.h"

#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#define SCOREP_DEBUG_MODULE_NAME CORE
#include <UTILS_Debug.h>

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
    bool is_process_master_on_node;
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
    .is_process_master_on_node = false,
    .is_profiling_enabled      = true,
    .is_tracing_enabled        = true,
    .is_oa_enabled             = true,
    .otf2_has_flushed          = false
};

#if HAVE( UNWINDING_SUPPORT )
bool scorep_is_unwinding_enabled = 0;
#endif

void
SCOREP_Status_Initialize( void )
{
    scorep_process_local_status.is_profiling_enabled = SCOREP_Env_DoProfiling();
    scorep_process_local_status.is_tracing_enabled   = SCOREP_Env_DoTracing();
#if HAVE( UNWINDING_SUPPORT )
    scorep_is_unwinding_enabled = SCOREP_Env_DoUnwinding();
#endif
}


void
SCOREP_Status_Finalize( void )
{
    UTILS_DEBUG_ENTRY();
}


void
SCOREP_Status_OnMppInit( void )
{
    assert( !scorep_process_local_status.mpp_is_initialized );
    assert( !scorep_process_local_status.mpp_is_finalized );
    scorep_process_local_status.mpp_is_initialized = true;

    UTILS_DEBUG_ENTRY();
    SCOREP_Ipc_Init();

    assert( scorep_process_local_status.mpp_comm_world_size == 0 );
    scorep_process_local_status.mpp_comm_world_size = SCOREP_Ipc_GetSize();
    assert( scorep_process_local_status.mpp_comm_world_size > 0 );

    assert( !scorep_process_local_status.mpp_rank_is_set );
    scorep_process_local_status.mpp_rank = SCOREP_Ipc_GetRank();
    assert( scorep_process_local_status.mpp_rank >= 0 );
    assert( scorep_process_local_status.mpp_rank < scorep_process_local_status.mpp_comm_world_size );
    scorep_process_local_status.mpp_rank_is_set = true;

    /* Get node ID */
    uint32_t node_id = SCOREP_Platform_GetNodeId();

    /* Gather node IDs of all processes */
    uint32_t* recvbuf = malloc( scorep_process_local_status.mpp_comm_world_size * sizeof( uint32_t ) );
    assert( recvbuf );
    SCOREP_Ipc_Allgather( &node_id,               /* send buffer */
                          recvbuf,                /* receive buffer */
                          1,                      /* count */
                          SCOREP_IPC_UINT32_T );  /* data type */

    /* Assume we are master for this node */
    scorep_process_local_status.is_process_master_on_node = true;
    /* Check whether this assumption is correct */
    for ( uint32_t i = scorep_process_local_status.mpp_rank; i-- > 0; )
    {
        if ( recvbuf[ i ] == node_id )
        {
            /* There is a process with a lower rank on the same node.
             * This means we are not the master on this node. */
            scorep_process_local_status.is_process_master_on_node = false;
            break;
        }
    }
    free( recvbuf );
}


void
SCOREP_Status_OnMppFinalize( void )
{
    UTILS_DEBUG_ENTRY();

    scorep_timing_reduce_runtime_management_timings();

    SCOREP_Ipc_Finalize();

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
    if ( SCOREP_Status_IsMpp() )
    {
        return scorep_process_local_status.mpp_is_finalized;
    }
    return true;
}


bool
SCOREP_Status_IsProcessMasterOnNode( void )
{
    assert( scorep_process_local_status.mpp_is_initialized );
    return scorep_process_local_status.is_process_master_on_node;
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
