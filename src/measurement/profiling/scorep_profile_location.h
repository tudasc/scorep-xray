/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015,
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

#ifndef SCOREP_PROFILE_LOCATION_H
#define SCOREP_PROFILE_LOCATION_H

/**
 * @file
 *
 * @brief Contains type definition and functions for profile location data.
 *
 */

#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_Profile_Tasking.h>
#include <scorep_profile_definition.h>
#include <scorep_location_management.h>

#include <stdint.h>
#include <stdbool.h>

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
    scorep_profile_node*                 current_implicit_node;    /**< Current callpath of this thread */
    scorep_profile_node*                 root_node;                /**< Root node of this thread */
    scorep_profile_node*                 creation_node;            /**< Node where the thread was created */
    uint32_t                             current_depth;            /**< Stores the current length of the callpath */
    uint32_t                             implicit_depth;           /**< Depth of the implicit task */
    scorep_profile_node*                 free_nodes;               /**< List of records for recycling */
    scorep_profile_sparse_metric_int*    free_int_metrics;         /**< List of records for recycling */
    scorep_profile_sparse_metric_double* free_double_metrics;      /**< List of records for recycling */
    scorep_profile_node*                 free_stubs;               /**< List of stubs for recycling */
    scorep_profile_node*                 foreign_stubs;            /**< List of stubs from other locations */
    uint32_t                             num_foreign_stubs;        /**< Number of objects in @a foreign_stubs */

    scorep_profile_node*                 current_task_node;        /**< Callpath node of the current task */
    struct scorep_profile_task*          current_task;             /**< Current task instance */
    struct scorep_profile_task*          implicit_task;            /** Impliciti task instance */
    struct scorep_profile_task*          free_tasks;               /**< Released task structures */
    struct scorep_profile_task*          foreign_tasks;            /**< Released tasks from other creators */
    uint32_t                             num_foreign_tasks;        /**< Number of objects in @a foreign_tasks */
    int64_t                              migration_sum;            /**< Sum of task migrations from/to this location */
    uint64_t                             migration_win;            /**< Number of tasks that migrated here */
    SCOREP_Location*                     location_data;            /**< Pointer to the Score-P location */
    scorep_profile_fork_list_node*       fork_list_head;           /**< Pointer to the list head of fork points */
    scorep_profile_fork_list_node*       fork_list_tail;           /**< Pointer to the list tail of fork points */
    struct scorep_profile_io_paradigm*   current_io_paradigm;
};

/**
 * Stores the substrate id of the profiling substrate. Set during initialization.
 */
extern size_t scorep_profile_substrate_id;

/* **************************************************************************************
   Functions
****************************************************************************************/

/**
 * Initializes the memory object exchange.
 */
void
scorep_profile_initialize_exchange( void );

/**
 * Finalizes the memory object exchange.
 */
void
scorep_profile_finalize_exchange( void );

/**
 * Creates a new location data structure.
 */
SCOREP_Profile_LocationData*
scorep_profile_create_location_data( SCOREP_Location* locationData );

/**
 * Deletes a location data structure.
 * @param location Location that is deleted.
 */
void
scorep_profile_delete_location_data( SCOREP_Profile_LocationData* location );

/**
 * Finalizes a location data structure. Is called during profile finalization.
 * Keeps the data structure alive and allows reuse of it after a
 * reinitialization of the profile.
 * @param location Location that is finalized.
 */
void
scorep_profile_finalize_location( SCOREP_Profile_LocationData* location );

/**
 * Reinitializes a location data object after profile finalization and
 * reinitialization of the profile.
 * @param location Location that is reinitialized.
 */
void
scorep_profile_reinitialize_location( SCOREP_Profile_LocationData* locationData );

/**
 * Returns the current node for a thread
 */
scorep_profile_node*
scorep_profile_get_current_node( SCOREP_Profile_LocationData* location );

/**
 * Sets the current node for a thread
 */
void
scorep_profile_set_current_node( SCOREP_Profile_LocationData* location,
                                 scorep_profile_node*         node );

/**
 * Adds a fork node to the list
 *
 * @param location      Location.
 * @param fork_node     The fork node.
 * @param profileDepth  Depth of the profile node in the call tree.
 * @param nestingLevel  The nesting level of the forked paralle region.
 */
void
scorep_profile_add_fork_node( SCOREP_Profile_LocationData* location,
                              scorep_profile_node*         fork_node,
                              uint32_t                     profileDepth,
                              uint32_t                     nestingLevel );

/**
 * Returns the fork node of the parallel region at @a nesting_level.
 *
 * @param location      Location.
 * @param nestingLevel  The nesting level of the forked paralle region.
 */
scorep_profile_node*
scorep_profile_get_fork_node( SCOREP_Profile_LocationData* location,
                              uint32_t                     nestingLevel );


/**
 * Returns the depth level of fork node of the parallel region at @a nesting_level.
 *
 * @param location      Location.
 * @param nestingLevel  The nesting level of the forked paralle region.
 */
uint32_t
scorep_profile_get_fork_depth( SCOREP_Profile_LocationData* location,
                               uint32_t                     nestingLevel );


/**
 * Removes the fork node of the parallel region at @a nesting_level,
 * from the list of fork nodes.
 *
 * @param location      Location.
 */
void
scorep_profile_remove_fork_node( SCOREP_Profile_LocationData* location );

/**
 * Releases a chain of stub nodes for recycling. The nodes must be
 * connected via parent-child links in the scorep_profile_node object.
 * @param location      The location which executes the release.
 * @param root          Pointer to the root of the chain.
 * @param leaf          Pointer to the last child end of the chain.
 * @param num           Number of elements in the chain.
 * @param localObjects  True if the task has not migrated during last switch.
 */
void
scorep_profile_release_stubs( SCOREP_Profile_LocationData* location,
                              scorep_profile_node*         root,
                              scorep_profile_node*         leaf,
                              uint32_t                     num,
                              bool                         localObjects );

/**
 * Returns a recycled stub objects if available.
 * @param location  The location on which a stub node is requested.
 * @retuns a recycled stub object if available. Otherwise it returns NULL.
 */
scorep_profile_node*
scorep_profile_recycle_stub( SCOREP_Profile_LocationData* location );

/**
 * Releases a task object for recycling.
 * @param location The location which executes the release.
 * @param task     The task object that should be released.
 */
void
scorep_profile_release_task( SCOREP_Profile_LocationData* location,
                             struct scorep_profile_task*  task );

/**
 * Returns a recycled task object if available.
 * @param location  The location on which the new task object is requested.
 * @retuns a recycled task object if available. Otherwise it returns NULL.
 */
struct scorep_profile_task*
scorep_profile_recycle_task( SCOREP_Profile_LocationData* location );


static inline SCOREP_Profile_LocationData*
scorep_profile_get_profile_data( struct SCOREP_Location* location )
{
    return ( SCOREP_Profile_LocationData* )
           SCOREP_Location_GetSubstrateData( location,
                                             scorep_profile_substrate_id );
}

#endif /* SCOREP_PROFILE_LOCATION_H */
