/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
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
 * @file
 *
 *
 *
 */

#include <config.h>

#include "SCOREP_Profile_OAConsumer.h"
#include "SCOREP_Memory.h"
#include "SCOREP_Config.h"
#include "SCOREP_Definitions.h"
#include <UTILS_Debug.h>

#include <UTILS_IO.h>
#include "scorep_profile_oaconsumer_process.h"
#include "scorep_profile_definition.h"
#include "scorep_profile_location.h"
#include "scorep_profile_node.h"
#include "scorep_profile_metric.h"
#include <SCOREP_Definitions.h>
#include "scorep_ipc.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

/* array of thread profile index pointers */
static thread_private_index_type** thread_index_pointer_array = NULL;

/* number of threads */
static uint32_t thread_count = 0;

#if HAVE( SCOREP_DEBUG )
/**
 * Prints registered region definitions
 */
static inline void
print_region_definitions( void );

/**
 * Prints given hash table
 */
static void
print_hash_table( const SCOREP_Hashtab* hashTable,
                  char*                 tag );

#endif /* HAVE( SCOREP_DEBUG ) */

void
SCOREP_OAConsumer_Initialize( SCOREP_RegionHandle phaseHandle )
{
    int i;

    UTILS_ASSERT( phaseHandle != SCOREP_INVALID_REGION );

#if HAVE( SCOREP_DEBUG )
    print_region_definitions();
#endif /* HAVE( SCOREP_DEBUG ) */

    /** Performs default profile call-tree transformation */
    SCOREP_Profile_Process();

    thread_count = scorep_oaconsumer_get_number_of_roots();

    /** Allocate and initialize thread-shared index data structures */
    thread_index_pointer_array = scorep_oaconsumer_initialize_index( phaseHandle );

    /** Loop over threads initializing thread-private index data structures*/
    for ( i = 0; i < thread_count; i++ )
    {
        /** Index all nodes starting from phase node*/
        scorep_profile_for_all( thread_index_pointer_array[ i ]->phase_node, &scorep_oaconsumer_count_index,  thread_index_pointer_array[ i ] );

#if HAVE( SCOREP_DEBUG )
        print_hash_table( thread_index_pointer_array[ i ]->shared_index->merged_regions_def_table, "REGIONS" );
        print_hash_table( thread_index_pointer_array[ i ]->static_measurements_table, "STATIC" );
#endif  /* HAVE( SCOREP_DEBUG ) */
    }
}

uint32_t
SCOREP_OAConsumer_GetDataSize( scorep_oaconsumer_data_types dataType )
{
    if ( thread_index_pointer_array[ 0 ] == NULL )
    {
        UTILS_WARNING( "SCOREP_OAConsumer_GetDataSize: thread_index_pointer_array[0] == NULL" );
        return -1;
    }
    switch ( dataType )
    {
        case FLAT_PROFILE:
            return thread_index_pointer_array[ 0 ]->shared_index->num_static_measurements;
        case MERGED_REGION_DEFINITIONS:
            return thread_index_pointer_array[ 0 ]->shared_index->num_def_regions_merged;
        case REGION_DEFINITIONS:
            return 0;
        case COUNTER_DEFINITIONS:
            return thread_index_pointer_array[ 0 ]->shared_index->num_counter_definitions;
        case CALLPATH_PROFILE_CONTEXTS:
            return 0;
        case CALLPATH_PROFILE_MEASUREMENTS:
            return 0;
        case NUMBER_OF_THREADS:
            return thread_count;
        default:
            return 0;
    }
}

void*
SCOREP_OAConsumer_GetData( scorep_oaconsumer_data_types dataType )
{
    if ( thread_index_pointer_array == NULL )
    {
        UTILS_WARNING( "SCOREP_OAConsumer_GetDataSize: thread_index_pointer_array == NULL" );
        return NULL;
    }
    switch ( dataType )
    {
        case FLAT_PROFILE:
            return scorep_oaconsumer_get_static_profile_measurements( thread_index_pointer_array );
        case MERGED_REGION_DEFINITIONS:
            return scorep_oaconsumer_get_merged_region_definitions( thread_index_pointer_array );
        case REGION_DEFINITIONS:
            return NULL;
        case COUNTER_DEFINITIONS:
            return scorep_oaconsumer_get_metric_definitions( thread_index_pointer_array );
        case CALLPATH_PROFILE_CONTEXTS:
            return NULL;
        case CALLPATH_PROFILE_MEASUREMENTS:
            return NULL;
        default:
            return NULL;
    }
}

void
SCOREP_OAConsumer_DismissData( void )
{
    if ( thread_index_pointer_array == NULL )
    {
        UTILS_WARNING( "SCOREP_OAConsumer_DismissData: data_index == NULL" );
        return;
    }

    free( thread_index_pointer_array[ 0 ]->shared_index->merged_region_def_buffer );
    free( thread_index_pointer_array[ 0 ]->shared_index->static_measurement_buffer );
    free( thread_index_pointer_array[ 0 ]->shared_index->counter_definition_buffer );
    SCOREP_Hashtab_FreeAll( thread_index_pointer_array[ 0 ]->shared_index->merged_regions_def_table,
                            &SCOREP_Hashtab_DeleteFree,
                            &SCOREP_Hashtab_DeleteNone );
    free( thread_index_pointer_array[ 0 ]->shared_index );

    int i;
    for ( i = 0; i < thread_count; i++ )
    {
        SCOREP_Hashtab_FreeAll( thread_index_pointer_array[ i ]->static_measurements_table,
                                &SCOREP_Hashtab_DeleteFree,
                                &SCOREP_Hashtab_DeleteNone );
        free( thread_index_pointer_array[ i ] );
    }
    free( thread_index_pointer_array );
    thread_count = 0;
}

#if HAVE( SCOREP_DEBUG )
static inline void
print_region_definitions( void )
{
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager, Region, region )
    {
        int index = SCOREP_LOCAL_HANDLE_TO_ID( handle, Region );
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "region definition index=%d,", index );

        if ( definition->name_handle != SCOREP_INVALID_STRING )
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, " name %s,", SCOREP_RegionHandle_GetName( handle ) );
        }
        if ( definition->file_name_handle != SCOREP_INVALID_STRING )
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, " file %s,", SCOREP_RegionHandle_GetFileName( handle ) );
        }
        uint32_t rfl           = definition->begin_line;
        uint32_t rel           = definition->end_line;
        uint32_t paradigm_type = ( uint32_t )definition->paradigm_type;
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, " rfl=%d, paradigm=%d\n", rfl, paradigm_type );
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

static void
print_hash_table( const SCOREP_Hashtab* hashTable,
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
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Item (%d,%d,%d)-",
                                    ( *( scorep_oa_key_type* )entry->key ).parent_region_id,
                                    ( *( scorep_oa_key_type* )entry->key ).region_id,
                                    ( *( scorep_oa_key_type* )entry->key ).metric_id );
        }
        else
        {
            UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "Item (X,X,X)-" );
        }

        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "%d\n", entry->value.uint32 );

        entry = SCOREP_Hashtab_IteratorNext( iter );
    }
    SCOREP_Hashtab_IteratorFree( iter );
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_OA, "///////////////////////////\n\n" );
}
#endif /* HAVE( SCOREP_DEBUG ) */
