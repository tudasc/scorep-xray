/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
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
 * @brief   Implmentation of MPI profiling events.
 *
 */

#include <config.h>
#include <SCOREP_Profile_MpiEvents.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Definitions.h>
#include <scorep_profile_mpi_events.h>

static SCOREP_MetricHandle scorep_bytes_send_metric = SCOREP_INVALID_METRIC;
static SCOREP_MetricHandle scorep_bytes_recv_metric = SCOREP_INVALID_METRIC;

void
SCOREP_Profile_InitializeMpi( void )
{
    scorep_bytes_send_metric =
        SCOREP_Definitions_NewMetric( "bytes_sent",
                                      "Bytes sent",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );

    scorep_bytes_recv_metric =
        SCOREP_Definitions_NewMetric( "bytes_received",
                                      "Bytes received",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );
}

void
SCOREP_Profile_MpiSend( SCOREP_Location*                 location,
                        SCOREP_MpiRank                   destinationRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesSent )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_send_metric, bytesSent );
}

void
SCOREP_Profile_MpiRecv( SCOREP_Location*                 location,
                        SCOREP_MpiRank                   sourceRank,
                        SCOREP_InterimCommunicatorHandle communicatorHandle,
                        uint32_t                         tag,
                        uint64_t                         bytesReceived )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_recv_metric, bytesReceived );
}


void
SCOREP_Profile_CollectiveEnd( SCOREP_Location*                 location,
                              SCOREP_InterimCommunicatorHandle communicatorHandle,
                              SCOREP_MpiRank                   rootRank,
                              SCOREP_MpiCollectiveType         collectiveType,
                              uint64_t                         bytesSent,
                              uint64_t                         bytesReceived )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_send_metric, bytesSent );
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_recv_metric, bytesReceived );
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
