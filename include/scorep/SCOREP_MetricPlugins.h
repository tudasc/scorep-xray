/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file       SCOREP_MetricPlugins.h
 *
 * @brief Metric plugin definitions.
 */


#ifndef SCOREP_METRIC_PLUGINS_H
#define SCOREP_METRIC_PLUGINS_H

/**
 * The developer of a metric plugin should provide a README file which
 * explains how to compile, install and use the plugin. In particular,
 * all supported metrics should be described in the README file.
 *
 * Each metric plugin has to include <tt>SCOREP_MetricPlugin.h</tt>
 * and implement 'get_info' function. Therefore, use the
 * SCOREP_METRIC_PLUGIN_ENTRY macro and provide the name of the plugin
 * library as the argument. For example, the metric plugin libexample.so
 * should use SCOREP_METRIC_PLUGIN_ENTRY( example ). The 'get_info'
 * function returns a SCOREP_Metric_Plugin_Info data structure which
 * contains information about the plugin and its metrics.
 *
 * Mandatory functions
 *
 * @ initialize
 * This function is called once per process event on those where no
 * metric values of the plugin will be recorded. It should check that
 * all requirements are met (e.g., are special libraries needed and
 * available, has the user appropriate rights to count implemented
 * metrics). If all requirements are met, data structures used by the
 * plugin can be initialized within this function.
 *
 * @ get_event_info
 * This function is called once per process. It is called after
 * initialization of the metric plugin and even on processes where none
 * of the requested metrics will be recorded. The function maps the
 * requested event string to specific metrics. For example, the metric
 * plugin can provide the functionality of wildcards and return multiple
 * metrics for a single event string. The returned list of metrics
 * should end with an element whose name is <tt>NULL</tt>. For each
 * metric the fields name, mode, value type, base, and exponent has to
 * be specified. The unit of a metric is optional and might be
 * <tt>NULL</tt>.
 *
 * @ add_counter
 * Depending on @ run_per this function is called per thread, per
 * process, per host, or only once. The function sets up the requested
 * metric and returns an ID which is used to address this metric later
 * on.
 *
 * @ finalize
 * This function is called once per process event on those where no
 * metric values of the plugin will be recorded. It finalizes the
 * internal infrastructure of a metric plugin (e.g., free allocated
 * resources).
 *
 * Mandatory variables
 *
 * @ run_per
 * Defines how often a metric should be measured.
 * Valid value are:
 * <ul>
 *  <li>
 *      <b>SCOREP_METRIC_PER_THREAD</b>:<br>
 *      Metric values are recorded for all threads of all processes
 *  </li>
 *  <li>
 *      <b>SCOREP_METRIC_PER_PROCESS</b>:<br>
 *      If processes are parallelized with multiple threads, the metric
 *      is recorded for processes, but only for the main thread of each
 *      process
 *  </li>
 *  <li>
 *      <b>SCOREP_METRIC_PER_HOST</b>:<br>
 *      Metric values are recorded for all nodes, but only on the first
 *      process, first thread of each node (not implemented yet)
 *  </li>
 *  <li>
 *      <b>SCOREP_METRIC_ONCE</b>:<br>
 *      Metric values are recorded on the first node, first process,
 *      first thread of a program (not implemented yet)
 *  </li>
 * </ul>
 *
 * @ sync
 * Defines synchronicity type of a metric.
 * Valid value are:
 * <ul>
 *  <li>
 *      <b>SCOREP_METRIC_STRICTLY_SYNC</b>:<br>
 *      Values must be recorded at every enter/leave event. Current
 *      value of this metric is queried by @scorep whenever an
 *      enter/leave event occurs.
 *  </li>
 *  <li>
 *      <b>SCOREP_METRIC_SYNC</b>:<br>
 *      Values are recorded at enter/leave events only. However, it is
 *      NOT absolutely necessary to record such metrics at every
 *      enter/leave event. Current value of this metric is queried by
 *      @scorep whenever an enter/leave event occurs. The metric may
 *      deliver a new value or not.
 *  </li>
 *  <li>
 *      <b>SCOREP_METRIC_ASYNC_EVENT</b>:<br>
 *      Metric values can be recorded at arbitrary points in time, but
 *      are written at enter/leave events. Whenever an enter/leave event
 *      occurs, @scorep queries the metric. The metric returns an
 *      arbitrary number of timestamp-value-pairs.
 *  </li>
 *  <li>
 *      <b>SCOREP_METRIC_ASYNC</b>:<br>
 *      This type of metrics is similar to SCOREP_METRIC_ASYNC_EVENT.
 *      It differs in that SCOREP_METRIC_ASYNC metrics are queried at
 *      specific intervals independent of the occurrence of enter/leave
 *      events.
 *  </li>
 * </ul>
 *
 * @ plugin_version
 * Should be set to SCOREP_METRIC_PLUGIN_VERSION
 *
 *
 * Depending on the plugin's synchronicity type there are some optional
 * functions and variables.
 *
 * Optional functions
 *
 * @ get_current_value
 * Used by strictly synchronous metric plugins only. Returns value of
 * requested metric.
 *
 * @ get_optional_value
 * Used by synchronous metric plugins, but not by strictly synchronous
 * ones. This function requests current value of a metric, but it is
 * valid that no value is returned (read: no update for this metric
 * available).
 *
 * @ get_all_values
 * Used by asynchronous metric plugins. This function is used to request
 * values of a asynchronous metric. The metric will return an arbitrary
 * number of timestamp-value-pairs.
 *
 * @ set_clock_function
 * Used by asynchronous metric plugins. This function passes a function
 * to the plugin, which can be used by the plugin to get a Score-P valid
 * timestamp.
 *
 * Optional variables
 *
 * @ delta_t
 * Defines interval between two calls to update metric value.
 * Ignored for strictly synchronous plugins.
 */

#include <stdbool.h>

#include <scorep/SCOREP_MetricTypes.h>


/** Current version of Score-P metric plugin interface */
#define SCOREP_METRIC_PLUGIN_VERSION 0

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

/** Macro used for implementation of the 'get_info' function */
#define SCOREP_METRIC_PLUGIN_ENTRY( _name ) \
    EXTERN SCOREP_Metric_Plugin_Info \
    SCOREP_MetricPlugin_ ## _name ## _get_info( void )


/* *********************************************************************
 * Type definitions
 **********************************************************************/

/**
 * Properties desribing a metric.
 * Used by @ add_counter function.
 *
 */
typedef struct SCOREP_Metric_Plugin_MetricProperties
{
    /** Plugin name */
    char*                  name;
    /** Additional information about the metric */
    char*                  description;
    /** Metric mode: valid combination of ACCUMULATED|ABSOLUTE|RELATIVE + POINT|START|LAST|NEXT */
    SCOREP_MetricMode      mode;
    /** Value type: signed 64 bit integer, unsigned 64 bit integer, double */
    SCOREP_MetricValueType value_type;
    /** Base of metric: decimal, binary */
    SCOREP_MetricBase      base;
    /** Exponent to scale metric: e.g. 3 for kilo */
    int64_t                exponent;
    /** Unit string of recorded metric */
    char*                  unit;
} SCOREP_Metric_Plugin_MetricProperties;

/**
 * Information describing the plugin.
 *
 */
typedef struct SCOREP_Metric_Plugin_Info
{
    /*
     * For all plugins
     */

    /** Should be set to SCOREP_PLUGIN_VERSION
     *  (needed for back- and forward compatibility)
     */
    uint32_t plugin_version;

    /** Runs per thread / per process / ... */
    SCOREP_MetricPer run_per;

    /** Runs synchronously/asynchronously */
    SCOREP_MetricSynchronicity sync;

    /** Adjust frequency of reading metric values. Score-P will reuqest
     *  metric values of a plugin, at the earliest, after @ delta_t
     *  ticks after last read. Used by plugins of synchronicity type
     *  SCOREP_METRIC_PLUGIN_SYNC, SCOREP_METRIC_PLUGIN_ASYNC_EVENT, and
     *  SCOREP_METRIC_PLUGIN_ASYNC. This value will be ignored for
     *  SCOREP_METRIC_PLUGIN_STRICTLY_SYNC metrics. Metrics of this kind
     *  will be read at every enter/leave event.
     */
    uint64_t delta_t;

    /** This functions is called once per process to initialize
     *  metric plugin. It should return 0 if successful.
     *
     *  @return 0 if successful.
     */
    int32_t ( * initialize )( void );

    /** This functions is called once per process to finalize
     *  metric plugin.
     */
    void ( * finalize )( void );

    /** This is called once per process and
     *  returns meta data about a metric plugin.
     *
     *  @param plugin_name      Name of an individual metric plugin.
     *
     *  @return Meta data (called properties) about this metric plugin.
     */
    SCOREP_Metric_Plugin_MetricProperties* ( *get_event_info )( char* plugin_name );

    /** Add a counter and returns its ID. The ID is generated by the
     *  plugin itself. In addition, the plugin is responsible for ID
     *  management (e.g. generate unique IDs if necessary, locking ID
     *  generation in multi thread environments). This function is
     *  called PER THREAD.
     *
     *  @param event_name       Name of an individual event handled by
     *                          this metric plugin.
     *
     *  @return ID of requested event.
     */
    int32_t ( * add_counter )( char* event_name );

    /*
     * For strictly synchronous plugins
     */
    /** This function must be implemented by strictly synchronous metric
     *  plugins.
     *
     *  @param id               Counter id (see @ add_counter).
     *
     *  @return Current value of requested counter.
     */
    uint64_t ( * get_current_value )( int32_t id );

    /*
     * For synchronous plugins
     */
    /** This function must be implemented by synchronous metric plugins.
     *
     *  @param      id          Counter id (see @ add_counter).
     *  @param[out] value       Value of requested counter.
     *
     *  @return True if value of requested counter was written,
     *          otherwise false.
     */
    bool ( * get_optional_value )( int32_t   id,
                                   uint64_t* value );

    /*
     * For asynchronous plugins
     */
    /** This function must be implemented by asynchronous metric
     *  plugins. It sets the function to get timestamps in Score-P time.
     */
    void ( * set_clock_function )( uint64_t ( * clock_time )( void ) );

    /** This function must be implemented by asynchronous metric
     *  plugins. It is used to get values of asynchronous metrics.
     *
     *  @param      id               Counter id (see @ add_counter).
     *  @param[out] time_value_list  Pointer to list with return values
     *                               (pairs of timestamp and value).
     *
     *  @return Length of list with return values
     */
    uint64_t ( * get_all_values )( int32_t                      id,
                                   SCOREP_MetricTimeValuePair** time_value_list );

    /** Some space for future stuff, should be zeroed */
    uint64_t reserved[ 100 ];
} SCOREP_Metric_Plugin_Info;


#endif /* SCOREP_METRIC_PLUGINS_H */
