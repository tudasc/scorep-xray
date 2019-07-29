/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
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

/**
 * @file
 * Provides extended output on errors in the profile.
 *
 *
 *
 */

#include <config.h>
#include <scorep_profile_debug.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>

#include <scorep_ipc.h>
#include <scorep_runtime_management.h>
#include <SCOREP_Thread_Mgmt.h>
#include <UTILS_Error.h>
#include <SCOREP_Definitions.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/*----------------------------------------------------------------------------------------
   internal helper functions
   --------------------------------------------------------------------------------------*/
static void
scorep_dump_node( FILE* file, scorep_profile_node* node )
{
    static char* type_name_map[] =
    {
        "regular region",
        "parameter string",
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

    if ( node->node_type >= sizeof( type_name_map ) / sizeof( char* ) )
    {
        fprintf( file, "unknown type: %d", node->node_type );
        return;
    }

    fprintf( file, "type: %s", type_name_map[ node->node_type ] );
    if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
    {
        fprintf( file, " name: %s", SCOREP_RegionHandle_GetName( scorep_profile_type_get_region_handle( node->type_specific_data ) ) );
        scorep_profile_callpath_parameters_t* parameters = scorep_profile_type_get_ptr_value( node->type_specific_data );
        if ( parameters )
        {
            fprintf( file, " %u parameter(s):", parameters->number );
            for ( int i = 0; i < parameters->number; i++ )
            {
                if ( SCOREP_ParameterHandle_GetType( parameters->parameters[ i ].parameter_handle ) == SCOREP_PARAMETER_STRING )
                {
                    fprintf( file, " %s = %s", SCOREP_ParameterHandle_GetName( parameters->parameters[ i ].parameter_handle ), SCOREP_StringHandle_Get( parameters->parameters[ i ].parameter_value.string_handle ) );
                }
                else
                {
                    fprintf( file, " %s = %" PRIi64, SCOREP_ParameterHandle_GetName( parameters->parameters[ i ].parameter_handle ), parameters->parameters[ i ].parameter_value.integer_value );
                }
            }
        }
    }
    else if ( node->node_type == SCOREP_PROFILE_NODE_THREAD_START )
    {
        fprintf( file, " fork node: %p",
                 scorep_profile_type_get_fork_node( node->type_specific_data ) );
    }
    else if ( node->node_type == SCOREP_PROFILE_NODE_PARAMETER_INTEGER )
    {
        fprintf( file, " value: %" PRIi64,
                 scorep_profile_type_get_int_value( node->type_specific_data ) );
    }
    else if ( node->node_type == SCOREP_PROFILE_NODE_PARAMETER_STRING )
    {
        fprintf( file, " value: %s",
                 SCOREP_StringHandle_Get( scorep_profile_type_get_int_value( node->type_specific_data ) ) );
    }
}

static void
scorep_dump_stack( FILE* file, SCOREP_Profile_LocationData* location )
{
    if ( location == NULL )
    {
        return;
    }
    fprintf( file, "Current stack of failing thread:\n" );
    uint32_t i = 0;
    fprintf( file, "\n" );
    for ( scorep_profile_node* current = location->current_task_node;
          current != NULL;
          current = current->parent, i++ )
    {
        fprintf( file, "%u %p\t", i, current );
        scorep_dump_node( file, current );
        fprintf( file, "\n" );
    }
    fprintf( file, "\n" );
}


static void
scorep_dump_subtree(  FILE*                file,
                      scorep_profile_node* node,
                      uint32_t             level )
{
    if ( node == NULL )
    {
        return;
    }

    fprintf( file, "%p ", node );
    for ( int i = 0; i < level; i++ )
    {
        fprintf( file, "| " );
    }
    fprintf( file, "+ " );
    scorep_dump_node( file, node );
    fprintf( file, "\n" );
    if ( node->first_child != NULL )
    {
        scorep_dump_subtree( file, node->first_child, level + 1 );
    }
    if ( node->next_sibling != NULL )
    {
        scorep_dump_subtree( file, node->next_sibling, level );
    }
}

void
scorep_profile_dump( FILE* file, SCOREP_Profile_LocationData* location )
{
    fprintf( file, "\n" );
    if ( SCOREP_Thread_InParallel() )
    {
        if ( location != NULL && location->root_node != NULL )
        {
            fprintf( file, "Current status of failing profile:\n" );
            scorep_dump_subtree( file, location->root_node->first_child, 0 );
        }
    }
    else
    {
        fprintf( file, "Current state of the profile of all threads:\n" );
        scorep_dump_subtree( file, scorep_profile.first_root_node, 0 );
    }
    fprintf( file, "\n" );
}

/*----------------------------------------------------------------------------------------
   visible functions
   --------------------------------------------------------------------------------------*/

void
scorep_profile_on_error( SCOREP_Profile_LocationData* location )
{
    /* Disable further profiling */
    scorep_profile.is_initialized = false;

    /* If core files are enabled, write a core file */
    if ( scorep_profile_do_core_files() &&
         ( !SCOREP_Thread_InParallel() || location != NULL ) )
    {
        uint32_t    thread   = 0;
        const char* dirname  = SCOREP_GetExperimentDirName();
        const char* basename = scorep_profile_get_basename();
        char*       filename = NULL;

        filename = ( char* )malloc( strlen( dirname ) + /* Directory      */
                                    strlen( basename )  /* basename       */
                                    + 32 );             /* constant stuff */

        if ( filename == NULL )
        {
            return;
        }

        if ( location != NULL )
        {
            thread = SCOREP_Location_GetId( location->location_data );
        }

        sprintf( filename, "%s/%s.%d.%u.core", dirname, basename,
                 SCOREP_Ipc_GetRank(), thread );

        FILE* file = fopen( filename, "a" );

        if ( file == NULL )
        {
            return;
        }

        fprintf( file, "ERROR on rank %d, thread %u\n\n",
                 SCOREP_Ipc_GetRank(), thread );

        scorep_dump_stack( file, location );
        scorep_profile_dump( file, location );

        fclose( file );
        UTILS_FATAL( "Cannot continue profiling, profiling core file written to '%s'.",
                     filename );
        free( filename );
    }

    UTILS_FATAL( "Cannot continue profiling. Activating core files (export "
                 "SCOREP_PROFILING_ENABLE_CORE_FILES=1) might provide more insight." );
}
