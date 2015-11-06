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
 * Copyright (c) 2009-2013, 2015,
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
#include <assert.h>

/* temporary debug switch */
static int do_print_out = 0;

/* array of thread profile index pointers */
static thread_private_index_type** thread_index_pointer_array = NULL;

/* number of threads */
static uint32_t thread_count = 0;

void
SCOREP_OAConsumer_Initialize( SCOREP_RegionHandle phaseHandle )
{
    int i;

    assert( phaseHandle != SCOREP_INVALID_REGION );

//    if ( do_print_out )
//	{
//		print_region_definitions();
//	}

    //   scorep_profile_dump_subtree( scorep_profile.first_root_node, 0 );

    /** Performs default profile call-tree transformation */
    SCOREP_Profile_Process();

    thread_count = scorep_oaconsumer_get_number_of_roots();

    /** Allocate and initialize thread-shread index data structures */
    thread_index_pointer_array = scorep_oa_consumer_initialize_index( phaseHandle );

    /** Loop over threads initializing thread-private index data structures*/
    for ( i = 0; i < thread_count; i++ )
    {
//		if ( do_print_out )
//		{
//			scorep_profile_dump_subtree( thread_index_pointer_array[i]->phase_node, 0 );
//		}

        /** Index all nodes starting from phase node*/
        scorep_profile_for_all( thread_index_pointer_array[ i ]->phase_node, &scorep_oaconsumer_count_index,  thread_index_pointer_array[ i ] );

//		if ( do_print_out )
//		{
//			print_hash_table( thread_index_pointer_array[i]->shared_index->merged_regions_def_table, "REGIONS" );
//			print_hash_table( thread_index_pointer_array[i]->static_measurements_table, "STATIC" );
//		}
    }
}

uint32_t
SCOREP_OAConsumer_GetDataSize( SCOREP_OAConsumer_DataTypes dataType )
{
    if ( thread_index_pointer_array[ 0 ] == NULL )
    {
        printf( "SCOREP_OAConsumer_GetDataSize: thread_index_pointer_array[0] == NULL\n" );
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
SCOREP_OAConsumer_GetData( SCOREP_OAConsumer_DataTypes dataType )
{
    if ( thread_index_pointer_array == NULL )
    {
        printf( "SCOREP_OAConsumer_GetDataSize: thread_index_pointer_array == NULL\n" );
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
        printf( "SCOREP_OAConsumer_DismissData: data_index == NULL\n" );
        return;
    }

    if ( thread_index_pointer_array[ 0 ]->shared_index )
    {
        if ( thread_index_pointer_array[ 0 ]->shared_index->merged_region_def_buffer )
        {
            free( thread_index_pointer_array[ 0 ]->shared_index->merged_region_def_buffer );
        }
        if ( thread_index_pointer_array[ 0 ]->shared_index->static_measurement_buffer )
        {
            free( thread_index_pointer_array[ 0 ]->shared_index->static_measurement_buffer );
        }
        if ( thread_index_pointer_array[ 0 ]->shared_index->counter_definition_buffer )
        {
            free( thread_index_pointer_array[ 0 ]->shared_index->counter_definition_buffer );
        }
        if ( thread_index_pointer_array[ 0 ]->shared_index->merged_regions_def_table )
        {
            SCOREP_Hashtab_FreeAll( thread_index_pointer_array[ 0 ]->shared_index->merged_regions_def_table, &free, &free );
        }
        free( thread_index_pointer_array[ 0 ]->shared_index );
    }

    int i;
    for ( i = 0; i < thread_count; i++ )
    {
        if ( thread_index_pointer_array[ i ] )
        {
            if ( thread_index_pointer_array[ i ]->static_measurements_table )
            {
                SCOREP_Hashtab_FreeAll( thread_index_pointer_array[ i ]->static_measurements_table, &free, &free  );
            }
            free( thread_index_pointer_array[ i ] );
        }
    }
    free( thread_index_pointer_array );
    thread_count = 0;
}
