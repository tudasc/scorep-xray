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
 * @file scorep_profile_cube4_writer_mpi.c
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 * Implements a profile writer in Cube 4 format for mpi applications.
 */

#include <config.h>
#include <sys/stat.h>
#include <inttypes.h>

#include <SCOREP_Memory.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Timing.h>
#include <SCOREP_Bitstring.h>

#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>
#include <scorep_definition_cube4.h>
#include <scorep_definitions.h>
#include <scorep_mpi.h>
#include <scorep_runtime_management.h>

#include <cubew_cube.h>
#include <cubew_cubew.h>
#include <cubew_services.h>

#define SCOREP_PROFILE_DENSE_METRIC ( ( SCOREP_MetricHandle )UINT64_MAX - 1 )

extern SCOREP_DefinitionManager  scorep_local_definition_manager;
extern SCOREP_DefinitionManager* scorep_unified_definition_manager;
extern SCOREP_MetricHandle       scorep_profile_active_task_metric;

/* *****************************************************************************
   Typedefs and variable declarations
*******************************************************************************/

/**
   Data set needed for various writing functions
 */
typedef struct
{
    cube_t*                       my_cube;          /**< Cube object that is created */
    cube_writer*                  cube_writer;      /**< Cube writer object */
    scorep_profile_node**         id_2_node;        /**< maps global sequence number */
    scorep_cube4_definitions_map* map;              /**< maps Score-P and Cube handles */
    uint32_t                      callpath_number;  /**< Number of callpathes */
    uint32_t                      global_threads;   /**< Global number of locations */
    uint32_t                      local_threads;    /**< Number of threads in this rank */
    uint32_t                      offset;           /**< Offset for this rank */
    uint32_t                      my_rank;          /**< This rank */
    uint32_t                      ranks_number;     /**< Number of ranks in COMM_WORLD */
    int*                          threads_per_rank; /**< List of elements per rank */
    int*                          offsets_per_rank; /**< List of offsets per rank */
    SCOREP_MetricHandle*          metric_map;       /**< map sequence no to handle */
    SCOREP_MetricHandle*          unified_map;      /**< map sequence to unified handle */
    uint8_t*                      bit_vector;       /**< Indicates callpath with values */
    int32_t                       has_tasks;        /**< Whether tasks occured */
    uint32_t                      num_unified;      /**< Number of unified metrics */
} scorep_cube_writing_data;


/**
   Defines a function type which returns a metric value from a given node.
   These functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Pointer to function specific data.
   @returns the metric value of @a node.
 */
typedef uint64_t ( *scorep_profile_get_uint64_func )( scorep_profile_node* node, void* data );

/**
   Defines a function type which returns a metric value from a given node.
   These functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Pointer to function specific data.
   @returns the metric value of @a node.
 */
typedef double ( *scorep_profile_get_doubles_func )( scorep_profile_node* node, void* data );

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
scorep_cube4_make_callpath_mapping( scorep_profile_node* node,
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
   Creates a mapping from global sequence numbers to local metric definitions. The
   global sequence numbers define the order in which the metrics are written.
   @param metric_number Number of unified definitions.
 */
static SCOREP_MetricHandle*
scorep_cube4_make_metric_mapping( uint32_t metric_number )
{
    uint32_t             i;
    SCOREP_MetricHandle* map = malloc( sizeof( SCOREP_MetricHandle ) * metric_number );
    if ( map == NULL )
    {
        return NULL;
    }
    for ( i = 0; i < metric_number; i++ )
    {
        map[ i ] = SCOREP_INVALID_METRIC;
    }

    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, Metric, metric )
    {
        map[ SCOREP_Metric_GetUnifiedSequenceNumber( handle ) ] = handle;
    }
    SCOREP_DEFINITION_FOREACH_WHILE();

    return map;
}

/**
   Creates a mapping from global sequence numbers to unified metric definitions. The
   global sequence numbers define the order in which the metrics are written.
 */
static SCOREP_MetricHandle*
scorep_cube4_make_unified_mapping()
{
    uint32_t             i             = 0;
    uint32_t             metric_number = SCOREP_Metric_GetNumberOfUnifiedDefinitions();
    SCOREP_MetricHandle* map           = malloc( sizeof( SCOREP_MetricHandle ) * metric_number );
    if ( map == NULL )
    {
        return NULL;
    }

    SCOREP_DEFINITION_FOREACH_DO( scorep_unified_definition_manager, Metric, metric )
    {
        map[ i ] = handle;
        i++;
    }
    SCOREP_DEFINITION_FOREACH_WHILE();

    return map;
}

/**
   Returns the sum of implicit runtime for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Ignored.
   @returns the implicit runtime of @a node.
 */
static double
scorep_profile_get_sum_time_value( scorep_profile_node* node, void* data )
{
    return ( ( double )node->inclusive_time.sum ) / ( ( double )SCOREP_GetClockResolution() );
}

/**
   Returns the sum of implicit runtime for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Ignored.
   @returns the implicit runtime of @a node.
 */
static double
scorep_profile_get_max_time_value( scorep_profile_node* node, void* data )
{
    if ( node->count == 0 )
    {
        return 0;
    }
    return ( ( double )node->inclusive_time.max ) * 1000000.0 / ( ( double )SCOREP_GetClockResolution() );
}

/**
   Returns the sum of implicit runtime for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Ignored.
   @returns the implicit runtime of @a node.
 */
static double
scorep_profile_get_min_time_value( scorep_profile_node* node, void* data )
{
    if ( node->count == 0 )
    {
        return 0;
    }
    return ( ( double )node->inclusive_time.min ) * 1000000.0 / ( ( double )SCOREP_GetClockResolution() );
}

/**
   Returns the sum of implicit runtime for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Ignored.
   @returns the implicit runtime of @a node.
 */
static double
scorep_profile_get_mean_time_value( scorep_profile_node* node, void* data )
{
    if ( node->count == 0 )
    {
        return 0;
    }
    return ( ( double )node->inclusive_time.sum ) * 1000000.0 / ( ( double )SCOREP_GetClockResolution() ) / ( ( double )node->count );
}



/**
   Returns the number of visits for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Ignored.
   @returns the number of visits of @a node.
 */
static uint64_t
scorep_profile_get_visits_value( scorep_profile_node* node, void* data )
{
    return node->count;
}



/**
   Returns the values of metrics for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node  Pointer to a node which should return the metric value.
   @param index Pointer to a uint8_t value that contains the index of the metric in the
                dense metric vector.
   @returns the number of visits of @a node.
 */
static uint64_t
scorep_profile_get_metrics_value_from_array( scorep_profile_node* node, void* index )
{
    return node->dense_metrics[ *( uint8_t* )index ].sum;
}

/**
   Returns the sparse uint64_t metric value in @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node  Pointer to a node which should return the metric value.
   @param data  Pointer to a metric handle specifying the desired metric.
   @returns the value of the specified metric if it has an entry in the node.
            Otherwise 0 is returned.
 */
static uint64_t
scorep_profile_get_sparse_uint64_value( scorep_profile_node* node, void* data )
{
    SCOREP_MetricHandle               metric  = *( SCOREP_MetricHandle* )data;
    scorep_profile_sparse_metric_int* current = node->first_int_sparse;

    if ( metric == SCOREP_INVALID_METRIC )
    {
        return 0;
    }

    while ( current != NULL )
    {
        if ( current->metric == metric )
        {
            switch ( SCOREP_Metric_GetProfilingType( metric ) )
            {
                case SCOREP_METRIC_PROFILING_TYPE_MAX:
                    printf( "%" PRIu64 "\n", current->max );
                    return current->max;
                case SCOREP_METRIC_PROFILING_TYPE_MIN:
                    return current->min;
                default:
                    return current->sum;
            }
        }
        current = current->next_metric;
    }
    return 0;
}

/**
   Returns the sparse double metric value in @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node  Pointer to a node which should return the metric value.
   @param data  Pointer to a metric handle specifying the desired metric.
   @returns the value of the specified metric if it has an entry in the node.
            Otherwise 0 is returned.
 */
static double
scorep_profile_get_sparse_double_value( scorep_profile_node* node, void* data )
{
    SCOREP_MetricHandle                  metric  = *( SCOREP_MetricHandle* )data;
    scorep_profile_sparse_metric_double* current = node->first_double_sparse;

    if ( metric == SCOREP_INVALID_METRIC )
    {
        return 0.0;
    }

    while ( current != NULL )
    {
        if ( current->metric == metric )
        {
            switch ( SCOREP_Metric_GetProfilingType( metric ) )
            {
                case SCOREP_METRIC_PROFILING_TYPE_MAX:
                    return current->max;
                case SCOREP_METRIC_PROFILING_TYPE_MIN:
                    return current->min;
                default:
                    return current->sum;
            }
        }
        current = current->next_metric;
    }
    return 0.0;
}

/**
   Returns a truth value whether a sparse double metric value exists in  @a node.
   This functions are given to scorep_profile_set_bitstring_for_metric.
   @param node  Pointer to a node which should return the metric value.
   @param data  Pointer to a metric handle specifying the desired metric.
   @returns the value of the specified metric if it has an entry in the node.
            Otherwise 0 is returned.
 */
static uint64_t
scorep_profile_has_sparse_double_value( scorep_profile_node* node, void* data )
{
    SCOREP_MetricHandle metric = *( SCOREP_MetricHandle* )data;
    if ( SCOREP_Metric_GetProfilingType( metric ) == SCOREP_METRIC_PROFILING_TYPE_MAX )
    {
        return 1;
    }

    return ( uint64_t )( scorep_profile_get_sparse_double_value( node, data ) == 0 ? 0 : 1 );
}

void
scorep_profile_set_bitstring_for_metric(
    scorep_cube_writing_data*      write_set,
    scorep_profile_get_uint64_func get_value,
    void*                          func_data )
{
    scorep_profile_node* node = NULL;

    /* Create empty bitstring */
    uint8_t* bits = malloc( SCOREP_Bitstring_GetByteSize( write_set->callpath_number ) );
    SCOREP_ASSERT( bits );
    SCOREP_Bitstring_Clear( bits, write_set->callpath_number );

    /* Iterate over all unified callpathes */
    for ( uint64_t cp_index = 0; cp_index < write_set->callpath_number; cp_index++ )
    {
        for ( uint64_t thread_index = 0;
              thread_index < write_set->local_threads; thread_index++ )
        {
            uint64_t node_index = thread_index * write_set->callpath_number + cp_index;
            node = write_set->id_2_node[ node_index ];
            if ( ( node != NULL ) && ( get_value( node, func_data ) != 0 ) )
            {
                SCOREP_Bitstring_Set( bits, cp_index  );
            }
        }
    }

    SCOREP_Mpi_Allreduce( bits, write_set->bit_vector,
                          ( write_set->callpath_number + 7 ) / 8,
                          SCOREP_MPI_UNSIGNED_CHAR, SCOREP_MPI_BOR );
    free( bits );
}

/* We must ensure that all ranks paricipate in collective operations. Thus, this
   function is a placeholder for @a scorep_profile_set_bitstring_for_metric in case
   a metric is not defined on this rank
 */
void
scorep_profile_set_bitstring_for_unknown_metric( scorep_cube_writing_data* write_set )
{
    /* Create empty bitstring */
    uint8_t* bits = malloc( SCOREP_Bitstring_GetByteSize( write_set->callpath_number ) );
    SCOREP_ASSERT( bits );
    SCOREP_Bitstring_Clear( bits, write_set->callpath_number );

    SCOREP_Mpi_Allreduce( bits, write_set->bit_vector,
                          ( write_set->callpath_number + 7 ) / 8,
                          SCOREP_MPI_UNSIGNED_CHAR, SCOREP_MPI_BOR );
    free( bits );
}


/* *INDENT-OFF* */

/**
   @def SCOREP_PROFILE_WRITE_CUBE_METRIC
   Code to write metric values in cube format. Used to reduce code replication.
 */
#define SCOREP_PROFILE_WRITE_CUBE_METRIC( type, TYPE, cube_type )	                \
static void                                                                             \
scorep_profile_write_cube_##cube_type(                                                  \
                                   scorep_cube_writing_data*                 write_set, \
                                   cube_metric*                              metric,    \
                                   scorep_profile_get_ ## cube_type ## _func get_value, \
                                   void*                                     func_data) \
{                                                                                       \
    scorep_profile_node* node          = NULL;                                          \
    cube_cnode*          cnode         = NULL;                                          \
    type *               local_values  = NULL;                                          \
    type *               global_values = NULL;                                          \
    int                  my_rank       = SCOREP_Mpi_GetRank();                          \
    if ( write_set->callpath_number == 0 ) return;                                      \
                                                                                        \
    local_values = ( type * )malloc( write_set->local_threads * sizeof( type ) );       \
                                                                                        \
    if ( write_set->my_rank == 0 )                                                      \
    {                                                                                   \
        /* Array of all values for one metric for one callpath for all locations */     \
        global_values = ( type * )malloc( write_set->global_threads * sizeof( type ) ); \
                                                                                        \
        /* Initialize writing of a new metric */                                        \
        cubew_reset( write_set->cube_writer );                                          \
        cubew_set_array( write_set->cube_writer, write_set->callpath_number );          \
        cube_set_known_cnodes_for_metric( write_set->my_cube, metric,                   \
                                          (char*)write_set->bit_vector );               \
    }                                                                                   \
    /* Iterate over all unified callpathes */                                           \
    for ( uint64_t cp_index = 0; cp_index < write_set->callpath_number; cp_index++ )    \
    {                                                                                   \
        if ( ! SCOREP_Bitstring_IsSet( write_set->bit_vector, cp_index ) )              \
	{                                                                               \
            continue;                                                                   \
        }                                                                               \
        for ( uint64_t thread_index = 0;                                                \
              thread_index < write_set->local_threads; thread_index ++ )                \
        {                                                                               \
            uint64_t node_index = thread_index * write_set->callpath_number + cp_index; \
            node = write_set->id_2_node[ node_index ];                                  \
            if ( node != NULL )                                                         \
            {                                                                           \
	        local_values[ thread_index ] = get_value( node, func_data );            \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                local_values[ thread_index ] = 0;                                       \
            }                                                                           \
        }                                                                               \
                                                                                        \
        /* Collect data from all processes */                                           \
        SCOREP_Mpi_Barrier();                                                           \
        SCOREP_Mpi_Gatherv( local_values, write_set->local_threads,                     \
                            SCOREP_MPI_ ## TYPE,                                        \
                            global_values, write_set->threads_per_rank,                 \
                            write_set->offsets_per_rank,                                \
                            SCOREP_MPI_ ## TYPE, 0 );                                   \
                                                                                        \
        /* Write data for one callpath */                                               \
        if ( write_set->my_rank == 0 )                                                  \
        {                                                                               \
	    cnode = cube_get_cnode( write_set->my_cube, cp_index );                     \
            cube_write_sev_row_of_##cube_type( write_set->my_cube, metric,              \
                                               cnode, global_values );                  \
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
   @param write_set       Structure containing write data.
   @param metric          The cube metric handle for the written metric.
   @param get_value       Functionpointer which returns the value for a given
                          profile node.
   @param func_data       Pointer to data that is passed to the @a get_value function
 */
SCOREP_PROFILE_WRITE_CUBE_METRIC( uint64_t, LONG_LONG, uint64 )

/**
   @function scorep_profile_write_cube_double
   Writes data for the metric @a metric to a cube object.
   @param write_set       Structure containing write data.
   @param metric          The cube metric handle for the written metric.
   @param get_value       Functionpointer which returns the value for a given
                          profile node.
   @param func_data       Pointer to data that is passed to the @a get_value function
 */
SCOREP_PROFILE_WRITE_CUBE_METRIC( double, DOUBLE, doubles )


/**
   Returns true, if sparce metric @a metric is written.
   @param metric          Metric handle that is goinf to be written.
 */
static bool
scorep_profile_check_if_metric_shall_be_written( scorep_cube_writing_data* write_set,
                                                 SCOREP_MetricHandle       metric )
{
    return ( metric != SCOREP_PROFILE_DENSE_METRIC ) &&
           ( metric != scorep_profile_active_task_metric ||
             write_set->has_tasks );
}

/* *****************************************************************************
   Handle scorep_cube_writing_data
*******************************************************************************/

static void
scorep_profile_delete_cube_writing_data( scorep_cube_writing_data* write_set )
{
    if ( write_set == NULL )
    {
        return;
    }
    free( write_set->id_2_node );
    free( write_set->offsets_per_rank );
    free( write_set->threads_per_rank );
    free( write_set->metric_map );
    free( write_set->bit_vector );
    if ( write_set->cube_writer )
    {
        cubew_finalize( write_set->cube_writer );
    }
    if ( write_set->map )
    {
        scorep_cube4_delete_definitions_map( write_set->map );
    }

    write_set->my_cube          = NULL;
    write_set->cube_writer      = NULL;
    write_set->id_2_node        = NULL;
    write_set->map              = NULL;
    write_set->threads_per_rank = NULL;
    write_set->offsets_per_rank = NULL;
    write_set->metric_map       = NULL;
    write_set->unified_map      = NULL;
    write_set->bit_vector       = NULL;
}

static bool
scorep_profile_init_cube_writing_data( scorep_cube_writing_data* write_set )
{
    /* Set all pointers to zero.
       If an malloc fails, we know how many can bee freed */
    write_set->my_cube          = NULL;
    write_set->cube_writer      = NULL;
    write_set->id_2_node        = NULL;
    write_set->map              = NULL;
    write_set->threads_per_rank = NULL;
    write_set->offsets_per_rank = NULL;
    write_set->metric_map       = NULL;
    write_set->bit_vector       = NULL;

    /* ------------------------------------ Start initializing */

    /* Get basic MPI data */
    write_set->my_rank       = SCOREP_Mpi_GetRank();
    write_set->local_threads = scorep_profile_get_number_of_threads();
    write_set->ranks_number  = SCOREP_Mpi_GetCommWorldSize();

    /* Get the number of unified callpath definitions to all ranks */
    if ( write_set->my_rank == 0 )
    {
        write_set->callpath_number = SCOREP_Callpath_GetNumberOfUnifiedDefinitions();
    }
    SCOREP_Mpi_Bcast( &write_set->callpath_number, 1, SCOREP_MPI_UNSIGNED, 0 );
    if ( write_set->callpath_number == 0 )
    {
        return false;
    }

    /* Calculate the global number of locations */
    SCOREP_Mpi_Reduce( &write_set->local_threads, &write_set->global_threads,
                       1, SCOREP_MPI_UNSIGNED, SCOREP_MPI_SUM, 0 );


    /* Calculate the offset of this thread in the value vector
       Normally, I need MPI_Exscan, but since it is not available in MPI 1.0 it is
       emulated with MPI_Scan.
     */
    SCOREP_Mpi_Scan( &write_set->local_threads, &write_set->offset, 1,
                     SCOREP_MPI_UNSIGNED, SCOREP_MPI_SUM );
    write_set->offset -= write_set->local_threads;


    /* Calculate the offsets of all ranks and the number of locations per rank */
    if ( write_set->my_rank == 0 )
    {
        size_t buffer_size = write_set->ranks_number * sizeof( int );
        write_set->threads_per_rank = ( int* )malloc( buffer_size );
        write_set->offsets_per_rank = ( int* )malloc( buffer_size );
    }
    SCOREP_Mpi_Gather( &write_set->local_threads, 1, SCOREP_MPI_UNSIGNED,
                       write_set->threads_per_rank, 1, SCOREP_MPI_INT, 0 );
    SCOREP_Mpi_Gather( &write_set->offset, 1, SCOREP_MPI_UNSIGNED,
                       write_set->offsets_per_rank, 1, SCOREP_MPI_INT, 0 );

    /* Get number of unified metrics to every rank */
    if ( write_set->my_rank == 0 )
    {
        write_set->num_unified = SCOREP_Metric_GetNumberOfUnifiedDefinitions();
    }
    SCOREP_Mpi_Bcast( &write_set->num_unified, 1, SCOREP_MPI_UNSIGNED, 0 );

    /* Create the mappings from cube to Score-P handles and vice versa */
    write_set->map = scorep_cube4_create_definitions_map();
    if ( write_set->map == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                      "Failed to allocate memory for definition mapping\n"
                      "Failed to write Cube 4 profile" );

        scorep_profile_delete_cube_writing_data( write_set );
        return false;
    }

    /* Create the Cube writer object and the Cube object */
    if ( write_set->my_rank == 0 )
    {
        /* Construct cube file name */
        const char* dirname  = SCOREP_GetExperimentDirName();
        char*       filename = NULL;

        filename = ( char* )malloc( strlen( dirname ) +               /* Directory     */
                                    1 +                               /* separator '/' */
                                    strlen( scorep_profile.basename ) /* basename      */
                                    + 1 );                            /* trailing '\0' */
        if ( filename == NULL )
        {
            SCOREP_ERROR_POSIX( "Failed to allocate memory for filename.\n"
                                "Failed to write Cube 4 profile" );
            scorep_profile_delete_cube_writing_data( write_set );
            return false;
        }
        sprintf( filename, "%s/%s", dirname, scorep_profile.basename );

        /* Create Cube objects */
        write_set->cube_writer
            = cubew_create( write_set->my_rank,        /* rank of this node          */
                            write_set->global_threads, /* global sum of threads      */
                            1,                         /* number of parallel writers */
                            filename,                  /* base file name             */
                            CUBE_FALSE );              /* no zlib compression        */
        free( filename );

        write_set->my_cube = cubew_get_cube( write_set->cube_writer );
    }

    /* Create bit_vector with all bits set. Used for dense metrics */
    write_set->bit_vector =
        ( uint8_t* )malloc( SCOREP_Bitstring_GetByteSize( write_set->callpath_number ) );
    SCOREP_ASSERT( write_set->bit_vector );
    SCOREP_Bitstring_SetAll( write_set->bit_vector, write_set->callpath_number );

    /* Check whether tasks has been used somewhere */
    int32_t has_tasks = scorep_profile_has_tasks();
    write_set->has_tasks = 0;
    SCOREP_Mpi_Allreduce( &has_tasks,
                          &write_set->has_tasks,
                          1,
                          SCOREP_MPI_INT,
                          SCOREP_MPI_BOR );

    return true;
}

static void
scorep_profile_add_mapping_to_cube_writing_data( scorep_cube_writing_data* write_set )
{
    /* Map global sequence numbers to profile nodes */
    write_set->id_2_node = calloc( write_set->callpath_number * write_set->local_threads,
                                   sizeof( scorep_profile_node* ) );

    scorep_profile_node* node = scorep_profile.first_root_node;
    for ( uint64_t i = 0; node != NULL; node = node->next_sibling )
    {
        scorep_profile_for_all( node,
                                scorep_cube4_make_callpath_mapping,
                                &write_set->id_2_node[ i ] );
        i += write_set->callpath_number;
    }

    /* Mapping from global sequence number to local metric handle. Defines
       order of writing metrics */
    write_set->metric_map = scorep_cube4_make_metric_mapping( write_set->num_unified );

    if ( write_set->my_rank == 0 )
    {
        write_set->unified_map = scorep_cube4_make_unified_mapping();
    }
}

/* *****************************************************************************
   Main writer function
*******************************************************************************/
void
scorep_profile_write_cube4()
{
    /*-------------------------------- Variable definition */

    /* Pointer to Cube 4 metric definition. Only used on Rank 0 */
    cube_metric* metric = NULL;

    /* Data set for Cube write functions */
    scorep_cube_writing_data write_set;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                         "Writing profile in Cube 4 format ..." );

    /* -------------------------------- Initialization, header and definitions */

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Prepare writing" );

    if ( !scorep_profile_init_cube_writing_data( &write_set ) )
    {
        return;
    }


    if ( write_set.my_rank == 0 )
    {
        /* generate header */
        cube_def_attr( write_set.my_cube, "Creator", "Score-P " PACKAGE_VERSION );
        cube_def_attr( write_set.my_cube, "CUBE_CT_AGGR", "SUM" );
        cube_def_mirror( write_set.my_cube, "file://" DOCDIR "/profile/" );

        /* Write definitions to cube */
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing definitions" );
        scorep_write_definitions_to_cube4( write_set.my_cube,
                                           write_set.map,
                                           write_set.ranks_number,
                                           write_set.threads_per_rank,
                                           write_set.has_tasks );
    }

    /* Build mapping from sequence number in unified callpath definitions to
       profile nodes */
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Create mappings" );
    scorep_profile_add_mapping_to_cube_writing_data( &write_set );

    /* -------------------------------- dense metrics */

    /* Write implicit time */
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing runtime" );

    scorep_profile_write_cube_doubles( &write_set, scorep_get_sum_time_handle(),
                                       &scorep_profile_get_sum_time_value, NULL );


    scorep_profile_write_cube_doubles( &write_set, scorep_get_max_time_handle(),
                                       &scorep_profile_get_max_time_value, NULL );

    scorep_profile_write_cube_doubles( &write_set, scorep_get_min_time_handle(),
                                       &scorep_profile_get_min_time_value, NULL );

    scorep_profile_write_cube_doubles( &write_set, scorep_get_mean_time_handle(),
                                       &scorep_profile_get_mean_time_value, NULL );


    /* Write visits */
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing visits" );
    scorep_profile_write_cube_uint64( &write_set, scorep_get_visits_handle(),
                                      &scorep_profile_get_visits_value, NULL );

    /* Write additional dense metrics (e.g. hardware counters) */
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing dense metrics" );
    for ( uint8_t i = 0; i  < scorep_profile.num_of_dense_metrics; i++ )
    {
        cube_metric* metric = NULL; /* Only used on rank 0 */

        if ( write_set.my_rank == 0 )
        {
            metric = scorep_get_cube4_metric( write_set.map,
                                              SCOREP_Metric_GetUnifiedHandle( scorep_profile.dense_metrics[ i ] ) );
        }

        /* When writing sparse metrics, we skip the time metric handles.
           Thus, invalidate these entries to avoid
           writing them twice. */
        if ( write_set.metric_map != NULL )
        {
            uint32_t current_number =
                SCOREP_Metric_GetUnifiedSequenceNumber( scorep_profile.dense_metrics[ i ] );
            write_set.metric_map[ current_number ] = SCOREP_PROFILE_DENSE_METRIC;
        }

        scorep_profile_write_cube_uint64( &write_set, metric,
                                          &scorep_profile_get_metrics_value_from_array, &i );
    }

    /* -------------------------------- sparse metrics */


    /* Write sparse metrics (e.g. user metrics) */
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing sparse metrics" );
    if ( write_set.metric_map != NULL )
    {
        cube_metric* metric = NULL; /* Only used on rank 0 */

        for ( uint32_t i = 0; i < write_set.num_unified; i++ )
        {
            if ( !scorep_profile_check_if_metric_shall_be_written( &write_set,
                                                                   write_set.metric_map[ i ] ) )
            {
                continue;
            }

            if ( write_set.my_rank == 0 )
            {
                metric = scorep_get_cube4_metric( write_set.map,
                                                  write_set.unified_map[ i ] );
            }

            if ( write_set.metric_map[ i ] == SCOREP_INVALID_METRIC )
            {
                scorep_profile_set_bitstring_for_unknown_metric( &write_set );
                scorep_profile_write_cube_doubles( &write_set,
                                                   metric,
                                                   &scorep_profile_get_sparse_double_value,
                                                   &write_set.metric_map[ i ] );
                continue;
            }

            switch ( SCOREP_Metric_GetValueType( write_set.metric_map[ i ] ) )
            {
                case SCOREP_METRIC_VALUE_INT64:
                case SCOREP_METRIC_VALUE_UINT64:
                    scorep_profile_set_bitstring_for_metric( &write_set,
                                                             &scorep_profile_get_sparse_uint64_value,
                                                             &write_set.metric_map[ i ] );
                    scorep_profile_write_cube_uint64( &write_set,
                                                      metric,
                                                      &scorep_profile_get_sparse_uint64_value,
                                                      &write_set.metric_map[ i ] );

                    break;
                case SCOREP_METRIC_VALUE_DOUBLE:
                    scorep_profile_set_bitstring_for_metric( &write_set,
                                                             &scorep_profile_has_sparse_double_value,
                                                             &write_set.metric_map[ i ] );
                    scorep_profile_write_cube_doubles( &write_set,
                                                       metric,
                                                       &scorep_profile_get_sparse_double_value,
                                                       &write_set.metric_map[ i ] );

                    break;
                default:
                    SCOREP_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                                  "Metric %s has unknown value type %d",
                                  SCOREP_Metric_GetName( write_set.metric_map[ i ] ),
                                  SCOREP_Metric_GetValueType( write_set.metric_map[ i ] ) );
            }
        }
    }
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile writing done" );

    /* Clean up */
    //cleanup:
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Clean up" );
    scorep_profile_delete_cube_writing_data( &write_set );
}
