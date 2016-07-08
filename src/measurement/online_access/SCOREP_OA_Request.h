/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_OA_REQUEST_H
#define SCOREP_OA_REQUEST_H

/**
 * @file
 *
 * @brief Functions for processing and storing of OA measurements requests
 *
 */

#include "SCOREP_Types.h"
#include "SCOREP_Location.h"

typedef enum scorep_oa_metric_source
{
    SCOREP_METRIC_SOURCE_NOT_SUPPORTED,
    SCOREP_METRIC_SOURCE_PAPI,
    SCOREP_METRIC_SOURCE_PERF,
    SCOREP_METRIC_SOURCE_RUSAGE,
    SCOREP_METRIC_SOURCE_PLUGIN,
    SCOREP_METRIC_SOURCE_MPI_PROFILING,
    SCOREP_METRIC_TIMER,
    SCOREP_METRIC_OTHER
} scorep_oa_metric_source;

typedef struct SCOREP_OA_MetricRequest
{
    scorep_oa_metric_source metric_source;
    char*                   metric_name;
    int16_t                 plugin_index;
    int32_t                 oa_index;
} SCOREP_OA_MetricRequest;

/**
 * Initializes requests submission, prepares Score-P measurement reconfiguration
 */
void
scorep_oa_requests_begin( void );

/**
 * Configures Score-P measurement core according to the received measurement requests
 */
void
scorep_oa_requests_end( void );

/**
 * Add a request specified by metric name and metric source to the list to configure Score-P measurement core with
 *
 * @param metricName   a name of the requested metric
 * @param pluginName   a name of the plugin for which the metric is requested
 * @param metricSource a metric source of the request
 */
void
scorep_oa_requests_add_metric_by_name( char*                   metricName,
                                       char*                   pluginName,
                                       scorep_oa_metric_source metricSource );

/**
 * Provides the measurement requests received over OA.
 */
SCOREP_OA_MetricRequest*
SCOREP_OA_RequestGet( uint32_t metricId );

/**
 * Since timer measurements don't get definitions, but still are measured and can be requested over OA
 * this is a special function returning metric request associated with time measurements
 */
SCOREP_OA_MetricRequest*
SCOREP_OA_RequestGetExecutionTime( void );

/**
 * Returns the number of submitted requests
 */
uint32_t
SCOREP_OA_GetNumberOfRequests( void );

/**
 * Frees datastructure allocated for storing requests related information
 */
void
SCOREP_OA_RequestsDismiss( void );

#endif /* SCOREP_OA_REQUEST_H */
