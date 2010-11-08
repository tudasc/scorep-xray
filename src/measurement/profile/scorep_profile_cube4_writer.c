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

/**
 * @file scorep_profile_cube4_writer.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 * Implements a profile writer in Cube 4 format.
 */

#include <config.h>
#include <sys/stat.h>

#include "SCOREP_Memory.h"
#include "scorep_utility/SCOREP_Utils.h"
#include "SCOREP_Definitions.h"

#include "scorep_profile_definition.h"
#include "scorep_definition_cube4.h"
#include "scorep_mpi.h"
#include "scorep_runtime_management.h"

#include "cubew_cube.h"
#include "cubew_cubew.h"
#include "cubew_services.h"

/**
   Defines a function type which returns a metric value from a given node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @returns the metric value of @a node.
 */
typedef uint64_t ( *scorep_profile_get_value_func )( scorep_profile_node* node );

/**
   Contains a mapping structure between scorep handles and cube handles.
 */
static scorep_cube4_definitions_map* scorep_map;

/* *****************************************************************************
   Internal helper functions
*******************************************************************************/

/**
   Creates a mapping from cube id numbers (cube_cnode->id) to scorep_profile_node
   instances. It is a processing function for a scorep_profile_for_all call.
   The files which is filled is given as @a param. In order to find a mapping
   from the callpath handle stored in each profile node  to cube handles, it
   uses the static global @ref scorep_map.
   @param node  Pointer to the current porfile node.
   @param param Pointer to a field of profile nodes. The index of the entry
                which corresponds to the current node is: cube_cnode_id.
 */
static void
scorep_cube4_make_mapping( scorep_profile_node* node,
                           void*                param )
{
    cube_cnode*           cnode     = NULL;
    scorep_profile_node** id_2_node = ( scorep_profile_node** )param;

    if ( node->callpath_handle != SCOREP_INVALID_REGION )
    {
        cnode = scorep_get_cube4_callpath( scorep_map, node->callpath_handle );
        if ( cnode != NULL )
        {
            id_2_node[ cube_cnode_get_id( cnode ) ] = node;
        }
    }
}

/**
   Returns the sum of implicit runtime for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @returns the implicit runtime of @a node.
 */
uint64_t
scorep_profile_get_time_value( scorep_profile_node* node )
{
    return node->inclusive_time.sum;
}

/**
   Returns the number of visits for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @returns the number of visits of @a node.
 */
uint64_t
scorep_profile_get_visits_value( scorep_profile_node* node )
{
    return node->count;
}

/**
   Writes data for the metric @a metric to a cube object.
   @param my_cube         Pointer to a cube object to which the data is written.
   @param cube_writer     Pointer to a writer for the @a my_cube.
   @param metric          The cube metric handle for the written metric.
   @param id_to_node      A field which maps cube cnode ids (cube_cnode->id)
                          to callpathes. Hereby, each location has an array
                          of @a callpath_number length. The index within the
                          array matches the cube_cnode id. The value is a
                          pointer to the profile node which represents the
                          corresponding callpath to the cnode in the given
                          location.
   @param map             Mapping structure to map between scorep and cube handles
   @param callpath_number The number of callpathes in the calltree
   @param location_number The number of locations.
   @param get_value       Functionpointer which returns the value for a given
                          profile node.
 */
static void
scorep_profile_write_cube_metric( cube_t*                       my_cube,
                                  cube_writer*                  cube_writer,
                                  cube_metric*                  metric,
                                  scorep_profile_node**         id_2_node,
                                  scorep_cube4_definitions_map* map,
                                  uint64_t                      callpath_number,
                                  uint64_t                      location_number,
                                  scorep_profile_get_value_func get_value )
{
    scorep_profile_node* node  = NULL;
    cube_cnode*          cnode = NULL;

    /* Array of all values for one metric for one callpath for all locations */
    uint64_t* values     = ( uint64_t* )malloc( location_number * sizeof( uint64_t ) );
    char*     bit_vector = ( char* )malloc( ( callpath_number + 7 ) / 8 );
    memset( bit_vector, 0xFF, ( callpath_number + 7 ) / 8 );

    /* Initialize writing of a new metric */
    cubew_reset( cube_writer );
    cubew_set_array( cube_writer, callpath_number );
    cube_metric_set_known_cnodes( metric, bit_vector );

    /* Iterate over all cube cnodes */
    for (; cubew_is_valid_id( cube_writer ) == CUBE_TRUE; cubew_step( cube_writer ) )
    {
        /* Gather data from locations */
        int id = cubew_get_next_callnode_id( cube_writer );
        for ( uint64_t i = 0; i < location_number; i++ )
        {
            node = id_2_node[ i * callpath_number + id ];
            if ( node != NULL )
            {
                values[ i ] = get_value( node );
            }
            else
            {
                values[ i ] = 0;
            }
        }

        /* Write data for one callpath */
        if ( cubew_do_i_write( cube_writer ) )
        {
            /* Assume that the first location contains all callpathes that
               appear in this process. */
            cnode = scorep_get_cube4_callpath( map, id_2_node[ id ]->callpath_handle );
            cube_write_sev_row_of_uint64( my_cube, metric, cnode, values );
        }
    }

    /* Clean up */
    free( values );

    /* bit_vector is currently freed by the cube object.
       However, I think good practice would be that the component which
       allocate memory frees it. */
    //free( bit_vector );
}

/**
   Writes profile data to a cube object.
   @param my_cube      Pointer to a cube object to which the data is written.
   @param cube_writer  Pointer to a writer for the @a my_cube.
   @param map          Mapping structure to map between scorep and cube handles.
 */
static void
scorep_profile_write_data_to_cube4( cube_t*                       my_cube,
                                    cube_writer*                  cube_writer,
                                    scorep_cube4_definitions_map* map )
{
    uint64_t              callpath_number = scorep_cube4_get_number_of_callpathes( map );
    uint64_t              location_number = scorep_profile_get_number_of_threads();
    scorep_profile_node*  node            = scorep_profile.first_root_node;
    cube_metric*          metric          = NULL;
    scorep_profile_node** id_2_node       = ( scorep_profile_node** )
                                            calloc( callpath_number * location_number, sizeof( scorep_profile_node* ) );

    /* Generate Mapping from callpath id to profile node */
    scorep_map = map;
    for ( uint64_t i = 0; node != NULL; node = node->next_sibling )
    {
        scorep_profile_for_all( node,
                                scorep_cube4_make_mapping,
                                &id_2_node[ i ] );

        i += callpath_number;
    }
    scorep_map = NULL;

    /* Write implicit time */
    metric = scorep_get_cube4_metric( map, ( SCOREP_CounterHandle )1 );
    scorep_profile_write_cube_metric( my_cube, cube_writer, metric, id_2_node, map,
                                      callpath_number, location_number,
                                      &scorep_profile_get_time_value );


    /* Write visits */
    metric = scorep_get_cube4_metric( map, ( SCOREP_CounterHandle )2 );
    scorep_profile_write_cube_metric( my_cube, cube_writer, metric, id_2_node, map,
                                      callpath_number, location_number,
                                      &scorep_profile_get_visits_value );


    /* Clean up */
    free( id_2_node );
}

/* *****************************************************************************
   Main writer function
*******************************************************************************/
void
scorep_profile_write_cube4()
{
    uint64_t                      number_of_writers = 1; /* Initially one writer, in MPI case it can
                                                            be more */
    uint64_t                      number_of_threads = scorep_profile_get_number_of_threads();
    uint64_t                      my_rank           = SCOREP_Mpi_GetRank();
    cube_t*                       my_cube           = NULL; /* The cube structure */
    cube_writer*                  cube_writer       = NULL; /* The cube writer */
    char*                         filename          = NULL; /* Contains the basename for the cube file */
    char*                         dirname           = SCOREP_GetExperimentDirName();
    scorep_cube4_definitions_map* map               = NULL;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                         "Writing profile in Cube 4 format ..." );

    /* Create definition mapping tables */
    map = scorep_cube4_create_definitions_map();
    if ( map == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                      "Failed to allocate ememory for defintion mapping\n"
                      "Failed to write Cube 4 profile" );
        return;
    }

    /* Allocate memory for full filename */
    filename = ( char* )malloc( strlen( dirname ) +               /* Directory     */
                                1 +                               /* separator '/' */
                                strlen( scorep_profile_basename ) /* basename      */
                                + 1 );                            /* trailing '\0' */
    if ( filename == NULL )
    {
        SCOREP_ERROR_POSIX( "Failed to allocate memory for filename.\n"
                            "Failed to write Cube 4 profile" );
        goto cleanup;
    }

    /* Create full filename */
    sprintf( filename, "%s/%s", dirname, scorep_profile_basename );

    /* Create writer object. FALSE -> no zlib compression */
    cube_writer
        = cubew_create( my_rank,           /* rank of this node */
                        number_of_threads, /* sum of all threads of all nodes */
                        number_of_writers, /* number of parallel writers */
                        filename,          /* base file name */
                        CUBE_FALSE );      /* zlib compression */

    /* Retrieve the cube object to which all data is written */
    my_cube = cubew_get_cube( cube_writer );

    /* generate header */
    cube_def_mirror( my_cube, "http://icl.cs.utk.edu/software/kojak/" );
    cube_def_mirror( my_cube, "http://www.fz-juelich.de/jsc/kojak/" );
    cube_def_attr( my_cube, "description", "need a description" );
    cube_def_attr( my_cube, "experiment time", "need a date" );

    /* Write definitions to cube */
    scorep_write_definitions_to_cube4( my_cube, map );

    /* Write data to cube */
    scorep_profile_write_data_to_cube4( my_cube, cube_writer, map );

    /* Clean up */
cleanup:
    if ( cube_writer )
    {
        cubew_finalize( cube_writer );
    }
    if ( map )
    {
        scorep_cube4_delete_definitions_map( map );
    }
    if ( filename )
    {
        free( filename );
    }
}
