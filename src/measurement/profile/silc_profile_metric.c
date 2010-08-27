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
 * @file silc_profile_metric.c Implementation of functions for metric manipulation in the
 * profile tree.
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <stddef.h>

#include "SILC_Memory.h"

#include "silc_profile_metric.h"

/* ***************************************************************************************
   Dense metrics
*****************************************************************************************/

/* Initializes a dense metric. */
void
silc_profile_init_dense_metric( silc_profile_dense_metric* metric )
{
    metric->sum         = 0;
    metric->min         = UINT64_MAX;
    metric->max         = 0;
    metric->squares     = 0;
    metric->start_value = 0;
}

/* Updates the statistics of one dense metric on an exit event. */
void
silc_profile_update_dense_metric( silc_profile_dense_metric* metric,
                                  uint64_t                   end_value )
{
    double diff = end_value - metric->start_value;
    metric->sum += diff;
    if ( metric->min > diff )
    {
        metric->min = diff;
    }
    if ( metric->max < diff )
    {
        metric->max = diff;
    }
    metric->squares += diff * diff;
}

/* Copies the value of a dense metric to another dense metric. */
void
silc_profile_copy_dense_metric( silc_profile_dense_metric* destination,
                                silc_profile_dense_metric* source )
{
    destination->sum     = source->sum;
    destination->min     = source->min;
    destination->max     = source->max;
    destination->squares = source->squares;
}

/* Merges the statistics of a dense metrics to another metric */
void
silc_profile_merge_dense_metric( silc_profile_dense_metric* destination,
                                 silc_profile_dense_metric* source )
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
    destination->squares += source->squares;
}

/* ***************************************************************************************
   Sparse metrics for integer values
*****************************************************************************************/

/* Creates a new sparse metric struct instance for integer values */
silc_profile_sparse_metric_int*
silc_profile_create_sparse_int(
    SILC_CounterHandle metric,
    uint64_t           value )
{
    silc_profile_sparse_metric_int* new_sparse = ( silc_profile_sparse_metric_int* )
                                                 SILC_Memory_AllocForProfile( sizeof( silc_profile_sparse_metric_int ) );
    new_sparse->metric      = metric;
    new_sparse->count       = 1;
    new_sparse->sum         = value;
    new_sparse->min         = value;
    new_sparse->max         = value;
    new_sparse->squares     = value * value;
    new_sparse->next_metric = ( silc_profile_sparse_metric_int* )NULL;
    return new_sparse;
}

/* Copy constructor for sparse metric for integer values. */
silc_profile_sparse_metric_int*
silc_profile_copy_sparse_int( silc_profile_sparse_metric_int* source )
{
    silc_profile_sparse_metric_int* new_sparse = ( silc_profile_sparse_metric_int* )
                                                 SILC_Memory_AllocForProfile( sizeof( silc_profile_sparse_metric_int ) );
    new_sparse->metric      = source->metric;
    new_sparse->count       = source->count;
    new_sparse->sum         = source->sum;
    new_sparse->min         = source->min;
    new_sparse->max         = source->max;
    new_sparse->squares     = source->squares;
    new_sparse->next_metric = ( silc_profile_sparse_metric_int* )NULL;
    return new_sparse;
}

/** Updates a sparse metric struct instance for integer values */
void
silc_profile_update_sparse_int( silc_profile_sparse_metric_int* metric,
                                uint64_t                        value )
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
silc_profile_merge_sparse_metric_int( silc_profile_sparse_metric_int* destination,
                                      silc_profile_sparse_metric_int* source )
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

/* Creates a new sparse metric struct instance for double values */
silc_profile_sparse_metric_double*
silc_profile_create_sparse_double(
    SILC_CounterHandle metric,
    double             value )
{
    silc_profile_sparse_metric_double* new_sparse = ( silc_profile_sparse_metric_double* )
                                                    SILC_Memory_AllocForProfile( sizeof( silc_profile_sparse_metric_double ) );
    new_sparse->metric      = metric;
    new_sparse->count       = 1;
    new_sparse->sum         = value;
    new_sparse->min         = value;
    new_sparse->max         = value;
    new_sparse->squares     = value * value;
    new_sparse->next_metric = NULL;
    return new_sparse;
}

/* Copy constructor for  sparse metric for double values */
silc_profile_sparse_metric_double*
silc_profile_copy_sparse_double( silc_profile_sparse_metric_double* source )
{
    silc_profile_sparse_metric_double* new_sparse = ( silc_profile_sparse_metric_double* )
                                                    SILC_Memory_AllocForProfile( sizeof( silc_profile_sparse_metric_double ) );
    new_sparse->metric      = source->metric;
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
silc_profile_update_sparse_double( silc_profile_sparse_metric_double* metric,
                                   uint64_t                           value )
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
silc_profile_merge_sparse_metric_double( silc_profile_sparse_metric_double* destination,
                                         silc_profile_sparse_metric_double* source )
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
