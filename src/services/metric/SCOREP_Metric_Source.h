/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
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

/**
 *
 *  @file
 *
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

#include <scorep/SCOREP_MetricTypes.h>

#include <UTILS_Error.h>


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

/** This data structure contains the information about the set of counters,
 *  which are currently measured. The layout of this structure varies
 *  depending on the used hardware counter library. Thus, it is only declared
 *  as an incomplete type at this point. The full definition is then contained
 *  in the actual implementation file. */
typedef struct SCOREP_Metric_EventSet SCOREP_Metric_EventSet;


/* *********************************************************************
 * Functions
 **********************************************************************/

struct SCOREP_Location;

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
     * At this point all configure variables are set to their current
     * environment values. The metric source can also do calls to the definition
     * interface from this point on.
     */
    uint32_t ( * metric_source_initialize )( void );

    /**
     * Callback to register a location to the metric source.
     */
    SCOREP_Metric_EventSet* ( *metric_source_initialize_location )( struct SCOREP_Location*    location,
                                                                    SCOREP_MetricSynchronicity metricSynchronicity,
                                                                    SCOREP_MetricPer metricType );

    /**
     * Optional synchronization callback.
     */
    void ( * metric_source_synchronize )( SCOREP_MetricSynchronizationMode syncMode );

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
     * There are three different functions used for reading metric values:
     * @ metric_source_strictly_synchronous_read, @ metric_source_synchronous_read,
     * and @ metric_source_asynchronous_read.
     *
     * @ metric_source_strictly_synchronous_read is used to read values
     * of strictly synchronous metrics. Strictly synchronous metrics are
     * recorded at every function enter/exit event and have to provide
     * a metric value. This function gets an array as argument to which
     * values are written. Each metric of the requested event set
     * @ eventSet must write exactely one value at its corresponding
     * position within the value array (e.g. metric i writes its value
     * in @ values[i]).
     *
     * @ metric_source_synchronous_read is used tio read values of
     * synchronous metrics (note the missing 'strictly'). In contrast
     * to @ metric_source_strictly_synchronous_read it is allowed to
     * skip writting of specific metric values. Therefore, @ is_updated
     * is used to indicate whether a value was written for a metric or
     * not. For example, if a new value of metric i should be written,
     * its value is stored in @ values[i] and @ is_updated[i] is set to
     * true. Otherwise @ is_updated[i] is set to false and @ value[i] do
     * not have to be touched.
     *
     * @ metric_source_asynchronous_read is used to read values
     * of asynchronous metrics. Asynchronous metrics can be measured at
     * function enter/leave events but also at arbitrary points in time.
     * Each time a asynchronous metric is called to write its values,
     * the has to provide an arbitrary count of timestamp-value-pairs.
     * For example, mettic i of event set @ eventSet write its
     * timestamp-values-pairs in @ timevalue_pointer[i]. In addition,
     * metric i has to store the number of written timestamp-values-
     * pairs in @ num-pairs[i].
     */

    /**
     * Reads values of counters relative to the time of @ref metric_source_register().
     * For all metrics values must be written!
     *
     *  @param eventSet An event set, that contains the definition of the counters
     *                  that should be measured.
     *  @param values   An array, to which the counter values are written.
     */
    void ( * metric_source_strictly_synchronous_read )( SCOREP_Metric_EventSet* eventSet,
                                                        uint64_t*               values );

    /**
     * Reads values of counters relative to the time of @ref metric_source_register().
     * It is allowed that specific metrics of @eventSet are not updated (no values are
     * written).
     *
     *  @param      eventSet     An event set, that contains the definition of the counters
     *                           that should be measured.
     *  @param[out] values       Reference to array that will be filled with values from
     *                           active metrics.
     *  @param[out] isUpdated    An array which indicates whether a new value of a specfic
     *                           metric was written (@ is_updated[i] == true ) or not
     *                           (@ is_updated[i] == false ).
     *  @param      forceUpdate  Update of all metric value in this event set is enforced.
     */
    void ( * metric_source_synchronous_read )( SCOREP_Metric_EventSet* eventSet,
                                               uint64_t*               values,
                                               bool*                   isUpdated,
                                               bool                    forceUpdate );

    /**
     * Read values of counters relative to the time of @ref metric_source_register() asynchronously.
     *
     *  @param      eventSet            An event set, that contains the definition of the counters
     *                                  that should be measured.
     *  @param[out] timevaluePointer    An array, to which the counter values are written.
     *  @param[out] numPairs            Number of pairs (timestamp + value) written for each
     *                                  individual metric.
     *  @param      forceUpdate         Update of all metric value in this event set is enforced.
     */
    void ( * metric_source_asynchronous_read )( SCOREP_Metric_EventSet*      eventSet,
                                                SCOREP_MetricTimeValuePair** timevaluePointer,
                                                uint64_t**                   numPairs,
                                                bool                         forceUpdate );

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
} SCOREP_MetricSource;

#endif /* SCOREP_METRIC_SOURCE_H */
