/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 * Implements a profile writer in Cube 4 format.
 */

#include <config.h>
#include <scorep_profile_cube4_writer.h>
#include <sys/stat.h>
#include <inttypes.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Memory.h>
#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <SCOREP_Timer_Utils.h>
#include <SCOREP_Bitstring.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Definitions.h>

#include <scorep_profile_definition.h>
#include <scorep_profile_location.h>
#include <scorep_profile_task_init.h>
#include <scorep_ipc.h>
#include <scorep_runtime_management.h>
#include <scorep_location.h>

#define SCOREP_PROFILE_DENSE_METRIC ( ( SCOREP_MetricHandle )UINT32_MAX - 1 )

extern SCOREP_MetricHandle scorep_profile_migration_loss_metric;
extern SCOREP_MetricHandle scorep_profile_migration_win_metric;

extern void
scorep_cluster_write_cube4( scorep_cube_writing_data* writeData );

/* *****************************************************************************
   Typedefs and variable declarations
*******************************************************************************/

/**
   Defines a function type which returns a metric value from a given node.
   These functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Pointer to function specific data.
   @returns the metric value of @a node.
 */
typedef uint64_t
( * scorep_profile_get_uint64_func )( scorep_profile_node* node,
                                      void*                data );

/**
   Defines a function type which returns a metric value from a given node.
   These functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Pointer to function specific data.
   @returns the metric value of @a node.
 */
typedef double
( * scorep_profile_get_doubles_func )( scorep_profile_node* node,
                                       void*                data );


/**
   Defines a function type which returns a metric value from a given node.
   These functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Pointer to function specific data.
   @returns the metric value of @a node.
 */
typedef cube_type_tau_atomic
( * scorep_profile_get_cube_type_tau_atomic_func )( scorep_profile_node* node,
                                                    void*                data );

/**
   Contains a mapping structure between scorep handles and cube handles.
 */
static scorep_cube4_definitions_map* scorep_map;

/**
   Contains a zero element for cube_type_tau_atomic
 */
static const cube_type_tau_atomic
    scorep_cube_type_tau_atomic_zero = { 0, 0.0, 0.0, 0.0, 0.0 };

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
make_callpath_mapping( scorep_profile_node* node,
                       void*                param )
{
    /* Check whether this node has a callpath handle assigned. Thread nodes have none */
    if ( !node->callpath_handle )
    {
        return;
    }

    /* Use the sequence number of the unified definitions as index. */
    uint64_t              index     = SCOREP_CallpathHandle_GetUnifiedId( node->callpath_handle );
    scorep_profile_node** id_2_node = ( scorep_profile_node** )param;

    id_2_node[ index ] = node;
}

/**
   Creates a mapping from global sequence numbers to local metric definitions. The
   global sequence numbers define the order in which the metrics are written.
   @param metricNumber Number of unified definitions.
 */
static SCOREP_MetricHandle*
make_metric_mapping( uint32_t metricNumber )
{
    uint32_t             i;
    SCOREP_MetricHandle* map = malloc( sizeof( SCOREP_MetricHandle ) * metricNumber );
    if ( map == NULL )
    {
        return NULL;
    }
    for ( i = 0; i < metricNumber; i++ )
    {
        map[ i ] = SCOREP_INVALID_METRIC;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager, Metric, metric )
    {
        map[ SCOREP_MetricHandle_GetUnifiedId( handle ) ] = handle;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    return map;
}

/**
   Creates a mapping from global sequence numbers to unified metric definitions. The
   global sequence numbers define the order in which the metrics are written.
   @param metric_number Number of unified definitions.
 */
static SCOREP_MetricHandle*
make_unified_metrics_mapping( uint32_t metric_number )
{
    uint32_t             i   = 0;
    SCOREP_MetricHandle* map = malloc( sizeof( SCOREP_MetricHandle ) * metric_number );
    if ( map == NULL )
    {
        return NULL;
    }

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager, Metric, metric )
    {
        map[ i ] = handle;
        i++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

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
get_sum_time_value( scorep_profile_node* node, void* data )
{
    return ( ( double )node->inclusive_time.sum ) / ( ( double )SCOREP_Timer_GetClockResolution() );
}

/**
   Returns the sum of implicit runtime for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Ignored.
   @returns the implicit runtime of @a node.
 */
static double
get_max_time_value( scorep_profile_node* node, void* data )
{
    return ( ( double )node->inclusive_time.max ) / ( ( double )SCOREP_Timer_GetClockResolution() );
}

/**
   Returns the sum of implicit runtime for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Ignored.
   @returns the implicit runtime of @a node.
 */
static double
get_min_time_value( scorep_profile_node* node, void* data )
{
    return ( ( double )node->inclusive_time.min ) / ( ( double )SCOREP_Timer_GetClockResolution() );
}

/**
   Returns the number of visits for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Ignored.
   @returns the number of visits of @a node.
 */
static uint64_t
get_visits_value( scorep_profile_node* node, void* data )
{
    return node->count;
}



/**
   Returns the number of hits for @a node.
   This functions are given to scorep_profile_write_cube_metric.
   @param node Pointer to a node which should return the metric value.
   @param data Ignored.
   @returns the number of visits of @a node.
 */
static uint64_t
get_hits_value( scorep_profile_node* node, void* data )
{
    return node->hits;
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
get_metrics_value_from_array( scorep_profile_node* node, void* index )
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
get_sparse_uint64_value( scorep_profile_node* node, void* data )
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
            switch ( SCOREP_MetricHandle_GetProfilingType( metric ) )
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
get_sparse_double_value( scorep_profile_node* node, void* data )
{
    SCOREP_MetricHandle                  metric  = *( SCOREP_MetricHandle* )data;
    scorep_profile_sparse_metric_double* current = node->first_double_sparse;

    if ( metric == SCOREP_INVALID_METRIC )
    {
        return 0.0;
    }

    while ( current != NULL )
    {
        if ( current->handle == metric )
        {
            switch ( SCOREP_MetricHandle_GetProfilingType( metric ) )
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
   Returns the sparse double metric value in @a node as tuple.
   This functions are given to scorep_profile_write_cube_metric.
   @param node  Pointer to a node which should return the metric value.
   @param data  Pointer to a metric handle specifying the desired metric.
   @returns the value of the specified metric if it has an entry in the node.
            Otherwise 0 is returned.
 */
static cube_type_tau_atomic
get_sparse_tuple_value_from_double( scorep_profile_node* node, void* data )
{
    SCOREP_MetricHandle                  metric  = *( SCOREP_MetricHandle* )data;
    scorep_profile_sparse_metric_double* current = node->first_double_sparse;
    cube_type_tau_atomic                 value;

    if ( metric == SCOREP_INVALID_METRIC )
    {
        return scorep_cube_type_tau_atomic_zero;
    }

    while ( current != NULL )
    {
        if ( current->handle == metric )
        {
            value.N    = current->count;
            value.Min  = current->min;
            value.Max  = current->max;
            value.Sum  = current->sum;
            value.Sum2 = current->squares;
            return value;
        }
        current = current->next_metric;
    }
    return scorep_cube_type_tau_atomic_zero;
}

/**
   Returns the sparse int metric value in @a node as tuple.
   This functions are given to scorep_profile_write_cube_metric.
   @param node  Pointer to a node which should return the metric value.
   @param data  Pointer to a metric handle specifying the desired metric.
   @returns the value of the specified metric if it has an entry in the node.
            Otherwise 0 is returned.
 */
static cube_type_tau_atomic
get_sparse_tuple_value_from_uint64( scorep_profile_node* node, void* data )
{
    SCOREP_MetricHandle               metric  = *( SCOREP_MetricHandle* )data;
    scorep_profile_sparse_metric_int* current = node->first_int_sparse;
    cube_type_tau_atomic              value;

    if ( metric == SCOREP_INVALID_METRIC )
    {
        return scorep_cube_type_tau_atomic_zero;
    }

    while ( current != NULL )
    {
        if ( current->metric == metric )
        {
            value.N    = current->count;
            value.Min  = current->min;
            value.Max  = current->max;
            value.Sum  = current->sum;
            value.Sum2 = current->squares;
            return value;
        }
        current = current->next_metric;
    }
    return scorep_cube_type_tau_atomic_zero;
}

/**
   Returns a truth value whether a sparse double metric value exists in  @a node.
   This functions are given to set_bitstring_for_metric.
   @param node  Pointer to a node which should return the metric value.
   @param data  Pointer to a metric handle specifying the desired metric.
   @returns the value of the specified metric if it has an entry in the node.
            Otherwise 0 is returned.
 */
static uint64_t
has_sparse_double_value( scorep_profile_node* node, void* data )
{
    SCOREP_MetricHandle metric = *( SCOREP_MetricHandle* )data;
    if ( SCOREP_MetricHandle_GetProfilingType( metric ) == SCOREP_METRIC_PROFILING_TYPE_MAX )
    {
        return 1;
    }

    return ( uint64_t )( get_sparse_double_value( node, data ) == 0 ? 0 : 1 );
}

static void
set_bitstring_for_metric(
    scorep_cube_writing_data*      writeSet,
    scorep_profile_get_uint64_func getValue,
    void*                          funcData )
{
    scorep_profile_node* node = NULL;

    /* Create empty bitstring */
    uint8_t* bits = malloc( SCOREP_Bitstring_GetByteSize( writeSet->callpath_number ) );
    UTILS_ASSERT( bits );
    SCOREP_Bitstring_Clear( bits, writeSet->callpath_number );

    /* Iterate over all unified callpathes */
    for ( uint64_t cp_index = 0; cp_index < writeSet->callpath_number; cp_index++ )
    {
        for ( uint64_t thread_index = 0;
              thread_index < writeSet->local_threads; thread_index++ )
        {
            uint64_t node_index = thread_index * writeSet->callpath_number + cp_index;
            node = writeSet->id_2_node[ node_index ];
            if ( ( node != NULL ) && ( getValue( node, funcData ) != 0 ) )
            {
                SCOREP_Bitstring_Set( bits, cp_index  );
            }
        }
    }

    SCOREP_Ipc_Allreduce( bits, writeSet->bit_vector,
                          ( writeSet->callpath_number + 7 ) / 8,
                          SCOREP_IPC_UNSIGNED_CHAR, SCOREP_IPC_BOR );
    free( bits );
}

/**
   We must ensure that all ranks participate in collective operations. Thus, this
   function is a placeholder for @a set_bitstring_for_metric in case
   a metric is not defined on this rank
 */
static void
set_bitstring_for_unknown_metric( scorep_cube_writing_data* writeSet )
{
    /* Create empty bitstring */
    uint8_t* bits = malloc( SCOREP_Bitstring_GetByteSize( writeSet->callpath_number ) );
    UTILS_ASSERT( bits );
    SCOREP_Bitstring_Clear( bits, writeSet->callpath_number );

    SCOREP_Ipc_Allreduce( bits, writeSet->bit_vector,
                          ( writeSet->callpath_number + 7 ) / 8,
                          SCOREP_IPC_UNSIGNED_CHAR, SCOREP_IPC_BOR );
    free( bits );
}


/* *INDENT-OFF* */

/**
   @def SCOREP_PROFILE_WRITE_CUBE_METRIC
   Code to write metric values in cube format. Used to reduce code replication.
 */
#define SCOREP_PROFILE_WRITE_CUBE_METRIC( type, TYPE, NUMBER, cube_type, zero )         \
static void                                                                             \
write_cube_##cube_type(                                                                 \
                        scorep_cube_writing_data*                 writeSet,             \
                        cube_metric*                              metric,               \
                        scorep_profile_get_ ## cube_type ## _func getValue,             \
                        void*                                     funcData)             \
{                                                                                       \
    scorep_profile_node* node          = NULL;                                          \
    cube_cnode*          cnode         = NULL;                                          \
    type *               local_values  = NULL;                                          \
    type *               global_values = NULL;                                          \
    int                  my_rank       = SCOREP_Ipc_GetRank();                          \
    if ( writeSet->callpath_number == 0 ) return;                                       \
                                                                                        \
    local_values = ( type * )malloc( writeSet->local_threads * sizeof( type ) );        \
                                                                                        \
    if ( writeSet->my_rank == 0 )                                                       \
    {                                                                                   \
        /* Array of all values for one metric for one callpath for all locations */     \
        global_values = ( type * )malloc( writeSet->global_threads * sizeof( type ) );  \
                                                                                        \
        /* Initialize writing of a new metric */                                        \
        cubew_reset( writeSet->cube_writer );                                           \
        cubew_set_array( writeSet->cube_writer, writeSet->callpath_number );            \
        cube_set_known_cnodes_for_metric( writeSet->my_cube, metric,                    \
                                          (char*)writeSet->bit_vector );                \
    }                                                                                   \
    /* Iterate over all unified callpathes */                                           \
    for ( uint64_t cp_index = 0; cp_index < writeSet->callpath_number; cp_index++ )     \
    {                                                                                   \
        if ( ! SCOREP_Bitstring_IsSet( writeSet->bit_vector, cp_index ) )               \
	{                                                                               \
            continue;                                                                   \
        }                                                                               \
        for ( uint64_t thread_index = 0;                                                \
              thread_index < writeSet->local_threads; thread_index ++ )                 \
        {                                                                               \
            uint64_t node_index = thread_index * writeSet->callpath_number + cp_index;  \
            node = writeSet->id_2_node[ node_index ];                                   \
            if ( node != NULL )                                                         \
            {                                                                           \
	        local_values[ thread_index ] = getValue( node, funcData );              \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                local_values[ thread_index ] = zero;                                    \
            }                                                                           \
        }                                                                               \
                                                                                        \
        /* Collect data from all processes */                                           \
        SCOREP_Ipc_Barrier();                                                           \
        if ( writeSet->same_thread_num )                                                \
        {                                                                               \
            SCOREP_Ipc_Gather( local_values, global_values,                             \
                               writeSet->local_threads * NUMBER,                        \
                               SCOREP_IPC_ ## TYPE, 0 );                                \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            SCOREP_Ipc_Gatherv( local_values, writeSet->local_threads * NUMBER,         \
                                global_values, writeSet->items_per_rank,                \
                                SCOREP_IPC_ ## TYPE, 0 );                               \
        }                                                                               \
                                                                                        \
        /* Write data for one callpath */                                               \
        if ( writeSet->my_rank == 0 )                                                   \
        {                                                                               \
	    cnode = cube_get_cnode( writeSet->my_cube, cp_index );                      \
            cube_write_sev_row_of_ ## cube_type( writeSet->my_cube, metric,             \
                                                 cnode, global_values );                \
        }                                                                               \
    }                                                                                   \
                                                                                        \
    /* Clean up */                                                                      \
    free( global_values );                                                              \
    free( local_values );                                                               \
}

/* *INDENT-ON* */

/**
   @function write_cube_uint64
   Writes data for the metric @a metric to a cube object.
   @param writeSet       Structure containing write data.
   @param metric         The cube metric handle for the written metric.
   @param getValue       Functionpointer which returns the value for a given
                         profile node.
   @param funcData       Pointer to data that is passed to the @a get_value function
 */
SCOREP_PROFILE_WRITE_CUBE_METRIC( uint64_t, UINT64_T, 1, uint64, 0 )

/**
   @function write_cube_double
   Writes data for the metric @a metric to a cube object.
   @param writeSet       Structure containing write data.
   @param metric         The cube metric handle for the written metric.
   @param getValue       Functionpointer which returns the value for a given
                         profile node.
   @param funcData       Pointer to data that is passed to the @a get_value function
 */
SCOREP_PROFILE_WRITE_CUBE_METRIC( double, DOUBLE, 1, doubles, 0.0 )

/**
   @function write_cube_cube_type_tau_atomic
   Writes data for the metric @a metric to a cube object.
   @param writeSet       Structure containing write data.
   @param metric         The cube metric handle for the written metric.
   @param getValue       Functionpointer which returns the value for a given
                         profile node.
   @param funcData       Pointer to data that is passed to the @a get_value function
 */
SCOREP_PROFILE_WRITE_CUBE_METRIC( cube_type_tau_atomic, BYTE,
                                  sizeof( cube_type_tau_atomic ),
                                  cube_type_tau_atomic,
                                  scorep_cube_type_tau_atomic_zero )

/**
   Returns true, if sparce metric @a metric is written.
   @param writeSet  Structure containing write data.
   @param metric    Metric handle that is going to be written.
 */
static bool
check_if_metric_shall_be_written( scorep_cube_writing_data* writeSet,
                                  SCOREP_MetricHandle       metric )
{
    return metric != SCOREP_PROFILE_DENSE_METRIC &&
           ( ( metric != scorep_profile_migration_loss_metric &&
               metric != scorep_profile_migration_win_metric ) ||
             writeSet->has_tasks );
}

/* *****************************************************************************
   Handle scorep_cube_writing_data
*******************************************************************************/

/**
   Destroys a scorep_cube_writing_data data structure.
   @param writeSet the data structire that is destroyed.
 */
static void
delete_cube_writing_data( scorep_cube_writing_data* writeSet )
{
    if ( writeSet == NULL )
    {
        return;
    }
    free( writeSet->id_2_node );
    free( writeSet->offsets_per_rank );
    free( writeSet->items_per_rank );
    free( writeSet->metric_map );
    free( writeSet->unified_metric_map );
    free( writeSet->bit_vector );
    if ( writeSet->cube_writer )
    {
        cubew_finalize( writeSet->cube_writer );
    }
    if ( writeSet->map )
    {
        scorep_cube4_delete_definitions_map( writeSet->map );
    }

    writeSet->my_cube            = NULL;
    writeSet->cube_writer        = NULL;
    writeSet->id_2_node          = NULL;
    writeSet->map                = NULL;
    writeSet->items_per_rank     = NULL;
    writeSet->offsets_per_rank   = NULL;
    writeSet->metric_map         = NULL;
    writeSet->unified_metric_map = NULL;
    writeSet->bit_vector         = NULL;
}

/**
   Initializes a scorep_cube_writing_data object.
   @param writeSet    The data structure that is initialized. Must already be allocated.
   @returns whether initialization was successful.
 */
static bool
init_cube_writing_data( scorep_cube_writing_data* writeSet )
{
    /* Set all pointers to zero.
       If an malloc fails, we know how many can bee freed */
    writeSet->my_cube            = NULL;
    writeSet->cube_writer        = NULL;
    writeSet->id_2_node          = NULL;
    writeSet->map                = NULL;
    writeSet->items_per_rank     = NULL;
    writeSet->offsets_per_rank   = NULL;
    writeSet->metric_map         = NULL;
    writeSet->unified_metric_map = NULL;
    writeSet->bit_vector         = NULL;

    /* ------------------------------------ Start initializing */

    /* Get basic MPI data */
    writeSet->my_rank       = SCOREP_Ipc_GetRank();
    writeSet->local_threads = scorep_profile_get_number_of_threads();
    writeSet->ranks_number  = SCOREP_Ipc_GetSize();

    /* Get the number of unified callpath definitions to all ranks */
    if ( writeSet->my_rank == 0 )
    {
        writeSet->callpath_number = SCOREP_Definitions_GetNumberOfUnifiedCallpathDefinitions();
    }
    SCOREP_Ipc_Bcast( &writeSet->callpath_number, 1, SCOREP_IPC_UINT32_T, 0 );
    if ( writeSet->callpath_number == 0 )
    {
        return false;
    }

    /* Calculate the offsets of all ranks and the number of locations per rank */
    if ( writeSet->my_rank == 0 )
    {
        size_t buffer_size = writeSet->ranks_number * sizeof( int );
        writeSet->items_per_rank   = ( int* )malloc( buffer_size );
        writeSet->offsets_per_rank = ( int* )malloc( buffer_size );
    }

    SCOREP_Ipc_Gather( &writeSet->local_threads,
                       writeSet->items_per_rank,
                       1, SCOREP_IPC_UINT32_T, 0 );

    if ( writeSet->my_rank == 0 )
    {
        writeSet->global_threads  = 0;
        writeSet->same_thread_num = 1;
        for ( int32_t i = 0; i < writeSet->ranks_number; ++i )
        {
            if ( writeSet->local_threads != writeSet->items_per_rank[ i ] )
            {
                writeSet->same_thread_num = 0;
            }
            writeSet->offsets_per_rank[ i ] = writeSet->global_threads;
            writeSet->global_threads       += writeSet->items_per_rank[ i ];
        }
    }

    /* Distribute the global number of locations */
    SCOREP_Ipc_Bcast( &writeSet->global_threads, 1, SCOREP_IPC_UINT32_T, 0 );

    /* Distribute whether all ranks have the same number of locations */
    SCOREP_Ipc_Bcast( &writeSet->same_thread_num, 1, SCOREP_IPC_UINT32_T, 0 );

    /* Distribute the per-rank offsets */
    SCOREP_Ipc_Scatter( writeSet->offsets_per_rank,
                        &writeSet->offset,
                        1, SCOREP_IPC_UINT32_T, 0 );

    /* Get number of unified metrics to every rank */
    if ( writeSet->my_rank == 0 )
    {
        writeSet->num_unified_metrics = scorep_unified_definition_manager->metric.counter;
    }
    SCOREP_Ipc_Bcast( &writeSet->num_unified_metrics, 1, SCOREP_IPC_UINT32_T, 0 );

    /* Create the mappings from cube to Score-P handles and vice versa */
    writeSet->map = scorep_cube4_create_definitions_map();
    if ( writeSet->map == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_MEM_ALLOC_FAILED,
                     "Failed to allocate memory for definition mapping\n"
                     "Failed to write Cube 4 profile" );

        delete_cube_writing_data( writeSet );
        return false;
    }

    /* Create the Cube writer object and the Cube object */
    if ( writeSet->my_rank == 0 )
    {
        /* Construct cube file name */
        const char* dirname  = SCOREP_GetExperimentDirName();
        char*       filename = NULL;
        const char* basename = scorep_profile_get_basename();

        filename = ( char* )malloc( strlen( dirname ) +  /* Directory     */
                                    1 +                  /* separator '/' */
                                    strlen( basename )   /* basename      */
                                    + 1 );               /* trailing '\0' */
        if ( filename == NULL )
        {
            UTILS_ERROR_POSIX( "Failed to allocate memory for filename.\n"
                               "Failed to write Cube 4 profile" );
            delete_cube_writing_data( writeSet );
            return false;
        }
        sprintf( filename, "%s/%s", dirname, basename );

        /* Create Cube objects */
        writeSet->cube_writer
            = cubew_create( writeSet->my_rank,        /* rank of this node          */
                            writeSet->global_threads, /* global sum of threads      */
                            1,                        /* number of parallel writers */
                            filename,                 /* base file name             */
                            CUBE_FALSE );             /* no zlib compression        */
        free( filename );

        writeSet->my_cube = cubew_get_cube( writeSet->cube_writer );
    }

    /* Create bit_vector with all bits set. Used for dense metrics */
    writeSet->bit_vector =
        ( uint8_t* )malloc( SCOREP_Bitstring_GetByteSize( writeSet->callpath_number ) );
    UTILS_ASSERT( writeSet->bit_vector );
    SCOREP_Bitstring_SetAll( writeSet->bit_vector, writeSet->callpath_number );

    /* Check whether tasks has been used somewhere */
    int32_t has_tasks = scorep_profile_has_tasks();
    writeSet->has_tasks = 0;
    SCOREP_Ipc_Allreduce( &has_tasks,
                          &writeSet->has_tasks,
                          1,
                          SCOREP_IPC_INT32_T,
                          SCOREP_IPC_BOR );

    return true;
}

static void
add_mapping_to_cube_writing_data( scorep_cube_writing_data* writeSet )
{
    /* Map global sequence numbers to profile nodes */
    writeSet->id_2_node = calloc( writeSet->callpath_number * writeSet->local_threads,
                                  sizeof( scorep_profile_node* ) );

    scorep_profile_node* node = scorep_profile.first_root_node;
    for ( uint64_t i = 0; node != NULL; node = node->next_sibling )
    {
        scorep_profile_for_all( node,
                                make_callpath_mapping,
                                &writeSet->id_2_node[ i ] );
        i += writeSet->callpath_number;
    }

    /* Mapping from global sequence number to local metric handle. Defines
       order of writing metrics */
    writeSet->metric_map = make_metric_mapping( writeSet->num_unified_metrics );

    if ( writeSet->my_rank == 0 )
    {
        writeSet->unified_metric_map = make_unified_metrics_mapping( writeSet->num_unified_metrics );
    }
}

/* *****************************************************************************
   Main writer function
*******************************************************************************/
void
scorep_profile_write_cube4( bool writeTuples )
{
    /*-------------------------------- Variable definition */

    /* Pointer to Cube 4 metric definition. Only used on Rank 0 */
    cube_metric* metric = NULL;

    /* Data set for Cube write functions */
    scorep_cube_writing_data write_set;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                        "Writing profile in Cube 4 format ..." );

    /* -------------------------------- Initialization, header and definitions */

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Prepare writing" );

    if ( !init_cube_writing_data( &write_set ) )
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
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing definitions" );
        scorep_write_definitions_to_cube4( write_set.my_cube,
                                           write_set.map,
                                           write_set.ranks_number,
                                           write_set.global_threads,
                                           write_set.has_tasks,
                                           writeTuples );

        if ( SCOREP_IsUnwindingEnabled() )
        {
            /*
             * Record the number of sampling related definitions in the profile.
             * The names directly correspond to the OTF2 definition names.
             */
            char buffer[ 32 ];
            sprintf( buffer, "%u", scorep_unified_definition_manager->calling_context.counter );
            cube_def_attr( write_set.my_cube, "Score-P::DefinitionCounters::CallingContext", buffer );
            sprintf( buffer, "%u", scorep_unified_definition_manager->interrupt_generator.counter );
            cube_def_attr( write_set.my_cube, "Score-P::DefinitionCounters::InterruptGenerator", buffer );
        }
    }

    /* Build mapping from sequence number in unified callpath definitions to
       profile nodes */
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Create mappings" );
    add_mapping_to_cube_writing_data( &write_set );

    /* Write clustering mappings */
    scorep_cluster_write_cube4( &write_set );

    /* -------------------------------- dense metrics */

    /* Write implicit time and visits */
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing runtime" );

    write_cube_doubles( &write_set, scorep_get_sum_time_handle(),
                        &get_sum_time_value, NULL );

    write_cube_doubles( &write_set, scorep_get_max_time_handle(),
                        &get_max_time_value, NULL );

    write_cube_doubles( &write_set, scorep_get_min_time_handle(),
                        &get_min_time_value, NULL );

    write_cube_uint64( &write_set, scorep_get_visits_handle(),
                       &get_visits_value, NULL );

    if ( SCOREP_IsUnwindingEnabled() )
    {
        write_cube_uint64( &write_set, scorep_get_hits_handle(),
                           &get_hits_value, NULL );
    }

    /* Write additional dense metrics (e.g. hardware counters) */
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing dense metrics" );
    for ( uint8_t i = 0; i  < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
    {
        cube_metric*        metric        = NULL; /* Only used on rank 0 */
        SCOREP_MetricHandle metric_handle = SCOREP_Metric_GetStrictlySynchronousMetricHandle( i );

        if ( write_set.my_rank == 0 )
        {
            metric = scorep_get_cube4_metric( write_set.map,
                                              SCOREP_MetricHandle_GetUnified( metric_handle ) );
        }

        /* When writing sparse metrics, we skip the time metric handles.
           Thus, invalidate these entries to avoid
           writing them twice. */
        if ( write_set.metric_map != NULL )
        {
            uint32_t current_number =
                SCOREP_MetricHandle_GetUnifiedId( metric_handle );
            write_set.metric_map[ current_number ] = SCOREP_PROFILE_DENSE_METRIC;
        }

        write_cube_uint64( &write_set, metric,
                           &get_metrics_value_from_array, &i );
    }

    /* -------------------------------- sparse metrics */

    /* Update offsets and items_per_rank if we write tuples */
    if ( ( write_set.my_rank == 0 ) && writeTuples )
    {
        for ( uint32_t i = 0; i < write_set.ranks_number; i++ )
        {
            write_set.items_per_rank[ i ]   *= sizeof( cube_type_tau_atomic );
            write_set.offsets_per_rank[ i ] *= sizeof( cube_type_tau_atomic );
        }
    }


    /* Write sparse metrics (e.g. user metrics) */
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Writing sparse metrics" );
    if ( write_set.metric_map != NULL )
    {
        cube_metric* metric = NULL; /* Only used on rank 0 */

        for ( uint32_t i = 0; i < write_set.num_unified_metrics; i++ )
        {
            if ( !check_if_metric_shall_be_written( &write_set,
                                                    write_set.metric_map[ i ] ) )
            {
                continue;
            }

            if ( write_set.my_rank == 0 )
            {
                metric = scorep_get_cube4_metric( write_set.map,
                                                  write_set.unified_metric_map[ i ] );
            }

            if ( write_set.metric_map[ i ] == SCOREP_INVALID_METRIC )
            {
                set_bitstring_for_unknown_metric( &write_set );
                if ( writeTuples )
                {
                    write_cube_cube_type_tau_atomic( &write_set,
                                                     metric,
                                                     &get_sparse_tuple_value_from_double,
                                                     &write_set.metric_map[ i ] );
                }
                else
                {
                    write_cube_doubles( &write_set,
                                        metric,
                                        &get_sparse_double_value,
                                        &write_set.metric_map[ i ] );
                }
                continue;
            }

            switch ( SCOREP_MetricHandle_GetValueType( write_set.metric_map[ i ] ) )
            {
                case SCOREP_METRIC_VALUE_INT64:
                case SCOREP_METRIC_VALUE_UINT64:
                    set_bitstring_for_metric( &write_set,
                                              &get_sparse_uint64_value,
                                              &write_set.metric_map[ i ] );
                    if ( writeTuples )
                    {
                        write_cube_cube_type_tau_atomic( &write_set,
                                                         metric,
                                                         &get_sparse_tuple_value_from_uint64,
                                                         &write_set.metric_map[ i ] );
                    }
                    else
                    {
                        write_cube_uint64( &write_set,
                                           metric,
                                           &get_sparse_uint64_value,
                                           &write_set.metric_map[ i ] );
                    }

                    break;
                case SCOREP_METRIC_VALUE_DOUBLE:
                    set_bitstring_for_metric( &write_set,
                                              &has_sparse_double_value,
                                              &write_set.metric_map[ i ] );
                    if ( writeTuples )
                    {
                        write_cube_cube_type_tau_atomic( &write_set,
                                                         metric,
                                                         &get_sparse_tuple_value_from_double,
                                                         &write_set.metric_map[ i ] );
                    }
                    else
                    {
                        write_cube_doubles( &write_set,
                                            metric,
                                            &get_sparse_double_value,
                                            &write_set.metric_map[ i ] );
                    }

                    break;
                default:
                    UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                                 "Metric %s has unknown value type %d",
                                 SCOREP_MetricHandle_GetName( write_set.metric_map[ i ] ),
                                 SCOREP_MetricHandle_GetValueType( write_set.metric_map[ i ] ) );
            }
        }
    }
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile writing done" );

    /* Clean up */
    //cleanup:
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Clean up" );
    delete_cube_writing_data( &write_set );
}
