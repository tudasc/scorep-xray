/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_PROFILE_TYPES_H
#define SCOREP_PROFILE_TYPES_H

/**
 * @file        scorep_profile_types.h
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Types needed for the profile call tree
 *
 */


#include <SCOREP_Types.h>
#include <SCOREP_DefinitionHandles.h>
#include <stdint.h>
#include <stdbool.h>

/* -------------------------------------------- Preliminary definition of missing types */

//typedef void* SCOREP_StringHandle;

/** List of profile node types */
typedef enum
{
    scorep_profile_node_regular_region,
    scorep_profile_node_parameter_string,
    scorep_profile_node_parameter_integer,
    scorep_profile_node_thread_fork,
    scorep_profile_node_thread_root,
    scorep_profile_node_thread_start
} scorep_profile_node_type;

/** Contains all data for one profile node */
typedef struct scorep_profile_node_struct
{
    SCOREP_CallpathHandle                callpath_handle;
    struct scorep_profile_node_struct*   parent;
    struct scorep_profile_node_struct*   first_child;
    struct scorep_profile_node_struct*   next_sibling;
    scorep_profile_dense_metric*         dense_metrics;
    scorep_profile_sparse_metric_double* first_double_sparse;
    scorep_profile_sparse_metric_int*    first_int_sparse;
    scorep_profile_dense_metric          implicit_time;
    uint64_t                             count;            // For dense metrics
    uint64_t                             first_enter_time; // Required by Scalasca
    uint64_t                             last_exit_time;   // Required by Scalasca
    scorep_profile_node_type             node_type;
    uint64_t                             type_specific_data;
} scorep_profile_node;

/** Thread local data for the profiling system */
typedef struct SCOREP_Profile_LocationData
{
    scorep_profile_node* current_node;  // Current callpath of this thread
    scorep_profile_node* root_node;     // Root node of this thread
    scorep_profile_node* fork_node;     // Last Fork node created by this thread
    scorep_profile_node* creation_node; // Node where the thread was created
} SCOREP_Profile_LocationData;

/** Global profile definition data */
typedef struct
{
    /* Points to the first root node. Further root nodes are added as siblings
       to a root node. */
    scorep_profile_node* first_root_node;

    /* Number of metrics in dense representation. All enter/exit events expect this
       number of metrics. */
    uint32_t num_of_dense_metrics;

    /* Array containing the Metric defintion handle for the metrics in dense
       representation. All enter/exit events expect the metrics in this order. */
    SCOREP_CounterHandle* dense_metrics;
} scorep_profile_definition;

typedef struct
{
    SCOREP_ParameterHandle handle;
    int64_t                value;
} scorep_profile_integer_node_data;

typedef struct
{
    SCOREP_ParameterHandle handle;
    SCOREP_StringHandle    value;
} scorep_profile_string_node_data;


/** Global profile definition instance */
extern scorep_profile_definition scorep_profile;

/** Flag wether the profile is initialized */
extern bool scorep_profile_is_initialized;

/** @def  SCOREP_PROFILE_ASSURE_INITIALIZED
          Check wether the profiling system is initialized.
 */
#define SCOREP_PROFILE_ASSURE_INITIALIZED if ( !scorep_profile_is_initialized ) { return; }

/** @def SCOREP_PROFILE_STOP
    Disables further construction of the profile.
 */
#define SCOREP_PROFILE_STOP scorep_profile_is_initialized = false;


#endif // SCOREP_PROFILE_TYPES_H
