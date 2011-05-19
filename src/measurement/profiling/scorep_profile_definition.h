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

#ifndef SCOREP_PROFILE_DEFINITION_H
#define SCOREP_PROFILE_DEFINITION_H

/**
 * @file        scorep_profile_definition.h
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief The global profile definition struct and functions
 *
 */

#include <stdint.h>

#include "scorep_profile_node.h"

/** @def  SCOREP_PROFILE_ASSURE_INITIALIZED
          Check wether the profiling system is initialized.
 */
#define SCOREP_PROFILE_ASSURE_INITIALIZED if ( !scorep_profile_is_initialized ) { return; }

/** @def SCOREP_PROFILE_STOP
    Disables further construction of the profile.
 */
#define SCOREP_PROFILE_STOP scorep_profile_is_initialized = false;

/** Global profile definition data */
typedef struct
{
    /** Points to the first root node. Further root nodes are added as siblings
        to a root node. */
    scorep_profile_node* first_root_node;

    /** Number of metrics in dense representation. All enter/exit events expect this
        number of metrics. */
    uint32_t num_of_dense_metrics;

    /** Array containing the Metric defintion handle for the metrics in dense
        representation. All enter/exit events expect the metrics in this order. */
    SCOREP_CounterHandle* dense_metrics;

    /** Maximum possible depth of the calltree */
    uint64_t max_callpath_depth;

    /** Maximum number of callpathes */
    uint64_t max_callpath_num;

    /** Maximum callpath depth actually reached during the run */
    uint64_t reached_depth;

    /** True if collapse nodes occur */
    bool has_collapse_node;
} scorep_profile_definition;

/**
   Global profile definition instance
 */
extern scorep_profile_definition scorep_profile;

/** Flag wether the profile is initialized */
extern bool scorep_profile_is_initialized;

/**
   Contains the basename for profile files.
 */
extern char* scorep_profile_basename;

/**
   Initializes the profile definition struct
 */
void
scorep_profile_init_definition( uint64_t              max_callpath_depth,
                                uint64_t              max_Callpath_num,
                                uint32_t              num_dense_metrics,
                                SCOREP_CounterHandle* metrics );

/**
   Resets the profile definition struct
 */
void
scorep_profile_delete_definition();

/**
   Returns the number of locations stored in the profile.
   @return number of locations stored in the profile.
 */
uint64_t
scorep_profile_get_number_of_threads();

/**
   Dumps the tree structure to the screen.
 */
void
scorep_profile_dump();

SCOREP_ParameterHandle scorep_profile_param_instance;

#endif // SCOREP_PROFILE_DEFINITION_H
