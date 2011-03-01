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
 * @file scorep_profile_cube4_writer_mpi.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 * Implements a profile writer in Cube 4 format for mpi applications.
 */

#include <config.h>
#include <sys/stat.h>

#include "SCOREP_Memory.h"
#include "scorep_utility/SCOREP_Utils.h"
#include "SCOREP_Definitions.h"
#include "SCOREP_Timing.h"

#include "scorep_profile_definition.h"
#include "scorep_definition_cube4.h"
#include "scorep_mpi.h"
#include "scorep_runtime_management.h"

#include "cubew_cube.h"
#include "cubew_cubew.h"
#include "cubew_services.h"

/* *****************************************************************************
   Typedefs and variable declarations
*******************************************************************************/

/**
   Defines a function type which returns a metric value from a given node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @returns the metric value of @a node.
 */
typedef uint64_t ( *scorep_profile_get_uint64_func )( scorep_profile_node* node );

/**
   Defines a function type which returns a metric value from a given node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @returns the metric value of @a node.
 */
typedef double ( *scorep_profile_get_doubles_func )( scorep_profile_node* node );

/**
   Contains a mapping structure between scorep handles and cube handles.
 */
static scorep_cube4_definitions_map* scorep_map;

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
    /* Check whether this node has a callpath handle assigned. Thread nodes have none */
    if ( !node->callpath_handle )
    {
        return;
    }

    /* Use the sequence number of the unified definitions as index. */
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
static double
scorep_profile_get_time_value( scorep_profile_node* node )
{
    return ( ( double )node->inclusive_time.sum ) / ( ( double )SCOREP_GetClockResolution() );
}



/**
   Returns the number of visits for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @returns the number of visits of @a node.
 */
static uint64_t
scorep_profile_get_visits_value( scorep_profile_node* node )
{
    return node->count;
}

/* *INDENT-OFF* */

/**
   @def SCOREP_PROFILE_RESET_CUBE
   Code to reset the cube struct in data writing. Used to reduce code replication.
 */
#define SCOREP_PROFILE_WRITE_CUBE_METRIC( type, TYPE, cube_type )	                \
static void                                                                             \
scorep_profile_write_cube_##cube_type(                                                  \
                                  cube_t*                             my_cube,          \
                                  cube_writer*                        cube_writer,      \
                                  cube_metric*                        metric,           \
                                  scorep_profile_node**               id_2_node,        \
                                  scorep_cube4_definitions_map*       map,              \
                                  uint32_t                            callpath_number,  \
                                  uint32_t                            global_threads,   \
                                  uint32_t                            local_threads,    \
                                  uint32_t                            offset,           \
                                  int*                                recvcnts,         \
                                  int*                                displs,           \
                                  scorep_profile_get_ ## cube_type ## _func get_value ) \
{                                                                                       \
    scorep_profile_node* node          = NULL;                                          \
    cube_cnode*          cnode         = NULL;                                          \
    type *               local_values  = NULL;                                          \
    type *               global_values = NULL;                                          \
    int                  my_rank       = SCOREP_Mpi_GetRank();                          \
    char*                bit_vector    = NULL;                                          \
    if ( callpath_number == 0 ) return;                                                 \
                                                                                        \
    local_values = ( type * )malloc( local_threads * sizeof( type ) );                  \
                                                                                        \
    if ( my_rank == 0 )                                                                 \
    {                                                                                   \
        /* Array of all values for one metric for one callpath for all locations */     \
        global_values = ( type * )malloc( global_threads * sizeof( type ) );            \
        bit_vector    = ( char* )malloc( ( callpath_number + 7 ) / 8 );                 \
        SCOREP_ASSERT( bit_vector );                                                    \
        memset( bit_vector, 0xFF, ( callpath_number + 7 ) / 8 );                        \
                                                                                        \
        /* Initialize writing of a new metric */                                        \
        cubew_reset( cube_writer );                                                     \
        cubew_set_array( cube_writer, callpath_number );                                \
        cube_set_known_cnodes_for_metric( my_cube, metric, bit_vector );                \
    }                                                                                   \
    /* Iterate over all unified callpathes */                                           \
    for ( uint64_t cp_index = 0; cp_index < callpath_number; cp_index++ )               \
    {                                                                                   \
        for ( uint64_t thread_index = 0; thread_index < local_threads; thread_index++ ) \
        {                                                                               \
            node = id_2_node[ thread_index * callpath_number + cp_index ];              \
            if ( node != NULL )                                                         \
            {                                                                           \
                local_values[ thread_index ] = get_value( node );                       \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                local_values[ thread_index ] = 0;                                       \
            }                                                                           \
        }                                                                               \
                                                                                        \
        /* Collect data from all processes */                                           \
        SCOREP_Mpi_Gatherv( local_values, local_threads, SCOREP_MPI_ ## TYPE,           \
                            global_values, recvcnts, displs, SCOREP_MPI_ ## TYPE, 0 );  \
                                                                                        \
        /* Write data for one callpath */                                               \
        if ( my_rank == 0 )                                                             \
        {                                                                               \
	    cnode = cube_get_cnode( my_cube, cp_index );                                \
            cube_write_sev_row_of_##cube_type( my_cube, metric, cnode, global_values ); \
        }                                                                               \
    }                                                                                   \
                                                                                        \
    /* Clean up */                                                                      \
    free( global_values );                                                              \
    free( local_values );                                                               \
}

/* *INDENT-ON* */

/**
   @function scorep_profile_write_cube_uint64
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
   @param global_threads  The number of threads in the unified definitions. Only needed
                          on rank 0. Ignored on other ranks.
   @param local_threads   The number of locations in this process.
   @param offset          Position in the global value vector where this process writes.
   @param recvcnts        Array with the number of threads for every process. Only needed
                          on rank 0. Ignored on other ranks.
   @param displs          Array with offsets for every process. Only needed on rank 0.
                          Ignored on other ranks
   @param get_value       Functionpointer which returns the value for a given
                          profile node.
 */
SCOREP_PROFILE_WRITE_CUBE_METRIC( uint64_t, LONG_LONG, uint64 )

/**
   @function scorep_profile_write_cube_double
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
   @param global_threads  The number of threads in the unified definitions. Only needed
                          on rank 0. Ignored on other ranks.
   @param local_threads   The number of locations in this process.
   @param offset          Position in the global value vector where this process writes.
   @param recvcnts        Array with the number of threads for every process. Only needed
                          on rank 0. Ignored on other ranks.
   @param displs          Array with offsets for every process. Only needed on rank 0.
                          Ignored on other ranks
   @param get_value       Functionpointer which returns the value for a given
                          profile node.
 */
SCOREP_PROFILE_WRITE_CUBE_METRIC( double, DOUBLE, doubles )

/* *****************************************************************************
   Main writer function
*******************************************************************************/
void
scorep_profile_write_cube4()
{
    /*-------------------------------- Variable definition */

    /* Number of ranks for parallel writing of Cube files.
       Currently, only serial writing */
    int32_t number_of_writers = 1;

    /* The cube structure */
    cube_t* my_cube = NULL;

    /* The cube writer */
    cube_writer* cube_writer = NULL;

    /* Contains the basename for the cube file */
    char* filename = NULL;

    /* Directory where the data is stored */
    char* dirname = SCOREP_GetExperimentDirName();

    /* Number of this rank in MPI_COMM_WORLD */
    uint32_t my_rank = SCOREP_Mpi_GetRank();

    /* Number of ranks in MPI_COMM_WORLD */
    uint32_t ranks_number = SCOREP_Mpi_GetCommWorldSize();

    /* Number of threads in this rank */
    uint32_t local_threads = scorep_profile_get_number_of_threads();

    /* Sum of all threads in all processes. Only used on Rank 0 */
    uint32_t global_threads = 0;

    /* Number of callpathes in the unified definitions */
    uint32_t callpath_number = 0;

    /* Offset for values of this process in the value vector */
    uint32_t offset = 0;

    /* List of numbers of locations on each process. Only used on rank 0 */
    int* threads_per_rank = NULL;

    /* List of offsets of every process in the value vector.
       Only used on rank 0 */
    int* offset_per_rank = NULL;

    /* Mapping from global sequence number to local profile node of every thread */
    scorep_profile_node** id_2_node = NULL;

    /* Pointer to Cube 4 metric definition. Only used on Rank 0 */
    cube_metric* metric = NULL;

    /* Pointer to a profile node, used to iterate over root nodes */
    scorep_profile_node* node = scorep_profile.first_root_node;

    /* Mapping structure for mapping cbe handles to Score-P handle */
    scorep_cube4_definitions_map* map = NULL;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                         "Writing profile in Cube 4 format ..." );

    /* -------------------------------- Initialization */

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Prepare writing" );

    /* Perform initilazation for on rank 0 */
    if ( my_rank == 0 )
    {
        /* Get number of callpathes in unified definitions */
        callpath_number = SCOREP_Callpath_GetNumberOfUnifiedDefinitions();

        /* Create vectors with offsets and recvcounts on rank 0 */
        threads_per_rank = ( int* )malloc( ranks_number * sizeof( int ) );
        offset_per_rank  = ( int* )malloc( ranks_number * sizeof( int ) );
    }

    /* Distribute number of callpathes in unified definitions */
    SCOREP_Mpi_Bcast( &callpath_number, 1, SCOREP_MPI_UNSIGNED, 0 );
    if ( callpath_number == 0 )
    {
        goto cleanup;
    }

    /* Get sum of locations of all processes */
    SCOREP_Mpi_Reduce( &local_threads, &global_threads, 1, SCOREP_MPI_UNSIGNED,
                       SCOREP_MPI_SUM, 0 );

    /* Calculate offset of this thread in the value vector */
    SCOREP_Mpi_Exscan( &local_threads, &offset, 1, SCOREP_MPI_UNSIGNED, SCOREP_MPI_SUM );

    /* Collect number of threads from every rank to rank 0. */
    SCOREP_Mpi_Gather( &local_threads, 1, SCOREP_MPI_UNSIGNED, threads_per_rank, 1,
                       SCOREP_MPI_INT, 0 );

    /* Collect number of offsets from every rank to rank 0. */
    SCOREP_Mpi_Gather( &offset, 1, SCOREP_MPI_UNSIGNED, offset_per_rank, 1, SCOREP_MPI_INT, 0 );

    /* Create definition mapping tables */
    map = scorep_cube4_create_definitions_map();
    if ( map == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                      "Failed to allocate ememory for defintion mapping\n"
                      "Failed to write Cube 4 profile" );
        goto cleanup;
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
                            global_threads,    /* sum of all threads of all nodes */
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
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing definitions" );
        scorep_write_definitions_to_cube4( my_cube, map, ranks_number, threads_per_rank );
    }

    /* Build mapping from sequence number in unified callpath definitions to
       profile nodes */
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Create mappings" );
    id_2_node = calloc( callpath_number * local_threads, sizeof( scorep_profile_node* ) );

    for ( uint64_t i = 0; node != NULL; node = node->next_sibling )
    {
        scorep_profile_for_all( node,
                                scorep_cube4_make_mapping,
                                &id_2_node[ i ] );
        i += callpath_number;
    }

    /* Write implicit time */
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing runtime" );
    if ( my_rank == 0 )
    {
        metric = scorep_get_cube4_metric( map, ( SCOREP_CounterHandle )1 );
    }
    scorep_profile_write_cube_doubles( my_cube, cube_writer, metric, id_2_node, map,
                                       callpath_number, global_threads, local_threads,
                                       offset, threads_per_rank, offset_per_rank,
                                       &scorep_profile_get_time_value );


    /* Write visits */
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing visits" );
    if ( my_rank == 0 )
    {
        metric = scorep_get_cube4_metric( map, ( SCOREP_CounterHandle )2 );
    }
    scorep_profile_write_cube_uint64( my_cube, cube_writer, metric, id_2_node, map,
                                      callpath_number, global_threads, local_threads,
                                      offset, threads_per_rank, offset_per_rank,
                                      &scorep_profile_get_visits_value );


    /* Clean up */
cleanup:
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Clean up" );
    free( id_2_node );
    free( offset_per_rank );
    free( threads_per_rank );
    if ( cube_writer )
    {
        cubew_finalize( cube_writer );
    }
    if ( map )
    {
        scorep_cube4_delete_definitions_map( map );
    }
    free( filename );
}
