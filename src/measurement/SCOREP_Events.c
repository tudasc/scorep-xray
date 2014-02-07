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
 * @brief   Declaration of event recording functions to be used by the
 *          adapter layer.
 *
 *
 */

#include <config.h>

#include <SCOREP_Events.h>

#include <stdlib.h>
#include <stdio.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include <SCOREP_Definitions.h>
#include <definitions/SCOREP_Definitions.h>
#include <SCOREP_Properties.h>
#include <tracing/SCOREP_Tracing_Events.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Profile_Tasking.h>
#include <SCOREP_Profile_MpiEvents.h>
#include <SCOREP_Metric_Management.h>

#include "scorep_events_common.h"
#include "scorep_runtime_management.h"
#include "scorep_types.h"


/**
 * Process a region enter event in the measurement system.
 */
static void
scorep_enter_region( uint64_t            timestamp,
                     SCOREP_RegionHandle regionHandle,
                     uint64_t*           metricValues,
                     SCOREP_Location*    location )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%u",
                        scorep_handle_to_id( regionHandle ) );

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_Enter( location, regionHandle,
                              SCOREP_RegionHandle_GetType( regionHandle ),
                              timestamp, metricValues );

        SCOREP_Metric_WriteToProfile( location );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Metric_WriteToTrace( location,
                                    timestamp );

        SCOREP_Tracing_Enter( location,
                              timestamp,
                              regionHandle );
    }
}


void
SCOREP_EnterRegion( SCOREP_RegionHandle regionHandle )
{
    SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp     = scorep_get_timestamp( location );
    uint64_t*        metric_values = SCOREP_Metric_Read( location );

    scorep_enter_region( timestamp, regionHandle, metric_values, location );
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
    UTILS_BUG_ON( !location && SCOREP_Location_GetType( location ) == SCOREP_LOCATION_TYPE_CPU_THREAD,
                  "SCOREP_Location_EnterRegion() must not be used for CPU thread locations." );

    if ( !location )
    {
        location = SCOREP_Location_GetCurrentCPULocation();
    }

    SCOREP_Location_SetLastTimestamp( location, timestamp );

    uint64_t* metric_values = SCOREP_Metric_Read( location );

    scorep_enter_region( timestamp, regionHandle, metric_values, location );
}


/**
 * Process a region exit event in the measurement system.
 */
static void
scorep_exit_region( uint64_t            timestamp,
                    SCOREP_RegionHandle regionHandle,
                    uint64_t*           metricValues,
                    SCOREP_Location*    location )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%u",
                        scorep_handle_to_id( regionHandle ) );

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Metric_WriteToProfile( location );

        SCOREP_Profile_Exit( location,
                             regionHandle,
                             timestamp,
                             metricValues );
    }

    if ( scorep_tracing_consume_event() )
    {
        if ( metricValues )
        {
            /* @todo: Writing metrics to trace file will be improved in the near future */

            SCOREP_Metric_WriteToTrace( location,
                                        timestamp );
        }

        SCOREP_Tracing_Leave( location,
                              timestamp,
                              regionHandle );
    }
}


void
SCOREP_ExitRegion( SCOREP_RegionHandle regionHandle )
{
    SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp     = scorep_get_timestamp( location );
    uint64_t*        metric_values = SCOREP_Metric_Read( location );

    scorep_exit_region( timestamp, regionHandle, metric_values, location );
}


void
SCOREP_Location_ExitRegion( SCOREP_Location*    location,
                            uint64_t            timestamp,
                            SCOREP_RegionHandle regionHandle )
{
    UTILS_BUG_ON( !location && SCOREP_Location_GetType( location ) == SCOREP_LOCATION_TYPE_CPU_THREAD,
                  "SCOREP_Location_ExitRegion() must not be used for CPU thread locations." );

    if ( !location )
    {
        location = SCOREP_Location_GetCurrentCPULocation();
    }

    SCOREP_Location_SetLastTimestamp( location, timestamp );

    uint64_t* metric_values = SCOREP_Metric_Read( location );

    scorep_exit_region( timestamp, regionHandle, metric_values, location );
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
                        scorep_handle_to_id( regionHandle ) );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_StoreRewindPoint( location, regionHandle, timestamp );
    }
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
                        scorep_handle_to_id( regionHandle ) );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ExitRewindRegion( location, regionHandle, leavetimestamp, do_rewind );
    }
}

/**
 * Add an attribute to the current attribute list.
 */
void
SCOREP_AddAttribute( SCOREP_AttributeHandle attrHandle,
                     void*                  value )
{
    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
        SCOREP_Tracing_AddAttribute( location, attrHandle, value );
    }
}


/**
 * Add an attribute to the attribute list of \p location.
 */
void
SCOREP_Location_AddAttribute( SCOREP_Location*       location,
                              SCOREP_AttributeHandle attrHandle,
                              void*                  value )
{
    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_AddAttribute( location, attrHandle, value );
    }
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
                        scorep_handle_to_id( communicatorHandle ),
                        tag,
                        ( unsigned long long )bytesSent );

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_MpiSend( location,
                                destinationRank,
                                communicatorHandle,
                                tag,
                                bytesSent );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiSend( location,
                                timestamp,
                                destinationRank,
                                communicatorHandle,
                                tag,
                                bytesSent );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
    }
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
                        scorep_handle_to_id( communicatorHandle ),
                        tag,
                        ( unsigned long long )bytesReceived );

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_MpiRecv( location,
                                sourceRank,
                                communicatorHandle,
                                tag,
                                bytesReceived );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiRecv( location,
                                timestamp,
                                sourceRank,
                                communicatorHandle,
                                tag,
                                bytesReceived );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
    }
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

    scorep_enter_region( timestamp, regionHandle, metric_values, location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiCollectiveBegin( location,
                                           timestamp );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
    }

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


    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_CollectiveEnd( location,
                                      communicatorHandle,
                                      rootRank,
                                      collectiveType,
                                      bytesSent,
                                      bytesReceived );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiCollectiveEnd( location,
                                         timestamp,
                                         communicatorHandle,
                                         rootRank,
                                         collectiveType,
                                         bytesSent,
                                         bytesReceived );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
    }

    scorep_exit_region( timestamp, regionHandle, metric_values, location );
}

void
SCOREP_MpiIsendComplete( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiIsendComplete( location,
                                         timestamp,
                                         requestId );
    }
}

void
SCOREP_MpiIrecvRequest( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiIrecvRequest( location,
                                        timestamp,
                                        requestId );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
    }
}

void
SCOREP_MpiRequestTested( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiRequestTested( location,
                                         timestamp,
                                         requestId );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
    }
}

void
SCOREP_MpiRequestCancelled( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiRequestCancelled( location,
                                            timestamp,
                                            requestId );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
    }
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

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_MpiSend( location,
                                destinationRank,
                                communicatorHandle,
                                tag,
                                bytesSent );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiIsend( location,
                                 timestamp,
                                 destinationRank,
                                 communicatorHandle,
                                 tag,
                                 bytesSent,
                                 requestId );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
    }
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

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_MpiRecv( location,
                                sourceRank,
                                communicatorHandle,
                                tag,
                                bytesReceived );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiIrecv( location,
                                 timestamp,
                                 sourceRank,
                                 communicatorHandle,
                                 tag,
                                 bytesReceived,
                                 requestId );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
    }
}


void
SCOREP_RmaWinCreate( SCOREP_InterimRmaWindowHandle windowHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaWinCreate( location,
                                     timestamp,
                                     windowHandle );
    }
}


void
SCOREP_Location_RmaWinCreate( SCOREP_Location*              location,
                              uint64_t                      timestamp,
                              SCOREP_InterimRmaWindowHandle windowHandle )
{
    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaWinCreate( location,
                                     timestamp,
                                     windowHandle );
    }
}


void
SCOREP_RmaWinDestroy( SCOREP_InterimRmaWindowHandle windowHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaWinDestroy( location,
                                      timestamp,
                                      windowHandle );
    }
}


void
SCOREP_Location_RmaWinDestroy( SCOREP_Location*              location,
                               uint64_t                      timestamp,
                               SCOREP_InterimRmaWindowHandle windowHandle )
{
    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaWinDestroy( location,
                                      timestamp,
                                      windowHandle );
    }
}


void
SCOREP_RmaCollectiveBegin()
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaCollectiveBegin( location,
                                           timestamp );
    }
}


void
SCOREP_RmaCollectiveEnd( SCOREP_MpiCollectiveType      collectiveOp,
                         SCOREP_RmaSyncLevel           syncLevel,
                         SCOREP_InterimRmaWindowHandle windowHandle,
                         uint32_t                      root,
                         uint64_t                      bytesSent,
                         uint64_t                      bytesReceived )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaCollectiveEnd( location,
                                         timestamp,
                                         collectiveOp,
                                         syncLevel,
                                         windowHandle,
                                         root,
                                         bytesSent,
                                         bytesReceived );
    }
}


void
SCOREP_RmaGroupSync( SCOREP_RmaSyncLevel           syncLevel,
                     SCOREP_InterimRmaWindowHandle windowHandle,
                     SCOREP_GroupHandle            groupHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaGroupSync( location,
                                     timestamp,
                                     syncLevel,
                                     windowHandle,
                                     groupHandle );
    }
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

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaRequestLock( location,
                                       timestamp,
                                       windowHandle,
                                       remote,
                                       lockId,
                                       lockType );
    }
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

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaAcquireLock( location,
                                       timestamp,
                                       windowHandle,
                                       remote,
                                       lockId,
                                       lockType );
    }
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

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaTryLock( location,
                                   timestamp,
                                   windowHandle,
                                   remote,
                                   lockId,
                                   lockType );
    }
}


void
SCOREP_RmaReleaseLock( SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      lockId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaReleaseLock( location,
                                       timestamp,
                                       windowHandle,
                                       remote,
                                       lockId );
    }
}


void
SCOREP_RmaSync( SCOREP_InterimRmaWindowHandle windowHandle,
                uint32_t                      remote,
                SCOREP_RmaSyncType            syncType )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaSync( location,
                                timestamp,
                                windowHandle,
                                remote,
                                syncType );
    }
}


void
SCOREP_RmaWaitChange( SCOREP_InterimRmaWindowHandle windowHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "RMA window:%x", windowHandle );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaWaitChange( location,
                                      timestamp,
                                      windowHandle );
    }
}


void
SCOREP_RmaPut( SCOREP_InterimRmaWindowHandle windowHandle,
               uint32_t                      remote,
               uint64_t                      bytes,
               uint64_t                      matchingId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaPut( location,
                               timestamp,
                               windowHandle,
                               remote,
                               bytes,
                               matchingId );
    }
}


void
SCOREP_Location_RmaPut( SCOREP_Location*              location,
                        uint64_t                      timestamp,
                        SCOREP_InterimRmaWindowHandle windowHandle,
                        uint32_t                      remote,
                        uint64_t                      bytes,
                        uint64_t                      matchingId )
{
    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaPut( location,
                               timestamp,
                               windowHandle,
                               remote,
                               bytes,
                               matchingId );
    }
}


void
SCOREP_RmaGet( SCOREP_InterimRmaWindowHandle windowHandle,
               uint32_t                      remote,
               uint64_t                      bytes,
               uint64_t                      matchingId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaGet( location,
                               timestamp,
                               windowHandle,
                               remote,
                               bytes,
                               matchingId );
    }
}



void
SCOREP_Location_RmaGet( SCOREP_Location*              location,
                        uint64_t                      timestamp,
                        SCOREP_InterimRmaWindowHandle windowHandle,
                        uint32_t                      remote,
                        uint64_t                      bytes,
                        uint64_t                      matchingId )
{
    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaGet( location,
                               timestamp,
                               windowHandle,
                               remote,
                               bytes,
                               matchingId );
    }
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

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaAtomic( location,
                                  timestamp,
                                  windowHandle,
                                  remote,
                                  type,
                                  bytesSent,
                                  bytesReceived,
                                  matchingId );
    }
}


void
SCOREP_RmaOpCompleteBlocking( SCOREP_InterimRmaWindowHandle windowHandle,
                              uint64_t                      matchingId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaOpCompleteBlocking( location,
                                              timestamp,
                                              windowHandle,
                                              matchingId );
    }
}


void
SCOREP_Location_RmaOpCompleteBlocking( SCOREP_Location*              location,
                                       uint64_t                      timestamp,
                                       SCOREP_InterimRmaWindowHandle windowHandle,
                                       uint64_t                      matchingId )
{
    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaOpCompleteBlocking( location,
                                              timestamp,
                                              windowHandle,
                                              matchingId );
    }
}


void
SCOREP_RmaOpCompleteNonBlocking( SCOREP_InterimRmaWindowHandle windowHandle,
                                 uint64_t                      matchingId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaOpCompleteNonBlocking( location,
                                                 timestamp,
                                                 windowHandle,
                                                 matchingId );
    }
}


void
SCOREP_RmaOpTest( SCOREP_InterimRmaWindowHandle windowHandle,
                  uint64_t                      matchingId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaOpTest( location,
                                  timestamp,
                                  windowHandle,
                                  matchingId );
    }
}


void
SCOREP_RmaOpCompleteRemote( SCOREP_InterimRmaWindowHandle windowHandle,
                            uint64_t                      matchingId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaOpCompleteRemote( location,
                                            timestamp,
                                            windowHandle,
                                            matchingId );
    }
}


void
SCOREP_ThreadAcquireLock( SCOREP_ParadigmType paradigm,
                          uint32_t            lockId,
                          uint32_t            acquisitionOrder )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Lock:%x", lockId );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadAcquireLock( location,
                                          timestamp,
                                          paradigm,
                                          lockId,
                                          acquisitionOrder );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_LOCK_EVENT_COMPLETE );
    }
}


void
SCOREP_ThreadReleaseLock( SCOREP_ParadigmType paradigm,
                          uint32_t            lockId,
                          uint32_t            acquisitionOrder )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Lock:%x", lockId );

    /* Nothing to do for profiling. */

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadReleaseLock( location,
                                          timestamp,
                                          paradigm,
                                          lockId,
                                          acquisitionOrder );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_LOCK_EVENT_COMPLETE );
    }
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

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_TriggerInteger( location,
                                       sampling_set->metric_handles[ 0 ],
                                       value );
    }

    if ( scorep_tracing_consume_event() )
    {
        union
        {
            uint64_t uint64;
            int64_t  int64;
        } union_value;
        union_value.int64 = value;

        SCOREP_Tracing_Metric( location,
                               timestamp,
                               counterHandle,
                               &union_value.uint64 );
    }
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

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( counterHandle, SamplingSet );
    UTILS_BUG_ON( sampling_set->number_of_metrics != 1,
                  "User sampling set with more than one metric" );

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_TriggerInteger( location,
                                       sampling_set->metric_handles[ 0 ],
                                       value );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_Metric( location,
                               timestamp,
                               counterHandle,
                               &value );
    }
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
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( counterHandle, SamplingSet );
    UTILS_BUG_ON( sampling_set->number_of_metrics != 1,
                  "User sampling set with more than one metric" );

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_TriggerInteger( location,
                                       sampling_set->metric_handles[ 0 ],
                                       value );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_Metric( location,
                               timestamp,
                               counterHandle,
                               &value );
    }
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

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_TriggerDouble( location,
                                      sampling_set->metric_handles[ 0 ],
                                      value );
    }

    if ( scorep_tracing_consume_event() )
    {
        union
        {
            uint64_t uint64;
            double   float64;
        } union_value;
        union_value.float64 = value;

        SCOREP_Tracing_Metric( location,
                               timestamp,
                               counterHandle,
                               &union_value.uint64 );
    }
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

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_ParameterInteger( location,
                                         parameterHandle,
                                         value );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ParameterInt64( location,
                                       timestamp,
                                       parameterHandle,
                                       value );
    }
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

    if ( scorep_profiling_consume_event() )
    {
        /* The SCOREP_Profile_ParameterInteger handles unsigned and signed integers */
        SCOREP_Profile_ParameterInteger( location,
                                         parameterHandle,
                                         value );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ParameterUint64( location,
                                        timestamp,
                                        parameterHandle,
                                        value );
    }
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

    if ( scorep_profiling_consume_event() )
    {
        SCOREP_Profile_ParameterString( location,
                                        parameterHandle,
                                        string_handle );
    }

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ParameterString( location,
                                        timestamp,
                                        parameterHandle,
                                        string_handle );
    }
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

/**
 * Adds a meta data entry
 */
void
SCOREP_AddLocationProperty( const char* name,
                            const char* value )
{
    SCOREP_Location*      location = SCOREP_Location_GetCurrentCPULocation();
    SCOREP_LocationHandle handle   = SCOREP_Location_GetLocationHandle( location );
    SCOREP_Definitions_NewLocationProperty( handle,
                                            name,
                                            value );
}
