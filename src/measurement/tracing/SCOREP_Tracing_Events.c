/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @file        src/measurement/tracing/SCOREP_Tracing_Events.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief  Fuctions to produce records in the trace file.
 *
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>


#include <scorep_utility/SCOREP_Error.h>
#include <scorep_utility/SCOREP_Debug.h>


#include <otf2/otf2.h>


#include <SCOREP_Types.h>
#include <scorep_thread.h>
#include <SCOREP_Definitions.h>
#include <scorep_definition_structs.h>
#include <scorep_definitions.h>


#include "SCOREP_Tracing_Events.h"


extern OTF2_TypeID* scorep_current_metric_types;

void
SCOREP_Tracing_MeasurementOnOff( SCOREP_Thread_LocationData* location,
                                 uint64_t                    timestamp,
                                 bool                        on )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_MeasurementOnOff( evt_writer,
                                     NULL,
                                     timestamp,
                                     on
                                     ? OTF2_MEASUREMENT_ON
                                     : OTF2_MEASUREMENT_OFF );
}


void
SCOREP_Tracing_Metric( SCOREP_Thread_LocationData* location,
                       uint64_t                    timestamp,
                       SCOREP_SamplingSetHandle    samplingSet,
                       uint8_t                     numberOfMetrics,
                       const uint64_t*             metricValues )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_Metric( evt_writer,
                           NULL,
                           timestamp,
                           SCOREP_LOCAL_HANDLE_TO_ID( samplingSet, SamplingSet ),
                           numberOfMetrics,
                           scorep_current_metric_types,
                           ( const OTF2_MetricValue* )metricValues );
}


void
SCOREP_Tracing_Enter( SCOREP_Thread_LocationData* location,
                      uint64_t                    timestamp,
                      SCOREP_RegionHandle         regionHandle )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_Enter( evt_writer,
                          NULL,
                          timestamp,
                          SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
}


void
SCOREP_Tracing_Leave( SCOREP_Thread_LocationData* location,
                      uint64_t                    timestamp,
                      SCOREP_RegionHandle         regionHandle )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_Leave( evt_writer,
                          NULL,
                          timestamp,
                          SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
}


void
SCOREP_Tracing_MpiSend( SCOREP_Thread_LocationData*       location,
                        uint64_t                          timestamp,
                        SCOREP_MpiRank                    destinationRank,
                        SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                        uint32_t                          tag,
                        uint64_t                          bytesSent )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_MpiSend( evt_writer,
                            NULL,
                            timestamp,
                            destinationRank,
                            SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                            tag,
                            bytesSent );
}


void
SCOREP_Tracing_MpiRecv( SCOREP_Thread_LocationData*       location,
                        uint64_t                          timestamp,
                        SCOREP_MpiRank                    sourceRank,
                        SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                        uint32_t                          tag,
                        uint64_t                          bytesSent )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_MpiRecv( evt_writer,
                            NULL,
                            timestamp,
                            sourceRank,
                            SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                            tag,
                            bytesSent );
}


static OTF2_Mpi_CollectiveType
scorep_tracing_collective_to_otf2( SCOREP_MpiCollectiveType scorep_type )
{
    switch ( scorep_type )
    {
#define CONVERT( name ) \
    case SCOREP_COLLECTIVE_MPI_ ## name: \
        return OTF2_MPI_ ## name

        CONVERT( BARRIER );
        CONVERT( BCAST );
        CONVERT( GATHER );
        CONVERT( GATHERV );
        CONVERT( SCATTER );
        CONVERT( SCATTERV );
        CONVERT( ALLGATHER );
        CONVERT( ALLGATHERV );
        CONVERT( ALLTOALL );
        CONVERT( ALLTOALLV );
        CONVERT( ALLTOALLW );
        CONVERT( ALLREDUCE );
        CONVERT( REDUCE );
        CONVERT( REDUCE_SCATTER );
        CONVERT( REDUCE_SCATTER_BLOCK );
        CONVERT( SCAN );
        CONVERT( EXSCAN );

        default:
            SCOREP_BUG( "Unknown collective type" );
            return 0;

#undef CONVERT
    }
}


void
SCOREP_Tracing_MpiCollectiveBegin( SCOREP_Thread_LocationData* location,
                                   uint64_t                    timestamp )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_MpiCollectiveBegin( evt_writer,
                                       NULL,
                                       timestamp );
}


void
SCOREP_Tracing_MpiCollectiveEnd( SCOREP_Thread_LocationData*       location,
                                 uint64_t                          timestamp,
                                 SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                                 SCOREP_MpiRank                    rootRank,
                                 SCOREP_MpiCollectiveType          collectiveType,
                                 uint64_t                          bytesSent,
                                 uint64_t                          bytesReceived )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    uint32_t        root_rank;
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
                                     scorep_tracing_collective_to_otf2( collectiveType ),
                                     SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                                     root_rank,
                                     bytesSent,
                                     bytesReceived );
}


void
SCOREP_Tracing_MpiIsendComplete( SCOREP_Thread_LocationData* location,
                                 uint64_t                    timestamp,
                                 SCOREP_MpiRequestId         requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_MpiIsendComplete( evt_writer,
                                     NULL,
                                     timestamp,
                                     requestId );
}


void
SCOREP_Tracing_MpiIrecvRequest( SCOREP_Thread_LocationData* location,
                                uint64_t                    timestamp,
                                SCOREP_MpiRequestId         requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_MpiIrecvRequest( evt_writer,
                                    NULL,
                                    timestamp,
                                    requestId );
}


void
SCOREP_Tracing_MpiRequestTested( SCOREP_Thread_LocationData* location,
                                 uint64_t                    timestamp,
                                 SCOREP_MpiRequestId         requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_MpiRequestTest( evt_writer,
                                   NULL,
                                   timestamp,
                                   requestId );
}


void
SCOREP_Tracing_MpiRequestCancelled( SCOREP_Thread_LocationData* location,
                                    uint64_t                    timestamp,
                                    SCOREP_MpiRequestId         requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_MpiRequestCancelled( evt_writer,
                                        NULL,
                                        timestamp,
                                        requestId );
}


void
SCOREP_Tracing_MpiIsend(  SCOREP_Thread_LocationData*       location,
                          uint64_t                          timestamp,
                          SCOREP_MpiRank                    destinationRank,
                          SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                          uint32_t                          tag,
                          uint64_t                          bytesSent,
                          SCOREP_MpiRequestId               requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_MpiIsend( evt_writer,
                             NULL,
                             timestamp,
                             destinationRank,
                             SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                             tag,
                             bytesSent,
                             requestId );
}


void
SCOREP_Tracing_MpiIrecv( SCOREP_Thread_LocationData*       location,
                         uint64_t                          timestamp,
                         SCOREP_MpiRank                    sourceRank,
                         SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                         uint32_t                          tag,
                         uint64_t                          bytesReceived,
                         SCOREP_MpiRequestId               requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_MpiIrecv( evt_writer,
                             NULL,
                             timestamp,
                             sourceRank,
                             SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                             tag,
                             bytesReceived,
                             requestId );
}


void
SCOREP_Tracing_OmpFork( SCOREP_Thread_LocationData* location,
                        uint64_t                    timestamp,
                        uint32_t                    nRequestedThreads )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_OmpFork( evt_writer,
                            NULL,
                            timestamp,
                            nRequestedThreads );
}


void
SCOREP_Tracing_OmpJoin( SCOREP_Thread_LocationData* location,
                        uint64_t                    timestamp )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_OmpJoin( evt_writer,
                            NULL,
                            timestamp );
}


void
SCOREP_Tracing_OmpAcquireLock( SCOREP_Thread_LocationData* location,
                               uint64_t                    timestamp,
                               uint32_t                    lockId,
                               uint32_t                    acquisitionOrder )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_OmpAcquireLock( evt_writer,
                                   NULL,
                                   timestamp,
                                   lockId,
                                   acquisitionOrder );
}


void
SCOREP_Tracing_OmpReleaseLock( SCOREP_Thread_LocationData* location,
                               uint64_t                    timestamp,
                               uint32_t                    lockId,
                               uint32_t                    acquisitionOrder )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_OmpReleaseLock( evt_writer,
                                   NULL,
                                   timestamp,
                                   lockId,
                                   acquisitionOrder );
}


void
SCOREP_Tracing_OmpTaskCreate( SCOREP_Thread_LocationData* location,
                              uint64_t                    timestamp,
                              uint64_t                    taskId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_OmpTaskCreate( evt_writer,
                                  NULL,
                                  timestamp,
                                  taskId );
}

void
SCOREP_Tracing_OmpTaskSwitch( SCOREP_Thread_LocationData* location,
                              uint64_t                    timestamp,
                              uint64_t                    taskId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_OmpTaskSwitch( evt_writer,
                                  NULL,
                                  timestamp,
                                  taskId );
}

void
SCOREP_Tracing_OmpTaskComplete( SCOREP_Thread_LocationData* location,
                                uint64_t                    timestamp,
                                uint64_t                    taskId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_OmpTaskComplete( evt_writer,
                                    NULL,
                                    timestamp,
                                    taskId );
}


void
SCOREP_Tracing_ParameterInt64( SCOREP_Thread_LocationData* location,
                               uint64_t                    timestamp,
                               SCOREP_ParameterHandle      parameterHandle,
                               int64_t                     value )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_ParameterInt( evt_writer,
                                 NULL,
                                 timestamp,
                                 SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                 value );
}

void
SCOREP_Tracing_ParameterUint64( SCOREP_Thread_LocationData* location,
                                uint64_t                    timestamp,
                                SCOREP_ParameterHandle      parameterHandle,
                                uint64_t                    value )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_ParameterUnsignedInt( evt_writer,
                                         NULL,
                                         timestamp,
                                         SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                         value );
}


void
SCOREP_Tracing_ParameterString( SCOREP_Thread_LocationData* location,
                                uint64_t                    timestamp,
                                SCOREP_ParameterHandle      parameterHandle,
                                SCOREP_StringHandle         stringHandle )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Thread_GetTraceLocationData( location )->otf_writer;

    OTF2_EvtWriter_ParameterString( evt_writer,
                                    NULL,
                                    timestamp,
                                    SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                    SCOREP_LOCAL_HANDLE_TO_ID( stringHandle, String ) );
}
