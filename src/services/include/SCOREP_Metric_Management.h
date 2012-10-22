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

#ifndef SCOREP_METRIC_MANAGEMENT_H
#define SCOREP_METRIC_MANAGEMENT_H

/**
 *  @status     alpha
 *
 *  @file       SCOREP_Metric_Management.h
 *
 *  @author     Ronny Tschueter (ronny.tschueter@zih.tu-dresden.de)
 *  @maintainer Ronny Tschueter (ronny.tschueter@zih.tu-dresden.de)
 */

#include <SCOREP_ErrorCodes.h>
#include <SCOREP_Location.h>

/* *********************************************************************
 * Functions called directly by measurement environment
 **********************************************************************/

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

/** @brief  Returns the sampling set handle to the measurement system.
 */
SCOREP_SamplingSetHandle
SCOREP_Metric_GetSamplingSet( void );

/** @brief  Returns the number of a synchronous metrics.
 *
 *  @return Returns the number of a synchronous metrics.
 */
uint32_t
SCOREP_Metric_GetNumberOfSynchronousStrictMetrics( void );

/** @brief  Returns the handle of a synchronous metric to the measurement system.
 *
 *  @param index                Index of requested metric.
 *
 *  @return Returns the handle of a synchronous metric to the measurement system.
 */
SCOREP_SamplingSetHandle
SCOREP_Metric_GetSynchronousStrictMetricHandle( uint32_t index );

/** @brief  Returns all handles of additional scoped metrics for current location.
 *
 *  @param location             Location data.
 *
 *  @return Returns all handles of additional scoped metrics for current location.
 */
SCOREP_SamplingSetHandle*
SCOREP_Metric_GetAdditionalScopedMetricHandles( SCOREP_Location* location );

/** @brief Writes all metrics of a location in tracing mode.
 *
 *  @param location             Location data.
 *  @param timestamp            Time when event occurred.
 */
void
SCOREP_Metric_WriteToTrace( SCOREP_Location* locationData,
                            uint64_t         timestamp );

/** @brief Get number of additional scorep metrics recorded by @a location.
 *
 *  @param location             Location data.
 */
uint32_t
SCOREP_Metric_GetNumberOfAdditionalScopedMetrics( SCOREP_Location* location );

#endif /* SCOREP_METRIC_MANAGEMENT_H */
