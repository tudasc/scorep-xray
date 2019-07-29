/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015-2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * profile tree.
 *
 *
 *
 */

#include <config.h>
#include <SCOREP_Profile.h>

#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Unwinding.h>
#include <scorep_location_management.h>
#include <scorep_type_utils.h>

#define SCOREP_DEBUG_MODULE_NAME PROFILE
#include <UTILS_Debug.h>
#include <UTILS_IO.h>

#include "scorep_profile_node.h"
#include "scorep_profile_definition.h"
#include "scorep_profile_cluster.h"
#include "scorep_profile_mpi_events.h"
#include "scorep_profile_location.h"
#include "scorep_profile_process.h"
#include "scorep_profile_writer.h"
#include "scorep_profile_event_base.h"
#include <SCOREP_Substrates_Management.h>
#include <SCOREP_RuntimeManagement.h>
#include "scorep_profile_task_init.h"
#include <SCOREP_Profile_MpiEvents.h>
#include <SCOREP_Thread_Mgmt.h>
#include <scorep_profile_io.h>

#include <string.h>

/* ***************************************************************************************
   Type definitions and variables
*****************************************************************************************/

/**
   Mutex for exclusive execution when adding a new location to the profile.
 */
static SCOREP_Mutex scorep_profile_location_mutex;

static SCOREP_RegionHandle thread_create_wait_regions;

static SCOREP_MetricHandle bytes_allocated_metric           = SCOREP_INVALID_METRIC;
static SCOREP_MetricHandle bytes_freed_metric               = SCOREP_INVALID_METRIC;
static SCOREP_MetricHandle bytes_leaked_metric              = SCOREP_INVALID_METRIC;
static SCOREP_MetricHandle max_heap_memory_allocated_metric = SCOREP_INVALID_METRIC;

SCOREP_ParameterHandle scorep_profile_param_instance = SCOREP_INVALID_PARAMETER;

/****************************************************************************************
   internal helper functions
 *****************************************************************************************/

/* *INDENT-OFF* */
static void parameter_uint64( SCOREP_Location* thread, uint64_t timestamp, SCOREP_ParameterHandle param, uint64_t value );

static void write_sparse_metrics_enter( SCOREP_Location* location, uint64_t timestamp, SCOREP_SamplingSetHandle samplingSet, const uint64_t* values );
static void write_sparse_metrics_exit( SCOREP_Location* location, uint64_t timestamp, SCOREP_SamplingSetHandle samplingSet, const uint64_t* values );
/* *INDENT-ON* */

static inline void
setup_start_from_parent( scorep_profile_node* node )
{
    scorep_profile_node* parent = node->parent;
    int                  i;

    /* Correct first start time, on first enter */
    if ( node->first_enter_time == -1 )
    {
        node->first_enter_time = parent->inclusive_time.start_value;
    }

    /* Store start values for dense metrics */
    node->count++;
    if ( parent != NULL )
    {
        /* If no enclosing region is present, no dense metric values can be associated */
        node->inclusive_time.start_value = parent->inclusive_time.start_value;
        for ( i = 0; i < SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics(); i++ )
        {
            node->dense_metrics[ i ].start_value = parent->dense_metrics[ i ].start_value;
        }
    }
}


/* ***************************************************************************************
   Initialization / Finalization
*****************************************************************************************/

#include "scorep_profile_confvars.inc.c"

void
SCOREP_Profile_Initialize( size_t substrateId )
{
    UTILS_DEBUG_ENTRY();

    if ( scorep_profile.is_initialized )
    {
        return;
    }
    scorep_profile_substrate_id = substrateId;

    SCOREP_MutexCreate( &scorep_profile_location_mutex );

    scorep_cluster_initialize();
    scorep_profile_init_definition();
    scorep_profile_initialize_exchange();
    scorep_profile_task_initialize();
    scorep_profile_init_rma();
    scorep_profile_io_init();

    if ( !scorep_profile.reinitialize )
    {
        scorep_profile_param_instance =
            SCOREP_Definitions_NewParameter( "instance", SCOREP_PARAMETER_INT64 );
    }
    else
    {
        /* Reallocate space for dense metrics on root nodes */
        uint32_t num_dense_metrics = SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics();

        uint32_t size = num_dense_metrics * sizeof( scorep_profile_dense_metric );

        scorep_profile_node* current = scorep_profile.first_root_node;
        while ( current != NULL )
        {
            SCOREP_Profile_LocationData* profile_location =
                scorep_profile_type_get_location_data( current->type_specific_data );
            scorep_profile_reinitialize_location( profile_location );
            if ( num_dense_metrics > 0 )
            {
                current->dense_metrics = ( scorep_profile_dense_metric* )
                                         SCOREP_Location_AllocForProfile( profile_location->location_data, size );

                scorep_profile_init_dense_metric( &current->inclusive_time );
                scorep_profile_init_dense_metric_array( current->dense_metrics,
                                                        num_dense_metrics );
            }
            current = current->next_sibling;
        }
    }

    UTILS_ASSERT( scorep_profile_param_instance );

    SCOREP_SourceFileHandle file = SCOREP_Definitions_NewSourceFile( "THREADS" );
    thread_create_wait_regions = SCOREP_Definitions_NewRegion(
        "THREADS",
        NULL,
        file,
        SCOREP_INVALID_LINE_NO,
        SCOREP_INVALID_LINE_NO,
        SCOREP_PARADIGM_MEASUREMENT,
        SCOREP_REGION_ARTIFICIAL /* SCOREP_REGION_PARALLEL ? */ );

    bytes_allocated_metric =
        SCOREP_Definitions_NewMetric( "ALLOCATION_SIZE",
                                      "Size of allocated heap memory",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                      SCOREP_INVALID_METRIC );

    bytes_freed_metric =
        SCOREP_Definitions_NewMetric( "DEALLOCATION_SIZE",
                                      "Size of released heap memory",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                      SCOREP_INVALID_METRIC );

    bytes_leaked_metric =
        SCOREP_Definitions_NewMetric( "bytes_leaked",
                                      "Size of allocated heap memory that was not released",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE,
                                      SCOREP_INVALID_METRIC );

    max_heap_memory_allocated_metric =
        SCOREP_Definitions_NewMetric( "maximum_heap_memory_allocated",
                                      "Maximum amount of heap memory allocated at a time",
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_POINT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_MAX,
                                      SCOREP_INVALID_METRIC );
}


size_t
SCOREP_Profile_Finalize( void )
{
    UTILS_DEBUG_ENTRY();

    scorep_profile_node*         current     = scorep_profile.first_root_node;
    SCOREP_Profile_LocationData* thread_data = NULL;

    /* Update all root nodes which survive a finalize, because locations are not
       reinitialized. Assume that the siblings of scorep_profile.first_root_node
       are all of type SCOREP_PROFILE_NODE_THREAD_ROOT. */
    while ( current != NULL )
    {
        if ( current->node_type == SCOREP_PROFILE_NODE_THREAD_ROOT )
        {
            /* Cut off children */
            current->first_child = NULL;

            /* Metrics are freed, too */
            current->dense_metrics       = NULL;
            current->first_double_sparse = NULL;
            current->first_int_sparse    = NULL;

            /* Reset thread local storage */
            thread_data = scorep_profile_type_get_location_data( current->type_specific_data );
            scorep_profile_finalize_location( thread_data );
        }
        else
        {
            UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT, "Root node of wrong type %d",
                         current->node_type );
        }

        /* Process next */
        current = current->next_sibling;
    }

    /* Reset profile definition struct */
    scorep_profile_delete_definition();

    /* Finalize sub-systems */
    scorep_cluster_finalize();
    scorep_profile_finalize_exchange();
    scorep_profile_io_finalize();

    /* Delete mutex */
    SCOREP_MutexDestroy( &scorep_profile_location_mutex );
    return scorep_profile_substrate_id;
}


/**
 * Enables profile recording. This function activates profile recording.
 * @ref enable_recording.
 *
 * @param location     A pointer to the thread location data of the thread that executed
 *                     the enable recording event.
 * @param timestamp    The timestamp, when the enable recording event occurred.
 * @param regionHandle The handle of the region for which the enable recording occurred.
 * @param metricValues Array of the dense metric values.
 */
static void
enable_recording( SCOREP_Location*    location,
                  uint64_t            timestamp,
                  SCOREP_RegionHandle regionHandle,
                  uint64_t*           metricValues )
{
    SCOREP_Profile_Exit( location,
                         timestamp,
                         regionHandle,
                         metricValues );
}


/**
 * Disables profile recording. This function deactivates profile recording.
 * @ref disable_recording.
 *
 * @param location     A pointer to the thread location data of the thread that executed
 *                     the disable recording event.
 * @param timestamp    The timestamp, when the disable recording event occurred.
 * @param regionHandle The handle of the region for which the disable recording occurred.
 * @param metricValues Array of the dense metric values.
 */
static void
disable_recording( SCOREP_Location*    location,
                   uint64_t            timestamp,
                   SCOREP_RegionHandle regionHandle,
                   uint64_t*           metricValues )
{
    SCOREP_Profile_Enter( location,
                          timestamp,
                          regionHandle,
                          metricValues );
}


/**
 * Triggered on location creation, i.e. when a location is encountered the first
 * time. Note that several threads can share the same location data.
 *
 * @param locationData       Location data of the current thread.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
static void
on_location_creation( SCOREP_Location* locationData,
                      SCOREP_Location* parentLocationData )
{
    SCOREP_Profile_LocationData* parent_data = NULL;
    SCOREP_Profile_LocationData* thread_data = NULL;
    scorep_profile_node*         node        = NULL;
    uint64_t                     thread_id   = 0;
    scorep_profile_type_data_t   node_data;
    memset( &node_data, 0, sizeof( node_data ) );

    SCOREP_Profile_LocationData* profile_data =
        scorep_profile_create_location_data( locationData );
    UTILS_BUG_ON( !profile_data, "Failed creating profile location data." );
    SCOREP_Location_SetSubstrateData( locationData, profile_data,
                                      scorep_profile_substrate_id );

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: Create Location" );

    /* Initialize type specific data structure */
    thread_data = scorep_profile_get_profile_data( locationData );
    thread_id   = SCOREP_Location_GetId( locationData );
    UTILS_ASSERT( thread_data != NULL );

    scorep_profile_type_set_location_data( &node_data, thread_data );
    scorep_profile_type_set_int_value( &node_data, thread_id );

    /* Create thread root node */
    node = scorep_profile_create_node( thread_data,
                                       NULL,
                                       SCOREP_PROFILE_NODE_THREAD_ROOT,
                                       node_data, 0,
                                       SCOREP_PROFILE_TASK_CONTEXT_TIED );

    /* Disable profiling if node creation failed */
    if ( node == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Failed to create location" );
        SCOREP_PROFILE_STOP( thread_data );
        return;
    }

    /* Update thread location data */
    thread_data->root_node = node;

    if ( parentLocationData != NULL )
    {
        parent_data                = scorep_profile_get_profile_data( parentLocationData );
        thread_data->creation_node = NULL;
        thread_data->current_depth = 0;
    }

    /* Add it to the profile node list */
    if ( parent_data == NULL )
    {
        /* It is the initial thread. Insert as first new root node. */
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Initial location created" );

        SCOREP_MutexLock( scorep_profile_location_mutex );
        node->next_sibling             = scorep_profile.first_root_node;
        scorep_profile.first_root_node = node;
        SCOREP_MutexUnlock( scorep_profile_location_mutex );
    }
    else
    {
        /* Append after parent root node */
        SCOREP_MutexLock( scorep_profile_location_mutex );
        node->next_sibling                   = parent_data->root_node->next_sibling;
        parent_data->root_node->next_sibling = node;
        SCOREP_MutexUnlock( scorep_profile_location_mutex );
    }

    /* Make the root node the current node of the location.
       It allows to use the location without an activation, e.g.,
       for non-CPU-locations. */
    scorep_profile_set_current_node( thread_data, node );

    /* Hack: To display MIN/MAX Cube metrics from the unique per-process-metrics
     * location (see SCOREP_Location_AcquirePerProcessMetricsLocation()) we need
     * to enter an artificial region. Identify the location by comparing type
     * and name. */
    if ( SCOREP_Location_GetType( locationData ) == SCOREP_LOCATION_TYPE_METRIC )
    {
        extern char scorep_per_process_metrics_location_name[];
        const char* name = SCOREP_Location_GetName( locationData );
        if ( strncmp( name, scorep_per_process_metrics_location_name,
                      strlen( scorep_per_process_metrics_location_name ) ) == 0 )
        {
            static SCOREP_RegionHandle per_process_metrics;
            static bool                first_visit = true;
            if ( first_visit )
            {
                /* Define the artificial region only once */
                first_visit = false;
                SCOREP_SourceFileHandle file = SCOREP_Definitions_NewSourceFile( "PER PROCESS METRICS" );
                per_process_metrics = SCOREP_Definitions_NewRegion(
                    "PER PROCESS METRICS",
                    NULL,
                    file,
                    SCOREP_INVALID_LINE_NO,
                    SCOREP_INVALID_LINE_NO,
                    SCOREP_PARADIGM_MEASUREMENT,
                    SCOREP_REGION_ARTIFICIAL );
            }

            /* Enter the artificial program region */
            scorep_profile_type_data_t program_region_data;
            memset( &program_region_data, 0, sizeof( program_region_data ) );
            scorep_profile_type_set_region_handle( &program_region_data, SCOREP_GetProgramRegion() );
            scorep_profile_node* program_root;
            program_root = scorep_profile_find_create_child( thread_data,
                                                             node,
                                                             SCOREP_PROFILE_NODE_REGULAR_REGION,
                                                             program_region_data,
                                                             0 /* timestamp */ );

            /* Enter the artificial metric region */
            scorep_profile_type_data_t type_data;
            memset( &type_data, 0, sizeof( type_data ) );
            scorep_profile_type_set_region_handle( &type_data, per_process_metrics );
            node = scorep_profile_create_node( thread_data,
                                               program_root,
                                               SCOREP_PROFILE_NODE_REGULAR_REGION,
                                               type_data,
                                               0 /* timestamp */, false );

            scorep_profile_add_child( program_root, node );
            scorep_profile_set_current_node( thread_data, node );
        }
    }
}


/**
 * Clean up the location specific profile data at the end of a phase or at the
 * end of the measurement.
 *
 * @param location The location object holding the profile data to be deleted
 */
static void
delete_location_data( SCOREP_Location* location )
{
    SCOREP_Memory_FreeProfileMem( location );
    scorep_profile_delete_location_data( scorep_profile_get_profile_data( location ) );
}


/**
 * Triggered at the start of every thread/parallel region. Always triggered,
 * even after thread creation. In contrast to creation this function may be
 * triggered multiple times, e.g. if we reenter a parallel region again or if
 * we reuse the location/thread in a different parallel region.
 *
 * @param locationData       Location data of the current thread inside the parallel
 * region.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 * @param forkSequenceCount  The forkSequenceCount of the activated thread.
 */
static void
on_location_activation( SCOREP_Location* locationData,
                        SCOREP_Location* parentLocationData,
                        uint32_t         forkSequenceCount )
{
    SCOREP_Profile_LocationData* thread_data    = NULL;
    SCOREP_Profile_LocationData* parent_data    = NULL;
    scorep_profile_node*         root           = NULL;
    scorep_profile_node*         node           = NULL;
    scorep_profile_node*         creation_point = NULL;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: Activated thread" );

    SCOREP_PROFILE_ASSURE_INITIALIZED;
    UTILS_ASSERT( locationData != NULL );

    /* Get root node of the thread */
    thread_data = scorep_profile_get_profile_data( locationData );
    if ( thread_data == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Thread activated which was not created." );
        SCOREP_PROFILE_STOP( thread_data );
        return;
    }
    root = thread_data->root_node;
    UTILS_ASSERT( root != NULL );

    /* Find creation point if available */
    if ( parentLocationData != NULL )
    {
        parent_data = scorep_profile_get_profile_data( parentLocationData );
        if ( parent_data != NULL )
        {
            creation_point             = scorep_profile_get_fork_node( parent_data, forkSequenceCount );
            thread_data->current_depth = scorep_profile_get_fork_depth( parent_data, forkSequenceCount );
        }
    }

    /* Check wether such an activation node already exists */
    node = root->first_child;
    while ( ( node != NULL ) &&
            ( ( node->node_type != SCOREP_PROFILE_NODE_THREAD_START ) ||
              ( creation_point != scorep_profile_type_get_fork_node( node->type_specific_data ) ) ) )
    {
        node = node->next_sibling;
    }

    /* Create new node if no one exists */
    if ( node == NULL )
    {
        scorep_profile_type_data_t data;
        memset( &data, 0, sizeof( data ) );
        scorep_profile_type_set_fork_node( &data, creation_point );
        node = scorep_profile_create_node( thread_data,
                                           root,
                                           SCOREP_PROFILE_NODE_THREAD_START,
                                           data, 0,
                                           SCOREP_PROFILE_TASK_CONTEXT_TIED );

        /* Disable profiling if node creation failed */
        if ( node == NULL )
        {
            UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                         "Failed to create location" );
            SCOREP_PROFILE_STOP( thread_data );
            return;
        }

        node->next_sibling = root->first_child;
        root->first_child  = node;
    }

    /* Now node points to the starting point of the thread.
       Make it the current node of the thread. */
    scorep_profile_set_current_node( thread_data, node );
}


/**
 * Triggered after the end of every thread/parallel region, i.e. in the join
 * event.
 *
 * @param locationData Location data of the deactivated thread inside the
 * parallel region.
 * @param parentLocationData Location data of the parent thread, may equal @a
 * locationData.
 */
static void
on_location_deactivation( SCOREP_Location* locationData,
                          SCOREP_Location* parentLocationData )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: Deactivated thread" );

    /* Remove the current node. */
    scorep_profile_set_current_node( scorep_profile_get_profile_data( locationData ),
                                     NULL );
}


void
SCOREP_Profile_Process( void )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Exit all regions that are not exited, yet. We assume that we post-process
       only when we are outside of a parallel region. Thus, we only exit
       regions on the main location.
     */
    SCOREP_Location*     location  = SCOREP_Location_GetCurrentCPULocation();
    uint64_t             exit_time = SCOREP_Timer_GetClockTicks();
    scorep_profile_node* node      = NULL;
    uint64_t*            metrics   = SCOREP_Metric_Read( location );

    if ( location != NULL )
    {
        do
        {
            node = scorep_profile_get_current_node( scorep_profile_get_profile_data( location ) );
            while ( ( node != NULL ) &&
                    ( node->node_type != SCOREP_PROFILE_NODE_REGULAR_REGION ) &&
                    ( node->node_type != SCOREP_PROFILE_NODE_COLLAPSE ) )
            {
                node = node->parent;
            }
            if ( node == NULL )
            {
                break;
            }

            if ( node->node_type == SCOREP_PROFILE_NODE_REGULAR_REGION )
            {
                SCOREP_RegionHandle region =
                    scorep_profile_type_get_region_handle( node->type_specific_data );
                fprintf( stderr, "Warning: Force exit for region %s\n",
                         SCOREP_RegionHandle_GetName( region ) );
                SCOREP_Profile_Exit( location, exit_time, region, metrics );
            }
            else if ( node->node_type == SCOREP_PROFILE_NODE_COLLAPSE )
            {
                fprintf( stderr, "Warning: Force exit from collapsed node\n" );
                SCOREP_Profile_Exit( location, exit_time,
                                     SCOREP_INVALID_REGION, metrics );
            }
            else
            {
                break;
            }
        }
        while ( node != NULL );
    }

    /* Substitute collapse nodes by normal region nodes */
    scorep_profile_process_collapse();

    /* Reenumerate clusters */
    scorep_cluster_postprocess();

    /* Reorganize parameter nodes */
    if ( scorep_profile_output_format != SCOREP_PROFILE_OUTPUT_TAU_SNAPSHOT )
    {
        scorep_profile_process_parameters();
    }

    /* Thread start node expansion */
    scorep_profile_expand_threads();
    scorep_profile_sort_threads();
    scorep_profile_process_tasks();

    /* Make phases to the root of separate trees */
    scorep_profile_process_phases();

    /* Register callpath and assign callpath handles to every node */
    scorep_profile_assign_callpath_to_master();
    scorep_profile_assign_callpath_to_workers();

    /* Perform clustering */
    if ( scorep_profile_output_format == SCOREP_PROFILE_OUTPUT_KEY_THREADS )
    {
        scorep_profile_cluster_key_threads();
    }

    if ( scorep_profile_output_format == SCOREP_PROFILE_OUTPUT_CLUSTER_THREADS )
    {
        scorep_profile_init_num_threads_metric();
        scorep_profile_cluster_same_location();
    }
}


/**
   Writes the Profile. The output format can be set via environment variable
   SCOREP_PROFILING_FORMAT. Possible values are None, TauSnapshot, Cube4, Default.
   Should be called after unification.
 */
static void
write( void )
{
    if ( scorep_profile_output_format == SCOREP_PROFILE_OUTPUT_NONE )
    {
        return;
    }
    else if ( scorep_profile_output_format == SCOREP_PROFILE_OUTPUT_TAU_SNAPSHOT )
    {
        scorep_profile_write_tau_snapshot();
    }
    else
    {
        scorep_profile_write_cube4( scorep_profile_output_format );
    }
}


/* ***************************************************************************************
   Callpath events
*****************************************************************************************/


void
SCOREP_Profile_Enter( SCOREP_Location*    thread,
                      uint64_t            timestamp,
                      SCOREP_RegionHandle region,
                      uint64_t*           metrics )
{
    //printf( "%u: Enter %s\n", SCOREP_Location_GetId( thread ), SCOREP_RegionHandle_GetName( region ) );
    scorep_profile_node* node = NULL;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                        "Enter event of profiling system called" );

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Check wether we exceed the depth */
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( thread );
    location->current_depth++;

    /* Enter on current position */
    scorep_profile_node* current_node = scorep_profile_get_current_node( location );
    SCOREP_RegionType    type         = SCOREP_RegionHandle_GetType( region );
    node = scorep_profile_enter( location,
                                 current_node,
                                 region,
                                 type,
                                 timestamp,
                                 metrics );
    UTILS_ASSERT( node != NULL );

    /* Update current node pointer */
    scorep_profile_set_current_node( location, node );

    /* If dynamic region call trigger parameter */
    switch ( type )
    {
        case SCOREP_REGION_DYNAMIC:
        case SCOREP_REGION_DYNAMIC_PHASE:
        case SCOREP_REGION_DYNAMIC_LOOP:
        case SCOREP_REGION_DYNAMIC_FUNCTION:
        case SCOREP_REGION_DYNAMIC_LOOP_PHASE:

            /* Identify cluster regions */
            scorep_cluster_on_enter_dynamic( location, node );

            /* For Dynamic Regions we use a special "instance" parameter defined
             * during initialization */
            parameter_uint64( thread, 0,
                              scorep_profile_param_instance,
                              node->count );
    }
}


void
SCOREP_Profile_Exit( SCOREP_Location*    thread,
                     uint64_t            timestamp,
                     SCOREP_RegionHandle region,
                     uint64_t*           metrics )
{
    //printf( "%u: Exit %s\n", SCOREP_Location_GetId( thread ), SCOREP_RegionHandle_GetName( region ) );
    int                          i;
    scorep_profile_node*         node   = NULL;
    scorep_profile_node*         parent = NULL;
    SCOREP_Profile_LocationData* location;
    SCOREP_RegionType            type = SCOREP_RegionHandle_GetType( region );

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                        "Exit event of profiling system called" );

    SCOREP_PROFILE_ASSURE_INITIALIZED;
    location = scorep_profile_get_profile_data( thread );

    /* Store task metrics if exiting a parallel region */
    if ( type == SCOREP_REGION_PARALLEL )
    {
        scorep_profile_update_task_metrics( location );
    }

    /* Get current node */
    UTILS_ASSERT( location != NULL );

    node = scorep_profile_get_current_node( location );
    UTILS_ASSERT( node != NULL );
    parent = scorep_profile_exit( location, node, region, timestamp, metrics );

    /* Check, if clustering has to be done */
    switch ( type )
    {
        case SCOREP_REGION_DYNAMIC:
        case SCOREP_REGION_DYNAMIC_PHASE:
        case SCOREP_REGION_DYNAMIC_LOOP:
        case SCOREP_REGION_DYNAMIC_FUNCTION:
        case SCOREP_REGION_DYNAMIC_LOOP_PHASE:
            scorep_cluster_if_necessary( location, node );
            break;
    }

    /* Update current node */
    scorep_profile_set_current_node( location, parent );
}


static void
program_begin( SCOREP_Location*     location,
               uint64_t             timestamp,
               SCOREP_StringHandle  programName,
               uint32_t             numberOfProgramArgs,
               SCOREP_StringHandle* programArguments,
               SCOREP_RegionHandle  programRegionHandle )
{
    uint64_t* metric_values = SCOREP_Metric_Read( location );
    SCOREP_Profile_Enter( location,
                          timestamp,
                          SCOREP_GetProgramRegion(),
                          metric_values );
    SCOREP_Metric_WriteSynchronousMetrics( location, timestamp, write_sparse_metrics_enter );
    scorep_profile_number_of_program_args = numberOfProgramArgs;
}


static void
program_end( SCOREP_Location*    location,
             uint64_t            timestamp,
             SCOREP_ExitStatus   exitStatus,
             SCOREP_RegionHandle programRegionHandle )
{
    uint64_t* metric_values = SCOREP_Metric_Read( location );
    SCOREP_Metric_WriteSynchronousMetrics( location, timestamp, write_sparse_metrics_exit );
    SCOREP_Profile_Exit( location,
                         timestamp,
                         SCOREP_GetProgramRegion(),
                         metric_values );
    if ( SCOREP_Metric_GetNumberOfStrictlySynchronousMetrics() > 0 &&
         !SCOREP_Thread_IsIntialThread() )
    {
        UTILS_WARNING( "Program ended on a thread different from the intial thread. "
                       "Per thread metric values for the program region (%s) might be invalid.",
                       SCOREP_RegionHandle_GetName( programRegionHandle ) );
    }
}


/**
   Called on enter events to update the profile accordingly.
   @param location     A pointer to the thread location data of the thread that executed
                       the enter event.
   @param timestamp    The timestamp, when the region was entered.
   @param regionHandle The handle of the entered region.
   @param metricValues An array with metric samples which were taken on the enter event.
                       The samples must be in the same order as the metric definitions
                       at the @ref SCOREP_Profile_Initialize call.
 */
static void
enter_region( SCOREP_Location*    location,
              uint64_t            timestamp,
              SCOREP_RegionHandle regionHandle,
              uint64_t*           metricValues )
{
    SCOREP_Profile_Enter( location,
                          timestamp,
                          regionHandle,
                          metricValues );

    SCOREP_Metric_WriteSynchronousMetrics( location, timestamp, write_sparse_metrics_enter );
}


/**
   Called on exit events to update the profile accordingly.
   @param location     A pointer to the thread location data of the thread that executed
                       the exit event.
   @param timestamp    The timestamp, when the region was left.
   @param regionHandle The handle of the left region.
   @param metricValues An array with metric samples which were taken on the exit event.
                       The samples must be in the same order as the metric definitions
                       at the @ref SCOREP_Profile_Initialize call.
 */
static void
exit_region( SCOREP_Location*    location,
             uint64_t            timestamp,
             SCOREP_RegionHandle regionHandle,
             uint64_t*           metricValues )
{
    SCOREP_Metric_WriteSynchronousMetrics( location, timestamp, write_sparse_metrics_exit );

    SCOREP_Profile_Exit( location,
                         timestamp,
                         regionHandle,
                         metricValues );
}


/**
   Called on samples to update the profile accordingly.
   @param thread                        A pointer to the thread location data of
                                        the thread that executed the exit event.
   @param timestamp                     The timestamp, when the region was left.
   @param currentCallingContextHandle   Current calling context
   @param previousCallingContextHandle  Last known calling context
   @param unwindDistance                Unwind distance
   @param interruptGeneratorHandle      Interrupt generator of this sample
   @param metrics                       An array with metric samples which were taken on the sample event.
                                        The samples must be in the same order as the metric definitions
                                        at the @ref SCOREP_Profile_Initialize call.
 */
static void
sample( SCOREP_Location*                thread,
        uint64_t                        timestamp,
        SCOREP_CallingContextHandle     currentCallingContextHandle,
        SCOREP_CallingContextHandle     previousCallingContextHandle,
        uint32_t                        unwindDistance,
        SCOREP_InterruptGeneratorHandle interruptGeneratorHandle,
        uint64_t*                       metrics )
{
    SCOREP_Unwinding_ProcessCallingContext( thread,
                                            timestamp,
                                            metrics,
                                            currentCallingContextHandle,
                                            previousCallingContextHandle,
                                            unwindDistance,
                                            SCOREP_Profile_Enter,
                                            SCOREP_Profile_Exit );

    SCOREP_Profile_LocationData* location     = scorep_profile_get_profile_data( thread );
    scorep_profile_node*         current_node = scorep_profile_get_current_node( location );
    current_node->hits++;
}

/**
   Called on enter events to update the profile accordingly.
   @param thread A pointer to the thread location data of the thread that executed
                 the enter event.
   @param timestamp                    The timestamp, when the region was entered.
   @param currentCallingContextHandle  Current calling context
   @param previousCallingContextHandle Last known calling context
   @param unwindDistance               Unwind distance
   @param metrics                      An array with metric samples which were taken on the enter event.
                                       The samples must be in the same order as the metric definitions
                                       at the @ref SCOREP_Profile_Initialize call.
 */
static void
calling_context_enter( SCOREP_Location*            thread,
                       uint64_t                    timestamp,
                       SCOREP_CallingContextHandle currentCallingContextHandle,
                       SCOREP_CallingContextHandle previousCallingContextHandle,
                       uint32_t                    unwindDistance,
                       uint64_t*                   metrics )
{
    /* This will only process leave events resulting from the previous
     * calling context and enter from the current, except the last one into
     * the current calling context */
    SCOREP_Unwinding_ProcessCallingContext( thread,
                                            timestamp,
                                            metrics,
                                            SCOREP_CallingContextHandle_GetParent( currentCallingContextHandle ),
                                            previousCallingContextHandle,
                                            unwindDistance - 1,
                                            SCOREP_Profile_Enter,
                                            SCOREP_Profile_Exit );

    enter_region( thread,
                  timestamp,
                  SCOREP_CallingContextHandle_GetRegion( currentCallingContextHandle ),
                  metrics );
}

/**
   Called on exit events to update the profile accordingly.
   @param thread A pointer to the thread location data of the thread that executed
                 the exit event.
   @param timestamp                    The timestamp, when the region was left.
   @param currentCallingContextHandle  Current calling context
   @param previousCallingContextHandle Last known calling context
   @param unwindDistance               Unwind distance
   @param metrics                      An array with metric samples which were taken on the exit event.
                                       The samples must be in the same order as the metric definitions
                                       at the @ref SCOREP_Profile_Initialize call.
 */
static void
calling_context_exit( SCOREP_Location*            thread,
                      uint64_t                    timestamp,
                      SCOREP_CallingContextHandle currentCallingContextHandle,
                      SCOREP_CallingContextHandle previousCallingContextHandle,
                      uint32_t                    unwindDistance,
                      uint64_t*                   metrics )
{
    /* This will only process leave events resulting from the previous
     * calling context */
    SCOREP_Unwinding_ProcessCallingContext( thread,
                                            timestamp,
                                            metrics,
                                            currentCallingContextHandle,
                                            previousCallingContextHandle,
                                            1,
                                            SCOREP_Profile_Enter,
                                            SCOREP_Profile_Exit );

    exit_region( thread,
                 timestamp,
                 SCOREP_CallingContextHandle_GetRegion( currentCallingContextHandle ),
                 metrics );
}

/**
   Called when a user metric / atomic / context event for signed integer values was triggered.
   @param location      A pointer to the thread location data of the thread that executed
                        the event.
   @param timestamp     Unused.
   @param counterHandle Handle of the sampling set definition.
   @param value         Sample for the metric.
 */
static void
trigger_counter_int64( SCOREP_Location*         location,
                       uint64_t                 timestamp,
                       SCOREP_SamplingSetHandle counterHandle,
                       int64_t                  value )
{
    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( counterHandle, SamplingSet );
    if ( sampling_set->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_sampling_set =
            ( SCOREP_ScopedSamplingSetDef* )sampling_set;
        sampling_set = SCOREP_LOCAL_HANDLE_DEREF( scoped_sampling_set->sampling_set_handle,
                                                  SamplingSet );
        UTILS_BUG_ON( scoped_sampling_set->recorder_handle
                      != SCOREP_Location_GetLocationHandle( location ),
                      "Writing scoped metric by the wrong recorder." );
    }
    UTILS_BUG_ON( sampling_set->number_of_metrics != 1,
                  "User sampling set with more than one metric" );

    SCOREP_Profile_TriggerInteger( location,
                                   sampling_set->metric_handles[ 0 ],
                                   ( uint64_t )value ); //Looses precision
}


void
SCOREP_Profile_TriggerInteger( SCOREP_Location*    thread,
                               SCOREP_MetricHandle metric,
                               uint64_t            value )
{
    scorep_profile_node*              node     = NULL;
    scorep_profile_sparse_metric_int* current  = NULL;
    scorep_profile_sparse_metric_int* next     = NULL;
    SCOREP_Profile_LocationData*      location = NULL;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    location = scorep_profile_get_profile_data( thread );

    /* Validity check */
    node = scorep_profile_get_current_node( location );
    if ( node == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Metric triggered outside of a region." );
        SCOREP_PROFILE_STOP( location );
        return;
    }

    scorep_profile_trigger_int64( location, metric, value, node,
                                  SCOREP_PROFILE_TRIGGER_UPDATE_VALUE_AS_IS );
}

static void
write_sparse_metrics_enter( SCOREP_Location*         thread,
                            uint64_t                 timestamp,
                            SCOREP_SamplingSetHandle samplingSet,
                            const uint64_t*          values )
{
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( thread );
    scorep_profile_node*         node     = scorep_profile_get_current_node( location );


    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( samplingSet, SamplingSet );
    if ( sampling_set->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_sampling_set =
            ( SCOREP_ScopedSamplingSetDef* )sampling_set;
        sampling_set = SCOREP_LOCAL_HANDLE_DEREF( scoped_sampling_set->sampling_set_handle,
                                                  SamplingSet );
    }

    /* Make sure that sampling set contains only one metric.
     * We handle each synchronous metric in its individual
     * sampling sets to permit synchronous metrics to skip
     * writing a value if the metric was not updated. Therefore,
     * a sampling set of a synchronous metric needs to contain
     * exactly one metric. */
    UTILS_ASSERT( sampling_set->number_of_metrics == 1 );
    SCOREP_MetricValueType value_type = SCOREP_MetricHandle_GetValueType(
        sampling_set->metric_handles[ 0 ] );
    switch ( value_type )
    {
        case SCOREP_METRIC_VALUE_INT64:
        case SCOREP_METRIC_VALUE_UINT64:
            scorep_profile_trigger_int64( location, sampling_set->metric_handles[ 0 ], values[ 0 ],
                                          node, SCOREP_PROFILE_TRIGGER_UPDATE_BEGIN_VALUE );
            break;
        case SCOREP_METRIC_VALUE_DOUBLE:
        {
            double value = ( ( double* )values )[ 0 ];
            scorep_profile_trigger_double( location, sampling_set->metric_handles[ 0 ], value, node,
                                           SCOREP_PROFILE_TRIGGER_UPDATE_BEGIN_VALUE );
            break;
        }
        default:
            UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "Unknown metric value type %u",
                         value_type );
    }
}

static void
write_sparse_metrics_exit( SCOREP_Location*         thread,
                           uint64_t                 timestamp,
                           SCOREP_SamplingSetHandle samplingSet,
                           const uint64_t*          values )
{
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( thread );
    scorep_profile_node*         node     = scorep_profile_get_current_node( location );
    if ( node == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Metric triggered outside of a region." );
        SCOREP_PROFILE_STOP( location );
        return;
    }


    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( samplingSet, SamplingSet );
    if ( sampling_set->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_sampling_set =
            ( SCOREP_ScopedSamplingSetDef* )sampling_set;
        sampling_set = SCOREP_LOCAL_HANDLE_DEREF( scoped_sampling_set->sampling_set_handle,
                                                  SamplingSet );
    }

    /* Make sure that sampling set contains only one metric.
     * We handle each synchronous metric in its individual
     * sampling sets to permit synchronous metrics to skip
     * writing a value if the metric was not updated. Therefore,
     * a sampling set of a synchronous metric needs to contain
     * exactly one metric. */
    UTILS_ASSERT( sampling_set->number_of_metrics == 1 );

    SCOREP_MetricValueType value_type = SCOREP_MetricHandle_GetValueType(
        sampling_set->metric_handles[ 0 ] );
    switch ( value_type )
    {
        case SCOREP_METRIC_VALUE_INT64:
        case SCOREP_METRIC_VALUE_UINT64:
            scorep_profile_trigger_int64( location, sampling_set->metric_handles[ 0 ], values[ 0 ],
                                          node, SCOREP_PROFILE_TRIGGER_UPDATE_END_VALUE );
            break;
        case SCOREP_METRIC_VALUE_DOUBLE:
        {
            double value = ( ( double* )values )[ 0 ];
            scorep_profile_trigger_double( location, sampling_set->metric_handles[ 0 ], value, node,
                                           SCOREP_PROFILE_TRIGGER_UPDATE_END_VALUE );
            break;
        }
        default:
            UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "Unknown metric value type %u",
                         value_type );
    }
}

/**
   Called when a user metric / atomic / context event for unsigned integer values was triggered.
   @param location      A pointer to the thread location data of the thread that executed
                        the event.
   @param timestamp     Unused.
   @param counterHandle Handle of the sampling set definition.
   @param value         Sample for the metric.
 */
static void
trigger_counter_uint64( SCOREP_Location*         location,
                        uint64_t                 timestamp,
                        SCOREP_SamplingSetHandle counterHandle,
                        uint64_t                 value )
{
    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( counterHandle, SamplingSet );
    if ( sampling_set->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_sampling_set =
            ( SCOREP_ScopedSamplingSetDef* )sampling_set;
        sampling_set = SCOREP_LOCAL_HANDLE_DEREF( scoped_sampling_set->sampling_set_handle,
                                                  SamplingSet );
        UTILS_BUG_ON( scoped_sampling_set->recorder_handle
                      != SCOREP_Location_GetLocationHandle( location ),
                      "Writing scoped metric by the wrong recorder." );
    }
    UTILS_BUG_ON( sampling_set->number_of_metrics != 1,
                  "User sampling set with more than one metric" );

    SCOREP_Profile_TriggerInteger( location,
                                   sampling_set->metric_handles[ 0 ],
                                   value );
}


/**
   Called when a user metric / atomic / context event for double values was triggered.
   @param location      A pointer to the thread location data of the thread that executed
                        the event.
   @param timestamp     Unused.
   @param counterHandle Handle of the sampling set definition.
   @param value         Sample for the metric.
 */
static void
trigger_counter_double( SCOREP_Location*         location,
                        uint64_t                 timestamp,
                        SCOREP_SamplingSetHandle counterHandle,
                        double                   value )
{
    SCOREP_SamplingSetDef* sampling_set
        = SCOREP_LOCAL_HANDLE_DEREF( counterHandle, SamplingSet );
    if ( sampling_set->is_scoped )
    {
        SCOREP_ScopedSamplingSetDef* scoped_sampling_set =
            ( SCOREP_ScopedSamplingSetDef* )sampling_set;
        sampling_set = SCOREP_LOCAL_HANDLE_DEREF( scoped_sampling_set->sampling_set_handle,
                                                  SamplingSet );
        UTILS_BUG_ON( scoped_sampling_set->recorder_handle
                      != SCOREP_Location_GetLocationHandle( location ),
                      "Writing scoped metric by the wrong recorder." );
    }
    UTILS_BUG_ON( sampling_set->number_of_metrics != 1,
                  "User sampling set with more than one metric" );

    SCOREP_Profile_TriggerDouble( location,
                                  sampling_set->metric_handles[ 0 ],
                                  value );
}


void
SCOREP_Profile_TriggerDouble( SCOREP_Location*    thread,
                              SCOREP_MetricHandle metric,
                              double              value )
{
    scorep_profile_node*                 node     = NULL;
    scorep_profile_sparse_metric_double* current  = NULL;
    scorep_profile_sparse_metric_double* next     = NULL;
    SCOREP_Profile_LocationData*         location = NULL;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    location = scorep_profile_get_profile_data( thread );

    /* Validity check */
    node = scorep_profile_get_current_node( location );
    if ( node == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Metric triggered outside of a region." );
        SCOREP_PROFILE_STOP( location );
        return;
    }

    scorep_profile_trigger_double( location, metric, value, node,
                                   SCOREP_PROFILE_TRIGGER_UPDATE_VALUE_AS_IS );
}

/**
   Called when a signed integer parameter was triggered
   @param thread    A pointer to the thread location data of the thread that executed
                    the event.
   @param timestamp Unused.
   @param param     Handle of the triggered parameter.
   @param value     The parameter integer value.
 */
static void
parameter_int64( SCOREP_Location*       thread,
                 uint64_t               timestamp,
                 SCOREP_ParameterHandle param,
                 int64_t                value )
{
    parameter_uint64( thread, timestamp, param, ( uint64_t )value );
}


static void
parameter_uint64( SCOREP_Location*       thread,
                  uint64_t               timestamp,
                  SCOREP_ParameterHandle param,
                  uint64_t               value )
{
    scorep_profile_node*       node = NULL;
    scorep_profile_type_data_t node_data;
    memset( &node_data, 0, sizeof( node_data ) );

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* If we exceed the maximum callpath depth -> do nothing.
       Do not even increase the depth level, because we do not know how many parameters
       were entered on an exit event. */
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( thread );

    if ( location->current_depth >= scorep_profile.max_callpath_depth )
    {
        return;
    }
    location->current_depth++;

    /* Initialize type specific data */
    scorep_profile_type_set_parameter_handle( &node_data, param );
    scorep_profile_type_set_int_value( &node_data, value );

    /* Set name */

    /* Get new callpath node */
    /* If this parameter is the "instance" type, we will always create a new
     * node */
    scorep_profile_node* parent = scorep_profile_get_current_node( location );
    if ( param == scorep_profile_param_instance )
    {
        node = scorep_profile_create_node( location,
                                           parent,
                                           SCOREP_PROFILE_NODE_PARAMETER_INTEGER,
                                           node_data,
                                           -1,
                                           scorep_profile_get_task_context( parent ) );
        node->next_sibling  = parent->first_child;
        parent->first_child = node;
    }
    else
    {
        node = scorep_profile_find_create_child( location,
                                                 parent,
                                                 SCOREP_PROFILE_NODE_PARAMETER_INTEGER,
                                                 node_data, -1 );
    }

    /* Disable profiling if node creation failed */
    if ( node == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Failed to create location" );
        SCOREP_PROFILE_STOP( location );
        return;
    }

    /* Store start values for dense metrics */
    setup_start_from_parent( node );

    /* Update current node pointer */
    scorep_profile_set_current_node( location, node );
}


void
SCOREP_Profile_ParameterString( SCOREP_Location*       thread,
                                uint64_t               timestamp,
                                SCOREP_ParameterHandle param,
                                SCOREP_StringHandle    string )
{
    scorep_profile_node*       node = NULL;
    scorep_profile_type_data_t node_data;
    memset( &node_data, 0, sizeof( node_data ) );

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* If we exceed the maximum callpath depth -> do nothing.
       Do not even increase the depth level, because we do not know how many parameters
       were entered on an exit event. */
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( thread );
    if ( location->current_depth >= scorep_profile.max_callpath_depth )
    {
        return;
    }
    location->current_depth++;

    /* Initialize type specific data */
    scorep_profile_type_set_parameter_handle( &node_data, param );
    scorep_profile_type_set_string_handle( &node_data, string );

    /* Get new callpath node */
    node = scorep_profile_find_create_child( location,
                                             scorep_profile_get_current_node( location ),
                                             SCOREP_PROFILE_NODE_PARAMETER_STRING,
                                             node_data, -1 );

    /* Disable profiling if node creation failed */
    if ( node == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                     "Failed to create location" );
        SCOREP_PROFILE_STOP( location );
        return;
    }

    /* Store start values for dense metrics */
    setup_start_from_parent( node );

    /* Update current node pointer */
    scorep_profile_set_current_node( location, node );
}


/* ***************************************************************************************
   Thread events
*****************************************************************************************/


/**
 * Called if one or more threads are created by this region. It is used to Mark the
 * creation point in the profile tree. This allows the reconstruction of the full
 * callpathes per thread later.
 * @param threadData        A pointer to the thread location data of the thread that executed
 *                          the event.
 * @param timestamp         Unused.
 * @param paradigm          Unused.
 * @param nRequestedThreads Unused.
 * @param forkSequenceCount forkSequenceCount of the newly created parallel region. It
 *                          should be the same number that will be passed to
 *                          SCOREP_Profile_OnThreadActivation for the threads of the newly
 *                          created parallel region.
 */
static void
thread_fork( SCOREP_Location*    threadData,
             uint64_t            timestamp,
             SCOREP_ParadigmType paradigm,
             uint32_t            nRequestedThreads,
             uint32_t            forkSequenceCount )
{
    scorep_profile_node*         fork_node = NULL;
    SCOREP_Profile_LocationData* location  = NULL;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: On Fork" );
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    location  = scorep_profile_get_profile_data( threadData );
    fork_node = scorep_profile_get_current_node( location );

    /* In case the fork node is a thread start node, this thread started at the same
       node like its parent thread. Thus, transfer the pointer. */
    if ( fork_node->node_type == SCOREP_PROFILE_NODE_THREAD_START &&
         scorep_profile_type_get_fork_node( fork_node->type_specific_data ) != NULL )
    {
        fork_node = scorep_profile_type_get_fork_node( fork_node->type_specific_data );
    }

    /* Store current fork node */
    scorep_profile_set_fork_node( fork_node, true );
    scorep_profile_add_fork_node( location, fork_node, location->current_depth, forkSequenceCount );
}


/**
 * Called after a parallel region is finished. Is used to clean up some thread
 * management data.
 * @param locationData  A pointer to the thread location data of the thread that executed
 *                      the event.
 * @param timestamp     Unused.
 * @param paradigm      Unused.
 */
static void
thread_join( SCOREP_Location*    locationData,
             uint64_t            timestamp,
             SCOREP_ParadigmType paradigm )
{
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( locationData );
    scorep_profile_remove_fork_node( location );
}


/**
 * Called when a thread begins its execution.
 *
 * @param location            A pointer to the thread location data of the thread that executed
 *                            the event.
 * @param timestamp           The timestamp, when the thread begin event occurred.
 * @param paradigm            Unused.
 * @param threadTeam          Unused.
 * @param createSequenceCount Unused.
 */
static void
thread_begin( SCOREP_Location*                 location,
              uint64_t                         timestamp,
              SCOREP_ParadigmType              paradigm,
              SCOREP_InterimCommunicatorHandle threadTeam,
              uint32_t                         createSequenceCount )
{
    uint64_t* metric_values = SCOREP_Metric_Read( location );

    SCOREP_Profile_Enter( location,
                          timestamp,
                          SCOREP_GetProgramRegion(),
                          metric_values );
    SCOREP_Profile_Enter( location,
                          timestamp,
                          thread_create_wait_regions,
                          metric_values );
}


/**
 * Called when the thread finishes its execution.
 *
 * @param locationData        A pointer to the thread location data of the thread that executed
 *                            the event.
 * @param timestamp           The timestamp, when the thread begin event occurred.
 * @param paradigm            Unused.
 * @param threadTeam          Unused.
 * @param createSequenceCount Unused.
 */
static void
thread_end( SCOREP_Location*                 location,
            uint64_t                         timestamp,
            SCOREP_ParadigmType              paradigm,
            SCOREP_InterimCommunicatorHandle threadTeam,
            uint32_t                         createSequenceCount )
{
    uint64_t* metric_values = SCOREP_Metric_Read( location );

    SCOREP_Profile_Exit( location,
                         timestamp,
                         thread_create_wait_regions,
                         metric_values );
    SCOREP_Profile_Exit( location,
                         timestamp,
                         SCOREP_GetProgramRegion(),
                         metric_values );
}


typedef struct leaked_memory_memento leaked_memory_memento;
struct leaked_memory_memento
{
    scorep_profile_node*         node;
    SCOREP_Profile_LocationData* location;
    leaked_memory_memento*       next;
};


static leaked_memory_memento* leaked_memory_memento_free_list;

static void
track_alloc( struct SCOREP_Location* threadData,
             uint64_t                timestamp,
             uint64_t                addrAllocated,
             size_t                  bytesAllocated,
             void*                   substrateData[],
             size_t                  bytesAllocatedMetric,
             size_t                  bytesAllocatedProcess )
{
    UTILS_ASSERT( substrateData );
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( threadData );
    SCOREP_Profile_TriggerInteger( threadData, bytes_allocated_metric, bytesAllocated );
    SCOREP_Profile_TriggerInteger( threadData, max_heap_memory_allocated_metric, bytesAllocatedProcess );

    leaked_memory_memento* memento = leaked_memory_memento_free_list;
    if ( memento )
    {
        leaked_memory_memento_free_list = leaked_memory_memento_free_list->next;
    }
    else
    {
        memento = SCOREP_Location_AllocForProfile( threadData, sizeof( leaked_memory_memento ) );
    }
    memento->node                                = scorep_profile_get_current_node( location );
    memento->location                            = location;
    memento->next                                = NULL;
    substrateData[ scorep_profile_substrate_id ] = memento;
}


static void
track_realloc( struct SCOREP_Location* threadData,
               uint64_t                timestamp,
               uint64_t                oldAddr,
               size_t                  oldBytesAllocated,
               void*                   oldSubstrateData[],
               uint64_t                newAddr,
               size_t                  newBytesAllocated,
               void*                   newSubstrateData[],
               size_t                  bytesAllocatedMetric,
               size_t                  bytesAllocatedProcess )
{
    UTILS_ASSERT( oldSubstrateData );
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( threadData );

    SCOREP_Profile_TriggerInteger( threadData, bytes_freed_metric, oldBytesAllocated );
    SCOREP_Profile_TriggerInteger( threadData, bytes_allocated_metric, newBytesAllocated );
    SCOREP_Profile_TriggerInteger( threadData, max_heap_memory_allocated_metric, bytesAllocatedProcess );

    leaked_memory_memento* memento = oldSubstrateData[ scorep_profile_substrate_id ];
    memento->node     = scorep_profile_get_current_node( location );
    memento->location = location;
    UTILS_BUG_ON( memento->next != NULL );

    if ( oldAddr != newAddr )
    {
        newSubstrateData[ scorep_profile_substrate_id ] = memento;
    }
}


static void
track_free( struct SCOREP_Location* threadData,
            uint64_t                timestamp,
            uint64_t                addrFreed,
            size_t                  bytesFreed,
            void*                   substrateData[],
            size_t                  bytesAllocatedMetric,
            size_t                  bytesAllocatedProcess )
{
    UTILS_ASSERT( substrateData );
    SCOREP_Profile_LocationData* location = scorep_profile_get_profile_data( threadData );
    SCOREP_Profile_TriggerInteger( threadData, bytes_freed_metric, bytesFreed );

    leaked_memory_memento* free_me = substrateData[ scorep_profile_substrate_id ];
    if ( !free_me )
    {
        UTILS_WARNING( "Address %p freed but provides no substrate data.", ( void* )addrFreed );
        return;
    }
    free_me->next                                = leaked_memory_memento_free_list;
    leaked_memory_memento_free_list              = free_me;
    substrateData[ scorep_profile_substrate_id ] = NULL;
}


static void
leaked_memory( uint64_t addrLeaked, size_t bytesLeaked, void* substrateData[] )
{
    /* This function triggers a counter, which should not happen when recording is disabled.
     * Therfore, return. */
    if ( !SCOREP_RecordingEnabled() )
    {
        return;
    }
    UTILS_ASSERT( substrateData );
    leaked_memory_memento* leak = substrateData[ scorep_profile_substrate_id ];
    UTILS_ASSERT( leak );
    scorep_profile_trigger_int64( leak->location, bytes_leaked_metric, bytesLeaked, leak->node,
                                  SCOREP_PROFILE_TRIGGER_UPDATE_VALUE_AS_IS );
}

static bool
get_requirement( SCOREP_Substrates_RequirementFlag flag )
{
    switch ( flag )
    {
        case SCOREP_SUBSTRATES_REQUIREMENT_CREATE_EXPERIMENT_DIRECTORY:
            return true;
        case SCOREP_SUBSTRATES_REQUIREMENT_PREVENT_ASYNC_METRICS:
            UTILS_WARN_ONCE( "The profiling substrate prevents recording of asynchronous metrics." );
            return true;
        case SCOREP_SUBSTRATES_REQUIREMENT_PREVENT_PER_HOST_AND_ONCE_METRICS:
            UTILS_WARN_ONCE( "The profiling substrate prevents recording of PER_HOST or ONCE metrics." );
            return true;
        default:
            return false;
    }
}

static void
dump_manifest( FILE* manifestFile, const char* relativeSourceDir, const char* targetDir )
{
    UTILS_ASSERT( manifestFile );

    SCOREP_ConfigManifestSectionHeader( manifestFile, "Profiling" );
    char name[ 200 ];
    sprintf( name, "%s.cubex", scorep_profile_basename );

    switch ( scorep_profile_output_format )
    {
        case SCOREP_PROFILE_OUTPUT_CUBE4:
            SCOREP_ConfigManifestSectionEntry( manifestFile, name, "CUBE4 result file of the summary measurement." );
            break;
        case SCOREP_PROFILE_OUTPUT_CUBE_TUPLE:
            SCOREP_ConfigManifestSectionEntry( manifestFile, name, "Extended set of statistics in CUBE4 format." );
            break;
        case SCOREP_PROFILE_OUTPUT_THREAD_SUM:
            SCOREP_ConfigManifestSectionEntry( manifestFile, name,
                                               "Sums all locations within a location group and stores the data in Cube4 format." );
            break;
        case SCOREP_PROFILE_OUTPUT_THREAD_TUPLE:
            SCOREP_ConfigManifestSectionEntry( manifestFile, name,
                                               "Sums all locations within a location group and stores in addition some statistical"
                                               " data about the distribution among the locations of a location group." );
            break;
        case SCOREP_PROFILE_OUTPUT_KEY_THREADS:
            SCOREP_ConfigManifestSectionEntry( manifestFile, name,
                                               "Stores the initial location, the slowest location and the fastest location per process. "
                                               "Sums all other locations within a location group. The result is stored in Cube4 format." );
            break;
        case SCOREP_PROFILE_OUTPUT_CLUSTER_THREADS:
            SCOREP_ConfigManifestSectionEntry( manifestFile, name,
                                               "Clusters locations within a location group if they have the same calltree structure. "
                                               "Sums locations within a cluster. Stores the result in Cube4 format." );
            break;
        case SCOREP_PROFILE_OUTPUT_TAU_SNAPSHOT:
            SCOREP_ConfigManifestSectionEntry( manifestFile, "tau/snapshot.<rank>.0.0", "TAU snapshot files." );
            break;
    }

    if ( scorep_profile_enable_core_files )
    {
        char core_name[ 200 ];
        sprintf( core_name, "%s.<rank>.<thrd>.core", scorep_profile_basename );

        SCOREP_ConfigManifestSectionEntry( manifestFile, core_name,
                                           "State of the profiling at error condition. (exists only in case of failure)" );
    }
}

const static SCOREP_Substrates_Callback substrate_callbacks[ SCOREP_SUBSTRATES_NUM_MODES ][ SCOREP_SUBSTRATES_NUM_EVENTS ] =
{
    {   /* SCOREP_SUBSTRATES_RECORDING_ENABLED */
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( OnTracingBufferFlushBegin, ON_TRACING_BUFFER_FLUSH_BEGIN, SCOREP_Profile_Enter ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( OnTracingBufferFlushEnd,   ON_TRACING_BUFFER_FLUSH_END,   SCOREP_Profile_Exit ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ProgramBegin,              PROGRAM_BEGIN,                 program_begin ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ProgramEnd,                PROGRAM_END,                   program_end ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( EnterRegion,               ENTER_REGION,                  enter_region ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ExitRegion,                EXIT_REGION,                   exit_region ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( Sample,                    SAMPLE,                        sample ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( CallingContextEnter,       CALLING_CONTEXT_ENTER,         calling_context_enter ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( CallingContextExit,        CALLING_CONTEXT_EXIT,          calling_context_exit ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiSend,                   MPI_SEND,                      SCOREP_Profile_MpiSend ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiRecv,                   MPI_RECV,                      SCOREP_Profile_MpiRecv ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiCollectiveEnd,          MPI_COLLECTIVE_END,            SCOREP_Profile_CollectiveEnd ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiIsend,                  MPI_ISEND,                     SCOREP_Profile_MpiIsend ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( MpiIrecv,                  MPI_IRECV,                     SCOREP_Profile_MpiIrecv ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaCollectiveEnd,          RMA_COLLECTIVE_END,            SCOREP_Profile_RmaCollectiveEnd ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaSync,                   RMA_SYNC,                      SCOREP_Profile_RmaSync ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaGroupSync,              RMA_GROUP_SYNC,                SCOREP_Profile_RmaGroupSync ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaPut,                    RMA_PUT,                       SCOREP_Profile_RmaPut ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaGet,                    RMA_GET,                       SCOREP_Profile_RmaGet ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( RmaAtomic,                 RMA_ATOMIC,                    SCOREP_Profile_RmaAtomic ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerCounterInt64,       TRIGGER_COUNTER_INT64,         trigger_counter_int64 ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerCounterUint64,      TRIGGER_COUNTER_UINT64,        trigger_counter_uint64 ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerCounterDouble,      TRIGGER_COUNTER_DOUBLE,        trigger_counter_double ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerParameterInt64,     TRIGGER_PARAMETER_INT64,       parameter_int64 ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerParameterUint64,    TRIGGER_PARAMETER_UINT64,      parameter_uint64 ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TriggerParameterString,    TRIGGER_PARAMETER_STRING,      SCOREP_Profile_ParameterString ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinFork,        THREAD_FORK_JOIN_FORK,         thread_fork ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinJoin,        THREAD_FORK_JOIN_JOIN,         thread_join ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTaskSwitch,  THREAD_FORK_JOIN_TASK_SWITCH,  SCOREP_Profile_TaskSwitch ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTaskBegin,   THREAD_FORK_JOIN_TASK_BEGIN,   SCOREP_Profile_TaskBegin ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinTaskEnd,     THREAD_FORK_JOIN_TASK_END,     SCOREP_Profile_TaskEnd ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitBegin,     THREAD_CREATE_WAIT_BEGIN,      thread_begin ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitEnd,       THREAD_CREATE_WAIT_END,        thread_end ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TrackAlloc,                TRACK_ALLOC,                   track_alloc ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TrackRealloc,              TRACK_REALLOC,                 track_realloc ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( TrackFree,                 TRACK_FREE,                    track_free ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( EnableRecording,           ENABLE_RECORDING,              enable_recording ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( DisableRecording,          DISABLE_RECORDING,             disable_recording ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( IoOperationComplete,       IO_OPERATION_COMPLETE,         scorep_profile_io_operation_complete ),
    },
    {        /* SCOREP_SUBSTRATES_RECORDING_DISABLED */
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinFork,        THREAD_FORK_JOIN_FORK,         thread_fork ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadForkJoinJoin,        THREAD_FORK_JOIN_JOIN,         thread_join ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitBegin,     THREAD_CREATE_WAIT_BEGIN,      thread_begin ),
        SCOREP_ASSIGN_SUBSTRATE_CALLBACK( ThreadCreateWaitEnd,       THREAD_CREATE_WAIT_END,        thread_end ),
    }
};

const static SCOREP_Substrates_Callback substrate_mgmt_callbacks[ SCOREP_SUBSTRATES_NUM_MGMT_EVENTS ] =
{
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( InitSubstrate,             INIT_SUBSTRATE,               SCOREP_Profile_Initialize ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( FinalizeSubstrate,         FINALIZE_SUBSTRATE,           SCOREP_Profile_Finalize ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( OnLocationCreation,        ON_LOCATION_CREATION,         on_location_creation ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( OnLocationDeletion,        ON_LOCATION_DELETION,         delete_location_data ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( OnCpuLocationActivation,   ON_CPU_LOCATION_ACTIVATION,   on_location_activation ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( OnCpuLocationDeactivation, ON_CPU_LOCATION_DEACTIVATION, on_location_deactivation ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( PreUnifySubstrate,         PRE_UNIFY_SUBSTRATE,          SCOREP_Profile_Process ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( WriteData,                 WRITE_DATA,                   write ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( CoreTaskCreate,            CORE_TASK_CREATE,             SCOREP_Profile_CreateTaskData ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( CoreTaskComplete,          CORE_TASK_COMPLETE,           SCOREP_Profile_FreeTaskData ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( InitializeMpp,             INITIALIZE_MPP,               SCOREP_Profile_InitializeMpp ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( LeakedMemory,              LEAKED_MEMORY,                leaked_memory ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( GetRequirement,            GET_REQUIREMENT,              get_requirement ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( DumpManifest,              DUMP_MANIFEST,                dump_manifest ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( IoParadigmEnter,           IO_PARADIGM_ENTER,            scorep_profile_io_paradigm_enter ),
    SCOREP_ASSIGN_SUBSTRATE_MGMT_CALLBACK( IoParadigmLeave,           IO_PARADIGM_LEAVE,            scorep_profile_io_paradigm_leave ),
};



const SCOREP_Substrates_Callback*
SCOREP_Profile_GetSubstrateMgmtCallbacks()
{
    return substrate_mgmt_callbacks;
}

const SCOREP_Substrates_Callback*
SCOREP_Profile_GetSubstrateCallbacks( SCOREP_Substrates_Mode mode )
{
    return substrate_callbacks[ mode ];
}
