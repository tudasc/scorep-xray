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
 *  @status     alpha
 *
 *  @file       SCOREP_Metric_Source.h
 *
 *  @author     Ronny Tschueter (ronny.tschueter@zih.tu-dresden.de)
 *  @maintainer Ronny Tschueter (ronny.tschueter@zih.tu-dresden.de)
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


/** Array index where data of global 'synchronous strict' metrics is expected */
#define SYNCHRONOUS_STRICT_METRICS_INDEX 0

/** Array index where data of per-process metrics is expected */
#define PER_PROCESS_METRICS_INDEX 1

/** Number of metrics with special handling (at the moment: global
 *  'synchronous strict' and per-process metrics). This value can be
 *  used as first index to store data of per-system-tree-class metrics
 *  in arrays. */
#define NUMBER_OF_RESERVED_METRICS 2

/* *********************************************************************
 * Type definitions
 **********************************************************************/

/** This data structure contains the information about the set of counters,
 *  which are currently measured. The layout of this structure varies
 *  depending on the used hardware counter library. Thus, it is only declared
 *  as an incomplete type at this point. The full definition is then contained
 *  in the actual implementation file. */
typedef struct SCOREP_Metric_EventSet SCOREP_Metric_EventSet;

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


/* *********************************************************************
 * Functions
 **********************************************************************/

/**
 * A metric source needs to provide numerous functions for the metric
 * management system. These ones are collected in this structure for
 * easy handling.
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
    SCOREP_ErrorCode ( * metric_source_register )( void );

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
    SCOREP_Metric_EventSet** ( *metric_source_initialize_location )( SCOREP_Location* );

    /**
     * Frees memory associated to requested metric event set.
     *
     * This function is intended to be used to free event sets of additional metrics.
     * To free event sets of synchronous strict metrics the user should use
     * @a metric_source_finalize_location.
     */
    void ( * metric_source_free_additional_metric_event_set )( SCOREP_Metric_EventSet* );

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
     * @param eventSet  Reference to active set of metrics.
     *
     * @return It returns the overall number of metrics for this source.
     */
    uint32_t ( * metric_source_num_of_metrics )( SCOREP_Metric_EventSet* eventSet );

    /**
     * Returns name of the @ i-th metric in @ eventSet.
     *
     * @param eventSet  Reference to active set of metrics.
     * @param i         Index of the metric.
     *
     * @return It returns the name of requested metric.
     */
    const char* ( *metric_source_name )( SCOREP_Metric_EventSet * eventSet,
                                         uint32_t i );

    /**
     * Returns description of the @ i-th metric in @ eventSet.
     *
     * @param eventSet  Reference to active set of metrics.
     * @param i         Index of the metric.
     *
     * @return It returns the description of requested metric.
     */
    const char* ( *metric_source_description )( SCOREP_Metric_EventSet * eventSet,
                                                uint32_t i );

    /**
     * Returns a string containing a representation of the unit of the
     * @ i-th metric in @ eventSet.
     *
     * @param eventSet  Reference to active set of metrics.
     * @param i         Index of the metric.
     *
     * @return It returns the unit string of requested metric.
     */
    const char* ( *metric_source_unit )( SCOREP_Metric_EventSet * eventSet,
                                         uint32_t i );

    /**
     * Returns the properties of the @ i-th metric in @ eventSet.
     *
     * @param eventSet  Reference to active set of metrics.
     * @param i         Index of the metric.
     *
     * @return It returns the properties of requested metric.
     */
    SCOREP_Metric_Properties ( * metric_source_props )( SCOREP_Metric_EventSet* eventSet,
                                                        uint32_t                i );

    /**
     * Returns the clock rate.
     *
     * @return It returns the clock rate determined by implemented metric source.
     */
    uint64_t ( * metric_source_clock_rate )( void );
} SCOREP_MetricSource;

#endif // SCOREP_METRIC_SOURCE_H
