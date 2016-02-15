/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * profile tree.
 *
 *
 *
 */

#include <config.h>
#include <stddef.h>

#include "SCOREP_Memory.h"

#include "scorep_profile_metric.h"
#include "scorep_profile_location.h"

/* ***************************************************************************************
   Dense metrics
*****************************************************************************************/

/* Initializes a dense metric. */
void
scorep_profile_init_dense_metric( scorep_profile_dense_metric* metric )
{
    metric->sum              = 0;
    metric->min              = UINT64_MAX;
    metric->max              = 0;
    metric->squares          = 0;
    metric->start_value      = 0;
    metric->intermediate_sum = 0;
}

/* Initializes an array of dense metrics */
void
scorep_profile_init_dense_metric_array( scorep_profile_dense_metric* metric,
                                        uint32_t                     number )
{
    for ( uint32_t i = 0; i < number; i++ )
    {
        scorep_profile_init_dense_metric( &metric[ i ] );
    }
}

/* Updates the statistics of one dense metric on an exit event. */
void
scorep_profile_update_dense_metric( scorep_profile_dense_metric* metric,
                                    uint64_t                     end_value )
{
    uint64_t diff = end_value - metric->start_value + metric->intermediate_sum;
    metric->intermediate_sum = 0;

    metric->sum += diff;
    if ( metric->min > diff )
    {
        metric->min = diff;
    }
    if ( metric->max < diff )
    {
        metric->max = diff;
    }
    metric->squares += ( diff * diff );
}

/* Copies the value of a dense metric to another dense metric. */
void
scorep_profile_copy_dense_metric( scorep_profile_dense_metric* destination,
                                  scorep_profile_dense_metric* source )
{
    destination->sum              = source->sum;
    destination->min              = source->min;
    destination->max              = source->max;
    destination->squares          = source->squares;
    destination->intermediate_sum = source->intermediate_sum;
}

/* Merges the statistics of a dense metrics to another metric */
void
scorep_profile_merge_dense_metric( scorep_profile_dense_metric* destination,
                                   scorep_profile_dense_metric* source )
{
    destination->sum += source->sum;
    if ( destination->min > source->min )
    {
        destination->min = source->min;
    }
    if ( destination->max < source->max )
    {
        destination->max = source->max;
    }
    destination->squares          += source->squares;
    destination->intermediate_sum += source->intermediate_sum;
}

/* ***************************************************************************************
   Sparse metrics for integer values
*****************************************************************************************/
static scorep_profile_sparse_metric_int*
scorep_profile_alloc_sparse_int( SCOREP_Profile_LocationData* location )
{
    scorep_profile_sparse_metric_int* new_sparse = NULL;

    if ( location->free_int_metrics != NULL )
    {
        new_sparse                 = location->free_int_metrics;
        location->free_int_metrics = new_sparse->next_metric;
    }
    else
    {
        new_sparse = ( scorep_profile_sparse_metric_int* )
                     SCOREP_Location_AllocForProfile( location->location_data, sizeof( scorep_profile_sparse_metric_int ) );
    }
    return new_sparse;
}

/* Creates a new sparse metric struct instance for integer values */
scorep_profile_sparse_metric_int*
scorep_profile_create_sparse_int( SCOREP_Profile_LocationData* location,
                                  SCOREP_MetricHandle          metric,
                                  uint64_t                     value )
{
    scorep_profile_sparse_metric_int* new_sparse =
        scorep_profile_alloc_sparse_int( location );
    if ( new_sparse == NULL )
    {
        return NULL;
    }

    new_sparse->metric      = metric;
    new_sparse->count       = 1;
    new_sparse->sum         = value;
    new_sparse->min         = value;
    new_sparse->max         = value;
    new_sparse->squares     = value * value;
    new_sparse->next_metric = ( scorep_profile_sparse_metric_int* )NULL;
    return new_sparse;
}

/* Copy constructor for sparse metric for integer values. */
scorep_profile_sparse_metric_int*
scorep_profile_copy_sparse_int( SCOREP_Profile_LocationData*      location,
                                scorep_profile_sparse_metric_int* source )
{
    scorep_profile_sparse_metric_int* new_sparse =
        scorep_profile_alloc_sparse_int( location );
    if ( new_sparse == NULL )
    {
        return NULL;
    }

    new_sparse->metric      = source->metric;
    new_sparse->count       = source->count;
    new_sparse->sum         = source->sum;
    new_sparse->min         = source->min;
    new_sparse->max         = source->max;
    new_sparse->squares     = source->squares;
    new_sparse->next_metric = ( scorep_profile_sparse_metric_int* )NULL;
    return new_sparse;
}

/* Updates a sparse metric struct instance for integer values */
void
scorep_profile_update_sparse_int( scorep_profile_sparse_metric_int* metric,
                                  uint64_t                          value )
{
    metric->count++;
    metric->sum += value;
    if ( metric->min > value )
    {
        metric->min = value;
    }
    if ( metric->max < value )
    {
        metric->max = value;
    }
    metric->squares += value * value;
}

/* Merges the content of a sparse metric to another sparse metric */
void
scorep_profile_merge_sparse_metric_int( scorep_profile_sparse_metric_int* destination,
                                        scorep_profile_sparse_metric_int* source )
{
    destination->count += source->count;
    destination->sum   += source->sum;
    if ( destination->min > source->min )
    {
        destination->min = source->min;
    }
    if ( destination->max < source->max )
    {
        destination->max = source->max;
    }
    destination->squares += source->squares;
}

/* ***************************************************************************************
   Sparse metrics for double values
*****************************************************************************************/

static scorep_profile_sparse_metric_double*
scorep_profile_alloc_sparse_double( SCOREP_Profile_LocationData* location )
{
    scorep_profile_sparse_metric_double* new_sparse = location->free_double_metrics;
    if ( new_sparse )
    {
        location->free_double_metrics = new_sparse->next_metric;
    }
    else
    {
        new_sparse = SCOREP_Location_AllocForProfile( location->location_data, sizeof( *new_sparse ) );
    }
    return new_sparse;
}

/* Creates a new sparse metric struct instance for double values */
scorep_profile_sparse_metric_double*
scorep_profile_create_sparse_double( SCOREP_Profile_LocationData* location,
                                     SCOREP_AnyHandle             handle,
                                     double                       value )
{
    scorep_profile_sparse_metric_double* new_sparse =
        scorep_profile_alloc_sparse_double( location );

    new_sparse->handle      = handle;
    new_sparse->count       = 1;
    new_sparse->sum         = value;
    new_sparse->min         = value;
    new_sparse->max         = value;
    new_sparse->squares     = value * value;
    new_sparse->next_metric = NULL;
    return new_sparse;
}

/* Copy constructor for  sparse metric for double values */
scorep_profile_sparse_metric_double*
scorep_profile_copy_sparse_double( SCOREP_Profile_LocationData*         location,
                                   scorep_profile_sparse_metric_double* source )
{
    scorep_profile_sparse_metric_double* new_sparse =
        scorep_profile_alloc_sparse_double( location );
    if ( new_sparse == NULL )
    {
        return NULL;
    }

    new_sparse->handle      = source->handle;
    new_sparse->count       = source->count;
    new_sparse->sum         = source->sum;
    new_sparse->min         = source->min;
    new_sparse->max         = source->max;
    new_sparse->squares     = source->squares;
    new_sparse->next_metric = NULL;
    return new_sparse;
}

/** Updates a sparse metric struct instance for double values */
void
scorep_profile_update_sparse_double( scorep_profile_sparse_metric_double* metric,
                                     double                               value )
{
    metric->count++;
    metric->sum += value;
    if ( metric->min > value )
    {
        metric->min = value;
    }
    if ( metric->max < value )
    {
        metric->max = value;
    }
    metric->squares += value * value;
}

/* Merges the content of a sparse metric to another sparse metric */
void
scorep_profile_merge_sparse_metric_double( scorep_profile_sparse_metric_double* destination,
                                           scorep_profile_sparse_metric_double* source )
{
    destination->count += source->count;
    destination->sum   += source->sum;
    if ( destination->min > source->min )
    {
        destination->min = source->min;
    }
    if ( destination->max < source->max )
    {
        destination->max = source->max;
    }
    destination->squares += source->squares;
}
