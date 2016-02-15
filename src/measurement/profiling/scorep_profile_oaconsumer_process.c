/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * Implementation of functions to process profile data before transferring over OA module.
 *
 */

#include <config.h>

#include "SCOREP_Profile_OAConsumer.h"
#include <SCOREP_Memory.h>
#include <SCOREP_Config.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Location.h>

#include <UTILS_IO.h>
#include <UTILS_Debug.h>
#include "scorep_profile_oaconsumer_process.h"
#include "scorep_profile_definition.h"
#include "scorep_profile_location.h"
#include "scorep_profile_node.h"
#include "scorep_profile_metric.h"
#include <SCOREP_Definitions.h>
#include <SCOREP_OA_Request.h>
#include <scorep_ipc.h>

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>


static int do_print_out = 0;


/** Prints registered metric definitions
 */
static void
print_metric_definitions( void );

/** Prints registered region definitions
 */
static void
print_region_definitions( void );

uint32_t
scorep_oaconsumer_get_number_of_roots( void )
{
    uint32_t             thread_count = 0;
    scorep_profile_node* thread_root  = scorep_profile.first_root_node;
    while ( thread_root != NULL )
    {
        if ( thread_root->node_type == SCOREP_PROFILE_NODE_THREAD_ROOT )
        {
            thread_count++;
        }
        thread_root = thread_root->next_sibling;
    }
    return thread_count;
}

scorep_profile_node*
scorep_oaconsumer_get_phase_node( scorep_profile_node* node,
                                  uint32_t             phase_id )
{
    scorep_profile_node* phase_node = NULL;
    if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        SCOREP_RegionHandle region_handle     = scorep_profile_type_get_region_handle( node->type_specific_data );
        uint32_t            current_region_id = SCOREP_RegionHandle_GetId( region_handle );
        if ( current_region_id == phase_id )
        {
            phase_node = node;
            return phase_node;
        }
    }

    if ( node->first_child != NULL )
    {
        phase_node = scorep_oaconsumer_get_phase_node( node->first_child, phase_id );
    }

    if ( phase_node )
    {
        return phase_node;
    }

    if ( node->next_sibling != NULL && node->node_type != SCOREP_PROFILE_NODE_THREAD_ROOT )
    {
        phase_node = scorep_oaconsumer_get_phase_node( node->next_sibling, phase_id );
    }

    return phase_node;
}

SCOREP_OA_FlatProfileMeasurement*
scorep_oaconsumer_get_static_profile_measurements( thread_private_index_type** private_index_pointer_array )
{
    int i, thread_count;

    assert( private_index_pointer_array );

    shared_index_type* shared_index = private_index_pointer_array[ 0 ]->shared_index;

    assert( shared_index );

    /** Allocate static measurements buffer*/
    shared_index->static_measurement_buffer = calloc( shared_index->num_static_measurements,
                                                      sizeof( SCOREP_OA_FlatProfileMeasurement ) );
    assert( shared_index->static_measurement_buffer );
    /** Traverse all thread call-trees and copy flat profile measurements into transmission buffer */
    thread_count = scorep_oaconsumer_get_number_of_roots();
    for ( i = 0; i < thread_count; i++ )
    {
        /** Copy flat profile measurements of thread i to the buffer*/
        scorep_profile_for_all( private_index_pointer_array[ i ]->phase_node,
                                &scorep_oaconsumer_copy_static_measurement,
                                private_index_pointer_array[ i ] );
    }


    if ( do_print_out )
    {
        printf( "STATIC MEASUREMENTS:\n" );
        int i;
        for ( i = 0; i < shared_index->num_static_measurements; i++ )
        {
            printf( "RECORD %i\t| rank %" PRIu64 "\t| thread %d\t| region_id %d\t| samples %" PRIu64 "\t| metric_id %d\t| value %" PRIu64 "\t|\n",
                    i,
                    shared_index->static_measurement_buffer[ i ].rank,
                    shared_index->static_measurement_buffer[ i ].thread,
                    shared_index->static_measurement_buffer[ i ].region_id,
                    shared_index->static_measurement_buffer[ i ].samples,
                    shared_index->static_measurement_buffer[ i ].metric_id,
                    shared_index->static_measurement_buffer[ i ].int_val
                    );
        }
    }
    return shared_index->static_measurement_buffer;
}
SCOREP_OA_CallPathRegionDef*
scorep_oaconsumer_get_merged_region_definitions( thread_private_index_type** private_index_pointer_array )
{
    int i, thread_count;
    assert( private_index_pointer_array );

    shared_index_type* shared_index = private_index_pointer_array[ 0 ]->shared_index;

    assert( shared_index );

    /** Allocate merged region definitions buffer*/
    shared_index->merged_region_def_buffer = calloc(      shared_index->num_def_regions_merged,
                                                          sizeof( SCOREP_OA_CallPathRegionDef ) );
    assert( shared_index->merged_region_def_buffer );

    thread_count = scorep_oaconsumer_get_number_of_roots();
    for ( i = 0; i < thread_count; i++ )
    {
        /** Copy merged regions definitions to the buffer*/
        scorep_profile_for_all( private_index_pointer_array[ i ]->phase_node,
                                &scorep_oaconsumer_copy_merged_region_definitions,
                                private_index_pointer_array[ i ] );
    }

    if ( do_print_out )
    {
        printf( "MERGED DEFINITIONS:\n" );
        int i;
        for ( i = 0; i < shared_index->num_def_regions_merged; i++ )
        {
            printf( "DEFINITION %i\t| region_id %d\t| name %s\t| rfl %d\t| rel %d\t| paradigm %d\t| file %s\t|\n",
                    i,
                    shared_index->merged_region_def_buffer[ i ].region_id,
                    shared_index->merged_region_def_buffer[ i ].name,
                    shared_index->merged_region_def_buffer[ i ].rfl,
                    shared_index->merged_region_def_buffer[ i ].rel,
                    shared_index->merged_region_def_buffer[ i ].paradigm_type,
                    shared_index->merged_region_def_buffer[ i ].file
                    );
        }
    }
    return shared_index->merged_region_def_buffer;
}

SCOREP_OA_CallPathCounterDef*
scorep_oaconsumer_get_metric_definitions( thread_private_index_type** private_index_pointer_array )
{
    int i;

    assert( private_index_pointer_array );

    shared_index_type* shared_index = private_index_pointer_array[ 0 ]->shared_index;

    assert( shared_index );

    /** Allocate static measurements buffer*/
    shared_index->counter_definition_buffer = calloc( shared_index->num_counter_definitions,
                                                      sizeof( SCOREP_OA_CallPathCounterDef ) );

    assert( shared_index->counter_definition_buffer );

    /** Insert manually execution time metric definition */
    MetricRequest* exec_time_request = SCOREP_OA_RequestGetExecutionTime();
    if ( exec_time_request )
    {
        const char* time_unit = "usec";
        strncpy(  shared_index->counter_definition_buffer[ exec_time_request->oa_index ].name,
                  exec_time_request->metric_name,
                  MAX_COUNTER_NAME_LENGTH );
        strncpy( shared_index->counter_definition_buffer[ exec_time_request->oa_index ].unit,
                 time_unit,
                 MAX_COUNTER_UNIT_LENGTH );
        shared_index->counter_definition_buffer[ exec_time_request->oa_index ].counter_id = exec_time_request->psc_index;
        shared_index->counter_definition_buffer[ exec_time_request->oa_index ].status     = 0;
    }

    /**Copy Score-P requested counter definitions*/
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager, Metric, metric )
    {
        MetricRequest* metric_request = SCOREP_OA_RequestGet( SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric ) );

        if ( metric_request != NULL )
        {
            shared_index->counter_definition_buffer[ metric_request->oa_index ].counter_id = metric_request->psc_index;
            shared_index->counter_definition_buffer[ metric_request->oa_index ].status     = 0;

            strncpy(  shared_index->counter_definition_buffer[ metric_request->oa_index ].name,
                      metric_request->metric_name,
                      MAX_COUNTER_NAME_LENGTH );

            if ( definition->unit_handle != SCOREP_INVALID_STRING )
            {
                const char* unit = SCOREP_LOCAL_HANDLE_DEREF( definition->unit_handle, String )->string_data;
                strncpy( shared_index->counter_definition_buffer[ metric_request->oa_index ].unit,
                         unit,
                         MAX_COUNTER_UNIT_LENGTH );
            }
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    if ( do_print_out )
    {
        printf( "Metric definition buffer:\n" );
        for ( i = 0; i < shared_index->num_counter_definitions; i++ )
        {
            printf( "Metric definition %d, id %d, name: %s, unit: %s\n",
                    i,
                    shared_index->counter_definition_buffer[ i ].counter_id,
                    shared_index->counter_definition_buffer[ i ].name,
                    shared_index->counter_definition_buffer[ i ].unit );
        }
    }
    return shared_index->counter_definition_buffer;
}

int32_t
scorep_oaconsumer_get_metric_request_index_pointer( SCOREP_MetricHandle metric_handle,
                                                    uint32_t*           metric_index )
{
    //return  0;
    if ( !metric_index )
    {
        return 0;
    }
    MetricRequest* metric_request = SCOREP_OA_RequestGet( SCOREP_LOCAL_HANDLE_TO_ID( metric_handle, Metric ) );
    if ( metric_request )
    {
        ( *metric_index ) = ( uint32_t )metric_request->oa_index;
        return 1;
    }
    else
    {
        return 0;
    }
}


void
scorep_oaconsumer_initialize_metric_def( shared_index_type* shared_index )
{
    uint32_t metric_count = SCOREP_OA_GetNumberOfRequests();

    shared_index->num_counter_definitions = metric_count;

    SCOREP_SamplingSetHandle sampling_set_handle = SCOREP_Metric_GetStrictlySynchronousSamplingSet();
    if ( sampling_set_handle != SCOREP_INVALID_SAMPLING_SET )
    {
        shared_index->dense_metrics_sampling_set = SCOREP_LOCAL_HANDLE_DEREF( sampling_set_handle, SamplingSet );
        assert( shared_index->dense_metrics_sampling_set );
    }
    else
    {
        //printf( "sampling_set_handle of the profile dense metric is not registered!\n" );
        shared_index->dense_metrics_sampling_set = NULL;
    }
}

thread_private_index_type**
scorep_oa_consumer_initialize_index( SCOREP_RegionHandle phase_handle )
{
    int i = 0;

    /** Get number of threads*/
    uint32_t thread_count = scorep_oaconsumer_get_number_of_roots();

    /** Allocate array of thread private index data structures, one entry per thread root in profile*/
    thread_private_index_type** private_index_pointer_array = calloc( thread_count, sizeof( thread_private_index_type* ) );

    assert( private_index_pointer_array );

    /** Allocate and initialize shared part of index*/
    shared_index_type* shared_index = calloc( 1, sizeof( shared_index_type ) );

    assert( shared_index );

    shared_index->rank                     = ( uint64_t )SCOREP_Ipc_GetRank();
    shared_index->num_static_measurements  = 0;
    shared_index->num_def_regions_merged   = 0;
    shared_index->merged_regions_def_table = SCOREP_Hashtab_CreateSize( 11, &SCOREP_Hashtab_HashOAKeys,
                                                                        &SCOREP_Hashtab_CompareOAKeys );
    assert( shared_index->merged_regions_def_table );

    shared_index->merged_region_def_buffer  = NULL;
    shared_index->static_measurement_buffer = NULL;
    shared_index->counter_definition_buffer = NULL;

    /** Initialize metric definition index */
    scorep_oaconsumer_initialize_metric_def( shared_index );

    /** Go through  all thread roots and initialize thread private index data structures */
    scorep_profile_node* thread_root = scorep_profile.first_root_node;
    while ( thread_root != NULL )
    {
        if ( thread_root->node_type == SCOREP_PROFILE_NODE_THREAD_ROOT )
        {
            private_index_pointer_array[ i ] = calloc( 1, sizeof( thread_private_index_type ) );

            //data_index[i]->phase_node=scorep_oaconsumer_get_phase_node( thread_root,
            //															SCOREP_RegionHandle_GetId( phase_handle ) );
            assert( private_index_pointer_array[ i ] );

            private_index_pointer_array[ i ]->shared_index = shared_index;

            private_index_pointer_array[ i ]->phase_node = thread_root;

            private_index_pointer_array[ i ]->thread = i;

            private_index_pointer_array[ i ]->static_measurements_table = SCOREP_Hashtab_CreateSize( 10, &SCOREP_Hashtab_HashOAKeys,
                                                                                                     &SCOREP_Hashtab_CompareOAKeys );

            assert( private_index_pointer_array[ i ]->static_measurements_table );

            assert( private_index_pointer_array[ i ]->phase_node );

            i++;
        }
        thread_root = thread_root->next_sibling;
    }
    return private_index_pointer_array;
}

void
scorep_oaconsumer_count_index( scorep_profile_node* node,
                               void*                param )
{
    int i;

    assert( node );
    assert( param );

//    if ( do_print_out )
//    {
//        printf( "scorep_oaconsumer_count_index: node->type = %d\n", ( int )node->node_type );
//    }
    if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        thread_private_index_type* thread_private_index = ( thread_private_index_type* )param;
        shared_index_type*         shared_index         = thread_private_index->shared_index;

        /** Generate merged region definition key*/
        SCOREP_OA_Key* region_key = scorep_oaconsumer_generate_region_key( node );
        //uint64_t region_key = scorep_oaconsumer_generate_region_key( node );

        /** Index merged region definition key in hash table*/
        shared_index->num_def_regions_merged = scorep_oa_index_data_key(  shared_index->merged_regions_def_table,
                                                                          region_key,
                                                                          shared_index->num_def_regions_merged );

        SCOREP_OA_Key* static_meas_key = NULL;

        /** Generate static measurement key for TIME and this region*/
        MetricRequest* execution_time = SCOREP_OA_RequestGetExecutionTime();
        if ( execution_time )
        {
            static_meas_key = scorep_oaconsumer_generate_static_measurement_key(     region_key,
                                                                                     execution_time->oa_index );

            /** Index Time static measurement key in hash table*/
            shared_index->num_static_measurements = scorep_oa_index_data_key( thread_private_index->static_measurements_table,
                                                                              static_meas_key,
                                                                              shared_index->num_static_measurements );
            free( static_meas_key );
        }

        /** Index dense metrics */
        if ( shared_index->dense_metrics_sampling_set )
        {
            for ( i = 0; i < shared_index->dense_metrics_sampling_set->number_of_metrics; i++ )
            {
                uint32_t metric_index;

                if ( !scorep_oaconsumer_get_metric_request_index_pointer( shared_index->dense_metrics_sampling_set->metric_handles[ i ], &metric_index ) )
                {
                    continue;
                }

                /** Generate static measurement key for TIME and this region*/
                SCOREP_OA_Key* static_meas_key = scorep_oaconsumer_generate_static_measurement_key(     region_key,
                                                                                                        metric_index );

                /** Index static measurement key in hash table*/
                shared_index->num_static_measurements = scorep_oa_index_data_key( thread_private_index->static_measurements_table,
                                                                                  static_meas_key,
                                                                                  shared_index->num_static_measurements );
                free( static_meas_key );
            }
        }

        /** Index sparse integer metrics */
        scorep_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
        while ( sparse_int != NULL )
        {
            /** Translate metric handle to OA metric definition index*/
            uint32_t metric_index;
            if ( scorep_oaconsumer_get_metric_request_index_pointer( sparse_int->metric, &metric_index ) )
            {
                /** Generate static measurement key for metric and this region*/
                static_meas_key = scorep_oaconsumer_generate_static_measurement_key(      region_key,
                                                                                          metric_index );
                /** Index static measurement key in hash table*/
                shared_index->num_static_measurements = scorep_oa_index_data_key( thread_private_index->static_measurements_table,
                                                                                  static_meas_key,
                                                                                  shared_index->num_static_measurements );
                free( static_meas_key );
            }

            sparse_int = sparse_int->next_metric;
        }

        /** Index sparse double metrics */
        scorep_profile_sparse_metric_double* sparse_double = node->first_double_sparse;
        while ( sparse_double != NULL )
        {
            /** Translate metric handle to OA metric definition index*/
            uint32_t metric_index;
            if ( scorep_oaconsumer_get_metric_request_index_pointer( sparse_double->handle, &metric_index ) )
            {
                /** Generate static measurement key for metric and this region*/
                static_meas_key = scorep_oaconsumer_generate_static_measurement_key(      region_key,
                                                                                          metric_index );
                /** Index static measurement key in hash table*/
                shared_index->num_static_measurements = scorep_oa_index_data_key( thread_private_index->static_measurements_table,
                                                                                  static_meas_key,
                                                                                  shared_index->num_static_measurements );
                free( static_meas_key );
            }

            sparse_double = sparse_double->next_metric;
        }

        free( region_key );
    }
}

uint32_t
scorep_oa_index_data_key( SCOREP_Hashtab* hash_table,
                          SCOREP_OA_Key*  key,
                          uint32_t        current_index )
{
    SCOREP_Hashtab_Entry* entry = NULL;
    size_t                index;

    /** Search for already indexed key */
    entry = SCOREP_Hashtab_Find(    hash_table,
                                    &( *key ),
                                    &index );
    /** If not found, store given key-index pair*/
    if ( !entry )
    {
//        if ( do_print_out )
//        {
//            printf( " MISS (%d,%d,%d)-%d\n",   key->parent_region_id,
//                    key->region_id,
//                    key->metric_id,
//                    current_index );
//        }
        SCOREP_OA_Key* entry_key = calloc( 1, sizeof( SCOREP_OA_Key ) );
        assert( entry_key );
        entry_key->parent_region_id = key->parent_region_id;
        entry_key->region_id        = key->region_id;
        entry_key->metric_id        = key->metric_id;
        uint32_t* entry_index = calloc( 1, sizeof( uint32_t ) );
        assert( entry_index );
        *entry_index = current_index;

        SCOREP_Hashtab_Insert(  hash_table,
                                ( void* )( entry_key ),
                                ( void* )( entry_index ),
                                &index );
        current_index++;
    }
    else
    {
//        if ( do_print_out )
//        {
//            printf( " HIT (%d,%d,%d)-%d\n",    key->parent_region_id,
//                    key->region_id,
//                    key->metric_id,
//                    current_index );
//        }
    }

    return current_index;
}

static uint32_t
check_region_definition_merge_needed( scorep_profile_node* node )
{
    if ( node == NULL )
    {
        return 0;
    }

    /** Chekc whether merge is possible */
    scorep_profile_node* parent_node = node->parent;
    if ( parent_node == NULL )
    {
        return 0;
    }

    if ( parent_node->node_type != SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        return 0;
    }

    /** Check whether merge is needed */
    SCOREP_RegionHandle region_handle        = scorep_profile_type_get_region_handle( node->type_specific_data );
    SCOREP_RegionHandle parent_region_handle = scorep_profile_type_get_region_handle( parent_node->type_specific_data );

    /** Merging definition is required only in case of MPI and OMP regions which don't have file region first line number */
    if ( !( SCOREP_RegionHandle_GetParadigmType( region_handle ) == SCOREP_PARADIGM_MPI ) &&
         !( SCOREP_RegionHandle_GetParadigmType( region_handle ) == SCOREP_PARADIGM_OPENMP ) )
    {
        return 0;
    }

    if ( SCOREP_RegionHandle_GetBeginLine( region_handle ) != 0 )
    {
        return 0;
    }

    return 1;
}

/** Updates a specified flat profile measurement in the corresponding buffer
 *
 * @param static_meas_key hash key of the flat profile measurement
 * @param value of the flat profile measurement
 * @param samples samples of the flat profile measurement
 * @param data_index index data structure of a thread
 */
static int32_t
update_static_measurement( SCOREP_OA_Key*             static_meas_key,
                           uint64_t                   value,
                           uint64_t                   samples,
                           thread_private_index_type* thread_private_index )
{
    assert( thread_private_index );

    shared_index_type* shared_index = thread_private_index->shared_index;

    assert( shared_index );
    assert( shared_index->static_measurement_buffer );
    assert( shared_index->merged_regions_def_table );
    assert( shared_index->merged_regions_def_table );

    SCOREP_Hashtab_Entry* entry = NULL;
    size_t                index;

    /** Search for static measurement key and acquire the index */
    entry = SCOREP_Hashtab_Find(    thread_private_index->static_measurements_table,
                                    &( *static_meas_key ),
                                    &index );


    assert( entry );

    uint32_t static_meas_index = *( uint32_t* )( entry->value );

    /** Extract merged region definition key */
    uint32_t metric_id = static_meas_key->metric_id;

    /** Zero the metric_id in order to transform the key to the merged region definition key*/
    static_meas_key->metric_id = 0;

    /** Search for merged region definition key and acquire the index */
    index = 0;
    entry = NULL;
    entry = SCOREP_Hashtab_Find(    shared_index->merged_regions_def_table,
                                    &( *static_meas_key ),
                                    &index );

    assert( entry );

    uint32_t merged_region_def_index = *( uint32_t* )( entry->value );

    /** Update corresponding record in static measurement buffer */
    shared_index->static_measurement_buffer[ static_meas_index ].measurement_id = static_meas_index;
    shared_index->static_measurement_buffer[ static_meas_index ].rank           = shared_index->rank;
    shared_index->static_measurement_buffer[ static_meas_index ].thread         = thread_private_index->thread;
    shared_index->static_measurement_buffer[ static_meas_index ].region_id      = merged_region_def_index;
    shared_index->static_measurement_buffer[ static_meas_index ].samples       += samples;
    shared_index->static_measurement_buffer[ static_meas_index ].metric_id      = metric_id;
    shared_index->static_measurement_buffer[ static_meas_index ].int_val       += value;

    return 1;
}

void
scorep_oaconsumer_copy_static_measurement( scorep_profile_node* node,
                                           void*                param )
{
    assert( node );
    assert( param );

    if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        thread_private_index_type* thread_private_index = ( thread_private_index_type* )param;
        shared_index_type*         shared_index         = thread_private_index->shared_index;

        /** Generate merged region definition key*/
        SCOREP_OA_Key* region_key = scorep_oaconsumer_generate_region_key( node );


        SCOREP_OA_Key* static_meas_key = NULL;

        /** Generate static measurement key for TIME and this region*/
        MetricRequest* execution_time = SCOREP_OA_RequestGetExecutionTime();
        if ( execution_time )
        {
            static_meas_key = scorep_oaconsumer_generate_static_measurement_key(     region_key,
                                                                                     SCOREP_OA_COUNTER_TIME );
            /** Update static measurement record which corresponds to the Time key */
            update_static_measurement(      static_meas_key,
                                            node->inclusive_time.sum,
                                            node->count,
                                            thread_private_index );
            free( static_meas_key );
        }

        /** Copy dense metrics */
        if ( shared_index->dense_metrics_sampling_set )
        {
            int i;
            for ( i = 0; i < shared_index->dense_metrics_sampling_set->number_of_metrics; i++ )
            {
                uint32_t metric_index;

                if ( !scorep_oaconsumer_get_metric_request_index_pointer( shared_index->dense_metrics_sampling_set->metric_handles[ i ], &metric_index ) )
                {
                    continue;
                }

                /** Generate static measurement key for TIME and this region*/
                SCOREP_OA_Key* static_meas_key = scorep_oaconsumer_generate_static_measurement_key(     region_key,
                                                                                                        metric_index );

                /** Update static measurement record which corresponds to the key */
                update_static_measurement(      static_meas_key,
                                                node->dense_metrics[ i ].sum,
                                                node->count,
                                                thread_private_index );
                free( static_meas_key );
            }
        }

        /** Copy sparse integer metrics */
        scorep_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
        while ( sparse_int != NULL )
        {
            /** Translate metric handle to OA metric definition index*/
            uint32_t metric_index;

            if ( scorep_oaconsumer_get_metric_request_index_pointer( sparse_int->metric, &metric_index ) )
            {
                /** Generate static measurement key for metric and this region*/
                static_meas_key = scorep_oaconsumer_generate_static_measurement_key(      region_key,
                                                                                          metric_index );
                update_static_measurement(      static_meas_key,
                                                sparse_int->sum,
                                                sparse_int->count,
                                                thread_private_index );
                free( static_meas_key );
            }
            sparse_int = sparse_int->next_metric;
        }

        /** Copy sparse double metrics */
        scorep_profile_sparse_metric_double* sparse_double = node->first_double_sparse;
        while ( sparse_double != NULL )
        {
            /** Translate metric handle to OA metric definition index*/
            uint32_t metric_index;

            if ( scorep_oaconsumer_get_metric_request_index_pointer( sparse_double->handle, &metric_index ) )
            {
                /** Generate static measurement key for metric and this region*/
                static_meas_key = scorep_oaconsumer_generate_static_measurement_key(      region_key,
                                                                                          metric_index );
                update_static_measurement(      static_meas_key,
                                                sparse_double->sum,
                                                sparse_double->count,
                                                thread_private_index );
                free( static_meas_key );
            }
            sparse_double = sparse_double->next_metric;
        }


        free( region_key );
    }
}

void
scorep_oaconsumer_copy_merged_region_definitions( scorep_profile_node* node,
                                                  void*                param )
{
    /** Check whether node and param data structures are valid*/
    assert( node );
    assert( param );

    if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        thread_private_index_type* thread_private_index = ( thread_private_index_type* )param;
        shared_index_type*         shared_index         = thread_private_index->shared_index;

        /** Generate merged region definition key*/
        SCOREP_OA_Key* region_key = scorep_oaconsumer_generate_region_key( node );

        SCOREP_Hashtab_Entry* entry = NULL;
        size_t                index;

        /** Search for static measurement key and acquire the index */
        entry = SCOREP_Hashtab_Find(    shared_index->merged_regions_def_table,
                                        &( *region_key ),
                                        &index );
        /** Check whether entry was found. If not then a region being currently parsed was not previously indexed*/
        assert( entry );

        uint32_t region_index = *( uint32_t* )( entry->value );

        /** Get associated region handle of this node */
        SCOREP_RegionHandle region_handle = scorep_profile_type_get_region_handle( node->type_specific_data );

        /** Get associated region handle of parent node */
        SCOREP_RegionHandle parent_region_handle = region_handle;

        /** Check whether region file name and line numbers for this region definition have to be taken from parent region
         * and whether it is possible (i.g. parent region exists). This is called region definition merging. */
        if ( check_region_definition_merge_needed( node ) )
        {
            scorep_profile_node* parent_node = node->parent;
            parent_region_handle = scorep_profile_type_get_region_handle( parent_node->type_specific_data );
        }

        /** Copy data into the merged regions buffer*/
        shared_index->merged_region_def_buffer[ region_index ].region_id     = SCOREP_RegionHandle_GetId( region_handle );
        shared_index->merged_region_def_buffer[ region_index ].rfl           = SCOREP_RegionHandle_GetBeginLine( parent_region_handle );
        shared_index->merged_region_def_buffer[ region_index ].rel           = SCOREP_RegionHandle_GetEndLine( parent_region_handle );
        shared_index->merged_region_def_buffer[ region_index ].paradigm_type = ( uint32_t )SCOREP_RegionHandle_GetParadigmType( region_handle );
        const char* name = SCOREP_RegionHandle_GetName( region_handle );
        strncpy( shared_index->merged_region_def_buffer[ region_index ].name,
                 name,
                 MAX_REGION_NAME_LENGTH );

        const char* file_full_path = SCOREP_RegionHandle_GetFileName( parent_region_handle );
        if ( file_full_path == NULL )
        {
            sprintf( shared_index->merged_region_def_buffer[ region_index ].file, "unknown" );
        }
        else
        {
            const char* file = UTILS_IO_GetWithoutPath( file_full_path );
            strncpy( shared_index->merged_region_def_buffer[ region_index ].file,
                     file,
                     MAX_FILE_NAME_LENGTH );
        }
        free( region_key );
    }
}

SCOREP_OA_Key*
scorep_oaconsumer_generate_region_key( scorep_profile_node* node )
{
    SCOREP_OA_Key* new_key = calloc( 1, sizeof( SCOREP_OA_Key ) );

    assert( new_key );

    SCOREP_RegionHandle region_handle = scorep_profile_type_get_region_handle( node->type_specific_data );

    uint32_t current_region_id = SCOREP_RegionHandle_GetId( region_handle );
    uint32_t parent_region_id  = 0;

    /** Check whether region file name and line numbers for this region definition have to be taken from parent region
     * and whether it is possible (i.g. parent region exists). This is called region definition merging. */
    if ( check_region_definition_merge_needed( node ) )
    {
        scorep_profile_node* parent_node = node->parent;
        parent_region_id = SCOREP_RegionHandle_GetId( scorep_profile_type_get_region_handle(
                                                          parent_node->type_specific_data ) );
    }

    new_key->parent_region_id = parent_region_id;
    new_key->region_id        = current_region_id;
    new_key->metric_id        = 0;

    return new_key;
}

SCOREP_OA_Key*
scorep_oaconsumer_generate_static_measurement_key( SCOREP_OA_Key* region_key,
                                                   uint32_t       counter_id )
{
    SCOREP_OA_Key* new_key = calloc( 1, sizeof( SCOREP_OA_Key ) );
    assert( new_key );
    new_key->parent_region_id = region_key->parent_region_id;
    new_key->region_id        = region_key->region_id;
    new_key->metric_id        = counter_id;

    return new_key;
}

int32_t
SCOREP_Hashtab_CompareOAKeys( const void* key,
                              const void* item_key )
{
    int32_t return_value = 0;
    if ( ( ( SCOREP_OA_Key* )key )->parent_region_id != ( ( SCOREP_OA_Key* )item_key )->parent_region_id )
    {
        return_value = 1;
    }
    if ( ( ( SCOREP_OA_Key* )key )->region_id != ( ( SCOREP_OA_Key* )item_key )->region_id )
    {
        return_value = 1;
    }
    if ( ( ( SCOREP_OA_Key* )key )->metric_id != ( ( SCOREP_OA_Key* )item_key )->metric_id )
    {
        return_value = 1;
    }

    return return_value;
}

size_t
SCOREP_Hashtab_HashOAKeys( const void* key )
{
    return ( ( SCOREP_OA_Key* )key )->region_id * UINT32_C( 2654435761l );
}

/** Prints given hash table
 */
static void
print_hash_table( const SCOREP_Hashtab* hash_table,
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
            printf( "Item (%d,%d,%d)-", ( *( SCOREP_OA_Key* )entry->key ).parent_region_id,
                    ( *( SCOREP_OA_Key* )entry->key ).region_id,
                    ( *( SCOREP_OA_Key* )entry->key ).metric_id );
        }
        else
        {
            printf( "Item (X,X,X)-" );
        }
        if ( entry->value )
        {
            printf( "%d\n", *( uint32_t* )( entry->value ) );
        }
        else
        {
            printf( "X\n" );
        }
        entry = SCOREP_Hashtab_IteratorNext( iter );
    }
    SCOREP_Hashtab_IteratorFree( iter );
    printf( "///////////////////////////\n\n" );
}

static void
print_region_definitions( void )
{
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager, Region, region )
    {
        int index = SCOREP_LOCAL_HANDLE_TO_ID( handle, Region );
        printf( "region definition index=%d,", index );

        if ( definition->name_handle != SCOREP_INVALID_STRING )
        {
            printf( " name %s,", SCOREP_RegionHandle_GetName( handle ) );
        }
        if ( definition->file_name_handle != SCOREP_INVALID_STRING )
        {
            printf( " file %s,", SCOREP_RegionHandle_GetFileName( handle ) );
        }
        uint32_t rfl           = definition->begin_line;
        uint32_t rel           = definition->end_line;
        uint32_t paradigm_type = ( uint32_t )definition->paradigm_type;
        printf( " rfl=%d,paradigm=%d\n",
                rfl,
                paradigm_type );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static void
print_metric_definitions( void )
{
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager, Metric, metric )
    {
        int index = SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric );
        printf( "Metric definition index=%d,", index );

        if ( definition->name_handle != SCOREP_INVALID_STRING )
        {
            printf( " name %s\n", SCOREP_MetricHandle_GetName( handle ) );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}
