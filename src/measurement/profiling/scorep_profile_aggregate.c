/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * The file contains function to perform aggregation of per-thread data to
 * per process data.
 */

#include <config.h>
#include <scorep_profile_aggregate.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_cube4_writer.h>
#include <SCOREP_Definitions.h>

#include <UTILS_Error.h>

/* **************************************************************************************
 * static function
 ***************************************************************************************/

/* *INDENT-OFF* */
/**
 * @def NOT_PERMITTED_TEMPLATE
 * Template for aggregation functions that are not allowed.
 * Even if an aggregation operation is not possible for a particular
 * data type, we need to create a stub function because the compiler
 * wants a target for the call in scorep_profile_aggegate_ ## type.
 */
#define NOT_PERMITTED_TEMPLATE( name, type )                                        \
static void                                                                         \
name ## _ ## type( type* localValues,                                               \
                   type* aggregatedValues,                                          \
                   scorep_cube_writing_data* writeSet )                             \
{                                                                                   \
    UTILS_BUG( "Calling of function " #name " with type " #type " not permitted." );\
}

/**
 * @def AGGREGATE_SUM_TEMPLATE
 * Template for the sum aggregation function. It requires the '+' operator
 * to work on the basic data type.
 */
#define AGGREGATE_SUM_TEMPLATE( type )                                              \
static void                                                                         \
aggregate_sum_ ## type ( type* localValues,                                         \
                         type* aggregatedValues,                                    \
                         scorep_cube_writing_data* writeSet )                       \
{                                                                                   \
    type result = (type) 0;                                                         \
    for (uint32_t i = 0; i < writeSet->local_threads; i++ )                         \
    {                                                                               \
        result += localValues[i];                                                   \
    }                                                                               \
    *aggregatedValues = result;                                                     \
}
/* *INDENT-ON* */

/**
 * Calculates the sum of an array of doubles.
 * @param localValues      An array with the data values for all threads on this process.
 * @param aggregatedValues An array to which teh aggregated data is written.
 * @param writeSet         Pointer to a data structure with data relevent for writing.
 */
AGGREGATE_SUM_TEMPLATE( double )

/**
 * Calculates the sum of an array of 64-bit unsigned integers.
 * @param localValues      An array with the data values for all threads on this process.
 * @param aggregatedValues An array to which teh aggregated data is written.
 * @param writeSet         Pointer to a data structure with data relevent for writing.
 */
AGGREGATE_SUM_TEMPLATE( uint64_t )

/**
 * Prints an error message. The sum aggregation is not supposed to work on
 * cube_type_tau_atomic.
 */
NOT_PERMITTED_TEMPLATE( aggregate_sum, cube_type_tau_atomic )

/**
 * Calculates the statistical key values over the threads sum.
 * @param localCalues      An array with the data values for all threads on this process.
 * @param aggregatedCalues An array to which teh aggregated data is written.
 * @param writeSet         Pointer to a data structure with data relevent for writing.
 */
static void
aggregate_key_values_cube_type_tau_atomic( cube_type_tau_atomic*     localValues,
                                           cube_type_tau_atomic*     aggregatedValues,
                                           scorep_cube_writing_data* writeSet )
{
    aggregatedValues[ 0 ] = scorep_cube_type_tau_atomic_zero;

    for ( int i = 0; i < writeSet->local_threads; i++ )
    {
        if ( localValues[ i ].N > 0 )
        {
            aggregatedValues[ 0 ].N++;
            aggregatedValues[ 0 ].Sum  += localValues[ i ].Sum;
            aggregatedValues[ 0 ].Sum2 += localValues[ i ].Sum * localValues[ i ].Sum;
            if ( localValues[ i ].Sum < aggregatedValues[ 0 ].Min )
            {
                aggregatedValues[ 0 ].Min = localValues[ i ].Sum;
            }
            if ( localValues[ i ].Sum > aggregatedValues[ 0 ].Max )
            {
                aggregatedValues[ 0 ].Max = localValues[ i ].Sum;
            }
        }
    }
}

/**
 * Prints an error message. The key value aggregation is not supposed to work on
 * uint64_t.
 */
NOT_PERMITTED_TEMPLATE( aggregate_key_values, uint64_t )

/**
 * Prints an error message. The key value aggregation is not supposed to work on
 * double.
 */
NOT_PERMITTED_TEMPLATE( aggregate_key_values, double )

/* **************************************************************************************
 * interface function
 ***************************************************************************************/

uint32_t
scorep_profile_get_aggregated_items( uint32_t                    threadNum,
                                     SCOREP_Profile_OutputFormat format )
{
    switch ( format )
    {
        case SCOREP_PROFILE_OUTPUT_THREAD_SUM:
        case SCOREP_PROFILE_OUTPUT_THREAD_TUPLE:
            return 1;
    }
    return threadNum;
}

/* *INDENT-OFF* */
#define SCOREP_PROFILE_AGGREGATE_TEMPLATE( type )                                   \
void                                                                                \
scorep_profile_aggregate_ ## type ( type** localValues,                             \
                                    type** aggregatedValues,                        \
                                    scorep_cube_writing_data* writeSet )            \
{                                                                                   \
    switch( writeSet->format )                                                      \
    {                                                                               \
    case SCOREP_PROFILE_OUTPUT_THREAD_SUM:                                          \
        aggregate_sum_ ## type( *localValues, *aggregatedValues, writeSet )   ;     \
        return;                                                                     \
                                                                                    \
    case SCOREP_PROFILE_OUTPUT_THREAD_TUPLE:                                        \
        aggregate_key_values_ ## type( *localValues,                                \
                                       *aggregatedValues, writeSet );               \
        return;                                                                     \
                                                                                    \
    }                                                                               \
                                                                                    \
    /* No aggregation: avoid a copy by switching both pointers */                   \
    type* swap = *localValues;                                                      \
    *localValues = *aggregatedValues;                                               \
    *aggregatedValues = swap;                                                       \
}
/* *INDENT-ON* */

SCOREP_PROFILE_AGGREGATE_TEMPLATE( double )

SCOREP_PROFILE_AGGREGATE_TEMPLATE( uint64_t )

SCOREP_PROFILE_AGGREGATE_TEMPLATE( cube_type_tau_atomic )


void
scorep_profile_init_layout( const scorep_cube_writing_data* writeSet,
                            scorep_cube_layout*             layout )
{
    if ( writeSet->has_tasks )
    {
        layout->metric_list = SCOREP_CUBE_METRIC_TASK_METRICS;
    }
    else
    {
        layout->metric_list = SCOREP_CUBE_METRIC_NONE;
    }

    switch ( writeSet->format )
    {
        case SCOREP_PROFILE_OUTPUT_CUBE4:
            layout->location_layout    = SCOREP_CUBE_LOCATION_ALL;
            layout->metric_list       += SCOREP_CUBE_METRIC_VISITS;
            layout->dense_metric_type  = SCOREP_CUBE_DATA_SCALAR;
            layout->sparse_metric_type = SCOREP_CUBE_DATA_SCALAR;
            return;

        case SCOREP_PROFILE_OUTPUT_CUBE_TUPLE:
            layout->location_layout    = SCOREP_CUBE_LOCATION_ALL;
            layout->metric_list       += SCOREP_CUBE_METRIC_VISITS;
            layout->dense_metric_type  = SCOREP_CUBE_DATA_SCALAR;
            layout->sparse_metric_type = SCOREP_CUBE_DATA_TUPLE;
            return;

        case SCOREP_PROFILE_OUTPUT_THREAD_SUM:
            layout->location_layout = SCOREP_CUBE_LOCATION_ONE_PER_PROCESS;
            layout->metric_list    += SCOREP_CUBE_METRIC_VISITS
                                      + SCOREP_CUBE_METRIC_NUM_THREADS;
            layout->dense_metric_type  = SCOREP_CUBE_DATA_SCALAR;
            layout->sparse_metric_type = SCOREP_CUBE_DATA_SCALAR;
            return;

        case SCOREP_PROFILE_OUTPUT_THREAD_TUPLE:
            layout->location_layout    = SCOREP_CUBE_LOCATION_ONE_PER_PROCESS;
            layout->metric_list       += SCOREP_CUBE_METRIC_VISITS;
            layout->dense_metric_type  = SCOREP_CUBE_DATA_TUPLE;
            layout->sparse_metric_type = SCOREP_CUBE_DATA_TUPLE;
            return;

        case SCOREP_PROFILE_OUTPUT_KEY_THREADS:
            layout->location_layout    = SCOREP_CUBE_LOCATION_KEY_THREADS;
            layout->metric_list       += SCOREP_CUBE_METRIC_VISITS;
            layout->dense_metric_type  = SCOREP_CUBE_DATA_SCALAR;
            layout->sparse_metric_type = SCOREP_CUBE_DATA_SCALAR;
            return;

        case SCOREP_PROFILE_OUTPUT_CLUSTER_THREADS:
            layout->location_layout    = SCOREP_CUBE_LOCATION_CLUSTER;
            layout->metric_list       += SCOREP_CUBE_METRIC_VISITS;
            layout->dense_metric_type  = SCOREP_CUBE_DATA_SCALAR;
            layout->sparse_metric_type = SCOREP_CUBE_DATA_SCALAR;
            return;
    }

    UTILS_BUG( "Requested output format %d without layout description.",
               writeSet->format );
}
