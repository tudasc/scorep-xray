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

#ifndef SCOREP_DEFINITION_CUBE4_H
#define SCOREP_DEFINITION_CUBE4_H

/**
 * @file scorep_definition_cube4.h
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 * Declares functions and types needed for writing definitions in Cube 4
 * format.
 */

#include <scorep_utility/SCOREP_Utils.h>

#include <cubew_cubew.h>
#include <cubew_cube.h>
#include <SCOREP_Definitions.h>

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
   @param ranks   Number of ranks
   @param threads Array with @a ranks entries. Each entry specify the number of threads
                  on a rank with this index.
 */
void
scorep_write_definitions_to_cube4( cube_t*                       my_cube,
                                   scorep_cube4_definitions_map* map,
                                   uint32_t                      ranks,
                                   int*                          threads );

/**
   Creates an instance of @ref scorep_cube4_definitions_map.
   @param returns a pointer to the newly created instance. If an error occured,
   NULL is returned.
 */
scorep_cube4_definitions_map*
scorep_cube4_create_definitions_map();

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
   @param cube_handle Pointer to the cube cnode defintion structure.
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
   @param cube_handle Pointer to the cube metric defintion structure.
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
   Returns the handle used for metric 'inclusive time'.
 */
SCOREP_MetricHandle
scorep_get_time_metric_handle();

/**
   Returns the handle used for the metric 'number of visits'.
 */
SCOREP_MetricHandle
scorep_get_visits_metric_handle();

#endif /* SCOREP_DEFINITION_CUBE4_H */
