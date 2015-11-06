/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
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
 * @brief Event consumption functions for tracing.
 */


#include <config.h>

#include "SCOREP_Tracing_Events.h"
#include "SCOREP_Tracing.h"

#include <stdlib.h>
#include <stdio.h>


#include <UTILS_Error.h>
#include <UTILS_Debug.h>


#include <otf2/otf2.h>


#include <SCOREP_Types.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Properties.h>

#include <scorep_status.h>

#include "scorep_tracing_internal.h"
#include "scorep_tracing_types.h"
#include <scorep_events_common.h>

#include <SCOREP_Substrates_Management.h>
#include <SCOREP_Metric_Management.h>


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


void
SCOREP_Tracing_Metric( SCOREP_Location*         location,
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
enter( SCOREP_Location*    location,
       uint64_t            timestamp,
       SCOREP_RegionHandle regionHandle,
       uint64_t*           metricValues )
{
    SCOREP_Metric_WriteToTrace( location, timestamp );

    SCOREP_TracingData* tracing_data       = scorep_tracing_get_trace_data( location );
    OTF2_EvtWriter*     evt_writer         = tracing_data->otf_writer;
    OTF2_AttributeList* otf_attribute_list = tracing_data->otf_attribute_list;

    OTF2_EvtWriter_Enter( evt_writer,
                          ( OTF2_AttributeList* )otf_attribute_list,
                          timestamp,
                          SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
}


static void
leave( SCOREP_Location*    location,
       uint64_t            timestamp,
       SCOREP_RegionHandle regionHandle,
       uint64_t*           metricValues )
{
    SCOREP_TracingData* tracing_data       = scorep_tracing_get_trace_data( location );
    OTF2_EvtWriter*     evt_writer         = tracing_data->otf_writer;
    OTF2_AttributeList* otf_attribute_list = tracing_data->otf_attribute_list;

    if ( metricValues )
    {
        /* @todo: Writing metrics to trace file will be improved in the near future */

        SCOREP_Metric_WriteToTrace( location,
                                    timestamp );
    }

    OTF2_EvtWriter_Leave( evt_writer,
                          ( OTF2_AttributeList* )otf_attribute_list,
                          timestamp,
                          SCOREP_LOCAL_HANDLE_TO_ID( regionHandle, Region ) );
}


static void
add_attribute( SCOREP_Location*       location,
               SCOREP_AttributeHandle attributeHandle,
               void*                  value )
{
    OTF2_AttributeList* otf_attribute_list =
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

        case SCOREP_ATTRIBUTE_TYPE_INTERIM_RMA_WINDOW:
            otf_val.rmaWinRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_InterimRmaWindowHandle* )value ), InterimRmaWindow );
            otf_type = OTF2_TYPE_RMA_WIN;
            break;

        case SCOREP_ATTRIBUTE_TYPE_STRING:
            otf_val.stringRef = SCOREP_LOCAL_HANDLE_TO_ID(
                *( ( SCOREP_StringHandle* )value ), String );
            otf_type = OTF2_TYPE_STRING;
            break;

        default:
            UTILS_BUG( "Invalid attribute type: %u", attrType );
    }

    OTF2_AttributeList_AddAttribute( otf_attribute_list,
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
                    SCOREP_MpiCollectiveType         collectiveType,
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
rma_win_create( SCOREP_Location*              location,
                uint64_t                      timestamp,
                SCOREP_InterimRmaWindowHandle windowHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaWinCreate( evt_writer,
                                 NULL,
                                 timestamp,
                                 SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_win_destroy( SCOREP_Location*              location,
                 uint64_t                      timestamp,
                 SCOREP_InterimRmaWindowHandle windowHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaWinDestroy( evt_writer,
                                  NULL,
                                  timestamp,
                                  SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ) );

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
rma_collective_end( SCOREP_Location*              location,
                    uint64_t                      timestamp,
                    SCOREP_MpiCollectiveType      collectiveOp,
                    SCOREP_RmaSyncLevel           syncLevel,
                    SCOREP_InterimRmaWindowHandle windowHandle,
                    uint32_t                      root,
                    uint64_t                      bytesSent,
                    uint64_t                      bytesReceived )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
rma_group_sync( SCOREP_Location*              location,
                uint64_t                      timestamp,
                SCOREP_RmaSyncLevel           syncLevel,
                SCOREP_InterimRmaWindowHandle windowHandle,
                SCOREP_GroupHandle            groupHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
rma_request_lock( SCOREP_Location*              location,
                  uint64_t                      timestamp,
                  SCOREP_InterimRmaWindowHandle windowHandle,
                  uint32_t                      remote,
                  uint64_t                      lockId,
                  SCOREP_LockType               lockType )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
rma_acquire_lock( SCOREP_Location*              location,
                  uint64_t                      timestamp,
                  SCOREP_InterimRmaWindowHandle windowHandle,
                  uint32_t                      remote,
                  uint64_t                      lockId,
                  SCOREP_LockType               lockType )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
rma_try_lock( SCOREP_Location*              location,
              uint64_t                      timestamp,
              SCOREP_InterimRmaWindowHandle windowHandle,
              uint32_t                      remote,
              uint64_t                      lockId,
              SCOREP_LockType               lockType )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
rma_release_lock( SCOREP_Location*              location,
                  uint64_t                      timestamp,
                  SCOREP_InterimRmaWindowHandle windowHandle,
                  uint32_t                      remote,
                  uint64_t                      lockId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
rma_sync( SCOREP_Location*              location,
          uint64_t                      timestamp,
          SCOREP_InterimRmaWindowHandle windowHandle,
          uint32_t                      remote,
          SCOREP_RmaSyncType            syncType )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
rma_wait_change( SCOREP_Location*              location,
                 uint64_t                      timestamp,
                 SCOREP_InterimRmaWindowHandle windowHandle )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaWaitChange( evt_writer,
                                  NULL,
                                  timestamp,
                                  SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ) );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_put( SCOREP_Location*              location,
         uint64_t                      timestamp,
         SCOREP_InterimRmaWindowHandle windowHandle,
         uint32_t                      remote,
         uint64_t                      bytes,
         uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
rma_get( SCOREP_Location*              location,
         uint64_t                      timestamp,
         SCOREP_InterimRmaWindowHandle windowHandle,
         uint32_t                      remote,
         uint64_t                      bytes,
         uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
rma_atomic( SCOREP_Location*              location,
            uint64_t                      timestamp,
            SCOREP_InterimRmaWindowHandle windowHandle,
            uint32_t                      remote,
            SCOREP_RmaAtomicType          type,
            uint64_t                      bytesSent,
            uint64_t                      bytesReceived,
            uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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


static void
rma_op_complete_blocking( SCOREP_Location*              location,
                          uint64_t                      timestamp,
                          SCOREP_InterimRmaWindowHandle windowHandle,
                          uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaOpCompleteBlocking( evt_writer,
                                          NULL,
                                          timestamp,
                                          SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                                          matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_op_complete_non_blocking( SCOREP_Location*              location,
                              uint64_t                      timestamp,
                              SCOREP_InterimRmaWindowHandle windowHandle,
                              uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaOpCompleteNonBlocking( evt_writer,
                                             NULL,
                                             timestamp,
                                             SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                                             matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_op_test( SCOREP_Location*              location,
             uint64_t                      timestamp,
             SCOREP_InterimRmaWindowHandle windowHandle,
             uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

    OTF2_EvtWriter_RmaOpTest( evt_writer,
                              NULL,
                              timestamp,
                              SCOREP_LOCAL_HANDLE_TO_ID( windowHandle, InterimRmaWindow ),
                              matchingId );

    /*
     * scorep_rewind_set_affected_paradigm ?
     */
}


static void
rma_op_complete_remote( SCOREP_Location*              location,
                        uint64_t                      timestamp,
                        SCOREP_InterimRmaWindowHandle windowHandle,
                        uint64_t                      matchingId )
{
    OTF2_EvtWriter* evt_writer = scorep_tracing_get_trace_data( location )->otf_writer;

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
set_rewind_affected_thread_paradigm( SCOREP_Location*    location,
                                     SCOREP_ParadigmType paradigm )
{
    switch ( paradigm )
    {
#define case_break( thread_model, rewind_paradigm ) \
    case SCOREP_PARADIGM_ ## thread_model: \
        scorep_rewind_set_affected_paradigm( location, SCOREP_REWIND_PARADIGM_ ## rewind_paradigm ); \
        break;

        case_break( OPENMP, THREAD_FORK_JOIN );
        case_break( PTHREAD, THREAD_CREATE_WAIT );

#undef case_break
        default:
            UTILS_BUG( "Invalid threading model: %u", paradigm );
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
                               scorep_tracing_get_otf2_paradigm( paradigm ),
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
                               scorep_tracing_get_otf2_paradigm( paradigm ) );

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
                                 createSequenceCount );

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
                               createSequenceCount );

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
                                createSequenceCount );

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
                              createSequenceCount );

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
                                      scorep_tracing_get_otf2_paradigm( paradigm ),
                                      lockId,
                                      acquisitionOrder );

    set_rewind_affected_thread_paradigm( location, paradigm );
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
                                      scorep_tracing_get_otf2_paradigm( paradigm ),
                                      lockId,
                                      acquisitionOrder );

    set_rewind_affected_thread_paradigm( location, paradigm );
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
                       SCOREP_MetricHandle      metricHandle,
                       SCOREP_SamplingSetHandle counterHandle,
                       int64_t                  value )
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


static void
trigger_counter_uint64( SCOREP_Location*         location,
                        uint64_t                 timestamp,
                        SCOREP_MetricHandle      metricHandle,
                        SCOREP_SamplingSetHandle counterHandle,
                        uint64_t                 value )
{
    SCOREP_Tracing_Metric( location,
                           timestamp,
                           counterHandle,
                           &value );
}


static void
trigger_counter_double( SCOREP_Location*         location,
                        uint64_t                 timestamp,
                        SCOREP_MetricHandle      metricHandle,
                        SCOREP_SamplingSetHandle counterHandle,
                        uint64_t                 value )
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
     * a leck in the needed memory. */
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

const static SCOREP_Substrates_Callback substrate_callbacks[ SCOREP_SUBSTRATES_NUM_MODES ][ SCOREP_SUBSTRATES_NUM_EVENTS ] =
{
    {   /* SCOREP_SUBSTRATES_RECORDING_ENABLED */
        [ SCOREP_EVENT_INIT_SUBSTRATE ]               = ( SCOREP_Substrates_Callback )SCOREP_Tracing_Initialize,
        [ SCOREP_EVENT_FINALIZE_SUBSTRATE ]           = ( SCOREP_Substrates_Callback )SCOREP_Tracing_Finalize,
        [ SCOREP_EVENT_ENABLE_RECORDING ]             = ( SCOREP_Substrates_Callback )enable_recording,
        [ SCOREP_EVENT_DISABLE_RECORDING ]            = ( SCOREP_Substrates_Callback )disable_recording,
        [ SCOREP_EVENT_ON_LOCATION_CREATION ]         = ( SCOREP_Substrates_Callback )SCOREP_Tracing_OnLocationCreation,
        [ SCOREP_EVENT_ON_LOCATION_DELETION ]         = ( SCOREP_Substrates_Callback )SCOREP_Tracing_DeleteLocationData,
        [ SCOREP_EVENT_PRE_UNIFY_SUBSTRATE ]          = ( SCOREP_Substrates_Callback )SCOREP_Tracing_FinalizeEventWriters,
        [ SCOREP_EVENT_WRITE_DATA ]                   = ( SCOREP_Substrates_Callback )SCOREP_Tracing_Write,
        [ SCOREP_EVENT_INITIALIZE_MPP ]               = ( SCOREP_Substrates_Callback )SCOREP_Tracing_OnMppInit,
        [ SCOREP_EVENT_ENSURE_GLOBAL_ID ]             = ( SCOREP_Substrates_Callback )SCOREP_Tracing_AssignLocationId,
        [ SCOREP_EVENT_ENTER_REGION ]                 = ( SCOREP_Substrates_Callback )enter,
        [ SCOREP_EVENT_EXIT_REGION ]                  = ( SCOREP_Substrates_Callback )leave,
        [ SCOREP_EVENT_ENTER_REWIND_REGION ]          = ( SCOREP_Substrates_Callback )store_rewind_point,
        [ SCOREP_EVENT_EXIT_REWIND_REGION ]           = ( SCOREP_Substrates_Callback )exit_rewind_point,
        [ SCOREP_EVENT_MPI_SEND ]                     = ( SCOREP_Substrates_Callback )mpi_send,
        [ SCOREP_EVENT_MPI_RECV ]                     = ( SCOREP_Substrates_Callback )mpi_recv,
        [ SCOREP_EVENT_MPI_COLLECTIVE_BEGIN ]         = ( SCOREP_Substrates_Callback )mpi_collective_begin,
        [ SCOREP_EVENT_MPI_COLLECTIVE_END ]           = ( SCOREP_Substrates_Callback )mpi_collective_end,
        [ SCOREP_EVENT_MPI_ISEND ]                    = ( SCOREP_Substrates_Callback )mpi_isend_complete,
        [ SCOREP_EVENT_MPI_IRECV ]                    = ( SCOREP_Substrates_Callback )mpi_irecv_request,
        [ SCOREP_EVENT_MPI_REQUEST_TESTED ]           = ( SCOREP_Substrates_Callback )mpi_request_tested,
        [ SCOREP_EVENT_MPI_REQUEST_CANCELLED ]        = ( SCOREP_Substrates_Callback )mpi_request_cancelled,
        [ SCOREP_EVENT_MPI_ISEND ]                    = ( SCOREP_Substrates_Callback )mpi_isend,
        [ SCOREP_EVENT_MPI_IRECV ]                    = ( SCOREP_Substrates_Callback )mpi_irecv,
        [ SCOREP_EVENT_RMA_WIN_CREATE ]               = ( SCOREP_Substrates_Callback )rma_win_create,
        [ SCOREP_EVENT_RMA_WIN_DESTROY ]              = ( SCOREP_Substrates_Callback )rma_win_destroy,
        [ SCOREP_EVENT_RMA_COLLECTIVE_BEGIN ]         = ( SCOREP_Substrates_Callback )rma_collective_begin,
        [ SCOREP_EVENT_RMA_COLLECTIVE_END ]           = ( SCOREP_Substrates_Callback )rma_collective_end,
        [ SCOREP_EVENT_RMA_TRY_LOCK ]                 = ( SCOREP_Substrates_Callback )rma_try_lock,
        [ SCOREP_EVENT_RMA_ACQUIRE_LOCK ]             = ( SCOREP_Substrates_Callback )rma_acquire_lock,
        [ SCOREP_EVENT_RMA_REQUEST_LOCK ]             = ( SCOREP_Substrates_Callback )rma_request_lock,
        [ SCOREP_EVENT_RMA_RELEASE_LOCK ]             = ( SCOREP_Substrates_Callback )rma_release_lock,
        [ SCOREP_EVENT_RMA_SYNC ]                     = ( SCOREP_Substrates_Callback )rma_sync,
        [ SCOREP_EVENT_RMA_GROUP_SYNC ]               = ( SCOREP_Substrates_Callback )rma_group_sync,
        [ SCOREP_EVENT_RMA_PUT ]                      = ( SCOREP_Substrates_Callback )rma_put,
        [ SCOREP_EVENT_RMA_GET ]                      = ( SCOREP_Substrates_Callback )rma_get,
        [ SCOREP_EVENT_RMA_ATOMIC ]                   = ( SCOREP_Substrates_Callback )rma_atomic,
        [ SCOREP_EVENT_RMA_WAIT_CHANGE ]              = ( SCOREP_Substrates_Callback )rma_wait_change,
        [ SCOREP_EVENT_RMA_OP_COMPLETE_BLOCKING ]     = ( SCOREP_Substrates_Callback )rma_op_complete_blocking,
        [ SCOREP_EVENT_RMA_OP_COMPLETE_NON_BLOCKING ] = ( SCOREP_Substrates_Callback )rma_op_complete_non_blocking,
        [ SCOREP_EVENT_RMA_OP_TEST ]                  = ( SCOREP_Substrates_Callback )rma_op_test,
        [ SCOREP_EVENT_RMA_OP_COMPLETE_REMOTE ]       = ( SCOREP_Substrates_Callback )rma_op_complete_remote,
        [ SCOREP_EVENT_THREAD_ACQUIRE_LOCK ]          = ( SCOREP_Substrates_Callback )thread_acquire_lock,
        [ SCOREP_EVENT_THREAD_RELEASE_LOCK ]          = ( SCOREP_Substrates_Callback )thread_release_lock,
        [ SCOREP_EVENT_TRIGGER_COUNTER_INT64 ]        = ( SCOREP_Substrates_Callback )trigger_counter_int64,
        [ SCOREP_EVENT_TRIGGER_COUNTER_UINT64 ]       = ( SCOREP_Substrates_Callback )trigger_counter_uint64,
        [ SCOREP_EVENT_TRIGGER_COUNTER_DOUBLE ]       = ( SCOREP_Substrates_Callback )trigger_counter_double,
        [ SCOREP_EVENT_TRIGGER_PARAMETER_INT64 ]      = ( SCOREP_Substrates_Callback )parameter_int64,
        [ SCOREP_EVENT_TRIGGER_PARAMETER_UINT64 ]     = ( SCOREP_Substrates_Callback )parameter_uint64,
        [ SCOREP_EVENT_TRIGGER_PARAMETER_STRING ]     = ( SCOREP_Substrates_Callback )parameter_string,
        [ SCOREP_EVENT_THREAD_FORK_JOIN_FORK ]        = ( SCOREP_Substrates_Callback )thread_fork,
        [ SCOREP_EVENT_THREAD_FORK_JOIN_JOIN ]        = ( SCOREP_Substrates_Callback )thread_join,
        [ SCOREP_EVENT_THREAD_FORK_JOIN_TEAM_BEGIN ]  = ( SCOREP_Substrates_Callback )thread_team_begin,
        [ SCOREP_EVENT_THREAD_FORK_JOIN_TEAM_END ]    = ( SCOREP_Substrates_Callback )thread_team_end,
        [ SCOREP_EVENT_THREAD_FORK_JOIN_TASK_CREATE ] = ( SCOREP_Substrates_Callback )thread_task_create,
        [ SCOREP_EVENT_THREAD_FORK_JOIN_TASK_SWITCH ] = ( SCOREP_Substrates_Callback )thread_task_switch,
        [ SCOREP_EVENT_THREAD_FORK_JOIN_TASK_BEGIN ]  = ( SCOREP_Substrates_Callback )thread_task_begin,
        [ SCOREP_EVENT_THREAD_FORK_JOIN_TASK_END ]    = ( SCOREP_Substrates_Callback )thread_task_complete,
        [ SCOREP_EVENT_THREAD_CREATE_WAIT_CREATE ]    = ( SCOREP_Substrates_Callback )thread_create,
        [ SCOREP_EVENT_THREAD_CREATE_WAIT_WAIT ]      = ( SCOREP_Substrates_Callback )thread_wait,
        [ SCOREP_EVENT_THREAD_CREATE_WAIT_BEGIN ]     = ( SCOREP_Substrates_Callback )thread_begin,
        [ SCOREP_EVENT_THREAD_CREATE_WAIT_END ]       = ( SCOREP_Substrates_Callback )thread_end,
        [ SCOREP_EVENT_ADD_ATTRIBUTE ]                = ( SCOREP_Substrates_Callback )add_attribute
    },
    {         /* SCOREP_SUBSTRATES_RECORDING_DISABLED */
        [ SCOREP_EVENT_INIT_SUBSTRATE ]       = ( SCOREP_Substrates_Callback )SCOREP_Tracing_Initialize,
        [ SCOREP_EVENT_FINALIZE_SUBSTRATE ]   = ( SCOREP_Substrates_Callback )SCOREP_Tracing_Finalize,
        [ SCOREP_EVENT_ON_LOCATION_CREATION ] = ( SCOREP_Substrates_Callback )SCOREP_Tracing_OnLocationCreation,
        [ SCOREP_EVENT_ON_LOCATION_DELETION ] = ( SCOREP_Substrates_Callback )SCOREP_Tracing_DeleteLocationData,
        [ SCOREP_EVENT_PRE_UNIFY_SUBSTRATE ]  = ( SCOREP_Substrates_Callback )SCOREP_Tracing_FinalizeEventWriters,
        [ SCOREP_EVENT_WRITE_DATA ]           = ( SCOREP_Substrates_Callback )SCOREP_Tracing_Write,
        [ SCOREP_EVENT_INITIALIZE_MPP ]       = ( SCOREP_Substrates_Callback )SCOREP_Tracing_OnMppInit,
        [ SCOREP_EVENT_ENSURE_GLOBAL_ID ]     = ( SCOREP_Substrates_Callback )SCOREP_Tracing_AssignLocationId
    }
};


const SCOREP_Substrates_Callback*
SCOREP_Tracing_GetSubstrateCallbacks( SCOREP_Substrates_Mode mode )
{
    return substrate_callbacks[ mode ];
}
