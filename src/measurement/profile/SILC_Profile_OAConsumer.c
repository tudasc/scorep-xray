/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file SILC_Profile_OAConsumer.c Implementation of functions to access profile data by OA module.
 *
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include "SILC_Profile_OAConsumer.h"

#include "SILC_Memory.h"
#include "silc_utility/SILC_Utils.h"
#include "SILC_Config.h"

#include "silc_profile_definition.h"
#include "silc_profile_node.h"
#include "silc_profile_metric.h"

#include <stdio.h>

static uint64_t number_of_nodes           = 0;
static uint64_t number_of_metrics         = 0;
static int64_t  current_context_index     = 0;
static int64_t  current_measurement_index = 0;


static void
silc_oaconsumer_count
(
    silc_profile_node* node,
    void*              param
);

static int64_t
silc_oaconsumer_copy_node
(
    silc_profile_node*        node,
    SILC_OA_PeriscopeSummary* summary_buffer,
    int64_t                   parent_index
);

static void
silc_oaconsumer_copy_sub_tree
(
    silc_profile_node*        node,
    SILC_OA_PeriscopeSummary* summary_buffer,
    uint64_t                  parent_index,
    int32_t                   level
);

SILC_OA_PeriscopeSummary*
SILC_Profile_GetPeriscopeSummary
(
)
{
    printf( "Entering %s\n", __FUNCTION__ );
    SILC_OA_PeriscopeSummary* summary_buffer;
    number_of_nodes   = 0;
    number_of_metrics = 0;
    silc_profile_for_all( silc_profile.first_root_node, &silc_oaconsumer_count, NULL );

    printf( "Number of nodes = %ld, Number of measurements = %ld\n", number_of_nodes,
            number_of_metrics + number_of_nodes * ( silc_profile.num_of_dense_metrics + 1 ) );

    SILC_OA_PeriscopeContext*     context_buffer = calloc( number_of_nodes, sizeof( SILC_OA_PeriscopeContext ) );

    SILC_OA_PeriscopeMeasurement* measurement_buffer = calloc( number_of_metrics + number_of_nodes * ( silc_profile.num_of_dense_metrics + 1 ),
                                                               sizeof( SILC_OA_PeriscopeMeasurement ) );

    summary_buffer                     = calloc( 1, sizeof( SILC_OA_PeriscopeSummary ) );
    summary_buffer->context_buffer     = context_buffer;
    summary_buffer->measurement_buffer = measurement_buffer;
    summary_buffer->counter_def_buffer = NULL;
    summary_buffer->region_def_buffer  = NULL;
    summary_buffer->context_size       = number_of_nodes;
    summary_buffer->measurement_size   = number_of_metrics + number_of_nodes * ( silc_profile.num_of_dense_metrics + 1 );
    summary_buffer->counter_def_size   = 0;
    summary_buffer->region_def_size    = 0;

    silc_oaconsumer_copy_sub_tree( silc_profile.first_root_node, summary_buffer, 0, 0 );
    printf( "Leaving %s\n", __FUNCTION__ );
    return summary_buffer;
}

void
silc_oaconsumer_count
(
    silc_profile_node* node,
    void*              param
)
{
    if ( node == NULL )
    {
        return;
    }
    if ( node->node_type == silc_profile_node_regular_region )
    {
        number_of_nodes++;

        silc_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
        while ( sparse_int != NULL )
        {
            number_of_metrics++;
            sparse_int = sparse_int->next_metric;
        }
    }
}

int64_t
silc_oaconsumer_copy_node
(
    silc_profile_node*        node,
    SILC_OA_PeriscopeSummary* summary_buffer,
    int64_t                   parent_index
)
{
    if ( node == NULL || summary_buffer == NULL )
    {
        printf( "node == NULL || summary_buffer == NULL\n" );
        return -1;
    }

    if ( node->node_type == silc_profile_node_regular_region )
    {
        if ( current_context_index >= summary_buffer->context_size )
        {
            printf( "Prealocated size for context buffer is not sufficient!\n" );
            return -1;
        }

        ///Copy node to the buffer

        summary_buffer->context_buffer[ current_context_index ].region_id         = 0;                                      ///@TODO access definition system here and get unique ID
        summary_buffer->context_buffer[ current_context_index ].context_id        = current_context_index;                  ///@TODO access definition system here and get unique ID
        summary_buffer->context_buffer[ current_context_index ].parent_context_id = parent_index;                           ///@TODO access definition system here and get unique ID
        summary_buffer->context_buffer[ current_context_index ].thread_id         = 0;                                      ///@TODO store current thread_id in a global variable and assign it here
        summary_buffer->context_buffer[ current_context_index ].rank              = 0;
        summary_buffer->context_buffer[ current_context_index ].call_count        = node->count;

        printf( "Node inserted: context_id=%ld, parent_id=%ld, thread_id=%d, rank=%d, call_count=%ld |",
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
        printf( " time_%ld=%ld/%ld", current_measurement_index,
                summary_buffer->measurement_buffer[ current_measurement_index ].sum,
                summary_buffer->measurement_buffer[ current_measurement_index ].count );
        current_measurement_index++;

        int i;
        for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
        {
            if ( current_measurement_index >= summary_buffer->measurement_size )
            {
                printf( "Prealocated size for measurement buffer is not sufficient!\n" );
                break;
            }
            summary_buffer->measurement_buffer[ current_measurement_index ].sum        = node->dense_metrics[ i ].sum;
            summary_buffer->measurement_buffer[ current_measurement_index ].count      = node->count;
            summary_buffer->measurement_buffer[ current_measurement_index ].context_id = current_context_index;
            summary_buffer->measurement_buffer[ current_measurement_index ].counter_id = 0;
            printf( " dense_%ld=%ld/%ld", current_measurement_index,
                    summary_buffer->measurement_buffer[ current_measurement_index ].sum,
                    summary_buffer->measurement_buffer[ current_measurement_index ].count );
            current_measurement_index++;
        }
        printf( " |" );
        silc_profile_sparse_metric_int* sparse_int = node->first_int_sparse;
        while ( sparse_int != NULL )
        {
            if ( current_measurement_index >= summary_buffer->measurement_size )
            {
                printf( "Prealocated size for measurement buffer is not sufficient!\n" );
                break;
            }
            summary_buffer->measurement_buffer[ current_measurement_index ].sum        = sparse_int->sum;
            summary_buffer->measurement_buffer[ current_measurement_index ].count      = sparse_int->count;
            summary_buffer->measurement_buffer[ current_measurement_index ].context_id = current_context_index;
            summary_buffer->measurement_buffer[ current_measurement_index ].counter_id = 0;
            printf( " sparse_%ld=%ld/%ld", current_measurement_index,
                    summary_buffer->measurement_buffer[ current_measurement_index ].sum,
                    summary_buffer->measurement_buffer[ current_measurement_index ].count );
            current_measurement_index++;
            sparse_int = sparse_int->next_metric;
        }
        printf( " |\n" );
        current_context_index++;
        return current_context_index - 1;
    }
    else
    {
        return current_context_index;
    }
}



void
silc_oaconsumer_copy_sub_tree
(
    silc_profile_node*        node,
    SILC_OA_PeriscopeSummary* summary_buffer,
    uint64_t                  parent_index,
    int32_t                   level
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
        printf( "| " );
    }
    printf( "+ type: %s, time=%ld, count=%ld \n", type_name_map[ node->node_type ], node->inclusive_time.sum, node->count );
    for ( i = 0; i < level; i++ )
    {
        printf( "| " );
    }
    int64_t my_index = silc_oaconsumer_copy_node( node, summary_buffer, parent_index );

    if ( node->first_child != NULL )
    {
        silc_oaconsumer_copy_sub_tree( node->first_child, summary_buffer, my_index, level + 1 );
    }
    if ( node->next_sibling != NULL )
    {
        silc_oaconsumer_copy_sub_tree( node->next_sibling, summary_buffer, parent_index, level );
    }
}
