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
 * @file scorep_profile_metric.c Implementation of functions for metric manipulation in the
 * profile tree.
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <stddef.h>

#include "SCOREP_Memory.h"

#include "scorep_profile_metric.h"

/* ***************************************************************************************
   Dense metrics
*****************************************************************************************/

/* Initializes a dense metric. */
void
scorep_profile_init_dense_metric( scorep_profile_dense_metric* metric )
{
    metric->sum         = 0;
    metric->min         = UINT64_MAX;
    metric->max         = 0;
    metric->squares     = 0;
    metric->start_value = 0;
}

/* Updates the statistics of one dense metric on an exit event. */
void
scorep_profile_update_dense_metric( scorep_profile_dense_metric* metric,
                                    uint64_t                     end_value )
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
scorep_profile_copy_dense_metric( scorep_profile_dense_metric* destination,
                                  scorep_profile_dense_metric* source )
{
    destination->sum     = source->sum;
    destination->min     = source->min;
    destination->max     = source->max;
    destination->squares = source->squares;
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
    destination->squares += source->squares;
}

/* ***************************************************************************************
   Sparse metrics for integer values
*****************************************************************************************/

/* Creates a new sparse metric struct instance for integer values */
scorep_profile_sparse_metric_int*
scorep_profile_create_sparse_int(
    SCOREP_CounterHandle metric,
    uint64_t             value )
{
    scorep_profile_sparse_metric_int* new_sparse = ( scorep_profile_sparse_metric_int* )
                                                   SCOREP_Memory_AllocForProfile( sizeof( scorep_profile_sparse_metric_int ) );
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
scorep_profile_copy_sparse_int( scorep_profile_sparse_metric_int* source )
{
    scorep_profile_sparse_metric_int* new_sparse = ( scorep_profile_sparse_metric_int* )
                                                   SCOREP_Memory_AllocForProfile( sizeof( scorep_profile_sparse_metric_int ) );
    new_sparse->metric      = source->metric;
    new_sparse->count       = source->count;
    new_sparse->sum         = source->sum;
    new_sparse->min         = source->min;
    new_sparse->max         = source->max;
    new_sparse->squares     = source->squares;
    new_sparse->next_metric = ( scorep_profile_sparse_metric_int* )NULL;
    return new_sparse;
}

/** Updates a sparse metric struct instance for integer values */
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

/* Creates a new sparse metric struct instance for double values */
scorep_profile_sparse_metric_double*
scorep_profile_create_sparse_double(
    SCOREP_CounterHandle metric,
    double               value )
{
    scorep_profile_sparse_metric_double* new_sparse = ( scorep_profile_sparse_metric_double* )
                                                      SCOREP_Memory_AllocForProfile( sizeof( scorep_profile_sparse_metric_double ) );
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
scorep_profile_sparse_metric_double*
scorep_profile_copy_sparse_double( scorep_profile_sparse_metric_double* source )
{
    scorep_profile_sparse_metric_double* new_sparse = ( scorep_profile_sparse_metric_double* )
                                                      SCOREP_Memory_AllocForProfile( sizeof( scorep_profile_sparse_metric_double ) );
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
scorep_profile_update_sparse_double( scorep_profile_sparse_metric_double* metric,
                                     uint64_t                             value )
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
