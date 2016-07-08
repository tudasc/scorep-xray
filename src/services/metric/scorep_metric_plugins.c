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
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015-2016,
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
 *  @brief This module implements support for metric plugins.
 */

#include <config.h>

#include "SCOREP_Metric_Source.h"
#include "scorep_status.h"
#include "SCOREP_Location.h"

#include <scorep/SCOREP_MetricPlugins.h>
#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Memory.h>

#include <UTILS_Debug.h>
#include <UTILS_CStr.h>

#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>


#define BUFFER_SIZE 512

typedef struct meta_data_struct
{
    /** Meta data about metrics (e.g. metric name, units) */
    SCOREP_Metric_Plugin_MetricProperties* properties;
    /** Flag to indicate whether to call free() for this memory address or not */
    bool                                   free_memory;
} meta_data;

/** Per plugin library information */
typedef struct metric_plugin_struct
{
    /** Info from get_info() */
    SCOREP_Metric_Plugin_Info info;
    /** Handle (should be closed when finalize) */
    void*                     dlfcn_handle;
    /** Plugin name */
    char*                     plugin_name;
    /** Index of additional environment variable in @ additional_environment_variables array */
    uint32_t                  additional_event_environment_variable_index;
    /** Number of selected event_names */
    uint32_t                  num_selected_events;
    /** Selected event_names */
    char**                    selected_events;
    /** Meta data management */
    meta_data*                metrics_meta_data;
} metric_plugin;

/** Data structure to handle an individual metric provided by a plugin */
typedef struct individual_metric_struct
{
    /** ID, which was produced by the plugin */
    int32_t                                plugin_metric_id;
    /** Meta data about this metric (e.g. metric name, units) */
    SCOREP_Metric_Plugin_MetricProperties* meta_data;
    /** Time between calls to plugin */
    uint64_t                               delta_t;
    /** Last timestamp when the metric was recorded */
    uint64_t                               last_timestamp;

    /** Functions for getting a metric value */
    uint64_t ( * getValue )( int32_t );
    /** Functions for getting an optional metric value */
    bool ( * getOptionalValue )( int32_t,
                                 uint64_t* );
    /** Functions for getting multiple metric values */
    uint64_t ( * getAllValues )( int32_t,
                                 SCOREP_MetricTimeValuePair** );
} individual_metric;

/** Item in the list of metric data structure */
typedef struct metric_list_item_struct
{
    /** Array of individual metric specifications */
    individual_metric               metric;
    /** Next item in the list */
    struct metric_list_item_struct* next;
} metric_list_item;

/** Data structure of a metric event set */
struct SCOREP_Metric_EventSet
{
    /** Overall number of metrics within this event set */
    uint32_t          number_of_metrics;
    /** Array of individual metric specifications */
    metric_list_item* metrics_list;
};


/* *********************************************************************
 * Global variables
 **********************************************************************/

/** Static variable to control initialize status of the metric plugins source.
 *  If it is 0 it is initialized. */
static int metric_plugins_initialized = 1;

/** Static variable to check whether plugins are used.
 *  If it is true plugins are used. */
static bool are_metric_plugins_used = false;

/** A number of plugin libraries, [sync_type][library_nr] */
static metric_plugin* metric_plugin_handles[ SCOREP_METRIC_SYNC_TYPE_MAX ];

/* Number of selected plugins */
static uint32_t num_selected_plugins = 0;

/** Number of used plugins per sync type*/
static uint32_t num_plugins[ SCOREP_METRIC_SYNC_TYPE_MAX ];

/** Number of additional environment variables used to specify events of individual plugins */
static uint32_t num_additional_environment_variables;

#include "scorep_metric_plugins_confvars.inc.c"


/* *********************************************************************
 * Helper functions
 **********************************************************************/

static SCOREP_Metric_EventSet*
create_event_set( void );


/* *********************************************************************
 * Metric source management
 **********************************************************************/

/** @brief  Registers configuration variables for the metric sources.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
register_source( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " register metric plugins source!" );

    /* Register environment variables to specify used plugins */
    SCOREP_ErrorCode status;
    status = SCOREP_ConfigRegister( "metric", scorep_metric_plugins_confvars );
    if ( status != SCOREP_SUCCESS )
    {
        UTILS_ERROR( status,
                     "Registration of plugins configure variables failed." );
    }

    return SCOREP_SUCCESS;
}

/** @brief Called on deregistration of the metric source.
 */
static void
deregister_source( void )
{
    /* Free environment variables for plugin specification */

    free( scorep_metric_plugins );
    free( scorep_metric_plugins_separator );

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
 *  For metrics of other types (e.g. per-process) we cannot determine
 *  whether this location will be responsible to record metrics of this
 *  type. Responsibility will be determine while initializing location.
 *  Therefore, within this function we don't known how many additional
 *  metrics will be recorded by a specific location.
 *
 *  @return Returns the number of used 'strictly synchronous' metrics.
 */
static uint32_t
initialize_source( void )
{
    /* Number of used 'strictly synchronous' metrics */
    uint32_t metric_counts = 0;

    if ( metric_plugins_initialized )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialize plugins metric source." );
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " list of plugins = %s\n", scorep_metric_plugins );

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

        metric_plugin* current_plugin;

        /* Read content of environment variable */
        env_var_content = UTILS_CStr_dup( scorep_metric_plugins );

        /* Return if environment variable is empty */
        if ( strlen( env_var_content ) == 0 )
        {
            free( env_var_content );
            return 0;
        }

        /* Read plugin names from specification string */
        token = strtok( env_var_content, scorep_metric_plugins_separator );
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
            token = strtok( NULL, scorep_metric_plugins_separator );
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

            metric_plugin_handles[ info.sync ] =
                realloc( metric_plugin_handles[ info.sync ],
                         num_plugins[ info.sync ] * sizeof( metric_plugin ) );
            UTILS_BUG_ON( metric_plugin_handles[ info.sync ] == NULL, "Out of memory" );
            current_plugin = &metric_plugin_handles[ info.sync ][ num_plugins[ info.sync ] - 1 ];

            /* Clear out current plugin */
            memset( current_plugin, 0, sizeof( metric_plugin ) );

            SCOREP_ConfigVariable* variable = SCOREP_ConfigGetData( "metric", current_plugin_name );

            if ( !variable )
            {
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

                variable = SCOREP_ConfigGetData( "metric", current_plugin_name );
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
                current_plugin->info.set_clock_function( SCOREP_Timer_GetClockTicks );
            }

            /* Initialize plugin */
            if ( current_plugin->info.initialize() )
            {
                UTILS_WARNING( "Error while initializing plugin %s, initialization returned != 0\n", current_plugin_name );
                continue;
            }

            /*
             * Read events selected for current plugin
             *
             * For example, if the user set SCOREP_METRIC_<pluginname> to token1,token2,token3
             * the next loop will iterate over the tokens token1, token2, and token3.
             */
            if ( *( char** )variable->variableReference )
            {
                env_var_content = UTILS_CStr_dup( *( char** )variable->variableReference );
            }
            token = strtok( env_var_content, scorep_metric_plugins_separator );
            while ( token )
            {
                /*
                 * A plugin can extend one token to several metrics,
                 * e.g. the token 'token2' taken from the example above
                 * may result in metricB and metricC. In this case
                 * get_event_info() will return a list of
                 * SCOREP_Metric_Plugin_MetricProperties with two entries.
                 */
                SCOREP_Metric_Plugin_MetricProperties* metric_infos = info.get_event_info( token );
                UTILS_BUG_ON( metric_infos == NULL, "Error while initializing plugin metric %s, no info returned\n", token );
                UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Adding metric %s for plugin counter library: lib%s.so", token, current_plugin_name );

                /* Iterate over the info items */
                bool is_first_info_item = true;
                for ( SCOREP_Metric_Plugin_MetricProperties* current_metric_info = metric_infos; current_metric_info->name != NULL; current_metric_info++ )
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

                    /* Handle metric meta data */
                    current_plugin->metrics_meta_data =
                        realloc( current_plugin->metrics_meta_data, current_plugin->num_selected_events * sizeof( meta_data ) );
                    UTILS_BUG_ON( current_plugin->metrics_meta_data == NULL, "Out of memory" );

                    /* Set reference to meta data of current metric */
                    current_plugin->metrics_meta_data[ current_plugin->num_selected_events - 1 ].properties  = current_metric_info;
                    current_plugin->metrics_meta_data[ current_plugin->num_selected_events - 1 ].free_memory = is_first_info_item;

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
                    are_metric_plugins_used = true;

                    is_first_info_item = false;
                } /* End of: for all metrics related to the metric string */

                /* Handle next plugin */
                token = strtok( NULL, scorep_metric_plugins_separator );
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

        metric_plugins_initialized = 0;
    }

    return metric_counts;
}

/** @brief Metric source finalization.
 */
static void
finalize_source( void )
{
    /* Call only, if previously initialized */
    if ( !metric_plugins_initialized )
    {
        for ( uint32_t i = 0; i < SCOREP_METRIC_SYNC_TYPE_MAX; i++ )
        {
            for ( uint32_t j = 0; j < num_plugins[ i ]; j++ )
            {
                /* Call finalization function of plugin */
                metric_plugin_handles[ i ][ j ].info.finalize();

                /* Free resources */
                for ( uint32_t event = 0; event < metric_plugin_handles[ i ][ j ].num_selected_events; event++ )
                {
                    free( metric_plugin_handles[ i ][ j ].metrics_meta_data[ event ].properties->name );
                    if ( metric_plugin_handles[ i ][ j ].metrics_meta_data[ event ].free_memory )
                    {
                        free( metric_plugin_handles[ i ][ j ].metrics_meta_data[ event ].properties );
                    }
                }
                free( metric_plugin_handles[ i ][ j ].metrics_meta_data );
                free( metric_plugin_handles[ i ][ j ].selected_events );
                free( metric_plugin_handles[ i ][ j ].plugin_name );

                dlclose( metric_plugin_handles[ i ][ j ].dlfcn_handle );
            }
            free( metric_plugin_handles[ i ] );
            metric_plugin_handles[ i ] = NULL;
            num_plugins[ i ]           = 0;
        }

        /* Set metric plugin usage flag */
        are_metric_plugins_used = false;

        /* Set initialization flag */
        metric_plugins_initialized = 1;
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " finalize metric plugins source." );

        num_selected_plugins = 0;
    }
}

/** @brief  Location specific initialization function for metric sources.
 *
 *  @param location             Location data.
 *  @param event_sets           Event sets of all metrics.
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
initialize_location( struct SCOREP_Location*    location,
                     SCOREP_MetricSynchronicity syncType,
                     SCOREP_MetricPer           metricType )
{
    if ( !are_metric_plugins_used )
    {
        return NULL;
    }

    SCOREP_Metric_EventSet* event_set = NULL;
    metric_list_item*       new_item  = NULL;

    /* For all plugins of currently selected type */
    for ( uint32_t plugin_index = 0; plugin_index < num_plugins[ syncType ]; plugin_index++ )
    {
        /* Check whether this location is responsible to record metrics of requested sync type */
        metric_plugin* current_plugin = &metric_plugin_handles[ syncType ][ plugin_index ];

        if ( current_plugin->info.run_per != metricType )
        {
            continue;
        }

        if ( event_set == NULL )
        {
            event_set = create_event_set();
        }

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, "Score-P location (%p) adds own plugin metrics for plugin %s:",
                            location,
                            current_plugin->plugin_name );

        /* For all selected events */
        for ( uint32_t event = 0; event < current_plugin->num_selected_events; event++ )
        {
            /* Add select metric */
            if ( new_item == NULL )
            {
                new_item = SCOREP_Memory_AllocForMisc( sizeof( metric_list_item ) );
            }

            new_item->next                    = NULL;
            new_item->metric.meta_data        = current_plugin->metrics_meta_data[ event ].properties;
            new_item->metric.delta_t          = current_plugin->info.delta_t;
            new_item->metric.plugin_metric_id = current_plugin->info.add_counter( current_plugin->selected_events[ event ] );
            /* Check whether adding metric finished successfully */
            if ( new_item->metric.plugin_metric_id < 0 )
            {
                /*
                 * Adding this metric failed.
                 * Continue with the next metric.
                 * Reuse the new_item object.
                 */
                UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                             "Error while adding plugin metric \"%s\"\n",
                             current_plugin->selected_events[ event ] );
                continue;
            }

            /* Set function pointers corresponding to current synchronicity type */
            switch ( current_plugin->info.sync )
            {
                case SCOREP_METRIC_STRICTLY_SYNC:
                    /* Strictly synchronous plugins have to implement 'getValue' function */
                    new_item->metric.getValue = current_plugin->info.get_current_value;
                    break;

                case SCOREP_METRIC_SYNC:
                    /* Synchronous plugins have to implement 'getOptionalValue' function */
                    new_item->metric.getOptionalValue = current_plugin->info.get_optional_value;
                    break;

                case SCOREP_METRIC_ASYNC:
                case SCOREP_METRIC_ASYNC_EVENT:
                    /* Asynchronous plugins have to implement 'getAllValues' function */
                    new_item->metric.getAllValues = current_plugin->info.get_all_values;
                    break;

                default:
                    UTILS_WARNING( "Unknown metric synchronicity type." );
            }

            /* Add new_item to the list of all metrics */
            new_item->next          = event_set->metrics_list;
            event_set->metrics_list = new_item;

            /* Increment metric counter */
            event_set->number_of_metrics++;

            /* Reset new_item */
            new_item = NULL;
        } /* END for all selected events */
    }     /* END for all plugins of currently selected type */

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric source initialized location!" );

    return event_set;
}

/** @brief Location specific finalization function for metric sources.
 *
 *  @param eventSet  Reference to active set of metrics.
 */
static void
finalize_location( SCOREP_Metric_EventSet* eventSet )
{
    free( eventSet );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric source finalized location!" );
}

/** @brief Frees memory allocated for requested event set.
 *
 *  @param eventSet  Reference to active set of metrics.
 */
static void
free_event_set( SCOREP_Metric_EventSet* eventSet )
{
    free( eventSet );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric source freed event set!" );
}

/** @brief Optional synchronization callback.
 *
 *  @param syncMode  Synchronization mode (point in time when the synchronization
 *                   happens, e.g., begin or end of measurement)
 */
static void
synchronize( SCOREP_MetricSynchronizationMode syncMode )
{
    /* Call only, if previously initialized */
    if ( !metric_plugins_initialized )
    {
        for ( uint32_t i = 0; i < SCOREP_METRIC_SYNC_TYPE_MAX; i++ )
        {
            for ( uint32_t j = 0; j < num_plugins[ i ]; j++ )
            {
                /*
                 * Call synchronization function of plugin
                 * - synchronize() callback is supported since metric plugin interface version 1
                 * - synchronize() callback has to be set
                 * - Must have at least one event. This avoids calling synchronize on plugins where init failed
                 */

                if ( metric_plugin_handles[ i ][ j ].info.plugin_version > 0 &&
                     metric_plugin_handles[ i ][ j ].info.synchronize != NULL &&
                     metric_plugin_handles[ i ][ j ].num_selected_events > 0 )
                {
                    bool is_responsible = true;
                    switch ( metric_plugin_handles[ i ][ j ].info.run_per )
                    {
                        case SCOREP_METRIC_PER_HOST:
                            if ( !SCOREP_Status_IsProcessMasterOnNode()
                                 || SCOREP_Location_GetId( SCOREP_Location_GetCurrentCPULocation() ) != 0 )
                            {
                                is_responsible = false;
                            }
                            break;
                        case SCOREP_METRIC_ONCE:
                            if ( SCOREP_Status_GetRank() != 0
                                 || SCOREP_Location_GetId( SCOREP_Location_GetCurrentCPULocation() ) != 0 )
                            {
                                is_responsible = false;
                            }
                            break;
                        default:
                            break;
                    }
                    metric_plugin_handles[ i ][ j ].info.synchronize( is_responsible, syncMode );
                }
            }
        }

        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric source synchronized!" );
    }
}

/** @brief Reads values of all metrics in the active event set.
 *
 *  @param eventSet  Reference to active set of metrics.
 *  @param values    Reference to array that will be filled with values from active metrics.
 */
static void
strictly_synchronous_read( SCOREP_Metric_EventSet* eventSet,
                           uint64_t*               values )
{
    UTILS_ASSERT( eventSet );
    UTILS_ASSERT( values );

    uint32_t i = 0;
    for ( metric_list_item* item  = eventSet->metrics_list; item != NULL; item = item->next )
    {
        /* RonnyT @ todo move getValue function pointer from single metric to event set */
        UTILS_ASSERT( item->metric.getValue );
        UTILS_ASSERT( i < eventSet->number_of_metrics );

        values[ i ] = item->metric.getValue( item->metric.plugin_metric_id );
        i++;
    }
}

/** @brief Synchronous call to reads values of metrics in the active event set.
 *         The array @ isUpdated indicates whether a new value for corresponding
 *         metric was written or not.
 *
 *  @param      eventSet     An event set, that contains the definition of the counters
 *                           that should be measured.
 *  @param[out] values       Reference to array that will be filled with values from
 *                           active metrics.
 *  @param[out] isUpdated    An array which indicates whether a new value of a specfic
 *                           metric was written (@ isUpdated[i] == true ) or not
 *                           (@ isUpdated[i] == false ).
 *  @param      forceUpdate  Update of all metric value in this event set is enforced.
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

    uint64_t recent_timestamp = SCOREP_Timer_GetClockTicks();
    uint32_t i                = 0;
    for ( metric_list_item* item  = eventSet->metrics_list; item != NULL; item = item->next )
    {
        UTILS_ASSERT( i < eventSet->number_of_metrics );

        if ( ( recent_timestamp - item->metric.last_timestamp > item->metric.delta_t )
             || forceUpdate )
        {
            /* RonnyT @ todo move getOptionalValue function pointer from single metric to event set */
            UTILS_ASSERT( item->metric.getOptionalValue );

            isUpdated[ i ] = item->metric.getOptionalValue( item->metric.plugin_metric_id,
                                                            &( values[ i ] ) );
            item->metric.last_timestamp = recent_timestamp;
        }
        else
        {
            isUpdated[ i ] = false;
        }
        i++;
    }
}

/** @brief Reads values of all metrics in asynchronous event set.
 *
 *  @param      eventSet            An event set, that contains the definition of the counters
 *                                  that should be measured.
 *  @param[out] timeValuePairs      An array, to which the counter values are written.
 *  @param[out] numPairs            Number of pairs (timestamp + value) written for each
 *                                  individual metric.
 *  @param      forceUpdate         If true an update of all metric values in this event set is enforced.
 */
static void
asynchronous_read( SCOREP_Metric_EventSet*      eventSet,
                   SCOREP_MetricTimeValuePair** timeValuePairs,
                   uint64_t**                   numPairs,
                   bool                         forceUpdate )
{
    UTILS_ASSERT( eventSet );
    UTILS_ASSERT( timeValuePairs );

    uint64_t recent_timestamp = SCOREP_Timer_GetClockTicks();

    *numPairs = malloc( eventSet->number_of_metrics * sizeof( uint64_t ) );
    UTILS_ASSERT( *numPairs );

    uint32_t i = 0;
    for ( metric_list_item* item  = eventSet->metrics_list; item != NULL; item = item->next )
    {
        UTILS_ASSERT( i < eventSet->number_of_metrics );

        timeValuePairs[ i ] = NULL;

        if ( ( recent_timestamp - item->metric.last_timestamp > item->metric.delta_t )
             || forceUpdate )
        {
            /* RonnyT @ todo move getAllValues function pointer from single metric to event set */
            UTILS_ASSERT( item->metric.getAllValues );

            ( *numPairs )[ i ] = item->metric.getAllValues( item->metric.plugin_metric_id,
                                                            &( timeValuePairs[ i ] ) );
            item->metric.last_timestamp = recent_timestamp;
        }
        else
        {
            ( *numPairs )[ i ] = 0;
        }
        i++;
    }
}

/** @brief  Gets number of active metrics.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *
 *  @return Returns number of active metrics.
 */
static uint32_t
get_number_of_metrics( SCOREP_Metric_EventSet* eventSet )
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
get_metric_name( SCOREP_Metric_EventSet* eventSet,
                 uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    uint32_t i = 0;
    for ( metric_list_item* item  = eventSet->metrics_list; item != NULL; item = item->next )
    {
        UTILS_ASSERT( i < eventSet->number_of_metrics );

        if ( i == metricIndex )
        {
            return item->metric.meta_data->name;
        }
        i++;
    }

    return "";
}

/** @brief  Gets description of requested metric.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns description of requested metric.
 */
static const char*
get_metric_description( SCOREP_Metric_EventSet* eventSet,
                        uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    uint32_t i = 0;
    for ( metric_list_item* item  = eventSet->metrics_list; item != NULL; item = item->next )
    {
        UTILS_ASSERT( i < eventSet->number_of_metrics );

        if ( i == metricIndex )
        {
            return item->metric.meta_data->description;
        }
        i++;
    }

    return "";
}

/** @brief  Gets unit of requested metric.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns unit of requested metric.
 */
static const char*
get_metric_unit( SCOREP_Metric_EventSet* eventSet,
                 uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    uint32_t i = 0;
    for ( metric_list_item* item  = eventSet->metrics_list; item != NULL; item = item->next )
    {
        UTILS_ASSERT( i < eventSet->number_of_metrics );

        if ( i == metricIndex )
        {
            return item->metric.meta_data->unit;
        }
        i++;
    }

    return "";
}

/** @brief  Gets properties of requested metric.
 *
 *  @param  eventSet    Reference to active set of metrics.
 *  @param  metricIndex Index of requested metric.
 *
 *  @return Returns properties of requested metric.
 */
static SCOREP_Metric_Properties
get_metric_properties( SCOREP_Metric_EventSet* eventSet,
                       uint32_t                metricIndex )
{
    UTILS_ASSERT( eventSet );

    SCOREP_Metric_Properties props;

    uint32_t i = 0;
    for ( metric_list_item* item  = eventSet->metrics_list; item != NULL; item = item->next )
    {
        UTILS_ASSERT( i < eventSet->number_of_metrics );

        if ( i == metricIndex )
        {
            props.name           = item->metric.meta_data->name;
            props.description    = item->metric.meta_data->description;
            props.unit           = item->metric.meta_data->unit;
            props.mode           = item->metric.meta_data->mode;
            props.value_type     = item->metric.meta_data->value_type;
            props.base           = item->metric.meta_data->base;
            props.exponent       = item->metric.meta_data->exponent;
            props.source_type    = SCOREP_METRIC_SOURCE_TYPE_PLUGIN;
            props.profiling_type = SCOREP_METRIC_PROFILING_TYPE_INCLUSIVE;

            return props;
        }
        i++;
    }

    props.name           = "";
    props.description    = "";
    props.unit           = "";
    props.mode           = SCOREP_INVALID_METRIC_MODE;
    props.value_type     = SCOREP_INVALID_METRIC_VALUE_TYPE;
    props.base           = SCOREP_INVALID_METRIC_BASE;
    props.exponent       = 0;
    props.source_type    = SCOREP_INVALID_METRIC_SOURCE_TYPE;
    props.profiling_type = SCOREP_INVALID_METRIC_PROFILING_TYPE;

    return props;
}

/** Implementation of the metric source initialization/finalization struct */
const SCOREP_MetricSource SCOREP_Metric_Plugins =
{
    SCOREP_METRIC_SOURCE_TYPE_PLUGIN,
    &register_source,
    &initialize_source,
    &initialize_location,
    &synchronize,
    &free_event_set,
    &finalize_location,
    &finalize_source,
    &deregister_source,
    &strictly_synchronous_read,
    &synchronous_read,
    &asynchronous_read,
    &get_number_of_metrics,
    &get_metric_name,
    &get_metric_description,
    &get_metric_unit,
    &get_metric_properties
};


/* *********************************************************************
 * Implementation of helper functions
 **********************************************************************/

/** @brief Set up Score-P event set data structure.
 *
 *  @return Returns Score-P event set initialized with default values.
 */
static SCOREP_Metric_EventSet*
create_event_set( void )
{
    SCOREP_Metric_EventSet* event_set = calloc( 1, sizeof( SCOREP_Metric_EventSet ) );
    UTILS_ASSERT( event_set );

    return event_set;
}
