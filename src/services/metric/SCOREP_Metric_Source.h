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

/**
 * @file SCOREP_Metric.h
 *
 *  This file contains the interface for the hardware counter access. This module can
 *  be build with different implementations supporting different hardware counter
 *  libraries. All of these libraries are accessed through the same interface.
 *
 *  In the first implementation only PAPI and resource usage counters are supported.
 *  Later on, one could build modules for NEC or Solaris support.
 */

#ifndef SCOREP_METRIC_SOURCE_H
#define SCOREP_METRIC_SOURCE_H

#include <stdint.h>

#include "SCOREP_Types.h"
#include "SCOREP_Location.h"

/** @defgroup SCOREP_Metric
 *
 * Metric sources provide the capability of recording additional information as part
 * of the event records. Possible metric sources may be hardware counter or resource
 * usage information provided by the operating system.
 *
 * To request the measurement of certain metric sources, the user is required to
 * set environment variables. See documentation of specific metric sources for
 * more information about relevant environment variables.
 */


/* *********************************************************************
 * Type definitions
 **********************************************************************/

/** This type encodes the properties of a metric. */
typedef struct SCOREP_Metric_Properties
{
    const char*                name;
    const char*                description;
    SCOREP_MetricSourceType    source_type;
    SCOREP_MetricMode          mode;
    SCOREP_MetricValueType     value_type;
    SCOREP_MetricBase          base;
    int64_t                    exponent;
    const char*                unit;
    SCOREP_MetricProfilingType profiling_type;
} SCOREP_Metric_Properties;

/** This struct contains the information about the set of counters, which
 *  are currently measured. The layout of this struct varies depending on the
 *  used hardware counter library. Thus, it is only declared as an incomplete
 *  type at this point. The full definition is then contained in the actual
 *  implementation file.
 */
typedef struct SCOREP_Metric_EventSet SCOREP_Metric_EventSet;


/* *********************************************************************
 * Functions
 **********************************************************************/

/**
 * An metric source needs to provide numerous functions for the metric management system.
 * These are collected in this structure for easy handling.
 */
typedef struct SCOREP_MetricSource
{
    /**
     * The type of the metric source
     */
    SCOREP_MetricSourceType metric_source_type;

    /**
     * Register the metric source.
     *
     * The main purpose is to allow the metric source to register config variables
     * to the system.
     */
    SCOREP_Error_Code ( * metric_source_register )( void );

    /**
     * Initialize the metric source for measurement.
     *
     * At this point all configure variables are set to there current
     * environment values. The metric source can also do calls to the definition
     * interface from this point on.
     */
    uint32_t ( * metric_source_initialize )( void );

    /**
     * Callback to register a location to the metric source.
     */
    SCOREP_Metric_EventSet* ( *metric_source_initialize_location )( void );

    /**
     * Finalizes the per-location data from this metric source.
     */
    void ( * metric_source_finalize_location )( SCOREP_Metric_EventSet* );

    /**
     * Finalizes the metric source for measurement.
     */
    void ( * metric_source_finalize )( void );

    /**
     * De-register the metric_source.
     */
    void ( * metric_source_deregister )( void );

    /**
     * Reads values of counters relative to the time of @ref metric_source_register().
     *
     *  @param eventSet An event set, that contains the definition of the counters
     *                  that should be measured.
     *  @param values   An array, to which the counter values are written.
     */
    void ( * metric_source_read )( SCOREP_Metric_EventSet*,
                                   uint64_t* );

    /**
     * Returns number of metrics.
     *
     * @return It returns the overall number of metrics for this source.
     */
    int32_t ( * metric_source_num_of_metrics )( void );

    /**
     * Returns name of metric i.
     *
     * @param i Index of the metric.
     *
     * @return It returns the name of requested metric.
     */
    const char* ( *metric_source_name )( uint32_t );

    /**
     * Returns description of metric i.
     *
     * @param i Index of the metric.
     *
     * @return It returns the description of requested metric.
     */
    const char* ( *metric_source_description )( uint32_t );

    /**
     * Returns a string containing a representation of the unit of metric i.
     *
     * @param i Index of the counter.
     *
     * @return It returns the unit string of requested metric.
     */
    const char* ( *metric_source_unit )( uint32_t );

    /**
     * Returns the properties of metric i.
     *
     * @param i Index of the metric.
     *
     * @return It returns the properties of requested metric.
     */
    SCOREP_Metric_Properties ( * metric_source_props )( uint32_t );

    /**
     * Returns the clock rate.
     *
     * @return It returns the clock rate determined by implemented metric source.
     */
    uint64_t ( * metric_source_clock_rate )( void );
} SCOREP_MetricSource;

#endif // SCOREP_METRIC_SOURCE_H
