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
 *  @status alpha
 *  @file scorep_metric_rusage.c
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
#include <sys/time.h>
#include <sys/resource.h>

#include "SCOREP_Config.h"
#include <scorep_utility/SCOREP_Utils.h>

#include "SCOREP_Metric_Source.h"

/** @def SCOREP_RUSAGE_CNTR_MAXNUM Maximum number of resource usage counters concurrently used by a process */
#define SCOREP_RUSAGE_CNTR_MAXNUM 16

/** @defgroup SCOREP_Metric_RUSAGE Resource Usage Metric Source
 *  @ingroup SCOREP_Metric
 *
 *  This metric source uses the Unix system call \c getrusage to provide information
 *  about consumed resources and operating system events of processes such as
 *  user/system time, received signals, and number of page faults. The manual page
 *  of \c getrusage provides a list of resource usage metrics. Please note that the
 *  availability of specific metrics depends on the operating system.
 *
 *  You can enable recording of resource usage metrics by setting the environment variable
 *  SCOREP_METRIC_RUSAGE. The variable should contain a comma-separated list of metric names.
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

    RU_INVALID
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

/** Contains the name of requested metrics. */
char* scorep_metrics_rusage = NULL;

/** Contains the separator of metric names. */
char* scorep_metrics_rusage_separator = NULL;

/** Configuration variables for the resource usage metric source.
 *  Current configuration variables are:
 *  @li @c SCOREP_METRIC_RUSAGE list of requested metric names.
 *  @li @c SCOREP_METRIC_RUSAGE_SEP character that separates single metric names.
 */
SCOREP_ConfigVariable scorep_metric_rusage_configs[] = {
    {
        "rusage",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_rusage,
        NULL,
        "",
        "Resource ussage metric names.",
        "List of requested resource usage metric names that will be collected during program run.\n"
    },
    {
        "rusage_sep",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_rusage_separator,
        NULL,
        ",",
        "Separator of resource usage metric names.",
        "Character that separates resource ussage metric names in RUSAGE.\n"
    },
    SCOREP_CONFIG_TERMINATOR
};

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
    { RU_UTIME,    "ru_utime",    "s",     "user CPU time used",               SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, -6    },
    { RU_STIME,    "ru_stime",    "s",     "system CPU time used",             SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, -6    },
    { RU_MAXRSS,   "ru_maxrss",   "Bytes", "maximum resident set size",        SCOREP_METRIC_MODE_ABSOLUTE,    SCOREP_METRIC_BASE_BINARY,  10    },
    { RU_IXRSS,    "ru_ixrss",    "Bytes", "integral shared memory size",      SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_BINARY,  10    },
    { RU_IDRSS,    "ru_idrss",    "Bytes", "integral unshared data size",      SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_BINARY,  10    },
    { RU_ISRSS,    "ru_isrss",    "Bytes", "integral unshared stack size",     SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_BINARY,  10    },
    { RU_MINFLT,   "ru_minflt",   "#",     "page reclaims (soft page faults)", SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_MAJFLT,   "ru_majflt",   "#",     "page faults (hard page faults)",   SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_NSWAP,    "ru_nswap",    "#",     "number of swaps",                  SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_INBLOCK,  "ru_inblock",  "#",     "block input operations",           SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_OUBLOCK,  "ru_oublock",  "#",     "block output operations",          SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_MSGSND,   "ru_msgsnd",   "#",     "IPC messages sent",                SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_MSGRCV,   "ru_msgrcv",   "#",     "IPC messages received",            SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_NSIGNALS, "ru_nsignals", "#",     "signals received",                 SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_NVCSW,    "ru_nvcsw",    "#",     "voluntary context switches",       SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, 0     },
    { RU_NIVCSW,   "ru_nivcsw",   "#",     "involuntary context switches",     SCOREP_METRIC_MODE_ACCUMULATED, SCOREP_METRIC_BASE_DECIMAL, 0     }
};

/* *INDENT-ON* */

/** Metric data structure */
struct SCOREP_Metric_EventSet
{
    struct rusage ru;
};

/** Vector of active resource usage counters */
static scorep_rusage_metric* scorep_rusage_active_metrics[ SCOREP_RUSAGE_CNTR_MAXNUM ];

/** Number of active resource usage counters */
static int32_t number_of_metrics = 0;


/** @brief Reads the configuration from environment variables and configuration
 *         files and initializes the resource usage counter source. It must be
 *         called before other functions depending on this metric source are
 *         used by the measurement system.
 */
void
scorep_metric_rusage_open()
{
    /** Content of environment variable SCOREP_METRIC_RUSAGE */
    char* env_metrics;
    /** Pointer to sinlge character of metric specification string */
    char* token;

    /* Read content of environment variable "SCOREP_METRIC_RUSAGE" */
    env_metrics = SCOREP_CStr_dup( scorep_metrics_rusage );

    /* Return if environment variable is empty */
    if ( strlen( env_metrics ) == 0 )
    {
        return;
    }
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "SCOREP_METRIC_RUSAGE=%s", env_metrics );

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
            scorep_rusage_active_metrics[ number_of_metrics++ ] = &( scorep_rusage_metrics[ i ] );
        }
    }
    else
    {
        scorep_rusage_metric_type index;

        /* Read resource usage counter from specification string */
        token = strtok( env_metrics, scorep_metrics_rusage_separator );
        while ( token )
        {
            index = RU_INVALID;

            /* Check if we exceed maximum number of concurrently used resource usage metrics */
            assert( number_of_metrics < SCOREP_RUSAGE_CNTR_MAXNUM );

            /* Search counter name in vector of counter specifications */
            for ( int i = 0; i < SCOREP_RUSAGE_CNTR_MAXNUM; i++ )
            {
                if ( strcmp( scorep_rusage_metrics[ i ].name, token ) == 0 )
                {
                    index = ( int )scorep_rusage_metrics[ i ].index;
                }
            }

            /* If found, add the address of this counter specification to vector
             * of active counters; otherwise abort */
            assert( index != RU_INVALID );
            scorep_rusage_active_metrics[ number_of_metrics++ ] = &( scorep_rusage_metrics[ index ] );

            /* Get next token */
            token = strtok( NULL, scorep_metrics_rusage_separator );
        }
    }

    free( env_metrics );
}

/** @brief
 *//*
   void
   rusage_free( struct SCOREP_Metric_EventSet* rusage )
   {
    printf( "rusage_free\n" );

    if ( rusage == NULL )
    {
        return;
    }

    free( rusage );
   }*/


/* *********************************************************************
 * Metric source management
 **********************************************************************/

/** @brief  Registers configuration variables for the metric sources.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
SCOREP_Error_Code
scorep_metric_rusage_register()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " register rusage metric source!" );

    return SCOREP_ConfigRegister( "metric", scorep_metric_rusage_configs );
}

/** @brief Called on deregistration of the metric source.
 */
void
scorep_metric_rusage_deregister()
{
    free( scorep_metrics_rusage );
    free( scorep_metrics_rusage_separator );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " rusage metric source deregister!" );
}

/** @brief  Initialize this metric source.
 *
 *  @return Returns the number of used metrics.
 */
uint32_t
scorep_metric_rusage_initialize_source()
{
    if ( scorep_metric_rusage_initialize )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialize rusage metric source source." );

        /* Read specified metric from respective environment variable.
         * After this call number_of_metrics will be set to the number of
         * metrics specified by the user. */
        scorep_metric_rusage_open();

        /* Set flag */
        scorep_metric_rusage_initialize = 0;

        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialization of rusage metric source done." );
    }

    return number_of_metrics;
}

/** @brief Metric source finalization.
 */
void
scorep_metric_rusage_finalize_source()
{
    /* Call only, if previously initialized */
    if ( !scorep_metric_rusage_initialize )
    {
        /* Reset vector of active metrics */
        for ( uint32_t i = 0; i < SCOREP_RUSAGE_CNTR_MAXNUM; i++ )
        {
            scorep_rusage_active_metrics[ i ] = NULL;
        }
        /* Reset number of active rusage metrics */
        number_of_metrics = 0;

        /* Set initialization flag */
        scorep_metric_rusage_initialize = 1;
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " finalize rusage metric source." );
    }
}

/** @brief  Location specific initialization function for metric sources.
 *
 *  @return Returns a reference to management data used to handle set of active metrics.
 */
SCOREP_Metric_EventSet*
scorep_metric_rusage_initialize_location()
{
    SCOREP_Metric_EventSet* event_set = malloc( sizeof( struct SCOREP_Metric_EventSet ) );
    assert( event_set );

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric source initialized location!" );

    return event_set;
}

/** @brief Location specific finalization function for metric sources.
 *
 *  @param eventSet  Reference to active set of metrics.
 */
void
scorep_metric_rusage_finalize_location( SCOREP_Metric_EventSet* eventSet )
{
    free( eventSet );

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric source finalized location!" );
}

/** @brief Reads values of all metrics in the active event set.
 *
 *  @param eventSet  Reference to active set of metrics.
 *  @param values    Reference to array that will be filled with values from active metrics.
 */
void
scorep_metric_rusage_read( SCOREP_Metric_EventSet* eventSet,
                           uint64_t*               values )
{
    assert( eventSet );
    assert( values );

    /* Get resource usage statistics
     *
     * SCOREP_RUSAGE_SCOPE refers to one of the two modes:
     *  - RUSAGE_THREAD: statistics for calling thread
     *  - RUSAGE_SELF:   statistics for calling process, in multi-threaded applications
     *                   it is the sum of resources used by all threads of calling process
     * Please see configuration output to determine which mode is used by Score-P on your system. */
    int ret = getrusage( SCOREP_RUSAGE_SCOPE, &( eventSet->ru ) );
    assert( ret != -1 );

    for ( uint32_t i = 0; i < number_of_metrics; i++ )
    {
        switch ( scorep_rusage_active_metrics[ i ]->index )
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
 *  @return Returns number of active metrics.
 */
int32_t
scorep_metric_rusage_get_number_of_metrics()
{
    return number_of_metrics;
}

/** @brief  Returns name of metric @metricIndex.
 *
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns name of requested metric.
 */
const char*
scorep_metric_rusage_get_metric_name( uint32_t metricIndex )
{
    if ( metricIndex < number_of_metrics )
    {
        return scorep_rusage_active_metrics[ metricIndex ]->name;
    }
    else
    {
        return "";
    }
}

/** @brief  Gets description of requested metric.
 *
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns description of requested metric.
 */
const char*
scorep_metric_rusage_get_metric_description( uint32_t metricIndex )
{
    if ( metricIndex < number_of_metrics )
    {
        return scorep_rusage_active_metrics[ metricIndex ]->description;
    }
    else
    {
        return "";
    }
}

/** @brief  Gets unit of requested metric.
 *
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns unit of requested metric.
 */
const char*
scorep_metric_rusage_get_metric_unit( uint32_t metricIndex )
{
    if ( metricIndex < number_of_metrics )
    {
        return scorep_rusage_active_metrics[ metricIndex ]->unit;
    }
    else
    {
        return "";
    }
}

/** @brief  Gets properties of requested metric.
 *
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns properties of requested metric.
 */
SCOREP_Metric_Properties
scorep_metric_rusage_get_metric_properties( uint32_t metricIndex )
{
    SCOREP_Metric_Properties props;

    if ( metricIndex < number_of_metrics )
    {
        props.name           = scorep_rusage_active_metrics[ metricIndex ]->name;
        props.description    = scorep_rusage_active_metrics[ metricIndex ]->description;
        props.source_type    = SCOREP_METRIC_SOURCE_TYPE_RUSAGE;
        props.mode           = scorep_rusage_active_metrics[ metricIndex ]->mode;
        props.value_type     = SCOREP_METRIC_VALUE_UINT64;
        props.base           = scorep_rusage_active_metrics[ metricIndex ]->base;
        props.exponent       = scorep_rusage_active_metrics[ metricIndex ]->exponent;
        props.unit           = scorep_rusage_active_metrics[ metricIndex ]->unit;
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
uint64_t
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
