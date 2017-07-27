/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012, 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_PROFILE_CUBE4_WRITER_H
#define SCOREP_PROFILE_CUBE4_WRITER_H

/**
 * @file
 *
 *
 */

#include <SCOREP_DefinitionHandles.h>
#include <scorep_definition_cube4.h>
#include <scorep_profile_node.h>
#include <cubew_cube.h>
#include <cubew_services.h>

/**
   Data set needed for various writing functions
 */
typedef struct
{
    /* available on all processes initialization */
    SCOREP_Profile_OutputFormat format;              /**< specifies the output format */
    uint32_t                    my_rank;             /**< This rank */
    uint32_t                    root_rank;           /**< Rank that collects the data */
    uint32_t                    ranks_number;        /**< Number of ranks in COMM_WORLD */
    uint32_t                    local_threads;       /**< Number of threads in this rank */
    uint32_t                    local_items;         /**< Number of items from this rank */
    uint32_t                    callpath_number;     /**< Number of callpathes */
    uint32_t                    global_items;        /**< Global number of items per row */
    uint32_t                    offset;              /**< Offset for this rank */
    uint32_t                    num_unified_metrics; /**< Number of unified metrics */
    uint8_t*                    bit_vector;          /**< Indicates callpath with values */
    int32_t                     has_tasks;           /**< Whether tasks occurred */
    int32_t                     same_thread_num;     /**< Non-zero if same number of
                                                          threads on all ranks */

    /* available on all processes after definition mapping creation */
    scorep_profile_node** id_2_node;                /**< maps global sequence number */
    SCOREP_MetricHandle*  metric_map;               /**< map sequence no to handle */

    /* Use only on root process */
    cube_t* my_cube;                                /**< Cube object that is created */
    int*    items_per_rank;                         /**< List of data items per rank */
    int*    offsets_per_rank;                       /**< List of offsets per rank */

    /* Use only on root process after definition mapping creation */
    SCOREP_MetricHandle*          unified_metric_map; /**< map sequence to unified handle */
    scorep_cube4_definitions_map* map;                /**< maps Score-P and Cube handles */
} scorep_cube_writing_data;

extern const cube_type_tau_atomic scorep_cube_type_tau_atomic_zero;

#endif /* SCOREP_PROFILE_CUBE4_WRITER_H */
