/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_PROFILE_AGGREGATION_H
#define SCOREP_PROFILE_AGGREGATION_H

/**
 * @file
 *
 * The file defines aggregation functions used by other compilation units
 * of the profile.
 */

#include <scorep_profile_cube4_writer.h>
#include <scorep_definition_cube4.h>


/* *INDENT-OFF* */

/*
 * Template for the generation of aggregation function header definitions.
 * for different data types.
 */
#define SCOREP_PROFILE_AGGREGATE_HEADER_TEMPLATE( type )                   \
void                                                                       \
scorep_profile_aggregate_ ## type ( type** localValues,                    \
                                    type** aggregatedValues,               \
                                    scorep_cube_writing_data* writeSet )
/* *INDENT-ON* */

/**
 * Aggregates data of type double for one metric, one callpath, one one process.
 * @param localValues       An array with the data values for all threads on this process.
 * @param aggregatedValues  An array to which teh aggregated data is written.
 * @param writeSet          Pointer to a data structure with data relevent for writing.
 */
SCOREP_PROFILE_AGGREGATE_HEADER_TEMPLATE( double );

/**
 * Aggregates data of 64-bit unsigned integers for one metric, one callpath, one one
 * process.
 * @param localValues       An array with the data values for all threads on this process.
 * @param aggregatedValues  An array to which teh aggregated data is written.
 * @param writeSet          Pointer to a data structure with data relevent for writing.
 */
SCOREP_PROFILE_AGGREGATE_HEADER_TEMPLATE( uint64_t );

/**
 * Aggregates data of type cube_type_tau_atomic for one metric, one callpath, one one
 * process.
 * @param localValues       An array with the data values for all threads on this process.
 * @param aggregatedValues  An array to which teh aggregated data is written.
 * @param writeSet          Pointer to a data structure with data relevent for writing.
 */
SCOREP_PROFILE_AGGREGATE_HEADER_TEMPLATE( cube_type_tau_atomic );

#undef SCOREP_PROFILE_AGGREGATE_HEADER_TEMPLATE

/**
 * Determines how many data values are written on the current process.
 * @param threadNum  The number of treads on this process.
 * @param format     The profile output format.
 */
uint32_t
scorep_profile_get_aggregated_items( uint32_t                    threadNum,
                                     SCOREP_Profile_OutputFormat format );

/**
 * Initializes the layout structure for a given format.
 * @param writeSet Pointer to a data structure with data relevent for writing.
 * @param layout   The layout structure that is initialized.
 */
void
scorep_profile_init_layout( const scorep_cube_writing_data* writeSet,
                            scorep_cube_layout*             layout );

#endif
