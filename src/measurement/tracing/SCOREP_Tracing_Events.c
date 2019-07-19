/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015-2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2017-2018,
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
 * @brief Event consumption functions for tracing.
 */


#include <config.h>

#include "SCOREP_Tracing.h"
#include "SCOREP_Tracing_ThreadInteraction.h"
#include "SCOREP_Tracing_Events.h"

#include <SCOREP_IoManagement.h>

#include <stdlib.h>
#include <stdio.h>


#include <UTILS_Error.h>
#include <UTILS_Debug.h>


#include <otf2/otf2.h>


#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Types.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Paradigms.h>
#include <SCOREP_Properties.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Substrates_Management.h>
#include <SCOREP_Unwinding.h>
#include <SCOREP_Config.h>

#include <scorep_environment.h>
#include <scorep_status.h>

#include "scorep_tracing_internal.h"
#include "scorep_tracing_types.h"


/**
 * Enables tracing recording. This function activates tracing recording.
 * @ref enable_recording.
 *
 * @param location     A pointer to the thread location data of the thread that executed
 *                     the enable recording event.
 * @param timestamp    The timestamp, when the enable recording event occurred.
 * @param regionHandle The handle of the region for which the enable recording occurred.
 * @param metricValues Unused.
 *
 */
static void
enable_recording( SCOREP_Location*    location,
                  uint64_t            timestamp,
                  SCOREP_RegionHandle regionHandle,
                  uint64_t*           metricValues )
{
    SCOREP_TracingData* tracing_data = scorep_tracing_get_trace_data( location );
    OTF2_EvtWriter*     evt_writer   = tracing_data->otf_writer;

    OTF2_EvtWriter_MeasurementOnOff( evt_writer,
                                     NULL,
                                     timestamp,
                                     OTF2_MEASUREMENT_ON );

    /* flush the attribute list for this location */
    OTF2_AttributeList_RemoveAllAttributes( tracing_data->otf_attribute_list );
}


/**
 * Disables tracing recording. This function deactivates tracing recording.
 * @ref disable_recording.
 *
 * @param location     A pointer to the thread location data of the thread that executed
 *                     the disable recording event.
 * @param timestamp    The timestamp, when the disable recording event occurred.
 * @param regionHandle The handle of the region for which the disable recording occurred.
 * @param metricValues Unused.
 *
 */
static void
disable_recording( SCOREP_Location*    location,
                   uint64_t            timestamp,
                   SCOREP_RegionHandle regionHandle,
                   uint64_t*           metricValues )
{
    SCOREP_TracingData* tracing_data = scorep_tracing_get_trace_data( location );
    OTF2_EvtWriter*     evt_writer   = tracing_data->otf_writer;

    OTF2_EvtWriter_MeasurementOnOff( evt_writer,
                                     NULL,
                                     timestamp,
                                     OTF2_MEASUREMENT_OFF );

    /* flush the attribute list for this location */
    OTF2_AttributeList_RemoveAllAttributes( tracing_data->otf_attribute_list );
}


static void
write_metric( SCOREP_Location*         location,
              uint64_t                 timestamp,
              SCOREP_SamplingSetHandle samplingSet,
              const uint64_t*          metricValues )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
program_begin( SCOREP_Location*     location,
               uint64_t             timestamp,
               SCOREP_StringHandle  programName,
               uint32_t             numberOfProgramArgs,
               SCOREP_StringHandle* programArguments,
               SCOREP_RegionHandle  regionHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_StringRef args[ numberOfProgramArgs ];
    for ( uint32_t i = 0; i < numberOfProgramArgs; i++ )
    {
        args[ i ] = SCOREP_LOCAL_HANDLE_TO_ID( programArguments[ i ], String );
    }
    OTF2_EvtWriter_ProgramBegin( evt_writer,
                                 NULL,
                                 timestamp,
                                 SCOREP_LOCAL_HANDLE_TO_ID( programName, String ),
                                 numberOfProgramArgs,
                                 args );
}

static void
program_end( SCOREP_Location*    location,
             uint64_t            timestamp,
             SCOREP_ExitStatus   exitStatus,
             SCOREP_RegionHandle regionHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ProgramEnd( evt_writer,
                               NULL,
                               timestamp,
                               exitStatus == SCOREP_INVALID_EXIT_STATUS
                               ? OTF2_UNDEFINED_INT64
                               : ( int64_t )exitStatus );
}



static void
enter( SCOREP_Location*    location,
       uint64_t            timestamp,
       SCOREP_RegionHandle regionHandle,
       uint64_t*           metricValues )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;

    SCOREP_Metric_WriteAsynchronousMetrics( location, write_metric );
    SCOREP_Metric_WriteStrictlySynchronousMetrics( location, timestamp, write_metric );
    SCOREP_Metric_WriteSynchronousMetrics( location, timestamp, write_metric );
    OTF2_EvtWriter_Enter( evt_writer,
                          attribute_list,
                          timestamp,
                          SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
}


static void
leave( SCOREP_Location*    location,
       uint64_t            timestamp,
       SCOREP_RegionHandle regionHandle,
       uint64_t*           metricValues )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;

    SCOREP_Metric_WriteAsynchronousMetrics( location, write_metric );
    SCOREP_Metric_WriteStrictlySynchronousMetrics( location, timestamp, write_metric );
    SCOREP_Metric_WriteSynchronousMetrics( location, timestamp, write_metric );
    OTF2_EvtWriter_Leave( evt_writer,
                          attribute_list,
                          timestamp,
                          SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
}


static void
calling_context_enter( SCOREP_Location*            location,
                       uint64_t                    timestamp,
                       SCOREP_CallingContextHandle currentCallingContextHandle,
                       SCOREP_CallingContextHandle previousCallingContextHandle,
                       uint32_t                    unwindDistance,
                       uint64_t*                   metricValues )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;

    if ( scorep_tracing_convert_calling_context )
    {
        /* This will only process leave events resulting from the previous
         * calling context and enter from the current, except the last one into
         * the current calling context */
        tracing_data->otf_attribute_list = NULL;
        SCOREP_Unwinding_ProcessCallingContext( location,
                                                timestamp,
                                                metricValues,
                                                SCOREP_CallingContextHandle_GetParent( currentCallingContextHandle ),
                                                previousCallingContextHandle,
                                                unwindDistance - 1,
                                                enter,
                                                leave );

        /* The function we actually enter gets the attribute list */
        tracing_data->otf_attribute_list = attribute_list;
        enter( location,
               timestamp,
               SCOREP_CallingContextHandle_GetRegion( currentCallingContextHandle ),
               metricValues );
    }
    else
    {
        SCOREP_Metric_WriteAsynchronousMetrics( location, write_metric );
        SCOREP_Metric_WriteStrictlySynchronousMetrics( location, timestamp, write_metric );
        SCOREP_Metric_WriteSynchronousMetrics( location, timestamp, write_metric );
        OTF2_EvtWriter_CallingContextEnter( evt_writer,
                                            attribute_list,
                                            timestamp,
                                            SCOREP_LOCAL_HANDLE_TO_ID( currentCallingContextHandle, CallingContext ),
                                            unwindDistance );
    }
}


static void
calling_context_leave( SCOREP_Location*            location,
                       uint64_t                    timestamp,
                       SCOREP_CallingContextHandle currentCallingContextHandle,
                       SCOREP_CallingContextHandle previousCallingContextHandle,
                       uint32_t                    unwindDistance,
                       uint64_t*                   metricValues )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;

    if ( scorep_tracing_convert_calling_context )
    {
        /* This will only process leave events resulting from the previous
         * calling context */
        tracing_data->otf_attribute_list = NULL;
        SCOREP_Unwinding_ProcessCallingContext( location,
                                                timestamp,
                                                metricValues,
                                                currentCallingContextHandle,
                                                previousCallingContextHandle,
                                                1,
                                                enter,
                                                leave );

        /* The function we actually enter gets the attribute list */
        tracing_data->otf_attribute_list = attribute_list;
        leave( location,
               timestamp,
               SCOREP_CallingContextHandle_GetRegion( currentCallingContextHandle ),
               metricValues );
    }
    else
    {
        SCOREP_Metric_WriteAsynchronousMetrics( location, write_metric );
        SCOREP_Metric_WriteStrictlySynchronousMetrics( location, timestamp, write_metric );
        SCOREP_Metric_WriteSynchronousMetrics( location, timestamp, write_metric );
        OTF2_EvtWriter_CallingContextLeave( evt_writer,
                                            attribute_list,
                                            timestamp,
                                            SCOREP_LOCAL_HANDLE_TO_ID( currentCallingContextHandle, CallingContext ) );
    }
}


static void
sample( SCOREP_Location*                location,
        uint64_t                        timestamp,
        SCOREP_CallingContextHandle     currentCallingContextHandle,
        SCOREP_CallingContextHandle     previousCallingContextHandle,
        uint32_t                        unwindDistance,
        SCOREP_InterruptGeneratorHandle interruptGeneratorHandle,
        uint64_t*                       metricValues )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;

    SCOREP_Metric_WriteAsynchronousMetrics( location, write_metric );
    if ( scorep_tracing_convert_calling_context )
    {
        /* No place for them yet, maybe last enter in the future, but currenlty there
         * is no way the user could add attributes to samples anyway. */
        OTF2_AttributeList_RemoveAllAttributes( attribute_list );

        SCOREP_Unwinding_ProcessCallingContext( location,
                                                timestamp,
                                                metricValues,
                                                currentCallingContextHandle,
                                                previousCallingContextHandle,
                                                unwindDistance,
                                                enter,
                                                leave );
    }
    else if ( currentCallingContextHandle != SCOREP_INVALID_CALLING_CONTEXT )
    {
        SCOREP_Metric_WriteStrictlySynchronousMetrics( location, timestamp, write_metric );
        SCOREP_Metric_WriteSynchronousMetrics( location, timestamp, write_metric );
        OTF2_EvtWriter_CallingContextSample( evt_writer,
                                             attribute_list,
                                             timestamp,
                                             SCOREP_LOCAL_HANDLE_TO_ID( currentCallingContextHandle, CallingContext ),
                                             unwindDistance,
                                             SCOREP_LOCAL_HANDLE_TO_ID( interruptGeneratorHandle, InterruptGenerator ) );
    }
}


static void
add_attribute( SCOREP_Location*       location,
               SCOREP_AttributeHandle attributeHandle,
               void*                  value )
{
    if ( !SCOREP_RecordingEnabled() )
    {
        return;
    }
    OTF2_AttributeList* attribute_list =
        scorep_tracing_get_trace_data( location )->otf_attribute_list;

    OTF2_AttributeValue  otf_val;
    OTF2_Type            otf_type;
    SCOREP_AttributeType attrType = SCOREP_AttributeHandle_GetType( attributeHandle );

    switch ( attrType )
    {
        case SCOREP_ATTRIBUTE_TYPE_FLOAT:
            otf_val.float32 = *( ( float* )value );
            otf_type        = OTF2_TYPE_FLOAT;
            break;
        case SCOREP_ATTRIBUTE_TYPE_DOUBLE:
            otf_val.float64 = *( ( double* )value );
            otf_type        = OTF2_TYPE_DOUBLE;
            break;

        case SCOREP_ATTRIBUTE_TYPE_INT8:
            otf_val.int8 = *( ( int8_t* )value );
            otf_type     = OTF2_TYPE_INT8;
            break;
        case SCOREP_ATTRIBUTE_TYPE_INT16:
            otf_val.int16 = *( ( int16_t* )value );
            otf_type      = OTF2_TYPE_INT16;
            break;
        case SCOREP_ATTRIBUTE_TYPE_INT32:
            otf_val.int32 = *( ( int32_t* )value );
            otf_type      = OTF2_TYPE_INT32;
            break;
        case SCOREP_ATTRIBUTE_TYPE_INT64:
            otf_val.int64 = *( ( int64_t* )value );
            otf_type      = OTF2_TYPE_INT64;
            break;

        case SCOREP_ATTRIBUTE_TYPE_UINT8:
            otf_val.uint8 = *( ( uint8_t* )value );
            otf_type      = OTF2_TYPE_UINT8;
            break;
        case SCOREP_ATTRIBUTE_TYPE_UINT16:
            otf_val.uint16 = *( ( uint16_t* )value );
            otf_type       = OTF2_TYPE_UINT16;
            break;
        case SCOREP_ATTRIBUTE_TYPE_UINT32:
            otf_val.uint32 = *( ( uint32_t* )value );
            otf_type       = OTF2_TYPE_UINT32;
            break;
        case SCOREP_ATTRIBUTE_TYPE_UINT64:
            otf_val.uint64 = *( ( uint64_t* )value );
            otf_type       = OTF2_TYPE_UINT64;
            break;

        case SCOREP_ATTRIBUTE_TYPE_ATTRIBUTE:
            otf_val.attributeRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_AttributeHandle* )value ), Attribute );
            otf_type = OTF2_TYPE_ATTRIBUTE;
            break;

        case SCOREP_ATTRIBUTE_TYPE_INTERIM_COMMUNICATOR:
            otf_val.commRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_InterimCommunicatorHandle* )value ), InterimCommunicator );
            otf_type = OTF2_TYPE_COMM;
            break;

        case SCOREP_ATTRIBUTE_TYPE_GROUP:
            otf_val.groupRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_GroupHandle* )value ), Group );
            otf_type = OTF2_TYPE_GROUP;
            break;

        case SCOREP_ATTRIBUTE_TYPE_LOCATION:
            otf_val.locationRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_LocationHandle* )value ), Location );
            otf_type = OTF2_TYPE_LOCATION;
            break;

        case SCOREP_ATTRIBUTE_TYPE_METRIC:
            otf_val.metricRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_MetricHandle* )value ), Metric );
            otf_type = OTF2_TYPE_METRIC;
            break;

        case SCOREP_ATTRIBUTE_TYPE_PARAMETER:
            otf_val.parameterRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_ParameterHandle* )value ), Parameter );
            otf_type = OTF2_TYPE_PARAMETER;
            break;

        case SCOREP_ATTRIBUTE_TYPE_REGION:
            otf_val.regionRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_RegionHandle* )value ), Region );
            otf_type = OTF2_TYPE_REGION;
            break;

        case SCOREP_ATTRIBUTE_TYPE_RMA_WINDOW:
            otf_val.rmaWinRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_RmaWindowHandle* )value ), RmaWindow );
            otf_type = OTF2_TYPE_RMA_WIN;
            break;

        case SCOREP_ATTRIBUTE_TYPE_STRING:
            otf_val.stringRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_StringHandle* )value ), String );
            otf_type = OTF2_TYPE_STRING;
            break;

        case SCOREP_ATTRIBUTE_TYPE_SOURCE_CODE_LOCATION:
            otf_val.stringRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_SourceCodeLocationHandle* )value ), SourceCodeLocation );
            otf_type = OTF2_TYPE_SOURCE_CODE_LOCATION;
            break;

        case SCOREP_ATTRIBUTE_TYPE_CALLING_CONTEXT:
            otf_val.stringRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_SourceCodeLocationHandle* )value ), CallingContext );
            otf_type = OTF2_TYPE_CALLING_CONTEXT;
            break;

        case SCOREP_ATTRIBUTE_TYPE_INTERRUPT_GENERATOR:
            otf_val.stringRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_SourceCodeLocationHandle* )value ), InterruptGenerator );
            otf_type = OTF2_TYPE_INTERRUPT_GENERATOR;
            break;

        default:
            UTILS_BUG( "Invalid attribute type: %u", attrType );
    }

    OTF2_AttributeList_AddAttribute( attribute_list,
                                     SCOREP_LOCAL_HANDLE_TO_ID( attributeHandle, Attribute ),
                                     otf_type,
                                     otf_val );
}


static void
mpi_send( SCOREP_Location*                 location,
          uint64_t                         timestamp,
          SCOREP_MpiRank                   destinationRank,
          SCOREP_InterimCommunicatorHandle communicatorHandle,
          uint32_t                         tag,
          uint64_t                         bytesSent )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_MpiSend( evt_writer,
                            NULL,
                            timestamp,
                            destinationRank,
                            SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, InterimCommunicator ),
                            tag,
                            bytesSent );

    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


static void
mpi_recv( SCOREP_Location*                 location,
          uint64_t                         timestamp,
          SCOREP_MpiRank                   sourceRank,
          SCOREP_InterimCommunicatorHandle communicatorHandle,
          uint32_t                         tag,
          uint64_t                         bytesSent )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_MpiRecv( evt_writer,
                            NULL,
                            timestamp,
                            sourceRank,
                            SCOREP_LOCAL_HANDLE_TO_ID( communicatorHandle, InterimCommunicator ),
                            tag,
                            bytesSent );

    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


static void
mpi_collective_begin( SCOREP_Location* location,
                      uint64_t         timestamp )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_MpiCollectiveBegin( evt_writer,
                                       NULL,
                                       timestamp );
    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


static void
mpi_collective_end( SCOREP_Location*                 location,
                    uint64_t                         timestamp,
                    SCOREP_InterimCommunicatorHandle communicatorHandle,
                    SCOREP_MpiRank                   rootRank,
                    SCOREP_CollectiveType            collectiveType,
                    uint64_t                         bytesSent,
                    uint64_t                         bytesReceived )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
mpi_isend_complete( SCOREP_Location*    location,
                    uint64_t            timestamp,
                    SCOREP_MpiRequestId requestId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_MpiIsendComplete( evt_writer,
                                     NULL,
                                     timestamp,
                                     requestId );

    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


static void
mpi_irecv_request( SCOREP_Location*    location,
                   uint64_t            timestamp,
                   SCOREP_MpiRequestId requestId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_MpiIrecvRequest( evt_writer,
                                    NULL,
                                    timestamp,
                                    requestId );
    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


static void
mpi_request_tested( SCOREP_Location*    location,
                    uint64_t            timestamp,
                    SCOREP_MpiRequestId requestId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_MpiRequestTest( evt_writer,
                                   NULL,
                                   timestamp,
                                   requestId );
    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


static void
mpi_request_cancelled( SCOREP_Location*    location,
                       uint64_t            timestamp,
                       SCOREP_MpiRequestId requestId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_MpiRequestCancelled( evt_writer,
                                        NULL,
                                        timestamp,
                                        requestId );
    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_MPI );
}


static void
mpi_isend( SCOREP_Location*                 location,
           uint64_t                         timestamp,
           SCOREP_MpiRank                   destinationRank,
           SCOREP_InterimCommunicatorHandle communicatorHandle,
           uint32_t                         tag,
           uint64_t                         bytesSent,
           SCOREP_MpiRequestId              requestId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
mpi_irecv( SCOREP_Location*                 location,
           uint64_t                         timestamp,
           SCOREP_MpiRank                   sourceRank,
           SCOREP_InterimCommunicatorHandle communicatorHandle,
           uint32_t                         tag,
           uint64_t                         bytesReceived,
           SCOREP_MpiRequestId              requestId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
rma_win_create( SCOREP_Location*       location,
                uint64_t               timestamp,
                SCOREP_RmaWindowHandle windowHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaWinCreate( evt_writer,
                                 NULL,
                                 timestamp,
                                 SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_win_destroy( SCOREP_Location*       location,
                 uint64_t               timestamp,
                 SCOREP_RmaWindowHandle windowHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaWinDestroy( evt_writer,
                                  NULL,
                                  timestamp,
                                  SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_collective_begin( SCOREP_Location* location,
                      uint64_t         timestamp )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaCollectiveBegin( evt_writer,
                                       NULL,
                                       timestamp );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_collective_end( SCOREP_Location*       location,
                    uint64_t               timestamp,
                    SCOREP_CollectiveType  collectiveOp,
                    SCOREP_RmaSyncLevel    syncLevel,
                    SCOREP_RmaWindowHandle windowHandle,
                    uint32_t               root,
                    uint64_t               bytesSent,
                    uint64_t               bytesReceived )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaCollectiveEnd( evt_writer,
                                     NULL,
                                     timestamp,
                                     scorep_tracing_collective_type_to_otf2( collectiveOp ),
                                     scorep_tracing_rma_sync_level_to_otf2( syncLevel ),
                                     SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                                     root,
                                     bytesSent,
                                     bytesReceived );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_group_sync( SCOREP_Location*       location,
                uint64_t               timestamp,
                SCOREP_RmaSyncLevel    syncLevel,
                SCOREP_RmaWindowHandle windowHandle,
                SCOREP_GroupHandle     groupHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaGroupSync( evt_writer,
                                 NULL,
                                 timestamp,
                                 scorep_tracing_rma_sync_level_to_otf2( syncLevel ),
                                 SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                                 SCOREP_LOCAL_HANDLE_TO_ID( groupHandle, Group ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_request_lock( SCOREP_Location*       location,
                  uint64_t               timestamp,
                  SCOREP_RmaWindowHandle windowHandle,
                  uint32_t               remote,
                  uint64_t               lockId,
                  SCOREP_LockType        lockType )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    uint32_t remote_rank;
    if ( remote == SCOREP_ALL_TARGET_RANKS )
    {
        remote_rank = OTF2_UNDEFINED_UINT32;
    }
    else
    {
        remote_rank = ( uint32_t )remote;
    }

    OTF2_EvtWriter_RmaRequestLock( evt_writer,
                                   NULL,
                                   timestamp,
                                   SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                                   remote_rank,
                                   lockId,
                                   scorep_tracing_lock_type_to_otf2( lockType ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_acquire_lock( SCOREP_Location*       location,
                  uint64_t               timestamp,
                  SCOREP_RmaWindowHandle windowHandle,
                  uint32_t               remote,
                  uint64_t               lockId,
                  SCOREP_LockType        lockType )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    uint32_t remote_rank;
    if ( remote == SCOREP_ALL_TARGET_RANKS )
    {
        remote_rank = OTF2_UNDEFINED_UINT32;
    }
    else
    {
        remote_rank = ( uint32_t )remote;
    }

    OTF2_EvtWriter_RmaAcquireLock( evt_writer,
                                   NULL,
                                   timestamp,
                                   SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                                   remote_rank,
                                   lockId,
                                   scorep_tracing_lock_type_to_otf2( lockType ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_try_lock( SCOREP_Location*       location,
              uint64_t               timestamp,
              SCOREP_RmaWindowHandle windowHandle,
              uint32_t               remote,
              uint64_t               lockId,
              SCOREP_LockType        lockType )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    uint32_t remote_rank;
    if ( remote == SCOREP_ALL_TARGET_RANKS )
    {
        remote_rank = OTF2_UNDEFINED_UINT32;
    }
    else
    {
        remote_rank = ( uint32_t )remote;
    }

    OTF2_EvtWriter_RmaTryLock( evt_writer,
                               NULL,
                               timestamp,
                               SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                               remote_rank,
                               lockId,
                               scorep_tracing_lock_type_to_otf2( lockType ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_release_lock( SCOREP_Location*       location,
                  uint64_t               timestamp,
                  SCOREP_RmaWindowHandle windowHandle,
                  uint32_t               remote,
                  uint64_t               lockId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    uint32_t remote_rank;
    if ( remote == SCOREP_ALL_TARGET_RANKS )
    {
        remote_rank = OTF2_UNDEFINED_UINT32;
    }
    else
    {
        remote_rank = ( uint32_t )remote;
    }

    OTF2_EvtWriter_RmaReleaseLock( evt_writer,
                                   NULL,
                                   timestamp,
                                   SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                                   remote_rank,
                                   lockId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_sync( SCOREP_Location*       location,
          uint64_t               timestamp,
          SCOREP_RmaWindowHandle windowHandle,
          uint32_t               remote,
          SCOREP_RmaSyncType     syncType )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaSync( evt_writer,
                            NULL,
                            timestamp,
                            SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                            remote,
                            scorep_tracing_rma_sync_type_to_otf2( syncType ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_wait_change( SCOREP_Location*       location,
                 uint64_t               timestamp,
                 SCOREP_RmaWindowHandle windowHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaWaitChange( evt_writer,
                                  NULL,
                                  timestamp,
                                  SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_put( SCOREP_Location*       location,
         uint64_t               timestamp,
         SCOREP_RmaWindowHandle windowHandle,
         uint32_t               remote,
         uint64_t               bytes,
         uint64_t               matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaPut( evt_writer,
                           NULL,
                           timestamp,
                           SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                           remote,
                           bytes,
                           matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_get( SCOREP_Location*       location,
         uint64_t               timestamp,
         SCOREP_RmaWindowHandle windowHandle,
         uint32_t               remote,
         uint64_t               bytes,
         uint64_t               matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaGet( evt_writer,
                           NULL,
                           timestamp,
                           SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                           remote,
                           bytes,
                           matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_atomic( SCOREP_Location*       location,
            uint64_t               timestamp,
            SCOREP_RmaWindowHandle windowHandle,
            uint32_t               remote,
            SCOREP_RmaAtomicType   type,
            uint64_t               bytesSent,
            uint64_t               bytesReceived,
            uint64_t               matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaAtomic( evt_writer,
                              NULL,
                              timestamp,
                              SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                              remote,
                              scorep_tracing_rma_atomic_type_to_otf2( type ),
                              bytesSent,
                              bytesReceived,
                              matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_op_complete_blocking( SCOREP_Location*       location,
                          uint64_t               timestamp,
                          SCOREP_RmaWindowHandle windowHandle,
                          uint64_t               matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaOpCompleteBlocking( evt_writer,
                                          NULL,
                                          timestamp,
                                          SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                                          matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_op_complete_non_blocking( SCOREP_Location*       location,
                              uint64_t               timestamp,
                              SCOREP_RmaWindowHandle windowHandle,
                              uint64_t               matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaOpCompleteNonBlocking( evt_writer,
                                             NULL,
                                             timestamp,
                                             SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                                             matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_op_test( SCOREP_Location*       location,
             uint64_t               timestamp,
             SCOREP_RmaWindowHandle windowHandle,
             uint64_t               matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaOpTest( evt_writer,
                              NULL,
                              timestamp,
                              SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                              matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_op_complete_remote( SCOREP_Location*       location,
                        uint64_t               timestamp,
                        SCOREP_RmaWindowHandle windowHandle,
                        uint64_t               matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaOpCompleteRemote( evt_writer,
                                        NULL,
                                        timestamp,
                                        SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, RmaWindow ),
                                        matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
io_create_handle( SCOREP_Location*      location,
                  uint64_t              timestamp,
                  SCOREP_IoHandleHandle handle,
                  SCOREP_IoAccessMode   mode,
                  SCOREP_IoCreationFlag creationFlags,
                  SCOREP_IoStatusFlag   statusFlags )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoCreateHandle( evt_writer,
                                   attribute_list,
                                   timestamp,
                                   SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ),
                                   scorep_tracing_io_access_mode_to_otf2( mode ),
                                   scorep_tracing_io_creation_flags_to_otf2( creationFlags ),
                                   scorep_tracing_io_status_flags_to_otf2( statusFlags ) );
}


static void
io_destroy_handle( SCOREP_Location*      location,
                   uint64_t              timestamp,
                   SCOREP_IoHandleHandle handle )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoDestroyHandle( evt_writer,
                                    attribute_list,
                                    timestamp,
                                    SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ) );
}


static void
io_duplicate_handle( SCOREP_Location*      location,
                     uint64_t              timestamp,
                     SCOREP_IoHandleHandle oldHandle,
                     SCOREP_IoHandleHandle newHandle,
                     SCOREP_IoStatusFlag   statusFlags )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoDuplicateHandle( evt_writer,
                                      NULL,
                                      timestamp,
                                      SCOREP_LOCAL_HANDLE_TO_ID( oldHandle, IoHandle ),
                                      SCOREP_LOCAL_HANDLE_TO_ID( newHandle, IoHandle ),
                                      scorep_tracing_io_status_flags_to_otf2( statusFlags ) );
}


static void
io_seek( SCOREP_Location*      location,
         uint64_t              timestamp,
         SCOREP_IoHandleHandle handle,
         int64_t               offsetRequest,
         SCOREP_IoSeekOption   whence,
         uint64_t              offsetResult )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoSeek( evt_writer,
                           attribute_list,
                           timestamp,
                           SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ),
                           offsetRequest,
                           scorep_tracing_io_seek_option_to_otf2( whence ),
                           offsetResult );
}


static void
io_change_status_flags( SCOREP_Location*      location,
                        uint64_t              timestamp,
                        SCOREP_IoHandleHandle handle,
                        SCOREP_IoStatusFlag   statusFlags )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoChangeStatusFlags( evt_writer,
                                        attribute_list,
                                        timestamp,
                                        SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ),
                                        scorep_tracing_io_status_flags_to_otf2( statusFlags ) );
}


static void
io_delete_file( SCOREP_Location*      location,
                uint64_t              timestamp,
                SCOREP_IoParadigmType ioParadigm,
                SCOREP_IoFileHandle   ioFile )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoDeleteFile( evt_writer,
                                 attribute_list,
                                 timestamp,
                                 SCOREP_IoMgmt_GetParadigmId( ioParadigm ),
                                 SCOREP_LOCAL_HANDLE_TO_ID( ioFile, IoFile ) );
}


static void
io_operation_begin( SCOREP_Location*       location,
                    uint64_t               timestamp,
                    SCOREP_IoHandleHandle  handle,
                    SCOREP_IoOperationMode mode,
                    SCOREP_IoOperationFlag operationFlags,
                    uint64_t               bytesRequest,
                    uint64_t               matchingId )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    if ( bytesRequest == SCOREP_IO_UNKOWN_TRANSFER_SIZE )
    {
        bytesRequest = OTF2_UNDEFINED_UINT64;
    }

    OTF2_EvtWriter_IoOperationBegin( evt_writer,
                                     attribute_list,
                                     timestamp,
                                     SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ),
                                     scorep_tracing_io_operation_mode_to_otf2( mode ),
                                     scorep_tracing_io_operation_flag_to_otf2( operationFlags ),
                                     bytesRequest,
                                     matchingId );
}


static void
io_operation_issued( SCOREP_Location*      location,
                     uint64_t              timestamp,
                     SCOREP_IoHandleHandle handle,
                     uint64_t              matchingId )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoOperationIssued( evt_writer,
                                      attribute_list,
                                      timestamp,
                                      SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ),
                                      matchingId );
}


static void
io_operation_test( SCOREP_Location*      location,
                   uint64_t              timestamp,
                   SCOREP_IoHandleHandle handle,
                   uint64_t              matchingId )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoOperationTest( evt_writer,
                                    attribute_list,
                                    timestamp,
                                    SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ),
                                    matchingId );
}


static void
io_operation_complete( SCOREP_Location*       location,
                       uint64_t               timestamp,
                       SCOREP_IoHandleHandle  handle,
                       SCOREP_IoOperationMode mode,
                       uint64_t               bytesResult,
                       uint64_t               matchingId )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    if ( bytesResult == SCOREP_IO_UNKOWN_TRANSFER_SIZE )
    {
        bytesResult = OTF2_UNDEFINED_UINT64;
    }

    OTF2_EvtWriter_IoOperationComplete( evt_writer,
                                        attribute_list,
                                        timestamp,
                                        SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ),
                                        bytesResult,
                                        matchingId );
}


static void
io_operation_cancelled( SCOREP_Location*      location,
                        uint64_t              timestamp,
                        SCOREP_IoHandleHandle handle,
                        uint64_t              matchingId )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoOperationCancelled( evt_writer,
                                         attribute_list,
                                         timestamp,
                                         SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ),
                                         matchingId );
}


static void
io_acquire_lock( SCOREP_Location*      location,
                 uint64_t              timestamp,
                 SCOREP_IoHandleHandle handle,
                 SCOREP_LockType       lockType )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoAcquireLock( evt_writer,
                                  attribute_list,
                                  timestamp,
                                  SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ),
                                  scorep_tracing_lock_type_to_otf2( lockType ) );
}


static void
io_release_lock( SCOREP_Location*      location,
                 uint64_t              timestamp,
                 SCOREP_IoHandleHandle handle,
                 SCOREP_LockType       lockType )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoReleaseLock( evt_writer,
                                  attribute_list,
                                  timestamp,
                                  SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ),
                                  scorep_tracing_lock_type_to_otf2( lockType ) );
}


static void
io_try_lock( SCOREP_Location*      location,
             uint64_t              timestamp,
             SCOREP_IoHandleHandle handle,
             SCOREP_LockType       lockType )
{
    SCOREP_TracingData* tracing_data   = scorep_tracing_get_trace_data( location );
    OTF2_AttributeList* attribute_list = tracing_data->otf_attribute_list;
    OTF2_EvtWriter*     evt_writer     = tracing_data->otf_writer;

    OTF2_EvtWriter_IoTryLock( evt_writer,
                              attribute_list,
                              timestamp,
                              SCOREP_LOCAL_HANDLE_TO_ID( handle, IoHandle ),
                              scorep_tracing_lock_type_to_otf2( lockType ) );
}


static void
set_rewind_affected_thread_paradigm( SCOREP_Location*    location,
                                     SCOREP_ParadigmType paradigm )
{
    switch ( SCOREP_Paradigms_GetParadigmClass( paradigm ) )
    {
#define case_break( threading_class ) \
    case SCOREP_PARADIGM_CLASS_ ## threading_class: \
        scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_ ## threading_class ); \
        break

        case_break( THREAD_FORK_JOIN );
        case_break( THREAD_CREATE_WAIT );

#undef case_break
        default:
            UTILS_BUG( "Unhandled threading model: %u", paradigm );
    }
}


static void
thread_fork( SCOREP_Location*    location,
             uint64_t            timestamp,
             SCOREP_ParadigmType paradigm,
             uint32_t            nRequestedThreads,
             uint32_t            forkSequenceCount )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadFork( evt_writer,
                               NULL,
                               timestamp,
                               scorep_tracing_paradigm_to_otf2( paradigm ),
                               nRequestedThreads );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


static void
thread_team_begin( SCOREP_Location*                 location,
                   uint64_t                         timestamp,
                   SCOREP_ParadigmType              paradigm,
                   SCOREP_InterimCommunicatorHandle threadTeam )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadTeamBegin( evt_writer,
                                    NULL,
                                    timestamp,
                                    SCOREP_LOCAL_HANDLE_TO_ID( threadTeam, InterimCommunicator ) );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


static void
thread_team_end( SCOREP_Location*                 location,
                 uint64_t                         timestamp,
                 SCOREP_ParadigmType              paradigm,
                 SCOREP_InterimCommunicatorHandle threadTeam )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadTeamEnd( evt_writer,
                                  NULL,
                                  timestamp,
                                  SCOREP_LOCAL_HANDLE_TO_ID( threadTeam, InterimCommunicator ) );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


static void
thread_join( SCOREP_Location*    location,
             uint64_t            timestamp,
             SCOREP_ParadigmType paradigm )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadJoin( evt_writer,
                               NULL,
                               timestamp,
                               scorep_tracing_paradigm_to_otf2( paradigm ) );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


static void
thread_create( SCOREP_Location*                 location,
               uint64_t                         timestamp,
               SCOREP_ParadigmType              paradigm,
               SCOREP_InterimCommunicatorHandle threadTeam,
               uint32_t                         createSequenceCount )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadCreate( evt_writer,
                                 NULL,
                                 timestamp,
                                 SCOREP_LOCAL_HANDLE_TO_ID( threadTeam,
                                                            InterimCommunicator ),
                                 createSequenceCount == SCOREP_THREAD_INVALID_SEQUENCE_COUNT ? OTF2_UNDEFINED_UINT64 : createSequenceCount );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


static void
thread_wait( SCOREP_Location*                 location,
             uint64_t                         timestamp,
             SCOREP_ParadigmType              paradigm,
             SCOREP_InterimCommunicatorHandle threadTeam,
             uint32_t                         createSequenceCount )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadWait( evt_writer,
                               NULL,
                               timestamp,
                               SCOREP_LOCAL_HANDLE_TO_ID( threadTeam,
                                                          InterimCommunicator ),
                               createSequenceCount == SCOREP_THREAD_INVALID_SEQUENCE_COUNT ? OTF2_UNDEFINED_UINT64 : createSequenceCount );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


static void
thread_begin( SCOREP_Location*                 location,
              uint64_t                         timestamp,
              SCOREP_ParadigmType              paradigm,
              SCOREP_InterimCommunicatorHandle threadTeam,
              uint32_t                         createSequenceCount )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadBegin( evt_writer,
                                NULL,
                                timestamp,
                                SCOREP_LOCAL_HANDLE_TO_ID( threadTeam,
                                                           InterimCommunicator ),
                                createSequenceCount == SCOREP_THREAD_INVALID_SEQUENCE_COUNT ? OTF2_UNDEFINED_UINT64 : createSequenceCount );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


static void
thread_end( SCOREP_Location*                 location,
            uint64_t                         timestamp,
            SCOREP_ParadigmType              paradigm,
            SCOREP_InterimCommunicatorHandle threadTeam,
            uint32_t                         createSequenceCount )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadEnd( evt_writer,
                              NULL,
                              timestamp,
                              SCOREP_LOCAL_HANDLE_TO_ID( threadTeam,
                                                         InterimCommunicator ),
                              createSequenceCount == SCOREP_THREAD_INVALID_SEQUENCE_COUNT ? OTF2_UNDEFINED_UINT64 : createSequenceCount );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


static void
thread_acquire_lock( SCOREP_Location*    location,
                     uint64_t            timestamp,
                     SCOREP_ParadigmType paradigm,
                     uint32_t            lockId,
                     uint32_t            acquisitionOrder )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadAcquireLock( evt_writer,
                                      NULL,
                                      timestamp,
                                      scorep_tracing_paradigm_to_otf2( paradigm ),
                                      lockId,
                                      acquisitionOrder );

    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_THREAD_LOCKING );
}


static void
thread_release_lock( SCOREP_Location*    location,
                     uint64_t            timestamp,
                     SCOREP_ParadigmType paradigm,
                     uint32_t            lockId,
                     uint32_t            acquisitionOrder )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadReleaseLock( evt_writer,
                                      NULL,
                                      timestamp,
                                      scorep_tracing_paradigm_to_otf2( paradigm ),
                                      lockId,
                                      acquisitionOrder );

    scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_THREAD_LOCKING );
}


static void
thread_task_create( SCOREP_Location*                 location,
                    uint64_t                         timestamp,
                    SCOREP_ParadigmType              paradigm,
                    SCOREP_InterimCommunicatorHandle threadTeam,
                    uint32_t                         threadId,
                    uint32_t                         generationNumber )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadTaskCreate( evt_writer,
                                     NULL,
                                     timestamp,
                                     SCOREP_LOCAL_HANDLE_TO_ID( threadTeam, InterimCommunicator ),
                                     threadId,
                                     generationNumber );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


static void
thread_task_switch( SCOREP_Location*                 location,
                    uint64_t                         timestamp,
                    uint64_t*                        metricValues,
                    SCOREP_ParadigmType              paradigm,
                    SCOREP_InterimCommunicatorHandle threadTeam,
                    uint32_t                         threadId,
                    uint32_t                         generationNumber,
                    SCOREP_TaskHandle                taskHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadTaskSwitch( evt_writer,
                                     NULL,
                                     timestamp,
                                     SCOREP_LOCAL_HANDLE_TO_ID( threadTeam, InterimCommunicator ),
                                     threadId,
                                     generationNumber );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


static void
thread_task_begin( SCOREP_Location*                 location,
                   uint64_t                         timestamp,
                   SCOREP_RegionHandle              regionHandle,
                   uint64_t*                        metricValues,
                   SCOREP_ParadigmType              paradigm,
                   SCOREP_InterimCommunicatorHandle threadTeam,
                   uint32_t                         threadId,
                   uint32_t                         generationNumber,
                   SCOREP_TaskHandle                taskHandle )
{
    thread_task_switch( location,
                        timestamp,
                        metricValues,
                        paradigm,
                        threadTeam,
                        threadId,
                        generationNumber,
                        taskHandle );

    enter( location, timestamp, regionHandle, NULL );
}


static void
thread_task_complete( SCOREP_Location*                 location,
                      uint64_t                         timestamp,
                      SCOREP_RegionHandle              regionHandle,
                      uint64_t*                        metricValues,
                      SCOREP_ParadigmType              paradigm,
                      SCOREP_InterimCommunicatorHandle threadTeam,
                      uint32_t                         threadId,
                      uint32_t                         generationNumber,
                      SCOREP_TaskHandle                taskHandle )
{
    leave( location, timestamp, regionHandle, metricValues );

    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ThreadTaskComplete( evt_writer,
                                       NULL,
                                       timestamp,
                                       SCOREP_LOCAL_HANDLE_TO_ID( threadTeam, InterimCommunicator ),
                                       threadId,
                                       generationNumber );

    set_rewind_affected_thread_paradigm( location, paradigm );
}


static void
trigger_counter_int64( SCOREP_Location*         location,
                       uint64_t                 timestamp,
                       SCOREP_SamplingSetHandle counterHandle,
                       int64_t                  value )
{
    union
    {
        uint64_t uint64;
        int64_t  int64;
    } union_value;
    union_value.int64 = value;

    write_metric( location,
                  timestamp,
                  counterHandle,
                  &union_value.uint64 );
}


static void
trigger_counter_uint64( SCOREP_Location*         location,
                        uint64_t                 timestamp,
                        SCOREP_SamplingSetHandle counterHandle,
                        uint64_t                 value )
{
    write_metric( location,
                  timestamp,
                  counterHandle,
                  &value );
}


static void
trigger_counter_double( SCOREP_Location*         location,
                        uint64_t                 timestamp,
                        SCOREP_SamplingSetHandle counterHandle,
                        double                   value )
{
    union
    {
        uint64_t uint64;
        double   float64;
    } union_value;
    union_value.float64 = value;

    write_metric( location,
                  timestamp,
                  counterHandle,
                  &union_value.uint64 );
}


static void
parameter_int64( SCOREP_Location*       location,
                 uint64_t               timestamp,
                 SCOREP_ParameterHandle parameterHandle,
                 int64_t                value )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ParameterInt( evt_writer,
                                 NULL,
                                 timestamp,
                                 SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                 value );
}

static void
parameter_uint64( SCOREP_Location*       location,
                  uint64_t               timestamp,
                  SCOREP_ParameterHandle parameterHandle,
                  uint64_t               value )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ParameterUnsignedInt( evt_writer,
                                         NULL,
                                         timestamp,
                                         SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                         value );
}


static void
parameter_string( SCOREP_Location*       location,
                  uint64_t               timestamp,
                  SCOREP_ParameterHandle parameterHandle,
                  SCOREP_StringHandle    stringHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ParameterString( evt_writer,
                                    NULL,
                                    timestamp,
                                    SCOREP_LOCAL_HANDLE_TO_ID( parameterHandle, Parameter ),
                                    SCOREP_LOCAL_HANDLE_TO_ID( stringHandle, String ) );
}


static void
store_rewind_point( SCOREP_Location*    location,
                    uint64_t            timestamp,
                    SCOREP_RegionHandle regionHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    uint32_t region_id = SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region );

    /* Save the current trace buffer state as a rewind point. */
    OTF2_EvtWriter_StoreRewindPoint( evt_writer, region_id );

    /* Push this rewind region on the stack to manage nested rewind points. */
    scorep_rewind_stack_push( location, region_id, timestamp );
}

static void
clear_rewind_point( SCOREP_Location* location,
                    uint32_t         region_id )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_ClearRewindPoint( evt_writer, region_id );
}

static inline void
rewind_trace_buffer( SCOREP_Location* location,
                     uint32_t         region_id )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_Rewind( evt_writer, region_id );
}

static void
exit_rewind_point( SCOREP_Location*    location,
                   uint64_t            leavetimestamp,
                   SCOREP_RegionHandle regionHandle,
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
     * a lack in the needed memory. */
    do
    {
        /* Remove from stack. */
        scorep_rewind_stack_pop( location, &id_pop, &entertimestamp, paradigm_affected );

        /* Remove nested rewind points from otf2 internal memory for rewind points. */
        if ( id != id_pop )
        {
            clear_rewind_point( location, id_pop );
        }
    }
    while ( id != id_pop );

    /* If the trace should be rewinded to the requested rewind point. */
    if ( do_rewind )
    {
        /* Rewind the trace buffer. */
        rewind_trace_buffer( location, id );

        /* Write events in the trace to mark the deleted section */

        disable_recording( location, entertimestamp, regionHandle, NULL );

        enable_recording( location, entertimestamp, regionHandle, NULL );

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
        /* Did it affect thread-create-wait events? */
        if ( paradigm_affected[ SCOREP_REWIND_PARADIGM_THREAD_CREATE_WAIT ] )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_CREATE_WAIT_EVENT_COMPLETE );
        }
        /* Did it affect thread-locking events? */
        if ( paradigm_affected[ SCOREP_REWIND_PARADIGM_THREAD_LOCKING ] )
        {
            SCOREP_InvalidateProperty( SCOREP_PROPERTY_THREAD_LOCK_EVENT_COMPLETE );
        }
    }

    /* And remove the rewind point from otf2 internal memory. */
    clear_rewind_point( location, id );
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

static void
dump_manifest( FILE* manifestFile, const char* relativeSourceDir, const char* targetDir )
{
    UTILS_ASSERT( manifestFile );

    SCOREP_ConfigManifestSectionHeader( manifestFile, "Tracing" );
    SCOREP_ConfigManifestSectionEntry( manifestFile, "traces.otf2", "OTF2 anchor file." );
    SCOREP_ConfigManifestSectionEntry( manifestFile, "traces.def", "OTF2 global definitions file." );
    SCOREP_ConfigManifestSectionEntry( manifestFile, "traces/", "Sub-directory containing per location trace data." );
}

static bool
get_requirement( SCOREP_Substrates_RequirementFlag flag )
{
    switch ( flag )
    {
        case SCOREP_SUBSTRATES_REQUIREMENT_CREATE_EXPERIMENT_DIRECTORY:
            return true;
        default:
            return false;
    }
}

const static SCOREP_Substrates_Callback substrate_callbacks[ SCOREP_SUBSTRATES_NUM_MODES ][ SCOREP_SUBSTRATES_NUM_EVENTS ] =
{
    /* SCOREP_SUBSTRATES_RECORDING_ENABLED */
    {
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ProgramBegin,             PROGRAM_BEGIN,                program_begin ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ProgramEnd,               PROGRAM_END,                  program_end ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( EnterRegion,              ENTER_REGION,                 enter ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ExitRegion,               EXIT_REGION,                  leave ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( Sample,                   SAMPLE,                       sample ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( CallingContextEnter,      CALLING_CONTEXT_ENTER,        calling_context_enter ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( CallingContextExit,       CALLING_CONTEXT_EXIT,         calling_context_leave ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( EnterRewindRegion,        ENTER_REWIND_REGION,          store_rewind_point ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ExitRewindRegion,         EXIT_REWIND_REGION,           exit_rewind_point ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiSend,                  MPI_SEND,                     mpi_send ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiRecv,                  MPI_RECV,                     mpi_recv ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiCollectiveBegin,       MPI_COLLECTIVE_BEGIN,         mpi_collective_begin ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiCollectiveEnd,         MPI_COLLECTIVE_END,           mpi_collective_end ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiIsendComplete,         MPI_ISEND_COMPLETE,           mpi_isend_complete ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiIrecvRequest,          MPI_IRECV_REQUEST,            mpi_irecv_request ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiRequestTested,         MPI_REQUEST_TESTED,           mpi_request_tested ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiRequestCancelled,      MPI_REQUEST_CANCELLED,        mpi_request_cancelled ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiIsend,                 MPI_ISEND,                    mpi_isend ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiIrecv,                 MPI_IRECV,                    mpi_irecv ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaWinCreate,             RMA_WIN_CREATE,               rma_win_create ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaWinDestroy,            RMA_WIN_DESTROY,              rma_win_destroy ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaCollectiveBegin,       RMA_COLLECTIVE_BEGIN,         rma_collective_begin ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaCollectiveEnd,         RMA_COLLECTIVE_END,           rma_collective_end ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaTryLock,               RMA_TRY_LOCK,                 rma_try_lock ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaAcquireLock,           RMA_ACQUIRE_LOCK,             rma_acquire_lock ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaRequestLock,           RMA_REQUEST_LOCK,             rma_request_lock ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaReleaseLock,           RMA_RELEASE_LOCK,             rma_release_lock ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaSync,                  RMA_SYNC,                     rma_sync ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaGroupSync,             RMA_GROUP_SYNC,               rma_group_sync ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaPut,                   RMA_PUT,                      rma_put ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaGet,                   RMA_GET,                      rma_get ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaAtomic,                RMA_ATOMIC,                   rma_atomic ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaWaitChange,            RMA_WAIT_CHANGE,              rma_wait_change ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaOpCompleteBlocking,    RMA_OP_COMPLETE_BLOCKING,     rma_op_complete_blocking ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaOpCompleteNonBlocking, RMA_OP_COMPLETE_NON_BLOCKING, rma_op_complete_non_blocking ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaOpTest,                RMA_OP_TEST,                  rma_op_test ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaOpCompleteRemote,      RMA_OP_COMPLETE_REMOTE,       rma_op_complete_remote ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadAcquireLock,        THREAD_ACQUIRE_LOCK,          thread_acquire_lock ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadReleaseLock,        THREAD_RELEASE_LOCK,          thread_release_lock ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerCounterInt64,      TRIGGER_COUNTER_INT64,        trigger_counter_int64 ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerCounterUint64,     TRIGGER_COUNTER_UINT64,       trigger_counter_uint64 ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerCounterDouble,     TRIGGER_COUNTER_DOUBLE,       trigger_counter_double ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerParameterInt64,    TRIGGER_PARAMETER_INT64,      parameter_int64 ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerParameterUint64,   TRIGGER_PARAMETER_UINT64,     parameter_uint64 ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerParameterString,   TRIGGER_PARAMETER_STRING,     parameter_string ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinFork,       THREAD_FORK_JOIN_FORK,        thread_fork ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinJoin,       THREAD_FORK_JOIN_JOIN,        thread_join ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTeamBegin,  THREAD_FORK_JOIN_TEAM_BEGIN,  thread_team_begin ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTeamEnd,    THREAD_FORK_JOIN_TEAM_END,    thread_team_end ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTaskCreate, THREAD_FORK_JOIN_TASK_CREATE, thread_task_create ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTaskSwitch, THREAD_FORK_JOIN_TASK_SWITCH, thread_task_switch ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTaskBegin,  THREAD_FORK_JOIN_TASK_BEGIN,  thread_task_begin ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTaskEnd,    THREAD_FORK_JOIN_TASK_END,    thread_task_complete ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitCreate,   THREAD_CREATE_WAIT_CREATE,    thread_create ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitWait,     THREAD_CREATE_WAIT_WAIT,      thread_wait ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitBegin,    THREAD_CREATE_WAIT_BEGIN,     thread_begin ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitEnd,      THREAD_CREATE_WAIT_END,       thread_end ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoCreateHandle,           IO_CREATE_HANDLE,             io_create_handle ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoDestroyHandle,          IO_DESTROY_HANDLE,            io_destroy_handle ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoDuplicateHandle,        IO_DUPLICATE_HANDLE,          io_duplicate_handle ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoSeek,                   IO_SEEK,                      io_seek ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoChangeStatusFlags,      IO_CHANGE_STATUS_FLAGS,       io_change_status_flags ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoDeleteFile,             IO_DELETE_FILE,               io_delete_file ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoOperationBegin,         IO_OPERATION_BEGIN,           io_operation_begin ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoOperationIssued,        IO_OPERATION_ISSUED,          io_operation_issued ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoOperationTest,          IO_OPERATION_TEST,            io_operation_test ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoOperationComplete,      IO_OPERATION_COMPLETE,        io_operation_complete ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoOperationCancelled,     IO_OPERATION_CANCELLED,       io_operation_cancelled ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoAcquireLock,            IO_ACQUIRE_LOCK,              io_acquire_lock ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoReleaseLock,            IO_RELEASE_LOCK,              io_release_lock ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoTryLock,                IO_TRY_LOCK,                  io_try_lock ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( EnableRecording,          ENABLE_RECORDING,             enable_recording ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( DisableRecording,         DISABLE_RECORDING,            disable_recording ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( WriteMetrics,             WRITE_POST_MORTEM_METRICS,    write_metric ),
    },
    /* SCOREP_SUBSTRATES_RECORDING_DISABLED */
    {
        0, /* all callbacks are NULL, but avoid empty initializer list */
    }
};

const static SCOREP_Substrates_Callback substrate_mgmt_callbacks[ SCOREP_SUBSTRATES_NUM_MGMT_EVENTS ] =
{
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( InitSubstrate,      INIT_SUBSTRATE,       SCOREP_Tracing_Initialize ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( FinalizeSubstrate,  FINALIZE_SUBSTRATE,   SCOREP_Tracing_Finalize ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( OnLocationCreation, ON_LOCATION_CREATION, SCOREP_Tracing_OnLocationCreation ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( OnLocationDeletion, ON_LOCATION_DELETION, SCOREP_Tracing_DeleteLocationData ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( PreUnifySubstrate,  PRE_UNIFY_SUBSTRATE,  SCOREP_Tracing_FinalizeEventWriters ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( WriteData,          WRITE_DATA,           SCOREP_Tracing_Write ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( InitializeMpp,      INITIALIZE_MPP,       SCOREP_Tracing_OnMppInit ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( EnsureGlobalId,     ENSURE_GLOBAL_ID,     SCOREP_Tracing_AssignLocationId ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( AddAttribute,       ADD_ATTRIBUTE,        add_attribute ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( GetRequirement,     GET_REQUIREMENT,      get_requirement ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( DumpManifest,       DUMP_MANIFEST,        dump_manifest ),
};


const SCOREP_Substrates_Callback*
SCOREP_Tracing_GetSubstrateMgmtCallbacks()
{
    return substrate_mgmt_callbacks;
}

const SCOREP_Substrates_Callback*
SCOREP_Tracing_GetSubstrateCallbacks( SCOREP_Substrates_Mode mode )
{
    return substrate_callbacks[ mode ];
}
