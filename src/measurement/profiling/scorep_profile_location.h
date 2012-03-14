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

#ifndef SCOREP_PROFILE_TYPES_H
#define SCOREP_PROFILE_TYPES_H

/**
 * @file        scorep_profile_location.h
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Contains type definition and functions for profile location data.
 *
 */


#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>
#include <scorep_utility/SCOREP_Hashtab.h>
#include <stdint.h>
#include <stdbool.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_task_table.h>

/* **************************************************************************************
   Typedefs
****************************************************************************************/

/**
 * Data structure type for profile location data. Contains information about a
 * location that is needed by the profiling system.
 */
struct SCOREP_Profile_LocationData
{
    scorep_profile_node*                 current_implicit_node; /**< Current callpath of this thread */
    scorep_profile_node*                 root_node;             /**< Root node of this thread */
    scorep_profile_node*                 fork_node;             /**< Last Fork node created by this thread */
    scorep_profile_node*                 creation_node;         /**< Node where the thread was created */
    uint32_t                             current_depth;         /**< Stores the current length of the callpath */
    uint32_t                             fork_depth;            /**< Depth of last fork node */
    uint32_t                             implicit_depth;        /**< Depth of the implicit task */
    scorep_profile_node*                 free_nodes;            /**< List of records for recycling */
    scorep_profile_sparse_metric_int*    free_int_metrics;      /**< List of records for recycling */
    scorep_profile_sparse_metric_double* free_double_metrics;   /**< List of records for recycling */

    scorep_profile_node*                 current_task_node;     /**< Callpath node of the current task */
    scorep_profile_task_table*           tasks;                 /**< Hashtable to track task instances */
    scorep_profile_task*                 current_task;          /**< Current task instance */
};

/* **************************************************************************************
   Functions
****************************************************************************************/

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

#endif // SCOREP_PROFILE_TYPES_H
