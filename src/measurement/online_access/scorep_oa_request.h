/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

#ifndef SCOREP_OA_REQUEST_H_
#define SCOREP_OA_REQUEST_H_

/**
 * @file        scorep_oa_request.h
 * @maintainer  Yury Oleynik <oleynik@in.tum.de>
 *
 * @brief Functions for processing and storing of OA measurements requests
 *
 */

#include "SCOREP_Types.h"
#include <SCOREP_Location.h>

typedef enum OA_MetricSource
{
    SCOREP_METRIC_SOURCE_NOT_SUPPORTED,
    SCOREP_METRIC_SOURCE_PAPI,
    SCOREP_METRIC_SOURCE_RUSAGE,
    SCOREP_METRIC_SOURCE_MPI_PROFILING,
    SCOREP_METRIC_TIMER,
    SCOREP_METRIC_OTHER
} OA_MetricSource;

typedef enum OA_RequestType
{
    SCOREP_REQUEST_UNDEFINED_SOURCE,
    SCOREP_REQUEST_BY_NAME,
    SCOREP_REQUEST_PERISCOPE_CODE,
} OA_RequestType;

typedef struct MetricRequest
{
    OA_RequestType  request_type;
    OA_MetricSource metric_source;
    int32_t         psc_index;
    char*           metric_name;
    int32_t         oa_index;
} MetricRequest;

/**
 * Initializes requests submittion, prepares Score-P measurement reconfiguration
 */
void
SCOREP_OA_RequestBegin
(
);

/**
 * Configures Score-P measurement core according to the received measurement requests
 */
void
SCOREP_OA_RequestsSubmit
(
    SCOREP_Location* location
);

/**
 * Add a request specified by metric name and metric source to the list to configure Score-P measurement core with
 *
 * @param metric_name a name of the requested metric
 * @param metric_source a metric source of the request
 */
void
SCOREP_OA_RequestsAddMetricByName
(
    char*           metric_name,
    OA_MetricSource metric_source
);

/**
 * Adds a request specified by Periscope code to the request list to configure Score-P measurement core with
 *
 * @param request_string a string containing name of the metric to measure
 */
void
SCOREP_OA_RequestsAddPeriscopeMetric
(
    int metric_code
);

/**
 * Provides the measurement requests received over OA.
 */
MetricRequest*
SCOREP_OA_RequestGet
(
    uint32_t metric_ID
);

/**
 * Since timer measurements don't get definitions, but still are measured and can be requested over OA
 * this is a special function returning metric request associated with time measurements
 */
MetricRequest*
SCOREP_OA_RequestGetExecutionTime
(
);

/**
 * Returns the number of submitted requests
 */
uint32_t
SCOREP_OA_GetNumberOfRequests
(
);

/**
 * Frees datastructure allocated for storing requests related information
 */
void
SCOREP_OA_RequestsDismiss
(
);

#endif /* SCOREP_OA_REQUEST_H_ */
