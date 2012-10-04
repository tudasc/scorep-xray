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

#ifndef SCOREP_PROFILE_LOCATION_H
#define SCOREP_PROFILE_LOCATION_H

/**
 * @file        scorep_profile_location.h
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Contains type definition and functions for profile location data.
 *
 */

#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Hashtab.h>
#include <stdint.h>
#include <stdbool.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_task_table.h>

/* **************************************************************************************
   Typedefs
****************************************************************************************/

typedef struct scorep_profile_fork_list_node scorep_profile_fork_list_node;

/**
 * Data structure type for profile location data. Contains information about a
 * location that is needed by the profiling system.
 */
struct SCOREP_Profile_LocationData
{
    scorep_profile_node*                 current_implicit_node;         /**< Current callpath of this thread */
    scorep_profile_node*                 root_node;                     /**< Root node of this thread */
    scorep_profile_node*                 creation_node;                 /**< Node where the thread was created */
    uint32_t                             current_depth;                 /**< Stores the current length of the callpath */
    uint32_t                             implicit_depth;                /**< Depth of the implicit task */
    scorep_profile_node*                 free_nodes;                    /**< List of records for recycling */
    scorep_profile_sparse_metric_int*    free_int_metrics;              /**< List of records for recycling */
    scorep_profile_sparse_metric_double* free_double_metrics;           /**< List of records for recycling */

    scorep_profile_node*                 current_task_node;             /**< Callpath node of the current task */
    scorep_profile_task_table*           tasks;                         /**< Hashtable to track task instances */
    scorep_profile_task*                 current_task;                  /**< Current task instance */
    uint32_t                             num_location_specific_metrics; /**< Number of scoped metrics recorded by this location
                                                                             (in addition to dense metrics recorded by all locations) */
    SCOREP_Location*                     location_data;                 /**< Pointer to the Score-P location */
    scorep_profile_fork_list_node*       fork_list_head;                /**< Pointer to the list head of fork points */
    scorep_profile_fork_list_node*       fork_list_tail;                /**< Pointer to the list tail of fork points */
};

/* **************************************************************************************
   Functions
****************************************************************************************/

/**
   Creates a new locationd data structure.
 */
SCOREP_Profile_LocationData*
scorep_profile_create_location_data( SCOREP_Location* locationData );

/**
   Deletes a location data structure.
   @param location Location that is deleted.
 */
void
scorep_profile_delete_location_data( SCOREP_Profile_LocationData* location );

/**
   Finalizes a location data structure. Is called during profile finalization.
   Keeps the data structure alive and allows reuse of it after a
   reinitialization of the profile.
   @param location Location that is finalized.
 */
void
scorep_profile_finalize_location( SCOREP_Profile_LocationData* location );

/**
   Reinitializes a location data object after profile finalization and
   reinitialization of the profile.
   @param location Location that is reinitialized.
 */
void
scorep_profile_reinitialize_location( SCOREP_Profile_LocationData* locationData );

/**
   Returns the current node for a thread
 */
scorep_profile_node*
scorep_profile_get_current_node( SCOREP_Profile_LocationData* location );

/**
   Sets the current node for a thread
 */
void
scorep_profile_set_current_node( SCOREP_Profile_LocationData* location,
                                 scorep_profile_node*         node );

/**
 * Sets number of location specific metrics
 *
 * @param location                      Location.
 * @param numLocationSpecificMetrics    Number of location specific metrics.
 */
void
scorep_profile_set_num_location_metrics( SCOREP_Profile_LocationData* location,
                                         uint32_t                     numLocationSpecificMetrics );

/**
 * Gets number of location specific metrics
 *
 * @param location                      Location.
 */
uint32_t
scorep_profile_get_num_location_metrics( SCOREP_Profile_LocationData* location );

/**
 * Adds a fork node to the list
 *
 * @param location      Location.
 * @param fork_node     The fork node.
 * @param profile_depth Depth of the profile node in the call tree.
 * @param nesting_level The nesting level of the forked paralle region.
 */
void
scorep_profile_add_fork_node( SCOREP_Profile_LocationData* location,
                              scorep_profile_node*         fork_node,
                              uint32_t                     profile_depth,
                              uint32_t                     nesting_level );

/**
 * Returns the fork node of the parallel region at @a nesting_level.
 *
 * @param location      Location.
 * @param nesting_level The nesting level of the forked paralle region.
 */
scorep_profile_node*
scorep_profile_get_fork_node( SCOREP_Profile_LocationData* location,
                              uint32_t                     nesting_level );


/**
 * Returns the depth level of fork node of the parallel region at @a nesting_level.
 *
 * @param location      Location.
 * @param nesting_level The nesting level of the forked paralle region.
 */
uint32_t
scorep_profile_get_fork_depth( SCOREP_Profile_LocationData* location,
                               uint32_t                     nesting_level );


/**
 * Removes the fork node of the parallel region at @a nesting_level,
 * from the list of fork nodes.
 *
 * @param location      Location.
 * @param nesting_level The nesting level of the forked paralle region.
 */
void
scorep_profile_remove_fork_node( SCOREP_Profile_LocationData* location,
                                 uint32_t                     nesting_level );

#endif // SCOREP_PROFILE_LOCATION_H
