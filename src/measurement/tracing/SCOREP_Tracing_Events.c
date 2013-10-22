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
 * @brief Event consumption functions for tracing.
 */


#include <config.h>

#include "SCOREP_Tracing_Events.h"

#include <stdlib.h>
#include <stdio.h>


#include <UTILS_Error.h>
#include <UTILS_Debug.h>


#include <otf2/otf2.h>


#include <SCOREP_Types.h>
#include <SCOREP_Definitions.h>
#include <definitions/SCOREP_Definitions.h>
#include <SCOREP_Properties.h>

#include <scorep_status.h>

#include "scorep_tracing_internal.h"
#include "scorep_tracing_types.h"


void
SCOREP_Tracing_MeasurementOnOff( SCOREP_Location* location,
                                 uint64_t         timestamp,
                                 bool             on )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MeasurementOnOff( evt_writer,
                                     NULL,
                                     timestamp,
                                     on
                                     ? OTF2_MEASUREMENT_ON
                                     : OTF2_MEASUREMENT_OFF );
}


void
SCOREP_Tracing_Metric( SCOREP_Location*         location,
                       uint64_t                 timestamp,
                       SCOREP_SamplingSetHandle samplingSet,
                       const uint64_t*          metricValues )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( samplingSet, SamplingSet );
    uint32_t sequence_number = sampling_set->sequence_number;
    if ( sampling_set->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_sampling_set =
            ( SCOREP_ScopedSamplingSetDef* )sampling_set;
        sampling_set = SCOREP_LOCAL_HANDLE_DEREF( scoped_sampling_set->sampling_set_handle,
                                                  SamplingSet );
    }

    OTF2_Type* value_types = ( OTF2_Type* )(
        ( char* )sampling_set + sampling_set->tracing_cache_offset );
    OTF2_EvtWriter_Metric( evt_writer,
                           NULL,
                           timestamp,
                           sequence_number,
                           sampling_set->number_of_metrics,
                           value_types,
                           ( const OTF2_MetricValue* )metricValues );
}


void
SCOREP_Tracing_Enter( SCOREP_Location*    location,
                      uint64_t            timestamp,
                      SCOREP_RegionHandle regionHandle )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_Enter( evt_writer,
                          NULL,
                          timestamp,
                          SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
}


void
SCOREP_Tracing_Leave( SCOREP_Location*    location,
                      uint64_t            timestamp,
                      SCOREP_RegionHandle regionHandle )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_Leave( evt_writer,
                          NULL,
                          timestamp,
                          SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
}


void
SCOREP_Tracing_MpiSend( SCOREP_Location*                 location,
                        uint64_t                         timestamp,
                        SCOREP_MpiRank                   destinationRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesSent )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiSend( evt_writer,
                            NULL,
                            timestamp,
                            destinationRank,
                            SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, InterimCommunicator ),
                            tag,
                            bytesSent );

    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiRecv( SCOREP_Location*                 location,
                        uint64_t                         timestamp,
                        SCOREP_MpiRank                   sourceRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesSent )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiRecv( evt_writer,
                            NULL,
                            timestamp,
                            sourceRank,
                            SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, InterimCommunicator ),
                            tag,
                            bytesSent );

    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiCollectiveBegin( SCOREP_Location* location,
                                   uint64_t         timestamp )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiCollectiveBegin( evt_writer,
                                       NULL,
                                       timestamp );
    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiCollectiveEnd( SCOREP_Location*                 location,
                                 uint64_t                         timestamp,
                                 SCOREP_InterimCommunicatorHandle communicatorHandle,
                                 SCOREP_MpiRank                   rootRank,
                                 SCOREP_MpiCollectiveType         collectiveType,
                                 uint64_t                         bytesSent,
                                 uint64_t                         bytesReceived )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    uint32_t root_rank;
    if ( rootRank == SCOREP_INVALID_ROOT_RANK )
    {
        root_rank = OTF2_UNDEFINED_UINT32;
    }
    else
    {
        root_rank = ( uint32_t )rootRank;
    }

    OTF2_EvtWriter_MpiCollectiveEnd( evt_writer,
                                     NULL,
                                     timestamp,
                                     scorep_tracing_collective_type_to_otf2( collectiveType ),
                                     SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, InterimCommunicator ),
                                     root_rank,
                                     bytesSent,
                                     bytesReceived );

    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiIsendComplete( SCOREP_Location*    location,
                                 uint64_t            timestamp,
                                 SCOREP_MpiRequestId requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiIsendComplete( evt_writer,
                                     NULL,
                                     timestamp,
                                     requestId );

    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiIrecvRequest( SCOREP_Location*    location,
                                uint64_t            timestamp,
                                SCOREP_MpiRequestId requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiIrecvRequest( evt_writer,
                                    NULL,
                                    timestamp,
                                    requestId );
    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiRequestTested( SCOREP_Location*    location,
                                 uint64_t            timestamp,
                                 SCOREP_MpiRequestId requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiRequestTest( evt_writer,
                                   NULL,
                                   timestamp,
                                   requestId );
    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiRequestCancelled( SCOREP_Location*    location,
                                    uint64_t            timestamp,
                                    SCOREP_MpiRequestId requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiRequestCancelled( evt_writer,
                                        NULL,
                                        timestamp,
                                        requestId );
    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiIsend(  SCOREP_Location*                 location,
                          uint64_t                         timestamp,
                          SCOREP_MpiRank                   destinationRank,
                          SCOREP_InterimCommunicatorHandle communicatorHandle,
                          uint32_t                         tag,
                          uint64_t                         bytesSent,
                          SCOREP_MpiRequestId              requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiIsend( evt_writer,
                             NULL,
                             timestamp,
                             destinationRank,
                             SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, InterimCommunicator ),
                             tag,
                             bytesSent,
                             requestId );

    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiIrecv( SCOREP_Location*                 location,
                         uint64_t                         timestamp,
                         SCOREP_MpiRank                   sourceRank,
                         SCOREP_InterimCommunicatorHandle communicatorHandle,
                         uint32_t                         tag,
                         uint64_t                         bytesReceived,
                         SCOREP_MpiRequestId              requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiIrecv( evt_writer,
                             NULL,
                             timestamp,
                             sourceRank,
                             SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, InterimCommunicator ),
                             tag,
                             bytesReceived,
                             requestId );

    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


void
SCOREP_Tracing_RmaWinCreate( SCOREP_Location*              location,
                             uint64_t                      timestamp,
                             SCOREP_InterimRmaWindowHandle windowHandle )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaWinCreate( evt_writer,
                                 NULL,
                                 timestamp,
                                 SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaWinDestroy( SCOREP_Location*              location,
                              uint64_t                      timestamp,
                              SCOREP_InterimRmaWindowHandle windowHandle )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaWinDestroy( evt_writer,
                                  NULL,
                                  timestamp,
                                  SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaCollectiveBegin( SCOREP_Location* location,
                                   uint64_t         timestamp )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaCollectiveBegin( evt_writer,
                                       NULL,
                                       timestamp );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaCollectiveEnd( SCOREP_Location*              location,
                                 uint64_t                      timestamp,
                                 SCOREP_MpiCollectiveType      collectiveOp,
                                 SCOREP_RmaSyncLevel           syncLevel,
                                 SCOREP_InterimRmaWindowHandle windowHandle,
                                 uint32_t                      root,
                                 uint64_t                      bytesSent,
                                 uint64_t                      bytesReceived )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaCollectiveEnd( evt_writer,
                                     NULL,
                                     timestamp,
                                     scorep_tracing_collective_type_to_otf2( collectiveOp ),
                                     scorep_tracing_rma_sync_level_to_otf2( syncLevel ),
                                     SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                                     root,
                                     bytesSent,
                                     bytesReceived );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaGroupSync( SCOREP_Location*              location,
                             uint64_t                      timestamp,
                             SCOREP_RmaSyncLevel           syncLevel,
                             SCOREP_InterimRmaWindowHandle windowHandle,
                             SCOREP_GroupHandle            groupHandle )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaGroupSync( evt_writer,
                                 NULL,
                                 timestamp,
                                 scorep_tracing_rma_sync_level_to_otf2( syncLevel ),
                                 SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                                 SCOREP_LOCAL_HANDLE_TO_ID( groupHandle, Group ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaRequestLock( SCOREP_Location*              location,
                               uint64_t                      timestamp,
                               SCOREP_InterimRmaWindowHandle windowHandle,
                               uint32_t                      remote,
                               uint64_t                      lockId,
                               SCOREP_LockType               lockType )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaRequestLock( evt_writer,
                                   NULL,
                                   timestamp,
                                   SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                                   remote,
                                   lockId,
                                   scorep_tracing_lock_type_to_otf2( lockType ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaAcquireLock( SCOREP_Location*              location,
                               uint64_t                      timestamp,
                               SCOREP_InterimRmaWindowHandle windowHandle,
                               uint32_t                      remote,
                               uint64_t                      lockId,
                               SCOREP_LockType               lockType )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaAcquireLock( evt_writer,
                                   NULL,
                                   timestamp,
                                   SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                                   remote,
                                   lockId,
                                   scorep_tracing_lock_type_to_otf2( lockType ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaTryLock( SCOREP_Location*              location,
                           uint64_t                      timestamp,
                           SCOREP_InterimRmaWindowHandle windowHandle,
                           uint32_t                      remote,
                           uint64_t                      lockId,
                           SCOREP_LockType               lockType )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaTryLock( evt_writer,
                               NULL,
                               timestamp,
                               SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                               remote,
                               lockId,
                               scorep_tracing_lock_type_to_otf2( lockType ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaReleaseLock( SCOREP_Location*              location,
                               uint64_t                      timestamp,
                               SCOREP_InterimRmaWindowHandle windowHandle,
                               uint32_t                      remote,
                               uint64_t                      lockId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaReleaseLock( evt_writer,
                                   NULL,
                                   timestamp,
                                   SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                                   remote,
                                   lockId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaSync( SCOREP_Location*              location,
                        uint64_t                      timestamp,
                        SCOREP_InterimRmaWindowHandle windowHandle,
                        uint32_t                      remote,
                        SCOREP_RmaSyncType            syncType )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaSync( evt_writer,
                            NULL,
                            timestamp,
                            SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                            remote,
                            scorep_tracing_rma_sync_type_to_otf2( syncType ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaWaitChange( SCOREP_Location*              location,
                              uint64_t                      timestamp,
                              SCOREP_InterimRmaWindowHandle windowHandle )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaWaitChange( evt_writer,
                                  NULL,
                                  timestamp,
                                  SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaPut( SCOREP_Location*              location,
                       uint64_t                      timestamp,
                       SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      bytes,
                       uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaPut( evt_writer,
                           NULL,
                           timestamp,
                           SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                           remote,
                           bytes,
                           matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaGet( SCOREP_Location*              location,
                       uint64_t                      timestamp,
                       SCOREP_InterimRmaWindowHandle windowHandle,
                       uint32_t                      remote,
                       uint64_t                      bytes,
                       uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaGet( evt_writer,
                           NULL,
                           timestamp,
                           SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                           remote,
                           bytes,
                           matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaAtomic( SCOREP_Location*              location,
                          uint64_t                      timestamp,
                          SCOREP_InterimRmaWindowHandle windowHandle,
                          uint32_t                      remote,
                          SCOREP_RmaAtomicType          type,
                          uint64_t                      bytesSent,
                          uint64_t                      bytesReceived,
                          uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaAtomic( evt_writer,
                              NULL,
                              timestamp,
                              SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                              remote,
                              scorep_tracing_rma_atomic_type_to_otf2( type ),
                              bytesSent,
                              bytesReceived,
                              matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaOpCompleteBlocking( SCOREP_Location*              location,
                                      uint64_t                      timestamp,
                                      SCOREP_InterimRmaWindowHandle windowHandle,
                                      uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaOpCompleteBlocking( evt_writer,
                                          NULL,
                                          timestamp,
                                          SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                                          matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaOpCompleteNonBlocking( SCOREP_Location*              location,
                                         uint64_t                      timestamp,
                                         SCOREP_InterimRmaWindowHandle windowHandle,
                                         uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaOpCompleteNonBlocking( evt_writer,
                                             NULL,
                                             timestamp,
                                             SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                                             matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaOpTest( SCOREP_Location*              location,
                          uint64_t                      timestamp,
                          SCOREP_InterimRmaWindowHandle windowHandle,
                          uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaOpTest( evt_writer,
                              NULL,
                              timestamp,
                              SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                              matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


void
SCOREP_Tracing_RmaOpCompleteRemote( SCOREP_Location*              location,
                                    uint64_t                      timestamp,
                                    SCOREP_InterimRmaWindowHandle windowHandle,
                                    uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_RmaOpCompleteRemote( evt_writer,
                                        NULL,
                                        timestamp,
                                        SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                                        matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
set_rewind_affected_thread_paradigm( SCOREP_Location* location, SCOREP_ParadigmType paradigm )
{
    switch ( paradigm )
    {
#define case_break( thread_model, rewind_paradigm ) \
    case SCOREP_PARADIGM_ ## thread_model: \
        scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_ ## rewind_paradigm ); \
        break;

        case_break( OPENMP, THREAD_FORK_JOIN );
        case_break( THREAD_FORK_JOIN, THREAD_FORK_JOIN );

#undef case_break
        default:
            UTILS_BUG( "Invalid threading model." );
    }
}


void
SCOREP_Tracing_ThreadFork( SCOREP_Location*    location,
                           uint64_t            timestamp,
                           SCOREP_ParadigmType paradigm,
                           uint32_t            nRequestedThreads )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ThreadFork( evt_writer,
                               NULL,
                               timestamp,
                               scorep_tracing_get_otf2_paradigm( paradigm ),
                               nRequestedThreads );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


void
SCOREP_Tracing_ThreadTeamBegin( SCOREP_Location*                 location,
                                uint64_t                         timestamp,
                                SCOREP_ParadigmType              paradigm,
                                SCOREP_InterimCommunicatorHandle threadTeam )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ThreadTeamBegin( evt_writer,
                                    NULL,
                                    timestamp,
                                    SCOREP_LOCAL_HANDLE_TO_ID( threadTeam, InterimCommunicator ) );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


void
SCOREP_Tracing_ThreadTeamEnd( SCOREP_Location*                 location,
                              uint64_t                         timestamp,
                              SCOREP_ParadigmType              paradigm,
                              SCOREP_InterimCommunicatorHandle threadTeam )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ThreadTeamEnd( evt_writer,
                                  NULL,
                                  timestamp,
                                  SCOREP_LOCAL_HANDLE_TO_ID( threadTeam, InterimCommunicator ) );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


void
SCOREP_Tracing_ThreadJoin( SCOREP_Location*    location,
                           uint64_t            timestamp,
                           SCOREP_ParadigmType paradigm )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ThreadJoin( evt_writer,
                               NULL,
                               timestamp,
                               scorep_tracing_get_otf2_paradigm( paradigm ) );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


void
SCOREP_Tracing_ThreadAcquireLock( SCOREP_Location*    location,
                                  uint64_t            timestamp,
                                  SCOREP_ParadigmType paradigm,
                                  uint32_t            lockId,
                                  uint32_t            acquisitionOrder )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ThreadAcquireLock( evt_writer,
                                      NULL,
                                      timestamp,
                                      scorep_tracing_get_otf2_paradigm( paradigm ),
                                      lockId,
                                      acquisitionOrder );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


void
SCOREP_Tracing_ThreadReleaseLock( SCOREP_Location*    location,
                                  uint64_t            timestamp,
                                  SCOREP_ParadigmType paradigm,
                                  uint32_t            lockId,
                                  uint32_t            acquisitionOrder )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ThreadReleaseLock( evt_writer,
                                      NULL,
                                      timestamp,
                                      scorep_tracing_get_otf2_paradigm( paradigm ),
                                      lockId,
                                      acquisitionOrder );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


void
SCOREP_Tracing_ThreadTaskCreate( SCOREP_Location*                 location,
                                 uint64_t                         timestamp,
                                 SCOREP_ParadigmType              paradigm,
                                 SCOREP_InterimCommunicatorHandle threadTeam,
                                 uint32_t                         threadId,
                                 uint32_t                         generationNumber )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ThreadTaskCreate( evt_writer,
                                     NULL,
                                     timestamp,
                                     SCOREP_LOCAL_HANDLE_TO_ID( threadTeam, InterimCommunicator ),
                                     threadId,
                                     generationNumber );

    set_rewind_affected_thread_paradigm( location, paradigm );
}

void
SCOREP_Tracing_ThreadTaskSwitch( SCOREP_Location*                 location,
                                 uint64_t                         timestamp,
                                 SCOREP_ParadigmType              paradigm,
                                 SCOREP_InterimCommunicatorHandle threadTeam,
                                 uint32_t                         threadId,
                                 uint32_t                         generationNumber )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ThreadTaskSwitch( evt_writer,
                                     NULL,
                                     timestamp,
                                     SCOREP_LOCAL_HANDLE_TO_ID( threadTeam, InterimCommunicator ),
                                     threadId,
                                     generationNumber );

    set_rewind_affected_thread_paradigm( location, paradigm );
}

void
SCOREP_Tracing_ThreadTaskComplete( SCOREP_Location*                 location,
                                   uint64_t                         timestamp,
                                   SCOREP_ParadigmType              paradigm,
                                   SCOREP_InterimCommunicatorHandle threadTeam,
                                   uint32_t                         threadId,
                                   uint32_t                         generationNumber )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ThreadTaskComplete( evt_writer,
                                       NULL,
                                       timestamp,
                                       SCOREP_LOCAL_HANDLE_TO_ID( threadTeam, InterimCommunicator ),
                                       threadId,
                                       generationNumber );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


void
SCOREP_Tracing_ParameterInt64( SCOREP_Location*       location,
                               uint64_t               timestamp,
                               SCOREP_ParameterHandle parameterHandle,
                               int64_t                value )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ParameterInt( evt_writer,
                                 NULL,
                                 timestamp,
                                 SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                 value );
}

void
SCOREP_Tracing_ParameterUint64( SCOREP_Location*       location,
                                uint64_t               timestamp,
                                SCOREP_ParameterHandle parameterHandle,
                                uint64_t               value )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ParameterUnsignedInt( evt_writer,
                                         NULL,
                                         timestamp,
                                         SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                         value );
}


void
SCOREP_Tracing_ParameterString( SCOREP_Location*       location,
                                uint64_t               timestamp,
                                SCOREP_ParameterHandle parameterHandle,
                                SCOREP_StringHandle    stringHandle )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ParameterString( evt_writer,
                                    NULL,
                                    timestamp,
                                    SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                    SCOREP_LOCAL_HANDLE_TO_ID( stringHandle, String ) );
}


void
SCOREP_Tracing_StoreRewindPoint( SCOREP_Location*    location,
                                 SCOREP_RegionHandle regionHandle,
                                 uint64_t            timestamp )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    uint32_t region_id = SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region );

    /* Save the current trace buffer state as a rewind point. */
    OTF2_EvtWriter_StoreRewindPoint( evt_writer, region_id );

    /* Push this rewind region on the stack to manage nested rewind points. */
    scorep_rewind_stack_push( location, region_id, timestamp );
}

void
SCOREP_Tracing_ClearRewindPoint( SCOREP_Location* location,
                                 uint32_t         region_id )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_ClearRewindPoint( evt_writer, region_id );
}

void
SCOREP_Tracing_Rewind( SCOREP_Location* location,
                       uint32_t         region_id )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_Rewind( evt_writer, region_id );
}

void
SCOREP_Tracing_ExitRewindRegion( SCOREP_Location*    location,
                                 SCOREP_RegionHandle regionHandle,
                                 uint64_t            leavetimestamp,
                                 bool                do_rewind )
{
    uint64_t entertimestamp = 0;
    uint32_t id             = 0;
    uint32_t id_pop         = 0;
    bool     paradigm_affected[ SCOREP_REWIND_PARADIGM_MAX ];


    id = SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region );

    /* Search for the region id in the rewind stack, and print a warning when it is not found and leave function. */
    if ( scorep_rewind_stack_find( location, id ) == false )
    {
        UTILS_WARNING( "ID of rewind region is not in rewind stack, maybe "
                       "there was a buffer flush or a programming error!" );
        return;
    }

    /* Remove all rewind points from the stack and the buffer until the
     * searched region id for the requested rewind is found.
     * This ensures, that nested rewind regions could be managed without
     * a leck in the needed memory. */
    do
    {
        /* Remove from stack. */
        scorep_rewind_stack_pop( location, &id_pop, &entertimestamp, paradigm_affected );

        /* Remove nested rewind points from otf2 internal memory for rewind points. */
        if ( id != id_pop )
        {
            SCOREP_Tracing_ClearRewindPoint( location, id_pop );
        }
    }
    while ( id != id_pop );

    /* If the trace should be rewinded to the requested rewind point. */
    if ( do_rewind )
    {
        /* Rewind the trace buffer. */
        SCOREP_Tracing_Rewind( location, id );

        /* Write events in the trace to mark the deleted section */

        SCOREP_Tracing_MeasurementOnOff( location, entertimestamp, false );

        SCOREP_Tracing_MeasurementOnOff( location, leavetimestamp, true );

        /* Did it affect MPI events? */
        if ( paradigm_affected[ SCOREP_REWIND_PARADIGM_MPI ] )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
        }
        /* Did it affect thread-fork-join events? */
        if ( paradigm_affected[ SCOREP_REWIND_PARADIGM_THREAD_FORK_JOIN ] )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_FORK_JOIN_EVENT_COMPLETE );
        }
    }

    /* And remove the rewind point from otf2 internal memory. */
    SCOREP_Tracing_ClearRewindPoint( location, id );
}

size_t
SCOREP_Tracing_GetSamplingSetCacheSize( uint32_t numberOfMetrics )
{
    if ( SCOREP_IsTracingEnabled() )
    {
        return numberOfMetrics * sizeof( OTF2_Type );
    }
    return 0;
}

void
SCOREP_Tracing_CacheSamplingSet( SCOREP_SamplingSetHandle samplingSet )
{
    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_SamplingSetDef* sampling_set
            = SCOREP_LOCAL_HANDLE_DEREF( samplingSet, SamplingSet );

        OTF2_Type* value_types = ( OTF2_Type* )(
            ( char* )sampling_set + sampling_set->tracing_cache_offset );
        for ( uint8_t i = 0; i < sampling_set->number_of_metrics; i++ )
        {
            SCOREP_MetricHandle metric_handle = sampling_set->metric_handles[ i ];
            SCOREP_MetricDef*   metric        =
                SCOREP_LOCAL_HANDLE_DEREF( metric_handle, Metric );
            value_types[ i ]
                = scorep_tracing_metric_value_type_to_otf2( metric->value_type );
        }
    }
}
