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

/**
 * @file scorep_profile_cube4_writer_mpi.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 * Implements a profile writer in Cube 4 format for mpi applications.
 */

#include <config.h>
#include <sys/stat.h>
#include <mpi.h>

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

/* *****************************************************************************
   Internal helper functions
*******************************************************************************/

/**
   Creates a mapping from global sequence numbers to scorep_profile_node
   instances. It is a processing function for a scorep_profile_for_all call.
   The files which is filled is given as @a param.
   @param node  Pointer to the current porfile node.
   @param param Pointer to a field of profile nodes. The index of the entry
                which corresponds to the current node is the sequence number of
                the unified definitions.
 */
static void
scorep_cube4_make_mapping( scorep_profile_node* node,
                           void*                param )
{
    uint64_t              index     = SCOREP_Callpath_GetUnifiedSequenceNumber( node->callpath_handle );
    scorep_profile_node** id_2_node = ( scorep_profile_node** )param;

    id_2_node[ index ] = node;
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
   @param id_to_node      A field which maps unified sequence numbers
                          to callpathes. Hereby, each location has an array
                          of @a callpath_number length. The index within the
                          array matches the sequence number. The value is a
                          pointer to the profile node which represents the
                          corresponding callpath in the location.
   @param map             Mapping structure to map between scorep and cube handles
   @param callpath_number The number of callpathes in the unified definitions.
   @param global_threads  The number of threads in the unified defintions
   @param local_threads   The number of locations in this process.
   @param offset          Position in the global value vector where this process writes.
   @param recvcnts        Array with the number of threads for every process. Only needed
                          on rank 0. Ignored on other ranks.
   @param displs          Array with offsets for every process. Only needed on rank 0.
                          Ignored on other ranks
   @param get_value       Functionpointer which returns the value for a given
                          profile node.
 */
static void
scorep_profile_write_cube_metric( cube_t*                       my_cube,
                                  cube_writer*                  cube_writer,
                                  cube_metric*                  metric,
                                  scorep_profile_node**         id_2_node,
                                  scorep_cube4_definitions_map* map,
                                  int32_t                       callpath_number,
                                  int32_t                       global_threads,
                                  int32_t                       local_threads,
                                  int32_t                       offset,
                                  int32_t*                      recvcnts,
                                  int32_t*                      displs,
                                  scorep_profile_get_value_func get_value )
{
    scorep_profile_node* node          = NULL;
    cube_cnode*          cnode         = NULL;
    uint64_t*            local_values  = NULL;
    uint64_t*            global_values = NULL;
    char*                bit_vector    = NULL;
    int                  my_rank       = SCOREP_Mpi_GetRank();

    local_values = ( uint64_t* )malloc( local_threads * sizeof( uint64_t ) );

    if ( my_rank == 0 )
    {
        /* Array of all values for one metric for one callpath for all locations */
        global_values = ( uint64_t* )malloc( global_threads * sizeof( uint64_t ) );
        bit_vector    = ( char* )malloc( ( callpath_number + 7 ) / 8 );
        memset( bit_vector, 0xFF, ( callpath_number + 7 ) / 8 );

        /* Initialize writing of a new metric */
        cubew_reset( cube_writer );
        cubew_set_array( cube_writer, callpath_number );
        cube_metric_set_known_cnodes( metric, bit_vector );
    }

    /* Iterate over all unified callpathes */
    for ( uint64_t cp_index = 0; cp_index < callpath_number; cp_index++ )
    {
        for ( uint64_t thread = 0; thread < local_threads; thread++ )
        {
            node = id_2_node[ thread * callpath_number + cp_index ];
            if ( node != NULL )
            {
                local_values[ i ] = get_value( node );
            }
            else
            {
                local_values[ i ] = 0;
            }
        }

        /* Collect data from all processes */
        PMPI_Gatherv( local_values, local_threads, MPI_INT64, global_values, recvcnts,
                      displs, MPI_INT, 0, MPI_COMM_WORLD );

        /* Write data for one callpath */
        if ( my_rank == 0 )
        {
            /* Assume that the first location contains all callpathes that
               appear in this process. */
            cnode = scorep_get_cube4_callpath( map, SCOREP_Callpath_GetUnifiedHandle( id_2_node[ id ]->callpath_handle ) );
            cube_write_sev_row_of_uint64( my_cube, metric, cnode, values );
        }
    }

    /* Clean up */
    free( global_values );
    free( local_values );

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
    /*-------------------------------- Variable definition

       /* Number of this rank in MPI_COMM_WORLD */
    int32_t my_rank = SCOREP_Mpi_GetRank();

    /* Number of ranks in MPI_COMM_WORLD */
    int32_t ranks_number = SCOREP_Mpi_GetCommWorldSize();

    /* Number of threads in this rank */
    int32_t local_threads = scorep_profile_get_number_of_threads();

    /* Sum of all threads in all processes */
    int32_t global_threads = 0;

    /* Number of callpathes in the unified definitions */
    int32_t callpath_number = 0;

    /* Offset for values of this process in the value vector */
    int32_t offset = 0;

    /* List of numbers of locations on each process. Only used on rank 0 */
    int32_t* recvcnts = NULL;

    /* List of offsets of every process in the value vector. Only used on rank 0 */
    int32_t* displs = NULL;

    /* Mapping from global sequence number to local profile node of every thread */
    scorep_profile_node** id_2_node = NULL;

    /* Pointer to Cube 4 metric definition. Only used on Rank 0 */
    cube_metric* metric = NULL;

    /* -------------------------------- Initialization

       /* Perform initilazation for on rank 0 */
    if ( my_rank == 0 )
    {
        /* Get number of callpathes in unified definitions */
        callpath_number = SCOREP_Callpath_GetNumberOfUnifiedDefinitions();

        /* Create vectors with offsets and recvcounts on rank 0 */
        recvcnts = ( int32_t* )malloc( ranks_number * sizeof( int32_t ) );
        displs   = ( int32_t* )malloc( ranks_number * sizeof( int32_t ) );
    }

    /* Distribute number of callpathes in unified definitions */
    PMPI_Bcast( &callpath_number, 1, MPI_INT, 0, MPI_COMM_WORLD );

    /* Get sum of locations of all processes */
    PMPI_Allreduce( &local_threads, &global_threads, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD );

    /* Calculate offset of this thread in the value vector */
    PMPI_Exscan( &local_threads, &offset, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD );

    /* Collect number of threads from every rank to rank 0. */
    PMPI_Gather( &local_threads, 1, MPI_INT, recvcnts, 1, MPI_INT, 0, MPI_COMM_WORLD );

    /* Collect number of offsets from every rank to rank 0. */
    PMPI_Gather( &offset, 1, MPI_INT, displs, 1, MPI_INT, 0, MPI_COMM_WORLD );

    /* Build mapping from sequence number in unified callpath definitions to
       profile nodes */
    id_2_node = calloc( callpath_number * local_threads, sizeof( scorep_profile_node* ) );

    scorep_profile_node* node = scorep_profile.first_root_node;
    for ( uint64_t i = 0; node != NULL; node = node->next_sibling )
    {
        scorep_profile_for_all( node,
                                scorep_cube4_make_mapping,
                                &id_2_node[ i ] );
        i += callpath_number;
    }

    /* Write implicit time */
    if ( my_rank == 0 )
    {
        metric = scorep_get_cube4_metric( map, ( SCOREP_CounterHandle )1 );
    }
    scorep_profile_write_cube_metric( my_cube, cube_writer, metric, id_2_node, map,
                                      callpath_number, global_threads, local_threads,
                                      offset, recvcnts, displs,
                                      &scorep_profile_get_time_value );


    /* Write visits */
    if ( my_rank == 0 )
    {
        metric = scorep_get_cube4_metric( map, ( SCOREP_CounterHandle )2 );
    }
    scorep_profile_write_cube_metric( my_cube, cube_writer, metric, id_2_node, map,
                                      callpath_number, global_threads, local_threads,
                                      offset, recvcnts, displs,
                                      &scorep_profile_get_visits_value );


    /* Clean up */
    free( id_2_node );
    free( displs );
    free( recvcnts );
}

/* *****************************************************************************
   Main writer function
*******************************************************************************/
void
scorep_profile_write_cube4()
{
    int32_t      number_of_writers = 1;                  /* Initially one writer, in MPI
                                                            case it can be more */
    int32_t      number_of_threads = scorep_profile_get_number_of_threads();
    int32_t      my_rank           = SCOREP_Mpi_GetRank();
    cube_t*      my_cube           = NULL;                  /* The cube structure */
    cube_writer* cube_writer       = NULL;                  /* The cube writer */

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                         "Writing profile in Cube 4 format ..." );

    /* Create definition mapping tables */
    map = scorep_cube4_create_definitions_map();
    if ( map == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                      "Failed to allocat ememory for defintion mapping\n"
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

    /* Create writer object on rank 0 only */
    if ( my_rank == 0 )
    {
        cube_writer
            = cubew_create( my_rank,           /* rank of this node */
                            number_of_threads, /* sum of all threads of all nodes */
                            number_of_writers, /* number of parallel writers */
                            filename,          /* base file name */
                            CUBE_FALSE );      /* no zlib compression */

        /* Retrieve the cube object to which all data is written */
        my_cube = cubew_get_cube( cube_writer );

        /* generate header */
        cube_def_mirror( my_cube, "http://icl.cs.utk.edu/software/kojak/" );
        cube_def_mirror( my_cube, "http://www.fz-juelich.de/jsc/kojak/" );
        cube_def_attr( my_cube, "description", "need a description" );
        cube_def_attr( my_cube, "experiment time", "need a date" );

        /* Write definitions to cube */
        scorep_write_definitions_to_cube4( my_cube, map );
    }

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
