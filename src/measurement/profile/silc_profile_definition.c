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

/**
 * @file silc_profile_definition.c
 * Implementation of the profile definition.
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status ALPHA
 *
 */

#include <stddef.h>
#include <stdio.h>

#include "SILC_Memory.h"

#include "silc_profile_definition.h"

/*----------------------------------------------------------------------------------------
   Global variables
   -------------------------------------------------------------------------------------*/

silc_profile_definition silc_profile;

bool                    silc_profile_is_initialized = false;

/** Flag wether an initialize is a reinitialize */
bool silc_profile_reinitialize = false;

/*----------------------------------------------------------------------------------------
   Constructors / destriuctors
   -------------------------------------------------------------------------------------*/

/** Initializes the profile definition struct
 */
void
silc_profile_init_definition( uint32_t            max_callpath_depth,
                              uint32_t            max_callpath_num,
                              uint32_t            num_dense_metrics,
                              SILC_CounterHandle* metrics )
{
    int i;
    /* On reinitialization of the profile during a phase, do not overwrite the pointer to
     * exiting root nodes.
     */
    if ( !silc_profile_reinitialize )
    {
        silc_profile.first_root_node = NULL;
    }
    silc_profile_reinitialize = true;

    /* Store configuration */
    silc_profile.max_callpath_depth   = max_callpath_depth;
    silc_profile.max_callpath_num     = max_callpath_num;
    silc_profile.num_of_dense_metrics = num_dense_metrics;
    silc_profile.dense_metrics
        = ( SILC_CounterHandle* )
          SILC_Memory_AllocForProfile( num_dense_metrics * sizeof( SILC_CounterHandle ) );

    for ( i = 0; i < num_dense_metrics; i++ )
    {
        silc_profile.dense_metrics[ i ] = metrics[ i ];
    }
}

/** Resets the profile definition struct
 */
void
silc_profile_delete_definition()
{
    /* Do not reset first_root_node, because in Periscope phases the list of root nodes
       stay alive.
     */

    silc_profile.num_of_dense_metrics = 0;
    if ( silc_profile.dense_metrics )
    {
        silc_profile.dense_metrics = NULL;
    }
}

/*----------------------------------------------------------------------------------------
   Debug
   -------------------------------------------------------------------------------------*/
void
silc_profile_dump_subtree( silc_profile_node* node,
                           uint32_t           level )
{
    static char* type_name_map[] = {
        "regular region",
        "paramater string",
        "parameter integer",
        "thread root",
        "thread start"
    };

    int          i;
    for ( i = 0; i < level; i++ )
    {
        printf( "| " );
    }
    printf( "+ type: %s\n", type_name_map[ node->node_type ] );

    if ( node == NULL )
    {
        return;
    }
    if ( node->first_child != NULL )
    {
        silc_profile_dump_subtree( node->first_child, level + 1 );
    }
    if ( node->next_sibling != NULL )
    {
        silc_profile_dump_subtree( node->next_sibling, level );
    }
}

void
silc_profile_dump()
{
    printf( "\n" );
    silc_profile_dump_subtree( silc_profile.first_root_node, 0 );
    printf( "\n" );
}
