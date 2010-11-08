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
 * @file SCOREP_Profile_OAConsumer.c Implementation of functions to access profile data by OA module.
 *
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include "SCOREP_Profile_OAConsumer.h"

#include "SCOREP_Memory.h"
#include "scorep_utility/SCOREP_Utils.h"
#include "SCOREP_Config.h"

#include "scorep_profile_definition.h"
#include "scorep_profile_node.h"
#include "scorep_profile_metric.h"
#include "scorep_definitions.h"
#include "SCOREP_Definitions.h"

#include <stdio.h>
#include <string.h>

static uint64_t                 number_of_nodes           = 0;
static uint64_t                 number_of_metrics         = 0;
static int                      number_of_region_defs     = 0;
static int                      number_of_counter_defs    = 0;
static int64_t                  current_context_index     = 0;
static int64_t                  current_measurement_index = 0;

extern SCOREP_DefinitionManager scorep_local_definition_manager;

static void
scorep_oaconsumer_count
(
    scorep_profile_node* node,
    void*                param
);

static int64_t
scorep_oaconsumer_copy_node
(
    scorep_profile_node*        node,
    SCOREP_OA_PeriscopeSummary* summary_buffer,
    int64_t                     parent_index
);

static void
scorep_oaconsumer_copy_sub_tree
(
    scorep_profile_node*        node,
    SCOREP_OA_PeriscopeSummary* summary_buffer,
    uint64_t                    parent_index,
    int32_t                     level
);

static void
scorep_oaconsumer_get_definitions
(
    SCOREP_OA_PeriscopeRegionDef*  region_def_buffer,
    SCOREP_OA_PeriscopeCounterDef* counter_def_buffer,
    int                            region_def_count,
    int                            counter_def_count
);

static void
scorep_oaconsumer_get_definitions
(
    SCOREP_OA_PeriscopeRegionDef*  region_def_buffer,
    SCOREP_OA_PeriscopeCounterDef* counter_def_buffer,
    int                            region_def_count,
    int                            counter_def_count
)
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, Region, region )
    {
        int index = SCOREP_LOCAL_HANDLE_TO_ID( handle, Region );
        if ( index >= 0 && index < region_def_count )
        {
            region_def_buffer[ index ].region_id = index;
            if ( definition->name_handle != SCOREP_INVALID_STRING )
            {
                strncpy( region_def_buffer[ index ].name, SCOREP_Region_GetName( handle ), MAX_REGION_NAME_LENGTH );
                region_def_buffer[ index ].name[ MAX_REGION_NAME_LENGTH - 1 ] = '\0';
            }
            if ( definition->file_name_handle != SCOREP_INVALID_STRING )
            {
                strncpy( region_def_buffer[ index ].file, SCOREP_Region_GetFileName( handle ), MAX_FILE_NAME_LENGTH );
                region_def_buffer[ index ].file[ MAX_FILE_NAME_LENGTH - 1 ] = '\0';
            }
            region_def_buffer[ index ].rfl = definition->begin_line;
            printf( "region definition index=%d,name=%s,file=%s,rfl=%d\n", index, region_def_buffer[ index ].name, region_def_buffer[ index ].file, region_def_buffer[ index ].rfl );
        }
        else
        {
            printf( "Region definition ID=%d is out of bounds [0,%d)!", index, region_def_count );
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();

    //add implicit time to the beginning of counter definition buffer
    counter_def_buffer[ 0 ].counter_id = 0;
    strcpy( counter_def_buffer[ 0 ].name, "Time" );
}

SCOREP_OA_PeriscopeSummary*
SCOREP_Profile_GetPeriscopeSummary
(
)
{
    printf( "Entering %s\n", __FUNCTION__ );
    //definenition of summary buffer
    SCOREP_OA_PeriscopeSummary* summary_buffer;

    //requesting the numbers of call-tree nodes, measurements, region and counter definitions
    number_of_nodes   = 0;
    number_of_metrics = 0;
    scorep_profile_for_all( scorep_profile.first_root_node, &scorep_oaconsumer_count, NULL );
    number_of_region_defs  = SCOREP_GetNumberOfRegionDefinitions();
    number_of_counter_defs = SCOREP_GetNumberOfCounterDefinitions() + 1;   // element 0 is preserved for implicit time stored in the node

    printf( "Number of regions=%d counter=%d\n", number_of_region_defs, number_of_counter_defs );

    printf( "Number of nodes = %ld, Number of measurements = %ld\n", number_of_nodes,
            number_of_metrics + number_of_nodes * ( scorep_profile.num_of_dense_metrics + 1 ) );

    //allocating memory for buffers
    SCOREP_OA_PeriscopeContext*     context_buffer     = calloc( number_of_nodes, sizeof( SCOREP_OA_PeriscopeContext ) );
    SCOREP_OA_PeriscopeMeasurement* measurement_buffer = calloc( number_of_metrics + number_of_nodes * ( scorep_profile.num_of_dense_metrics + 1 ),
                                                                 sizeof( SCOREP_OA_PeriscopeMeasurement ) );
    SCOREP_OA_PeriscopeRegionDef*   region_def_buffer  = calloc( number_of_region_defs, sizeof( SCOREP_OA_PeriscopeRegionDef ) );
    SCOREP_OA_PeriscopeCounterDef*  counter_def_buffer = calloc( number_of_counter_defs, sizeof( SCOREP_OA_PeriscopeCounterDef ) );

    summary_buffer                     = calloc( 1, sizeof( SCOREP_OA_PeriscopeSummary ) );
    summary_buffer->context_buffer     = context_buffer;
    summary_buffer->measurement_buffer = measurement_buffer;
    summary_buffer->counter_def_buffer = counter_def_buffer;
    summary_buffer->region_def_buffer  = region_def_buffer;
    summary_buffer->context_size       = number_of_nodes;
    summary_buffer->measurement_size   = number_of_metrics + number_of_nodes * ( scorep_profile.num_of_dense_metrics + 1 );
    summary_buffer->counter_def_size   = number_of_counter_defs;
    summary_buffer->region_def_size    = number_of_region_defs;

    //pulling nodes together with measurements from the profile call-tree
    scorep_oaconsumer_copy_sub_tree( scorep_profile.first_root_node, summary_buffer, 0, 0 );

    //pulling region and counter definitions from definition handling module
    scorep_oaconsumer_get_definitions( summary_buffer->region_def_buffer,
                                       summary_buffer->counter_def_buffer,
                                       number_of_region_defs,
                                       number_of_counter_defs );

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,
                         "Leaving %s\n", __FUNCTION__ );
    return summary_buffer;
}

void
scorep_oaconsumer_count
(
    scorep_profile_node* node,
    void*                param
)
{
    if ( node == NULL )
    {
        return;
    }
    if ( node->node_type == scorep_profile_node_regular_region )
    {
        number_of_nodes++;

        scorep_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
        while ( sparse_int != NULL )
        {
            number_of_metrics++;
            sparse_int = sparse_int->next_metric;
        }
    }
}

int64_t
scorep_oaconsumer_copy_node
(
    scorep_profile_node*        node,
    SCOREP_OA_PeriscopeSummary* summary_buffer,
    int64_t                     parent_index
)
{
    if ( node == NULL || summary_buffer == NULL )
    {
        printf( "node == NULL || summary_buffer == NULL\n" );
        return -1;
    }

    if ( node->node_type == scorep_profile_node_regular_region )
    {
        if ( current_context_index >= summary_buffer->context_size )
        {
            printf( "Prealocated size for context buffer is not sufficient!\n" );
            return -1;
        }

        ///Copy node to the buffer

        summary_buffer->context_buffer[ current_context_index ].region_id         = SCOREP_CallPathHandleToRegionID( node->callpath_handle );                                    ///@TODO access definition system here and get unique ID
        summary_buffer->context_buffer[ current_context_index ].context_id        = current_context_index;                                                                       ///@TODO access definition system here and get unique ID
        summary_buffer->context_buffer[ current_context_index ].parent_context_id = parent_index;                                                                                ///@TODO access definition system here and get unique ID
        summary_buffer->context_buffer[ current_context_index ].thread_id         = 0;                                                                                           ///@TODO store current thread_id in a global variable and assign it here
        summary_buffer->context_buffer[ current_context_index ].rank              = 0;
        summary_buffer->context_buffer[ current_context_index ].call_count        = node->count;

        printf(
            "Node inserted: context_id=%ld, parent_id=%ld, thread_id=%d, rank=%d, call_count=%ld |",
            summary_buffer->context_buffer[ current_context_index ].context_id,
            summary_buffer->context_buffer[ current_context_index ].parent_context_id,
            summary_buffer->context_buffer[ current_context_index ].thread_id,
            summary_buffer->context_buffer[ current_context_index ].rank,
            summary_buffer->context_buffer[ current_context_index ].call_count );

        /// Copy implicit time to the measurement buffer

        summary_buffer->measurement_buffer[ current_measurement_index ].sum        = node->inclusive_time.sum;
        summary_buffer->measurement_buffer[ current_measurement_index ].count      = node->count;
        summary_buffer->measurement_buffer[ current_measurement_index ].context_id = current_context_index;
        summary_buffer->measurement_buffer[ current_measurement_index ].counter_id = 0;
        printf(
            " time_%ld=%ld/%ld", current_measurement_index,
            summary_buffer->measurement_buffer[ current_measurement_index ].sum,
            summary_buffer->measurement_buffer[ current_measurement_index ].count );
        current_measurement_index++;

        int i;
        for ( i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
        {
            if ( current_measurement_index >= summary_buffer->measurement_size )
            {
                printf(
                    "Prealocated size for measurement buffer is not sufficient!\n" );
                break;
            }
            summary_buffer->measurement_buffer[ current_measurement_index ].sum        = node->dense_metrics[ i ].sum;
            summary_buffer->measurement_buffer[ current_measurement_index ].count      = node->count;
            summary_buffer->measurement_buffer[ current_measurement_index ].context_id = current_context_index;
            summary_buffer->measurement_buffer[ current_measurement_index ].counter_id = 0;
            printf(
                " dense_%ld=%ld/%ld", current_measurement_index,
                summary_buffer->measurement_buffer[ current_measurement_index ].sum,
                summary_buffer->measurement_buffer[ current_measurement_index ].count );
            current_measurement_index++;
        }
        printf( " |" );
        scorep_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
        while ( sparse_int != NULL )
        {
            if ( current_measurement_index >= summary_buffer->measurement_size )
            {
                printf(
                    "Prealocated size for measurement buffer is not sufficient!\n" );
                break;
            }
            summary_buffer->measurement_buffer[ current_measurement_index ].sum        = sparse_int->sum;
            summary_buffer->measurement_buffer[ current_measurement_index ].count      = sparse_int->count;
            summary_buffer->measurement_buffer[ current_measurement_index ].context_id = current_context_index;
            summary_buffer->measurement_buffer[ current_measurement_index ].counter_id = 0;
            printf(
                " sparse_%ld=%ld/%ld", current_measurement_index,
                summary_buffer->measurement_buffer[ current_measurement_index ].sum,
                summary_buffer->measurement_buffer[ current_measurement_index ].count );
            current_measurement_index++;
            sparse_int = sparse_int->next_metric;
        }
        printf(
            " |\n" );
        current_context_index++;
        return current_context_index - 1;
    }
    else
    {
        return current_context_index;
    }
}



void
scorep_oaconsumer_copy_sub_tree
(
    scorep_profile_node*        node,
    SCOREP_OA_PeriscopeSummary* summary_buffer,
    uint64_t                    parent_index,
    int32_t                     level
)
{
    static char* type_name_map[] = {
        "regular region",
        "paramater string",
        "parameter integer",
        "thread root",
        "thread start"
    };
    if ( node == NULL )
    {
        return;
    }
    int i;
    for ( i = 0; i < level; i++ )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,
                             "| " );
    }
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,
                         "+ type: %s, time=%ld, count=%ld \n", type_name_map[ node->node_type ], node->inclusive_time.sum, node->count );
    for ( i = 0; i < level; i++ )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OA,
                             "| " );
    }
    int64_t my_index = scorep_oaconsumer_copy_node( node, summary_buffer, parent_index );

    if ( node->first_child != NULL )
    {
        scorep_oaconsumer_copy_sub_tree( node->first_child, summary_buffer, my_index, level + 1 );
    }
    if ( node->next_sibling != NULL )
    {
        scorep_oaconsumer_copy_sub_tree( node->next_sibling, summary_buffer, parent_index, level );
    }
}
