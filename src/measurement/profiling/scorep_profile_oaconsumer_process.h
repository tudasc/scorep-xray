/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_OA_CONSUMER_PROCESS_H
#define SCOREP_OA_CONSUMER_PROCESS_H

/**
 * @file
 *
 * @brief Helper functions for processing and transformation of profile for the needs of OA
 *
 */

#include "SCOREP_Profile_OAConsumer.h"
#include "SCOREP_Hashtab.h"

#include "scorep_profile_node.h"
#include "scorep_profile_definition.h"
#include <SCOREP_Definitions.h>

/**
   Contains index for the call-path profile of the OA phase region shared between all threads. It is constructed at the end
   of Online Access phase in order to ease transformation of and access to the call-path profile measurements
   for transferring them over Online Access Interface. Shared index consists of the merged region definition index,
   number of region definitions, number of flat profile measurements, number of counter definitions and pointers
   to transmission buffers for region definitions, flat profile and counter definitions. While constructing shared
   index merging of the region definitions is performed. This preprocessing
   assigns file name and the line numbers of the parent region (according to the call-tree hierarchy) to the
   regions definitions, which are missing this data when registered (e.g. MPI regions or OMP implicit barriers).
   Index is stored in form of a hash-tables: merged region definitions index where the key is a
   unique identifier of a merged region definition (region id, parent region id). The payload of the table
   is an integer index from 0 to num_def_regions_merged of a definition. This index is used for placement of
   the definitions into OA transmission buffers and cross-referencing between buffered definitions and measurements on the receive side.

   Here is a list which data this field contains:
   <dl>
    <dt>rank</dt>
    <dd>MPI rank of the profile index</dd>
    <dt>num_def_regions_merged</dt>
    <dd>Number of merged region definitions </dd>
    <dt>num_static_measurements</dt>
    <dd>Number of flat profile measurements</dd>
    <dt>num_counter_definitions</dt>
    <dd>Number of counter definitions</dd>
    <dt>merged_regions_def_table</dt>
    <dd>A pointer to the hash table containing index of the merged region definitions/dd>
    <dt>static_measurement_buffer</dt>
    <dd>A pointer to the buffer containing flat profile measurements</dd>
    <dt>merged_region_def_buffer</dt>
    <dd>A pointer to the buffer containing merged region definitions</dd>
    <dt>counter_definition_buffer</dt>
    <dd>A pointer to the buffer containing metric definitions</dd>
    <dt>dense_metrics_sampling_set</dt>
    <dd>A pointer to the definition of the profiling dense metric sampling set</dd>
   </dl>
 */
typedef struct
{
    uint64_t                          rank;
    uint32_t                          num_static_measurements;
    uint32_t                          num_def_regions_merged;
    uint32_t                          num_counter_definitions;
    SCOREP_Hashtab*                   merged_regions_def_table;     ///Hash table for mapping already registered region names region handles.
    SCOREP_OA_CallPathRegionDef*      merged_region_def_buffer;
    SCOREP_OA_FlatProfileMeasurement* static_measurement_buffer;
    SCOREP_OA_CallPathCounterDef*     counter_definition_buffer;
    SCOREP_SamplingSetDef*            dense_metrics_sampling_set;
} shared_index_type;

/**
   Contains index for the call-path profile of the OA phase region of one thread. It is constructed at the end
   of Online Access phase in order to ease transformation of and access to the call-path profile measurements
   for transferring them over Online Access Interface. While constructing the index flattening of the profile
   is performed. The preprocessing sums up profile measurements of the call-paths having the same region definition.
   Index is stored in form of a hash-table: flat profile measurements index where the key is a
   unique identifier of a flat profile measurement (parent region id, region id and counter id). The payload of the table
   is an integer index from 0 to num_static_measurements. This index is used for placement of the measurements into OA transmission
   buffer and cross-referencing between buffered definitions and measurements on the receive side.

   Here is a list which data this field contains:
   <dl>
    <dt>phase_node</dt>
    <dd>A pointer to the phase node of the profile call-tree</dd>
    <dt>thread</dt>
    <dd>Thread number of the profile index</dd>
    <dt>static_measurements_table</dt>
    <dd>A pointer to the hash table containing index of the flat profile measurements</dd>
    <dt>shared_index</dt>
    <dd>A pointer to the shared index data structure</dd>
   </dl>
 */
typedef struct
{
    scorep_profile_node* phase_node;
    uint32_t             thread;
    SCOREP_Hashtab*      static_measurements_table;
    shared_index_type*   shared_index;
} thread_private_index_type;

/**
   Contains a structure for a hash table key

   Here is a list which data this field contains:
   <dl>
    <dt>parent_region_id</dt>
    <dd>Region id of the parent region</dd>
    <dt>region_id</dt>
    <dd>Region id</dd>
    <dt>metric_id</dt>
    <dd>Metric id </dd>
   </dl>
 */
typedef struct
{
    uint32_t parent_region_id;
    uint32_t region_id;
    uint32_t metric_id;
} scorep_oa_key_type;

/*----------------------------------------------------------------------------------------
   Getters
   -------------------------------------------------------------------------------------*/

/** Returns number of thread root nodes in profile
 */
uint32_t
scorep_oaconsumer_get_number_of_roots( void );

/** Returns a pointer to the buffer with flat profile measurements of the given thread
 */
SCOREP_OA_FlatProfileMeasurement*
scorep_oaconsumer_get_static_profile_measurements( thread_private_index_type** privateIndexPointerArray );

/** Returns a pointer to the buffer with merged region definitions
 */
SCOREP_OA_CallPathRegionDef*
scorep_oaconsumer_get_merged_region_definitions( thread_private_index_type** privateIndexPointerArray );

/** Returns a pointer to the buffer with metric definitions
 */
SCOREP_OA_CallPathCounterDef*
scorep_oaconsumer_get_metric_definitions( thread_private_index_type** privateIndexPointerArray );

/** Allocates an array of pointers to indexes. Each thread has it is own index data structure.
 *  Also phase node corresponding to a given phase_handle is searched in each call-tree of each thread
 *  and stored in phase_node entry of the data_index_type
 */
thread_private_index_type**
scorep_oaconsumer_initialize_index( SCOREP_RegionHandle phaseHandle );

/** A function which creates index for a given profile node. A pointer to this function is passed
 * to @ref scorep_profile_for_all() which parses the profile call-tree
 *
 * @param node a root node for starting tree parsing
 * @param param a void pointer to index data structure of a currently indexed thread
 */
void
scorep_oaconsumer_count_index( scorep_profile_node* node,
                               void*                param );

#endif /* SCOREP_OA_CONSUMER_PROCESS_H */
