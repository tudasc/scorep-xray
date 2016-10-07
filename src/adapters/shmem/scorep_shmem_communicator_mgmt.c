/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 * @ingroup SHMEM_Wrapper
 */


#include <config.h>

#include <stdio.h>

#include <scorep_shmem_internal.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>
#include <UTILS_CStr.h>
#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME SHMEM
#include <UTILS_Debug.h>

#include <jenkins_hash.h>

#ifndef CALL_SHMEM
#error Macro 'CALL_SHMEM' is not defined
#endif

/**
 * All interim communicator definitions for the SHMEM PE groups.
 * They have a 1-on-1 relation with the RMA windows.
 */
scorep_definitions_manager_entry scorep_shmem_pe_groups;

static long* barrier_psync;
static long* bcast_psync;

/**
 *  Tracking of 'WORLD' and 'SELF' window
 */
SCOREP_RmaWindowHandle scorep_shmem_world_window_handle = SCOREP_INVALID_RMA_WINDOW;
SCOREP_RmaWindowHandle scorep_shmem_self_window_handle  = SCOREP_INVALID_RMA_WINDOW;

/* ***************************************************************************************
   Prototypes of auxiliary functions
*****************************************************************************************/

static SCOREP_InterimCommunicatorHandle
define_comm( int                                    start,
             int                                    stride,
             int                                    size,
             scorep_shmem_comm_definition_payload** payload );

/* ***************************************************************************************
   Define interim 'WORLD' communicator and RMA window
*****************************************************************************************/

#define WIN_WORLD_NAME "All PEs"
#define WIN_SELF_NAME  "Self PE"

void
scorep_shmem_setup_comm_world( void )
{
    UTILS_BUG_ON( scorep_shmem_number_of_pes == 0,
                  "Can't allocate buffers for 0 PEs." );

    barrier_psync = CALL_SHMEM( shmalloc )( sizeof( long ) * _SHMEM_BARRIER_SYNC_SIZE );
    UTILS_ASSERT( barrier_psync );
    for ( int i = 0; i < _SHMEM_BARRIER_SYNC_SIZE; i++ )
    {
        barrier_psync[ i ] = _SHMEM_SYNC_VALUE;
    }

    bcast_psync = CALL_SHMEM( shmalloc )( sizeof( long ) * _SHMEM_BCAST_SYNC_SIZE );
    UTILS_ASSERT( bcast_psync );
    for ( int i = 0; i < _SHMEM_BCAST_SYNC_SIZE; i++ )
    {
        bcast_psync[ i ] = _SHMEM_SYNC_VALUE;
    }

    CALL_SHMEM( shmem_barrier_all )();

    /* Define the group of all SHMEM locations. */
    scorep_shmem_define_shmem_locations();

    scorep_definitions_manager_init_entry( &scorep_shmem_pe_groups );
    scorep_definitions_manager_entry_alloc_hash_table( &scorep_shmem_pe_groups,
                                                       5 /* 32 hash buckets */ );

    /* Create interim 'WORLD' communicator once for a process */
    scorep_shmem_comm_definition_payload* comm_world_payload = NULL;
    SCOREP_InterimCommunicatorHandle      comm_world_handle  =
        define_comm( 0, 0, scorep_shmem_number_of_pes, &comm_world_payload );

    /* Create 'WORLD' window handle once for a process */
    scorep_shmem_world_window_handle =
        SCOREP_Definitions_NewRmaWindow( WIN_WORLD_NAME, comm_world_handle );
    comm_world_payload->rma_win = scorep_shmem_world_window_handle;

    SCOREP_RmaWinCreate( scorep_shmem_world_window_handle );

    if ( scorep_shmem_number_of_pes > 1 )
    {
        /* Create interim 'SELF' communicator once for a process */
        scorep_shmem_comm_definition_payload* comm_self_payload = NULL;
        SCOREP_InterimCommunicatorHandle      comm_self_handle  =
            define_comm( scorep_shmem_my_rank, 0, 1, &comm_self_payload );

        /* Create 'SELF' window handle once for a process */
        scorep_shmem_self_window_handle =
            SCOREP_Definitions_NewRmaWindow( WIN_SELF_NAME, comm_self_handle );
        comm_self_payload->rma_win = scorep_shmem_self_window_handle;

        SCOREP_RmaWinCreate( scorep_shmem_self_window_handle );
    }
    else
    {
        scorep_shmem_self_window_handle = scorep_shmem_world_window_handle;
    }

    scorep_shmem_rma_op_matching_id = 0;
}

void
scorep_shmem_teardown_comm_world( void )
{
    UTILS_ASSERT( barrier_psync );
    CALL_SHMEM( shfree )( barrier_psync );
    barrier_psync = NULL;

    UTILS_ASSERT( bcast_psync );
    CALL_SHMEM( shfree )( bcast_psync );
    bcast_psync = NULL;

    CALL_SHMEM( shmem_barrier_all )();

    free( scorep_shmem_pe_groups.hash_table );
}

/**
 * Trigger writing of RmaWinDestroy record for each window in the hash table.
 */
void
scorep_shmem_close_pe_group( void )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_BEGIN(
        &scorep_shmem_pe_groups,
        InterimCommunicator,
        SCOREP_Memory_GetLocalDefinitionPageManager() )
    {
        if ( definition->paradigm_type != SCOREP_PARADIGM_SHMEM )
        {
            continue;
        }

        scorep_shmem_comm_definition_payload* payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( handle );
        SCOREP_RmaWinDestroy( payload->rma_win );
    }
    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_END();
}

/**
 * Register a group of processing elements to the measurement system
 *
 * @param start                 The lowest processing element (PE) number
 *                              of the active set of PEs
 * @param stride                The log (base 2) of the stride between
 *                              consecutive PE numbers in the active set
 * @param size                  The number of PEs in the active set
 *
 * @return Handle of corresponding interim RMA window.
 */
SCOREP_RmaWindowHandle
scorep_shmem_get_pe_group( int start,
                           int stride,
                           int size )
{
    /* Check for group of all processing elements */
    if ( start == 0 && stride == 0 && size == scorep_shmem_number_of_pes )
    {
        return scorep_shmem_world_window_handle;
    }

    /* We map all self-like PE groups into one, regardless of 'stride' */
    if ( start == scorep_shmem_my_rank && size == 1 )
    {
        return scorep_shmem_self_window_handle;
    }

    scorep_shmem_comm_definition_payload* new_payload = NULL;
    SCOREP_InterimCommunicatorHandle      new_handle  =
        define_comm( start, stride, size, &new_payload );

    if ( new_payload )
    {
        char name[ 48 ];
        snprintf( name, sizeof( name ), "Active set %d:%d:%d", start, stride, size );

        /* First time we encounter this PE group */
        new_payload->rma_win =
            SCOREP_Definitions_NewRmaWindow( name, new_handle );

        SCOREP_RmaWinCreate( new_payload->rma_win );
    }
    else
    {
        new_payload = SCOREP_InterimCommunicatorHandle_GetPayload( new_handle );
    }

    return new_payload->rma_win;
}


/* ***************************************************************************************
   Auxiliary functions
*****************************************************************************************/


static uint32_t
init_payload_fn( void* payload_, uint32_t hashValue, va_list va )
{
    scorep_shmem_comm_definition_payload* payload = payload_;

    payload->pe_start = va_arg( va, int );
    hashValue         = jenkins_hash(
        &payload->pe_start,
        sizeof( payload->pe_start ),
        hashValue );

    payload->log_pe_stride = va_arg( va, int );
    hashValue              = jenkins_hash(
        &payload->log_pe_stride,
        sizeof( payload->log_pe_stride ),
        hashValue );

    payload->pe_size = va_arg( va, int );
    hashValue        = jenkins_hash(
        &payload->pe_size,
        sizeof( payload->pe_size ),
        hashValue );

    payload->rma_win = SCOREP_INVALID_RMA_WINDOW;

    return hashValue;
}

static bool
equal_payloads_fn( const void* payloadA_,
                   const void* payloadB_ )
{
    const scorep_shmem_comm_definition_payload* payloadA = payloadA_;
    const scorep_shmem_comm_definition_payload* payloadB = payloadB_;

    return payloadA->pe_start      == payloadB->pe_start &&
           payloadA->log_pe_stride == payloadB->log_pe_stride &&
           payloadA->pe_size       == payloadB->pe_size;
}


static SCOREP_InterimCommunicatorHandle
define_comm( int                                    start,
             int                                    stride,
             int                                    size,
             scorep_shmem_comm_definition_payload** payload )
{
    return SCOREP_Definitions_NewInterimCommunicatorCustom(
        NULL,
        &scorep_shmem_pe_groups,
        init_payload_fn,
        equal_payloads_fn,
        SCOREP_INVALID_INTERIM_COMMUNICATOR,
        SCOREP_PARADIGM_SHMEM,
        sizeof( **payload ),
        ( void** )payload,
        start,
        stride,
        size );
}
