/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2019,
 * Technische Universitaet Dresden, Germany
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
 *  @brief This module implements PERF support for Score-P.
 */

#include <config.h>

#include "SCOREP_Config.h"
#include "SCOREP_Location.h"
#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <UTILS_CStr.h>

#if defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901L ) && !defined( C99 )
#define C99
#endif

#if defined( __PGI )
// work around missing __builtin_constant_p used in swab.h.
// https://www.pgroup.com/userforum/viewtopic.php?t=6100
#define __builtin_constant_p( x ) ( 0 )
#endif
#include <linux/perf_event.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>

#include "SCOREP_Metric_Source.h"

/** @def SCOREP_METRIC_MAXNUM Maximum number of PERF metrics concurrently used by a process */
#define SCOREP_METRIC_MAXNUM 20

/**
 * @def STRICTLY_SYNCHRONOUS_METRIC Index of strictly synchronous metrics in the metric definition array
 * @def PER_PROCESS_METRIC          Index of per-process metrics in the metric definition array
 * @def MAX_METRIC_INDEX            Number of metric types supported by the PERF metric source,
 *                                  maximum number of elements in the metric definition array
 */
#define STRICTLY_SYNCHRONOUS_METRIC 0
#define PER_PROCESS_METRIC          1
#define MAX_METRIC_INDEX            2

/** @defgroup SCOREP_Metric_PERF SCOREP PERF Metric Source
 *  @ingroup SCOREP_Metric
 *
 *  This metric source uses the Perf Interface to
 *  access hardware performance counters.
 *
 *  First it is explained how to specify PERF metrics that will be recorded by every location.
 *
 *  You can enable recording of PERF performance metrics by setting the environment variable
 *  SCOREP_METRIC_PERF to a comma-separated list of metric names. Metric names can be any
 *  PERF preset names or PAPI native counter names. For example, set
 *  @verbatim SCOREP_METRIC_PERF=cycles,page-faults,llc-load-misses@endverbatim
 *  to record the number of CPU cycles, the number of page faults and Last Level Cache Load Misses.
 *  If any of the requested metrics is not recognized program execution
 *  will be aborted with an error message. The user can leave the environment variable unset to
 *  indicate that no metrics are requested. Use the tool 'perf list'
 *  to get a list of available PERF events. If you want to change the separator used in the
 *  list of PERF metric names set the environment variable \c SCOREP_METRIC_PERF_SEP to
 *  the needed character.
 *
 *  In addition it is possible to specify metrics that will be recorded per-process. Please use
 *  \c SCOREP_METRIC_PERF_PER_PROCESS for that reason.
 */

/**
 *  Data structure of PERF counter specification
 *
 *  SCOREP_MetricType, SCOREP_MetricValueType, SCOREP_MetricBase,
 *  and SCOREP_MetricProfilingType are implicit
 */
typedef struct scorep_perf_metric
{
    /** Metric name */
    char*    name;
    /** PERF type of this metric */
    int      type;
    /** Configuration bit mask */
    uint64_t config;
} scorep_perf_metric;

/**
 *   Metric definition data.
 */
typedef struct scorep_metric_definition_data
{
    /** Vector of active counters */
    scorep_perf_metric* active_metrics[ SCOREP_METRIC_MAXNUM ];
    /** Number of active counters */
    uint8_t             number_of_metrics;
} scorep_metric_definition_data;

/**
 *  Event map
 */
typedef struct scorep_event_map
{
    /** Identifier of event set */
    int      event_fd;
    /** Return values for the eventsets (additional one for PERF which introduces an offset) */
    uint64_t values[ SCOREP_METRIC_MAXNUM + 1 ];
    /** Number of recorded events in this set */
    int      num_events;
    /** Component identifier */
    int      component;
} scorep_event_map;

/**
 *  Implementation of Score-P event set data structure specific for the PERF metric source.
 */
struct SCOREP_Metric_EventSet
{
    /** A struct for each active component */
    scorep_event_map*              event_map[ SCOREP_METRIC_MAXNUM ];
    /** For each counter a pointer, that points to the event sets return values */
    uint64_t*                      values[ SCOREP_METRIC_MAXNUM ];
    /** Metric definition data */
    scorep_metric_definition_data* definitions;
};

#include "scorep_metric_perf_confvars.inc.c"

/* *********************************************************************
 * Definition of local functions
 **********************************************************************/

static void
metric_perf_add( char*                          name,
                 scorep_perf_metric             metric,
                 scorep_metric_definition_data* metricDefinition );

static SCOREP_Metric_EventSet*
metric_perf_create_event_set( scorep_metric_definition_data* definitions );

static scorep_metric_definition_data*
metric_perf_open( const char* listOfMetricNames,
                  const char* metricsSeparator );

static long
metric_perf_event_open( struct perf_event_attr* hwEvent,
                        pid_t                   pid,
                        int                     cpu,
                        int                     groupFd,
                        unsigned long           flags );

static void
metric_perf_test( scorep_metric_definition_data* metricDefs );

static void
metric_perf_close( void );

static void
metric_perf_warning( int   errcode,
                     char* note );

static void
metric_perf_error( int   errcode,
                   char* note );

static scorep_perf_metric
metric_perf_create_event_code( char* name );



/* *********************************************************************
 * Global variables
 **********************************************************************/

/**
 * Definition data of metrics (e.g. synchronous strict, per-process metrics).
 */
static scorep_metric_definition_data* metric_defs[ MAX_METRIC_INDEX ];

/**
 * Static variable to control initialization status of the metric adapter.
 * If it is 0, then PERF metric adapter is initialized.
 */
static int metric_perf_initialize = 1;



/* *********************************************************************
 * Helper functions
 **********************************************************************/

/** @brief Returns a PERF metric. If metric's type is PERF_TYPE_MAX
 *         the metric is invalid.
 */
static scorep_perf_metric
metric_perf_create_event_code( char* name )
{
    scorep_perf_metric return_metric;
    int                i, j, k;
    return_metric.type = PERF_TYPE_MAX;
    return_metric.name = name;
    /* hardware events */
    if ( ( strstr( name, "cpu-cycles" ) == name ) || ( strstr( name, "cycles" ) == name ) )
    {
        return_metric.type   = PERF_TYPE_HARDWARE;
        return_metric.config = PERF_COUNT_HW_CPU_CYCLES;
        return return_metric;
    }
#if HAVE( DECL_PERF_COUNT_HW_STALLED_CYCLES_FRONTEND )
    else
    if ( ( strstr( name, "stalled-cycles-frontend" ) == name ) || ( strstr( name, "idle-cycles-frontend" ) == name ) )
    {
        return_metric.type   = PERF_TYPE_HARDWARE;
        return_metric.config = PERF_COUNT_HW_STALLED_CYCLES_FRONTEND;
        return return_metric;
    }
#endif /* HAVE( DECL_PERF_COUNT_HW_STALLED_CYCLES_FRONTEND ) */
#if HAVE( DECL_PERF_COUNT_HW_STALLED_CYCLES_BACKEND )
    else
    if ( ( strstr( name, "stalled-cycles-backend" ) == name ) || ( strstr( name, "idle-cycles-backend" ) == name ) )
    {
        return_metric.type   = PERF_TYPE_HARDWARE;
        return_metric.config = PERF_COUNT_HW_STALLED_CYCLES_BACKEND;
        return return_metric;
    }
#endif /* HAVE( DECL_PERF_COUNT_HW_STALLED_CYCLES_BACKEND ) */
#if HAVE( DECL_PERF_COUNT_HW_REF_CPU_CYCLES )
    else
    if ( strstr( name, "ref-cycles" ) == name )
    {
        return_metric.type   = PERF_TYPE_HARDWARE;
        return_metric.config = PERF_COUNT_HW_REF_CPU_CYCLES;
        return return_metric;
    }
#endif /* HAVE( DECL_PERF_COUNT_HW_REF_CPU_CYCLES ) */
    else
    if ( strstr( name, "instructions" ) == name )
    {
        return_metric.type   = PERF_TYPE_HARDWARE;
        return_metric.config = PERF_COUNT_HW_INSTRUCTIONS;
        return return_metric;
    }
    if ( strstr( name, "cache-references" ) == name )
    {
        return_metric.type   = PERF_TYPE_HARDWARE;
        return_metric.config = PERF_COUNT_HW_CACHE_REFERENCES;
        return return_metric;
    }
    if ( strstr( name, "cache-misses" ) == name )
    {
        return_metric.type   = PERF_TYPE_HARDWARE;
        return_metric.config = PERF_COUNT_HW_CACHE_MISSES;
        return return_metric;
    }
    if ( ( strstr( name, "branch-instructions" ) == name ) || ( strstr( name, "branches" ) == name ) )
    {
        return_metric.type   = PERF_TYPE_HARDWARE;
        return_metric.config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS;
        return return_metric;
    }
    if ( strstr( name, "branch-misses" ) == name )
    {
        return_metric.type   = PERF_TYPE_HARDWARE;
        return_metric.config = PERF_COUNT_HW_BRANCH_MISSES;
        return return_metric;
    }
    if ( strstr( name, "bus-cycles" ) == name )
    {
        return_metric.type   = PERF_TYPE_HARDWARE;
        return_metric.config = PERF_COUNT_HW_BUS_CYCLES;
        return return_metric;
    }

    /* software events */
    if ( strstr( name, "cpu-clock" ) == name )
    {
        return_metric.type   = PERF_TYPE_SOFTWARE;
        return_metric.config = PERF_COUNT_SW_CPU_CLOCK;
    }
    if ( strstr( name, "task-clock" ) == name )
    {
        return_metric.type   = PERF_TYPE_SOFTWARE;
        return_metric.config = PERF_COUNT_SW_TASK_CLOCK;
    }
    if ( ( strstr( name, "page-faults" ) == name ) || ( strstr( name, "faults" ) == name ) )
    {
        return_metric.type   = PERF_TYPE_SOFTWARE;
        return_metric.config = PERF_COUNT_SW_PAGE_FAULTS;
    }
    if ( strstr( name, "minor-faults" ) == name )
    {
        return_metric.type   = PERF_TYPE_SOFTWARE;
        return_metric.config = PERF_COUNT_SW_PAGE_FAULTS_MIN;
    }
    if ( strstr( name, "major-faults" ) == name )
    {
        return_metric.type   = PERF_TYPE_SOFTWARE;
        return_metric.config = PERF_COUNT_SW_PAGE_FAULTS_MAJ;
    }
    if ( strstr( name, "major-faults" ) == name )
    {
        return_metric.type   = PERF_TYPE_SOFTWARE;
        return_metric.config = PERF_COUNT_SW_PAGE_FAULTS_MAJ;
    }
    if ( ( strstr( name, "context-switches" ) == name ) || ( strstr( name, "cs" ) == name ) )
    {
        return_metric.type   = PERF_TYPE_SOFTWARE;
        return_metric.config = PERF_COUNT_SW_CONTEXT_SWITCHES;
    }
    if ( ( strstr( name, "cpu-migrations" ) == name ) || ( strstr( name, "migrations" ) == name ) )
    {
        return_metric.type   = PERF_TYPE_SOFTWARE;
        return_metric.config = PERF_COUNT_SW_CPU_MIGRATIONS;
    }
#if HAVE( DECL_PERF_COUNT_SW_ALIGNMENT_FAULTS )
    if ( strstr( name, "alignment-faults" ) == name )
    {
        return_metric.type   = PERF_TYPE_SOFTWARE;
        return_metric.config = PERF_COUNT_SW_ALIGNMENT_FAULTS;
    }
#endif /* HAVE( DECL_PERF_COUNT_SW_ALIGNMENT_FAULTS ) */
#if HAVE( DECL_PERF_COUNT_SW_EMULATION_FAULTS )
    if ( strstr( name, "emulation-faults" ) == name )
    {
        return_metric.type   = PERF_TYPE_SOFTWARE;
        return_metric.config = PERF_COUNT_SW_EMULATION_FAULTS;
    }
#endif /* HAVE( DECL_PERF_COUNT_SW_EMULATION_FAULTS ) */

    if ( return_metric.type == PERF_TYPE_SOFTWARE )
    {
        return return_metric;
    }

    /* cache events */
    char  buffer[ 128 ];
    char* caches[ 7 ]  = { "L1-dcache-", "L1-icache-", "LLC-", "dTLB-", "iTLB-", "branch-", "node-" };
    int   num_caches   = 7;
    char* actions[ 3 ] = { "load", "store", "prefetch" };
    int   num_actions  = 2;
    char* results[ 3 ] = { "s", "-misses", "es" };
    int   num_results  = 3;
    for ( i = 0; i < num_caches; i++ )
    {
        if ( strstr( name, caches[ i ] ) == name )
        {
            for ( j = 0; j < num_actions; j++ )
            {
                for ( k = 0; k < num_results; k++ )
                {
                    int num_chars = snprintf( buffer, sizeof( buffer ), "%s%s%s", caches[ i ], actions[ j ], results[ k ] );
                    UTILS_BUG_ON( num_chars < 0 || num_chars >= sizeof( buffer ), "Failed to format event name." );
                    if ( strstr( name, buffer ) == name )
                    {
                        return_metric.type   = PERF_TYPE_HW_CACHE;
                        return_metric.config = i << 16 | j << 8;
                        if ( k == 1 )
                        {
                            return_metric.config |= 1;
                        }
                        return return_metric;
                    }
                }
            }
        }
    }
    /* raw events */
    if ( name[ 0 ] == 'r' )
    {
        /* get event */
        return_metric.type   = PERF_TYPE_RAW;
        return_metric.config = strtoll( &( name[ 1 ] ), NULL, 16 );
        return return_metric;
    }

    /* wrong metric */
    UTILS_ERROR( SCOREP_ERROR_PERF_INIT, "PERF metric not recognized: %s", name );
    return return_metric;
}


/** @brief Translation from syscall to something more readable
 */
static long
metric_perf_event_open( struct perf_event_attr* hwEvent,
                        pid_t                   pid,
                        int                     cpu,
                        int                     groupFd,
                        unsigned long           flags )
{
    int ret;

    ret = syscall( __NR_perf_event_open, hwEvent, pid, cpu,
                   groupFd, flags );
    return ret;
}

/** @brief Reads the configuration from environment variables and configuration
 *         files and initializes the performance counter library. It must be called
 *         before other functions of the library are used by the measurement system.
 *
 *  @param listOfMetricNames        Content of environment variable specifying
 *                                  requested resource usage metric names.
 *  @param metricsSeparator         Character separating entries in list of
 *                                  metric names.
 *
 *  @return Returns definition data of specified metrics.
 */
static scorep_metric_definition_data*
metric_perf_open( const char* listOfMetricNames,
                  const char* metricsSeparator )
{
    /** Content of environment variable SCOREP_METRIC_PERF */
    char* env_metrics;
    /** Individual metric */
    char* token;

    /* Get working copy of all metric names. */
    env_metrics = UTILS_CStr_dup( listOfMetricNames );

    /* Return if environment variable is empty */
    size_t str_len = strlen( env_metrics );
    if ( str_len == 0 )
    {
        goto out;
    }

    /* Count number of separator characters in list of metric names */
    size_t      list_alloc = 1;
    const char* position   = env_metrics;
    while ( *position )
    {
        if ( strchr( metricsSeparator, *position ) )
        {
            list_alloc++;
        }
        position++;
    }

    /* Allocate memory for array of metric names */
    char** metric_names = calloc( list_alloc, sizeof( char* ) );
    if ( !metric_names )
    {
        UTILS_ERROR_POSIX();
        goto out;
    }

    /* Parse list of metric names */
    size_t list_len = 0;
    token = strtok( env_metrics, metricsSeparator );
    while ( token )
    {
        if ( list_len >= list_alloc )
        {
            /* something strange has happened, we have
             * more entries as in the first run */
            goto out2;
        }

        metric_names[ list_len ] = token;

        token = strtok( NULL, metricsSeparator );
        list_len++;
    }

    /* Create new event set (variables initialized with zero) */
    scorep_metric_definition_data* metric_definition = calloc( 1, sizeof( scorep_metric_definition_data ) );
    UTILS_ASSERT( metric_definition );

    for ( uint32_t i = 0; i < list_len; i++ )
    {
        /* Current metric name */
        scorep_perf_metric metric = metric_perf_create_event_code( metric_names[ i ] );

        metric_perf_add( metric_names[ i ], metric, metric_definition );
    }

    /* Clean up */
    free( env_metrics );
    free( metric_names );

    /* Check whether event combination is valid. This is done here to
     * avoid errors when creating the event set for each thread, which
     * would multiply the error messages. */
    metric_perf_test( metric_definition );

    return metric_definition;

out2:
    free( metric_names );

out:
    free( env_metrics );
    return NULL;
}

/** @brief Finalizes the performance counter adapter. Frees memory allocated by
 *         metric_perf_open.
 */
static void
metric_perf_close( void )
{
    for ( uint32_t metric_index = 0; metric_index < MAX_METRIC_INDEX; metric_index++ )
    {
        if ( metric_defs[ metric_index ] == NULL
             || metric_defs[ metric_index ]->number_of_metrics == 0 )
        {
            continue;
        }

        for ( uint32_t i = 0; i < metric_defs[ metric_index ]->number_of_metrics; i++ )
        {
            free( metric_defs[ metric_index ]->active_metrics[ i ]->name );
            free( metric_defs[ metric_index ]->active_metrics[ i ] );
        }
        free( metric_defs[ metric_index ] );
        metric_defs[ metric_index ] = NULL;
    }
}

/** @brief  Creates per-thread counter sets.
 *
 *  @param definitions          Metric definition data.
 *
 *  @return It returns the new event set.
 */
static SCOREP_Metric_EventSet*
metric_perf_create_event_set( scorep_metric_definition_data* definitions )
{
    SCOREP_Metric_EventSet* event_set;
    int                     retval;
    int                     component;

    if ( definitions->number_of_metrics == 0 )
    {
        return NULL;
    }

    event_set = ( SCOREP_Metric_EventSet* )malloc( sizeof( SCOREP_Metric_EventSet ) );
    UTILS_ASSERT( event_set );

    /* Create event set */
    for ( uint32_t i = 0; i < SCOREP_METRIC_MAXNUM; i++ )
    {
        event_set->event_map[ i ] = NULL;
    }
    event_set->definitions = definitions;

    uint32_t j;
    component = 0;
    for ( uint32_t i = 0; i < definitions->number_of_metrics; i++ )
    {
        struct scorep_event_map* event_map;
        struct perf_event_attr   attr;
        memset( &attr, 0, sizeof( struct perf_event_attr ) );
        attr.type           = definitions->active_metrics[ i ]->type;
        attr.config         = definitions->active_metrics[ i ]->config;
        attr.exclude_kernel = 1;  /* don't count kernel */
        attr.exclude_hv     = 1;  /* don't count hypervisor */
        attr.read_format    = PERF_FORMAT_GROUP;

        /* Search for the event map that matches the counter */
        j = 0;
        while ( j < SCOREP_METRIC_MAXNUM &&
                event_set->event_map[ j ] != NULL &&
                event_set->event_map[ j ]->component != component )
        {
            j++;
        }
        if ( event_set->event_map[ j ] == NULL )
        {
            /* No event of this component yet! */
            attr.disabled                         = 1;
            event_set->event_map[ j ]             = ( struct scorep_event_map* )malloc( sizeof( scorep_event_map ) );
            event_set->event_map[ j ]->num_events = 0;
            event_set->event_map[ j ]->event_fd   = metric_perf_event_open( &attr, 0, -1, -1, 0 );
            if ( event_set->event_map[ j ]->event_fd < 0 )
            {
                metric_perf_error( event_set->event_map[ j ]->event_fd, "metric_perf_event_open" );
            }
            else
            {
                event_set->event_map[ j ]->component = component;
                event_map                            = event_set->event_map[ j ];
                /* we have to think of the offset (1) that is needed for reading a group of PERF events */
                event_set->values[ i ] = &( event_map->values[ event_map->num_events + 1 ] );
                event_map->num_events++;
            }
        }
        else
        {
            int fd = metric_perf_event_open( &attr, 0, -1, event_set->event_map[ j ]->event_fd, 0 );
            if ( fd < 0 )
            {
                metric_perf_error( fd, "metric_perf_event_open" );
            }
            else
            {
                event_map = event_set->event_map[ j ];
                /* we have to think of the offset (1) that is needed for reading a group of PERF events */
                event_set->values[ i ] = &( event_map->values[ event_map->num_events + 1 ] );
                event_map->num_events++;
            }
        }
    }

    /* For each used event map. There is currently only one component, so i is 0 (only one fd to activate)  */
    for ( uint32_t i = 0; i < SCOREP_METRIC_MAXNUM && event_set->event_map[ i ] != NULL; i++ )
    {
        retval = ioctl( event_set->event_map[ i ]->event_fd, PERF_EVENT_IOC_ENABLE );

        if ( retval != 0 )
        {
            metric_perf_error( errno, "ioctl( fd, PERF_EVENT_IOC_ENABLE )" );
        }
    }

    return event_set;
}

/** @brief Adds a new metric to internally managed vector.
 */
static void
metric_perf_add( char*                          name,
                 scorep_perf_metric             metric,
                 scorep_metric_definition_data* metricDefinition )
{
    UTILS_ASSERT( metricDefinition );

    if ( metricDefinition->number_of_metrics >= SCOREP_METRIC_MAXNUM )
    {
        UTILS_ERROR( SCOREP_ERROR_PERF_INIT, "Number of counters exceeds Score-P allowed maximum of %d", SCOREP_METRIC_MAXNUM );
    }
    else
    {
        metricDefinition->active_metrics[ metricDefinition->number_of_metrics ]         = ( scorep_perf_metric* )malloc( sizeof( scorep_perf_metric ) );
        metricDefinition->active_metrics[ metricDefinition->number_of_metrics ]->name   = UTILS_CStr_dup( name );
        metricDefinition->active_metrics[ metricDefinition->number_of_metrics ]->type   = metric.type;
        metricDefinition->active_metrics[ metricDefinition->number_of_metrics ]->config = metric.config;
        ( metricDefinition->number_of_metrics )++;
    }
}

/** @brief Prints a PERF-specific error message.
 *
 *  @param errcode error code
 *  @param note    Additonal information.
 */
static void
metric_perf_error( int   errcode,
                   char* note )
{
    UTILS_ERROR( SCOREP_ERROR_PERF_INIT, "%s: %s (fatal)\n", note ? note : "PERF", strerror( errno ) );
    _Exit( EXIT_FAILURE );
}

/** @brief Prints a PERF-specific warning message.
 *
 *  @param errcode Error code.
 *  @param note    Additonal information.
 */
static void
metric_perf_warning( int   errcode,
                     char* note )
{
    UTILS_WARNING( "%s: %s (ignored)\n", note ? note : "PERF", strerror( errno ) );
}


/** @brief Tests whether requested event combination is valid or not.
 *
 *  @param metricDefinition     Reference to metric definition data structure.
 */
static void
metric_perf_test( scorep_metric_definition_data* metricDefinition )
{
    int                      i;
    int                      j;
    int                      component;
    struct scorep_event_map* event_set[ SCOREP_METRIC_MAXNUM ];

    for ( i = 0; i < SCOREP_METRIC_MAXNUM; i++ )
    {
        event_set[ i ] = NULL;
    }

    for ( i = 0; i < metricDefinition->number_of_metrics; i++ )
    {
        struct perf_event_attr attr;
        memset( &attr, 0, sizeof( struct perf_event_attr ) );
        attr.type           = metricDefinition->active_metrics[ i ]->type;
        attr.config         = metricDefinition->active_metrics[ i ]->config;
        attr.exclude_kernel = 1;

        /* Search for the eventset that matches the counter */
        j         = 0;
        component = 0;
        while ( j < SCOREP_METRIC_MAXNUM &&
                event_set[ j ] != NULL &&
                event_set[ j ]->component != component )
        {
            j++;
        }

        if ( event_set[ j ] == NULL )
        {
            /* Create event set, if no matching found */
            attr.disabled            = 1;
            event_set[ j ]           = malloc( sizeof( scorep_event_map ) );
            event_set[ j ]->event_fd = metric_perf_event_open( &attr, 0, -1,
                                                               -1,  0 );
            if ( event_set[ j ]->event_fd < 0 )
            {
                metric_perf_error( event_set[ j ]->event_fd, "metric_perf_event_open (test)" );
            }
            else
            {
                event_set[ j ]->component  = component;
                event_set[ j ]->num_events = 1;
            }
        }
        else
        {
            int fd = metric_perf_event_open( &attr, 0, -1, event_set[ j ]->event_fd, 0 );
            if ( fd < 0 )
            {
                metric_perf_error( fd, "metric_perf_event_open (test, sub-event)" );
            }
            else
            {
                event_set[ j ]->num_events++;
            }
        }
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Event %s added to event set", metricDefinition->active_metrics[ i ]->name );
    }

    /* For each used eventset */
    for ( i = 0; i < SCOREP_METRIC_MAXNUM && event_set[ i ] != NULL; i++ )
    {
        close( event_set[ i ]->event_fd );
        free( event_set[ i ] );
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Event set tested OK" );
}


/* *********************************************************************
 * Adapter management
 **********************************************************************/

/* Make prototype public */
static void
free_event_set( SCOREP_Metric_EventSet* eventSet );

/** @brief  Registers configuration variables for the metric adapters.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
register_source( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " register PERF metric source!" );

    /* Register environment variables for 'synchronous strict' and 'per-process' metrics */
    SCOREP_ErrorCode status;
    status = SCOREP_ConfigRegister( "metric", scorep_metric_perf_confvars );
    if ( status != SCOREP_SUCCESS )
    {
        UTILS_ERROR( SCOREP_ERROR_PERF_INIT, "Registration of PERF configure variables failed." );
        return status;
    }

    return SCOREP_SUCCESS;
}

/** @brief Called on deregistration of the metric adapter. Currently, no action is performed
 *         on deregistration.
 */
static void
deregister_source( void )
{
    /* Free environment variables for 'synchronous strict' and per-process metrics */
    free( scorep_metrics_perf );
    free( scorep_metrics_perf_per_process );
    free( scorep_metrics_perf_separator );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " PERF metric source deregister!" );
}

/** @brief  Initializes the PERF metric adapter.
 *
 *  During initialization respective environment variables are read to
 *  determine which metrics has been specified by the user.
 *
 *  Because 'synchronous strict' metrics will be recorded by all
 *  locations, we known how many metrics of this specific type each
 *  location will record. This number is returned by this function.
 *
 *  For metrics of other types (e.g. per-process) we cannot determine
 *  whether this location will be responsible to record metrics of this
 *  type. Responsibility will be determine while initializing location.
 *  Therefore, within this function we don't known how many additional
 *  metrics will be recorded by a specific location.
 *
 *  @return It returns the number of used 'synchronous strict' metrics.
 */
static uint32_t
initialize_source( void )
{
    /* Number of used 'synchronous strict' metrics */
    uint32_t metric_counts = 0;

    if ( metric_perf_initialize )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialize PERF metric source." );

        /* FIRST: Read specification of global synchronous strict metrics from respective environment variable. */
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "[PERF] global synchronous strict metrics = %s", scorep_metrics_perf );

        metric_defs[ STRICTLY_SYNCHRONOUS_METRIC ] =
            metric_perf_open( scorep_metrics_perf, scorep_metrics_perf_separator );
        if ( metric_defs[ STRICTLY_SYNCHRONOUS_METRIC ] != NULL )
        {
            metric_counts = metric_defs[ STRICTLY_SYNCHRONOUS_METRIC ]->number_of_metrics;
        }

        /*
         * Handle additional metric types (e.g. per-process)
         */

        /* SECOND: Read specification of per-process metrics from respective environment variable. */
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "[PERF] per-process metrics = %s", scorep_metrics_perf_per_process );

        metric_defs[ PER_PROCESS_METRIC ] =
            metric_perf_open( scorep_metrics_perf_per_process, scorep_metrics_perf_separator );

        /* Set flag */
        metric_perf_initialize = 0;

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialization of PERF metric source done." );
    }

    return metric_counts;
}

/** @brief Adapter finalization.
 */
static void
finalize_source( void )
{
    /* Call only, if previously initialized */
    if ( !metric_perf_initialize )
    {
        metric_perf_close();

        /* Set initialization flag */
        metric_perf_initialize = 1;
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " finalize PERF metric source." );
    }
}

/** @brief  Location specific initialization function for metric adapters.
 *
 *  @param location             Location data.
 *  @param syncType             Current synchronicity type, e.g.
 *                              SCOREP_METRIC_STRICTLY_SYNC,
 *                              SCOREP_METRIC_SYNC,
 *                              SCOREP_METRIC_ASYNC, or
 *                              SCOREP_METRIC_ASYNC_EVENT.
 *  @param metricType           Current metric type, e.g.
 *                              SCOREP_METRIC_PER_THREAD,
 *                              SCOREP_METRIC_PER_PROCESS,
 *                              SCOREP_METRIC_PER_HOST, or
 *                              SCOREP_METRIC_ONCE.
 *
 *  @return Event set
 */
static SCOREP_Metric_EventSet*
initialize_location( SCOREP_Location*           locationData,
                     SCOREP_MetricSynchronicity syncType,
                     SCOREP_MetricPer           metricType )
{
    /*
     * Check whether this location has to record global strictly synchronous metrics
     */
    if ( syncType == SCOREP_METRIC_STRICTLY_SYNC
         && metricType == SCOREP_METRIC_PER_THREAD
         && metric_defs[ STRICTLY_SYNCHRONOUS_METRIC ] != NULL )
    {
        return metric_perf_create_event_set( metric_defs[ STRICTLY_SYNCHRONOUS_METRIC ] );
    }

    /*
     * Handle different kinds of per-location metrics
     *
     * Second: Check whether this location has to record per-process metrics
     */
    if ( syncType == SCOREP_METRIC_SYNC                   // synchronous metrics are requested
         && metricType == SCOREP_METRIC_PER_PROCESS       // per-process metrics are requested
         && metric_defs[ PER_PROCESS_METRIC ] != NULL )   // user has defined per-process metrics
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "[PERF] This location will record per-process metrics." );

        return metric_perf_create_event_set( metric_defs[ PER_PROCESS_METRIC ] );
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "PERF thread support initialized" );

    return NULL;
}

/** @brief Location specific finalization function for metric adapters.
 *
 *  @param eventSet             Event set to close (may be NULL).
 */
static void
finalize_location( SCOREP_Metric_EventSet* eventSet )
{
    if ( eventSet == NULL )
    {
        return;
    }

    free_event_set( eventSet );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " PERF metric source finalize location!" );
}

/** @brief Reads values of counters relative to the time of metric_perf_open().
 *         This function is used to write values of strictly synchronous metrics.
 *
 *  @param eventSet An event set, that contains the definition of the counters
 *                  that should be measured.
 *  @param values   An array, to which the counter values are written.
 */
static void
strictly_synchronous_read( SCOREP_Metric_EventSet* eventSet,
                           uint64_t*               values )
{
    UTILS_ASSERT( eventSet );
    UTILS_ASSERT( values );

    int retval;

    /* For each used eventset */
    for ( uint32_t i = 0; i < SCOREP_METRIC_MAXNUM && eventSet->event_map[ i ] != NULL; i++ )
    {
        retval = read( eventSet->event_map[ i ]->event_fd, eventSet->event_map[ i ]->values, ( eventSet->event_map[ i ]->num_events + 1 ) * sizeof( uint64_t ) );
        if ( retval != ( eventSet->event_map[ i ]->num_events + 1 ) * sizeof( uint64_t ) )
        {
            metric_perf_error( retval, "PERF read" );
        }
    }

    for ( uint32_t i = 0; i < eventSet->definitions->number_of_metrics; i++ )
    {
        values[ i ] = ( uint64_t )*eventSet->values[ i ];
    }
}

/** @brief Reads values of counters relative to the time of metric_perf_open().
 *         This function is used to write values of synchronous metrics.
 *
 *  @param eventSet[in]     An event set, that contains the definition of the counters
 *                          that should be measured.
 *  @param values[out]      Reference to array that will be filled with values from
 *                          active metrics.
 *  @param isUpdated[out]   An array which indicates whether a new value of a specific
 *                          metric was written (@ isUpdated[i] == true ) or not
 *                          (@ isUpdated[i] == false ).
 *  @param forceUpdate[in]  Update of all metric value in this event set is enforced.
 */
static void
synchronous_read( SCOREP_Metric_EventSet* eventSet,
                  uint64_t*               values,
                  bool*                   isUpdated,
                  bool                    forceUpdate )
{
    UTILS_ASSERT( eventSet );
    UTILS_ASSERT( values );
    UTILS_ASSERT( isUpdated );

    int retval;

    /* For each used eventset */
    for ( uint32_t i = 0; i < SCOREP_METRIC_MAXNUM && eventSet->event_map[ i ] != NULL; i++ )
    {
        retval = read( eventSet->event_map[ i ]->event_fd, eventSet->event_map[ i ]->values, ( eventSet->event_map[ i ]->num_events + 1 ) * sizeof( uint64_t ) );
        if ( retval != ( eventSet->event_map[ i ]->num_events + 1 ) * sizeof( uint64_t ) )
        {
            metric_perf_error( retval, "PERF read" );
        }
    }

    for ( uint32_t i = 0; i < eventSet->definitions->number_of_metrics; i++ )
    {
        values[ i ]    = ( uint64_t )*eventSet->values[ i ];
        isUpdated[ i ] = true;
    }
}

/** @brief Frees per-thread counter sets.
 *
 *  @param eventSet The event set that defines the measured counters
 *                  which should be freed.
 */
static void
free_event_set( SCOREP_Metric_EventSet* eventSet )
{
    /*
     * Check for ( eventSet == NULL) can be skipped here, because all functions
     * that call this one should have already checked eventSet.
     */

    /* Treat PERF failures at this point as non-fatal */

    /* For each used event map */
    for ( uint32_t i = 0; i < SCOREP_METRIC_MAXNUM && eventSet->event_map[ i ] != NULL; i++ )
    {
        int retval = ioctl( eventSet->event_map[ i ]->event_fd, PERF_EVENT_IOC_DISABLE );
        if ( retval )
        {
            metric_perf_warning( retval, "PERF ioctl( fd, PERF_EVENT_IOC_DISABLE)" );
        }
        retval = close( eventSet->event_map[ i ]->event_fd );
        if ( retval )
        {
            metric_perf_warning( retval, "PERF close( fd)" );
        }

        free( eventSet->event_map[ i ] );
    }

    /*
     * The issue with respect to IO tracing was explained above:
     * At this point we can resume IO tracing. Therefore a function or
     * macro like SCOREP_RESUME_IO_TRACING( CURRENT_THREAD ) would be
     * required.
     */

    free( eventSet );
}

/** @brief  Returns the number of recently used metrics.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *
 *  @return It returns the number of recently used metrics.
 */
static uint32_t
get_number_of_metrics( SCOREP_Metric_EventSet* eventSet )
{
    if ( eventSet == NULL )
    {
        return 0;
    }

    return eventSet->definitions->number_of_metrics;
}

/** @brief  Returns the name of requested metric.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns the name of requested metric.
 */
static const char*
get_metric_name( SCOREP_Metric_EventSet* eventSet,
                 uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    if ( metricIndex < eventSet->definitions->number_of_metrics )
    {
        return eventSet->definitions->active_metrics[ metricIndex ]->name;
    }
    else
    {
        return "";
    }
}


/** @brief  Returns a description of requested metric.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns a description of the unit of requested metric.
 */
static const char*
get_metric_description( SCOREP_Metric_EventSet* eventSet,
                        uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );
    return "";
}

/** @brief  Returns a string containing a representation of the unit of requested metric.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns a string containing a representation of the unit of requested metric.
 */
static const char*
get_metric_unit( SCOREP_Metric_EventSet* eventSet,
                 uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    if ( metricIndex < eventSet->definitions->number_of_metrics )
    {
        return "#";
    }
    else
    {
        return "";
    }
}

/** @brief  Returns properties of a metric. The metric is requested by @a metricIndex.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric
 *
 *  @return It returns property settings of requested metrics.
 */
static SCOREP_Metric_Properties
get_metric_properties( SCOREP_Metric_EventSet* eventSet,
                       uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    SCOREP_Metric_Properties props;

    if ( metricIndex < eventSet->definitions->number_of_metrics )
    {
        props.name           = eventSet->definitions->active_metrics[ metricIndex ]->name;
        props.description    = "";
        props.source_type    = SCOREP_METRIC_SOURCE_TYPE_PERF;
        props.mode           = SCOREP_METRIC_MODE_ACCUMULATED_START;
        props.value_type     = SCOREP_METRIC_VALUE_UINT64;
        props.base           = SCOREP_METRIC_BASE_DECIMAL;
        props.exponent       = 0;
        props.unit           = "#";
        props.profiling_type = SCOREP_METRIC_PROFILING_TYPE_INCLUSIVE;

        return props;
    }
    else
    {
        props.name           = "";
        props.description    = "";
        props.source_type    = SCOREP_INVALID_METRIC_SOURCE_TYPE;
        props.mode           = SCOREP_INVALID_METRIC_MODE;
        props.value_type     = SCOREP_INVALID_METRIC_VALUE_TYPE;
        props.base           = SCOREP_INVALID_METRIC_BASE;
        props.exponent       = 0;
        props.unit           = "";
        props.profiling_type = SCOREP_INVALID_METRIC_PROFILING_TYPE;

        return props;
    }
}


/** Implementation of the metric source initialization/finalization data structure */
const SCOREP_MetricSource SCOREP_Metric_Perf =
{
    SCOREP_METRIC_SOURCE_TYPE_PERF,
    &register_source,
    &initialize_source,
    &initialize_location,
    NULL,                                           // no synchronization function needed
    &free_event_set,
    &finalize_location,
    &finalize_source,
    &deregister_source,
    &strictly_synchronous_read,
    &synchronous_read,
    NULL,                                           // no asynchronous read function needed
    &get_number_of_metrics,
    &get_metric_name,
    &get_metric_description,
    &get_metric_unit,
    &get_metric_properties
};
