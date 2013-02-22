/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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

#ifndef SCOREP_PROFILE_CUBE4_WRITER_H
#define SCOREP_PROFILE_CUBE4_WRITER_H

/**
 * @file scorep_profile_cube4_writer.h
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 */

#include <SCOREP_DefinitionHandles.h>
#include <scorep_definition_cube4.h>
#include <scorep_profile_node.h>
#include <cubew_cube.h>
#include <cubew_cubew.h>
#include <cubew_services.h>

/**
   Data set needed for various writing functions
 */
typedef struct
{
    cube_t*                       my_cube;          /**< Cube object that is created */
    cube_writer*                  cube_writer;      /**< Cube writer object */
    scorep_profile_node**         id_2_node;        /**< maps global sequence number */
    scorep_cube4_definitions_map* map;              /**< maps Score-P and Cube handles */
    uint32_t                      callpath_number;  /**< Number of callpathes */
    uint32_t                      global_threads;   /**< Global number of locations */
    uint32_t                      local_threads;    /**< Number of threads in this rank */
    uint32_t                      offset;           /**< Offset for this rank */
    uint32_t                      my_rank;          /**< This rank */
    uint32_t                      ranks_number;     /**< Number of ranks in COMM_WORLD */
    int*                          threads_per_rank; /**< List of elements per rank */
    int*                          offsets_per_rank; /**< List of offsets per rank */
    SCOREP_MetricHandle*          metric_map;       /**< map sequence no to handle */
    SCOREP_MetricHandle*          unified_map;      /**< map sequence to unified handle */
    uint8_t*                      bit_vector;       /**< Indicates callpath with values */
    int32_t                       has_tasks;        /**< Whether tasks occured */
    uint32_t                      num_unified;      /**< Number of unified metrics */
    int32_t                       same_thread_num;  /**< Non-zero if same number of
                                                         threads on all ranks */
} scorep_cube_writing_data;

#endif // SCOREP_PROFILE_CUBE4_WRITER_H