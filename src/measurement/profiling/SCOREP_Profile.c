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
 * @file SCOREP_Profile.c Implementation of functions to construct/destruct the
 * profile tree.
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <SCOREP_Memory.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Config.h>
#include <SCOREP_Mutex.h>

#include <scorep_thread.h>
#include <inttypes.h>

#include <scorep_profile_node.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_process.h>
#include <scorep_profile_writer.h>
#include <scorep_profile_location.h>
#include <scorep_profile_task_table.h>
#include <scorep_profile_event_base.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Timing.h>

/* ***************************************************************************************
   Type definitions and variables
*****************************************************************************************/

/**
   Mutex for exclusive execution when adding a new location to the profile.
 */
static SCOREP_Mutex scorep_profile_location_mutex;

/* ***************************************************************************************
   internal helper functions
*****************************************************************************************/

static inline void
scorep_profile_setup_start_from_parent( scorep_profile_node* node )
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
        /* If no enclosing region is present, no dense metric valuies can be associated */
        node->inclusive_time.start_value = parent->inclusive_time.start_value;
        for ( i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
        {
            node->dense_metrics[ i ].start_value = parent->dense_metrics[ i ].start_value;
        }
    }
}

/* ***************************************************************************************
   Initialization / Finalization
*****************************************************************************************/

void
SCOREP_Profile_Register()
{
    SCOREP_ConfigRegister( "profiling", scorep_profile_configs );
}


void
SCOREP_Profile_Initialize( uint8_t              numDenseMetrics,
                           SCOREP_MetricHandle* metrics )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY,
                         "Initialize profile with %d dense metrics",
                         numDenseMetrics );

    if ( scorep_profile.is_initialized )
    {
        return;
    }

    SCOREP_MutexCreate( &scorep_profile_location_mutex );

    scorep_profile_init_definition( numDenseMetrics,
                                    metrics );

    if ( !scorep_profile.reinitialize )
    {
        scorep_profile_param_instance =
            SCOREP_DefineParameter( "instance", SCOREP_PARAMETER_INT64 );
    }
}

void
SCOREP_Profile_Finalize()
{
    scorep_profile_node*         current     = scorep_profile.first_root_node;
    SCOREP_Profile_LocationData* thread_data = NULL;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY,
                         "Delete profile definition." );

    /* Update all root nodes which survive a finalize, because locations are not
       reinitialized. Assume that the siblings of scorep_profile.first_root_node
       ar all of type scorep_profile_node_thread_root. */
    while ( current != NULL )
    {
        if ( current->node_type == scorep_profile_node_thread_root )
        {
            /* Cut off children */
            current->first_child = NULL;

            /* Reset thread local storage */
            thread_data                        = scorep_profile_type_get_location_data( current->type_specific_data );
            thread_data->current_task_node     = current;
            thread_data->current_implicit_node = current;
            thread_data->current_depth         = 0;
            thread_data->implicit_depth        = 0;
            thread_data->fork_node             = NULL;
            scorep_profile_task_finalize( thread_data );
        }
        else
        {
            SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT, "Root node of wrong type %d",
                          current->node_type );
        }

        /* Process next */
        current = current->next_sibling;
    }

    /* Reset profile definition struct */
    scorep_profile_delete_definition();

    /* Delete mutex */
    SCOREP_MutexDestroy( &scorep_profile_location_mutex );

    /* Free all requested memory */
    SCOREP_Memory_FreeProfileMem();
}

/* Allocate and initialize a valid SCOREP_Profile_LocationData object. */
SCOREP_Profile_LocationData*
SCOREP_Profile_CreateLocationData()
{
    /* Create location data structure.
     * The location data structure must not be deleted when the profile is reset
     * in a persicope phase. Thus the memory is not allocated from the profile
     * memory pages.
     */
    SCOREP_Profile_LocationData* data
        = SCOREP_Memory_AllocForMisc( sizeof( SCOREP_Profile_LocationData ) );

    /* Set default values. */
    data->current_implicit_node = NULL;
    data->root_node             = NULL;
    data->fork_node             = NULL;
    data->creation_node         = NULL;
    data->current_depth         = 0;
    data->implicit_depth        = 0;
    data->fork_depth            = 0;
    data->free_nodes            = NULL;
    data->free_int_metrics      = NULL;
    data->free_double_metrics   = NULL;
    data->current_task_node     = NULL;
    data->current_task_id       = 0;

    /* Initialize locations task instance table */
    scorep_profile_task_initialize( data );

    return data;
}


void
SCOREP_Profile_DeleteLocationData( SCOREP_Profile_LocationData* profileLocationData )
{
    if ( profileLocationData == NULL )
    {
        return;
    }

    /* Finlaize locations task instance table */
    scorep_profile_task_finalize( profileLocationData );
}

void
SCOREP_Profile_Process( SCOREP_Profile_ProcessingFlag processFlags )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Exit all regions that are not exited, yet. We assume that we post-process
       only when we are outside of a parallel region. Thus, we only exit
       regions on the main location.
     */
    uint64_t                    exit_time = SCOREP_GetClockTicks();
    SCOREP_Thread_LocationData* thread    = SCOREP_Thread_GetLocationData();
    scorep_profile_node*        node      = NULL;
    uint64_t*                   metrics   = NULL;

    if ( thread != NULL )
    {
        do
        {
            node = scorep_profile_get_current_node( thread );
            while ( ( node != NULL ) &&
                    ( node->node_type != scorep_profile_node_regular_region ) &&
                    ( node->node_type != scorep_profile_node_collapse ) )
            {
                node = node->parent;
            }
            if ( node == NULL )
            {
                break;
            }

            if ( node->node_type == scorep_profile_node_regular_region  )
            {
                SCOREP_RegionHandle region =
                    scorep_profile_type_get_region_handle( node->type_specific_data );
                fprintf( stderr, "Warning: Force exit for region %s\n",
                         SCOREP_Region_GetName( region ) );
                SCOREP_Profile_Exit( thread, region, exit_time, metrics );
            }
            else if ( node->node_type == scorep_profile_node_collapse )
            {
                fprintf( stderr, "Warning: Force exit from collapsed node\n" );
                SCOREP_Profile_Exit( thread, SCOREP_INVALID_REGION, exit_time, metrics );
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

    /* Substitute parameter entries by regions */
    if ( processFlags & SCOREP_Profile_ParamToRegion )
    {
        scorep_profile_substitute_parameter();
    }

    /* Thread start node expansion */
    if ( processFlags & SCOREP_Profile_ProcessThreads )
    {
        scorep_profile_expand_threads();
    }
    scorep_profile_sort_threads();
    scorep_profile_process_tasks();

    /* Make phases to the root of separate trees */
    if ( processFlags & SCOREP_Profile_Phase )
    {
        scorep_profile_process_phases();
    }

    /* Register callpath and assign callpath handles to every node */
    if ( processFlags & SCOREP_Profile_ProcessCallpath )
    {
        scorep_profile_assign_callpath_to_master();
        scorep_profile_assign_callpath_to_workers();
    }
}

void
SCOREP_Profile_Write()
{
    if ( scorep_profile.output_format == SCOREP_Profile_OutputNone )
    {
        return;
    }
    else if ( scorep_profile.output_format & SCOREP_Profile_OutputCube4 )
    {
        scorep_profile_write_cube4();
    }
    else if ( scorep_profile.output_format & SCOREP_Profile_OutputTauSnapshot )
    {
        scorep_profile_write_tau_snapshot();
    }
    else
    {
        SCOREP_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "Unsupported profile format" );
    }
}

void
SCOREP_Profile_SetCalltreeConfiguration( uint32_t maxCallpathDepth,
                                         uint32_t maxCallpathNum )
{
    scorep_profile.max_callpath_depth = maxCallpathDepth;
    scorep_profile.max_callpath_num   = maxCallpathNum;
}
/* ***************************************************************************************
   Callpath events
*****************************************************************************************/

void
SCOREP_Profile_Enter( SCOREP_Thread_LocationData* thread,
                      SCOREP_RegionHandle         region,
                      SCOREP_RegionType           type,
                      uint64_t                    timestamp,
                      uint64_t*                   metrics )
{
    //printf( "%u: Enter %s\n", SCOREP_Thread_GetLocationId( thread ), SCOREP_Region_GetName( region ) );
    scorep_profile_node* node = NULL;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                         "Enter event of profiling system called" );

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Check wether we excced the depth */
    SCOREP_Profile_LocationData* location =
        SCOREP_Thread_GetProfileLocationData( thread );
    location->current_depth++;

    /* Enter on current position */
    scorep_profile_node* current_node = scorep_profile_get_current_node( thread );
    node = scorep_profile_enter( location,
                                 current_node,
                                 region,
                                 type,
                                 timestamp,
                                 metrics );
    assert( node != NULL );

    /* Update current node pointer */
    scorep_profile_set_current_node( thread, node );

    /* If dynamic region call trigger parameter */
    switch ( type )
    {
        case SCOREP_REGION_DYNAMIC:
        case SCOREP_REGION_DYNAMIC_PHASE:
        case SCOREP_REGION_DYNAMIC_LOOP:
        case SCOREP_REGION_DYNAMIC_FUNCTION:
        case SCOREP_REGION_DYNAMIC_LOOP_PHASE:

            /* For Dynamic Regions we use a special "instance" paramter defined
             * during initialization */
            SCOREP_Profile_ParameterInteger( thread,
                                             scorep_profile_param_instance,
                                             node->count );
    }
}


void
SCOREP_Profile_Exit( SCOREP_Thread_LocationData* thread,
                     SCOREP_RegionHandle         region,
                     uint64_t                    timestamp,
                     uint64_t*                   metrics )
{
    //printf( "%u: Exit %s\n", SCOREP_Thread_GetLocationId( thread ), SCOREP_Region_GetName( region ) );
    int                          i;
    scorep_profile_node*         node   = NULL;
    scorep_profile_node*         parent = NULL;
    SCOREP_Profile_LocationData* location;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                         "Exit event of profiling system called" );

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Store task metrics if we leave a parallel region */
    if ( SCOREP_Region_GetType( region ) == SCOREP_REGION_OMP_PARALLEL )
    {
        scorep_profile_task_parallel_exit( thread );
    }

    /* Get current node */
    location = SCOREP_Thread_GetProfileLocationData( thread );
    SCOREP_ASSERT( location != NULL );

    node = scorep_profile_get_current_node( thread );
    assert( node != NULL );
    parent = scorep_profile_exit( location, node, region, timestamp, metrics );

    /* Update current node */
    scorep_profile_set_current_node( thread, parent );
}

void
SCOREP_Profile_TriggerInteger( SCOREP_Thread_LocationData* thread,
                               SCOREP_MetricHandle         metric,
                               uint64_t                    value )
{
    scorep_profile_node*              node     = NULL;
    scorep_profile_sparse_metric_int* current  = NULL;
    scorep_profile_sparse_metric_int* next     = NULL;
    SCOREP_Profile_LocationData*      location = NULL;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Validity check */
    node = scorep_profile_get_current_node( thread );
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Metric triggered outside of a region." );
        SCOREP_PROFILE_STOP;
        return;
    }

    location = SCOREP_Thread_GetProfileLocationData( thread );

    scorep_profile_trigger_int64( location, metric, value, node );
}

void
SCOREP_Profile_TriggerDouble( SCOREP_Thread_LocationData* thread,
                              SCOREP_MetricHandle         metric,
                              double                      value )
{
    scorep_profile_node*                 node     = NULL;
    scorep_profile_sparse_metric_double* current  = NULL;
    scorep_profile_sparse_metric_double* next     = NULL;
    SCOREP_Profile_LocationData*         location = NULL;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Validity check */
    node = scorep_profile_get_current_node( thread );
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Metric triggered outside of a region." );
        SCOREP_PROFILE_STOP;
        return;
    }

    location = SCOREP_Thread_GetProfileLocationData( thread );

    scorep_profile_trigger_double( location, metric, value, node );
}

void
SCOREP_Profile_ParameterString( SCOREP_Thread_LocationData* thread,
                                SCOREP_ParameterHandle      param,
                                SCOREP_StringHandle         string )
{
    scorep_profile_node*       node = NULL;
    scorep_profile_type_data_t node_data;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* If we exceed the maximum callpath depth -> do nothing.
       Do not even increase the depth level, because we do not know how many parameters
       were entered on an exit event. */
    SCOREP_Profile_LocationData* location =
        SCOREP_Thread_GetProfileLocationData( thread );
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
                                             scorep_profile_get_current_node( thread ),
                                             scorep_profile_node_parameter_string,
                                             node_data, -1 );

    /* Disable profiling if node creation failed */
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Failed to create location" );
        SCOREP_PROFILE_STOP;
        return;
    }

    /* Store start values for dense metrics */
    scorep_profile_setup_start_from_parent( node );

    /* Update current node pointer */
    scorep_profile_set_current_node( thread, node );
}

void
SCOREP_Profile_ParameterInteger( SCOREP_Thread_LocationData* thread,
                                 SCOREP_ParameterHandle      param,
                                 int64_t                     value )
{
    scorep_profile_node*       node = NULL;
    scorep_profile_type_data_t node_data;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* If we exceed the maximum callpath depth -> do nothing.
       Do not even increase the depth level, because we do not know how many parameters
       were entered on an exit event. */
    SCOREP_Profile_LocationData* location =
        SCOREP_Thread_GetProfileLocationData( thread );

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
    if ( param == scorep_profile_param_instance )
    {
        scorep_profile_node* parent = scorep_profile_get_current_node( thread );
        node = scorep_profile_create_node( location,
                                           scorep_profile_get_current_node( thread ),
                                           scorep_profile_node_parameter_integer,
                                           node_data,
                                           -1 );
        node->next_sibling  = parent->first_child;
        parent->first_child = node;
    }
    else
    {
        node = scorep_profile_find_create_child( location,
                                                 scorep_profile_get_current_node( thread ),
                                                 scorep_profile_node_parameter_integer,
                                                 node_data, -1 );
    }
    //param.name_handle = node->parent->string_data + " [" + node->parent->count + "]"

    /* Disable profiling if node creation failed */
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Failed to create location" );
        SCOREP_PROFILE_STOP;
        return;
    }

    /* Store start values for dense metrics */
    scorep_profile_setup_start_from_parent( node );

    /* Update current node pointer */
    scorep_profile_set_current_node( thread, node );
}

/* ***************************************************************************************
   Thread events
*****************************************************************************************/

void
SCOREP_Profile_OnThreadCreation( SCOREP_Thread_LocationData* locationData,
                                 SCOREP_Thread_LocationData* parentLocationData )
{
}

void
SCOREP_Profile_OnThreadActivation( SCOREP_Thread_LocationData* locationData,
                                   SCOREP_Thread_LocationData* parentLocationData )
{
    SCOREP_Profile_LocationData* thread_data    = NULL;
    SCOREP_Profile_LocationData* parent_data    = NULL;
    scorep_profile_node*         root           = NULL;
    scorep_profile_node*         node           = NULL;
    scorep_profile_node*         creation_point = NULL;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: Activated thread" );

    SCOREP_PROFILE_ASSURE_INITIALIZED;
    SCOREP_ASSERT( locationData != NULL );

    /* If it is the same location as the parent, do not do anything */
    if ( locationData == parentLocationData )
    {
        return;
    }

    /* Get root node of the thread */
    thread_data = SCOREP_Thread_GetProfileLocationData( locationData );
    if ( thread_data == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Thread activated which was not created." );
        SCOREP_PROFILE_STOP;
        return;
    }
    root = thread_data->root_node;
    SCOREP_ASSERT( root != NULL );

    /* Find creation point if available */
    if ( parentLocationData != NULL )
    {
        parent_data = SCOREP_Thread_GetProfileLocationData( parentLocationData );
        if ( parent_data != NULL )
        {
            creation_point = parent_data->fork_node;
        }
    }

    /* Check wether such an activation node already exists */
    node = root->first_child;
    while ( ( node != NULL ) &&
            ( ( node->node_type != scorep_profile_node_thread_start ) ||
              ( creation_point != scorep_profile_type_get_fork_node( node->type_specific_data ) ) ) )
    {
        node = node->next_sibling;
    }

    /* Create new node if no one exists */
    if ( node == NULL )
    {
        scorep_profile_type_data_t data;
        scorep_profile_type_set_fork_node( &data, creation_point );
        node = scorep_profile_create_node( thread_data,
                                           root,
                                           scorep_profile_node_thread_start,
                                           data, 0 );

        /* Disable profiling if node creation failed */
        if ( node == NULL )
        {
            SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                          "Failed to create location" );
            SCOREP_PROFILE_STOP;
            return;
        }

        node->next_sibling = root->first_child;
        root->first_child  = node;
    }

    /* Now node points to the starting point of the thread.
       Make it the current node of the thread. */
    scorep_profile_set_current_node( locationData, node );
}


void
SCOREP_Profile_OnThreadDectivation( SCOREP_Thread_LocationData* locationData,
                                    SCOREP_Thread_LocationData* parentLocationData )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: Deactivated thread" );

    /* If it is the same location as the parent, do not do anything */
    if ( locationData == parentLocationData )
    {
        return;
    }

    /* Remove the current node. */
    scorep_profile_set_current_node( locationData, NULL );
}


void
SCOREP_Profile_OnLocationCreation( SCOREP_Thread_LocationData* locationData,
                                   SCOREP_Thread_LocationData* parentLocationData )
{
    SCOREP_Profile_LocationData* parent_data = NULL;
    SCOREP_Profile_LocationData* thread_data = NULL;
    scorep_profile_node*         node        = NULL;
    scorep_profile_type_data_t   node_data;
    uint64_t                     thread_id = 0;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: Create Location" );

    /* Initialize type specific data structure */
    thread_data = SCOREP_Thread_GetProfileLocationData( locationData );
    thread_id   = SCOREP_Thread_GetLocationId( locationData );
    SCOREP_ASSERT( thread_data != NULL );

    scorep_profile_type_set_location_data( &node_data, thread_data );
    scorep_profile_type_set_int_value( &node_data, thread_id );

    /* Create thread root node */
    node = scorep_profile_create_node( thread_data,
                                       NULL,
                                       scorep_profile_node_thread_root,
                                       node_data, 0 );

    /* Disable profiling if node creation failed */
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Failed to create location" );
        SCOREP_PROFILE_STOP;
        return;
    }

    /* Update thread location data */
    thread_data->root_node = node;

    if ( parentLocationData != NULL )
    {
        parent_data                = SCOREP_Thread_GetProfileLocationData( parentLocationData );
        thread_data->creation_node = parent_data->fork_node;
        thread_data->current_depth = parent_data->fork_depth;
    }

    /* Add it to the profile node list */
    if ( parent_data == NULL )
    {
        /* It is the initial thread. Insert as first new root node. */
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Initial location created" );

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
}

void
SCOREP_Profile_OnFork( SCOREP_Thread_LocationData* threadData,
                       size_t                      maxChildThreads )
{
    scorep_profile_node*         fork_node   = NULL;
    SCOREP_Profile_LocationData* thread_data = NULL;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: On Fork" );
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    fork_node = scorep_profile_get_current_node( threadData );

    /* In case the fork node is a thread start node, this thread started at the same
       node like its parent thread. Thus, transfer the pointer. */
    if ( fork_node->node_type == scorep_profile_node_thread_start )
    {
        fork_node = scorep_profile_type_get_fork_node( fork_node->type_specific_data );
    }

    /* Store current fork node */
    thread_data             = SCOREP_Thread_GetProfileLocationData( threadData );
    thread_data->fork_node  = fork_node;
    thread_data->fork_depth = thread_data->current_depth;
}
