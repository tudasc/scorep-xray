/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
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
 * @brief Propertiy handling.
 *
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_DefinitionHandles.h>

#include "scorep_type_utils.h"
#include "scorep_status.h"

#include <tracing/SCOREP_Tracing.h>

#include "scorep_properties_management.h"
#include <SCOREP_Memory.h>

static struct scorep_property
{
    SCOREP_PropertyHandle    handle;
    SCOREP_PropertyCondition condition;
    bool                     initialValue;
}
scorep_properties[ SCOREP_PROPERTY_MAX ] =
{
    // SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE
    {
        .condition    = SCOREP_PROPERTY_CONDITION_ANY,
        .initialValue = true
    },
    // SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE
    {
        .condition    = SCOREP_PROPERTY_CONDITION_ANY,
        .initialValue = true
    },
    // SCOREP_PROPERTY_THREAD_CREATE_WAIT_EVENT_COMPLETE
    {
        .condition    = SCOREP_PROPERTY_CONDITION_ANY,
        .initialValue = true
    },
    // SCOREP_PROPERTY_THREAD_LOCK_EVENT_COMPLETE
    {
        .condition    = SCOREP_PROPERTY_CONDITION_ANY,
        .initialValue = true
    },
    // SCOREP_PROPERTY_PTHREAD_LOCATION_REUSED
    {
        .condition    = SCOREP_PROPERTY_CONDITION_ANY,
        .initialValue = false
    }
};

/**
 * TODO: check whether the call to scorep_properties_initialize can be done at a later time.
 *        If so, introduce a substrate mgmt array and register the initialize there.
 */

static void
substrate_initialize( size_t substrateId )
{
    /* use the variable but ignore it */
    ( void )substrateId;

    /* define properties */
    for ( int i = 0; i < SCOREP_PROPERTY_MAX; i++ )
    {
        scorep_properties[ i ].handle = SCOREP_Definitions_NewProperty(
            i,
            scorep_properties[ i ].condition,
            scorep_properties[ i ].initialValue );
    }
}


void
SCOREP_InvalidateProperty( SCOREP_Property property )
{
    UTILS_ASSERT( property < SCOREP_PROPERTY_MAX );
    SCOREP_PropertyDef* property_definition = SCOREP_LOCAL_HANDLE_DEREF(
        scorep_properties[ property ].handle,
        Property );
    property_definition->invalidated = true;
}


static void
mpi_send( struct SCOREP_Location*          location,
          uint64_t                         timestamp,
          SCOREP_MpiRank                   destinationRank,
          SCOREP_InterimCommunicatorHandle communicatorHandle,
          uint32_t                         tag,
          uint64_t                         bytesSent )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
}


static void
mpi_recv( struct SCOREP_Location*          location,
          uint64_t                         timestamp,
          SCOREP_MpiRank                   sourceRank,
          SCOREP_InterimCommunicatorHandle communicatorHandle,
          uint32_t                         tag,
          uint64_t                         bytesReceived )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
}


static void
mpi_collective_begin( struct SCOREP_Location* location,
                      uint64_t                timestamp )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
}


static void
mpi_collective_end( struct SCOREP_Location*          location,
                    uint64_t                         timestamp,
                    SCOREP_InterimCommunicatorHandle communicatorHandle,
                    SCOREP_MpiRank                   rootRank,
                    SCOREP_CollectiveType            collectiveType,
                    uint64_t                         bytesSent,
                    uint64_t                         bytesReceived )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
}


static void
mpi_irecv_request( struct SCOREP_Location* location,
                   uint64_t                timestamp,
                   SCOREP_MpiRequestId     requestId )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
}


static void
mpi_request_tested( struct SCOREP_Location* location,
                    uint64_t                timestamp,
                    SCOREP_MpiRequestId     requestId )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
}


static void
mpi_request_cancelled( struct SCOREP_Location* location,
                       uint64_t                timestamp,
                       SCOREP_MpiRequestId     requestId )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
}


static void
mpi_isend( struct SCOREP_Location*          location,
           uint64_t                         timestamp,
           SCOREP_MpiRank                   destinationRank,
           SCOREP_InterimCommunicatorHandle communicatorHandle,
           uint32_t                         tag,
           uint64_t                         bytesSent,
           SCOREP_MpiRequestId              requestId )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
}


static void
mpi_irecv( struct SCOREP_Location*          location,
           uint64_t                         timestamp,
           SCOREP_MpiRank                   sourceRank,
           SCOREP_InterimCommunicatorHandle communicatorHandle,
           uint32_t                         tag,
           uint64_t                         bytesReceived,
           SCOREP_MpiRequestId              requestId )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
}


static void
thread_acquire_lock( struct SCOREP_Location* location,
                     uint64_t                timestamp,
                     SCOREP_ParadigmType     paradigm,
                     uint32_t                lockId,
                     uint32_t                acquisitionOrder )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_LOCK_EVENT_COMPLETE );
}


static void
thread_release_lock( struct SCOREP_Location* location,
                     uint64_t                timestamp,
                     SCOREP_ParadigmType     paradigm,
                     uint32_t                lockId,
                     uint32_t                acquisitionOrder )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_LOCK_EVENT_COMPLETE );
}


static void
thread_fork( struct SCOREP_Location* location,
             uint64_t                timestamp,
             SCOREP_ParadigmType     paradigm,
             uint32_t                nRequestedThreads,
             uint32_t                forkSequenceCount )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
}


static void
thread_join( struct SCOREP_Location* location,
             uint64_t                timestamp,
             SCOREP_ParadigmType     paradigm )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
}


static void
thread_team_begin( struct SCOREP_Location*          location,
                   uint64_t                         timestamp,
                   SCOREP_ParadigmType              paradigm,
                   SCOREP_InterimCommunicatorHandle threadTeam )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
}


static void
thread_team_end( struct SCOREP_Location*          location,
                 uint64_t                         timestamp,
                 SCOREP_ParadigmType              paradigm,
                 SCOREP_InterimCommunicatorHandle threadTeam )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
}


static void
thread_task_create( struct SCOREP_Location*          location,
                    uint64_t                         timestamp,
                    SCOREP_ParadigmType              paradigm,
                    SCOREP_InterimCommunicatorHandle threadTeam,
                    uint32_t                         threadId,
                    uint32_t                         generationNumber )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
}


static void
thread_task_switch( struct SCOREP_Location*          location,
                    uint64_t                         timestamp,
                    uint64_t*                        metricValues,
                    SCOREP_ParadigmType              paradigm,
                    SCOREP_InterimCommunicatorHandle threadTeam,
                    uint32_t                         threadId,
                    uint32_t                         generationNumber,
                    SCOREP_TaskHandle                taskHandle )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
}


static void
thread_task_begin( struct SCOREP_Location*          location,
                   uint64_t                         timestamp,
                   SCOREP_RegionHandle              regionHandle,
                   uint64_t*                        metricValues,
                   SCOREP_ParadigmType              paradigm,
                   SCOREP_InterimCommunicatorHandle threadTeam,
                   uint32_t                         threadId,
                   uint32_t                         generationNumber,
                   SCOREP_TaskHandle                taskHandle )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
}


static void
thread_task_end( struct SCOREP_Location*          location,
                 uint64_t                         timestamp,
                 SCOREP_RegionHandle              regionHandle,
                 uint64_t*                        metricValues,
                 SCOREP_ParadigmType              paradigm,
                 SCOREP_InterimCommunicatorHandle threadTeam,
                 uint32_t                         threadId,
                 uint32_t                         generationNumber,
                 SCOREP_TaskHandle                taskHandle )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
}


static void
thread_create( struct SCOREP_Location*          location,
               uint64_t                         timestamp,
               SCOREP_ParadigmType              paradigm,
               SCOREP_InterimCommunicatorHandle threadTeam,
               uint32_t                         createSequenceCount )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_CREATE_WAIT_EVENT_COMPLETE );
}


static void
thread_wait( struct SCOREP_Location*          location,
             uint64_t                         timestamp,
             SCOREP_ParadigmType              paradigm,
             SCOREP_InterimCommunicatorHandle threadTeam,
             uint32_t                         createSequenceCount )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_CREATE_WAIT_EVENT_COMPLETE );
}


static void
thread_begin( struct SCOREP_Location*          location,
              uint64_t                         timestamp,
              SCOREP_ParadigmType              paradigm,
              SCOREP_InterimCommunicatorHandle threadTeam,
              uint32_t                         createSequenceCount )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_CREATE_WAIT_EVENT_COMPLETE );
}


static void
thread_end( struct SCOREP_Location*          location,
            uint64_t                         timestamp,
            SCOREP_ParadigmType              paradigm,
            SCOREP_InterimCommunicatorHandle threadTeam,
            uint32_t                         createSequenceCount )
{
    SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_CREATE_WAIT_EVENT_COMPLETE );
}


const static SCOREP_Substrates_Callback substrate_callbacks[ SCOREP_SUBSTRATES_NUM_EVENTS ] =
{
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiSend,                  MPI_SEND,                     mpi_send ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiRecv,                  MPI_RECV,                     mpi_recv ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiCollectiveBegin,       MPI_COLLECTIVE_BEGIN,         mpi_collective_begin ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiCollectiveEnd,         MPI_COLLECTIVE_END,           mpi_collective_end ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiIrecvRequest,          MPI_IRECV_REQUEST,            mpi_irecv_request ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiRequestTested,         MPI_REQUEST_TESTED,           mpi_request_tested ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiRequestCancelled,      MPI_REQUEST_CANCELLED,        mpi_request_cancelled ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiIsend,                 MPI_ISEND,                    mpi_isend ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiIrecv,                 MPI_IRECV,                    mpi_irecv ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadAcquireLock,        THREAD_ACQUIRE_LOCK,          thread_acquire_lock ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadReleaseLock,        THREAD_RELEASE_LOCK,          thread_release_lock ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinFork,       THREAD_FORK_JOIN_FORK,        thread_fork ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinJoin,       THREAD_FORK_JOIN_JOIN,        thread_join ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTeamBegin,  THREAD_FORK_JOIN_TEAM_BEGIN,  thread_team_begin ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTeamEnd,    THREAD_FORK_JOIN_TEAM_END,    thread_team_end ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTaskCreate, THREAD_FORK_JOIN_TASK_CREATE, thread_task_create ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTaskSwitch, THREAD_FORK_JOIN_TASK_SWITCH, thread_task_switch ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTaskBegin,  THREAD_FORK_JOIN_TASK_BEGIN,  thread_task_begin ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTaskEnd,    THREAD_FORK_JOIN_TASK_END,    thread_task_end ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitCreate,   THREAD_CREATE_WAIT_CREATE,    thread_create ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitWait,     THREAD_CREATE_WAIT_WAIT,      thread_wait ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitBegin,    THREAD_CREATE_WAIT_BEGIN,     thread_begin ),
    SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitEnd,      THREAD_CREATE_WAIT_END,       thread_end )
};



const static SCOREP_Substrates_Callback substrate_mgmt_callbacks[ SCOREP_SUBSTRATES_NUM_MGMT_EVENTS ] =
{
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( InitSubstrate, INIT_SUBSTRATE,       substrate_initialize )
};

/**
   Returns an array of property callbacks.
   @param mode Recording enabled/disabled.
   @return Array of property substrate callbacks for the requested mode.
 */
const SCOREP_Substrates_Callback*
scorep_properties_get_substrate_callbacks( void )
{
    return substrate_callbacks;
}

const SCOREP_Substrates_Callback*
scorep_properties_get_substrate_mgmt_callbacks( void )
{
    return substrate_mgmt_callbacks;
}
