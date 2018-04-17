/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015, 2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015, 2017-2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_METRIC_MANAGEMENT_H
#define SCOREP_METRIC_MANAGEMENT_H

/**
 *
 *  @file
 *
 */

#include <SCOREP_ErrorCodes.h>
#include <SCOREP_Location.h>
#include <SCOREP_Definitions.h>

/* *********************************************************************
 * Functions called directly by measurement environment
 **********************************************************************/

/* General metric handling */

/** @brief  Get recent values of all metrics.
 *
 *  @param location     Handle of location for which metrics will be recorded.
 *
 *  @return Returns pointer to value array filled with recent metric values,
 *          or NULL if we don't have metrics to read from.
 */
uint64_t*
SCOREP_Metric_Read( SCOREP_Location* location );

/** @brief  Reinitialize metric management. This functionality is used by
 *          Score-P Online Access to change recorded metrics between
 *          separate phases of program execution.
 */
SCOREP_ErrorCode
SCOREP_Metric_Reinitialize( void );

/* Functions with respect to strictly synchronous metrics */

/** @brief  Returns the sampling set handle to the measurement system.
 *
 *  @return Returns the sampling set handle to the measurement system.
 */
SCOREP_SamplingSetHandle
SCOREP_Metric_GetStrictlySynchronousSamplingSet( void );

/** @brief  Returns the number of a synchronous metrics.
 *
 *  @return Returns the number of a synchronous metrics.
 */
uint32_t
SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics( void );

/** @brief  Returns the handle of a synchronous metric to the measurement system.
 *
 *  @param index                Index of requested metric.
 *
 *  @return Returns the handle of a synchronous metric to the measurement system.
 */
SCOREP_MetricHandle
SCOREP_Metric_GetStrictlySynchronousMetricHandle( uint32_t index );


/*
 * A callback to be provided by substrates to
 * SCOREP_Metric_WriteStrictlySynchronousMetrics(),
 * SCOREP_Metric_WriteSynchronousMetrics(), and
 * SCOREP_Metric_WriteAsynchronousMetrics().
 * This MUST be the same as SCOREP_Substrates_WriteMetricsCb()
 * @param location location where metric was read
 * @param timestamp timestamp when metric was read
 * @param samplingSet the sampling set corresponding to @metricValues
 * @param metricValues the current values of the metrics. The length of metricValues can be gathered by checking the number of metrics in the samplingSet
 */
typedef void (* WriteMetricsCb)( SCOREP_Location*         location,
                                 uint64_t                 timestamp,
                                 SCOREP_SamplingSetHandle samplingSet,
                                 const uint64_t*          metricValues );

/** @brief Triggers WriteMetricsCb @a cb once with the strictly synchronous
 * metrics obtained in the last call to SCOREP_Metrics_Read() on @a location.
 *
 * This function shall only be called while processing enters, exits, and samples
 * @param location The location the metrics are provided from, reported back via cb
 * @param timestamp A corresponding timestamp, reported back via cb
 * @param cb a callback that processes the metrics
 */

void
SCOREP_Metric_WriteStrictlySynchronousMetrics( SCOREP_Location* location,
                                               uint64_t         timestamp,
                                               WriteMetricsCb   cb );

/** @brief Triggers WriteMetricsCb @a cb once per updated synchronous
 * metric where the synchronous metrics were obtained in the last call to
 * SCOREP_Metrics_Read() on @a location.
 *
 * This function shall only be called while processing enters, exits, and samples.
 * The callback might be called multiple times if multiple sampling sets are present.
 * @param location The location the metrics are provided from, reported back via cb
 * @param timestamp A corresponding timestamp, reported back via cb
 * @param cb a callback that processes the metrics
 */
void
SCOREP_Metric_WriteSynchronousMetrics( SCOREP_Location* location,
                                       uint64_t         timestamp,
                                       WriteMetricsCb   cb );

/** @brief Triggers WriteMetricsCb @a cb once per available asynchronous
 * metric where the asynchronous metrics were obtained in the last call to
 * SCOREP_Metrics_Read() on @a location.
 *
 * This function shall only be called while processing enters, exits, and samples.
 * The callback might be called multiple times if multiple sampling sets are present.
 *
 * @param location The location the metrics are provided from, reported back via cb
 * @param cb a callback that processes the metrics
 */
void
SCOREP_Metric_WriteAsynchronousMetrics( SCOREP_Location* location,
                                        WriteMetricsCb   cb );

#endif /* SCOREP_METRIC_MANAGEMENT_H */
