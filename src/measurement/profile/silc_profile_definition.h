/*
 * This file is part of the SILC project (http://www.silc.de)
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

#ifndef SILC_PROFILE_DEFINITION_H
#define SILC_PROFILE_DEFINITION_H

/**
 * @file        silc_profile_definition.h
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief The global profile definition struct and functions
 *
 */

#include <stdint.h>

#include "silc_profile_node.h"

/** @def  SILC_PROFILE_ASSURE_INITIALIZED
          Check wether the profiling system is initialized.
 */
#define SILC_PROFILE_ASSURE_INITIALIZED if ( !silc_profile_is_initialized ) { return; }

/** @def SILC_PROFILE_STOP
    Disables further construction of the profile.
 */
#define SILC_PROFILE_STOP silc_profile_is_initialized = false;

/** Global profile definition data */
typedef struct
{
    /** Points to the first root node. Further root nodes are added as siblings
        to a root node. */
    silc_profile_node* first_root_node;

    /** Number of metrics in dense representation. All enter/exit events expect this
        number of metrics. */
    uint32_t num_of_dense_metrics;

    /** Array containing the Metric defintion handle for the metrics in dense
        representation. All enter/exit events expect the metrics in this order. */
    SILC_CounterHandle* dense_metrics;

    /** Maximum possible depth of the calltree */
    uint32_t max_callpath_depth;

    /** Maximum number of callpathes */
    uint32_t max_callpath_num;
} silc_profile_definition;

/** Global profile definition instance */
extern silc_profile_definition silc_profile;

/** Flag wether the profile is initialized */
extern bool silc_profile_is_initialized;

/** Initializes the profile definition struct
 */
void
silc_profile_init_definition( uint32_t            max_callpath_depth,
                              uint32_t            max_Callpath_num,
                              uint32_t            num_dense_metrics,
                              SILC_CounterHandle* metrics );

/** Resets the profile definition struct
 */
void
silc_profile_delete_definition();

/**
   Dumps the tree structure to the screen.
 */
void
silc_profile_dump();

#endif // SILC_PROFILE_DEFINITION_H
