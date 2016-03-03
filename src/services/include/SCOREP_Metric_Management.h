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
 * Copyright (c) 2009-2013, 2015,
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
 *  @file       SCOREP_Metric_Management.h
 *
 */

#include <SCOREP_ErrorCodes.h>
#include <SCOREP_Location.h>

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

/** @brief Writes all metrics of a location in tracing mode.
 *
 *  @param location             Location data.
 *  @param timestamp            Time when event occurred.
 */
void
SCOREP_Metric_WriteToTrace( SCOREP_Location* location,
                            uint64_t         timestamp );

/** @brief Writes metrics (except strictly synchronous ones which are
 *         handled by enter/leave events) of a location in profiling mode.
 *
 *  @param location             Location data.
 */
void
SCOREP_Metric_WriteToProfile( SCOREP_Location* location );

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

#endif /* SCOREP_METRIC_MANAGEMENT_H */
