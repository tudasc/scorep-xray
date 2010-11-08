/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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

#include "scorep_profile_definition.h"

/*----------------------------------------------------------------------------------------
   Global variables
   -------------------------------------------------------------------------------------*/

scorep_profile_definition scorep_profile;

bool                      scorep_profile_is_initialized = false;

/** Flag wether an initialize is a reinitialize */
bool  scorep_profile_reinitialize = false;

char* scorep_profile_basename = NULL;

/*----------------------------------------------------------------------------------------
   Constructors / destriuctors
   -------------------------------------------------------------------------------------*/

/** Initializes the profile definition struct
 */
void
scorep_profile_init_definition( uint64_t              max_callpath_depth,
                                uint64_t              max_callpath_num,
                                uint32_t              num_dense_metrics,
                                SCOREP_CounterHandle* metrics )
{
    int i;
    /* On reinitialization of the profile during a phase, do not overwrite the pointer to
     * exiting root nodes.
     */
    if ( !scorep_profile_reinitialize )
    {
        scorep_profile.first_root_node = NULL;
    }
    scorep_profile_reinitialize = true;

    /* Store configuration */
    scorep_profile.max_callpath_depth   = max_callpath_depth;
    scorep_profile.max_callpath_num     = max_callpath_num;
    scorep_profile.num_of_dense_metrics = num_dense_metrics;
    scorep_profile.dense_metrics
        = ( SCOREP_CounterHandle* )
          SCOREP_Memory_AllocForProfile( num_dense_metrics * sizeof( SCOREP_CounterHandle ) );

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
        "thread start"
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
    printf( "+ type: %s", type_name_map[ node->node_type ] );
    if ( node->node_type == scorep_profile_node_regular_region )
    {
        printf( "  name: %s", SCOREP_Region_GetName( SCOREP_PROFILE_DATA2REGION( node->type_specific_data ) ) );
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
