/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2014-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011, 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_PROFILE_H
#define SCOREP_PROFILE_H

/**
 * @file
 *
 * @brief Interface called by the measurement core to construct the profile.
 *
 * The profile is created with the following structure:
 * For each location a node of type @ref scorep_profile_node_thread_root is created which
 * are linked together as siblings. The children of the @ref scorep_profile_node_thread_root
 * nodes are of type @ref scorep_profile_node_thread_start and represent each an activation
 * (start of a parallel region in OpenMP). In the initial thread one
 * @ref scorep_profile_node_thread_start node is created, too. They contain informatation
 * from which callpath they are created. Their children are the region or parameter
 * nodes called from within the parallel region.
 *
 * Whenever a region is entered, a child of the node representing the current callpath
 * is created, if it does not already exist. If a parameter is triggered a new node
 * for this parameter value is created if it does not already exists.
 *
 * During post-processing the structure of the profile altered. First the nodes of
 * type @scorep_profile_node_thread_start are expanded, so that for each thread the full
 * calltree appears below its root node.
 */


#include <SCOREP_Location.h>
#include <scorep_substrates_definition.h>

/* ------------------------------------------------------------------ Type definitions */

/**
   Thread local data for the profiling system.
 */
typedef struct SCOREP_Profile_LocationData SCOREP_Profile_LocationData;

/**
   The type for specifing the output format of the profile.
 */
typedef enum
{
    SCOREP_PROFILE_OUTPUT_NONE            = 0,
    SCOREP_PROFILE_OUTPUT_TAU_SNAPSHOT    = 1,
    SCOREP_PROFILE_OUTPUT_CUBE4           = 2,
    SCOREP_PROFILE_OUTPUT_CUBE_TUPLE      = 3,
    SCOREP_PROFILE_OUTPUT_THREAD_SUM      = 4,
    SCOREP_PROFILE_OUTPUT_THREAD_TUPLE    = 5,
    SCOREP_PROFILE_OUTPUT_KEY_THREADS     = 6,
    SCOREP_PROFILE_OUTPUT_CLUSTER_THREADS = 7
} SCOREP_Profile_OutputFormat;

#define SCOREP_PROFILE_OUTPUT_DEFAULT SCOREP_PROFILE_OUTPUT_CUBE4

/**
   The clustering mode.
 */
typedef enum
{
    /** No structural similarity required. */
    SCOREP_PROFILE_CLUSTER_NONE           = 0,
    /** The sub-trees structure must match. */
    SCOREP_PROFILE_CLUSTER_SUBTREE        = 1,
    /** The sub-trees structure and the number of
        visits must match. */
    SCOREP_PROFILE_CLUSTER_SUBTREE_VISITS = 2,
    /** The structure of the call-path to MPI calls must match.
        Nodes that are not on an MPI call-path may differ. */
    SCOREP_PROFILE_CLUSTER_MPI            = 3,
    /** Like above, but the number of visits of the MPI calls must match, too. */
    SCOREP_PROFILE_CLUSTER_MPI_VISITS     = 4,
    /** Like above, but the number of visits must match also match on all
        nodes on the call-path to an MPI function. */
    SCOREP_PROFILE_CLUSTER_MPI_VISITS_ALL = 5,
} SCOREP_Profile_ClusterMode;

/* ----------------------------------------------------- Initialization / Finalization */


/**
   Registers configuration variables to the SCOREP measurement system. Enables the
   configuration and program start from SCOREP configuration mechanisms.
 */
void
SCOREP_Profile_Register( void );


/**
   Initializes the profiling substrate.
   @param substrateId Id to access profiling-specific location and task data.
 */
void
SCOREP_Profile_Initialize( size_t substrateId );


/**
   Deletes a existing profile and profile definition. Before other profile functions
   are called, the profiling system needs to be reinitialized by a call to
   @ref SCOREP_Profile_Initialize.
   @return The substrate id that was passed to SCOREP_Profile_Initialize() in
   order to reinitialize not using the subsystem mechanism (see
   SCOREP_OA_RequestsSubmit()).
 */
size_t
SCOREP_Profile_Finalize( void );


/**
   Post processes a profile. This function performs transformations on the calltree
   like expansion of thread start nodes and assignment of callpath handles to every node.
   Which steps are included can be specified via parameters. It must be called before
   unification in order to register callpathes to be unified.
 */
void
SCOREP_Profile_Process( void );


/* -------------------------------------------------------------------- Callpath Events */


/**
   Called on enter events to update the profile accordingly.
   @param thread    A pointer to the thread location data of the thread that executed
                    the enter event.
   @param timestamp The timestamp, when the region was entered.
   @param region    The handle of the entered region.
   @param metrics   An array with metric samples which were taken on the enter event.
                    The samples must be in the same order as the metric definitions
                    at the @ref SCOREP_Profile_Initialize call.
 */
void
SCOREP_Profile_Enter( SCOREP_Location*    thread,
                      uint64_t            timestamp,
                      SCOREP_RegionHandle region,
                      uint64_t*           metrics );


/**
   Called on exit events to update the profile accordingly.
   @param thread    A pointer to the thread location data of the thread that executed
                    the exit event.
   @param timestamp The timestamp, when the region was left.
   @param region    The handle of the left region.
   @param metrics   An array with metric samples which were taken on the exit event.
                    The samples must be in the same order as the metric definitions
                    at the @ref SCOREP_Profile_Initialize call.
 */
void
SCOREP_Profile_Exit( SCOREP_Location*    thread,
                     uint64_t            timestamp,
                     SCOREP_RegionHandle region,
                     uint64_t*           metrics );


/**
   Called when a user metric / atomic / context event for integer values was triggered.
   @param thread A pointer to the thread location data of the thread that executed
                 the event.
   @param metric Handle of the triggered metric.
   @param value  Sample for the metric.
 */
void
SCOREP_Profile_TriggerInteger( SCOREP_Location*    thread,
                               SCOREP_MetricHandle metric,
                               uint64_t            value );


/**
   Called when a user metric / atomic / context event for double values was triggered.
   @param thread A pointer to the thread location data of the thread that executed
                 the event.
   @param metric Handle of the triggered metric.
   @param value  Sample for the metric.
 */
void
SCOREP_Profile_TriggerDouble( SCOREP_Location*    thread,
                              SCOREP_MetricHandle metric,
                              double              value );


/**
   Called when a string parameter was triggered
   @param thread    A pointer to the thread location data of the thread that executed
                    the event.
   @param timestamp Unused.
   @param param     Handle of the triggered parameter.
   @param string    Handle of the parameter string value.
 */
void
SCOREP_Profile_ParameterString( SCOREP_Location*       thread,
                                uint64_t               timestamp,
                                SCOREP_ParameterHandle param,
                                SCOREP_StringHandle    string );


/* --------------------------------------------------------------------- Thread Events */


/**
   Returns an array of callbacks for management purposes.
   @return Array of profiling substrate management callbacks.
 */
const SCOREP_Substrates_Callback*
SCOREP_Profile_GetSubstrateMgmtCallbacks( void );

/**
   Returns an array of callbacks for run time events.
   @param mode Recording enabled/disabled.
   @return Array of profiling substrate callbacks for the requested mode.
 */
const SCOREP_Substrates_Callback*
SCOREP_Profile_GetSubstrateCallbacks( SCOREP_Substrates_Mode mode );


#endif /* SCOREP_PROFILE_H */
