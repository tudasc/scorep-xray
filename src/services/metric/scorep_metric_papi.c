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
 *  @file scorep_metric_papi.c
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
#include <scorep_utility/SCOREP_Debug.h>
#include <scorep_utility/SCOREP_Utils.h>

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
 *  You can enable recording of resource usage metrics by setting the environment variable
 *  SCOREP_METRIC_PAPI to a comma-separated list of metric names. Metric names can be any
 *  PAPI preset names or PAPI native counter names. For example, set
 *  @verbatim SCOREP_METRIC_PAPI=PAPI_FP_OPS,PAPI_L2_TCM @endverbatim
 *  to record the number of floating point instructions and level 2 cache misses (both
 *  PAPI preset counters). The user can leave the environment variable unset to indicate
 *  that no metrics are requested. Use the tools \c papi_avail and \c papi_native_avail to
 *  get a list of available PAPI events. If you want to change the separator used in the
 *  list of PAPI metric names set the environment variable \c SCOREP_METRIC_PAPI_SEP to
 *  the needed character.
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
 *  Implementation of SCOREP event set data structure specific for the PAPI metric source.
 */
struct SCOREP_Metric_EventSet
{
    /** A struct for each active component */
    scorep_event_map* event_map[ SCOREP_METRIC_MAXNUM ];
    /** For each counter a pointer, that points to the event sets return values */
    long_long*        values[ SCOREP_METRIC_MAXNUM ];
};

/** Contains the name of requested metrics. */
char* scorep_metrics_papi = NULL;

/** Contains the separator of metric names. */
char* scorep_metrics_papi_separator = NULL;

/**
 *  Configuration variables for the metric adapter.
 *  Current configuration variables are:
 *  @li @c SCOREP_METRIC_PAPI list of requested metric names.
 *  @li @c SCOREP_METRIC_PAPI_SEP character that separates single metric names.
 */
SCOREP_ConfigVariable scorep_metric_papi_configs[] = {
    {
        "papi",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_papi,
        NULL,
        "",
        "Metric names.",
        "List of requested metric names that will be collected during program run.\n"
    },
    {
        "papi_sep",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_papi_separator,
        NULL,
        ",",
        "Separator of metric names.",
        "Character that separates metric names in PAPI.\n"
    },
    SCOREP_CONFIG_TERMINATOR
};



/* *********************************************************************
 * Definition of local functions
 **********************************************************************/

static void
scorep_metric_papi_add( char* name,
                        int   code,
                        bool  isAbsolute );

static void
scorep_metric_papi_test();

static void
scorep_metric_papi_descriptions( void );

static void
scorep_metric_papi_warning( int   errcode,
                            char* note );

static void
scorep_metric_papi_error( int   errcode,
                          char* note );



/* *********************************************************************
* Global variables
***********************************************************************/

/**
 * This vector contains specification data of used metrics.
 */
static scorep_papi_metric* metricv[ SCOREP_METRIC_MAXNUM ];

/**
 * Number of used metrics.
 */
static uint32_t number_of_metrics = 0;

/**
 * Static variable to control initialize status of the metric adapter.
 * If it is 0 it is initialized.
 */
static int scorep_metric_papi_initialize = 1;



/* *********************************************************************
 * Declarations from scorep_thread.h
 **********************************************************************/

extern SCOREP_Thread_LocationData*
SCOREP_Thread_GetLocationData();

extern uint64_t
SCOREP_Thread_GetLocationId( SCOREP_Thread_LocationData* locationData );

/* *********************************************************************
 * Helper functions
 **********************************************************************/

/** @brief  This function can be used to determine identifier of recent location.
 *
 *  @return It returns the identifier of recent location.
 */
unsigned long
scorep_metric_get_location_id()
{
    /* Get the thread id from the measurement system */
    SCOREP_Thread_LocationData* data = SCOREP_Thread_GetLocationData();
    SCOREP_ASSERT( data != NULL );

    uint64_t thread_id = SCOREP_Thread_GetLocationId( data );
    SCOREP_ASSERT( thread_id >> 32 == 0 );

    return ( unsigned long )thread_id;
}




/** @brief Reads the configuration from environment variables and configuration
 *         files and initializes the performance counter library. It must be called
 *         before other functions of the library are used by the measurement system.
 */
void
scorep_metric_papi_open()
{
    /** A leading exclamation mark let the metric be interpreted as absolute value counter.
     *  Appropriate metric properties have to be set. */
    bool              is_absolute;
    /** Content of environment variable SCOREP_METRIC_PAPI */
    char*             env_metrics;
    /** Separator of individual metrics in environment variable SCOREP_METRIC_PAPI */
    char*             env_metrics_sep;
    /** Individual metric */
    char*             token;
    /** PAPI return value. */
    int               retval;
    /** Info struct about PAPI event */
    PAPI_event_info_t info;

    /* Read environment variable "SCOREP_METRIC_PAPI". Return if
     * used and no PAPI timer used. */
    env_metrics = SCOREP_CStr_dup( scorep_metrics_papi );

    /* Return if environment variable is empty */
    if ( strlen( env_metrics ) == 0 )
    {
        return;
    }
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "SCOREP_METRIC_PAPI=%s", env_metrics );

    env_metrics_sep = SCOREP_CStr_dup( scorep_metrics_papi_separator );

    /* Initialize PAPI */
    retval = PAPI_library_init( PAPI_VER_CURRENT );
    assert( retval == PAPI_VER_CURRENT );

    /* PAPI code of recent metric */
    int   code;
    /* Metric name */
    char* component;
    /* Read metrics from specification string */
    token = strtok( env_metrics, env_metrics_sep );
    while ( token && ( number_of_metrics < SCOREP_METRIC_MAXNUM ) )
    {
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

        scorep_metric_papi_add( component, code, is_absolute );

        token = strtok( NULL, env_metrics_sep );
    }

    /* Clean up */
    free( env_metrics );
    free( env_metrics_sep );

    /* Check whether event combination is valid. This is done here to
     * avoid errors when creating the event set for each thread, which
     * would multiply the error messages. */
    scorep_metric_papi_test();

    scorep_metric_papi_descriptions();
}

/** @brief Finalizes the performance counter adapter. Frees memory allocated by
 *         scorep_metric_papi_open.
 */
void
scorep_metric_papi_close()
{
    for ( uint32_t i = 0; i < number_of_metrics; i++ )
    {
        free( metricv[ i ]->name );
        free( metricv[ i ] );
    }

    if ( number_of_metrics > 0 )
    {
        PAPI_shutdown();
    }

    number_of_metrics = 0;
}

/** @brief  Creates per-thread counter sets.
 *
 *  @return It returns the new event set.
 */
SCOREP_Metric_EventSet*
scorep_metric_papi_create_event_set()
{
    SCOREP_Metric_EventSet* event_set;
    int                     retval;
    int                     component;

    if ( number_of_metrics == 0 )
    {
        return NULL;
    }

    event_set = ( SCOREP_Metric_EventSet* )malloc( sizeof( SCOREP_Metric_EventSet ) );
    assert( event_set );

    /* Create event set */
    for ( uint32_t i = 0; i < SCOREP_METRIC_MAXNUM; i++ )
    {
        event_set->event_map[ i ] = NULL;
    }

    uint32_t j;
    for ( uint32_t i = 0; i < number_of_metrics; i++ )
    {
        struct scorep_event_map* eventset;

        #ifdef PAPIC
        component = PAPI_COMPONENT_INDEX( metricv[ i ]->papi_code );
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
        retval = PAPI_add_event( eventset->event_id, metricv[ i ]->papi_code );
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

    if ( eventSet == NULL )
    {
        return;
    }

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
 *  @param name       Name of recent metric.
 *  @param code       PAPI code of recent metric.
 *  @param isAbsolute Is true if this metric should be interpreted as absolute value.
 *                    Otherwise it is false.
 */
static void
scorep_metric_papi_add( char* name,
                        int   code,
                        bool  isAbsolute )
{
    if ( number_of_metrics >= SCOREP_METRIC_MAXNUM )
    {
        SCOREP_ERROR( SCOREP_ERROR_PAPI_INIT, "Number of counters exceeds Score-P allowed maximum of %d", SCOREP_METRIC_MAXNUM );
    }
    else
    {
        metricv[ number_of_metrics ]                   = ( scorep_papi_metric* )malloc( sizeof( scorep_papi_metric ) );
        metricv[ number_of_metrics ]->name             = SCOREP_CStr_dup( name );
        metricv[ number_of_metrics ]->description[ 0 ] = '\0';
        if ( isAbsolute )
        {
            metricv[ number_of_metrics ]->mode = SCOREP_METRIC_MODE_ABSOLUTE;
        }
        else
        {
            metricv[ number_of_metrics ]->mode = SCOREP_METRIC_MODE_ACCUMULATED;
        }
        metricv[ number_of_metrics ]->papi_code = code;
        number_of_metrics++;
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

    PAPI_perror( errcode, errstring, PAPI_MAX_STR_LEN );
    if ( errcode == PAPI_ESYS )
    {
        strncat( errstring, ": ", PAPI_MAX_STR_LEN - strlen( errstring ) - 1 );
        strncat( errstring, strerror( errno ), PAPI_MAX_STR_LEN - strlen( errstring ) - 1 );
    }
    SCOREP_ERROR( SCOREP_ERROR_PAPI_INIT, "%s: %s (fatal)\n", note ? note : "PAPI", errstring );
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

    PAPI_perror( errcode, errstring, PAPI_MAX_STR_LEN );
    if ( errcode == PAPI_ESYS )
    {
        strncat( errstring, ": ", PAPI_MAX_STR_LEN - strlen( errstring ) - 1 );
        strncat( errstring, strerror( errno ), PAPI_MAX_STR_LEN - strlen( errstring ) - 1 );
    }
    SCOREP_ERROR( SCOREP_WARNING, "%s: %s (ignored)\n", note ? note : "PAPI", errstring );
}

/** @brief Prints metric descriptions.
 */
static void
scorep_metric_papi_descriptions( void )
{
    int               j;
    int               retval;
    PAPI_event_info_t info;

    for ( int i = 0; i < number_of_metrics; i++ )
    {
        memset( &info, 0, sizeof( PAPI_event_info_t ) );
        retval = PAPI_get_event_info( metricv[ i ]->papi_code, &info );
        if ( retval != PAPI_OK )
        {
            scorep_metric_papi_error( retval, "PAPI_get_event_info" );
        }

        if ( strcmp( info.long_descr, metricv[ i ]->name ) != 0 )
        {
            strncpy( metricv[ i ]->description, info.long_descr, sizeof( metricv[ i ]->description ) - 1 );

            /* Tidy description if necessary */
            j = strlen( metricv[ i ]->description ) - 1;
            if ( metricv[ i ]->description[ j ] == '\n' )
            {
                metricv[ i ]->description[ j ] = '\0';
            }
            j = strlen( metricv[ i ]->description ) - 1;
            if ( metricv[ i ]->description[ j ] != '.' )
            {
                strncat( metricv[ i ]->description, ".", sizeof( metricv[ i ]->description ) - strlen( metricv[ i ]->description ) - 1 );
            }
        }

        if ( metricv[ i ]->papi_code & PAPI_PRESET_MASK )
        {
            /* PAPI preset */

            char* postfix_chp = info.postfix;
            char  derive_ch   = strcmp( info.derived, "DERIVED_SUB" ) ? '+' : '-';

            strncat( metricv[ i ]->description, " [ ", sizeof( metricv[ i ]->description ) - strlen( metricv[ i ]->description ) - 1 );
            strncat( metricv[ i ]->description, info.name[ 0 ], sizeof( metricv[ i ]->description ) - strlen( metricv[ i ]->description ) - 1 );

            for ( int k = 1; k < ( int )info.count; k++ )
            {
                char op[ 4 ];
                postfix_chp = postfix_chp ? strpbrk( ++postfix_chp, "+-*/" ) : NULL;
                sprintf( op, " %c ", ( postfix_chp ? *postfix_chp : derive_ch ) );
                strncat( metricv[ i ]->description, op, sizeof( metricv[ i ]->description ) - strlen( metricv[ i ]->description ) - 1 );
                strncat( metricv[ i ]->description, info.name[ k ], sizeof( metricv[ i ]->description ) - strlen( metricv[ i ]->description ) - 1 );
            }
            strncat( metricv[ i ]->description, " ]", sizeof( metricv[ i ]->description ) - strlen( metricv[ i ]->description ) - 1 );
            if ( strcmp( info.symbol, metricv[ i ]->name ) != 0 ) /* add preset name */
            {
                strncat( metricv[ i ]->description, " = ", sizeof( metricv[ i ]->description ) - strlen( metricv[ i ]->description ) - 1 );
                strncat( metricv[ i ]->description, info.symbol, sizeof( metricv[ i ]->description ) - strlen( metricv[ i ]->description ) - 1 );
            }
        }
    }
}

/** @brief Tests whether requested event combination is valid or not.
 */
static void
scorep_metric_papi_test()
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

    for ( i = 0; i < number_of_metrics; i++ )
    {
        #ifdef PAPIC
        /* Preset-counter belong to Component 0! */
        component = PAPI_COMPONENT_INDEX( metricv[ i ]->papi_code );
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
        retval = PAPI_add_event( event_set[ j ]->event_id, metricv[ i ]->papi_code );
        if ( retval != PAPI_OK )
        {
            char errstring[ PAPI_MAX_STR_LEN ];
            sprintf( errstring, "PAPI_add_event(%d:\"%s\")", i, metricv[ i ]->name );
            scorep_metric_papi_error( retval, errstring );
        }
//      vt_cntl_msg( 2, "Event %s added to event set", metricv[ i ]->name );
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Event %s added to event set", metricv[ i ]->name );
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

//  vt_cntl_msg( 2, "Event set tested OK" );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Event set tested OK" );
}


/* *********************************************************************
 * Adapter management
 **********************************************************************/

/** @brief  Registers configuration variables for the metric adapters.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
SCOREP_Error_Code
scorep_metric_papi_register()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " register PAPI metric source!" );

    return SCOREP_ConfigRegister( "metric", scorep_metric_papi_configs );
}

/** @brief Called on deregistration of the metric adapter. Currently, no action is performed
 *         on deregistration.
 */
void
scorep_metric_papi_deregister()
{
    free( scorep_metrics_papi );
    free( scorep_metrics_papi_separator );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " PAPI metric source deregister!" );
}

/** @brief  Initializes the metric adapter.
 *
 *  @return It returns the number of recently used metrics.
 */
uint32_t
scorep_metric_papi_initialize_source()
{
    if ( scorep_metric_papi_initialize )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialize PAPI metric source." );

        /* Initialize PAPI, determine number of specified metrics and set global variable */
        scorep_metric_papi_open();

        /* Set flag */
        scorep_metric_papi_initialize = 0;

        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialization of PAPI metric source done." );
    }

    return number_of_metrics;
}

/** @brief Adapter finalization.
 */
void
scorep_metric_papi_finalize_source()
{
    /* Call only, if previously initialized */
    if ( !scorep_metric_papi_initialize )
    {
        scorep_metric_papi_close();

        /* Set initialization flag */
        scorep_metric_papi_initialize = 1;
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " finalize PAPI metric source." );
    }
}

/** @brief  Location specific initialization function for metric adapters.
 *
 *  @return It returns the event set used by this location.
 */
SCOREP_Metric_EventSet*
scorep_metric_papi_initialize_location()
{
    if ( number_of_metrics == 0 )
    {
        return NULL;
    }

    int retval = PAPI_thread_init( &scorep_metric_get_location_id );
    if ( retval != PAPI_OK )
    {
        scorep_metric_papi_error( retval, "PAPI_thread_init" );
    }
//  vt_cntl_msg( 2, "PAPI thread support initialized" );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "PAPI thread support initialized" );

    return scorep_metric_papi_create_event_set();
}

/** @brief Location specific finalization function for metric adapters.
 *
 *  @param eventSet Event set to close.
 */
void
scorep_metric_papi_finalize_location( SCOREP_Metric_EventSet* eventSet )
{
    if ( number_of_metrics == 0 )
    {
        return;
    }

    scorep_metric_papi_free( eventSet );

    /* Ignore return value */
    ( void )PAPI_unregister_thread();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " PAPI metric source final location!" );
}

/** @brief Reads values of counters relative to the time of scorep_metric_papi_open().
 *
 *  @param eventSet An event set, that contains the definition of the counters
 *                  that should be measured.
 *  @param values   An array, to which the counter values are written.
 */
void
scorep_metric_papi_read( SCOREP_Metric_EventSet* eventSet,
                         uint64_t*               values )
{
    assert( eventSet );
    assert( values );

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

    for ( uint32_t i = 0; i < number_of_metrics; i++ )
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
 *  @return It returns the number of recently used metrics.
 */
int32_t
scorep_metric_papi_get_number_of_metrics()
{
    return number_of_metrics;
}

/** @brief  Returns the name of requested metric.
 *
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns the name of requested metric.
 */
const char*
scorep_metric_papi_get_metric_name( uint32_t metricIndex )
{
    if ( metricIndex < number_of_metrics )
    {
        return metricv[ metricIndex ]->name;
    }
    else
    {
        return "";
    }
}


/** @brief  Returns a description of requested metric.
 *
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns a description of the unit of requested metric.
 */
const char*
scorep_metric_papi_get_metric_description( uint32_t metricIndex )
{
    if ( metricIndex < number_of_metrics )
    {
        return metricv[ metricIndex ]->description;
    }
    else
    {
        return "";
    }
}

/** @brief  Returns a string containing a representation of the unit of requested metric.
 *
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns a string containing a representation of the unit of requested metric.
 */
const char*
scorep_metric_papi_get_metric_unit( uint32_t metricIndex )
{
    if ( metricIndex < number_of_metrics )
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
 *  @param  metricIndex Index of requested metric
 *
 *  @return It returns property settings of requested metrics.
 */
SCOREP_Metric_Properties
scorep_metric_papi_get_metric_properties( uint32_t metricIndex )
{
    SCOREP_Metric_Properties props;

    if ( metricIndex < number_of_metrics )
    {
        props.name           = metricv[ metricIndex ]->name;
        props.description    = metricv[ metricIndex ]->description;
        props.source_type    = SCOREP_METRIC_SOURCE_TYPE_PAPI;
        props.mode           = metricv[ metricIndex ]->mode;
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
uint64_t
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
        vt_error_msg( "Failed to access PAPI hardware info\n" );
    }
//  vt_cntl_msg( 2, "Clock rate: %f MHz", hwinfo->mhz );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Clock rate: %f MHz", hwinfo->mhz );

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
