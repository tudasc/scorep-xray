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
 * @status      alpha
 * @file        SCOREP_Events.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
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
#include <scorep_openmp.h>

#include "scorep_events_common.h"
#include "scorep_runtime_management.h"
#include "scorep_types.h"
#include "scorep_thread.h"


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

    if ( scorep_tracing_consume_event() )
    {
        if ( metricValues )
        {
            /* @todo: Writing metrics to trace file will be improved in the near future */

            SCOREP_Metric_WriteToTrace( location,
                                        timestamp );
        }

        SCOREP_Tracing_Enter( location,
                              timestamp,
                              regionHandle );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_Enter( location, regionHandle,
                              SCOREP_Region_GetType( regionHandle ),
                              timestamp, metricValues );
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

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_Exit( location,
                             regionHandle,
                             timestamp,
                             metricValues );
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

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ExitRewindRegion( location, regionHandle, leavetimestamp, do_rewind );
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

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_MpiSend( location,
                                destinationRank,
                                communicatorHandle,
                                tag,
                                bytesSent );
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

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_MpiRecv( location,
                                sourceRank,
                                communicatorHandle,
                                tag,
                                bytesReceived );
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

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiCollectiveBegin( location,
                                           timestamp );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
    }

    /* Nothing to do for profiling. */

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

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_CollectiveEnd( location,
                                      communicatorHandle,
                                      rootRank,
                                      collectiveType,
                                      bytesSent,
                                      bytesReceived );
    }

    scorep_exit_region( timestamp, regionHandle, metric_values, location );
}

void
SCOREP_MpiIsendComplete( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_MpiIsendComplete( location,
                                         timestamp,
                                         requestId );
    }

    /* Nothing to do for profiling. */
}

void
SCOREP_MpiIrecvRequest( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

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

    /* Nothing to do for profiling. */
}

void
SCOREP_MpiRequestTested( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

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

    /* Nothing to do for profiling. */
}

void
SCOREP_MpiRequestCancelled( SCOREP_MpiRequestId requestId )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "" );

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

    /* Nothing to do for profiling. */
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

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_MpiSend( location,
                                destinationRank,
                                communicatorHandle,
                                tag,
                                bytesSent );
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

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_MpiRecv( location,
                                sourceRank,
                                communicatorHandle,
                                tag,
                                bytesReceived );
    }
}


void
SCOREP_RmaWinCreate( SCOREP_InterimRmaWindowHandle windowHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaWinCreate( location,
                                     timestamp,
                                     windowHandle );
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_RmaWinDestroy( SCOREP_InterimRmaWindowHandle windowHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaWinDestroy( location,
                                      timestamp,
                                      windowHandle );
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_RmaCollectiveBegin()
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaCollectiveBegin( location,
                                           timestamp );
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_RmaCollectiveEnd( SCOREP_RmaSyncLevel           syncLevel,
                         SCOREP_InterimRmaWindowHandle windowHandle,
                         SCOREP_MpiCollectiveType      collectiveOp,
                         uint32_t                      root,
                         uint64_t                      bytesSent,
                         uint64_t                      bytesReceived )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaCollectiveEnd( location,
                                         timestamp,
                                         syncLevel,
                                         windowHandle,
                                         collectiveOp,
                                         root,
                                         bytesSent,
                                         bytesReceived );
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_RmaGroupSync( SCOREP_RmaSyncLevel           syncLevel,
                     SCOREP_InterimRmaWindowHandle windowHandle,
                     SCOREP_GroupHandle            groupHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaGroupSync( location,
                                     timestamp,
                                     syncLevel,
                                     windowHandle,
                                     groupHandle );
    }

    /* Nothing to do for profiling. */
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

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaRequestLock( location,
                                       timestamp,
                                       windowHandle,
                                       remote,
                                       lockId,
                                       lockType );
    }

    /* Nothing to do for profiling. */
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

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaAcquireLock( location,
                                       timestamp,
                                       windowHandle,
                                       remote,
                                       lockId,
                                       lockType );
    }

    /* Nothing to do for profiling. */
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

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaTryLock( location,
                                   timestamp,
                                   windowHandle,
                                   remote,
                                   lockId,
                                   lockType );
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_RmaReleaseLock( SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      lockId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaReleaseLock( location,
                                       timestamp,
                                       windowHandle,
                                       remote,
                                       lockId );
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_RmaSync( SCOREP_InterimRmaWindowHandle windowHandle,
                uint32_t                      remote,
                SCOREP_RmaSyncType            syncType )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaSync( location,
                                timestamp,
                                windowHandle,
                                remote,
                                syncType );
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_RmaWaitChange( SCOREP_InterimRmaWindowHandle windowHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "RMA window:%x", windowHandle );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaWaitChange( location,
                                      timestamp,
                                      windowHandle );
    }

    /* Nothing to do for profiling. */
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

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaPut( location,
                               timestamp,
                               windowHandle,
                               remote,
                               bytes,
                               matchingId );
    }

    /* Nothing to do for profiling. */
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

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaGet( location,
                               timestamp,
                               windowHandle,
                               remote,
                               bytes,
                               matchingId );
    }

    /* Nothing to do for profiling. */
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

    /* Nothing to do for profiling. */
}


void
SCOREP_RmaOpCompleteBlocking( SCOREP_InterimRmaWindowHandle windowHandle,
                              uint64_t                      matchingId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaOpCompleteBlocking( location,
                                              timestamp,
                                              windowHandle,
                                              matchingId );
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_RmaOpCompleteNonBlocking( SCOREP_InterimRmaWindowHandle windowHandle,
                                 uint64_t                      matchingId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaOpCompleteNonBlocking( location,
                                                 timestamp,
                                                 windowHandle,
                                                 matchingId );
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_RmaOpTest( SCOREP_InterimRmaWindowHandle windowHandle,
                  uint64_t                      matchingId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaOpTest( location,
                                  timestamp,
                                  windowHandle,
                                  matchingId );
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_RmaOpCompleteRemote( SCOREP_InterimRmaWindowHandle windowHandle,
                            uint64_t                      matchingId )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_RmaOpCompleteRemote( location,
                                            timestamp,
                                            windowHandle,
                                            matchingId );
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_ThreadAcquireLock( uint32_t           lockId,
                          uint32_t           acquisitionOrder,
                          SCOREP_ThreadModel model )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Lock:%x", lockId );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadAcquireLock( location,
                                          timestamp,
                                          lockId,
                                          acquisitionOrder,
                                          model );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_ThreadReleaseLock( uint32_t           lockId,
                          uint32_t           acquisitionOrder,
                          SCOREP_ThreadModel model )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Lock:%x", lockId );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadReleaseLock( location,
                                          timestamp,
                                          lockId,
                                          acquisitionOrder,
                                          model );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_ThreadTaskCreate( uint64_t taskId, SCOREP_ThreadModel model )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    /* use the timestamp from the associated enter */
    uint64_t timestamp = SCOREP_Location_GetLastTimestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTaskCreate( location, timestamp, taskId, model );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }

    /* Nothing to do for profiling. */
}


void
SCOREP_ThreadTaskSwitch( uint64_t taskId, SCOREP_ThreadModel model )
{
    SCOREP_Location* location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp = scorep_get_timestamp( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTaskSwitch( location, timestamp, taskId, model );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        uint64_t* metric_values = SCOREP_Metric_Read( location );
        SCOREP_Profile_TaskSwitch( location, taskId, timestamp, metric_values );
    }
}


void
SCOREP_ThreadTaskBegin( SCOREP_RegionHandle regionHandle,
                        uint64_t            taskId,
                        SCOREP_ThreadModel  model )
{
    SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp     = scorep_get_timestamp( location );
    uint64_t*        metric_values = SCOREP_Metric_Read( location );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ThreadTaskSwitch( location, timestamp, taskId, model );

        if ( metric_values )
        {
            /* @todo: Writing metrics to trace file will be improved in the near future */

            SCOREP_Metric_WriteToTrace( location,
                                        timestamp );
        }

        SCOREP_Tracing_Enter( location, timestamp, regionHandle );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TaskBegin( location,
                                  regionHandle,
                                  taskId,
                                  timestamp,
                                  metric_values );
    }
}


void
SCOREP_ThreadTaskEnd( SCOREP_RegionHandle regionHandle,
                      uint64_t            taskId,
                      SCOREP_ThreadModel  model )
{
    SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp     = scorep_get_timestamp( location );
    uint64_t*        metric_values = SCOREP_Metric_Read( location );

    if ( scorep_tracing_consume_event() )
    {
        if ( metric_values )
        {
            /* @todo: Writing metrics to trace file will be improved in the near future */

            SCOREP_Metric_WriteToTrace( location,
                                        timestamp );
        }

        SCOREP_Tracing_Leave( location, timestamp, regionHandle );
        SCOREP_Tracing_ThreadTaskComplete( location, timestamp, taskId, model );
    }
    else if ( !SCOREP_RecordingEnabled() )
    {
        if ( model == SCOREP_THREAD_MODEL_OPENMP )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TaskEnd( location,
                                regionHandle,
                                taskId,
                                timestamp,
                                metric_values );
    }
}


/**
 *
 */
void
SCOREP_ExitRegionOnException( SCOREP_RegionHandle regionHandle )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_EVENTS, "Reg:%u",
                        scorep_handle_to_id( regionHandle ) );

    /* DL: My proposal would be to call scorep_exit_region until we have
       a special event for exits on exceptions. However, for the profiling part
       no special event is planned, but I do not know about OTF2.
     */
    UTILS_NOT_YET_IMPLEMENTED();

    if ( scorep_tracing_consume_event() )
    {
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        /* TODO: The same like for the regular exit */
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

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TriggerInteger( location,
                                       sampling_set->metric_handles[ 0 ],
                                       value );
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

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_Metric( location,
                               timestamp,
                               counterHandle,
                               &value );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TriggerInteger( location,
                                       sampling_set->metric_handles[ 0 ],
                                       value );
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

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_TriggerDouble( location,
                                      sampling_set->metric_handles[ 0 ],
                                      value );
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

    if ( scorep_tracing_consume_event() )
    {
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        /* No action necessary */
    }
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

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ParameterInt64( location,
                                       timestamp,
                                       parameterHandle,
                                       value );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_ParameterInteger( location,
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

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ParameterUint64( location,
                                        timestamp,
                                        parameterHandle,
                                        value );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        /* The SCOREP_Profile_ParameterInteger handles unsigned and signed integers */
        SCOREP_Profile_ParameterInteger( location,
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

    SCOREP_StringHandle string_handle = SCOREP_DefineString( value );

    if ( scorep_tracing_consume_event() )
    {
        SCOREP_Tracing_ParameterString( location,
                                        timestamp,
                                        parameterHandle,
                                        string_handle );
    }

    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_ParameterString( location,
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
