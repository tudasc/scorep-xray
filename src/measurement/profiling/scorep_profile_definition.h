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

/* **************************************************************************************
   Defines
****************************************************************************************/

/**
   @def  SCOREP_PROFILE_ASSURE_INITIALIZED
         Check wether the profiling system is initialized.
 */
#define SCOREP_PROFILE_ASSURE_INITIALIZED if ( !scorep_profile.is_initialized ) { return; }

/**
   @def SCOREP_PROFILE_STOP
   Disables further construction of the profile.
 */
#define SCOREP_PROFILE_STOP scorep_profile.is_initialized = false;

/* **************************************************************************************
   Typedefs
****************************************************************************************/

/**
   Global profile definition data
 */
typedef struct
{
    /*--------------------- Profile data */

    /**
       Points to the first root node. Further root nodes are added as siblings
       to a root node.
     */
    scorep_profile_node* first_root_node;

    /**
       True if collapse nodes occur
     */
    bool has_collapse_node;

    /**
       Maximum callpath depth actually reached during the run
     */
    uint64_t reached_depth;

    /**
       Flag wether the profile is initialized
     */
    bool is_initialized;

    /**
       Flag wether an initialize is a reinitialize
     */
    bool reinitialize;

    /*--------------------- Configuration data */

/**
   Allows to limit the depth of the calltree. If the current
   callpath becomes longer than specified by this parameter,
   no further child nodes for this callpath are created.
   This limit allows a reduction of the number of callpathes,
   especially, if the application contains recursive function
   calls.
 */
    uint64_t max_callpath_depth;

/**
   Allows to limit the number of nodes in the calltree. If the
   number of nodes in the calltree reaches its limit, no further
   callpathes are created. All new callpathes are collapsed into
   a single node. This parameter allows to limit the memory
   usage of the profile.
 */
    uint64_t max_callpath_num;

/**
   Contains the basename for profile files.
 */
    char* basename;

/**
   Stores the configuration of the hash table size.
 */
    uint64_t task_table_size;

/**
   Stores the configuration setting for output format.
 */
    uint64_t output_format;
} scorep_profile_definition;

/* **************************************************************************************
   Global variables
****************************************************************************************/

/**
    Global profile definition instance
 */
extern scorep_profile_definition scorep_profile;


/**
   Configuration variable registration structures for the profiling system.
 */
extern SCOREP_ConfigVariable scorep_profile_configs[];


/* **************************************************************************************
   Functions
****************************************************************************************/

/**
   Initializes the profile definition struct
 */
void
scorep_profile_init_definition();

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
