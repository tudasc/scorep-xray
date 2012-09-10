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
 *  @file       scorep_metric_papi.c
 *
 *  @author     Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *  @maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *
 *  @brief This module implements PAPI support for Score-P.
 */

#include <config.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SCOREP_Config.h"
#include <SCOREP_RuntimeManagement.h>
#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <UTILS_CStr.h>

#include <SCOREP_Location.h>


#if defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901L ) && !defined( C99 )
#define C99
#endif
#include <papi.h>

#include "SCOREP_Metric_Source.h"

//#if !(defined(HAVE_DECL_LONG_LONG) && HAVE_DECL_LONG_LONG)
//# define long_long long long
//#endif /* HAVE_DECL_LONG_LONG */

#if PAPI_VER_CURRENT >= PAPI_VERSION_NUMBER( 3, 9, 0, 0 )
# define PAPIC
#endif

#if PAPI_VER_CURRENT >= PAPI_VERSION_NUMBER( 5, 0, 0, 0 )
# define PAPIV
#endif

#ifndef TIMER_PAPI_REAL_CYC
# define TIMER_PAPI_REAL_CYC 10
#endif /* TIMER_PAPI_REAL_CYC */
#ifndef TIMER_PAPI_REAL_USEC
# define TIMER_PAPI_REAL_USEC 11
#endif /* TIMER_PAPI_REAL_USEC */

/** @def SCOREP_METRIC_MAXNUM Maximum number of PAPI metrics concurrently used by a process */
#define SCOREP_METRIC_MAXNUM 20

/** @defgroup SCOREP_Metric_PAPI SCOREP PAPI Metric Source
 *  @ingroup SCOREP_Metric
 *
 *  This metric source uses the Performance Application Programming Interface (PAPI) to
 *  access hardware performance counters.
 *
 *  First it is explained how to specify PAPI metrics that will be recorded by every location.
 *
 *  You can enable recording of PAPI performance metrics by setting the environment variable
 *  SCOREP_METRIC_PAPI to a comma-separated list of metric names. Metric names can be any
 *  PAPI preset names or PAPI native counter names. For example, set
 *  @verbatim SCOREP_METRIC_PAPI=PAPI_FP_OPS,PAPI_L2_TCM @endverbatim
 *  to record the number of floating point instructions and level 2 cache misses (both
 *  PAPI preset counters). If any of the requested metrics is not recognized program execution
 *  will be aborted with an error message. The user can leave the environment variable unset to
 *  indicate that no metrics are requested. Use the tools \c papi_avail and \c papi_native_avail
 *  to get a list of available PAPI events. If you want to change the separator used in the
 *  list of PAPI metric names set the environment variable \c SCOREP_METRIC_PAPI_SEP to
 *  the needed character.
 *
 *  In addition it is possible to specify metrics that will be recorded per-process. Please use
 *  \c SCOREP_PAPI_PER_PROCESS for that reason.
 */

/**
 *  Data structure of PAPI counter specification
 *
 *  SCOREP_MetricType, SCOREP_MetricValueType, SCOREP_MetricBase,
 *  and SCOREP_MetricProfilingType are implicit
 */
typedef struct scorep_papi_metric
{
    /** Metric name */
    char*             name;
    /** Longer description for this metric */
    char              description[ PAPI_HUGE_STR_LEN ];
    /** PAPI code of this metric */
    int               papi_code;
    /** Mode of this metric (absolute or accumulated) */
    SCOREP_MetricMode mode;
} scorep_papi_metric;

/**
 *   Metric definition data.
 */
typedef struct scorep_metric_definition_data
{
    /** Vector of active resource usage counters */
    scorep_papi_metric* active_metrics[ SCOREP_METRIC_MAXNUM ];
    /** Number of active resource usage counters */
    int8_t              number_of_metrics;
} scorep_metric_definition_data;

/**
 *  An eventset for each component
 */
typedef struct scorep_event_map
{
    /** Identifier of eventset */
    int       event_id;
    /** Return values for the eventsets */
    long_long values[ SCOREP_METRIC_MAXNUM ];
    /** Number of recorded events in this set */
    int       num_of_events;
    /** Identifier of related PAPI component */
    int       component_id;
} scorep_event_map;

/**
 *  Implementation of Score-P event set data structure specific for the PAPI metric source.
 */
struct SCOREP_Metric_EventSet
{
    /** A struct for each active component */
    scorep_event_map*              event_map[ SCOREP_METRIC_MAXNUM ];
    /** For each counter a pointer, that points to the event sets return values */
    long_long*                     values[ SCOREP_METRIC_MAXNUM ];
    /** Metric definition data */
    scorep_metric_definition_data* definitions;
};

#include "scorep_metric_papi_confvars.inc.c"

/* *********************************************************************
 * Definition of local functions
 **********************************************************************/

static void
scorep_metric_papi_add( char*                          name,
                        int                            code,
                        bool                           isAbsolute,
                        scorep_metric_definition_data* metricDefs );

static void
scorep_metric_papi_test( scorep_metric_definition_data* metricDefs );

static void
scorep_metric_papi_descriptions( scorep_metric_definition_data* metricDefs );

static void
scorep_metric_papi_warning( int   errcode,
                            char* note );

static void
scorep_metric_papi_error( int   errcode,
                          char* note );



/* *********************************************************************
 * Global variables
 **********************************************************************/

/**
 * Definition data of metrics (synchronous strict and per-process metrics).
 */
static scorep_metric_definition_data* metric_defs[ NUMBER_OF_RESERVED_METRICS ];

/**
 * Static variable to control initialization status of the metric adapter.
 * If it is 0, then PAPI metric adapter is initialized.
 */
static int scorep_metric_papi_initialize = 1;



/* *********************************************************************
 * Helper functions
 **********************************************************************/

/** @brief  This function can be used to determine identifier of recent location.
 *
 *  @return It returns the identifier of recent location.
 */
static unsigned long
scorep_metric_get_location_id()
{
    /* Get the thread id from the measurement system */
    SCOREP_Location* data = SCOREP_Location_GetCurrentCPULocation();
    UTILS_ASSERT( data != NULL );

    return SCOREP_Location_GetId( data );
}




/** @brief Reads the configuration from environment variables and configuration
 *         files and initializes the performance counter library. It must be called
 *         before other functions of the library are used by the measurement system.
 *
 *  @param list_of_metric_names     Content of environment variable specifying
 *                                  requested resource usage metric names.
 *  @param metrics_separator        Character separating entries in list of
 *                                  metric names.
 *
 *  @return Returns definition data of specified metrics.
 */
static scorep_metric_definition_data*
scorep_metric_papi_open( const char* listOfMetricNames,
                         const char* metricsSeparator )
{
    /** A leading exclamation mark let the metric be interpreted as absolute value counter.
     *  Appropriate metric properties have to be set. */
    bool is_absolute;
    /** Content of environment variable SCOREP_METRIC_PAPI */
    char* env_metrics;
    /** Individual metric */
    char* token;
    /** PAPI return value. */
    int retval;
    /** Info struct about PAPI event */
    PAPI_event_info_t info;

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

    /* Initialize PAPI */
    retval = PAPI_library_init( PAPI_VER_CURRENT );
    UTILS_ASSERT( retval == PAPI_VER_CURRENT );

    /* PAPI code of recent metric */
    int code;
    /* Metric name */
    char* component;

    for ( uint32_t i = 0; i < list_len; i++ )
    {
        /* Current metric name */
        token = metric_names[ i ];

        if ( token[ 0 ] == '!' )
        {
            /* A leading exclamation mark indicates absolute metrics */
            is_absolute = true;
            token++;
        }
        else
        {
            is_absolute = false;
        }

        /* Try given metric name */
        code      = -1;
        component = token;

        retval = PAPI_event_name_to_code( component, &code );
        assert( retval == PAPI_OK && code != -1 );

        memset( &info, 0, sizeof( PAPI_event_info_t ) );
        retval = PAPI_get_event_info( code, &info );
        assert( retval == PAPI_OK );

        scorep_metric_papi_add( component, code, is_absolute, metric_definition );
    }

    /* Clean up */
    free( env_metrics );
    free( metric_names );

    /* Check whether event combination is valid. This is done here to
     * avoid errors when creating the event set for each thread, which
     * would multiply the error messages. */
    scorep_metric_papi_test( metric_definition );

    scorep_metric_papi_descriptions( metric_definition );

    return metric_definition;

out2:
    free( metric_names );

out:
    free( env_metrics );
    return NULL;
}

/** @brief Finalizes the performance counter adapter. Frees memory allocated by
 *         scorep_metric_papi_open.
 */
static void
scorep_metric_papi_close()
{
    /* PAPI_shutdown() should be called only if there were PAPI metrics */
    bool shutdown_papi = false;

    for ( uint32_t metric_index = 0; metric_index < NUMBER_OF_RESERVED_METRICS; metric_index++ )
    {
        if ( metric_defs[ metric_index ] == NULL
             || metric_defs[ metric_index ]->number_of_metrics == 0 )
        {
            continue;
        }

        shutdown_papi = true;

        for ( uint32_t i = 0; i < metric_defs[ metric_index ]->number_of_metrics; i++ )
        {
            free( metric_defs[ metric_index ]->active_metrics[ i ]->name );
            free( metric_defs[ metric_index ]->active_metrics[ i ] );
        }
        free( metric_defs[ metric_index ] );
        metric_defs[ metric_index ] = NULL;
    }

    if ( shutdown_papi )
    {
        PAPI_shutdown();
    }
}

/** @brief  Creates per-thread counter sets.
 *
 *  @param definitions          Metric definition data.
 *
 *  @return It returns the new event set.
 */
SCOREP_Metric_EventSet*
scorep_metric_papi_create_event_set( scorep_metric_definition_data* definitions )
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
    for ( uint32_t i = 0; i < definitions->number_of_metrics; i++ )
    {
        struct scorep_event_map* eventset;

        #ifdef PAPIC
        component = PAPI_COMPONENT_INDEX( definitions->active_metrics[ i ]->papi_code );
        #else
        component = 0;
        #endif

        /* Search for the eventset that matches the counter */
        j = 0;
        while ( event_set->event_map[ j ] != NULL && j < SCOREP_METRIC_MAXNUM && event_set->event_map[ j ]->component_id != component )
        {
            j++;
        }
        if ( event_set->event_map[ j ] == NULL ) /* No event of this component yet! */
        {
            event_set->event_map[ j ]                = ( struct scorep_event_map* )malloc( sizeof( scorep_event_map ) );
            event_set->event_map[ j ]->event_id      = PAPI_NULL;
            event_set->event_map[ j ]->num_of_events = 0;
            retval                                   = PAPI_create_eventset( &( event_set->event_map[ j ]->event_id ) );
            if ( retval != PAPI_OK )
            {
                scorep_metric_papi_error( retval, "PAPI_create_eventset" );
            }
            event_set->event_map[ j ]->component_id = component;
        }
        eventset = event_set->event_map[ j ];

        /* Add event to event set */
        retval = PAPI_add_event( eventset->event_id, definitions->active_metrics[ i ]->papi_code );
        if ( retval != PAPI_OK )
        {
            scorep_metric_papi_error( retval, "PAPI_add_event" );
        }

        /* For demux the values from eventset -> returnvector */
        event_set->values[ i ] = &( eventset->values[ eventset->num_of_events ] );
        eventset->num_of_events++;
    }

    /* For each used eventset */
    for ( uint32_t i = 0; i < SCOREP_METRIC_MAXNUM && event_set->event_map[ i ] != NULL; i++ )
    {
        /* Only one event set per thread/cpu can be running at any time,
         * so if another event set is running, PAPI_start() will return
         * PAPI_EISRUN.
         *
         * Possible solutions:
         * (1) We must stop the already running event set explicitly.
         * However, multiple event sets will be measured in a multiplexed
         * fashion. We won't be able to measure multiple event sets per
         * thread/cpu for the same time slice. */
        retval = PAPI_start( event_set->event_map[ i ]->event_id );
        if ( retval != PAPI_OK )
        {
            scorep_metric_papi_error( retval, "PAPI_start" );
        }
    }

    return event_set;
}

/** @brief Frees per-thread counter sets.
 *
 *  @param eventSet The event set that defines the measured counters
 *                  which should be freed.
 */
void
scorep_metric_papi_free( SCOREP_Metric_EventSet* eventSet )
{
    int       retval;
    long_long papi_vals[ SCOREP_METRIC_MAXNUM ];

    /*
     * Check for ( eventSet == NULL) can be skipped here, because all functions
     * that call this one should have already checked eventSet.
     */

    /* Treat PAPI failures at this point as non-fatal */

    /*
     * WARNING: PAPI may access prof file system while reading counters.
     * If Score-P provides feature like IO tracing, we must suspend
     * tracing of IO events at this point to avoid recording of events
     * caused by PAPI. Therefore we will need a function (or macro) like
     * SCOREP_SUSPEND_IO_TRACING( CURRENT_THREAD ).
     */

    /* For each used eventset */
    for ( uint32_t i = 0; i < SCOREP_METRIC_MAXNUM && eventSet->event_map[ i ] != NULL; i++ )
    {
        retval = PAPI_stop( eventSet->event_map[ i ]->event_id, papi_vals );
        if ( retval != PAPI_OK )
        {
            scorep_metric_papi_warning( retval, "PAPI_stop" );
        }
        else
        {
            /* Cleanup/destroy require successful PAPI_stop */
            retval = PAPI_cleanup_eventset( eventSet->event_map[ i ]->event_id );
            if ( retval != PAPI_OK )
            {
                scorep_metric_papi_warning( retval, "PAPI_cleanup_eventset" );
            }
            retval = PAPI_destroy_eventset( &eventSet->event_map[ i ]->event_id );
            if ( retval != PAPI_OK )
            {
                scorep_metric_papi_warning( retval, "PAPI_destroy_eventset" );
            }
        }
        free( eventSet->event_map[ i ] );
    }

    /*
     * The issue of PAPI and IO tracing was explained above:
     * At this point we can resume IO tracing. Therefore a function or
     * macro like SCOREP_RESUME_IO_TRACING( CURRENT_THREAD ) would be
     * required.
     */

    free( eventSet );
}

/** @brief Adds a new metric to internally managed vector.
 *
 *  @param name                 Name of recent metric.
 *  @param code                 PAPI code of recent metric.
 *  @param isAbsolute           Is true if this metric should be interpreted as absolute value.
 *                              Otherwise it is false.
 *  @param metricDefinition     Reference to metric definition data structure.
 */
static void
scorep_metric_papi_add( char*                          name,
                        int                            code,
                        bool                           isAbsolute,
                        scorep_metric_definition_data* metricDefinition )
{
    UTILS_ASSERT( metricDefinition );

    if ( metricDefinition->number_of_metrics >= SCOREP_METRIC_MAXNUM )
    {
        UTILS_ERROR( SCOREP_ERROR_PAPI_INIT, "Number of counters exceeds Score-P allowed maximum of %d", SCOREP_METRIC_MAXNUM );
    }
    else
    {
        metricDefinition->active_metrics[ metricDefinition->number_of_metrics ]                   = ( scorep_papi_metric* )malloc( sizeof( scorep_papi_metric ) );
        metricDefinition->active_metrics[ metricDefinition->number_of_metrics ]->name             = UTILS_CStr_dup( name );
        metricDefinition->active_metrics[ metricDefinition->number_of_metrics ]->description[ 0 ] = '\0';
        if ( isAbsolute )
        {
            metricDefinition->active_metrics[ metricDefinition->number_of_metrics ]->mode = SCOREP_METRIC_MODE_ABSOLUTE_NEXT;
        }
        else
        {
            metricDefinition->active_metrics[ metricDefinition->number_of_metrics ]->mode = SCOREP_METRIC_MODE_ACCUMULATED_START;
        }
        metricDefinition->active_metrics[ metricDefinition->number_of_metrics ]->papi_code = code;
        ( metricDefinition->number_of_metrics )++;
    }
}

/** @brief Prints a PAPI-specific error message.
 *
 *  @param errcode PAPI error code
 *  @param note    Additonal information.
 */
static void
scorep_metric_papi_error( int   errcode,
                          char* note )
{
    char errstring[ PAPI_MAX_STR_LEN ];

#ifdef PAPIV
    PAPI_perror( errstring );
#else
    PAPI_perror( errcode, errstring, PAPI_MAX_STR_LEN );
#endif
    if ( errcode == PAPI_ESYS )
    {
        strncat( errstring, ": ", PAPI_MAX_STR_LEN - strlen( errstring ) - 1 );
        strncat( errstring, strerror( errno ), PAPI_MAX_STR_LEN - strlen( errstring ) - 1 );
    }
    UTILS_ERROR( SCOREP_ERROR_PAPI_INIT, "%s: %s (fatal)\n", note ? note : "PAPI", errstring );

    _Exit( EXIT_FAILURE );
}

/** @brief Prints a PAPI-specific warning message.
 *
 *  @param errcode PAPI error code.
 *  @param note    Additonal information.
 */
static void
scorep_metric_papi_warning( int   errcode,
                            char* note )
{
    char errstring[ PAPI_MAX_STR_LEN ];

#ifdef PAPIV
    PAPI_perror( errstring );
#else
    PAPI_perror( errcode, errstring, PAPI_MAX_STR_LEN );
#endif

    if ( errcode == PAPI_ESYS )
    {
        strncat( errstring, ": ", PAPI_MAX_STR_LEN - strlen( errstring ) - 1 );
        strncat( errstring, strerror( errno ), PAPI_MAX_STR_LEN - strlen( errstring ) - 1 );
    }
    UTILS_WARNING( "%s: %s (ignored)\n", note ? note : "PAPI", errstring );
}

/** @brief Prints metric descriptions.
 *
 *  @param metricDefinition     Reference to metric definition data structure.
 */
static void
scorep_metric_papi_descriptions( scorep_metric_definition_data* metricDefinition )
{
    UTILS_ASSERT( metricDefinition );

    int               j;
    int               retval;
    PAPI_event_info_t info;

    for ( int8_t i = 0; i < metricDefinition->number_of_metrics; i++ )
    {
        memset( &info, 0, sizeof( PAPI_event_info_t ) );
        retval = PAPI_get_event_info( metricDefinition->active_metrics[ i ]->papi_code, &info );
        if ( retval != PAPI_OK )
        {
            scorep_metric_papi_error( retval, "PAPI_get_event_info" );
        }

        if ( strcmp( info.long_descr, metricDefinition->active_metrics[ i ]->name ) != 0 )
        {
            strncpy( metricDefinition->active_metrics[ i ]->description, info.long_descr, sizeof( metricDefinition->active_metrics[ i ]->description ) - 1 );

            /* Tidy description if necessary */
            j = strlen( metricDefinition->active_metrics[ i ]->description ) - 1;
            if ( metricDefinition->active_metrics[ i ]->description[ j ] == '\n' )
            {
                metricDefinition->active_metrics[ i ]->description[ j ] = '\0';
            }
            j = strlen( metricDefinition->active_metrics[ i ]->description ) - 1;
            if ( metricDefinition->active_metrics[ i ]->description[ j ] != '.' )
            {
                strncat( metricDefinition->active_metrics[ i ]->description, ".", sizeof( metricDefinition->active_metrics[ i ]->description ) - strlen( metricDefinition->active_metrics[ i ]->description ) - 1 );
            }
        }

        if ( metricDefinition->active_metrics[ i ]->papi_code & PAPI_PRESET_MASK )
        {
            /* PAPI preset */

            char* postfix_chp = info.postfix;
            char  derive_ch   = strcmp( info.derived, "DERIVED_SUB" ) ? '+' : '-';

            strncat( metricDefinition->active_metrics[ i ]->description, " [ ", sizeof( metricDefinition->active_metrics[ i ]->description ) - strlen( metricDefinition->active_metrics[ i ]->description ) - 1 );
            strncat( metricDefinition->active_metrics[ i ]->description, info.name[ 0 ], sizeof( metricDefinition->active_metrics[ i ]->description ) - strlen( metricDefinition->active_metrics[ i ]->description ) - 1 );

            for ( int k = 1; k < ( int )info.count; k++ )
            {
                char op[ 4 ];
                postfix_chp = postfix_chp ? strpbrk( ++postfix_chp, "+-*/" ) : NULL;
                sprintf( op, " %c ", ( postfix_chp ? *postfix_chp : derive_ch ) );
                strncat( metricDefinition->active_metrics[ i ]->description, op, sizeof( metricDefinition->active_metrics[ i ]->description ) - strlen( metricDefinition->active_metrics[ i ]->description ) - 1 );
                strncat( metricDefinition->active_metrics[ i ]->description, info.name[ k ], sizeof( metricDefinition->active_metrics[ i ]->description ) - strlen( metricDefinition->active_metrics[ i ]->description ) - 1 );
            }
            strncat( metricDefinition->active_metrics[ i ]->description, " ]", sizeof( metricDefinition->active_metrics[ i ]->description ) - strlen( metricDefinition->active_metrics[ i ]->description ) - 1 );
            if ( strcmp( info.symbol, metricDefinition->active_metrics[ i ]->name ) != 0 ) /* add preset name */
            {
                strncat( metricDefinition->active_metrics[ i ]->description, " = ", sizeof( metricDefinition->active_metrics[ i ]->description ) - strlen( metricDefinition->active_metrics[ i ]->description ) - 1 );
                strncat( metricDefinition->active_metrics[ i ]->description, info.symbol, sizeof( metricDefinition->active_metrics[ i ]->description ) - strlen( metricDefinition->active_metrics[ i ]->description ) - 1 );
            }
        }
    }
}

/** @brief Tests whether requested event combination is valid or not.
 *
 *  @param metricDefinition     Reference to metric definition data structure.
 */
static void
scorep_metric_papi_test( scorep_metric_definition_data* metricDefinition )
{
    uint32_t                 i;
    uint32_t                 j;
    int                      retval;
    int                      component;
    struct scorep_event_map* event_set[ SCOREP_METRIC_MAXNUM ];

    for ( i = 0; i < SCOREP_METRIC_MAXNUM; i++ )
    {
        event_set[ i ] = NULL;
    }

    for ( i = 0; i < metricDefinition->number_of_metrics; i++ )
    {
        #ifdef PAPIC
        /* Preset-counter belong to Component 0! */
        component = PAPI_COMPONENT_INDEX( metricDefinition->active_metrics[ i ]->papi_code );
        #else
        component = 0;
        #endif

        /* Search for the eventset that matches the counter */
        j = 0;
        while ( event_set[ j ] != NULL && j < SCOREP_METRIC_MAXNUM && event_set[ j ]->component_id != component )
        {
            j++;
        }

        /* Create eventset, if no matching found */
        if ( event_set[ j ] == NULL )
        {
            event_set[ j ]           = malloc( sizeof( scorep_event_map ) );
            event_set[ j ]->event_id = PAPI_NULL;
            retval                   = PAPI_create_eventset( &( event_set[ j ]->event_id ) );
            if ( retval != PAPI_OK )
            {
                scorep_metric_papi_error( retval, "PAPI_create_eventset" );
            }
            event_set[ j ]->component_id = component;
        }

        /* Add event to event set */
        retval = PAPI_add_event( event_set[ j ]->event_id, metricDefinition->active_metrics[ i ]->papi_code );
        if ( retval != PAPI_OK )
        {
            char errstring[ PAPI_MAX_STR_LEN ];
            sprintf( errstring, "PAPI_add_event(%d:\"%s\")", i, metricDefinition->active_metrics[ i ]->name );
            scorep_metric_papi_error( retval, errstring );
        }

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Event %s added to event set", metricDefinition->active_metrics[ i ]->name );
    }

    /* For each used eventset */
    for ( i = 0; i < SCOREP_METRIC_MAXNUM && event_set[ i ] != NULL; i++ )
    {
        retval = PAPI_cleanup_eventset( event_set[ i ]->event_id );
        if ( retval != PAPI_OK )
        {
            scorep_metric_papi_error( retval, "PAPI_cleanup_eventset" );
        }

        retval = PAPI_destroy_eventset( &( event_set[ i ]->event_id ) );
        if ( retval != PAPI_OK )
        {
            scorep_metric_papi_error( retval, "PAPI_destroy_eventset" );
        }
        free( event_set[ i ] );
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Event set tested OK" );
}


/* *********************************************************************
 * Adapter management
 **********************************************************************/

/** @brief  Registers configuration variables for the metric adapters.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
scorep_metric_papi_register()
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " register PAPI metric source!" );

    /* Register environment variables for 'synchronous strict' and 'per-process' metrics */
    SCOREP_ErrorCode status;
    status = SCOREP_ConfigRegister( "metric", scorep_metric_papi_configs );
    if ( status != SCOREP_SUCCESS )
    {
        UTILS_ERROR( SCOREP_ERROR_PAPI_INIT, "Registration of PAPI configure variables failed." );
        return status;
    }

    return SCOREP_SUCCESS;
}

/** @brief Called on deregistration of the metric adapter. Currently, no action is performed
 *         on deregistration.
 */
static void
scorep_metric_papi_deregister()
{
    /* Free environment variables for 'synchronous strict' and per-process metrics */
    free( scorep_metrics_papi );
    free( scorep_metrics_papi_per_process );
    free( scorep_metrics_papi_separator );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " PAPI metric source deregister!" );
}

/** @brief  Initializes the PAPI metric adapter.
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
 *  @return It returns the number of used 'synchronous strict' metrics.
 */
static uint32_t
scorep_metric_papi_initialize_source()
{
    /* Number of used 'synchronous strict' metrics */
    uint32_t metric_counts = 0;

    if ( scorep_metric_papi_initialize )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialize PAPI metric source." );

        /* FIRST: Read specification of global synchronous strict metrics from respective environment variable. */
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "[PAPI] global synchronous strict metrics = %s", scorep_metrics_papi );

        metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ] =
            scorep_metric_papi_open( scorep_metrics_papi, scorep_metrics_papi_separator );
        if ( metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ] != NULL )
        {
            metric_counts = metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ]->number_of_metrics;
        }

        /*
         * Handle additional metric types (e.g. per-process)
         */

        /* SECOND: Read specification of per-process metrics from respective environment variable. */
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "[PAPI] per-process metrics = %s", scorep_metrics_papi_per_process );

        metric_defs[ PER_PROCESS_METRICS_INDEX ] =
            scorep_metric_papi_open( scorep_metrics_papi_per_process, scorep_metrics_papi_separator );

        /* Set flag */
        scorep_metric_papi_initialize = 0;

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialization of PAPI metric source done." );
    }

    return metric_counts;
}

/** @brief Adapter finalization.
 */
static void
scorep_metric_papi_finalize_source()
{
    /* Call only, if previously initialized */
    if ( !scorep_metric_papi_initialize )
    {
        scorep_metric_papi_close();

        /* Set initialization flag */
        scorep_metric_papi_initialize = 1;
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " finalize PAPI metric source." );
    }
}

/** @brief  Location specific initialization function for metric adapters.
 *
 *  @return It returns the event set used by this location.
 */
static SCOREP_Metric_EventSet**
scorep_metric_papi_initialize_location( SCOREP_Location* locationData )
{
    /* Set to true after PAPI_thread_init() was called for this location */
    bool is_papi_thread_initialized = false;

    /* Return value of calls to PAPI functions */
    int retval;

    /* Collection of event sets for each metric scope
     * (synchronous strict, per-process) */
    // @todo Remove alloc by array decl
    SCOREP_Metric_EventSet** event_set_collection;
    event_set_collection = calloc( NUMBER_OF_RESERVED_METRICS, sizeof( SCOREP_Metric_EventSet* ) );
    UTILS_ASSERT( event_set_collection );

    /*
     * First: Check whether this location has to record global synchronous strict metrics
     */
    if ( metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ] != NULL )
    {
        retval = PAPI_thread_init( &scorep_metric_get_location_id );
        if ( retval != PAPI_OK )
        {
            scorep_metric_papi_error( retval, "PAPI_thread_init" );
        }
        is_papi_thread_initialized = true;

        event_set_collection[ SYNCHRONOUS_STRICT_METRICS_INDEX ] = scorep_metric_papi_create_event_set( metric_defs[ SYNCHRONOUS_STRICT_METRICS_INDEX ] );
    }

    /*
     * Handle different kinds of per-location metrics
     *
     * Second: Check whether this location has to record per-process metrics
     */
    if ( metric_defs[ PER_PROCESS_METRICS_INDEX ] != NULL       // user has defined per-process metrics
         && SCOREP_Location_GetId( locationData ) == 0 )        // this location is responsible to record per-process metrics (e.g. first thread of process)
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "[PAPI] This location will record per-process metrics." );

        /* Call PAPI_thread_init() once only */
        if ( !is_papi_thread_initialized )
        {
            retval = PAPI_thread_init( &scorep_metric_get_location_id );
            if ( retval != PAPI_OK )
            {
                scorep_metric_papi_error( retval, "PAPI_thread_init" );
            }
            is_papi_thread_initialized = true;
        }

        event_set_collection[ PER_PROCESS_METRICS_INDEX ] = scorep_metric_papi_create_event_set( metric_defs[ PER_PROCESS_METRICS_INDEX ] );
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "PAPI thread support initialized" );

    return event_set_collection;
}

/** @brief Location specific finalization function for metric adapters.
 *
 *  @param eventSet             Event set to close (may be NULL).
 */
static void
scorep_metric_papi_finalize_location( SCOREP_Metric_EventSet* eventSet )
{
    if ( eventSet == NULL )
    {
        return;
    }

    scorep_metric_papi_free( eventSet );

    /* Ignore return value */
    ( void )PAPI_unregister_thread();

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " PAPI metric source finalize location!" );
}

/** @brief Reads values of counters relative to the time of scorep_metric_papi_open().
 *
 *  @param eventSet An event set, that contains the definition of the counters
 *                  that should be measured.
 *  @param values   An array, to which the counter values are written.
 */
static void
scorep_metric_papi_read( SCOREP_Metric_EventSet* eventSet,
                         uint64_t*               values )
{
    UTILS_ASSERT( eventSet );
    UTILS_ASSERT( values );

    int retval;
    int i;

    /*
     * WARNING: PAPI may access prof file system while reading counters.
     * If Score-P provides feature like IO tracing, we must suspend
     * tracing of IO events at this point to avoid recording of events
     * caused by PAPI. Therefore we will need a function (or macro) like
     * SCOREP_SUSPEND_IO_TRACING( CURRENT_THREAD ).
     */

    /* For each used eventset */
    for ( uint32_t i = 0; i < SCOREP_METRIC_MAXNUM && eventSet->event_map[ i ] != NULL; i++ )
    {
        retval = PAPI_read( eventSet->event_map[ i ]->event_id, eventSet->event_map[ i ]->values );
        if ( retval != PAPI_OK )
        {
            scorep_metric_papi_error( retval, "PAPI_read" );
        }
    }

    for ( uint32_t i = 0; i < eventSet->definitions->number_of_metrics; i++ )
    {
        values[ i ] = ( uint64_t )*eventSet->values[ i ];
    }

    /*
     * The issue of PAPI and IO tracing was explained above:
     * At this point we can resume IO tracing. Therefore a function or
     * macro like SCOREP_RESUME_IO_TRACING( CURRENT_THREAD ) would be
     * required.
     */
}

/** @brief  Returns the number of recently used metrics.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *
 *  @return It returns the number of recently used metrics.
 */
static uint32_t
scorep_metric_papi_get_number_of_metrics( SCOREP_Metric_EventSet* eventSet )
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
scorep_metric_papi_get_metric_name( SCOREP_Metric_EventSet* eventSet,
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
scorep_metric_papi_get_metric_description( SCOREP_Metric_EventSet* eventSet,
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

/** @brief  Returns a string containing a representation of the unit of requested metric.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns a string containing a representation of the unit of requested metric.
 */
static const char*
scorep_metric_papi_get_metric_unit( SCOREP_Metric_EventSet* eventSet,
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
scorep_metric_papi_get_metric_properties( SCOREP_Metric_EventSet* eventSet,
                                          uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    SCOREP_Metric_Properties props;

    if ( metricIndex < eventSet->definitions->number_of_metrics )
    {
        props.name           = eventSet->definitions->active_metrics[ metricIndex ]->name;
        props.description    = eventSet->definitions->active_metrics[ metricIndex ]->description;
        props.source_type    = SCOREP_METRIC_SOURCE_TYPE_PAPI;
        props.mode           = eventSet->definitions->active_metrics[ metricIndex ]->mode;
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


/** @brief  Returns the clock rate.
 *
 *  @return It returns the clock rate.
 */
static uint64_t
scorep_metric_papi_clock_rate()
{
    double hertz = 0;

    #if TIMER == TIMER_PAPI_REAL_CYC
    const PAPI_hw_info_t* hwinfo = NULL;

    if ( !PAPI_is_initialized() )
    {
        /* Initialize PAPI, since it hasn't already been initialized */
        int retval = PAPI_library_init( PAPI_VER_CURRENT );
        if ( retval != PAPI_VER_CURRENT )
        {
            scorep_metric_papi_error( retval, "PAPI_library_init" );
        }
    }

    hwinfo = PAPI_get_hardware_info();
    if ( hwinfo == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_PAPI_INIT, "Failed to access PAPI hardware info\n" );
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Clock rate: %f MHz", hwinfo->mhz );

    hertz = hwinfo->mhz * 1000000.0;
    #endif /* TIMER */

    return ( uint64_t )hertz;
}


/**
 * Implementation of the metric source initialization/finalization data structure
 */
const SCOREP_MetricSource SCOREP_Metric_Papi =
{
    SCOREP_METRIC_SOURCE_TYPE_PAPI,
    &scorep_metric_papi_register,
    &scorep_metric_papi_initialize_source,
    &scorep_metric_papi_initialize_location,
    &scorep_metric_papi_free,
    &scorep_metric_papi_finalize_location,
    &scorep_metric_papi_finalize_source,
    &scorep_metric_papi_deregister,
    &scorep_metric_papi_read,
    &scorep_metric_papi_get_number_of_metrics,
    &scorep_metric_papi_get_metric_name,
    &scorep_metric_papi_get_metric_description,
    &scorep_metric_papi_get_metric_unit,
    &scorep_metric_papi_get_metric_properties,
    &scorep_metric_papi_clock_rate
};
