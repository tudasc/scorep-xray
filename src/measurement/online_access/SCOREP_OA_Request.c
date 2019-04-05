/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2014-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 *
 */

#include <config.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <SCOREP_OA_Request.h>
#include <scorep_status.h>
#include <scorep_profile_definition.h>
#include <SCOREP_Definitions.h>

#include <UTILS_CStr.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Profile.h>
#include <UTILS_Debug.h>
#include <SCOREP_Config.h>
#include <SCOREP_Task.h>
#include <SCOREP_Memory.h>


#define METRIC_DELIMITER ";"

/**
 * Internal hash-table storing metric requests using their name as a key. It is used to create
 * metric source configuration string later and to map Score-P metric definition codes back to
 * Periscope metric codes.
 */
static SCOREP_Hashtab* requests_by_name = NULL;

/**
 * Internal hash-table storing metric requests using their handle as a key. It is used when OA
 * sends measurements back to select requested metrics among the defined metrics.
 */
static SCOREP_Hashtab* requests_by_id = NULL;

/** Internal request handling statuses enum */
typedef enum requests_handling_status
{
    NOT_INITIALIZED,
    ACCEPTING,
    SUBMITTED
} requests_handling_status;

/** Internal status of the request handling */
static requests_handling_status requestsStatus         = NOT_INITIALIZED;

/** Counts the total size PAPI metric source configuration string */
static uint32_t size_of_papi_config_string             = 1;

/** Counts the total size PERF metric source configuration string */
static uint32_t size_of_perf_config_string             = 1;

/** Counts the total size RUSAGE metric source configuration string */
static uint32_t size_of_rusage_config_string           = 1;

/** Counts the total size PLUGIN metric source configuration string */
static uint32_t* size_of_plugin_config_string          = NULL;

/** Stores the maximum metric_definition id configured in the previous phase. All
 * the metrics having definition ID smaller then the one stored in this variable
 * will be ignored.
 */
static int32_t max_definition_id_previous_phase        = -1;

/**
 * The index of the next to be received request during accepting request state
 */
static int32_t phase_request_oa_index                  = 0;

/**
 * Since execution time dense metric doesn't get defined in the Score-P,
 * the request data for it is stored and accessed separately
 */
static SCOREP_OA_MetricRequest* execution_time_request = NULL;

/** Internal list of requested plugins */
struct plugin_array_struct
{
    char**   plugin_name;
    uint16_t max_entries;
    uint16_t next_free_entry;
} plugin_array;

/** Prints hash-table profile
 */
static void
print_hash_table_request( const SCOREP_Hashtab* hashTable,
                          char*                 tag );

/**
 * Indexes submitted requests for later use
 */
static inline void
request_submit( const char*             metricName,
                uint32_t                metricId,
                SCOREP_MetricSourceType metricSourceType );

/**
 * Helper function to add request for the execution time. Execution
 * time has to be handled separately since there is no definition
 * for time measurements in Score-P
 */
static inline void
request_exec_time_submit( void );

/**
 * Helper function to free SCOREP_OA_MetricRequest data structure
 */
static void
free_metric_request( void* request );

/**
 * Helper function to add plugin into the list of plugins
 */
static inline int16_t
find_plugin_index( char* pluginName );

/**
 * Helper function to initialize data structures for the request
 */
static inline void
init_requests( void );

/**
 * Helper function to free data structures used for the request
 */
static inline void
finalize_requests( void );

inline void
scorep_oa_requests_begin( void )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Begin requests..." );

    /** Check accepting requests is not already initialized*/
    UTILS_ASSERT( requestsStatus == NOT_INITIALIZED );
    if ( requestsStatus != NOT_INITIALIZED )
    {
        return;
    }
    /** Create hashtab storing requested metrics by name*/
    requests_by_name = SCOREP_Hashtab_CreateSize( 11, &SCOREP_Hashtab_HashString,
                                                  &SCOREP_Hashtab_CompareStrings );
    UTILS_ASSERT( requests_by_name );

    /** set the initial length of papi, rusage, perf and plugin configuration strings to 1 to account for a null termination character */
    size_of_papi_config_string   = 1;
    size_of_rusage_config_string = 1;
    size_of_perf_config_string   = 1;

    init_requests();

    for ( uint16_t i = 0; i < plugin_array.max_entries; i++ )
    {
        size_of_plugin_config_string[ i ] = 1;
    }
    phase_request_oa_index = 0;
    execution_time_request = NULL;

    requestsStatus = ACCEPTING;
}

inline void
scorep_oa_requests_end( void )
{
    UTILS_ASSERT( requestsStatus == ACCEPTING );
    if ( requestsStatus != ACCEPTING )
    {
        return;
    }

    char*  scorep_metrics_papi_mine   = NULL;
    char*  scorep_metrics_perf_mine   = NULL;
    char*  scorep_metrics_rusage_mine = NULL;
    char** scorep_metrics_plugin_mine = NULL;

    /** Allocate new config strings according to the total length of the requested metric strings*/
    scorep_metrics_papi_mine = calloc( size_of_papi_config_string, sizeof( char ) );
    UTILS_ASSERT( scorep_metrics_papi_mine );
    scorep_metrics_rusage_mine = calloc( size_of_rusage_config_string, sizeof( char ) );
    UTILS_ASSERT( scorep_metrics_rusage_mine );
    scorep_metrics_perf_mine = calloc( size_of_perf_config_string, sizeof( char ) );
    UTILS_ASSERT( scorep_metrics_perf_mine );
    scorep_metrics_plugin_mine = calloc( plugin_array.next_free_entry, sizeof( char* ) );
    UTILS_ASSERT( scorep_metrics_plugin_mine );
    for ( uint16_t i = 0; i < plugin_array.next_free_entry; i++ )
    {
        scorep_metrics_plugin_mine[ i ] = calloc( size_of_plugin_config_string[ i ], sizeof( char ) );
        UTILS_ASSERT( scorep_metrics_plugin_mine[ i ] );
    }

    /* Go over all requested metrics and store them to the corresponding metric source configuration string*/
    int                      put_delimiter_papi   = 0;
    int                      put_delimiter_perf   = 0;
    int                      put_delimiter_rusage = 0;
    int                      put_delimiter_plugin = 0;
    SCOREP_Hashtab_Iterator* iter                 = SCOREP_Hashtab_IteratorCreate( requests_by_name );
    SCOREP_Hashtab_Entry*    entry                = SCOREP_Hashtab_IteratorFirst( iter );
    while ( entry )
    {
        /** recover the index of the requested metric in the Periscope metric list array */
        SCOREP_OA_MetricRequest* metric_request = ( ( SCOREP_OA_MetricRequest* )entry->value.ptr );

        /** add the requested metric name to the corresponding metric source configuration string */
        switch ( metric_request->metric_source )
        {
            case SCOREP_METRIC_SOURCE_PAPI:
                if ( put_delimiter_papi )
                {
                    strcat( scorep_metrics_papi_mine, METRIC_DELIMITER );
                }
                strcat( scorep_metrics_papi_mine, ( char* )entry->key );
                put_delimiter_papi = 1;
                break;
            case SCOREP_METRIC_SOURCE_PERF:
                if ( put_delimiter_perf )
                {
                    strcat( scorep_metrics_perf_mine, METRIC_DELIMITER );
                }
                strcat( scorep_metrics_perf_mine, ( char* )entry->key );
                put_delimiter_perf = 1;
                break;
            case SCOREP_METRIC_SOURCE_RUSAGE:
                if ( put_delimiter_rusage )
                {
                    strcat( scorep_metrics_rusage_mine, METRIC_DELIMITER );
                }
                strcat( scorep_metrics_rusage_mine, ( char* )entry->key );
                put_delimiter_rusage = 1;
                break;
            case SCOREP_METRIC_SOURCE_PLUGIN:
                if ( put_delimiter_plugin )
                {
                    strcat( scorep_metrics_plugin_mine[ metric_request->plugin_index ], METRIC_DELIMITER );
                }
                strcat( scorep_metrics_plugin_mine[ metric_request->plugin_index ], ( char* )entry->key );
                put_delimiter_plugin = 1;
                break;
            default:
                break;
        }
        entry = SCOREP_Hashtab_IteratorNext( iter );
    }
    SCOREP_Hashtab_IteratorFree( iter );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Requested PAPI metrics: %s", scorep_metrics_papi_mine );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Requested PERF metrics: %s", scorep_metrics_perf_mine );
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Requested RUSAGE metrics: %s", scorep_metrics_rusage_mine );
    for ( uint16_t i = 0; i < plugin_array.next_free_entry; i++ )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Requested metrics for plugin %s: %s", plugin_array.plugin_name[ i ], scorep_metrics_plugin_mine[ i ] );
    }

    /**
     * Set metric source configuration string to the corresponding config variable
     */
    #if HAVE( PAPI )
    SCOREP_ConfigSetValue( "metric", "papi", scorep_metrics_papi_mine );
    SCOREP_ConfigSetValue( "metric", "papi_sep", METRIC_DELIMITER );
    #endif

    #if HAVE( METRIC_PERF )
    SCOREP_ConfigSetValue( "metric", "perf", scorep_metrics_perf_mine );
    SCOREP_ConfigSetValue( "metric", "perf_sep", METRIC_DELIMITER );
    #endif

    #if HAVE( GETRUSAGE )
    SCOREP_ConfigSetValue( "metric", "rusage", scorep_metrics_rusage_mine );
    SCOREP_ConfigSetValue( "metric", "rusage_sep", METRIC_DELIMITER );
    #endif

    /* Registers an environment variable for the plugin and plugin metrics. */
    /* Restarts the metric subsystem once to enforce it to read plugin and metric requests. */
    #if HAVE( DLFCN_SUPPORT )
    for ( uint16_t i = 0; i < plugin_array.next_free_entry; i++ )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Configuring metric plugin %s to %s", plugin_array.plugin_name[ i ], scorep_metrics_plugin_mine[ i ] );
        SCOREP_ConfigSetValue( "metric", plugin_array.plugin_name[ i ], scorep_metrics_plugin_mine[ i ] );
    }

    #endif

    /** free PAPI, PERF, RUsage, plugin config strings */
    free( scorep_metrics_papi_mine );
    free( scorep_metrics_perf_mine );
    free( scorep_metrics_rusage_mine );
    for ( uint16_t i = 0; i < plugin_array.next_free_entry; i++ )
    {
        free( scorep_metrics_plugin_mine[ i ] );
    }
    free( scorep_metrics_plugin_mine );

    /** Reinitialize metrics module */
    SCOREP_ErrorCode return_value = SCOREP_Metric_Reinitialize();
    UTILS_ASSERT( return_value == SCOREP_SUCCESS );

    /** Reinitialize profile module */
    // Finalize profile
    size_t substrate_id = SIZE_MAX; // denotes invalid substrate id
    if ( scorep_profile.is_initialized )
    {
        substrate_id = SCOREP_Profile_Finalize();
    }

    /* Cleanup the task stack */
    SCOREP_Task_ClearCurrent();

    // Initialize profile
    if ( !scorep_profile.is_initialized && scorep_profile.reinitialize )
    {
        UTILS_BUG_ON( substrate_id == SIZE_MAX, "Cannot initialize profiling substrate. "
                      "First initialization should be done in substrates_subsystem_init()." );
        SCOREP_Profile_Initialize( substrate_id /* number_of_metrics, metrics */ );
    }

    /** Create hashtab storing requested metrics by definition ID*/
    requests_by_id = SCOREP_Hashtab_CreateSize( 11, &SCOREP_Hashtab_HashInt32,
                                                &SCOREP_Hashtab_CompareInt32 );

    UTILS_ASSERT( requests_by_id );

    int32_t max_definition_id_this_phase = max_definition_id_previous_phase;

    /**
     * Go over defined metrics, check if the metric was requested and if so, store it in the
     * hash table by the metric definition id for later use
     */
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager, Metric, metric )
    {
        if ( definition->name_handle != SCOREP_INVALID_STRING )
        {
            const char* metric_name = SCOREP_MetricHandle_GetName( handle );
            uint32_t    metric_id   =  SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric );
            request_submit( metric_name, metric_id, definition->source_type );
        }

        if ( ( int32_t )SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric ) > max_definition_id_this_phase )
        {
            max_definition_id_this_phase = ( int32_t )SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    max_definition_id_previous_phase = max_definition_id_this_phase;

    /** metric request by name hash table is not needed any longer, free it */
    SCOREP_Hashtab_FreeAll( requests_by_name,
                            &SCOREP_Hashtab_DeleteFree,
                            &SCOREP_Hashtab_DeleteFree );
    requests_by_name = NULL;

#if HAVE( SCOREP_DEBUG )
    print_hash_table_request( requests_by_id, "submitted requests" );
#endif /* HAVE( SCOREP_DEBUG ) */

    requestsStatus = SUBMITTED;
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "End requests...\n\n" );
}

static inline void
request_exec_time_submit( void )
{
    if ( execution_time_request )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Execution time requested second time during this session..." );
        return;
    }
    execution_time_request = calloc( 1, sizeof( SCOREP_OA_MetricRequest ) );
    UTILS_ASSERT( execution_time_request );
    execution_time_request->metric_source = SCOREP_METRIC_TIMER;
    execution_time_request->oa_index      = phase_request_oa_index++;
    execution_time_request->metric_name   = UTILS_CStr_dup( "execution_time" );
}

static inline void
request_submit( const char*             metricName,
                uint32_t                metricId,
                SCOREP_MetricSourceType metricSourceType )
{
    /**
     * Since the definitions are duplicated when the metric module is reinitialized, the duplicates have to
     * be ignored when initializing request table. Therefore all the definitions having IDs less then the maximum
     * definition ID from the previous phase (stored in max_definition_id_previous_phase) are ignored. This is
     * only applied to the PAPI, PERF, RUSAGE and PLUGIN metrics
     */
    if ( ( int32_t )metricId <= max_definition_id_previous_phase &&
         ( metricSourceType == SCOREP_METRIC_SOURCE_TYPE_PAPI ||
           metricSourceType == SCOREP_METRIC_SOURCE_TYPE_PERF ||
           metricSourceType == SCOREP_METRIC_SOURCE_TYPE_RUSAGE ||
           metricSourceType == SCOREP_METRIC_SOURCE_TYPE_PLUGIN ) )
    {
        UTILS_WARNING( "Metric %d was defined in previous phase.", metricId );
        return;
    }

    SCOREP_Hashtab_Entry* entry = NULL;

    /** Search the metric name in the table of the requested metrics */
    entry = SCOREP_Hashtab_Find( requests_by_name,
                                 &( *metricName ),
                                 NULL );

    /** If the metric name was not found and the metric is RUSAGE metric search for "all" name
     * which is a request enabling all RUSAGE metrics
     */
    if ( !entry && metricSourceType == SCOREP_METRIC_SOURCE_TYPE_RUSAGE )
    {
        entry = SCOREP_Hashtab_Find( requests_by_name,
                                     &( "all" ),
                                     NULL );
    }

    if ( entry )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Metric %s,%d was found among requests.", metricName, metricId );

        /** Create request key, which is the definition id of the metric*/
        int32_t* request_key = calloc( 1, sizeof( int32_t ) );
        UTILS_ASSERT( request_key );
        *request_key = ( int32_t )metricId;

        /** Create value for the hash table entry. It carries source of the request associated with metric,
         * request type and  index of this metric in the Periscope metrics list if applicable*/
        SCOREP_OA_MetricRequest* request_value = calloc( 1, sizeof( SCOREP_OA_MetricRequest ) );
        UTILS_ASSERT( request_value );
        request_value->metric_source = ( ( SCOREP_OA_MetricRequest* )entry->value.ptr )->metric_source;
        request_value->oa_index      = phase_request_oa_index++;
        request_value->metric_name   = UTILS_CStr_dup( metricName );

        SCOREP_Hashtab_InsertPtr( requests_by_id,
                                  ( void* )( request_key ),
                                  ( void* )( request_value ),
                                  NULL );
    }
    else
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Metric %s,%d was NOT found among requests.", metricName, metricId );
    }
}

static inline void
init_requests( void )
{
    plugin_array.max_entries     = 8;
    plugin_array.next_free_entry = 0;
    plugin_array.plugin_name     = ( char** )malloc( plugin_array.max_entries * sizeof( char* ) );
    UTILS_ASSERT( plugin_array.plugin_name );
    size_of_plugin_config_string = ( uint32_t* )malloc( plugin_array.max_entries * sizeof( uint32_t ) );
    UTILS_ASSERT( size_of_plugin_config_string );
}

static inline void
finalize_requests( void )
{
    for ( uint16_t i = 0; i < plugin_array.next_free_entry; i++ )
    {
        free( plugin_array.plugin_name[ i ] );
    }
    free( size_of_plugin_config_string );
    free( plugin_array.plugin_name );
}

static int16_t
find_plugin_index( char* pluginName )
{
    int16_t i;
    for ( i = 0; i < plugin_array.next_free_entry; i++ )
    {
        if ( !strcmp( plugin_array.plugin_name[ i ], pluginName ) )
        {
            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "plugin %s found on location %d", pluginName, i );
            return i;
        }
    }

    if ( plugin_array.max_entries == plugin_array.next_free_entry )
    {
        plugin_array.max_entries <<= 1;
        plugin_array.plugin_name   = ( char** )realloc( plugin_array.plugin_name, plugin_array.max_entries );
        UTILS_BUG_ON( plugin_array.plugin_name == NULL, "Out of memory." );
    }

    plugin_array.plugin_name[ i ] = pluginName;
    plugin_array.next_free_entry++;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "plugin %s added to location %d", pluginName, i );
    return i;
}

void
scorep_oa_requests_add_metric_by_name( char*                   metricName,
                                       char*                   pluginName,
                                       scorep_oa_metric_source metricSource )
{
    UTILS_ASSERT( requestsStatus == ACCEPTING );
    if ( requestsStatus != ACCEPTING )
    {
        return;
    }

    if ( metricSource == SCOREP_METRIC_SOURCE_PLUGIN )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Requested metric %s for plugin %s", metricName, pluginName );
    }
    else
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Requested metric %s", metricName );
    }

    SCOREP_Hashtab_Entry* entry = NULL;

    if ( metricSource == SCOREP_METRIC_SOURCE_NOT_SUPPORTED )
    {
        free( metricName );
        free( pluginName );
        return;
    }

    /**
     * Rusage metrics request "ALL" keyword has to be lowercased in order to be accepted by metrics module
     */
    if ( metricSource == SCOREP_METRIC_SOURCE_RUSAGE )
    {
        if ( strcmp( metricName, "ALL" ) == 0 )
        {
            for ( int i = 0; metricName[ i ] != '\0'; i++ )
            {
                metricName[ i ] = ( char )tolower( metricName[ i ] );
            }
        }
    }

    /**
     * Handle execution time request separately due to missing definition
     */
    if ( strcmp( metricName, "execution_time" ) == 0 )
    {
        request_exec_time_submit();
        free( metricName );
        free( pluginName );
        return;
    }
    /** Search for already added similar metric request */
    entry = SCOREP_Hashtab_Find( requests_by_name,
                                 &( *metricName ),
                                 NULL );
    /** If not found, add metric request*/
    if ( !entry )
    {
        /** Create request key out of the name. Simply reuse the string since it was already duplicated in the parser*/
        char* request_key = metricName;

        UTILS_ASSERT( request_key );

        /** Create value for the hash table entry. It carries the index of this metric in the Periscope metrics list*/
        SCOREP_OA_MetricRequest* request_value = calloc( 1, sizeof( SCOREP_OA_MetricRequest ) );
        UTILS_ASSERT( request_value );
        request_value->metric_source = metricSource;
        request_value->metric_name   = NULL;
        if ( metricSource == SCOREP_METRIC_SOURCE_PLUGIN )
        {
            request_value->plugin_index = find_plugin_index( pluginName );
        }

        SCOREP_Hashtab_InsertPtr( requests_by_name,
                                  ( void* )( request_key ),
                                  ( void* )( request_value ),
                                  NULL );
        /** If it is a PAPI metric, add the size of the metric name string to the total size of papi
         *  metric name strings. Account for the delimiters between metric names. It will be later
         *  used to construct Score-P metric configuration string */
        if ( metricSource == SCOREP_METRIC_SOURCE_PAPI )
        {
            size_of_papi_config_string += strlen( request_key ) + strlen( METRIC_DELIMITER );
        }
        /** If it is a PERF metric, add the size of the metric name string to the total size of perf
         *  metric name strings. Account for the delimiters between metric names. It will be later
         *  used to construct Score-P metric configuration string */
        if ( metricSource == SCOREP_METRIC_SOURCE_PERF )
        {
            /** add string length + delimiter length + delimiter null termination */
            size_of_perf_config_string += strlen( request_key ) + strlen( METRIC_DELIMITER );
        }
        /** If it is a RUSAGE metric, add the size of the metric name string to the total size of rusage
         *  metric name strings. Account for the delimiters between metric names. It will be later
         *  used to construct Score-P metric configuration string */
        if ( metricSource == SCOREP_METRIC_SOURCE_RUSAGE )
        {
            /** add string length + delimiter length + delimiter null termination */
            size_of_rusage_config_string += strlen( request_key ) + strlen( METRIC_DELIMITER );
        }
        /** If it is a PLUGIN metric, add the size of the metric name string to the total size of plugin
         *  metric name strings. Account for the delimiters between metric names. It will be later
         *  used to construct Score-P metric configuration string */
        if ( metricSource == SCOREP_METRIC_SOURCE_PLUGIN )
        {
            /** add string length + delimiter length + delimiter null termination */
            size_of_plugin_config_string[ request_value->plugin_index ] += strlen( request_key ) + strlen( METRIC_DELIMITER );
        }
    }
    else
    {
        /** Free metric name pointer since the metric name was duplicated by MRI parser
         * before being passed to this function
         */
        free( metricName );
    }
}

SCOREP_OA_MetricRequest*
SCOREP_OA_RequestGet( uint32_t metricId )
{
    UTILS_ASSERT( requestsStatus == SUBMITTED );
    if ( requestsStatus != SUBMITTED )
    {
        return NULL;
    }

    int32_t               metric_ID_int = ( int32_t )metricId;
    SCOREP_Hashtab_Entry* entry;

    /** Search the metric ID in the table of the requested metrics IDs */
    entry = SCOREP_Hashtab_Find( requests_by_id,
                                 &( metric_ID_int ),
                                 NULL );

    if ( entry )
    {
        return ( SCOREP_OA_MetricRequest* )entry->value.ptr;
    }
    else
    {
        return NULL;
    }
}

SCOREP_OA_MetricRequest*
SCOREP_OA_RequestGetExecutionTime( void )
{
    return execution_time_request;
}

uint32_t
SCOREP_OA_GetNumberOfRequests( void )
{
    UTILS_ASSERT( requestsStatus == SUBMITTED );

    if ( requestsStatus != SUBMITTED )
    {
        return 0;
    }
    uint32_t return_value = ( uint32_t )SCOREP_Hashtab_Size( requests_by_id );

    if ( execution_time_request )
    {
        return_value++;
    }

    return return_value;
}

void
SCOREP_OA_RequestsDismiss( void )
{
    UTILS_ASSERT( requestsStatus == SUBMITTED );

    if ( requestsStatus != SUBMITTED )
    {
        return;
    }

    /** Free metric request hash table */
    SCOREP_Hashtab_FreeAll( requests_by_id,
                            &SCOREP_Hashtab_DeleteFree,
                            &free_metric_request );
    requests_by_id = NULL;
    if ( execution_time_request )
    {
        if ( execution_time_request->metric_name )
        {
            free( execution_time_request->metric_name );
        }
        free( execution_time_request );
    }
    requestsStatus = NOT_INITIALIZED;

    finalize_requests();
}

static void
free_metric_request( void* request )
{
    if ( !request )
    {
        return;
    }
    SCOREP_OA_MetricRequest* m_request = ( SCOREP_OA_MetricRequest* )request;
    if ( m_request->metric_name )
    {
        free( m_request->metric_name );
    }
    free( m_request );
}

#if HAVE( SCOREP_DEBUG )
static void
print_hash_table_request( const SCOREP_Hashtab* hashTable,
                          char*                 tag )
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "\n/////////////%s///////////\n", tag );
    SCOREP_Hashtab_Iterator* iter;
    SCOREP_Hashtab_Entry*    entry;

    iter  = SCOREP_Hashtab_IteratorCreate( hashTable );
    entry = SCOREP_Hashtab_IteratorFirst( iter );
    while ( entry )
    {
        if ( entry->key )
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Item (%d)-", ( *( int32_t* )entry->key ) );
        }
        else
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Item (X)-" );
        }
        if ( entry->value.ptr )
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "(%d,%d,%s,%d)\n",
                                    ( ( SCOREP_OA_MetricRequest* )entry->value.ptr )->metric_source,
                                    ( ( SCOREP_OA_MetricRequest* )entry->value.ptr )->oa_index,
                                    ( ( SCOREP_OA_MetricRequest* )entry->value.ptr )->metric_name,
                                    ( ( SCOREP_OA_MetricRequest* )entry->value.ptr )->plugin_index );
        }
        else
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "(X,X,X,X)\n" );
        }
        entry = SCOREP_Hashtab_IteratorNext( iter );
    }
    SCOREP_Hashtab_IteratorFree( iter );
    if ( execution_time_request )
    {
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Item (T)-(%d,%d,%s,%d)\n",
                                execution_time_request->metric_source,
                                execution_time_request->oa_index,
                                execution_time_request->metric_name,
                                execution_time_request->plugin_index );
    }

    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "///////////////////////////\n\n" );
}
#endif /* HAVE( SCOREP_DEBUG ) */
#undef METRIC_DELIMITER
