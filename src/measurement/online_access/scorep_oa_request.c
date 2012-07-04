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
 * @file scorep_oa_requests.c Implementation of functions to process measurements requests over OA module.
 *
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <scorep_oa_request.h>
#include <scorep_status.h>
#include <scorep_definition_structs.h>
#include <scorep_profile_definition.h>

#include <SCOREP_CStr.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_OA_PeriscopeMetrics.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Profile.h>

/** Defines specifying available Score-P metric sources */
#define                         SCOREP_METRIC_SOURCE_NOT_SUPPORTED              0
#define                         SCOREP_METRIC_SOURCE_PAPI                               1
#define                         SCOREP_METRIC_SOURCE_RUSAGE                             2

/** Helper function mapping Periscope metric groups to Score-P metric sources*/
int32_t
get_scorep_metric_source
(
    PSC_MetricGroup metric_group
);

/**
 * Internal hash-table storing metric requests using their name as a key. It is used to create
 * metric source configuration string later and to map Score-P metric definition codes back to
 * Periscope metric codes.
 */
SCOREP_Hashtab* requestsByName = NULL;

/** Internal request handling statuses enum */
typedef enum RequestsHandlingStatus
{
    NOT_INITIALIZED,
    ACCEPTING,
    SUBMITTED
}RequestsHandlingStatus;

/** Internal status of the request handling */
RequestsHandlingStatus requestsStatus = NOT_INITIALIZED;


/** Counts the total size PAPI metric source configuration string */
uint32_t size_of_papi_config_string   = 1;

/** Counts the total size RUSAGE metric source configuration string */
uint32_t size_of_rusage_config_string = 1;



void
SCOREP_OA_RequestBegin
(
)
{
    /** Check accepting requests is not already initialized*/
    assert( requestsStatus == NOT_INITIALIZED );

    /** Create hashtab storing requested metrics by name*/
    requestsByName = SCOREP_Hashtab_CreateSize( 11, &SCOREP_Hashtab_HashString,
                                                &SCOREP_Hashtab_CompareStrings );
    assert( requestsByName );

    /** set the intial length of papi and rusage configuration strings to 1 to account for a null termination caracter */
    size_of_papi_config_string   = 1;
    size_of_rusage_config_string = 1;


    requestsStatus = ACCEPTING;
}

void
SCOREP_OA_RequestsSubmit
(
)
{
    assert( requestsStatus == ACCEPTING );

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
    SCOREP_Hashtab_Iterator* iter;
    SCOREP_Hashtab_Entry*    entry;

    /* Go over all requested metrics and store them to the corresponding metric source configuration string*/
    iter  = SCOREP_Hashtab_IteratorCreate( requestsByName );
    entry = SCOREP_Hashtab_IteratorFirst( iter );
    while ( entry )
    {
        /** recover the index of the requested metric in the Periscope metric list array */
        uint32_t metric_index = *( ( uint32_t* )entry->value );

        /** add the requested metric name to the corresponding metric source configuration string */
        if ( get_scorep_metric_source( PSC_MetricList[ metric_index ].metricGroup ) == SCOREP_METRIC_SOURCE_PAPI )
        {
            strcat( scorep_metrics_papi_mine, PSC_MetricList[ metric_index ].metricName );
            strcat( scorep_metrics_papi_mine, ";" );
        }
        if ( get_scorep_metric_source( PSC_MetricList[ metric_index ].metricGroup ) == SCOREP_METRIC_SOURCE_RUSAGE )
        {
            strcat( scorep_metrics_rusage_mine, PSC_MetricList[ metric_index ].metricName );
            strcat( scorep_metrics_rusage_mine, ";" );
        }
        entry = SCOREP_Hashtab_IteratorNext( iter );
    }
    SCOREP_Hashtab_IteratorFree( iter );


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
    assert( SCOREP_Metric_reinitialize() == SCOREP_SUCCESS );


    /** Reinitialize profile module */

    // Finalize profile
    if ( scorep_profile.is_initialized )
    {
        SCOREP_Profile_Finalize();
    }

    // Initialize profile
    if ( !scorep_profile.is_initialized && scorep_profile.reinitialize )
    {
        uint32_t             number_of_metrics = 0;
        SCOREP_MetricHandle* metrics           = NULL;

        SCOREP_SamplingSetHandle sampling_set_handle = SCOREP_Metric_GetSamplingSet();
        if ( sampling_set_handle != SCOREP_INVALID_SAMPLING_SET )
        {
            SCOREP_SamplingSet_Definition* sampling_set
                              = SCOREP_LOCAL_HANDLE_DEREF( sampling_set_handle, SamplingSet );
            number_of_metrics = sampling_set->number_of_metrics;
            metrics           = sampling_set->metric_handles;
        }
        SCOREP_Profile_Initialize( number_of_metrics, metrics );
    }

    requestsStatus = SUBMITTED;
}

void
SCOREP_OA_RequestsAddMetric
(
    uint32_t metric_code
)
{
    int i;

    assert( requestsStatus == ACCEPTING );

    PSC_MetricID psc_metric_id = ( PSC_MetricID )metric_code;

    /* Search Periscope Metric List for the requested metric */
    for ( i = PSC_MIN_METRICID; i < PSC_MAX_METRICID; i++ )
    {
        if ( PSC_MetricList[ i ].metricID == psc_metric_id )
        {
            /** Check if the Periscope metric group is supported by Score-P */
            if ( get_scorep_metric_source( PSC_MetricList[ i ].metricGroup ) == SCOREP_METRIC_SOURCE_NOT_SUPPORTED )
            {
                printf( "Metric (%d) group %d is not supoorted!\n", PSC_MetricList[ i ].metricID, PSC_MetricList[ i ].metricGroup );
                break;
            }


            SCOREP_Hashtab_Entry* entry = NULL;
            size_t                index;

            /** Search for already added similar metric request */
            entry = SCOREP_Hashtab_Find(    requestsByName,
                                            &( PSC_MetricList[ i ].metricName ),
                                            &index );
            /** If not found, add metric request*/
            if ( !entry )
            {
                /** Create request key out of the name */
                char* request_key = SCOREP_CStr_dup( PSC_MetricList[ i ].metricName );
                assert( request_key );

                /** Create value for the hash table entry. It carries the index of this metric in the Periscope metrics list*/
                uint32_t* request_value = calloc( 1, sizeof( uint32_t ) );
                assert( request_value );
                *request_value = i;

                SCOREP_Hashtab_Insert(  requestsByName,
                                        ( void* )( request_key ),
                                        ( void* )( request_value ),
                                        &index );

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

void*
SCOREP_OA_RequestsGet
(
)
{
// not yet implemented
    return NULL;
}

void
SCOREP_OA_RequestsDismiss
(
)
{
    assert( requestsStatus == SUBMITTED );

    /** Free metric request hash table */
    SCOREP_Hashtab_FreeAll( requestsByName, &free, &free );

    requestsStatus = NOT_INITIALIZED;
}

int32_t
get_scorep_metric_source
(
    PSC_MetricGroup metric_group
)
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
