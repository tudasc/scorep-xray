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
 * Copyright (c) 2009-2014, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015-2016,
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

static void
copy_static_measurement( scorep_profile_node* node,
                         void*                param );

/**
 * Copies merged region definition of the given profile node to the merged region definition buffer
 * which is passed over @a param. The pointer to this function is passed to @ref scorep_profile_for_all()
 * in order to parse the whole call-tree
 *
 * @param node a pointer to node which merged region definition to be copied
 * @param param a void pointer to the merged region definition buffer
 */
static void
copy_merged_region_definitions( scorep_profile_node* node,
                                void*                param );

/*----------------------------------------------------------------------------------------
   Hashing
   -------------------------------------------------------------------------------------*/
/**
 * Hashing functions for region ids
 */
static size_t
hash_oa_keys( const void* key );

/**
 * Generates hash key for a merged region definition of a given profile node
 */
static scorep_oa_key_type*
generate_region_key( scorep_profile_node* node );

/**
 * Generates hash key for a flat profile measurement for a given region defined
 * with merged region definition key and a counter id
 */
static scorep_oa_key_type*
generate_static_measurement_key( scorep_oa_key_type* regionKey,
                                 uint32_t            counterId );

/**
 * Creates an entry in the given hash table and returns a suggested index if entry
 * was not already in the hash table or an index of entry if it is already in a
 * hash table
 *
 * @param hashTable    hash table in which a new entry has to be inserted
 * @param key          key of an entry
 * @param currentIndex suggested index for the new entry
 */
static uint32_t
index_data_key( SCOREP_Hashtab*     hashTable,
                scorep_oa_key_type* key,
                uint32_t            currentIndex );

/**
 * Hash-compare functions for region ids
 */
static int32_t
compare_oa_keys( const void* key,
                 const void* itemKey );

/*----------------------------------------------------------------------------------------
   Debug print
   -------------------------------------------------------------------------------------*/
#if HAVE( SCOREP_DEBUG )
/**
 * Prints registered metric definitions
 */
static inline void
print_metric_definitions( void );

/**
 * Prints static measurements
 */
static inline void
print_static_measurements( shared_index_type* shared_index );

/**
 * Prints merged definitions
 */
static inline void
print_merged_definitions( shared_index_type* shared_index );

/**
 * Prints metric definitions buffer
 */
static inline void
print_metric_definitions_buffer( shared_index_type* sharedIndex );

#endif /* HAVE( SCOREP_DEBUG ) */

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


/**
 * Searches for a OA phase node in the call-tree profile by a given region id
 */
static scorep_profile_node*
oaconsumer_get_phase_node( scorep_profile_node* node,
                           uint32_t             phaseId )
{
    scorep_profile_node* phase_node = NULL;
    if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        SCOREP_RegionHandle region_handle     = scorep_profile_type_get_region_handle( node->type_specific_data );
        uint32_t            current_region_id = SCOREP_RegionHandle_GetId( region_handle );
        if ( current_region_id == phaseId )
        {
            phase_node = node;
            return phase_node;
        }
    }

    if ( node->first_child != NULL )
    {
        phase_node = oaconsumer_get_phase_node( node->first_child, phaseId );
    }

    if ( phase_node )
    {
        return phase_node;
    }

    if ( node->next_sibling != NULL && node->node_type != SCOREP_PROFILE_NODE_THREAD_ROOT )
    {
        phase_node = oaconsumer_get_phase_node( node->next_sibling, phaseId );
    }

    return phase_node;
}


SCOREP_OA_FlatProfileMeasurement*
scorep_oaconsumer_get_static_profile_measurements( thread_private_index_type** privateIndexPointerArray )
{
    int thread_count;

    UTILS_ASSERT( privateIndexPointerArray );

    shared_index_type* shared_index = privateIndexPointerArray[ 0 ]->shared_index;

    UTILS_ASSERT( shared_index );

    /** Allocate static measurements buffer*/
    shared_index->static_measurement_buffer = calloc( shared_index->num_static_measurements,
                                                      sizeof( SCOREP_OA_FlatProfileMeasurement ) );
    UTILS_ASSERT( shared_index->static_measurement_buffer );
    /** Traverse all thread call-trees and copy flat profile measurements into transmission buffer */
    thread_count = scorep_oaconsumer_get_number_of_roots();
    for ( int i = 0; i < thread_count; i++ )
    {
        /** Copy flat profile measurements of thread i to the buffer*/
        scorep_profile_for_all( privateIndexPointerArray[ i ]->phase_node,
                                &copy_static_measurement,
                                privateIndexPointerArray[ i ] );
    }


#if HAVE( SCOREP_DEBUG )
    print_static_measurements( shared_index );
#endif /* HAVE( SCOREP_DEBUG ) */
    return shared_index->static_measurement_buffer;
}


SCOREP_OA_CallPathRegionDef*
scorep_oaconsumer_get_merged_region_definitions( thread_private_index_type** privateIndexPointerArray )
{
    int thread_count;
    UTILS_ASSERT( privateIndexPointerArray );

    shared_index_type* shared_index = privateIndexPointerArray[ 0 ]->shared_index;

    UTILS_ASSERT( shared_index );

    /** Allocate merged region definitions buffer*/
    shared_index->merged_region_def_buffer = calloc( shared_index->num_def_regions_merged,
                                                     sizeof( SCOREP_OA_CallPathRegionDef ) );
    UTILS_ASSERT( shared_index->merged_region_def_buffer );

    thread_count = scorep_oaconsumer_get_number_of_roots();
    for ( int i = 0; i < thread_count; i++ )
    {
        /** Copy merged regions definitions to the buffer*/
        scorep_profile_for_all( privateIndexPointerArray[ i ]->phase_node,
                                &copy_merged_region_definitions,
                                privateIndexPointerArray[ i ] );
    }

#if HAVE( SCOREP_DEBUG )
    print_merged_definitions( shared_index );
#endif /* HAVE( SCOREP_DEBUG ) */
    return shared_index->merged_region_def_buffer;
}


SCOREP_OA_CallPathCounterDef*
scorep_oaconsumer_get_metric_definitions( thread_private_index_type** privateIndexPointerArray )
{
    UTILS_ASSERT( privateIndexPointerArray );

    shared_index_type* shared_index = privateIndexPointerArray[ 0 ]->shared_index;

    UTILS_ASSERT( shared_index );

    /** Allocate static measurements buffer*/
    shared_index->counter_definition_buffer = calloc( shared_index->num_counter_definitions,
                                                      sizeof( SCOREP_OA_CallPathCounterDef ) );

    UTILS_ASSERT( shared_index->counter_definition_buffer );

    /** Insert manually execution time metric definition */
    SCOREP_OA_MetricRequest* exec_time_request = SCOREP_OA_RequestGetExecutionTime();
    if ( exec_time_request )
    {
        const char* time_unit = "nsec";
        strncpy( shared_index->counter_definition_buffer[ exec_time_request->oa_index ].name,
                 exec_time_request->metric_name,
                 MAX_COUNTER_NAME_LENGTH );
        strncpy( shared_index->counter_definition_buffer[ exec_time_request->oa_index ].unit,
                 time_unit,
                 MAX_COUNTER_UNIT_LENGTH );
        shared_index->counter_definition_buffer[ exec_time_request->oa_index ].status = 0;
    }

    /**Copy Score-P requested counter definitions*/
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager, Metric, metric )
    {
        SCOREP_OA_MetricRequest* metric_request = SCOREP_OA_RequestGet( SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric ) );

        if ( metric_request != NULL )
        {
            shared_index->counter_definition_buffer[ metric_request->oa_index ].status = 0;

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
#if HAVE( SCOREP_DEBUG )
    print_metric_definitions();
    print_metric_definitions_buffer( shared_index );
#endif /* HAVE( SCOREP_DEBUG ) */
    return shared_index->counter_definition_buffer;
}


/**
 * Stores the metric definition index specified with metric handle in to the provided address.
 * Returns 1 on success, 0 if metric was not found among requests.
 *
 * @param metricHandle metric handle to of the metric which index is to be returned
 * @param metricIndex  a pointer to the location where the index will be stored if found
 *
 * @return 1 if success, 0 otherwise
 */
static int32_t
get_metric_request_index_pointer( SCOREP_MetricHandle metricHandle,
                                  uint32_t*           metricIndex )
{
    if ( !metricIndex )
    {
        return 0;
    }
    SCOREP_OA_MetricRequest* metric_request = SCOREP_OA_RequestGet( SCOREP_LOCAL_HANDLE_TO_ID( metricHandle, Metric ) );
    if ( metric_request )
    {
        ( *metricIndex ) = ( uint32_t )metric_request->oa_index;
        return 1;
    }
    else
    {
        return 0;
    }
}


/*----------------------------------------------------------------------------------------
   Profile indexing
   -------------------------------------------------------------------------------------*/
/**
 * Checks the number of metrics definitions and establishes translation between Score-P internal metric id and
 * external metric id
 */
static inline void
initialize_metric_def( shared_index_type* sharedIndex )
{
    uint32_t metric_count = SCOREP_OA_GetNumberOfRequests();

    sharedIndex->num_counter_definitions = metric_count;

    SCOREP_SamplingSetHandle sampling_set_handle = SCOREP_Metric_GetStrictlySynchronousSamplingSet();
    if ( sampling_set_handle != SCOREP_INVALID_SAMPLING_SET )
    {
        sharedIndex->dense_metrics_sampling_set = SCOREP_LOCAL_HANDLE_DEREF( sampling_set_handle, SamplingSet );
        UTILS_ASSERT( sharedIndex->dense_metrics_sampling_set );
    }
    else
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "sampling_set_handle of the profile dense metric is not registered!" );
        sharedIndex->dense_metrics_sampling_set = NULL;
    }
}


thread_private_index_type**
scorep_oaconsumer_initialize_index( SCOREP_RegionHandle phaseHandle )
{
    int i = 0;

    /** Get number of threads*/
    uint32_t thread_count = scorep_oaconsumer_get_number_of_roots();

    /** Allocate array of thread private index data structures, one entry per thread root in profile*/
    thread_private_index_type** private_index_pointer_array = calloc( thread_count, sizeof( thread_private_index_type* ) );

    UTILS_ASSERT( private_index_pointer_array );

    /** Allocate and initialize shared part of index*/
    shared_index_type* shared_index = calloc( 1, sizeof( shared_index_type ) );

    UTILS_ASSERT( shared_index );

    shared_index->rank                     = ( uint64_t )SCOREP_Ipc_GetRank();
    shared_index->num_static_measurements  = 0;
    shared_index->num_def_regions_merged   = 0;
    shared_index->merged_regions_def_table = SCOREP_Hashtab_CreateSize( 11, &hash_oa_keys,
                                                                        &compare_oa_keys );
    UTILS_ASSERT( shared_index->merged_regions_def_table );

    shared_index->merged_region_def_buffer  = NULL;
    shared_index->static_measurement_buffer = NULL;
    shared_index->counter_definition_buffer = NULL;

    /** Initialize metric definition index */
    initialize_metric_def( shared_index );

    /** Go through  all thread roots and initialize thread private index data structures */
    scorep_profile_node* thread_root = scorep_profile.first_root_node;
    while ( thread_root != NULL )
    {
        if ( thread_root->node_type == SCOREP_PROFILE_NODE_THREAD_ROOT )
        {
            private_index_pointer_array[ i ] = calloc( 1, sizeof( thread_private_index_type ) );

            //data_index[ i ]->phase_node = oaconsumer_get_phase_node( thread_root,
            //                                                         SCOREP_RegionHandle_GetId( phase_handle ) );
            UTILS_ASSERT( private_index_pointer_array[ i ] );

            private_index_pointer_array[ i ]->shared_index = shared_index;

            private_index_pointer_array[ i ]->phase_node = thread_root;

            private_index_pointer_array[ i ]->thread = i;

            private_index_pointer_array[ i ]->static_measurements_table = SCOREP_Hashtab_CreateSize( 10, &hash_oa_keys,
                                                                                                     &compare_oa_keys );

            UTILS_ASSERT( private_index_pointer_array[ i ]->static_measurements_table );

            UTILS_ASSERT( private_index_pointer_array[ i ]->phase_node );

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
    UTILS_ASSERT( node );
    UTILS_ASSERT( param );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA,
                        "scorep_oaconsumer_count_index: node->type = %d",
                        ( int )node->node_type );

    if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        thread_private_index_type* thread_private_index = ( thread_private_index_type* )param;
        shared_index_type*         shared_index         = thread_private_index->shared_index;

        /** Generate merged region definition key*/
        scorep_oa_key_type* region_key = generate_region_key( node );

        /** Index merged region definition key in hash table*/
        shared_index->num_def_regions_merged = index_data_key( shared_index->merged_regions_def_table,
                                                               region_key,
                                                               shared_index->num_def_regions_merged );

        /** Generate static measurement key for TIME and this region*/
        SCOREP_OA_MetricRequest* execution_time = SCOREP_OA_RequestGetExecutionTime();
        if ( execution_time )
        {
            scorep_oa_key_type* static_meas_key = generate_static_measurement_key( region_key,
                                                                                   execution_time->oa_index );

            /** Index Time static measurement key in hash table*/
            shared_index->num_static_measurements = index_data_key( thread_private_index->static_measurements_table,
                                                                    static_meas_key,
                                                                    shared_index->num_static_measurements );
            free( static_meas_key );
        }

        /** Index dense metrics */
        if ( shared_index->dense_metrics_sampling_set )
        {
            for ( int i = 0; i < shared_index->dense_metrics_sampling_set->number_of_metrics; i++ )
            {
                uint32_t metric_index;

                if ( !get_metric_request_index_pointer( shared_index->dense_metrics_sampling_set->metric_handles[ i ], &metric_index ) )
                {
                    continue;
                }

                /** Generate static measurement key for TIME and this region*/
                scorep_oa_key_type* static_meas_key = generate_static_measurement_key( region_key,
                                                                                       metric_index );

                /** Index static measurement key in hash table*/
                shared_index->num_static_measurements = index_data_key( thread_private_index->static_measurements_table,
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
            if ( get_metric_request_index_pointer( sparse_int->metric, &metric_index ) )
            {
                /** Generate static measurement key for metric and this region*/
                scorep_oa_key_type* static_meas_key = generate_static_measurement_key( region_key,
                                                                                       metric_index );
                /** Index static measurement key in hash table*/
                shared_index->num_static_measurements = index_data_key( thread_private_index->static_measurements_table,
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
            if ( get_metric_request_index_pointer( sparse_double->metric, &metric_index ) )
            {
                /** Generate static measurement key for metric and this region*/
                scorep_oa_key_type* static_meas_key = generate_static_measurement_key( region_key,
                                                                                       metric_index );
                /** Index static measurement key in hash table*/
                shared_index->num_static_measurements = index_data_key( thread_private_index->static_measurements_table,
                                                                        static_meas_key,
                                                                        shared_index->num_static_measurements );
                free( static_meas_key );
            }

            sparse_double = sparse_double->next_metric;
        }

        free( region_key );
    }
}


static uint32_t
index_data_key( SCOREP_Hashtab*     hashTable,
                scorep_oa_key_type* key,
                uint32_t            currentIndex )
{
    SCOREP_Hashtab_Entry* entry = NULL;
    size_t                index;

    /** Search for already indexed key */
    entry = SCOREP_Hashtab_Find(    hashTable,
                                    &( *key ),
                                    &index );
    /** If not found, store given key-index pair*/
    if ( !entry )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "MISS (%d,%d,%d)-%d",
                            key->parent_region_id,
                            key->region_id,
                            key->metric_id,
                            currentIndex );
        scorep_oa_key_type* entry_key = calloc( 1, sizeof( scorep_oa_key_type ) );
        UTILS_ASSERT( entry_key );
        entry_key->parent_region_id = key->parent_region_id;
        entry_key->region_id        = key->region_id;
        entry_key->metric_id        = key->metric_id;
        SCOREP_Hashtab_InsertUint32( hashTable,
                                     ( void* )( entry_key ),
                                     currentIndex,
                                     &index );
        currentIndex++;
    }
    else
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OA, "HIT (%d,%d,%d)-%d",
                            key->parent_region_id,
                            key->region_id,
                            key->metric_id,
                            currentIndex );
    }

    return currentIndex;
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
 * @param staticMeasKey      hash key of the flat profile measurement
 * @param value              value of the flat profile measurement
 * @param samples            samples of the flat profile measurement
 * @param threadPrivateIndex index data structure of a thread
 */
static int32_t
update_static_measurement( scorep_oa_key_type*        staticMeasKey,
                           uint64_t                   value,
                           uint64_t                   samples,
                           thread_private_index_type* threadPrivateIndex )
{
    UTILS_ASSERT( threadPrivateIndex );

    shared_index_type* shared_index = threadPrivateIndex->shared_index;

    UTILS_ASSERT( shared_index );
    UTILS_ASSERT( shared_index->static_measurement_buffer );
    UTILS_ASSERT( shared_index->merged_regions_def_table );
    UTILS_ASSERT( shared_index->merged_regions_def_table );

    SCOREP_Hashtab_Entry* entry = NULL;
    size_t                index;

    /** Search for static measurement key and acquire the index */
    entry = SCOREP_Hashtab_Find(    threadPrivateIndex->static_measurements_table,
                                    &( *staticMeasKey ),
                                    &index );


    UTILS_ASSERT( entry );

    uint32_t static_meas_index = entry->value.uint32;

    /** Extract merged region definition key */
    uint32_t metric_id = staticMeasKey->metric_id;

    /** Zero the metric_id in order to transform the key to the merged region definition key*/
    staticMeasKey->metric_id = 0;

    /** Search for merged region definition key and acquire the index */
    index = 0;
    entry = NULL;
    entry = SCOREP_Hashtab_Find(    shared_index->merged_regions_def_table,
                                    &( *staticMeasKey ),
                                    &index );

    UTILS_ASSERT( entry );

    uint32_t merged_region_def_index = entry->value.uint32;

    /** Update corresponding record in static measurement buffer */
    shared_index->static_measurement_buffer[ static_meas_index ].measurement_id = static_meas_index;
    shared_index->static_measurement_buffer[ static_meas_index ].rank           = shared_index->rank;
    shared_index->static_measurement_buffer[ static_meas_index ].thread         = threadPrivateIndex->thread;
    shared_index->static_measurement_buffer[ static_meas_index ].region_id      = merged_region_def_index;
    shared_index->static_measurement_buffer[ static_meas_index ].samples       += samples;
    shared_index->static_measurement_buffer[ static_meas_index ].metric_id      = metric_id;
    shared_index->static_measurement_buffer[ static_meas_index ].int_val       += value;

    return 1;
}


static void
copy_static_measurement( scorep_profile_node* node,
                         void*                param )
{
    UTILS_ASSERT( node );
    UTILS_ASSERT( param );

    if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        thread_private_index_type* thread_private_index = ( thread_private_index_type* )param;
        shared_index_type*         shared_index         = thread_private_index->shared_index;

        /** Generate merged region definition key*/
        scorep_oa_key_type* region_key = generate_region_key( node );


        /** Generate static measurement key for TIME and this region*/
        SCOREP_OA_MetricRequest* execution_time = SCOREP_OA_RequestGetExecutionTime();
        if ( execution_time )
        {
            scorep_oa_key_type* static_meas_key = generate_static_measurement_key( region_key,
                                                                                   SCOREP_OA_COUNTER_TIME );
            /** Update static measurement record which corresponds to the Time key */
            update_static_measurement( static_meas_key,
                                       node->inclusive_time.sum,
                                       node->count,
                                       thread_private_index );
            free( static_meas_key );
        }

        /** Copy dense metrics */
        if ( shared_index->dense_metrics_sampling_set )
        {
            for ( int i = 0; i < shared_index->dense_metrics_sampling_set->number_of_metrics; i++ )
            {
                uint32_t metric_index;

                if ( !get_metric_request_index_pointer( shared_index->dense_metrics_sampling_set->metric_handles[ i ], &metric_index ) )
                {
                    continue;
                }

                /** Generate static measurement key for TIME and this region*/
                scorep_oa_key_type* static_meas_key = generate_static_measurement_key( region_key,
                                                                                       metric_index );

                /** Update static measurement record which corresponds to the key */
                update_static_measurement( static_meas_key,
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

            if ( get_metric_request_index_pointer( sparse_int->metric, &metric_index ) )
            {
                /** Generate static measurement key for metric and this region*/
                scorep_oa_key_type* static_meas_key = generate_static_measurement_key( region_key,
                                                                                       metric_index );
                update_static_measurement( static_meas_key,
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

            if ( get_metric_request_index_pointer( sparse_double->metric, &metric_index ) )
            {
                /** Generate static measurement key for metric and this region*/
                scorep_oa_key_type* static_meas_key = generate_static_measurement_key( region_key,
                                                                                       metric_index );
                update_static_measurement( static_meas_key,
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


static void
copy_merged_region_definitions( scorep_profile_node* node,
                                void*                param )
{
    /** Check whether node and param data structures are valid*/
    UTILS_ASSERT( node );
    UTILS_ASSERT( param );

    if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        thread_private_index_type* thread_private_index = ( thread_private_index_type* )param;
        shared_index_type*         shared_index         = thread_private_index->shared_index;

        /** Generate merged region definition key*/
        scorep_oa_key_type* region_key = generate_region_key( node );

        SCOREP_Hashtab_Entry* entry = NULL;
        size_t                index;

        /** Search for static measurement key and acquire the index */
        entry = SCOREP_Hashtab_Find( shared_index->merged_regions_def_table,
                                     &( *region_key ),
                                     &index );
        /** Check whether entry was found. If not then a region being currently parsed was not previously indexed*/
        UTILS_ASSERT( entry );

        uint32_t region_index = entry->value.uint32;

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


static scorep_oa_key_type*
generate_region_key( scorep_profile_node* node )
{
    scorep_oa_key_type* new_key = calloc( 1, sizeof( scorep_oa_key_type ) );

    UTILS_ASSERT( new_key );

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


static scorep_oa_key_type*
generate_static_measurement_key( scorep_oa_key_type* regionKey,
                                 uint32_t            counterId )
{
    scorep_oa_key_type* new_key = calloc( 1, sizeof( scorep_oa_key_type ) );
    UTILS_ASSERT( new_key );
    new_key->parent_region_id = regionKey->parent_region_id;
    new_key->region_id        = regionKey->region_id;
    new_key->metric_id        = counterId;

    return new_key;
}


static int32_t
compare_oa_keys( const void* key,
                 const void* itemKey )
{
    int32_t return_value = 0;
    if ( ( ( scorep_oa_key_type* )key )->parent_region_id != ( ( scorep_oa_key_type* )itemKey )->parent_region_id )
    {
        return_value = 1;
    }
    if ( ( ( scorep_oa_key_type* )key )->region_id != ( ( scorep_oa_key_type* )itemKey )->region_id )
    {
        return_value = 1;
    }
    if ( ( ( scorep_oa_key_type* )key )->metric_id != ( ( scorep_oa_key_type* )itemKey )->metric_id )
    {
        return_value = 1;
    }

    return return_value;
}


static size_t
hash_oa_keys( const void* key )
{
    return ( ( scorep_oa_key_type* )key )->region_id * UINT32_C( 2654435761l );
}


#if HAVE( SCOREP_DEBUG )
static inline void
print_metric_definitions( void )
{
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager, Metric, metric )
    {
        int index = SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric );
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Metric definition index=%d,", index );

        if ( definition->name_handle != SCOREP_INVALID_STRING )
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, " name %s\n", SCOREP_MetricHandle_GetName( handle ) );
        }
        else
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "\n" );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static inline void
print_static_measurements( shared_index_type* sharedIndex )
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "STATIC MEASUREMENTS:\n" );
    for ( int i = 0; i < sharedIndex->num_static_measurements; i++ )
    {
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA,
                                "RECORD %d\t| rank %" PRIu64 "\t| thread %d\t| region_id %d\t| samples %" PRIu64 "\t| metric_id %d\t| value %" PRIu64 "\t|\n",
                                i,
                                sharedIndex->static_measurement_buffer[ i ].rank,
                                sharedIndex->static_measurement_buffer[ i ].thread,
                                sharedIndex->static_measurement_buffer[ i ].region_id,
                                sharedIndex->static_measurement_buffer[ i ].samples,
                                sharedIndex->static_measurement_buffer[ i ].metric_id,
                                sharedIndex->static_measurement_buffer[ i ].int_val );
    }
}

static inline void
print_merged_definitions( shared_index_type* sharedIndex )
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "MERGED DEFINITIONS:" );
    for ( int i = 0; i < sharedIndex->num_def_regions_merged; i++ )
    {
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA,
                                "DEFINITION %d\t| region_id %d\t| name %s\t| rfl %d\t| rel %d\t| paradigm %d\t| file %s\t|\n",
                                i,
                                sharedIndex->merged_region_def_buffer[ i ].region_id,
                                sharedIndex->merged_region_def_buffer[ i ].name,
                                sharedIndex->merged_region_def_buffer[ i ].rfl,
                                sharedIndex->merged_region_def_buffer[ i ].rel,
                                sharedIndex->merged_region_def_buffer[ i ].paradigm_type,
                                sharedIndex->merged_region_def_buffer[ i ].file );
    }
}

static inline void
print_metric_definitions_buffer( shared_index_type* sharedIndex )
{
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Metric definition buffer:\n" );
    for ( int i = 0; i < sharedIndex->num_counter_definitions; i++ )
    {
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Metric definition %d, name: %s, unit: %s\n",
                                i,
                                sharedIndex->counter_definition_buffer[ i ].name,
                                sharedIndex->counter_definition_buffer[ i ].unit );
    }
}
#endif /* HAVE( SCOREP_DEBUG ) */
