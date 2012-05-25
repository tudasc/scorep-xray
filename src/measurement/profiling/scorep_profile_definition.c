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

/**
 * @file scorep_profile_definition.c
 * Implementation of the profile definition.
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <stddef.h>
#include <stdio.h>

#include "SCOREP_Memory.h"
#include "SCOREP_Definitions.h"
#include <SCOREP_Config.h>

#include "scorep_profile_definition.h"
#include "scorep_definitions.h"

/*----------------------------------------------------------------------------------------
   Global variables
   -------------------------------------------------------------------------------------*/

/**
    Global profile definition instance
 */
scorep_profile_definition scorep_profile =
{
    NULL,       /* first_root_node      */
    0,          /* num_of_dense_metrics */
    NULL,       /* dense_metrics        */
    false,      /* has_collapse_node    */
    0,          /* reached_depth        */
    false,      /* is_initialized       */
    false,      /* reinitialize         */
    0,          /* max_callpath_depth   */
    UINT64_MAX, /* max_callpath_num     */
    NULL,       /* basename             */
    0,          /* task_table_size      */
    0           /* output_format        */
};

/* ---------------------------------- Configuration variables */

#define SCOREP_Profile_OutputDefault SCOREP_Profile_OutputCube4

/**
   Bitset table for output format string configuration.
 */
static const SCOREP_ConfigType_SetEntry scorep_profile_format_table[] = {
    { "NONE",         SCOREP_Profile_OutputNone         },
    { "TAU_SNAPSHOT", SCOREP_Profile_OutputTauSnapshot  },
    { "CUBE4",        SCOREP_Profile_OutputCube4        },
    { "DEFAULT",      SCOREP_Profile_OutputDefault      },
    { NULL,           0                                 }
};

/**
   Configuration variable registration structures for the profiling system.
 */
SCOREP_ConfigVariable scorep_profile_configs[] = {
    { "task_table_size",
      SCOREP_CONFIG_TYPE_NUMBER,
      &scorep_profile.task_table_size,
      NULL,
      "64",
      "Size of the task tracing table",
      "Size of the task tracing table. Best performance is obtained, if the table size "
      "matches the number of tasks that are active at once. If yout program does not "
      "use tasks, you may set this value to zero, to save some memory" },

    { "max_callpath_depth",
      SCOREP_CONFIG_TYPE_NUMBER,
      &scorep_profile.max_callpath_depth,
      NULL,
      "30",
      "Maximum depth of the calltree",
      "Maximum depth of the calltree" },

    /*{ "max_callpath_num",
       SCOREP_CONFIG_TYPE_NUMBER,
       &scorep_profile.max_callpath_num,
       NULL,
       "1000000000",
       "Maximum number of nodes in the calltree",
       "Maximum number of nodes in the calltree" },*/

    { "base_name",
      SCOREP_CONFIG_TYPE_STRING,
      &scorep_profile.basename,
      NULL,
      "profile",
      "Base for construction of the profile filename",
      "String which is used as based to create the filenames for the profile files" },

    { "format",
      SCOREP_CONFIG_TYPE_BITSET,
      &scorep_profile.output_format,
      ( void* )scorep_profile_format_table,
      "DEFAULT",
      "Profile output format",
      "Sets the output format for the profile.\n"
      "The following formats are supported:\n"
      " NONE: No profile output. This does not disable profile recording.\n"
      " TAU_SNAPSHOT: Tau snapshot format.\n"
      " CUBE4: Cube4 format.\n"
      " DEFAULT: Default format. If Cube4 is supported, Cube4 is the default\n"
      "          else the Tau snapshot format is default.\n", },
    SCOREP_CONFIG_TERMINATOR
};

/*----------------------------------------------------------------------------------------
   Constructors / destriuctors
   -------------------------------------------------------------------------------------*/

/** Initializes the profile definition struct
 */
void
scorep_profile_init_definition( uint32_t             num_dense_metrics,
                                SCOREP_MetricHandle* metrics )
{
    int i;
    /* On reinitialization of the profile during a phase, do not overwrite the pointer to
     * exiting root nodes.
     */

    scorep_profile.reinitialize   = true;
    scorep_profile.is_initialized = true;

    /* Store configuration */
    scorep_profile.has_collapse_node    = false;
    scorep_profile.reached_depth        = 0;
    scorep_profile.num_of_dense_metrics = num_dense_metrics;
    scorep_profile.dense_metrics        = ( SCOREP_MetricHandle* )SCOREP_Memory_AllocForProfile( num_dense_metrics * sizeof( SCOREP_MetricHandle ) );

    for ( i = 0; i < num_dense_metrics; i++ )
    {
        scorep_profile.dense_metrics[ i ] = metrics[ i ];
    }
}

/** Resets the profile definition struct
 */
void
scorep_profile_delete_definition()
{
    scorep_profile.is_initialized = false;

    /* Do not reset first_root_node, because in Periscope phases the list of root nodes
       stay alive.
     */

    scorep_profile.num_of_dense_metrics = 0;
    if ( scorep_profile.dense_metrics )
    {
        scorep_profile.dense_metrics = NULL;
    }
}

/*----------------------------------------------------------------------------------------
   Other functions
   -------------------------------------------------------------------------------------*/

uint64_t
scorep_profile_get_number_of_threads()
{
    uint64_t             no_of_threads = 0;
    scorep_profile_node* current       = scorep_profile.first_root_node;

    while ( current != NULL )
    {
        if ( current->node_type == scorep_profile_node_thread_root )
        {
            no_of_threads++;
        }
        current = current->next_sibling;
    }

    return no_of_threads;
}

/*----------------------------------------------------------------------------------------
   Debug
   -------------------------------------------------------------------------------------*/
void
scorep_profile_dump_subtree( scorep_profile_node* node,
                             uint32_t             level )
{
    static char* type_name_map[] = {
        "regular region",
        "paramater string",
        "parameter integer",
        "thread root",
        "thread start",
        "collapse",
        "task root"
    };

    if ( node == NULL )
    {
        return;
    }

    printf( "%p ", node );
    for ( int i = 0; i < level; i++ )
    {
        printf( "| " );
    }
    printf( "+ type: %s; count: %llu; time: %llu", type_name_map[ node->node_type ], node->count, node->inclusive_time.sum );
    if ( node->node_type == scorep_profile_node_regular_region )
    {
        printf( "  name: %s, metrics: ", SCOREP_Region_GetName( scorep_profile_type_get_region_handle( node->type_specific_data ) ) );
        int i;
        for ( i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
        {
            printf( "%llu, ", node->dense_metrics[ i ].sum );
        }
    }
    printf( "\n" );
    if ( node->first_child != NULL )
    {
        scorep_profile_dump_subtree( node->first_child, level + 1 );
    }
    if ( node->next_sibling != NULL )
    {
        scorep_profile_dump_subtree( node->next_sibling, level );
    }
}

void
scorep_profile_dump()
{
    printf( "\n" );
    scorep_profile_dump_subtree( scorep_profile.first_root_node, 0 );
    printf( "\n" );
}
