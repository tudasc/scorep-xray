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
 *  @file       scorep_metric_rusage.c
 *
 *  @author     Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *  @maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *
 *  @brief This module implements support for resource usage counters.
 */

#include <config.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "SCOREP_Config.h"
#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>

#include "SCOREP_Metric_Source.h"


/** @defgroup SCOREP_Metric_RUSAGE Resource Usage Metric Source
 *  @ingroup SCOREP_Metric
 *
 *  This metric source uses the Unix system call \c getrusage to provide information
 *  about consumed resources and operating system events of processes such as
 *  user/system time, received signals, and number of page faults. The manual page
 *  of \c getrusage provides a list of resource usage metrics. Please note that the
 *  availability of specific metrics depends on the operating system.
 *
 *  First it is explained how to specify metrics that will be recorded by every location.
 *
 *  You can enable recording of resource usage metrics by setting the environment variable
 *  \c SCOREP_METRIC_RUSAGE. The variable should contain a comma-separated list of metric names.
 *  For example, set
 *  @verbatim SCOREP_METRIC_RUSAGE=ru_utime,ru_stime @endverbatim
 *  to record the user time and system time consumed by each process. If any of the requested
 *  metrics is not recognized program execution will be aborted with an error message. The
 *  user can leave the environment variable unset to indicate that no metrics are requested.
 *  It is also possible to set
 *  @verbatim SCOREP_METRIC_RUSAGE=all @endverbatim
 *  to record all resource usage metrics. However, this is not recommended as most operating
 *  systems does not support all metrics.
 *
 *  If you want to change the separator used in the list of resource usage metrics set the
 *  environment variable \c SCOREP_METRIC_RUSAGE_SEP to the needed character. For example,
 *  set
 *  @verbatim SCOREP_METRIC_RUSAGE_SEP=: @endverbatim
 *  to separate metrics by colons.
 *
 *  When using the resource usage counters for multi-threaded programs, at the moment the
 *  information displayed is valid for the whole process and not for each single thread.
 *
 *  In addition it is possible to specify metrics that will be recorded per-process. Please use
 *  \c SCOREP_METRIC_RUSAGE_PER_PROCESS for that reason.
 */

/** Resource usage counter indices */
typedef enum
{
    RU_UTIME    =  0,
    RU_STIME    =  1,
    RU_MAXRSS   =  2,
    RU_IXRSS    =  3,
    RU_IDRSS    =  4,
    RU_ISRSS    =  5,
    RU_MINFLT   =  6,
    RU_MAJFLT   =  7,
    RU_NSWAP    =  8,
    RU_INBLOCK  =  9,
    RU_OUBLOCK  = 10,
    RU_MSGSND   = 11,
    RU_MSGRCV   = 12,
    RU_NSIGNALS = 13,
    RU_NVCSW    = 14,
    RU_NIVCSW   = 15,

    SCOREP_RUSAGE_CNTR_MAXNUM
} scorep_rusage_metric_type;

/**
 *  Data structure of resource usage counter specification
 *
 *  SCOREP_MetricType, SCOREP_MetricValueType, SCOREP_MetricBase,
 *  and SCOREP_MetricProfilingType are implicit
 */
typedef struct scorep_rusage_metric_struct
{
    /** Internal index of this metric */
    const scorep_rusage_metric_type index;
    /** Name of this metric */
    const char*                     name;
    /** Base unit of this metric (e.g. seconds) */
    const char*                     unit;
    /** Longer description of this metric */
    const char*                     description;
    /** Mode of this metric (absolute or accumulated) */
    SCOREP_MetricMode               mode;
    /** Base of this metric (decimal or binary) */
    SCOREP_MetricBase               base;
    /** Exponent to scale values of this metric */
    int64_t                         exponent;
} scorep_rusage_metric;



/* *********************************************************************
 * Global variables
 **********************************************************************/

/** Static variable to control initialize status of the rusage metric source.
 *  If it is 0 it is initialized. */
static int scorep_metric_rusage_initialize = 1;

#include "scorep_metric_rusage_confvars.inc.c"

/* *INDENT-OFF* */

/** Vector of resource usage counter specifications
 *  @li struct timeval ru_utime    - user time used
 *  @li struct timeval ru_stime    - system time used
 *  @li long           ru_maxrss   - maximum resident set size
 *  @li long           ru_ixrss    - integral shared memory size
 *  @li long           ru_idrss    - integral unshared data size
 *  @li long           ru_isrss    - integral unshared stack size
 *  @li long           ru_minflt   - page reclaims
 *  @li long           ru_majflt   - page faults
 *  @li long           ru_nswap    - swaps
 *  @li long           ru_inblock  - block input operations
 *  @li long           ru_oublock  - block output operations
 *  @li long           ru_msgsnd   - messages sent
 *  @li long           ru_msgrcv   - messages received
 *  @li long           ru_nsignals - signals received
 *  @li long           ru_nvcsw    - voluntary context switches
 *  @li long           ru_nivcsw   - involuntary context switches
 *
 */
static scorep_rusage_metric scorep_rusage_metrics[ SCOREP_RUSAGE_CNTR_MAXNUM ] =
{
    { RU_UTIME,    "ru_utime",    "s",     "user CPU time used",               SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, -6    },
    { RU_STIME,    "ru_stime",    "s",     "system CPU time used",             SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, -6    },
    { RU_MAXRSS,   "ru_maxrss",   "Bytes", "maximum resident set size",        SCOREP_METRIC_MODE_ABSOLUTE_NEXT,       SCOREP_METRIC_BASE_BINARY,  10    },
    { RU_IXRSS,    "ru_ixrss",    "Bytes", "integral shared memory size",      SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_BINARY,  10    },
    { RU_IDRSS,    "ru_idrss",    "Bytes", "integral unshared data size",      SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_BINARY,  10    },
    { RU_ISRSS,    "ru_isrss",    "Bytes", "integral unshared stack size",     SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_BINARY,  10    },
    { RU_MINFLT,   "ru_minflt",   "#",     "page reclaims (soft page faults)", SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_MAJFLT,   "ru_majflt",   "#",     "page faults (hard page faults)",   SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_NSWAP,    "ru_nswap",    "#",     "number of swaps",                  SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_INBLOCK,  "ru_inblock",  "#",     "block input operations",           SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_OUBLOCK,  "ru_oublock",  "#",     "block output operations",          SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_MSGSND,   "ru_msgsnd",   "#",     "IPC messages sent",                SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_MSGRCV,   "ru_msgrcv",   "#",     "IPC messages received",            SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_NSIGNALS, "ru_nsignals", "#",     "signals received",                 SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_NVCSW,    "ru_nvcsw",    "#",     "voluntary context switches",       SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_NIVCSW,   "ru_nivcsw",   "#",     "involuntary context switches",     SCOREP_METRIC_MODE_ACCUMULATED_START,   SCOREP_METRIC_BASE_DECIMAL, 0     }
};

/* *INDENT-ON* */

/** Metric definition data */
typedef struct scorep_metric_definition_data scorep_metric_definition_data;
struct scorep_metric_definition_data
{
    /** Vector of active resource usage counters */
    scorep_rusage_metric* active_metrics[ SCOREP_RUSAGE_CNTR_MAXNUM ];
    /** Number of active resource usage counters */
    int8_t                number_of_metrics;
};

/** Metric data structure */
struct SCOREP_Metric_EventSet
{
    /** Structure to store resource usage metric values */
    struct rusage                  ru;
    /** Metric definition data */
    scorep_metric_definition_data* definitions;
};

/** Definition data of metrics for each kind of metrics */
static scorep_metric_definition_data* metric_defs[ NUMBER_OF_RESERVED_METRICS ];


/** @brief Reads the configuration from environment variables and configuration
 *         files and initializes the resource usage counter source. It must be
 *         called before other functions depending on this metric source are
 *         used by the measurement system.
 *
 *  @param listOfMetricNames        Content of environment variable specifying
 *                                  requested resource usage metric names.
 *  @param metricsSeparator         Character separating entries in list of
 *                                  metric names.
 *
 *  @return Returns definition data of specified metrics.
 */
static scorep_metric_definition_data*
scorep_metric_rusage_open( const char* listOfMetricNames,
                           const char* metricsSeparator )
{
    /* Working copy of environment variable content */
    char* env_metrics;
    /* Pointer to single character of metric specification string */
    char* token;

    /* Read content of environment variable */
    env_metrics = UTILS_CStr_dup( listOfMetricNames );

    /* Return if environment variable is empty */
    if ( strlen( env_metrics ) == 0 )
    {
        free( env_metrics );
        return NULL;
    }

    /* Create new event set (variables initialized with zero) */
    scorep_metric_definition_data* metric_defs = calloc( 1, sizeof( scorep_metric_definition_data ) );
    UTILS_ASSERT( metric_defs );

    /* Convert RUSAGE's letters to lower case */
    token = env_metrics;
    while ( *token )
    {
        *token = tolower( *token );
        token++;
    }

    if ( strcmp( env_metrics, "all" ) == 0 )
    {
        /* Add all resource usage counters */
        for ( uint32_t i = 0; i < SCOREP_RUSAGE_CNTR_MAXNUM; i++ )
        {
            metric_defs->active_metrics[ ( metric_defs->number_of_metrics )++ ] = &( scorep_rusage_metrics[ i ] );
        }
    }
    else
    {
        scorep_rusage_metric_type index;

        /* Read resource usage counter from specification string */
        token = strtok( env_metrics, metricsSeparator );
        while ( token )
        {
            index = SCOREP_RUSAGE_CNTR_MAXNUM;

            /* Check if we exceed maximum number of concurrently used resource usage metrics */
            UTILS_ASSERT( metric_defs->number_of_metrics < SCOREP_RUSAGE_CNTR_MAXNUM );

            /* Search counter name in vector of counter specifications */
            for ( int i = 0; i < SCOREP_RUSAGE_CNTR_MAXNUM; i++ )
            {
                if ( strcmp( scorep_rusage_metrics[ i ].name, token ) == 0 )
                {
                    index = scorep_rusage_metrics[ i ].index;
                }
            }

            /* If found, add the address of this counter specification to vector
             * of active counters; otherwise abort */
            UTILS_ASSERT( index != SCOREP_RUSAGE_CNTR_MAXNUM );
            metric_defs->active_metrics[ ( metric_defs->number_of_metrics )++ ] = &( scorep_rusage_metrics[ index ] );

            /* Get next token */
            token = strtok( NULL, metricsSeparator );
        }
    }

    free( env_metrics );

    return metric_defs;
}


/* *********************************************************************
 * Metric source management
 **********************************************************************/

/** @brief  Registers configuration variables for the metric sources.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
scorep_metric_rusage_register()
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " register rusage metric source!" );

    SCOREP_ErrorCode status;

    /* Register environment variables for 'synchronous strict' and 'per-process' metrics */
    status = SCOREP_ConfigRegister( "metric", scorep_metric_rusage_configs );
    if ( status != SCOREP_SUCCESS )
    {
        UTILS_WARNING( "Registration of RUSAGE configuration variables failed." );
    }

    return status;
}

/** @brief Called on deregistration of the metric source.
 */
static void
scorep_metric_rusage_deregister()
{
    /* Free environment variables for 'synchronous strict' and per-process metrics */
    free( scorep_metrics_rusage );
    free( scorep_metrics_rusage_per_process );
    free( scorep_metrics_rusage_separator );

    /* Reset array of pointers to metric definition data */
    for ( uint32_t i = 0; i < NUMBER_OF_RESERVED_METRICS; i++ )
    {
        metric_defs[ i ] = NULL;
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " rusage metric source deregister!" );
}

/** @brief  Initialize 'resource usage' metric source.
 *
 *  During initialization respective environment variables are read to
 *  determine which metrics has been specified by the user.
 *
 *  Because 'synchronous strict' metrics will be recorded by all
 *  locations, we known how many metrics of this specific type each
 *  location will record. This number is returned by this function.
 *
 *  For metrics of other types (e.g. per-process) we can not determine
 *  whether this location will be responsible to record metrics of this
 *  type. Responsibility will be determine while initializing location.
 *  Therefore, within this function we don't known how many additional
 *  metrics will be recorded by a specific location.
 *
 *  @return Returns the number of used 'synchronous strict' metrics.
 */
static uint32_t
scorep_metric_rusage_initialize_source()
{
    /* Number of used 'synchronous strict' metrics */
    uint32_t metric_counts = 0;

    if ( scorep_metric_rusage_initialize )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialize rusage metric source source." );

        /* FIRST: Read specification of global synchronous strict metrics from respective environment variable. */
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "[RUSAGE] global synchronous strict metrics = %s", scorep_metrics_rusage );
        metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ] =
            scorep_metric_rusage_open( scorep_metrics_rusage, scorep_metrics_rusage_separator );
        if ( metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ] != NULL )
        {
            metric_counts = metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ]->number_of_metrics;
        }

        /*
         * Handle additional metric types (e.g. per-process)
         */

        /* SECOND: Read specification of per-process metrics from respective environment variable. */
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "[RUSAGE] per-process metrics = %s", scorep_metrics_rusage_per_process );
        metric_defs[ PER_PROCESS_METRICS_INDEX ] =
            scorep_metric_rusage_open( scorep_metrics_rusage_per_process, scorep_metrics_rusage_separator );

        /* Set flag */
        scorep_metric_rusage_initialize = 0;

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialization of rusage metric source done." );
    }

    return metric_counts;
}

/** @brief Metric source finalization.
 */
static void
scorep_metric_rusage_finalize_source()
{
    /* Call only, if previously initialized */
    if ( !scorep_metric_rusage_initialize )
    {
        if ( metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ] != NULL )
        {
            /* Reset vector of active metrics */
            for ( uint32_t i = 0; i < SCOREP_RUSAGE_CNTR_MAXNUM; i++ )
            {
                metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ]->active_metrics[ i ] = NULL;
            }
            /* Reset number of active rusage metrics */
            metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ]->number_of_metrics = 0;
        }

        /* Set initialization flag */
        scorep_metric_rusage_initialize = 1;
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " finalize rusage metric source." );
    }
}

/** @brief  Location specific initialization function for metric sources.
 *
 *  @return Returns a reference to management data used to handle set of active metrics.
 */
static SCOREP_Metric_EventSet**
scorep_metric_rusage_initialize_location( SCOREP_Location* locationData )
{
    /* Collection of event sets for each metric scope (global synchr. strict,
     * per-process, <per-system-tree-level> */
    SCOREP_Metric_EventSet** event_set_collection;

    /* Allocate memory for event sets of additional metrics and another
     * two ones (global synchronous strict and per-process metrics) */
    /* @todo: replace malloc by static var */
    event_set_collection = calloc( NUMBER_OF_RESERVED_METRICS, sizeof( SCOREP_Metric_EventSet* ) );
    UTILS_ASSERT( event_set_collection );

    /*
     * First: Check whether this location has to record global synchronous strict metrics
     */
    if ( metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ] != NULL )
    {
        SCOREP_Metric_EventSet* synchronous_strict_metric_event_set = malloc( sizeof( struct SCOREP_Metric_EventSet ) );
        UTILS_ASSERT( synchronous_strict_metric_event_set );

        synchronous_strict_metric_event_set->definitions = metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ];

        event_set_collection[ SYNCHRONOUS_STRICT_METRICS_INDEX ] = synchronous_strict_metric_event_set;
    }

    /*
     * Handle different kinds of per-location metrics
     *
     * Second: Check whether this location has to record per-process metrics
     */
    if ( metric_defs[ PER_PROCESS_METRICS_INDEX ] != NULL       // user has defined per-process metrics
         && SCOREP_Location_GetId( locationData ) == 0 )        // this location is responsible to record per-process metrics (e.g. first thread of process)
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "[RUSAGE] This location will record per-process metrics." );

        SCOREP_Metric_EventSet* per_process_metric_event_set = malloc( sizeof( SCOREP_Metric_EventSet ) );
        UTILS_ASSERT( per_process_metric_event_set );

        per_process_metric_event_set->definitions = metric_defs[ PER_PROCESS_METRICS_INDEX ];

        event_set_collection[ PER_PROCESS_METRICS_INDEX ] = per_process_metric_event_set;
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric source initialized location!" );

    return event_set_collection;
}

/** @brief Location specific finalization function for metric sources.
 *
 *  @param eventSet  Reference to active set of metrics.
 */
static void
scorep_metric_rusage_finalize_location( SCOREP_Metric_EventSet* eventSet )
{
    free( eventSet );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric source finalized location!" );
}

/** @brief Frees memory allocated for requested event set.
 *
 *  @param eventSet  Reference to active set of metrics.
 */
static void
scorep_metric_rusage_free_event_set( SCOREP_Metric_EventSet* eventSet )
{
    free( eventSet );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric source freed event set!" );
}

/** @brief Reads values of all metrics in the active event set.
 *
 *  @param eventSet  Reference to active set of metrics.
 *  @param values    Reference to array that will be filled with values from active metrics.
 */
static void
scorep_metric_rusage_read( SCOREP_Metric_EventSet* eventSet,
                           uint64_t*               values )
{
    UTILS_ASSERT( eventSet );
    UTILS_ASSERT( values );

    /* Get resource usage statistics
     *
     * SCOREP_RUSAGE_SCOPE refers to one of the two modes:
     *  - RUSAGE_THREAD: statistics for calling thread
     *  - RUSAGE_SELF:   statistics for calling process, in multi-threaded applications
     *                   it is the sum of resources used by all threads of calling process
     * Please see configuration output to determine which mode is used by Score-P on your system. */
    int ret = getrusage( SCOREP_RUSAGE_SCOPE, &( eventSet->ru ) );
    UTILS_ASSERT( ret != -1 );

    for ( uint32_t i = 0; i < eventSet->definitions->number_of_metrics; i++ )
    {
        switch ( eventSet->definitions->active_metrics[ i ]->index )
        {
            case RU_UTIME:
                values[ i ] = ( ( uint64_t )eventSet->ru.ru_utime.tv_sec * 1e6 + ( uint64_t )eventSet->ru.ru_utime.tv_usec );
                break;

            case RU_STIME:
                values[ i ] = ( ( uint64_t )eventSet->ru.ru_stime.tv_sec * 1e6 + ( uint64_t )eventSet->ru.ru_stime.tv_usec );
                break;

            case RU_MAXRSS:
                values[ i ] = ( uint64_t )eventSet->ru.ru_maxrss;
                break;

            case RU_IXRSS:
                values[ i ] = ( uint64_t )eventSet->ru.ru_ixrss;
                break;

            case RU_IDRSS:
                values[ i ] = ( uint64_t )eventSet->ru.ru_idrss;
                break;

            case RU_ISRSS:
                values[ i ] = ( uint64_t )eventSet->ru.ru_isrss;
                break;

            case RU_MINFLT:
                values[ i ] = ( uint64_t )eventSet->ru.ru_minflt;
                break;

            case RU_MAJFLT:
                values[ i ] = ( uint64_t )eventSet->ru.ru_majflt;
                break;

            case RU_NSWAP:
                values[ i ] = ( uint64_t )eventSet->ru.ru_nswap;
                break;

            case RU_INBLOCK:
                values[ i ] = ( uint64_t )eventSet->ru.ru_inblock;
                break;

            case RU_OUBLOCK:
                values[ i ] = ( uint64_t )eventSet->ru.ru_oublock;
                break;

            case RU_MSGSND:
                values[ i ] = ( uint64_t )eventSet->ru.ru_msgsnd;
                break;

            case RU_MSGRCV:
                values[ i ] = ( uint64_t )eventSet->ru.ru_msgrcv;
                break;

            case RU_NSIGNALS:
                values[ i ] = ( uint64_t )eventSet->ru.ru_nsignals;
                break;

            case RU_NVCSW:
                values[ i ] = ( uint64_t )eventSet->ru.ru_nvcsw;
                break;

            case RU_NIVCSW:
                values[ i ] = ( uint64_t )eventSet->ru.ru_nivcsw;
                break;
        }
    }
}

/** @brief  Gets number of active metrics.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *
 *  @return Returns number of active metrics.
 */
static uint32_t
scorep_metric_rusage_get_number_of_metrics( SCOREP_Metric_EventSet* eventSet )
{
    if ( eventSet == NULL )
    {
        return 0;
    }

    return eventSet->definitions->number_of_metrics;
}

/** @brief  Returns name of metric @metricIndex.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns name of requested metric.
 */
static const char*
scorep_metric_rusage_get_metric_name( SCOREP_Metric_EventSet* eventSet,
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

/** @brief  Gets description of requested metric.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns description of requested metric.
 */
static const char*
scorep_metric_rusage_get_metric_description( SCOREP_Metric_EventSet* eventSet,
                                             uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    if ( metricIndex < eventSet->definitions->number_of_metrics )
    {
        return eventSet->definitions->active_metrics[ metricIndex ]->description;
    }
    else
    {
        return "";
    }
}

/** @brief  Gets unit of requested metric.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns unit of requested metric.
 */
static const char*
scorep_metric_rusage_get_metric_unit( SCOREP_Metric_EventSet* eventSet,
                                      uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    if ( metricIndex < eventSet->definitions->number_of_metrics )
    {
        return eventSet->definitions->active_metrics[ metricIndex ]->unit;
    }
    else
    {
        return "";
    }
}

/** @brief  Gets properties of requested metric.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns properties of requested metric.
 */
static SCOREP_Metric_Properties
scorep_metric_rusage_get_metric_properties( SCOREP_Metric_EventSet* eventSet,
                                            uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    SCOREP_Metric_Properties props;

    if ( metricIndex < eventSet->definitions->number_of_metrics )
    {
        props.name           = eventSet->definitions->active_metrics[ metricIndex ]->name;
        props.description    = eventSet->definitions->active_metrics[ metricIndex ]->description;
        props.source_type    = SCOREP_METRIC_SOURCE_TYPE_RUSAGE;
        props.mode           = eventSet->definitions->active_metrics[ metricIndex ]->mode;
        props.value_type     = SCOREP_METRIC_VALUE_UINT64;
        props.base           = eventSet->definitions->active_metrics[ metricIndex ]->base;
        props.exponent       = eventSet->definitions->active_metrics[ metricIndex ]->exponent;
        props.unit           = eventSet->definitions->active_metrics[ metricIndex ]->unit;
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

/** @brief  Determines clock rate with the help of PAPI.
 *
 *  @return Returns recent clock rate.
 */
static uint64_t
scorep_metric_rusage_clock_rate()
{
    // Resource usage counters don't provide a clock source.
    return 0;
}

/** Implementation of the metric source initialization/finalization struct */
const SCOREP_MetricSource SCOREP_Metric_Rusage =
{
    SCOREP_METRIC_SOURCE_TYPE_RUSAGE,
    &scorep_metric_rusage_register,
    &scorep_metric_rusage_initialize_source,
    &scorep_metric_rusage_initialize_location,
    &scorep_metric_rusage_free_event_set,
    &scorep_metric_rusage_finalize_location,
    &scorep_metric_rusage_finalize_source,
    &scorep_metric_rusage_deregister,
    &scorep_metric_rusage_read,
    &scorep_metric_rusage_get_number_of_metrics,
    &scorep_metric_rusage_get_metric_name,
    &scorep_metric_rusage_get_metric_description,
    &scorep_metric_rusage_get_metric_unit,
    &scorep_metric_rusage_get_metric_properties,
    &scorep_metric_rusage_clock_rate
};
