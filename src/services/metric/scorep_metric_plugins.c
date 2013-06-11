/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 *  @file       scorep_metric_plugins.c
 *
 *  @author     Ronny Tschueter <ronny.tschueter@tu-dresden.de>
 *  @maintainer Ronny Tschueter <ronny.tschueter@tu-dresden.de>
 *
 *  @brief This module implements support for metric plugins.
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

#include <dlfcn.h>

#include "SCOREP_Config.h"
#include <UTILS_Error.h>
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>

#include "SCOREP_Metric_Source.h"
#include <scorep/SCOREP_MetricPlugins.h>

#include <SCOREP_Timing.h>

#define SCOREP_METRIC_PLUGIN_MAX_PER_THREAD 16

#define BUFFER_SIZE 512


/** Per plugin library information */
typedef struct scorep_metric_plugin_struct
{
    /** Info from get_info() */
    SCOREP_Metric_Plugin_Info              info;
    /** Handle (should be closed when finalize) */
    void*                                  dlfcn_handle;
    /** Plugin name */
    char*                                  plugin_name;
    /** Index of additional environment variable in @ additional_environment_variables array */
    uint32_t                               additional_event_environment_variable_index;
    /** Number of selected event_names */
    int                                    num_selected_events;
    /** Selected event_names */
    char**                                 selected_events;
    /** Meta data about metrics (e.g. metric name, units) */
    SCOREP_Metric_Plugin_MetricProperties* metrics_meta_data;
} scorep_metric_plugin;

/** Data structure which a location uses to handle a single metric provided by a plugin */
typedef struct scorep_metric_plugin_individual_metric_struct
{
    /** ID, which was produced by the plugin */
    int32_t plugin_metric_id;
    /** Functions for getting a metric value */
    uint64_t ( * getValue )( int32_t );
    /** Functions for getting an optional metric value */
    bool ( * getOptionalValue )( int32_t,
                                 uint64_t* );
    /** Functions for getting a multiple metric values */
    uint64_t ( * getAllValues )( int32_t,
                                 SCOREP_MetricTimeValuePair** );
    /** Meta data about this metric (e.g. metric name, units) */
    SCOREP_Metric_Plugin_MetricProperties* meta_data;
    /** Time between calls to plugin */
    uint64_t                               delta_t;
} scorep_metric_plugin_individual_metric_struct;

/** Metric data structure */
struct SCOREP_Metric_EventSet
{
    /** Overall number of metrics within this event set */
    uint32_t                                      number_of_metrics;
    /** Array of individual metric specifications */
    scorep_metric_plugin_individual_metric_struct metrics[ SCOREP_METRIC_PLUGIN_MAX_PER_THREAD ];
    /** Array of last timestamps where each metric was recorded */
    uint64_t                                      last_timestamps[ SCOREP_METRIC_PLUGIN_MAX_PER_THREAD ];
};


/* *********************************************************************
 * Global variables
 **********************************************************************/

/** Static variable to control initialize status of the metric plugins source.
 *  If it is 0 it is initialized. */
static int scorep_metric_plugins_initialize = 1;

/** Static variable to check whether plugins are used.
 *  If it is true plugins are used. */
static bool is_scorep_metric_plugin_used = false;

/** A number of plugin libraries, [sync_type][library_nr] */
static scorep_metric_plugin* scorep_metric_plugin_handles[ SCOREP_METRIC_SYNC_TYPE_MAX ];

/** Number of used plugins per sync type*/
static uint32_t num_plugins[ SCOREP_METRIC_SYNC_TYPE_MAX ];

/** Number of additional environment variables used to specify events of individual plugins */
static uint32_t num_additional_environment_variables;

#include "scorep_metric_plugins_confvars.inc.c"


/* *********************************************************************
 * Helper functions
 **********************************************************************/

static SCOREP_Metric_EventSet*
create_metric_plugin_defines( void );


/* *********************************************************************
 * Metric source management
 **********************************************************************/

/** @brief  Registers configuration variables for the metric sources.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
scorep_metric_plugins_register( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " register metric plugins source!" );

    /* Register environment variables to specify ised plugins */
    SCOREP_ErrorCode status;
    status = SCOREP_ConfigRegister( "metric", scorep_metric_plugins_configs );
    if ( status != SCOREP_SUCCESS )
    {
        UTILS_ERROR( SCOREP_ERROR_PAPI_INIT, "Registration of plugins configure variables failed." );
    }

    return status;
}

/** @brief Called on deregistration of the metric source.
 */
static void
scorep_metric_plugins_deregister( void )
{
    /* Free environment variables for plugin specification */

    free( scorep_metrics_plugins );
    free( scorep_metrics_plugins_separator );

    for ( uint32_t i = 0; i < num_additional_environment_variables; i++ )
    {
        additional_environment_variables_container* additional_env_var_struct
            = &additional_environment_variables[ i ];
        free( additional_env_var_struct->event_variable );
    }

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric plugins source deregister!" );
}

/** @brief  Initialize metric plugins source.
 *
 *  During initialization respective environment variables are read to
 *  determine which metrics has been specified by the user.
 *
 *  Because 'strictly synchronous' metrics will be recorded by all
 *  locations, we known how many metrics of this specific type each
 *  location will record. This number is returned by this function.
 *
 *  For metrics of other types (e.g. per-process) we can not determine
 *  whether this location will be responsible to record metrics of this
 *  type. Responsibility will be determine while initializing location.
 *  Therefore, within this function we don't known how many additional
 *  metrics will be recorded by a specific location.
 *
 *  @return Returns the number of used 'strictly synchronous' metrics.
 */
static uint32_t
scorep_metric_plugins_initialize_source( void )
{
    /* Number of used 'strictly synchronous' metrics */
    uint32_t metric_counts = 0;

    if ( scorep_metric_plugins_initialize )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialize plugins metric source." );
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " list of plugins = %s\n", scorep_metrics_plugins );

        /* Working copy of environment variable content */
        char* env_var_content;
        /* Pointer to single character of metric specification string */
        char* token;

        /* Buffer to extend plugin name with .so suffix */
        char buffer[ BUFFER_SIZE ];
        /* Handle from dlopen() */
        void* handle;
        /* Information about plugin */
        SCOREP_Metric_Plugin_Info info;
        /* String with error message if there is a problem while dlopen() */
        char* dl_lib_error;

        /* List of plugin names */
        char** plugins = NULL;

        /* Use union to get rid of compiler warning */
        union
        {
            void*                     void_ptr;
            SCOREP_Metric_Plugin_Info ( * function )( void );
        } get_info;

        scorep_metric_plugin* current_plugin;

        /* Number of selected plugins */
        uint32_t num_selected_plugins = 0;

        /* Read content of environment variable */
        env_var_content = UTILS_CStr_dup( scorep_metrics_plugins );

        /* Return if environment variable is empty */
        if ( strlen( env_var_content ) == 0 )
        {
            free( env_var_content );
            return 0;
        }

        /* Read plugin names from specification string */
        token = strtok( env_var_content, scorep_metrics_plugins_separator );
        while ( token )
        {
            bool is_new_plugin = true;

            /* Avoid loading the same plugin multiple times */
            for ( uint32_t i = 0; i < num_selected_plugins; i++ )
            {
                if ( strcmp( plugins[ i ], token ) == 0 )
                {
                    /* This plugin was already specified */
                    is_new_plugin = false;
                    break;
                }
            }

            if ( is_new_plugin )
            {
                num_selected_plugins++;
                /* Allocate the plugin name buffer */
                plugins = realloc( plugins, num_selected_plugins * sizeof( char* ) );
                UTILS_BUG_ON( plugins == NULL, "Out of memory." );
                /* Copy the content to the buffer */
                plugins[ num_selected_plugins - 1 ] = UTILS_CStr_dup( token );
            }

            /* Handle next plugin */
            token = strtok( NULL, scorep_metrics_plugins_separator );
        }
        free( env_var_content );

        /* Go through all plugins */
        for ( uint32_t i = 0; i < num_selected_plugins; i++ )
        {
            char* current_plugin_name = plugins[ i ];

            /* Load it from LD_LIBRARY_PATH*/
            snprintf( buffer, BUFFER_SIZE, "lib%s.so", current_plugin_name );

            /* Now use dlopen to load dynamic library
             * RTLD_NOW: all undefined symbols in the library are resolved
             *           before dlopen() returns, if this cannot be done,
             *           an error is returned. */
            handle = dlopen( buffer, RTLD_NOW );

            /* If it is not valid */
            dl_lib_error = dlerror();
            if ( dl_lib_error != NULL )
            {
                UTILS_WARNING( "Could not open metric plugin %s. Error message was: %s",
                               current_plugin_name,
                               dl_lib_error );
                continue;
            }

            /* Now get the address where the
             * info symbol is loaded into memory */
            snprintf( buffer, BUFFER_SIZE, "SCOREP_MetricPlugin_%s_get_info", current_plugin_name );
            get_info.void_ptr = dlsym( handle, buffer );
            dl_lib_error      = dlerror();
            if ( dl_lib_error != NULL )
            {
                UTILS_WARNING( "Could not find symbol 'get_info' of metric plugin %s. Error message was: %s",
                               current_plugin_name,
                               dl_lib_error );
                dlclose( handle );
                continue;
            }

            /* Call get_info function of plugin */
            info = get_info.function();

            if ( info.plugin_version > SCOREP_METRIC_PLUGIN_VERSION )
            {
                UTILS_WARNING( "Incompatible version of metric plugin detected. You may experience problems. (Version of %s plugin %u, support up to version %u)\n",
                               current_plugin_name,
                               info.plugin_version,
                               SCOREP_METRIC_PLUGIN_VERSION );
            }

            if ( info.initialize == NULL )
            {
                UTILS_WARNING( "Initialization function not implemented in plugin %s\n", current_plugin_name );
                /* Try loading next */
                continue;
            }

            if ( info.add_counter == NULL )
            {
                UTILS_WARNING( "Function 'add_counter' not implemented in plugin %s\n", current_plugin_name );
                /* Try loading next */
                continue;
            }

            if ( info.get_event_info == NULL )
            {
                UTILS_WARNING( "Function 'get_event_info' not implemented in plugin %s\n", current_plugin_name );
                /* Try loading next */
                continue;
            }

            if ( info.finalize == NULL )
            {
                UTILS_WARNING( "Finalization function not implemented in plugin %s\n", current_plugin_name );
                /* Try loading next */
                continue;
            }

            /* Check the 'run per' type */
            switch ( info.run_per )
            {
                case SCOREP_METRIC_PER_THREAD:
                case SCOREP_METRIC_PER_PROCESS:
                case SCOREP_METRIC_PER_HOST:
                case SCOREP_METRIC_ONCE:
                    break;
                default:
                    UTILS_WARNING( "Invalid 'run per' type implemented in plugin %s\n", current_plugin_name );
                    /* Try loading next */
                    continue;
            }

            /* Check the type of plugin */
            switch ( info.sync )
            {
                case SCOREP_METRIC_STRICTLY_SYNC:
                    /* Strictly synchronous plugins have to be recorded per thread */
                    if ( info.run_per != SCOREP_METRIC_PER_THREAD )
                    {
                        UTILS_WARNING( "Strictly synchronous plugins have to be recorded per thread (%s)\n", current_plugin_name );
                        /* Try loading next */
                        continue;
                    }
                    if ( info.get_current_value == NULL )
                    {
                        UTILS_WARNING( "Function 'get_current_value' not implemented in plugin %s\n", current_plugin_name );
                        /* Try loading next */
                        continue;
                    }
                    break;

                case SCOREP_METRIC_SYNC:
                    /* Synchronous plugins have to implement get_current_value */
                    if ( info.get_optional_value == NULL )
                    {
                        UTILS_WARNING( "Function 'get_optional_value' not implemented in plugin %s\n", current_plugin_name );
                        /* Try loading next */
                        continue;
                    }
                    break;

                case SCOREP_METRIC_ASYNC_EVENT:
                case SCOREP_METRIC_ASYNC:
                    /* Asynchronous plugins have to implement get_all_values and set_pform_wtime_function */
                    if ( info.get_all_values == NULL )
                    {
                        UTILS_WARNING( "Function 'get_all_values' not implemented in plugin %s\n", current_plugin_name );
                        /* Try loading next */
                        continue;
                    }
                    if ( info.set_clock_function == NULL )
                    {
                        UTILS_WARNING( "Function 'set_clock_function' not implemented in plugin %s\n", current_plugin_name );
                        /* Try loading next */
                        continue;
                    }
                    break;

                default:
                    UTILS_WARNING( "Invalid synchronicity type implemented in plugin %s\n", current_plugin_name );
                    /* Try loading next */
                    continue;
            }
            num_plugins[ info.sync ]++;

            scorep_metric_plugin_handles[ info.sync ] =
                realloc( scorep_metric_plugin_handles[ info.sync ],
                         num_plugins[ info.sync ] * sizeof( scorep_metric_plugin ) );
            UTILS_BUG_ON( scorep_metric_plugin_handles[ info.sync ] == NULL, "Out of memory" );
            current_plugin = &scorep_metric_plugin_handles[ info.sync ][ num_plugins[ info.sync ] - 1 ];

            /* Clear out current plugin */
            memset( current_plugin, 0, sizeof( scorep_metric_plugin ) );

            UTILS_BUG_ON( num_additional_environment_variables >= PLUGINS_MAX,
                          "Maximum number of additional environment variables for metric plugins reached." );
            additional_environment_variables_container* additional_environment_variable
                                                                        = &additional_environment_variables[ num_additional_environment_variables ];
            current_plugin->additional_event_environment_variable_index = num_additional_environment_variables;
            num_additional_environment_variables++;

            /* Register additional per-plugin environment variables */
            additional_environment_variable->config_variables[ 0 ].name              = current_plugin_name;
            additional_environment_variable->config_variables[ 0 ].type              = SCOREP_CONFIG_TYPE_STRING;
            additional_environment_variable->config_variables[ 0 ].variableReference = &( additional_environment_variable->event_variable );
            additional_environment_variable->config_variables[ 0 ].variableContext   = NULL;
            additional_environment_variable->config_variables[ 0 ].defaultValue      = "";
            additional_environment_variable->config_variables[ 0 ].shortHelp         = "Specify list of used events from this plugin";
            additional_environment_variable->config_variables[ 0 ].longHelp          = "List of requested event names from this plugin that will be used during program run.";

            /* @ todo: if SCOREP_CONFIG_TERMINATOR changes we will get in trouble because it's likely to forget to adapt this code  */
            additional_environment_variable->config_variables[ 1 ].name              = NULL;
            additional_environment_variable->config_variables[ 1 ].type              = SCOREP_INVALID_CONFIG_TYPE;
            additional_environment_variable->config_variables[ 1 ].variableReference = NULL;
            additional_environment_variable->config_variables[ 1 ].variableContext   = NULL;
            additional_environment_variable->config_variables[ 1 ].defaultValue      = NULL;
            additional_environment_variable->config_variables[ 1 ].shortHelp         = NULL;
            additional_environment_variable->config_variables[ 1 ].longHelp          = NULL;

            SCOREP_ErrorCode status;
            status = SCOREP_ConfigRegister( "metric", additional_environment_variable->config_variables );
            if ( status != SCOREP_SUCCESS )
            {
                UTILS_WARNING( "Registration of individual Metric Plugins configuration variables failed." );
            }
            status = SCOREP_ConfigApplyEnv();
            if ( status != SCOREP_SUCCESS )
            {
                UTILS_WARNING( "Evaluation of individual Metric Plugins configuration variables failed." );
            }

            /* Add handle (should be closed in the end) */
            current_plugin->dlfcn_handle = handle;

            /* Store the info object of the plugin */
            current_plugin->info = info;

            /* Store the name of the plugin */
            current_plugin->plugin_name = UTILS_CStr_dup( current_plugin_name );

            /* Give plugin the wtime function to make it possible to convert times */
            if ( current_plugin->info.set_clock_function != NULL )
            {
                current_plugin->info.set_clock_function( SCOREP_GetClockTicks );
            }

            /* Initialize plugin */
            if ( current_plugin->info.initialize() )
            {
                UTILS_WARNING( "Error while initializing plugin %s, initialization returned != 0\n", current_plugin_name );
                continue;
            }

            /* Read events selected for current plugin */
            env_var_content = UTILS_CStr_dup( additional_environment_variable->event_variable );
            token           = strtok( env_var_content, scorep_metrics_plugins_separator );
            while ( token )
            {
                SCOREP_Metric_Plugin_MetricProperties* metric_infos = info.get_event_info( token );
                UTILS_BUG_ON( metric_infos == NULL, "Error while initializing plugin metric %s, no info returned\n", token );
                UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Adding metric %s for plugin counter library: lib%s.so", token, current_plugin_name );

                /* Store data structure containing meta data about current metrics */
                current_plugin->metrics_meta_data = metric_infos;

                /* Iterate over the info items */
                SCOREP_Metric_Plugin_MetricProperties* current_metric_info = metric_infos;
                for ( current_metric_info = metric_infos; current_metric_info->name != NULL; current_metric_info++ )
                {
                    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Retrieved metric %s for plugin counter library: lib%s.so."
                                        " Initializing data structures", current_metric_info->name, current_plugin_name );

                    /* Event is part of this plugin */
                    current_plugin->num_selected_events++;

                    /* Allocate space for events */
                    current_plugin->selected_events =
                        realloc( current_plugin->selected_events, current_plugin->num_selected_events * sizeof( char* ) );
                    UTILS_BUG_ON( current_plugin->selected_events == NULL, "Out of memory" );

                    /* The metric is everything after "plugin_" */
                    current_plugin->selected_events[ current_plugin->num_selected_events - 1 ] =
                        current_metric_info->name;

                    /* If a unit is provided, use it */
                    if ( current_metric_info->unit == NULL )
                    {
                        current_metric_info->unit = "#";
                    }
                    /* If a description is provided, use it */
                    if ( current_metric_info->description == NULL )
                    {
                        current_metric_info->description = "";
                    }

                    /* Metric plugins are in use */
                    is_scorep_metric_plugin_used = true;
                } /* End of: for all metrics related to the metric string */

                /* Handle next plugin */
                token = strtok( NULL, scorep_metrics_plugins_separator );
            }
            free( env_var_content );

            if ( info.sync == SCOREP_METRIC_STRICTLY_SYNC )
            {
                /* Only count strictly synchronous metrics here */
                metric_counts += current_plugin->num_selected_events;
            }
        }

        for ( uint32_t i = 0; i < num_selected_plugins; i++ )
        {
            free( plugins[ i ] );
        }
        free( plugins );

        /* ************************************************************** */

        scorep_metric_plugins_initialize = 0;
    }

    return metric_counts;
}

/** @brief Metric source finalization.
 */
static void
scorep_metric_plugins_finalize_source( void )
{
    /* Call only, if previously initialized */
    if ( !scorep_metric_plugins_initialize )
    {
        for ( uint32_t i = 0; i < SCOREP_METRIC_SYNC_TYPE_MAX; i++ )
        {
            for ( uint32_t j = 0; j < num_plugins[ i ]; j++ )
            {
                /* Call finalization function of plugin */
                scorep_metric_plugin_handles[ i ][ j ].info.finalize();

                /* Free resources */
                for ( uint32_t event = 0; event < scorep_metric_plugin_handles[ i ][ j ].num_selected_events; event++ )
                {
                    free( scorep_metric_plugin_handles[ i ][ j ].metrics_meta_data[ event ].name );
                }
                free( scorep_metric_plugin_handles[ i ][ j ].metrics_meta_data );
                free( scorep_metric_plugin_handles[ i ][ j ].selected_events );
                free( scorep_metric_plugin_handles[ i ][ j ].plugin_name );

                dlclose( scorep_metric_plugin_handles[ i ][ j ].dlfcn_handle );
            }
            free( scorep_metric_plugin_handles[ i ] );
        }

        /* Set metric plugin usage flag */
        is_scorep_metric_plugin_used = false;

        /* Set initialization flag */
        scorep_metric_plugins_initialize = 1;
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " finalize metric plugins source." );
    }
}

/** @brief  Location specific initialization function for metric sources.
 *
 *  @param location             Location data.
 *  @param event_sets           Event sets of all metrics.
 */
static SCOREP_Metric_EventSet*
scorep_metric_plugins_initialize_location( SCOREP_Location*           location,
                                           SCOREP_MetricSynchronicity sync_type,
                                           SCOREP_MetricPer           metric_type )
{
    if ( !is_scorep_metric_plugin_used )
    {
        return NULL;
    }

    SCOREP_Metric_EventSet*                        plugin_metric_defines = NULL;
    scorep_metric_plugin_individual_metric_struct* current;

    /* For all plugins of currently selected type */
    for ( uint32_t plugin = 0; plugin < num_plugins[ sync_type ]; plugin++ )
    {
        /* Check whether this location is responsible to record metrics of requested sync type */
        scorep_metric_plugin* current_plugin = &scorep_metric_plugin_handles[ sync_type ][ plugin ];

        if ( current_plugin->info.run_per != metric_type )
        {
            continue;
        }

        if ( plugin_metric_defines == NULL )
        {
            plugin_metric_defines = create_metric_plugin_defines();
        }

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Score-P location (%p) adds own plugin metrics for plugin %s:",
                            location,
                            current_plugin->plugin_name );

        /* Get the current metrics for this location and sync type */
        current = plugin_metric_defines->metrics;

        /* Get the number of metrics for this location and sync type */
        uint32_t* current_size = &( plugin_metric_defines->number_of_metrics );

        /* For all selected events */
        for ( uint32_t event = 0; event < current_plugin->num_selected_events; event++ )
        {
            if ( *current_size >= SCOREP_METRIC_PLUGIN_MAX_PER_THREAD )
            {
                UTILS_WARNING( "You're about to add more then %i plugin counters,"
                               "which is impossible\n", SCOREP_METRIC_PLUGIN_MAX_PER_THREAD );
                continue;
            }

            /* Add metric */
            current[ *current_size ].meta_data        = &( current_plugin->metrics_meta_data[ event ] );
            current[ *current_size ].plugin_metric_id =
                current_plugin->info.add_counter( current_plugin->selected_events[ event ] );
            /* Check whether adding metric finished successfully */
            if ( current[ *current_size ].plugin_metric_id < 0 )
            {
                UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                             "Error while adding plugin metric \"%s\"\n",
                             current_plugin->selected_events[ event ] );
                continue;
            }

            current[ *current_size ].delta_t = current_plugin->info.delta_t;

            /* Set 'per type' stuff */
            switch ( current_plugin->info.sync )
            {
                case SCOREP_METRIC_STRICTLY_SYNC:
                    /* Strictly synchronous plugins have to implement 'getValue' function */
                    current[ *current_size ].getValue = current_plugin->info.get_current_value;
                    break;

                case SCOREP_METRIC_SYNC:
                    /* Synchronous plugins have to implement 'getOptionalValue' function */
                    current[ *current_size ].getOptionalValue = current_plugin->info.get_optional_value;
                    break;

                case SCOREP_METRIC_ASYNC:
                case SCOREP_METRIC_ASYNC_EVENT:
                    /* Asynchronous plugins have to implement 'getAllValues' function */
                    current[ *current_size ].getAllValues = current_plugin->info.get_all_values;
                    break;
            }

            /* Next metric */
            ( *current_size )++;
        } /* END for all selected events */
    }     /* END for all plugins by this type */

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric source initialized location!" );

    return plugin_metric_defines;
}

/** @brief Location specific finalization function for metric sources.
 *
 *  @param eventSet  Reference to active set of metrics.
 */
static void
scorep_metric_plugins_finalize_location( SCOREP_Metric_EventSet* eventSet )
{
    free( eventSet );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric source finalized location!" );
}

/** @brief Frees memory allocated for requested event set.
 *
 *  @param eventSet  Reference to active set of metrics.
 */
static void
scorep_metric_plugins_free_event_set( SCOREP_Metric_EventSet* eventSet )
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
scorep_metric_plugins_strictly_synchronous_read( SCOREP_Metric_EventSet* eventSet,
                                                 uint64_t*               values )
{
    UTILS_ASSERT( eventSet );
    UTILS_ASSERT( values );

    for ( uint32_t i = 0; i  < eventSet->number_of_metrics; i++ )
    {
        /* RonnyT @ todo move getValue function pointer from single metric to event set */
        UTILS_ASSERT( eventSet->metrics[ i ].getValue );

        values[ i ] = eventSet->metrics[ i ].getValue( eventSet->metrics[ i ].plugin_metric_id );
    }
}

/** @brief Synchronous call to reads values of metrics in the active event set.
 *         The array @ is_updated indicates whether a new value for corresponding
 *         metric was written or not.
 *
 *  @param eventSet[in]     An event set, that contains the definition of the counters
 *                          that should be measured.
 *  @param values[out]      Reference to array that will be filled with values from
 *                          active metrics.
 *  @param is_updated[out]  An array which indicates whether a new value of a specfic
 *                          metric was written (@ is_updated[i] == true ) or not
 *                          (@ is_updated[i] == false ).
 *  @param force_update[in] Update of all metric value in this event set is enforced.
 */
static void
scorep_metric_plugins_synchronous_read( SCOREP_Metric_EventSet* eventSet,
                                        uint64_t*               values,
                                        bool*                   is_updated,
                                        bool                    force_update )
{
    UTILS_ASSERT( eventSet );
    UTILS_ASSERT( values );
    UTILS_ASSERT( is_updated );

    uint64_t recent_timestamp = SCOREP_GetClockTicks();

    for ( uint32_t i = 0; i  < eventSet->number_of_metrics; i++ )
    {
        if ( ( recent_timestamp - eventSet->last_timestamps[ i ] > eventSet->metrics[ i ].delta_t )
             || force_update )
        {
            /* RonnyT @ todo move getOptionalValue function pointer from single metric to event set */
            UTILS_ASSERT( eventSet->metrics[ i ].getOptionalValue );

            is_updated[ i ] = eventSet->metrics[ i ].getOptionalValue( eventSet->metrics[ i ].plugin_metric_id,
                                                                       &( values[ i ] ) );
            eventSet->last_timestamps[ i ] = recent_timestamp;
        }
        else
        {
            is_updated[ i ] = false;
        }
    }
}

/** @brief Reads values of all metrics in asynchronous event set.
 *
 *  @param eventSet[in]             An event set, that contains the definition of the counters
 *                                  that should be measured.
 *  @param timevalue_pointer[out]   An array, to which the counter values are written.
 *  @param num_pairs[out]           Number of pairs (timestamp + value) written for each
 *                                  individual metric.
 *  @param force_update[in]         Update of all metric value in this event set is enforced.
 */
static void
scorep_metric_plugins_asynchronous_read( SCOREP_Metric_EventSet*      eventSet,
                                         SCOREP_MetricTimeValuePair** timevalue_pointer,
                                         uint64_t**                   num_pairs,
                                         bool                         force_update )
{
    UTILS_ASSERT( eventSet );
    UTILS_ASSERT( timevalue_pointer );

    uint64_t recent_timestamp = SCOREP_GetClockTicks();

    *num_pairs = malloc( eventSet->number_of_metrics * sizeof( uint64_t ) );
    UTILS_ASSERT( *num_pairs );

    for ( uint32_t i = 0; i  < eventSet->number_of_metrics; i++ )
    {
        timevalue_pointer[ i ] = NULL;

        if ( ( recent_timestamp - eventSet->last_timestamps[ i ] > eventSet->metrics[ i ].delta_t )
             || force_update )
        {
            /* RonnyT @ todo move getAllValues function pointer from single metric to event set */
            UTILS_ASSERT( eventSet->metrics[ i ].getAllValues );

            ( *num_pairs )[ i ] = eventSet->metrics[ i ].getAllValues( eventSet->metrics[ i ].plugin_metric_id,
                                                                       &( timevalue_pointer[ i ] ) );
            eventSet->last_timestamps[ i ] = recent_timestamp;
        }
        else
        {
            ( *num_pairs )[ i ] = 0;
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
scorep_metric_plugins_get_number_of_metrics( SCOREP_Metric_EventSet* eventSet )
{
    if ( eventSet == NULL )
    {
        return 0;
    }

    return eventSet->number_of_metrics;
}

/** @brief  Returns name of metric @metricIndex.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns name of requested metric.
 */
static const char*
scorep_metric_plugins_get_metric_name( SCOREP_Metric_EventSet* eventSet,
                                       uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    if ( metricIndex < eventSet->number_of_metrics )
    {
        return eventSet->metrics[ metricIndex ].meta_data->name;
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
scorep_metric_plugins_get_metric_description( SCOREP_Metric_EventSet* eventSet,
                                              uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    if ( metricIndex < eventSet->number_of_metrics )
    {
        return eventSet->metrics[ metricIndex ].meta_data->description;
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
scorep_metric_plugins_get_metric_unit( SCOREP_Metric_EventSet* eventSet,
                                       uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    if ( metricIndex < eventSet->number_of_metrics )
    {
        return eventSet->metrics[ metricIndex ].meta_data->unit;
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
scorep_metric_plugins_get_metric_properties( SCOREP_Metric_EventSet* eventSet,
                                             uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    SCOREP_Metric_Properties props;

    if ( metricIndex < eventSet->number_of_metrics )
    {
        props.name        = eventSet->metrics[ metricIndex ].meta_data->name;
        props.description = eventSet->metrics[ metricIndex ].meta_data->description;
        props.unit        = eventSet->metrics[ metricIndex ].meta_data->unit;
        props.mode        = eventSet->metrics[ metricIndex ].meta_data->mode;
        props.value_type  = eventSet->metrics[ metricIndex ].meta_data->value_type;
        props.base        = eventSet->metrics[ metricIndex ].meta_data->base;
        props.exponent    = eventSet->metrics[ metricIndex ].meta_data->exponent;

        props.source_type    = SCOREP_METRIC_SOURCE_TYPE_PLUGIN;
        props.profiling_type = SCOREP_METRIC_PROFILING_TYPE_INCLUSIVE;
    }
    else
    {
        props.name        = "";
        props.description = "";
        props.unit        = "";
        props.mode        = SCOREP_INVALID_METRIC_MODE;
        props.value_type  = SCOREP_INVALID_METRIC_VALUE_TYPE;
        props.base        = SCOREP_INVALID_METRIC_BASE;
        props.exponent    = 0;

        props.source_type    = SCOREP_INVALID_METRIC_SOURCE_TYPE;
        props.profiling_type = SCOREP_INVALID_METRIC_PROFILING_TYPE;
    }

    return props;
}

/** @brief  Determines clock rate with the help of PAPI.
 *
 *  @return Returns recent clock rate.
 */
static uint64_t
scorep_metric_plugins_clock_rate( void )
{
    // Resource usage counters don't provide a clock source.
    return 0;
}

/** Implementation of the metric source initialization/finalization struct */
const SCOREP_MetricSource SCOREP_Metric_Plugins =
{
    SCOREP_METRIC_SOURCE_TYPE_PLUGIN,
    &scorep_metric_plugins_register,
    &scorep_metric_plugins_initialize_source,
    &scorep_metric_plugins_initialize_location,
    &scorep_metric_plugins_free_event_set,
    &scorep_metric_plugins_finalize_location,
    &scorep_metric_plugins_finalize_source,
    &scorep_metric_plugins_deregister,
    &scorep_metric_plugins_strictly_synchronous_read,
    &scorep_metric_plugins_synchronous_read,
    &scorep_metric_plugins_asynchronous_read,
    &scorep_metric_plugins_get_number_of_metrics,
    &scorep_metric_plugins_get_metric_name,
    &scorep_metric_plugins_get_metric_description,
    &scorep_metric_plugins_get_metric_unit,
    &scorep_metric_plugins_get_metric_properties,
    &scorep_metric_plugins_clock_rate
};


/* *********************************************************************
 * Implementation of helper functions
 **********************************************************************/

/** @brief Set up Score-P event set data structure.
 *
 *  @return Returns Score-P event set initialized with default values.
 */
static SCOREP_Metric_EventSet*
create_metric_plugin_defines( void )
{
    SCOREP_Metric_EventSet* metric_plugin_defines = calloc( 1, sizeof( SCOREP_Metric_EventSet ) );
    UTILS_ASSERT( metric_plugin_defines );

    return metric_plugin_defines;
}
