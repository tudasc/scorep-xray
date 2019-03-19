/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
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

#ifndef SCOREP_PROFILE_DEFINITION_H
#define SCOREP_PROFILE_DEFINITION_H

/**
 * @file
 *
 * @brief The global profile definition struct and functions
 *
 */

#include <stdint.h>

#include <scorep_profile_node.h>
#include <scorep_profile_debug.h>

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
#define SCOREP_PROFILE_STOP( location )       \
    do {                                      \
        scorep_profile_on_error( location );  \
    } while ( 0 );

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
} scorep_profile_definition;

/* **************************************************************************************
   Global variables
****************************************************************************************/

/**
    Global profile definition instance
 */
extern scorep_profile_definition scorep_profile;


extern uint32_t scorep_profile_number_of_program_args;


/* **************************************************************************************
   Functions
****************************************************************************************/

/**
   Initializes the profile definition struct
 */
void
scorep_profile_init_definition( void );

/**
   Resets the profile definition struct
 */
void
scorep_profile_delete_definition( void );

/**
   Returns the configuration value for SCOREP_PROFILE_TASK_EXCHANGE_NUM.
 */
uint64_t
scorep_profile_get_task_exchange_num( void );

/**
   Returns the configuration value for SCOREP_PROFILE_BASENAME.
 */
const char*
scorep_profile_get_basename( void );

/**
   Returns the configuration value for SCOREP_PROFILE_OUTPUT_FORMAT.
 */
uint64_t
scorep_profile_get_output_format( void );

/**
   Returns the configuration value for SCOREP_PROFILE_CLUSTER_COUNT.
 */
uint64_t
scorep_profile_get_cluster_count( void );

/**
   Returns the configuration value for SCOREP_PROFILE_CLUSTER_MODE.
 */
SCOREP_Profile_ClusterMode
scorep_profile_get_cluster_mode( void );

/**
   Returns the configuration value for SCOREP_PROFILE_CLUSTERED_REGION.
 */
const char*
scorep_profile_get_clustered_region( void );

/**
   Returns the configuration value for SCORE_PROFILE_ENABLE_CLUSTERING.
 */
bool
scorep_profile_do_clustering( void );

/**
   Returns the configuration value for SCORE_PROFILE_ENABLE_CORE_FILES.
 */
bool
scorep_profile_do_core_files( void );


/**
   Returns the number of locations stored in the profile.
   @return number of locations stored in the profile.
 */
uint64_t
scorep_profile_get_number_of_threads( void );


extern SCOREP_ParameterHandle scorep_profile_param_instance;

#endif /* SCOREP_PROFILE_DEFINITION_H */
