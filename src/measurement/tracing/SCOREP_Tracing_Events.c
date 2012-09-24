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
#include <SCOREP_Properties.h>

#include <scorep_thread.h>
#include <scorep_definition_structs.h>
#include <scorep_definitions.h>


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

    SCOREP_SamplingSet_Definition* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( samplingSet, SamplingSet );
    uint32_t sequence_number = sampling_set->sequence_number;
    if ( sampling_set->is_scoped )
    {
        SCOREP_ScopedSamplingSet_Definition* scoped_sampling_set =
            ( SCOREP_ScopedSamplingSet_Definition* )sampling_set;
        sampling_set = SCOREP_LOCAL_HANDLE_DEREF( scoped_sampling_set->sampling_set_handle,
                                                  SamplingSet );
    }

    OTF2_Type value_types[ sampling_set->number_of_metrics ];
    for ( uint8_t i = 0; i < sampling_set->number_of_metrics; i++ )
    {
        SCOREP_MetricHandle       metric_handle = sampling_set->metric_handles[ i ];
        SCOREP_Metric_Definition* metric        =
            SCOREP_LOCAL_HANDLE_DEREF( metric_handle, Metric );
        value_types[ i ]
            = scorep_tracing_metric_value_type_to_otf2( metric->value_type );
    }

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
SCOREP_Tracing_MpiSend( SCOREP_Location*                  location,
                        uint64_t                          timestamp,
                        SCOREP_MpiRank                    destinationRank,
                        SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                        uint32_t                          tag,
                        uint64_t                          bytesSent )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiSend( evt_writer,
                            NULL,
                            timestamp,
                            destinationRank,
                            SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                            tag,
                            bytesSent );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiRecv( SCOREP_Location*                  location,
                        uint64_t                          timestamp,
                        SCOREP_MpiRank                    sourceRank,
                        SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                        uint32_t                          tag,
                        uint64_t                          bytesSent )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiRecv( evt_writer,
                            NULL,
                            timestamp,
                            sourceRank,
                            SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                            tag,
                            bytesSent );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiCollectiveBegin( SCOREP_Location* location,
                                   uint64_t         timestamp )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiCollectiveBegin( evt_writer,
                                       NULL,
                                       timestamp );
    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiCollectiveEnd( SCOREP_Location*                  location,
                                 uint64_t                          timestamp,
                                 SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                                 SCOREP_MpiRank                    rootRank,
                                 SCOREP_MpiCollectiveType          collectiveType,
                                 uint64_t                          bytesSent,
                                 uint64_t                          bytesReceived )
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
                                     SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                                     root_rank,
                                     bytesSent,
                                     bytesReceived );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_MPI );
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

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_MPI );
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
    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_MPI );
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
    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_MPI );
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
    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiIsend(  SCOREP_Location*                  location,
                          uint64_t                          timestamp,
                          SCOREP_MpiRank                    destinationRank,
                          SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                          uint32_t                          tag,
                          uint64_t                          bytesSent,
                          SCOREP_MpiRequestId               requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiIsend( evt_writer,
                             NULL,
                             timestamp,
                             destinationRank,
                             SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                             tag,
                             bytesSent,
                             requestId );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_MPI );
}


void
SCOREP_Tracing_MpiIrecv( SCOREP_Location*                  location,
                         uint64_t                          timestamp,
                         SCOREP_MpiRank                    sourceRank,
                         SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                         uint32_t                          tag,
                         uint64_t                          bytesReceived,
                         SCOREP_MpiRequestId               requestId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_MpiIrecv( evt_writer,
                             NULL,
                             timestamp,
                             sourceRank,
                             SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, LocalMPICommunicator ),
                             tag,
                             bytesReceived,
                             requestId );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_MPI );
}


void
SCOREP_Tracing_OmpFork( SCOREP_Location* location,
                        uint64_t         timestamp,
                        uint32_t         nRequestedThreads )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_OmpFork( evt_writer,
                            NULL,
                            timestamp,
                            nRequestedThreads );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_OPENMP );
}


void
SCOREP_Tracing_OmpJoin( SCOREP_Location* location,
                        uint64_t         timestamp )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_OmpJoin( evt_writer,
                            NULL,
                            timestamp );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_OPENMP );
}


void
SCOREP_Tracing_OmpAcquireLock( SCOREP_Location* location,
                               uint64_t         timestamp,
                               uint32_t         lockId,
                               uint32_t         acquisitionOrder )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_OmpAcquireLock( evt_writer,
                                   NULL,
                                   timestamp,
                                   lockId,
                                   acquisitionOrder );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_OPENMP );
}


void
SCOREP_Tracing_OmpReleaseLock( SCOREP_Location* location,
                               uint64_t         timestamp,
                               uint32_t         lockId,
                               uint32_t         acquisitionOrder )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_OmpReleaseLock( evt_writer,
                                   NULL,
                                   timestamp,
                                   lockId,
                                   acquisitionOrder );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_OPENMP );
}


void
SCOREP_Tracing_OmpTaskCreate( SCOREP_Location* location,
                              uint64_t         timestamp,
                              uint64_t         taskId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_OmpTaskCreate( evt_writer,
                                  NULL,
                                  timestamp,
                                  taskId );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_OPENMP );
}

void
SCOREP_Tracing_OmpTaskSwitch( SCOREP_Location* location,
                              uint64_t         timestamp,
                              uint64_t         taskId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_OmpTaskSwitch( evt_writer,
                                  NULL,
                                  timestamp,
                                  taskId );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_OPENMP );
}

void
SCOREP_Tracing_OmpTaskComplete( SCOREP_Location* location,
                                uint64_t         timestamp,
                                uint64_t         taskId )
{
    OTF2_EvtWriter* evt_writer = SCOREP_Location_GetTracingData( location )->otf_writer;

    OTF2_EvtWriter_OmpTaskComplete( evt_writer,
                                    NULL,
                                    timestamp,
                                    taskId );

    scorep_rewind_set_affected_paradigm( location, SCOREP_PARADIGM_OPENMP );
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
    scorep_rewind_stack_push( region_id, timestamp );
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
    bool     paradigm_affected[ SCOREP_PARADIGM_MAX ];


    id = SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region );

    /* Search for the region id in the rewind stack, and print a warning when it is not found and leave function. */
    if ( scorep_rewind_stack_find( id ) == false )
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
        scorep_rewind_stack_pop( &id_pop, &entertimestamp, paradigm_affected );

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
        if ( paradigm_affected[ SCOREP_PARADIGM_MPI ] )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_MPI_COMMUNICATION_COMPLETE );
        }
        /* Did it affect OMP events? */
        if ( paradigm_affected[ SCOREP_PARADIGM_OPENMP ] )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_OPENMP_EVENT_COMPLETE );
        }
    }

    /* And remove the rewind point from otf2 internal memory. */
    SCOREP_Tracing_ClearRewindPoint( location, id );
}
