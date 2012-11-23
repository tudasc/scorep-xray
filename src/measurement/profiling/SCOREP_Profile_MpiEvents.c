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
 * @file        SCOREP_Profile_MpiEvents.c
 * @maintainer  Daniel Lorenz<d.lorenz@fz-juelich.de>
 *
 * @brief   Implmentation of MPI profiling events.
 *
 */

#include <config.h>
#include <SCOREP_Profile_MpiEvents.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Definitions.h>

static SCOREP_MetricHandle scorep_bytes_send_metric = SCOREP_INVALID_METRIC;
static SCOREP_MetricHandle scorep_bytes_recv_metric = SCOREP_INVALID_METRIC;

void
SCOREP_Profile_InitializeMpi( void )
{
    scorep_bytes_send_metric =
        SCOREP_DefineMetric( "bytes_sent",
                             "Bytes sent",
                             SCOREP_METRIC_SOURCE_TYPE_OTHER,
                             SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                             SCOREP_METRIC_VALUE_UINT64,
                             SCOREP_METRIC_BASE_DECIMAL,
                             0,
                             "",
                             SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );

    scorep_bytes_recv_metric =
        SCOREP_DefineMetric( "bytes_received",
                             "Bytes received",
                             SCOREP_METRIC_SOURCE_TYPE_OTHER,
                             SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                             SCOREP_METRIC_VALUE_UINT64,
                             SCOREP_METRIC_BASE_DECIMAL,
                             0,
                             "",
                             SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE );
}

void
SCOREP_Profile_MpiSend( SCOREP_Location*                  location,
                        SCOREP_MpiRank                    destinationRank,
                        SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                        uint32_t                          tag,
                        uint64_t                          bytesSent )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_send_metric, bytesSent );
}

void
SCOREP_Profile_MpiRecv( SCOREP_Location*                  location,
                        SCOREP_MpiRank                    sourceRank,
                        SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                        uint32_t                          tag,
                        uint64_t                          bytesReceived )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_recv_metric, bytesReceived );
}


void
SCOREP_Profile_CollectiveEnd( SCOREP_Location*                  location,
                              SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                              SCOREP_MpiRank                    rootRank,
                              SCOREP_MpiCollectiveType          collectiveType,
                              uint64_t                          bytesSent,
                              uint64_t                          bytesReceived )
{
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_send_metric, bytesSent );
    SCOREP_Profile_TriggerInteger( location, scorep_bytes_recv_metric, bytesReceived );
}
