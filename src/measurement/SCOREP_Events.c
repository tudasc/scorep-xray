/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
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
 * @brief   Declaration of event recording functions to be used by the
 *          adapter layer.
 *
 *
 */

#include <config.h>

#include <SCOREP_Events.h>
#include <scorep_substrates_definition.h>

#include <stdlib.h>
#include <stdio.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Properties.h>
#include <tracing/SCOREP_Tracing_Events.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Profile_Tasking.h>
#include <SCOREP_Profile_MpiEvents.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Task.h>

#include "scorep_events_common.h"
#include "scorep_runtime_management.h"
#include "scorep_types.h"


/**
 * Process a region enter event in the measurement system.
 */
static inline void
enter_region( SCOREP_Location*    location,
              uint64_t            timestamp,
              SCOREP_RegionHandle regionHandle,
              uint64_t*           metricValues )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%u",
                        SCOREP_Definitions_HandleToId( regionHandle ) );

    SCOREP_Task_Enter( location, regionHandle );

    SCOREP_CALL_SUBSTRATE( EnterRegion, ENTER_REGION,
                           ( location, timestamp, regionHandle, metricValues ) )
}


void
SCOREP_EnterRegion( SCOREP_RegionHandle regionHandle )
{
    SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp     = scorep_get_timestamp( location );
    uint64_t*        metric_values = SCOREP_Metric_Read( location );

    enter_region( location, timestamp, regionHandle, metric_values );
}


/*
 * NOTE: If dense metrics are used in the profile,
 * they need to be specified for these two functions!
 */
void
SCOREP_Location_EnterRegion( SCOREP_Location*    location,
                             uint64_t            timestamp,
                             SCOREP_RegionHandle regionHandle )
{
    UTILS_BUG_ON( location && SCOREP_Location_GetType( location ) == SCOREP_LOCATION_TYPE_CPU_THREAD,
                  "SCOREP_Location_EnterRegion() must not be used for CPU thread locations." );

    if ( !location )
    {
        location = SCOREP_Location_GetCurrentCPULocation();
    }

    SCOREP_Location_SetLastTimestamp( location, timestamp );

    uint64_t* metric_values = SCOREP_Metric_Read( location );

    enter_region( location, timestamp, regionHandle, metric_values );
}


/**
 * Process a region exit event in the measurement system.
 */
static inline void
exit_region( SCOREP_Location*    location,
             uint64_t            timestamp,
             SCOREP_RegionHandle regionHandle,
             uint64_t*           metricValues )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%u",
                        SCOREP_Definitions_HandleToId( regionHandle ) );

    SCOREP_CALL_SUBSTRATE( ExitRegion, EXIT_REGION,
                           ( location, timestamp, regionHandle, metricValues ) )

    SCOREP_Task_Exit( location );
}


void
SCOREP_ExitRegion( SCOREP_RegionHandle regionHandle )
{
    SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp     = scorep_get_timestamp( location );
    uint64_t*        metric_values = SCOREP_Metric_Read( location );

    exit_region( location, timestamp, regionHandle, metric_values );
}


void
SCOREP_Location_ExitRegion( SCOREP_Location*    location,
                            uint64_t            timestamp,
                            SCOREP_RegionHandle regionHandle )
{
    UTILS_BUG_ON( location && SCOREP_Location_GetType( location ) == SCOREP_LOCATION_TYPE_CPU_THREAD,
                  "SCOREP_Location_ExitRegion() must not be used for CPU thread locations." );

    if ( !location )
    {
        location = SCOREP_Location_GetCurrentCPULocation();
    }

    SCOREP_Location_SetLastTimestamp( location, timestamp );

    uint64_t* metric_values = SCOREP_Metric_Read( location );

    exit_region( location, timestamp, regionHandle, metric_values );
}


/**
 * Generate a rewind region enter event in the measurement system.
 */
void
SCOREP_EnterRewindRegion( SCOREP_RegionHandle regionHandle )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "RwR:%u",
                        SCOREP_Definitions_HandleToId( regionHandle ) );

    SCOREP_CALL_SUBSTRATE( EnterRewindRegion, ENTER_REWIND_REGION,
                           ( location, timestamp, regionHandle ) )
}


/**
 * Generate a rewind region exit event in the measurement system.
 */
void
SCOREP_ExitRewindRegion( SCOREP_RegionHandle regionHandle, bool do_rewind )
{
    SCOREP_Location* location       = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         leavetimestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "RwR:%u",
                        SCOREP_Definitions_HandleToId( regionHandle ) );

    SCOREP_CALL_SUBSTRATE( ExitRewindRegion, EXIT_REWIND_REGION,
                           ( location, leavetimestamp, regionHandle, do_rewind ) )
}


/**
 * Add an attribute to the current attribute list.
 */
void
SCOREP_AddAttribute( SCOREP_AttributeHandle attributeHandle,
                     void*                  value )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    SCOREP_CALL_SUBSTRATE( AddAttribute, ADD_ATTRIBUTE,
                           ( location, attributeHandle, value ) )
}


/**
 * Add an attribute to the attribute list of \p location.
 */
void
SCOREP_Location_AddAttribute( SCOREP_Location*       location,
                              SCOREP_AttributeHandle attributeHandle,
                              void*                  value )
{
    SCOREP_CALL_SUBSTRATE( AddAttribute, ADD_ATTRIBUTE,
                           ( location, attributeHandle, value ) )
}


static inline void
add_location_property( SCOREP_Location* location,
                       const char*      name,
                       const char*      value )
{
    SCOREP_LocationHandle handle = SCOREP_Location_GetLocationHandle( location );
    SCOREP_Definitions_NewLocationProperty( handle, name, value );
}


/**
 * Adds a meta data entry
 */
void
SCOREP_AddLocationProperty( const char* name,
                            const char* value )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    add_location_property( location, name, value );
}


/**
 * Add a location property to \p location.
 */
void
SCOREP_Location_AddLocationProperty( SCOREP_Location* location,
                                     const char*      name,
                                     const char*      value )
{
    add_location_property( location, name, value );
}


/**
 * Process an mpi send event in the measurement system.
 */
void
SCOREP_MpiSend( SCOREP_MpiRank                   destinationRank,
                SCOREP_InterimCommunicatorHandle communicatorHandle,
                uint32_t                         tag,
                uint64_t                         bytesSent )
{
    UTILS_BUG_ON( destinationRank < 0, "Invalid rank passed to SCOREP_MpiSend" );


    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Dst:%d Comm:%u Tag:%u Bytes:%llu",
                        destinationRank,
                        SCOREP_Definitions_HandleToId( communicatorHandle ),
                        tag,
                        ( unsigned long long )bytesSent );

    SCOREP_CALL_SUBSTRATE( MpiSend, MPI_SEND,
                           ( location, timestamp, destinationRank,
                             communicatorHandle, tag, bytesSent ) )
}


/**
 * Process an mpi recv event in the measurement system.
 */
void
SCOREP_MpiRecv( SCOREP_MpiRank                   sourceRank,
                SCOREP_InterimCommunicatorHandle communicatorHandle,
                uint32_t                         tag,
                uint64_t                         bytesReceived )
{
    UTILS_BUG_ON( sourceRank < 0, "Invalid rank passed to SCOREP_MpiRecv" );


    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Src:%u Comm:%u Tag:%u Bytes:%llu",
                        sourceRank,
                        SCOREP_Definitions_HandleToId( communicatorHandle ),
                        tag,
                        ( unsigned long long )bytesReceived );

    SCOREP_CALL_SUBSTRATE( MpiRecv, MPI_RECV,
                           ( location, timestamp, sourceRank,
                             communicatorHandle, tag, bytesReceived ) )
}

/**
 * Process an mpi collective begin event in the measurement system.
 */
uint64_t
SCOREP_MpiCollectiveBegin( SCOREP_RegionHandle regionHandle )
{
    SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp     = scorep_get_timestamp( location );
    uint64_t*        metric_values = SCOREP_Metric_Read( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    enter_region( location, timestamp, regionHandle, metric_values );

    SCOREP_CALL_SUBSTRATE( MpiCollectiveBegin, MPI_COLLECTIVE_BEGIN,
                           ( location, timestamp ) )

    return timestamp;
}

/**
 * Process an mpi collective end event in the measurement system.
 */
void
SCOREP_MpiCollectiveEnd( SCOREP_RegionHandle              regionHandle,
                         SCOREP_InterimCommunicatorHandle communicatorHandle,
                         SCOREP_MpiRank                   rootRank,
                         SCOREP_MpiCollectiveType         collectiveType,
                         uint64_t                         bytesSent,
                         uint64_t                         bytesReceived )
{
    UTILS_BUG_ON( ( rootRank < 0 && rootRank != SCOREP_INVALID_ROOT_RANK ),
                  "Invalid rank passed to SCOREP_MpiCollectiveEnd\n" );

    SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp     = scorep_get_timestamp( location );
    uint64_t*        metric_values = SCOREP_Metric_Read( location );


    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_CALL_SUBSTRATE( MpiCollectiveEnd, MPI_COLLECTIVE_END,
                           ( location, timestamp, communicatorHandle, rootRank,
                             collectiveType, bytesSent, bytesReceived ) )

    exit_region( location, timestamp, regionHandle, metric_values );
}

void
SCOREP_MpiIsendComplete( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_CALL_SUBSTRATE( MpiIsendComplete, MPI_ISEND_COMPLETE,
                           ( location, timestamp, requestId ) )
}

void
SCOREP_MpiIrecvRequest( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_CALL_SUBSTRATE( MpiIrecvRequest, MPI_IRECV_REQUEST,
                           ( location, timestamp, requestId ) )
}

void
SCOREP_MpiRequestTested( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_CALL_SUBSTRATE( MpiRequestTested, MPI_REQUEST_TESTED,
                           ( location, timestamp, requestId ) )
}

void
SCOREP_MpiRequestCancelled( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_CALL_SUBSTRATE( MpiRequestCancelled, MPI_REQUEST_CANCELLED,
                           ( location, timestamp, requestId ) )
}

void
SCOREP_MpiIsend(  SCOREP_MpiRank                   destinationRank,
                  SCOREP_InterimCommunicatorHandle communicatorHandle,
                  uint32_t                         tag,
                  uint64_t                         bytesSent,
                  SCOREP_MpiRequestId              requestId )
{
    UTILS_BUG_ON( destinationRank < 0, "Invalid rank passed to SCOREP_MpiIsend\n" );


    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_CALL_SUBSTRATE( MpiIsend, MPI_ISEND,
                           ( location, timestamp, destinationRank,
                             communicatorHandle, tag, bytesSent, requestId ) )
}

void
SCOREP_MpiIrecv( SCOREP_MpiRank                   sourceRank,
                 SCOREP_InterimCommunicatorHandle communicatorHandle,
                 uint32_t                         tag,
                 uint64_t                         bytesReceived,
                 SCOREP_MpiRequestId              requestId )
{
    UTILS_BUG_ON( sourceRank < 0,  "Invalid rank passed to SCOREP_MpiIrecv\n" );


    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_CALL_SUBSTRATE( MpiIrecv, MPI_IRECV,
                           ( location, timestamp, sourceRank,
                             communicatorHandle, tag, bytesReceived, requestId ) )
}


void
SCOREP_RmaWinCreate( SCOREP_InterimRmaWindowHandle windowHandle )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaWinCreate, RMA_WIN_CREATE,
                           ( location, timestamp, windowHandle ) )
}


void
SCOREP_Location_RmaWinCreate( SCOREP_Location*              location,
                              uint64_t                      timestamp,
                              SCOREP_InterimRmaWindowHandle windowHandle )
{
    SCOREP_CALL_SUBSTRATE( RmaWinCreate, RMA_WIN_CREATE,
                           ( location, timestamp, windowHandle ) )
}


void
SCOREP_RmaWinDestroy( SCOREP_InterimRmaWindowHandle windowHandle )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaWinDestroy, RMA_WIN_DESTROY,
                           ( location, timestamp, windowHandle ) )
}


void
SCOREP_Location_RmaWinDestroy( SCOREP_Location*              location,
                               uint64_t                      timestamp,
                               SCOREP_InterimRmaWindowHandle windowHandle )
{
    SCOREP_CALL_SUBSTRATE( RmaWinDestroy, RMA_WIN_DESTROY,
                           ( location, timestamp, windowHandle ) )
}


void
SCOREP_RmaCollectiveBegin( void )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaCollectiveBegin, RMA_COLLECTIVE_BEGIN,
                           ( location, timestamp ) )
}


void
SCOREP_RmaCollectiveEnd( SCOREP_MpiCollectiveType      collectiveOp,
                         SCOREP_RmaSyncLevel           syncLevel,
                         SCOREP_InterimRmaWindowHandle windowHandle,
                         uint32_t                      root,
                         uint64_t                      bytesSent,
                         uint64_t                      bytesReceived )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaCollectiveEnd, RMA_COLLECTIVE_END,
                           ( location, timestamp, collectiveOp, syncLevel,
                             windowHandle, root, bytesSent, bytesReceived ) )
}


void
SCOREP_RmaGroupSync( SCOREP_RmaSyncLevel           syncLevel,
                     SCOREP_InterimRmaWindowHandle windowHandle,
                     SCOREP_GroupHandle            groupHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaGroupSync, RMA_GROUP_SYNC,
                           ( location, timestamp, syncLevel,
                             windowHandle, groupHandle ) )
}


void
SCOREP_RmaRequestLock( SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      lockId,
                       SCOREP_LockType               lockType )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaRequestLock, RMA_REQUEST_LOCK,
                           ( location, timestamp, windowHandle,
                             remote, lockId, lockType ) )
}


void
SCOREP_RmaAcquireLock( SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      lockId,
                       SCOREP_LockType               lockType )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaAcquireLock, RMA_ACQUIRE_LOCK,
                           ( location, timestamp, windowHandle,
                             remote, lockId, lockType ) )
}


void
SCOREP_RmaTryLock( SCOREP_InterimRmaWindowHandle windowHandle,
                   uint32_t                      remote,
                   uint64_t                      lockId,
                   SCOREP_LockType               lockType )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaTryLock, RMA_TRY_LOCK,
                           ( location, timestamp, windowHandle,
                             remote, lockId, lockType ) )
}


void
SCOREP_RmaReleaseLock( SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      lockId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaReleaseLock, RMA_RELEASE_LOCK,
                           ( location, timestamp, windowHandle, remote, lockId ) )
}


void
SCOREP_RmaSync( SCOREP_InterimRmaWindowHandle windowHandle,
                uint32_t                      remote,
                SCOREP_RmaSyncType            syncType )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaSync, RMA_SYNC,
                           ( location, timestamp, windowHandle, remote, syncType ) )
}


void
SCOREP_RmaWaitChange( SCOREP_InterimRmaWindowHandle windowHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "RMA window:%x", windowHandle );

    SCOREP_CALL_SUBSTRATE( RmaWaitChange, RMA_WAIT_CHANGE,
                           ( location, timestamp, windowHandle ) )
}


void
SCOREP_RmaPut( SCOREP_InterimRmaWindowHandle windowHandle,
               uint32_t                      remote,
               uint64_t                      bytes,
               uint64_t                      matchingId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaPut, RMA_PUT,
                           ( location, timestamp, windowHandle,
                             remote, bytes, matchingId ) )
}


void
SCOREP_Location_RmaPut( SCOREP_Location*              location,
                        uint64_t                      timestamp,
                        SCOREP_InterimRmaWindowHandle windowHandle,
                        uint32_t                      remote,
                        uint64_t                      bytes,
                        uint64_t                      matchingId )
{
    SCOREP_CALL_SUBSTRATE( RmaPut, RMA_PUT,
                           ( location, timestamp, windowHandle,
                             remote, bytes, matchingId ) )
}


void
SCOREP_RmaGet( SCOREP_InterimRmaWindowHandle windowHandle,
               uint32_t                      remote,
               uint64_t                      bytes,
               uint64_t                      matchingId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaGet, RMA_GET,
                           ( location, timestamp, windowHandle,
                             remote, bytes, matchingId ) )
}



void
SCOREP_Location_RmaGet( SCOREP_Location*              location,
                        uint64_t                      timestamp,
                        SCOREP_InterimRmaWindowHandle windowHandle,
                        uint32_t                      remote,
                        uint64_t                      bytes,
                        uint64_t                      matchingId )
{
    SCOREP_CALL_SUBSTRATE( RmaGet, RMA_GET,
                           ( location, timestamp, windowHandle,
                             remote, bytes, matchingId ) )
}


void
SCOREP_RmaAtomic( SCOREP_InterimRmaWindowHandle windowHandle,
                  uint32_t                      remote,
                  SCOREP_RmaAtomicType          type,
                  uint64_t                      bytesSent,
                  uint64_t                      bytesReceived,
                  uint64_t                      matchingId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaAtomic, RMA_ATOMIC,
                           ( location, timestamp, windowHandle, remote,
                             type, bytesSent, bytesReceived, matchingId ) )
}


void
SCOREP_RmaOpCompleteBlocking( SCOREP_InterimRmaWindowHandle windowHandle,
                              uint64_t                      matchingId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaOpCompleteBlocking, RMA_OP_COMPLETE_BLOCKING,
                           ( location, timestamp, windowHandle, matchingId ) )
}


void
SCOREP_Location_RmaOpCompleteBlocking( SCOREP_Location*              location,
                                       uint64_t                      timestamp,
                                       SCOREP_InterimRmaWindowHandle windowHandle,
                                       uint64_t                      matchingId )
{
    SCOREP_CALL_SUBSTRATE( RmaOpCompleteBlocking, RMA_OP_COMPLETE_BLOCKING,
                           ( location, timestamp, windowHandle, matchingId ) )
}


void
SCOREP_RmaOpCompleteNonBlocking( SCOREP_InterimRmaWindowHandle windowHandle,
                                 uint64_t                      matchingId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaOpCompleteNonBlocking, RMA_OP_COMPLETE_NON_BLOCKING,
                           ( location, timestamp, windowHandle, matchingId ) )
}


void
SCOREP_RmaOpTest( SCOREP_InterimRmaWindowHandle windowHandle,
                  uint64_t                      matchingId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaOpTest, RMA_OP_TEST,
                           ( location, timestamp, windowHandle, matchingId ) )
}


void
SCOREP_RmaOpCompleteRemote( SCOREP_InterimRmaWindowHandle windowHandle,
                            uint64_t                      matchingId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    SCOREP_CALL_SUBSTRATE( RmaOpCompleteRemote, RMA_OP_COMPLETE_REMOTE,
                           ( location, timestamp, windowHandle, matchingId ) )
}


void
SCOREP_ThreadAcquireLock( SCOREP_ParadigmType paradigm,
                          uint32_t            lockId,
                          uint32_t            acquisitionOrder )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Lock:%x", lockId );

    SCOREP_CALL_SUBSTRATE( ThreadAcquireLock, THREAD_ACQUIRE_LOCK,
                           ( location, timestamp, paradigm,
                             lockId, acquisitionOrder ) )
}


void
SCOREP_ThreadReleaseLock( SCOREP_ParadigmType paradigm,
                          uint32_t            lockId,
                          uint32_t            acquisitionOrder )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Lock:%x", lockId );

    SCOREP_CALL_SUBSTRATE( ThreadReleaseLock, THREAD_RELEASE_LOCK,
                           ( location, timestamp, paradigm,
                             lockId, acquisitionOrder ) )
}


/**
 *
 */
void
SCOREP_TriggerCounterInt64( SCOREP_SamplingSetHandle counterHandle,
                            int64_t                  value )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( counterHandle, SamplingSet );
    UTILS_BUG_ON( sampling_set->number_of_metrics != 1,
                  "User sampling set with more than one metric" );

    SCOREP_CALL_SUBSTRATE( TriggerCounterInt64, TRIGGER_COUNTER_INT64,
                           ( location, timestamp,
                             sampling_set->metric_handles[ 0 ],
                             counterHandle, value ) )
}


static inline void
trigger_counter_uint64( SCOREP_Location*         location,
                        uint64_t                 timestamp,
                        SCOREP_SamplingSetHandle counterHandle,
                        uint64_t                 value )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( counterHandle, SamplingSet );
    UTILS_BUG_ON( sampling_set->number_of_metrics != 1,
                  "User sampling set with more than one metric" );

    SCOREP_CALL_SUBSTRATE( TriggerCounterUint64, TRIGGER_COUNTER_UINT64,
                           ( location, timestamp,
                             sampling_set->metric_handles[ 0 ],
                             counterHandle, value ) )
}


/**
 *
 */
void
SCOREP_TriggerCounterUint64( SCOREP_SamplingSetHandle counterHandle,
                             uint64_t                 value )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    trigger_counter_uint64( location, timestamp, counterHandle, value );
}


/**
 *
 */
void
SCOREP_Location_TriggerCounterUint64( SCOREP_Location*         location,
                                      uint64_t                 timestamp,
                                      SCOREP_SamplingSetHandle counterHandle,
                                      uint64_t                 value )
{
    trigger_counter_uint64( location, timestamp, counterHandle, value );
}


/**
 *
 */
void
SCOREP_TriggerCounterDouble( SCOREP_SamplingSetHandle counterHandle,
                             double                   value )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( counterHandle, SamplingSet );
    UTILS_BUG_ON( sampling_set->number_of_metrics != 1,
                  "User sampling set with more than one metric" );

    SCOREP_CALL_SUBSTRATE( TriggerCounterDouble, TRIGGER_COUNTER_DOUBLE,
                           ( location, timestamp,
                             sampling_set->metric_handles[ 0 ],
                             counterHandle, value ) )
}


/**
 *
 */
void
SCOREP_TriggerMarker( SCOREP_MarkerHandle markerHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    UTILS_NOT_YET_IMPLEMENTED();
}


/**
 *
 */
void
SCOREP_TriggerParameterInt64( SCOREP_ParameterHandle parameterHandle,
                              int64_t                value )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_CALL_SUBSTRATE( TriggerParameterInt64, TRIGGER_PARAMETER_INT64,
                           ( location, timestamp, parameterHandle, value ) )
}


/**
 *
 */
void
SCOREP_TriggerParameterUint64( SCOREP_ParameterHandle parameterHandle,
                               uint64_t               value )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_CALL_SUBSTRATE( TriggerParameterUint64, TRIGGER_PARAMETER_UINT64,
                           ( location, timestamp, parameterHandle, value ) )
}


/**
 *
 */
void
SCOREP_TriggerParameterString( SCOREP_ParameterHandle parameterHandle,
                               const char*            value )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_StringHandle string_handle = SCOREP_Definitions_NewString( value );

    SCOREP_CALL_SUBSTRATE( TriggerParameterString, TRIGGER_PARAMETER_STRING,
                           ( location, timestamp,
                             parameterHandle, string_handle ) )
}

/**
 * Returns the timestamp of the last triggered event on the current location.
 *
 */
uint64_t
SCOREP_GetLastTimeStamp( void )
{
    return SCOREP_Location_GetLastTimestamp( SCOREP_Location_GetCurrentCPULocation() );
}
