/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_DEFINITION_CUBE4_H
#define SCOREP_DEFINITION_CUBE4_H

/**
 * @file
 *
 *
 * Declares functions and types needed for writing definitions in Cube 4
 * format.
 */

#include <SCOREP_Hashtab.h>

#include <cubew_cubew.h>
#include <cubew_cube.h>
#include <SCOREP_Definitions.h>

#include <stdbool.h>

/**
   Contains the mapping structs for Cube 4 handles to SCOREP handles.
 */
typedef struct
{
    /* For retriveing Cube handles from SCOREP handles */
    SCOREP_Hashtab* region_table_cube;
    SCOREP_Hashtab* metric_table_cube;
    SCOREP_Hashtab* callpath_table_cube;

    /* For retriveing SCOREP handles from Cube handles */
    SCOREP_Hashtab* region_table_scorep;
    SCOREP_Hashtab* metric_table_scorep;
    SCOREP_Hashtab* callpath_table_scorep;
} scorep_cube4_definitions_map;

/**
   Writes the definitions to the Cube 4 struct, given by @a my_cube. Fills
   the mapping table given by map.
   @param my_cube Pointer to the cube struct to which the data is written.
   @param map     Pointer to an already inititialized mapping structure.
   @param ranks   Number of ranks.
   @param offsets Offset for global thread enumeration.
   @param write_task_metrics Indicates whether task related metric defintions are
                  written to the Cube file.
   @param write_tupels Indicates whether tuples are written.
 */
void
scorep_write_definitions_to_cube4( cube_t*                       my_cube,
                                   scorep_cube4_definitions_map* map,
                                   uint32_t                      ranks,
                                   int*                          offsets,
                                   bool                          write_task_metrics,
                                   bool                          write_tuples );

/**
   Creates an instance of @ref scorep_cube4_definitions_map.
   @param returns a pointer to the newly created instance. If an error occured,
   NULL is returned.
 */
scorep_cube4_definitions_map*
scorep_cube4_create_definitions_map( void );

/**
   Deletes the scorep_cube4_definitions_map given by @a map. All elements of
   the mapping tables are deleted and all memory is freed.
   @param map Pointer to the map that will be deleted. If it is NULL.
              nothing happens.
 */
void
scorep_cube4_delete_definitions_map( scorep_cube4_definitions_map* map );

/**
   Adds a region definition pair to the mapping.
   @param map         Pointer to the mapping structure, to which the mapping
                      is added.
   @param cube_handle Pointer to the cube region definition structure.
   @param scorep_handle SCOREP handle for the region.
 */
void
scorep_cube4_add_region_mapping( scorep_cube4_definitions_map* map,
                                 cube_region*                  cube_handle,
                                 SCOREP_RegionHandle           scorep_handle );

/**
   Adds a callpath definition pair to the mapping.
   @param map         Pointer to the mapping structure, to which the mapping
                      is added.
   @param cube_handle Pointer to the cube cnode definition structure.
   @param scorep_handle SCOREP handle for the callpath.
 */
void
scorep_cube4_add_callpath_mapping( scorep_cube4_definitions_map* map,
                                   cube_cnode*                   cube_handle,
                                   SCOREP_CallpathHandle         scorep_handle );

/**
   Adds a metric definition pair to the mapping.
   @param map         Pointer to the mapping structure, to which the mapping
                      is added.
   @param cube_handle Pointer to the cube metric definition structure.
   @param scorep_handle SCOREP handle for the metric.
 */
void
scorep_cube4_add_metric_mapping( scorep_cube4_definitions_map* map,
                                 cube_metric*                  cube_handle,
                                 SCOREP_MetricHandle           scorep_handle );

/**
   Returns the cube_region handle for a given scorep region handle.
   @param map     Pointer to the mapping struct.
   @param handle  The SCOREP region handle for which the cube handle is returned.
   @returns The cube handle for the region which is mapped to @a handle by the
            mapping. If @a handle was not found in the mapping, NULL is
            returned.
 */
cube_region*
scorep_get_cube4_region( scorep_cube4_definitions_map* map,
                         SCOREP_RegionHandle           handle );

/**
   Returns the cube_metric handle for a given scorep metric handle.
   @param map     Pointer to the mapping struct.
   @param handle  The SCOREP metric handle for which the cube handle is returned.
   @returns The cube handle for the metric which is mapped to @a handle by the
            mapping. If @a handle was not found in the mapping, NULL is
            returned.
 */
cube_metric*
scorep_get_cube4_metric( scorep_cube4_definitions_map* map,
                         SCOREP_MetricHandle           handle );

/**
   Returns the cube_cnode handle for a given scorep callpath handle.
   @param map     Pointer to the mapping struct.
   @param handle  The SCOREP callpath handle for which the cube handle is
                  returned.
   @returns The cube handle for the callpath which is mapped to @a handle by
            the mapping. If @a handle was not found in the mapping, NULL is
            returned.
 */
cube_cnode*
scorep_get_cube4_callpath( scorep_cube4_definitions_map* map,
                           SCOREP_CallpathHandle         handle );

/**
   Returns the SCOREP region handle for a given cube handle.
   @param map     Pointer to the mapping struct.
   @param handle  The Cube region handle for which the SCOREP handle is returned.
   @returns The SCOREP handle for the region which is mapped to @a handle by the
            mapping. If @a handle was not found in the mapping,
            SCOREP_INVALID_REGION is returned.
 */
SCOREP_RegionHandle
scorep_get_region_from_cube4( scorep_cube4_definitions_map* map,
                              cube_region*                  handle );

/**
   Returns the SCOREP counter handle for a given cube handle.
   @param map     Pointer to the mapping struct.
   @param handle  The Cube metric handle for which the SCOREP handle is returned.
   @returns The SCOREP handle for the metric which is mapped to @a handle by the
            mapping. If @a handle was not found in the mapping,
            SCOREP_INVALID_METRIC is returned.
 */
SCOREP_MetricHandle
scorep_get_metric_from_cube4( scorep_cube4_definitions_map* map,
                              cube_metric*                  handle );

/**
   Returns the SCOREP callpath handle for a given cube handle.
   @param map     Pointer to the mapping struct.
   @param handle  The Cube cnode handle for which the SCOREP handle is returned.
   @returns The SCOREP handle for the callpath which is mapped to @a handle by
            the mapping. If @a handle was not found in the mapping,
            SCOREP_INVALID_CALLPATH is returned.
 */
SCOREP_CallpathHandle
scorep_get_callpath_from_cube4( scorep_cube4_definitions_map* map,
                                cube_cnode*                   handle );


/**
   Returns the number of callpathes.
 */
uint64_t
scorep_cube4_get_number_of_callpathes( scorep_cube4_definitions_map* map );

/**
   Returns the handle used for the metric 'number of visits'.
 */
cube_metric*
scorep_get_visits_handle( void );

/**
   Returns the handle used for metric 'inclusive time sum'.
 */
cube_metric*
scorep_get_sum_time_handle( void );

/**
   Returns the handle used for metric 'inclusive time maximum'.
 */
cube_metric*
scorep_get_max_time_handle( void );

/**
   Returns the handle used for metric 'inclusive time minimum'.
 */
cube_metric*
scorep_get_min_time_handle( void );

#endif /* SCOREP_DEFINITION_CUBE4_H */
