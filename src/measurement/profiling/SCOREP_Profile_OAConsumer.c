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
 * @file SCOREP_Profile_OAConsumer.c Implementation of functions to access profile data used by OA module.
 *
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 *
 * @status alpha
 *
 */

#include <config.h>

#include "SCOREP_Profile_OAConsumer.h"
#include "SCOREP_Memory.h"
#include "SCOREP_Config.h"
#include "SCOREP_Definitions.h"

#include "scorep_utility/SCOREP_Utils.h"
#include "scorep_profile_definition.h"
#include "scorep_profile_node.h"
#include "scorep_profile_metric.h"
#include "scorep_definitions.h"
#include "scorep_mpi.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>


typedef struct
{
    uint64_t                            rank;
    uint32_t                            thread;
    uint32_t                            num_measurements;
    uint32_t                            num_contexts;
    uint32_t                            num_def_regions;
    uint32_t                            num_def_counters;
    uint32_t                            num_def_regions_merged;
    uint32_t                            num_static_measurements;
    SCOREP_Hashtab*                     merged_regions_def_table;   ///Hash table for mapping already registered region names region handles.
    SCOREP_Hashtab*                     static_measurements_table;
    scorep_profile_node*                phase_node;
    SCOREP_OA_StaticProfileMeasurement* static_measurement_buffer;
    SCOREP_OA_CallPathRegionDef*        merged_region_def_buffer;
} data_index_type;

extern SCOREP_DefinitionManager scorep_local_definition_manager;
static int                      do_print_out           = 0;
static data_index_type*         oa_consumer_data_index = NULL;
static uint64_t                 myrank                 = 0;

extern void
scorep_profile_dump_subtree( scorep_profile_node* node,
                             uint32_t             level );

scorep_profile_node*
scorep_oaconsumer_get_phase_node
(
    scorep_profile_node* node,
    uint32_t             phase_id
);

void
scorep_oaconsumer_count_index
(
    scorep_profile_node* node,
    void*                param
);

uint32_t
scorep_oa_index_data_key
(
    SCOREP_Hashtab* hash_table,
    SCOREP_OA_Key*  key,
    uint32_t        current_index
);

SCOREP_OA_Key*
scorep_oaconsumer_generate_region_key
(
    scorep_profile_node* node
);

SCOREP_OA_Key*
scorep_oaconsumer_generate_static_measurement_key
(
    SCOREP_OA_Key* region_key,
    uint32_t       counter_id
);

void
print_hash_table
(
    const SCOREP_Hashtab* hash_table,
    char*                 tag
);

void
print_region_definitions
(
);

int32_t
update_static_measurement
(
    SCOREP_OA_Key*   static_meas_key,
    uint64_t         value,
    uint64_t         samples,
    data_index_type* data_index
);

SCOREP_OA_StaticProfileMeasurement*
get_static_profile_measurements
(
);

SCOREP_OA_CallPathRegionDef*
get_merged_region_definitions
(
);

void
scorep_oaconsumer_copy_static_measurement
(
    scorep_profile_node* node,
    void*                param                          /// SCOREP_OA_StaticProfileMeasurement* buffer
);

void
scorep_oaconsumer_copy_merged_region_definitions
(
    scorep_profile_node* node,
    void*                param                          /// SCOREP_OA_StaticProfileMeasurement* buffer
);

int32_t
SCOREP_Hashtab_CompareOAKeys( const void* key,
                              const void* item_key );

size_t
SCOREP_Hashtab_HashOAKeys( const void* key );


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

void
SCOREP_OAConsumer_Initialize
(
    SCOREP_RegionHandle phase_handle
)
{
    /*
     * 0. Find the starting node from the given region handle
     * 1. Start traversing Call-Tree from the found node, count:
     *          1.1. number of contexts,
     *          1.2. number of measurements,
     *          1.3. insert hash def_regions_merged: key (region_key=parent_region_id:region_id) value (index++)
     *          1.4. insert hash static_measurements: key (region_key:counter_id) value (index++)
     */
    if ( phase_handle == SCOREP_INVALID_REGION )
    {
        printf( "ATTENTION! Phase region is handle is not valid!\n" );
        //return data_index;
        return;
    }
    /** Allocate data index */
    oa_consumer_data_index = calloc( 1, sizeof( data_index_type ) );

    /** Find node associated with phase region*/
    oa_consumer_data_index->phase_node = scorep_oaconsumer_get_phase_node( scorep_profile.first_root_node,
                                                                           SCOREP_GetRegionHandleToID( phase_handle ) );

    oa_consumer_data_index->rank = ( uint64_t )SCOREP_Mpi_GetRank();

    oa_consumer_data_index->thread = 0;

    if ( oa_consumer_data_index->phase_node == NULL )
    {
        printf( "ATTENTION! Phase node not found!\n" );
        //return data_index;
        return;
    }
    if ( do_print_out )
    {
        scorep_profile_dump_subtree( oa_consumer_data_index->phase_node, 0 );
    }
    /** Allocate key index tales for merged region definitions and static measurements */
    oa_consumer_data_index->merged_regions_def_table = SCOREP_Hashtab_CreateSize( 11, &SCOREP_Hashtab_HashOAKeys,
                                                                                  &SCOREP_Hashtab_CompareOAKeys );
    oa_consumer_data_index->static_measurements_table = SCOREP_Hashtab_CreateSize( 10, &SCOREP_Hashtab_HashOAKeys,
                                                                                   &SCOREP_Hashtab_CompareOAKeys );
    if ( do_print_out )
    {
        print_region_definitions();
    }

    /** Index all nodes starting from phase node*/
    scorep_profile_for_all( oa_consumer_data_index->phase_node, &scorep_oaconsumer_count_index, oa_consumer_data_index );

    if ( do_print_out )
    {
        print_hash_table( oa_consumer_data_index->merged_regions_def_table, "REGIONS" );
        print_hash_table( oa_consumer_data_index->static_measurements_table, "STATIC" );
    }
}

scorep_profile_node*
scorep_oaconsumer_get_phase_node
(
    scorep_profile_node* node,
    uint32_t             phase_id
)
{
    scorep_profile_node* phase_node = NULL;

    if ( node->node_type == scorep_profile_node_regular_region )
    {
        SCOREP_RegionHandle region_handle     = SCOREP_PROFILE_DATA2REGION( node->type_specific_data );
        uint32_t            current_region_id = SCOREP_GetRegionHandleToID( region_handle );
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

    if ( node->next_sibling != NULL )
    {
        phase_node = scorep_oaconsumer_get_phase_node( node->next_sibling, phase_id );
    }

    return phase_node;
}

void
scorep_oaconsumer_count_index
(
    scorep_profile_node* node,
    void*                param
)
{
    if ( node == NULL )
    {
        printf( "scorep_oaconsumer_count_index: node == NULL\n" );
        return;
    }
    if ( param == NULL )
    {
        printf( "scorep_oaconsumer_count_index: param == NULL\n" );
        return;
    }
    if ( do_print_out )
    {
        printf( "scorep_oaconsumer_count_index: node->type = %d\n", ( int )node->node_type );
    }
    if ( node->node_type == scorep_profile_node_regular_region )
    {
        data_index_type* data_index = ( data_index_type* )param;
        if ( do_print_out )
        {
            printf( "_count_index: |context %d\t|", data_index->num_contexts );
        }
        /** Count this node as a context*/
        data_index->num_contexts++;

        /** Generate merged region definition key*/
        SCOREP_OA_Key* region_key = scorep_oaconsumer_generate_region_key( node );
        //uint64_t region_key = scorep_oaconsumer_generate_region_key( node );


        /** Index merged region definition key in hash table*/
        data_index->num_def_regions_merged = scorep_oa_index_data_key(  data_index->merged_regions_def_table,
                                                                        region_key,
                                                                        data_index->num_def_regions_merged );


        if ( do_print_out )
        {
            printf( " meas %d\t|", data_index->num_measurements );
        }

        /** Count one measurement, since time is stored by default in every node*/
        data_index->num_measurements++;

        /** Generate static measurement key for TIME and this region*/
        SCOREP_OA_Key* static_meas_key = scorep_oaconsumer_generate_static_measurement_key(     region_key,
                                                                                                SCOREP_OA_COUNTER_TIME );

        /** Index static measurement key in hash table*/
        data_index->num_static_measurements = scorep_oa_index_data_key( data_index->static_measurements_table,
                                                                        static_meas_key,
                                                                        data_index->num_static_measurements );
        free( static_meas_key );

        /** Since counter definitions are not implemented, only first sparse int metric is considered as
         *      MPI Profiler LATE metric*/
        scorep_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
        if ( sparse_int != NULL )
        {
            if ( do_print_out )
            {
                printf( " meas %d\t|", data_index->num_measurements );
            }
            /** Count one int sparse metric as a measurement*/
            data_index->num_measurements++;
            /** Generate static measurement key for TIME and this region*/
            static_meas_key = scorep_oaconsumer_generate_static_measurement_key(      region_key,
                                                                                      SCOREP_OA_COUNTER_LATE );
            /** Index static measurement key in hash table*/
            data_index->num_static_measurements = scorep_oa_index_data_key( data_index->static_measurements_table,
                                                                            static_meas_key,
                                                                            data_index->num_static_measurements );
            free( static_meas_key );
        }
        else
        {
            if ( do_print_out )
            {
                printf( " \t\t\t|" );
            }
        }

        free( region_key );

        if ( do_print_out )
        {
            printf( "\n" );
        }
    }
}

void
print_hash_table
(
    const SCOREP_Hashtab* hash_table,
    char*                 tag
)
{
    printf( "\n/////////////%s////////\n", tag );
    SCOREP_Hashtab_Iterator* iter;
    SCOREP_Hashtab_Entry*    entry;

    iter  = SCOREP_Hashtab_IteratorCreate( hash_table );
    entry = SCOREP_Hashtab_IteratorFirst( iter );
    while ( entry )
    {
        printf( "Item (%d,%d,%d)-%d\n", ( *( SCOREP_OA_Key* )entry->key ).parent_region_id,
                ( *( SCOREP_OA_Key* )entry->key ).region_id,
                ( *( SCOREP_OA_Key* )entry->key ).metric_id,
                *( uint32_t* )( entry->value ) );
        entry = SCOREP_Hashtab_IteratorNext( iter );
    }
    SCOREP_Hashtab_IteratorFree( iter );
    printf( "///////////////////////////\n\n" );
}

uint32_t
scorep_oa_index_data_key
(
    SCOREP_Hashtab* hash_table,
    SCOREP_OA_Key*  key,
    uint32_t        current_index
)
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
        if ( do_print_out )
        {
            printf( " MISS (%d,%d,%d)-%d\t|",   key->parent_region_id,
                    key->region_id,
                    key->metric_id,
                    current_index );
        }
        SCOREP_OA_Key* entry_key = calloc( 1, sizeof( SCOREP_OA_Key ) );
        entry_key->parent_region_id = key->parent_region_id;
        entry_key->region_id        = key->region_id;
        entry_key->metric_id        = key->metric_id;
        int* entry_index = calloc( 1, sizeof( uint32_t ) );
        *entry_index = current_index;

        SCOREP_Hashtab_Insert(  hash_table,
                                ( void* )( entry_key ),
                                ( void* )( entry_index ),
                                &index );
        current_index++;
    }
    else
    {
        if ( do_print_out )
        {
            printf( " HIT (%d,%d,%d)-%d\t|",    key->parent_region_id,
                    key->region_id,
                    key->metric_id,
                    current_index );
        }
    }
    return current_index;
}


SCOREP_OA_Key*
scorep_oaconsumer_generate_region_key
(
    scorep_profile_node* node
)
{
    SCOREP_OA_Key*      new_key = calloc( 1, sizeof( SCOREP_OA_Key ) );

    SCOREP_RegionHandle region_handle = SCOREP_PROFILE_DATA2REGION( node->type_specific_data );

    uint32_t            current_region_id = SCOREP_GetRegionHandleToID( region_handle );
    uint32_t            parent_region_id  = 0;
    if ( SCOREP_Region_GetAdapterType( region_handle ) == SCOREP_ADAPTER_MPI )
    {
        scorep_profile_node* parent_node = node->parent;
        if ( parent_node != NULL )
        {
            if ( parent_node->node_type == scorep_profile_node_regular_region )
            {
                parent_region_id = SCOREP_GetRegionHandleToID(
                    SCOREP_PROFILE_DATA2REGION(
                        parent_node->type_specific_data ) );
            }
            else
            {
                printf( "scorep_oaconsumer_count_index: Attention! MPI node under non-regular profile node!\n" );
            }
        }
        else
        {
            printf( "scorep_oaconsumer_count_index: Attention! MPI node under NULL profile node!\n" );
        }
    }
    new_key->parent_region_id = parent_region_id;
    new_key->region_id        = current_region_id;
    new_key->metric_id        = 0;

    return new_key;
}

SCOREP_OA_Key*
scorep_oaconsumer_generate_static_measurement_key
(
    SCOREP_OA_Key* region_key,
    uint32_t       counter_id
)
{
    SCOREP_OA_Key* new_key = calloc( 1, sizeof( SCOREP_OA_Key ) );
    new_key->parent_region_id = region_key->parent_region_id;
    new_key->region_id        = region_key->region_id;
    new_key->metric_id        = counter_id;

    return new_key;
}


void
print_region_definitions
(
)
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, Region, region )
    {
        int index = SCOREP_LOCAL_HANDLE_TO_ID( handle, Region );
        printf( "region definition index=%d,", index );

        if ( definition->name_handle != SCOREP_INVALID_STRING )
        {
            printf( " name %s,", SCOREP_Region_GetName( handle ) );
        }
        if ( definition->file_name_handle != SCOREP_INVALID_STRING )
        {
            printf( " file %s,", SCOREP_Region_GetFileName( handle ) );
        }
        uint32_t rfl          = definition->begin_line;
        uint32_t rel          = definition->end_line;
        uint32_t adapter_type = ( uint32_t )definition->adapter_type;
        printf( " rfl=%d,adapter=%d\n",
                rfl,
                adapter_type );
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}

int32_t
update_static_measurement
(
    SCOREP_OA_Key*   static_meas_key,
    uint64_t         value,
    uint64_t         samples,
    data_index_type* data_index
)
{
    if ( data_index == NULL )
    {
        printf( "update_static_measurement: data_index == NULL\n" );
        return -1;
    }
    if ( data_index->static_measurement_buffer == NULL &&
         data_index->static_measurements_table == NULL &&
         data_index->merged_regions_def_table == NULL )
    {
        printf( "update_static_measurement: something is not initialized == NULL\n" );
        return -1;
    }
    SCOREP_Hashtab_Entry* entry = NULL;
    size_t                index;

    /** Search for static measurement key and aquire the index */
    entry = SCOREP_Hashtab_Find(    data_index->static_measurements_table,
                                    &( *static_meas_key ),
                                    &index );
    if ( !entry )
    {
        printf( "update_static_measurement: static_meas_key not found!\n" );
        return -1;
    }
    uint32_t static_meas_index = *( uint32_t* )( entry->value );

    /** Extract merged region definition key */
    uint32_t metric_id = static_meas_key->metric_id;

    /** Zero the metric_id in order to transform the key to the merged region definition key*/
    static_meas_key->metric_id = 0;

    /** Search for merged region definition key and aquire the index */
    index = 0;
    entry = NULL;
    entry = SCOREP_Hashtab_Find(    data_index->merged_regions_def_table,
                                    &( *static_meas_key ),
                                    &index );
    if ( !entry )
    {
        printf( "update_static_measurement: merged_region_def_key not found!\n" );
        return -1;
    }
    uint32_t merged_region_def_index = *( uint32_t* )( entry->value );


    /** Update corresponding record in static measurement buffer */
    data_index->static_measurement_buffer[ static_meas_index ].measurement_id = static_meas_index;
    data_index->static_measurement_buffer[ static_meas_index ].rank           = data_index->rank;
    data_index->static_measurement_buffer[ static_meas_index ].thread         = data_index->thread;
    data_index->static_measurement_buffer[ static_meas_index ].region_id      = merged_region_def_index;
    data_index->static_measurement_buffer[ static_meas_index ].samples       += samples;
    data_index->static_measurement_buffer[ static_meas_index ].metric_id      = metric_id;
    data_index->static_measurement_buffer[ static_meas_index ].int_val       += value;

    return 1;
}

void
scorep_oaconsumer_copy_static_measurement
(
    scorep_profile_node* node,
    void*                param                          /// COREP_OA_StaticProfileMeasurement* buffer
)
{
    if ( node == NULL )
    {
        printf( "scorep_oaconsumer_count_index: node == NULL\n" );
        return;
    }
    if ( param == NULL )
    {
        printf( "scorep_oaconsumer_count_index: param == NULL\n" );
        return;
    }
    if ( node->node_type == scorep_profile_node_regular_region )
    {
        data_index_type* data_index = ( data_index_type* )param;

        /** Generate merged region definition key*/
        SCOREP_OA_Key* region_key = scorep_oaconsumer_generate_region_key( node );

        /** Generate static measurement key for TIME and this region*/
        SCOREP_OA_Key* static_meas_key = scorep_oaconsumer_generate_static_measurement_key(     region_key,
                                                                                                SCOREP_OA_COUNTER_TIME );
        /** Update static measurement record which corresponds to the key */
        update_static_measurement(      static_meas_key,
                                        node->inclusive_time.sum,
                                        node->count,
                                        data_index );
        if ( do_print_out )
        {
            printf( "Node (%d,%d,%d) time=%" PRIu64 "\n",      region_key->parent_region_id,
                    region_key->region_id,
                    region_key->metric_id,
                    node->inclusive_time.sum );
        }
        /** Since counter definitions are not implemented, only first sparse int metric is considered as
         *      MPI Profiler LATE metric*/

        free( static_meas_key );

        scorep_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
        if ( sparse_int != NULL )
        {
            /** Generate static measurement key for TIME and this region*/
            static_meas_key = scorep_oaconsumer_generate_static_measurement_key(      region_key,
                                                                                      SCOREP_OA_COUNTER_LATE );
            update_static_measurement(      static_meas_key,
                                            sparse_int->sum,
                                            sparse_int->count,
                                            data_index );
            free( static_meas_key );
        }
        free( region_key );
    }
}

void
scorep_oaconsumer_copy_merged_region_definitions
(
    scorep_profile_node* node,
    void*                param                          /// COREP_OA_StaticProfileMeasurement* buffer
)
{
    if ( node == NULL )
    {
        printf( "scorep_oaconsumer_count_index: node == NULL\n" );
        return;
    }
    if ( param == NULL )
    {
        printf( "scorep_oaconsumer_count_index: param == NULL\n" );
        return;
    }
    if ( node->node_type == scorep_profile_node_regular_region )
    {
        data_index_type* data_index = ( data_index_type* )param;

        /** Generate merged region definition key*/
        SCOREP_OA_Key*        region_key = scorep_oaconsumer_generate_region_key( node );

        SCOREP_Hashtab_Entry* entry = NULL;
        size_t                index;

        /** Search for static measurement key and aquire the index */
        entry = SCOREP_Hashtab_Find(    data_index->merged_regions_def_table,
                                        &( *region_key ),
                                        &index );
        if ( !entry )
        {
            printf( "scorep_oaconsumer_copy_merged_region_definitions: region_key not found!\n" );
            return;
        }
        uint32_t region_index = *( uint32_t* )( entry->value );

        /** Get associated region handle of this node */
        SCOREP_RegionHandle region_handle = SCOREP_PROFILE_DATA2REGION( node->type_specific_data );

        /** Get associated region handle of parent node */
        SCOREP_RegionHandle parent_region_handle = region_handle;
        if ( SCOREP_Region_GetAdapterType( region_handle ) == SCOREP_ADAPTER_MPI )
        {
            scorep_profile_node* parent_node = node->parent;
            if ( parent_node != NULL )
            {
                if ( parent_node->node_type == scorep_profile_node_regular_region )
                {
                    parent_region_handle = SCOREP_PROFILE_DATA2REGION( parent_node->type_specific_data );
                }
                else
                {
                    printf( "scorep_oaconsumer_copy_merged_region_definitions: Attention! MPI node under non-regular profile node!\n" );
                }
            }
            else
            {
                printf( "scorep_oaconsumer_copy_merged_region_definitions: Attention! MPI node under NULL profile node!\n" );
            }
        }

        /** Copy data into the merged regions buffer*/
        data_index->merged_region_def_buffer[ region_index ].region_id    = region_index;
        data_index->merged_region_def_buffer[ region_index ].rfl          = SCOREP_Region_GetRfl( parent_region_handle );
        data_index->merged_region_def_buffer[ region_index ].rel          = SCOREP_Region_GetRel( parent_region_handle );
        data_index->merged_region_def_buffer[ region_index ].adapter_type = ( uint32_t )SCOREP_Region_GetAdapterType( region_handle );
        const char* name = SCOREP_Region_GetName( region_handle );
        strncpy( data_index->merged_region_def_buffer[ region_index ].name,
                 name,
                 MAX_REGION_NAME_LENGTH );
        const char* file = SCOREP_IO_GetWithoutPath( SCOREP_Region_GetFileName( parent_region_handle ) );
        strncpy( data_index->merged_region_def_buffer[ region_index ].file,
                 file,
                 MAX_FILE_NAME_LENGTH );
    }
}


SCOREP_OA_StaticProfileMeasurement*
get_static_profile_measurements
(
)
{
    if ( oa_consumer_data_index == NULL )
    {
        printf( "SCOREP_OAConsumer_GetStaticProfileMeasurement: data_index == NULL\n" );
        return NULL;
    }

    if ( oa_consumer_data_index->num_static_measurements == 0 )
    {
        printf( "get_merged_region_definitions: num_static_measurements==0\n" );
        return NULL;
    }

    /** Allocate static measurements buffer*/
    oa_consumer_data_index->static_measurement_buffer = calloc(     oa_consumer_data_index->num_static_measurements,
                                                                    sizeof( SCOREP_OA_StaticProfileMeasurement ) );

    /** Copy static measurements to the buffer*/
    scorep_profile_for_all( oa_consumer_data_index->phase_node,
                            &scorep_oaconsumer_copy_static_measurement,
                            oa_consumer_data_index );

    if ( do_print_out )
    {
        printf( "STATIC MEASUREMENTS:\n" );
        int i;
        for ( i = 0; i < oa_consumer_data_index->num_static_measurements; i++ )
        {
            printf( "RECORD %i\t| rank %" PRIu64 "\t| thread %d\t| region_id %d\t| samples %" PRIu64 "\t| metric_id %d\t| value %" PRIu64 "\t|\n",
                    i,
                    oa_consumer_data_index->static_measurement_buffer[ i ].rank,
                    oa_consumer_data_index->static_measurement_buffer[ i ].thread,
                    oa_consumer_data_index->static_measurement_buffer[ i ].region_id,
                    oa_consumer_data_index->static_measurement_buffer[ i ].samples,
                    oa_consumer_data_index->static_measurement_buffer[ i ].metric_id,
                    oa_consumer_data_index->static_measurement_buffer[ i ].int_val
                    );
        }
    }

    return oa_consumer_data_index->static_measurement_buffer;
}
SCOREP_OA_CallPathRegionDef*
get_merged_region_definitions
(
)
{
    if ( oa_consumer_data_index == NULL )
    {
        printf( "get_merged_region_definitions: data_index == NULL\n" );
        return NULL;
    }

    if ( oa_consumer_data_index->num_def_regions_merged == 0 )
    {
        printf( "get_merged_region_definitions: num_def_regions_merged==0\n" );
        return NULL;
    }

    /** Allocate merged region definitions buffer*/
    oa_consumer_data_index->merged_region_def_buffer = calloc(      oa_consumer_data_index->num_def_regions_merged,
                                                                    sizeof( SCOREP_OA_CallPathRegionDef ) );

    /** Copy merged regions definitions to the buffer*/
    scorep_profile_for_all( oa_consumer_data_index->phase_node,
                            &scorep_oaconsumer_copy_merged_region_definitions,
                            oa_consumer_data_index );

    if ( do_print_out )
    {
        printf( "MERGED DEFINITIONS:\n" );
        int i;
        for ( i = 0; i < oa_consumer_data_index->num_def_regions_merged; i++ )
        {
            printf( "DEFINITION %i\t| region_id %d\t| name %s\t| rfl %d\t| rel %d\t| adapter %d\t| file %s\t|\n",
                    i,
                    oa_consumer_data_index->merged_region_def_buffer[ i ].region_id,
                    oa_consumer_data_index->merged_region_def_buffer[ i ].name,
                    oa_consumer_data_index->merged_region_def_buffer[ i ].rfl,
                    oa_consumer_data_index->merged_region_def_buffer[ i ].rel,
                    oa_consumer_data_index->merged_region_def_buffer[ i ].adapter_type,
                    oa_consumer_data_index->merged_region_def_buffer[ i ].file
                    );
        }
    }
    return oa_consumer_data_index->merged_region_def_buffer;
}

uint32_t
SCOREP_OAConsumer_GetDataSize
(
    SCOREP_OAConsumer_DataTypes data_type
)
{
    if ( oa_consumer_data_index == NULL )
    {
        printf( "SCOREP_OAConsumer_GetDataSize: data_index == NULL\n" );
        return -1;
    }
    switch ( data_type )
    {
        case STATIC_PROFILE:
            return oa_consumer_data_index->num_static_measurements;
        case MERGED_REGION_DEFINITIONS:
            return oa_consumer_data_index->num_def_regions_merged;
        case REGION_DEFINITIONS:
            return 0;
        case COUNTER_DEFINITIONS:
            return 0;
        case CALLPATH_PROFILE_CONTEXTS:
            return 0;
        case CALLPATH_PROFILE_MEASUREMENTS:
            return 0;
        default:
            return 0;
    }
}

void*
SCOREP_OAConsumer_GetData
(
    SCOREP_OAConsumer_DataTypes data_type
)
{
    if ( oa_consumer_data_index == NULL )
    {
        printf( "SCOREP_OAConsumer_GetDataSize: data_index == NULL\n" );
        return NULL;
    }
    switch ( data_type )
    {
        case STATIC_PROFILE:
            return get_static_profile_measurements();
        case MERGED_REGION_DEFINITIONS:
            return get_merged_region_definitions();
        case REGION_DEFINITIONS:
            return NULL;
        case COUNTER_DEFINITIONS:
            return NULL;
        case CALLPATH_PROFILE_CONTEXTS:
            return NULL;
        case CALLPATH_PROFILE_MEASUREMENTS:
            return NULL;
        default:
            return NULL;
    }
}

void
SCOREP_OAConsumer_DismissData
(
)
{
    if ( oa_consumer_data_index == NULL )
    {
        printf( "SCOREP_OAConsumer_DismissData: data_index == NULL\n" );
        return;
    }
    if ( oa_consumer_data_index->merged_region_def_buffer )
    {
        free( oa_consumer_data_index->merged_region_def_buffer );
    }
    if ( oa_consumer_data_index->static_measurement_buffer )
    {
        free( oa_consumer_data_index->static_measurement_buffer );
    }
    if ( oa_consumer_data_index->merged_regions_def_table )
    {
        SCOREP_Hashtab_Free( oa_consumer_data_index->merged_regions_def_table );
    }
    if ( oa_consumer_data_index->static_measurements_table )
    {
        SCOREP_Hashtab_Free( oa_consumer_data_index->static_measurements_table );
    }
    free( oa_consumer_data_index );
}
