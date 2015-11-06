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
 * Copyright (c) 2009-2011,
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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <SCOREP_OA_Request.h>
#include <scorep_status.h>
#include <scorep_profile_definition.h>
#include <SCOREP_Definitions.h>

#include <UTILS_CStr.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_OA_PeriscopeMetrics.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Profile.h>
#include <UTILS_Debug.h>
#include <SCOREP_Config.h>
#include <SCOREP_Task.h>
#include <SCOREP_Memory.h>


/** Helper function mapping Periscope metric groups to Score-P metric sources*/
static OA_MetricSource
get_scorep_metric_source( PSC_MetricGroup metric_group );

/**
 * Internal hash-table storing metric requests using their name as a key. It is used to create
 * metric source configuration string later and to map Score-P metric definition codes back to
 * Periscope metric codes.
 */
static SCOREP_Hashtab* requestsByName = NULL;

/**
 * Internal hash-table storing metric requests using their handle as a key. It is used when OA is
 * sends measurements back to select requested metrics among the defined metrics.
 */
static SCOREP_Hashtab* requestsByID = NULL;

/** Internal request handling statuses enum */
typedef enum RequestsHandlingStatus
{
    NOT_INITIALIZED,
    ACCEPTING,
    SUBMITTED
}RequestsHandlingStatus;

/** Internal status of the request handling */
static RequestsHandlingStatus requestsStatus    = NOT_INITIALIZED;


/** Counts the total size PAPI metric source configuration string */
static uint32_t size_of_papi_config_string      = 1;

/** Counts the total size RUSAGE metric source configuration string */
static uint32_t size_of_rusage_config_string    = 1;

/** Stores the maximum metric_definition id configured in the previous phase. All
 * the metrics having definition ID smaller then the one stored in this variable
 * will be ignored.
 */
static int32_t max_definition_id_previous_phase = -1;

/**
 * The index of the next to be received request during accepting request state
 */
static int32_t phase_request_oa_index           = 0;

/**
 * Since execution time dense metric doesn't get defined in the Score-P,
 * the request data for it is stored and accessed separately
 */
static MetricRequest* execution_time_request    = NULL;

/** Prints call-tree profile
 */
extern void
scorep_profile_dump_subtree( scorep_profile_node* node,
                             uint32_t             level );

/** Prints hash-table profile
 */
static void
print_hash_table_request( const SCOREP_Hashtab* hash_table,
                          char*                 tag );

/**
 * Indexes submitted requests for later use
 */
void
scorep_oa_request_submit( const char*             metric_name,
                          uint32_t                metric_id,
                          SCOREP_MetricSourceType metric_source_type );

/**
 * Helper function to add request for the execution time. Execution
 * time has to be handeled separately since there is no definition
 * for time measurements in Score-P
 */
void
scorep_oa_request_exec_time_submit( void );

/**
 * Helper function to free MetricRequest data structure
 */
static void
free_metric_request( void* request );

void
SCOREP_OA_RequestBegin( void )
{
    /** Check accepting requests is not already initialized*/
    assert( requestsStatus == NOT_INITIALIZED );
    if ( requestsStatus != NOT_INITIALIZED )
    {
        return;
    }
    /** Create hashtab storing requested metrics by name*/
    requestsByName = SCOREP_Hashtab_CreateSize( 11, &SCOREP_Hashtab_HashString,
                                                &SCOREP_Hashtab_CompareStrings );
    assert( requestsByName );

    /** set the intial length of papi and rusage configuration strings to 1 to account for a null termination caracter */
    size_of_papi_config_string   = 1;
    size_of_rusage_config_string = 1;
    phase_request_oa_index       = 0;
    execution_time_request       = NULL;

    requestsStatus = ACCEPTING;
}

void
SCOREP_OA_RequestsSubmit( void )
{
    assert( requestsStatus == ACCEPTING );
    if ( requestsStatus != ACCEPTING )
    {
        return;
    }

    char* scorep_metrics_papi_mine   = NULL;
    char* scorep_metrics_rusage_mine = NULL;

    /** if PAPI config string is already allocated free it*/
    if ( scorep_metrics_papi_mine )
    {
        free( scorep_metrics_papi_mine );
    }
    /** if rusage config string is already allocated free it*/
    if ( scorep_metrics_rusage_mine )
    {
        free( scorep_metrics_rusage_mine );
    }

    /** Allcoate new config strings according to the total length of the requested metric strings*/
    scorep_metrics_papi_mine = calloc( size_of_papi_config_string, sizeof( char ) );
    assert( scorep_metrics_papi_mine );
    scorep_metrics_rusage_mine = calloc( size_of_rusage_config_string, sizeof( char ) );
    assert( scorep_metrics_rusage_mine );

    /* Go over all requested metrics and store them to the corresponding metric source configuration string*/
    int                      put_delimeter_papi   = 0;
    int                      put_delimeter_rusage = 0;
    SCOREP_Hashtab_Iterator* iter                 = SCOREP_Hashtab_IteratorCreate( requestsByName );
    SCOREP_Hashtab_Entry*    entry                = SCOREP_Hashtab_IteratorFirst( iter );
    while ( entry )
    {
        /** recover the index of the requested metric in the Periscope metric list array */
        MetricRequest* metric_request = ( ( MetricRequest* )entry->value );

        /** add the requested metric name to the corresponding metric source configuration string */
        if ( metric_request->metric_source == SCOREP_METRIC_SOURCE_PAPI )
        {
            if ( put_delimeter_papi )
            {
                strcat( scorep_metrics_papi_mine, ";" );
            }
            strcat( scorep_metrics_papi_mine, ( char* )entry->key );
            put_delimeter_papi = 1;
        }
        if ( metric_request->metric_source == SCOREP_METRIC_SOURCE_RUSAGE )
        {
            if ( put_delimeter_rusage )
            {
                strcat( scorep_metrics_rusage_mine, ";" );
            }
            strcat( scorep_metrics_rusage_mine, ( char* )entry->key );
            put_delimeter_rusage = 1;
        }
        entry = SCOREP_Hashtab_IteratorNext( iter );
    }
    SCOREP_Hashtab_IteratorFree( iter );

    //printf( "Requested PAPI metrics: %s\n", scorep_metrics_papi_mine );
    //printf( "Requested RUSAGE metrics: %s\n", scorep_metrics_rusage_mine );

    /**
     * Set metric source configuration string to the corresponding config variable
     */
    #if HAVE( PAPI )
    SCOREP_ConfigSetValue( "metric", "papi", scorep_metrics_papi_mine );
    SCOREP_ConfigSetValue( "metric", "papi_sep", ";" );
    #endif

    #if HAVE( GETRUSAGE )
    SCOREP_ConfigSetValue( "metric", "rusage", scorep_metrics_rusage_mine );
    SCOREP_ConfigSetValue( "metric", "rusage_sep", ";" );
    #endif
    /** if PAPI config string is already allocated free it*/
    if ( scorep_metrics_papi_mine )
    {
        free( scorep_metrics_papi_mine );
    }
    /** if rusage config string is already allocated free it*/
    if ( scorep_metrics_rusage_mine )
    {
        free( scorep_metrics_rusage_mine );
    }

    /** Reinitialize metrics module */
    SCOREP_ErrorCode return_value = SCOREP_Metric_Reinitialize();
    assert( return_value == SCOREP_SUCCESS );

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
    requestsByID =      SCOREP_Hashtab_CreateSize( 11, &SCOREP_Hashtab_HashInt32,
                                                   &SCOREP_Hashtab_CompareInt32 );



    assert( requestsByID );

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
            scorep_oa_request_submit( metric_name, metric_id, definition->source_type );
        }

        if ( ( int32_t )SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric ) > max_definition_id_this_phase )
        {
            max_definition_id_this_phase = ( int32_t )SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    max_definition_id_previous_phase = max_definition_id_this_phase;

    /** metric request by name hash table is not needed any longer, free it */
    SCOREP_Hashtab_FreeAll( requestsByName, &free, &free );
    requestsByName = NULL;

    //print_hash_table_request( requestsByID, "submitted requests" );

    requestsStatus = SUBMITTED;
}

void
scorep_oa_request_exec_time_submit( void )
{
    if ( execution_time_request )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Execution time requested second time during this session..." );
        return;
    }
    execution_time_request = calloc( 1, sizeof( MetricRequest ) );
    assert( execution_time_request );
    execution_time_request->request_type  = SCOREP_REQUEST_UNDEFINED_SOURCE;
    execution_time_request->metric_source = SCOREP_METRIC_TIMER;
    execution_time_request->oa_index      = phase_request_oa_index++;
    execution_time_request->psc_index     = 0;
    execution_time_request->metric_name   = UTILS_CStr_dup( "execution_time" );
}

void
scorep_oa_request_submit( const char*             metric_name,
                          uint32_t                metric_id,
                          SCOREP_MetricSourceType metric_source_type )
{
    /**
     * Since the definitions are duplicated when the metric module is reinitialized, the duplicats have to
     * be ignored when initializing request table. Therefore all the definitions having IDs less then the maximum
     * definition ID from the previous phase (stored in max_definition_id_previous_phase) are ignored. This is
     * only applied to the PAPI and RUSAGE metrics
     */
    if ( ( int32_t )metric_id <= max_definition_id_previous_phase &&
         ( metric_source_type == SCOREP_METRIC_SOURCE_TYPE_PAPI || metric_source_type == SCOREP_METRIC_SOURCE_TYPE_RUSAGE ) )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Metric %d was defined in previous phase", metric_id );
        return;
    }

    SCOREP_Hashtab_Entry* entry = NULL;

    /** Search the metric name in the table of the requested metrics */
    entry = SCOREP_Hashtab_Find(    requestsByName,
                                    &( *metric_name ),
                                    NULL );

    /** If the metric name was not found and the metric is RUSAGE metric search for "all" name
     * which is a request enabling all RUSAGE metrics
     */
    if ( !entry && metric_source_type == SCOREP_METRIC_SOURCE_TYPE_RUSAGE )
    {
        entry = SCOREP_Hashtab_Find(    requestsByName,
                                        &( "all" ),
                                        NULL );
    }

    if ( entry )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Metric %s,%d was found among requests", metric_name, metric_id );

        /** Create request key, which is the definition id of the metric*/
        int32_t* request_key = calloc( 1, sizeof( int32_t ) );
        assert( request_key );
        *request_key = ( int32_t )metric_id;

        /** Create value for the hash table entry. It carries source of the request associated with metric,
         * request type and  index of this metric in the Periscope metrics list if applicable*/
        MetricRequest* request_value = calloc( 1, sizeof( MetricRequest ) );
        assert( request_value );
        request_value->request_type  = ( ( MetricRequest* )entry->value )->request_type;
        request_value->metric_source = ( ( MetricRequest* )entry->value )->metric_source;
        request_value->psc_index     = ( ( MetricRequest* )entry->value )->psc_index;
        request_value->oa_index      = phase_request_oa_index++;
        request_value->metric_name   = UTILS_CStr_dup( metric_name );

        SCOREP_Hashtab_Insert(  requestsByID,
                                ( void* )( request_key ),
                                ( void* )( request_value ),
                                NULL );
    }
    else
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Metric %s,%d was NOT found among requests", metric_name, metric_id );
    }
}

void
SCOREP_OA_RequestsAddMetricByName( char*           metric_name,
                                   OA_MetricSource metric_source )
{
    int i;
    assert( requestsStatus == ACCEPTING );
    if ( requestsStatus != ACCEPTING )
    {
        return;
    }

    SCOREP_Hashtab_Entry* entry = NULL;

    if ( metric_source == SCOREP_METRIC_SOURCE_NOT_SUPPORTED )
    {
        free( metric_name );
        return;
    }

    /**
     * Rusage metrics request "ALL" keyword has to be lowercased in order to be accepted by metrics module
     */
    if ( metric_source == SCOREP_METRIC_SOURCE_RUSAGE )
    {
        if ( strcmp( metric_name, "ALL" ) == 0 )
        {
            for ( i = 0; metric_name[ i ] != '\0'; i++ )
            {
                metric_name[ i ] = ( char )tolower( metric_name[ i ] );
            }
        }
    }

    /**
     * Handle execution time request separately due to missing definition
     */
    if ( strcmp( metric_name, "execution_time" ) == 0 )
    {
        scorep_oa_request_exec_time_submit();
        free( metric_name );
        metric_name = NULL;
        return;
    }
    /** Search for already added similar metric request */
    entry = SCOREP_Hashtab_Find(    requestsByName,
                                    &( *metric_name ),
                                    NULL );
    /** If not found, add metric request*/
    if ( !entry )
    {
        /** Create request key out of the name. Simply reuse the string since it was already duplicated in the parser*/
        char* request_key =  metric_name;

        assert( request_key );

        /** Create value for the hash table entry. It carries the index of this metric in the Periscope metrics list*/
        MetricRequest* request_value = calloc( 1, sizeof( MetricRequest ) );
        assert( request_value );
        request_value->request_type  = SCOREP_REQUEST_BY_NAME;
        request_value->metric_source = metric_source;
        request_value->psc_index     = -1;
        request_value->metric_name   = NULL;

        SCOREP_Hashtab_Insert(  requestsByName,
                                ( void* )( request_key ),
                                ( void* )( request_value ),
                                NULL );

        /** If it is a PAPI metric, add the size of the metric name string to the total size of papi
         *  metric name strings. Account for the delimeters between metric names. It will be later
         *  used to construct Score-P metric configuration string */
        if ( metric_source == SCOREP_METRIC_SOURCE_PAPI )
        {
            size_of_papi_config_string += strlen( request_key ) + strlen( ";" );
        }
        /** If it is a RUSAGE metric, add the size of the metric name string to the total size of papi
         *  metric name strings. Account for the delimeters between metric names. It will be later
         *  used to construct Score-P metric configuration string */
        if ( metric_source == SCOREP_METRIC_SOURCE_RUSAGE )
        {
            /** add string length + delimeter length + delimeter null termination */
            size_of_rusage_config_string += strlen( request_key ) + strlen( ";" );
        }
    }
    else
    {
        /** Free metric name pointer since the metric name was duplicated by MRI parser
         * before being passed to this function
         */
        free( metric_name );
        metric_name = NULL;
    }
}

void
SCOREP_OA_RequestsAddPeriscopeMetric( int metric_code )
{
    int i;

    assert( requestsStatus == ACCEPTING );
    if ( requestsStatus != ACCEPTING )
    {
        return;
    }

    PSC_MetricID psc_metric_id = ( PSC_MetricID )metric_code;

    /* Search Periscope Metric List for the requested metric */
    for ( i = PSC_MIN_METRICID; i < PSC_MAX_METRICID; i++ )
    {
        if ( PSC_MetricList[ i ].metricID == psc_metric_id )
        {
            /** Check if the Periscope metric group is supported by Score-P */
            if ( get_scorep_metric_source( PSC_MetricList[ i ].metricGroup ) == SCOREP_METRIC_SOURCE_NOT_SUPPORTED )
            {
                UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "Metric (%d) group %d is not supoorted!", PSC_MetricList[ i ].metricID, PSC_MetricList[ i ].metricGroup );
                break;
            }


            SCOREP_Hashtab_Entry* entry = NULL;

            /** Search for already added similar metric request */
            entry = SCOREP_Hashtab_Find(    requestsByName,
                                            &( *PSC_MetricList[ i ].metricName ),
                                            NULL );
            /** If not found, add metric request*/
            if ( !entry )
            {
                /** Create request key out of the name */
                char* request_key = UTILS_CStr_dup( PSC_MetricList[ i ].metricName );
                assert( request_key );

                /** Create value for the hash table entry. It carries the index of this metric in the Periscope metrics list*/
                MetricRequest* request_value = calloc( 1, sizeof( MetricRequest ) );
                assert( request_value );
                request_value->request_type  = SCOREP_REQUEST_PERISCOPE_CODE;
                request_value->metric_source = get_scorep_metric_source( PSC_MetricList[ i ].metricGroup );
                request_value->psc_index     = i;

                SCOREP_Hashtab_Insert(  requestsByName,
                                        ( void* )( request_key ),
                                        ( void* )( request_value ),
                                        NULL );

                /** If it is a PAPI metric, add the size of the metric name string to the total size of papi
                 *  metric name strings. Account for the delimeters between metric names. It will be later
                 *  used to construct Score-P metric configuration string */
                if ( get_scorep_metric_source( PSC_MetricList[ i ].metricGroup ) == SCOREP_METRIC_SOURCE_PAPI )
                {
                    size_of_papi_config_string += strlen( request_key ) + strlen( ";" );
                }
                /** If it is a RUSAGE metric, add the size of the metric name string to the total size of papi
                 *  metric name strings. Account for the delimeters between metric names. It will be later
                 *  used to construct Score-P metric configuration string */
                if ( get_scorep_metric_source( PSC_MetricList[ i ].metricGroup ) == SCOREP_METRIC_SOURCE_RUSAGE )
                {
                    /** add string length + delimeter length + delimeter null termination */
                    size_of_rusage_config_string += strlen( request_key ) + strlen( ";" );
                }
            }

            break;
        }
    }
}

MetricRequest*
SCOREP_OA_RequestGet( uint32_t metric_ID )
{
    assert( requestsStatus == SUBMITTED );
    if ( requestsStatus != SUBMITTED )
    {
        return NULL;
    }

    int32_t               metric_ID_int = ( int32_t )metric_ID;
    SCOREP_Hashtab_Entry* entry;

    /** Search the metric ID in the table of the requested metrics IDs */
    entry = SCOREP_Hashtab_Find(    requestsByID,
                                    &( metric_ID_int ),
                                    NULL );

    if ( entry )
    {
        return ( MetricRequest* )entry->value;
    }
    else
    {
        return NULL;
    }
}

MetricRequest*
SCOREP_OA_RequestGetExecutionTime( void )
{
    return execution_time_request;
}

uint32_t
SCOREP_OA_GetNumberOfRequests( void )
{
    assert( requestsStatus == SUBMITTED );

    if ( requestsStatus != SUBMITTED )
    {
        return 0;
    }
    uint32_t return_value = ( uint32_t )SCOREP_Hashtab_Size( requestsByID );

    if ( execution_time_request )
    {
        return_value++;
    }

    return return_value;
}

void
SCOREP_OA_RequestsDismiss( void )
{
    assert( requestsStatus == SUBMITTED );

    if ( requestsStatus != SUBMITTED )
    {
        return;
    }

    /** Free metric request hash table */
    SCOREP_Hashtab_FreeAll( requestsByID, &free, &free_metric_request );
    requestsByID = NULL;
    if ( execution_time_request )
    {
        if ( execution_time_request->metric_name )
        {
            free( execution_time_request->metric_name );
        }
        free( execution_time_request );
    }
    requestsStatus = NOT_INITIALIZED;
}

static OA_MetricSource
get_scorep_metric_source( PSC_MetricGroup metric_group )
{
    switch ( metric_group )
    {
        case GROUP_PAPI_GENERIC:
        case GROUP_PAPI_NATIVE_P6:
        case GROUP_PAPI_NATIVE_NEHALEM:
        case GROUP_PAPI_NATIVE_ITANIUM:
        case GROUP_PAPI_NATIVE_NVIDIA:
            return SCOREP_METRIC_SOURCE_PAPI;
            break;
        case GROUP_RUSAGE:
            return SCOREP_METRIC_SOURCE_RUSAGE;
            break;
        default:
            return SCOREP_METRIC_SOURCE_NOT_SUPPORTED;
            break;
    }
}

void
free_metric_request( void* request )
{
    if ( !request )
    {
        return;
    }
    MetricRequest* m_request = ( MetricRequest* )request;
    if ( m_request->metric_name )
    {
        free( m_request->metric_name );
    }
    free( m_request );
}

static void
print_hash_table_request( const SCOREP_Hashtab* hash_table,
                          char*                 tag )
{
    printf( "\n/////////////%s///////////\n", tag );
    SCOREP_Hashtab_Iterator* iter;
    SCOREP_Hashtab_Entry*    entry;

    iter  = SCOREP_Hashtab_IteratorCreate( hash_table );
    entry = SCOREP_Hashtab_IteratorFirst( iter );
    while ( entry )
    {
        if ( entry->key )
        {
            printf( "Item (%d)-", ( *( int32_t* )entry->key ) );
        }
        else
        {
            printf( "Item (X)-" );
        }
        if ( entry->value )
        {
            printf( "(%d,%d,%d,%d,%s)\n", ( ( MetricRequest* )entry->value )->request_type,
                    ( ( MetricRequest* )entry->value )->metric_source,
                    ( ( MetricRequest* )entry->value )->psc_index,
                    ( ( MetricRequest* )entry->value )->oa_index,
                    ( ( MetricRequest* )entry->value )->metric_name );
        }
        else
        {
            printf( "(X,X,X)\n" );
        }
        entry = SCOREP_Hashtab_IteratorNext( iter );
    }
    SCOREP_Hashtab_IteratorFree( iter );
    if ( execution_time_request )
    {
        printf( "Item (T)-(%d,%d,%d,%d,%s)\n",
                execution_time_request->request_type,
                execution_time_request->metric_source,
                execution_time_request->psc_index,
                execution_time_request->oa_index,
                execution_time_request->metric_name );
    }

    printf( "///////////////////////////\n\n" );
}
