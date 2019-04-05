/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012, 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012, 2015,
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
 * @brief   Implmentation of MPP profiling events.
 *
 */

#include <config.h>
#include <SCOREP_Profile_MpiEvents.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Types.h>
#include <SCOREP_Definitions.h>
#include <scorep_profile_mpi_events.h>
#include <scorep_status.h>

static SCOREP_MetricHandle scorep_bytes_send_metric = SCOREP_INVALID_METRIC;
static SCOREP_MetricHandle scorep_bytes_recv_metric = SCOREP_INVALID_METRIC;
static SCOREP_MetricHandle scorep_bytes_get_metric  = SCOREP_INVALID_METRIC;
static SCOREP_MetricHandle scorep_bytes_put_metric  = SCOREP_INVALID_METRIC;

static SCOREP_ParameterHandle scorep_sync_type_param   = SCOREP_INVALID_PARAMETER;
static SCOREP_ParameterHandle scorep_sync_level_param  = SCOREP_INVALID_PARAMETER;
static SCOREP_ParameterHandle scorep_atomic_type_param = SCOREP_INVALID_PARAMETER;

static SCOREP_StringHandle scorep_unknown_string_handle;
static SCOREP_StringHandle scorep_combined_string_handle;

#define SCOREP_RMA_SYNC_TYPE( upper, lower, name ) \
    static SCOREP_StringHandle scorep_rma_sync_type_string_ ## lower;
SCOREP_RMA_SYNC_TYPES
#undef SCOREP_RMA_SYNC_TYPE

#define SCOREP_RMA_SYNC_LEVEL( upper, lower, name, value ) \
    static SCOREP_StringHandle scorep_rma_sync_level_string_ ## lower;
SCOREP_RMA_SYNC_LEVELS
#undef SCOREP_RMA_SYNC_LEVEL

#define SCOREP_RMA_ATOMIC_TYPE( upper, lower, name ) \
    static SCOREP_StringHandle scorep_rma_atomic_type_string_ ## lower;
SCOREP_RMA_ATOMIC_TYPES
#undef SCOREP_RMA_ATOMIC_TYPE


/* *******************************************************************************
 * static helper functions
 * ******************************************************************************/
static SCOREP_StringHandle
get_sync_type_name( SCOREP_RmaSyncType syncType )
{
#define SCOREP_RMA_SYNC_TYPE( upper, lower, name )      \
    case SCOREP_RMA_SYNC_TYPE_ ## upper: \
        return scorep_rma_sync_type_string_ ## lower;

    switch ( syncType )
    {
        SCOREP_RMA_SYNC_TYPES
    }

    return scorep_unknown_string_handle;

    #undef SCOREP_RMA_SYNC_TYPE
}

static SCOREP_StringHandle
get_sync_level_name( SCOREP_RmaSyncLevel syncLevel )
{
    #define SCOREP_RMA_SYNC_LEVEL( upper, lower, name, value )  \
    case SCOREP_RMA_SYNC_LEVEL_ ## upper: \
        return scorep_rma_sync_level_string_ ## lower;

    switch ( syncLevel )
    {
        SCOREP_RMA_SYNC_LEVELS
    }
    #undef SCOREP_RMA_SYNC_LEVEL

    return scorep_combined_string_handle;
}

static SCOREP_StringHandle
get_atomic_type_name( SCOREP_RmaAtomicType atomicType )
{
#define SCOREP_RMA_ATOMIC_TYPE( upper, lower, name )    \
    case SCOREP_RMA_ATOMIC_TYPE_ ## upper: \
        return scorep_rma_atomic_type_string_ ## lower;

    switch ( atomicType )
    {
        SCOREP_RMA_ATOMIC_TYPES
    }

    return scorep_unknown_string_handle;

    #undef SCOREP_RMA_ATOMIC_TYPE
}

/* *******************************************************************************
 * External interface
 * ******************************************************************************/

void
SCOREP_Profile_InitializeMpp( void )
{
    if ( !SCOREP_Status_IsMpp() )
    {
        return;
    }

    scorep_bytes_send_metric =
        SCOREP_Definitions_NewMetric( "bytes_sent",
                                      "Bytes sent",

                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                      SCOREP_INVALID_METRIC );

    scorep_bytes_recv_metric =
        SCOREP_Definitions_NewMetric( "bytes_received",
                                      "Bytes received",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                      SCOREP_INVALID_METRIC );
}

void
SCOREP_Profile_MpiSend( SCOREP_Location*                 location,
                        uint64_t                         timestamp,
                        SCOREP_MpiRank                   destinationRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesSent )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_send_metric, bytesSent );
}

void
SCOREP_Profile_MpiRecv( SCOREP_Location*                 location,
                        uint64_t                         timestamp,
                        SCOREP_MpiRank                   sourceRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesReceived )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_recv_metric, bytesReceived );
}


void
SCOREP_Profile_CollectiveEnd( SCOREP_Location*                 location,
                              uint64_t                         timestamp,
                              SCOREP_InterimCommunicatorHandle communicatorHandle,
                              SCOREP_MpiRank                   rootRank,
                              SCOREP_CollectiveType            collectiveType,
                              uint64_t                         bytesSent,
                              uint64_t                         bytesReceived )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_send_metric, bytesSent );
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_recv_metric, bytesReceived );
}

void
SCOREP_Profile_RmaCollectiveEnd( SCOREP_Location*       location,
                                 uint64_t               timestamp,
                                 SCOREP_CollectiveType  collectiveOp,
                                 SCOREP_RmaSyncLevel    syncLevel,
                                 SCOREP_RmaWindowHandle windowHandle,
                                 uint32_t               root,
                                 uint64_t               bytesSent,
                                 uint64_t               bytesReceived )
{
    SCOREP_Profile_ParameterString( location, 0, scorep_sync_level_param,
                                    get_sync_level_name( syncLevel ) );

    SCOREP_Profile_TriggerInteger( location, scorep_bytes_put_metric, bytesSent );
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_get_metric, bytesReceived );
}

void
SCOREP_Profile_MpiIsend( SCOREP_Location*                 location,
                         uint64_t                         timestamp,
                         SCOREP_MpiRank                   destinationRank,
                         SCOREP_InterimCommunicatorHandle communicatorHandle,
                         uint32_t                         tag,
                         uint64_t                         bytesSent,
                         SCOREP_MpiRequestId              requestId )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_send_metric, bytesSent );
}

void
SCOREP_Profile_MpiIrecv( SCOREP_Location*                 location,
                         uint64_t                         timestamp,
                         SCOREP_MpiRank                   sourceRank,
                         SCOREP_InterimCommunicatorHandle communicatorHandle,
                         uint32_t                         tag,
                         uint64_t                         bytesReceived,
                         SCOREP_MpiRequestId              requestId )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_recv_metric, bytesReceived );
}

void
SCOREP_Profile_RmaSync( SCOREP_Location*       location,
                        uint64_t               timestamp,
                        SCOREP_RmaWindowHandle windowHandle,
                        uint32_t               remote,
                        SCOREP_RmaSyncType     syncType )
{
    SCOREP_Profile_ParameterString( location, 0, scorep_sync_level_param,
                                    get_sync_type_name( syncType ) );
}

void
SCOREP_Profile_RmaGroupSync( SCOREP_Location*       location,
                             uint64_t               timestamp,
                             SCOREP_RmaSyncLevel    syncLevel,
                             SCOREP_RmaWindowHandle windowHandle,
                             SCOREP_GroupHandle     groupHandle )
{
    SCOREP_Profile_ParameterString( location, timestamp, scorep_sync_level_param,
                                    get_sync_level_name( syncLevel ) );
}

void
SCOREP_Profile_RmaPut( SCOREP_Location*       location,
                       uint64_t               timestamp,
                       SCOREP_RmaWindowHandle windowHandle,
                       uint32_t               remote,
                       uint64_t               bytes,
                       uint64_t               matchingId )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_put_metric, bytes );
}

void
SCOREP_Profile_RmaGet( SCOREP_Location*       location,
                       uint64_t               timestamp,
                       SCOREP_RmaWindowHandle windowHandle,
                       uint32_t               remote,
                       uint64_t               bytes,
                       uint64_t               matchingId )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_get_metric, bytes );
}

void
SCOREP_Profile_RmaAtomic( SCOREP_Location*       location,
                          uint64_t               timestamp,
                          SCOREP_RmaWindowHandle windowHandle,
                          uint32_t               remote,
                          SCOREP_RmaAtomicType   type,
                          uint64_t               bytesSent,
                          uint64_t               bytesReceived,
                          uint64_t               matchingId )
{
    SCOREP_Profile_ParameterString( location, timestamp, scorep_atomic_type_param,
                                    get_atomic_type_name( type ) );

    SCOREP_Profile_TriggerInteger( location, scorep_bytes_put_metric, bytesSent );
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_get_metric, bytesReceived );
}

/* *******************************************************************************
 * Internal interface
 * ******************************************************************************/

void
scorep_profile_init_rma( void )
{
#define SCOREP_RMA_SYNC_TYPE( upper, lower, name )                      \
    scorep_rma_sync_type_string_ ## lower = SCOREP_Definitions_NewString( name );
    SCOREP_RMA_SYNC_TYPES
    #undef SCOREP_RMA_SYNC_TYPE

#define SCOREP_RMA_SYNC_LEVEL( upper, lower, name, value )                      \
    scorep_rma_sync_level_string_ ## lower = SCOREP_Definitions_NewString( name );
    SCOREP_RMA_SYNC_LEVELS
    #undef SCOREP_RMA_SYNC_LEVEL

#define SCOREP_RMA_ATOMIC_TYPE( upper, lower, name )                    \
    scorep_rma_atomic_type_string_ ## lower = SCOREP_Definitions_NewString( name );
    SCOREP_RMA_ATOMIC_TYPES
    #undef SCOREP_RMA_ATOMIC_TYPE

    scorep_unknown_string_handle  = SCOREP_Definitions_NewString( "unknown" );
    scorep_combined_string_handle = SCOREP_Definitions_NewString( "combined" );

    scorep_bytes_put_metric =
        SCOREP_Definitions_NewMetric( "bytes_put",
                                      "Bytes sent in one-sided communication",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                      SCOREP_INVALID_METRIC );

    scorep_bytes_get_metric =
        SCOREP_Definitions_NewMetric( "bytes_get",
                                      "Bytes received in one-sided communication",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                      SCOREP_INVALID_METRIC );

    scorep_sync_type_param =
        SCOREP_Definitions_NewParameter( "synchronization type",
                                         SCOREP_PARAMETER_STRING );

    scorep_sync_level_param =
        SCOREP_Definitions_NewParameter( "synchronization level",
                                         SCOREP_PARAMETER_STRING );

    scorep_atomic_type_param =
        SCOREP_Definitions_NewParameter( "atomic type",
                                         SCOREP_PARAMETER_STRING );
}

SCOREP_MetricHandle
scorep_profile_get_bytes_recv_metric_handle( void )
{
    return scorep_bytes_recv_metric;
}

SCOREP_MetricHandle
scorep_profile_get_bytes_send_metric_handle( void )
{
    return scorep_bytes_send_metric;
}
