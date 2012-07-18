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
#include <SCOREP_Debug.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Config.h>
#include <SCOREP_Mutex.h>

#include <scorep_thread.h>
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>

#include <scorep_profile_node.h>
#include <scorep_profile_definition.h>
#include <scorep_profile_process.h>
#include <scorep_profile_writer.h>
#include <scorep_profile_location.h>
#include <scorep_profile_task_table.h>
#include <scorep_profile_event_base.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Timing.h>
#include <SCOREP_Location.h>
#include "SCOREP_Metric_Management.h"

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
    else if ( numDenseMetrics > 0 )
    {
        /* Reallocate space for dense metrics on root nodes */
        uint32_t size = numDenseMetrics * sizeof( scorep_profile_dense_metric );

        scorep_profile_node* current = scorep_profile.first_root_node;
        while ( current != NULL )
        {
            current->dense_metrics = ( scorep_profile_dense_metric* )
                                     SCOREP_Memory_AllocForProfile( size );

            scorep_profile_init_dense_metric( &current->inclusive_time );
            scorep_profile_init_dense_metric_array( current->dense_metrics,
                                                    numDenseMetrics );

            current = current->next_sibling;
        }
    }
    assert( scorep_profile_param_instance );
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
       are all of type scorep_profile_node_thread_root. */
    while ( current != NULL )
    {
        if ( current->node_type == scorep_profile_node_thread_root )
        {
            /* Cut off children */
            current->first_child = NULL;

            /* Metrics are freed, too */
            current->dense_metrics       = NULL;
            current->first_double_sparse = NULL;
            current->first_int_sparse    = NULL;

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

SCOREP_Profile_LocationData*
SCOREP_Profile_CreateLocationData( SCOREP_Location* locationData )
{
    return scorep_profile_create_location_data( locationData );
}


void
SCOREP_Profile_DeleteLocationData( SCOREP_Profile_LocationData* profileLocationData )
{
    scorep_profile_delete_location_data( profileLocationData );
}

void
SCOREP_Profile_Process( SCOREP_Location* location )
{
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Exit all regions that are not exited, yet. We assume that we post-process
       only when we are outside of a parallel region. Thus, we only exit
       regions on the main location.
     */
    uint64_t             exit_time = SCOREP_GetClockTicks();
    scorep_profile_node* node      = NULL;
    uint64_t*            metrics   = SCOREP_Metric_read( location );

    if ( location != NULL )
    {
        do
        {
            node = scorep_profile_get_current_node( SCOREP_Location_GetProfileData( location ) );
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
                SCOREP_Profile_Exit( location, region, exit_time, metrics );
            }
            else if ( node->node_type == scorep_profile_node_collapse )
            {
                fprintf( stderr, "Warning: Force exit from collapsed node\n" );
                SCOREP_Profile_Exit( location, SCOREP_INVALID_REGION,
                                     exit_time, metrics );
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
    if ( scorep_profile.output_format != SCOREP_Profile_OutputTauSnapshot )
    {
        scorep_profile_substitute_parameter();
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
SCOREP_Profile_Enter( SCOREP_Location*    thread,
                      SCOREP_RegionHandle region,
                      SCOREP_RegionType   type,
                      uint64_t            timestamp,
                      uint64_t*           metrics )
{
    //printf( "%u: Enter %s\n", SCOREP_Location_GetId( thread ), SCOREP_Region_GetName( region ) );
    scorep_profile_node* node = NULL;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                         "Enter event of profiling system called" );

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Check wether we excced the depth */
    SCOREP_Profile_LocationData* location =
        SCOREP_Location_GetProfileData( thread );
    location->current_depth++;

    /* Enter on current position */
    scorep_profile_node* current_node = scorep_profile_get_current_node( location );
    node = scorep_profile_enter( location,
                                 current_node,
                                 region,
                                 type,
                                 timestamp,
                                 metrics );
    assert( node != NULL );

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

            /* For Dynamic Regions we use a special "instance" parameter defined
             * during initialization */
            SCOREP_Profile_ParameterInteger( thread,
                                             scorep_profile_param_instance,
                                             node->count );
    }
}


void
SCOREP_Profile_Exit( SCOREP_Location*    thread,
                     SCOREP_RegionHandle region,
                     uint64_t            timestamp,
                     uint64_t*           metrics )
{
    //printf( "%u: Exit %s\n", SCOREP_Location_GetId( thread ), SCOREP_Region_GetName( region ) );
    int                          i;
    scorep_profile_node*         node   = NULL;
    scorep_profile_node*         parent = NULL;
    SCOREP_Profile_LocationData* location;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                         "Exit event of profiling system called" );

    SCOREP_PROFILE_ASSURE_INITIALIZED;
    location = SCOREP_Location_GetProfileData( thread );

    /* Store task metrics if we leave a parallel region */
    if ( SCOREP_Region_GetType( region ) == SCOREP_REGION_OMP_PARALLEL )
    {
        scorep_profile_task_parallel_exit( location );
    }

    /* Get current node */
    SCOREP_ASSERT( location != NULL );

    node = scorep_profile_get_current_node( location );
    assert( node != NULL );
    parent = scorep_profile_exit( location, node, region, timestamp, metrics );

    /* Update current node */
    scorep_profile_set_current_node( location, parent );
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

    location = SCOREP_Location_GetProfileData( thread );

    /* Validity check */
    node = scorep_profile_get_current_node( location );
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Metric triggered outside of a region." );
        SCOREP_PROFILE_STOP;
        return;
    }

    scorep_profile_trigger_int64( location, metric, value, node );
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

    location = SCOREP_Location_GetProfileData( thread );

    /* Validity check */
    node = scorep_profile_get_current_node( location );
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Metric triggered outside of a region." );
        SCOREP_PROFILE_STOP;
        return;
    }

    scorep_profile_trigger_double( location, metric, value, node );
}

void
SCOREP_Profile_ParameterString( SCOREP_Location*       thread,
                                SCOREP_ParameterHandle param,
                                SCOREP_StringHandle    string )
{
    scorep_profile_node*       node = NULL;
    scorep_profile_type_data_t node_data;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* If we exceed the maximum callpath depth -> do nothing.
       Do not even increase the depth level, because we do not know how many parameters
       were entered on an exit event. */
    SCOREP_Profile_LocationData* location =
        SCOREP_Location_GetProfileData( thread );
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
    setup_start_from_parent( node );

    /* Update current node pointer */
    scorep_profile_set_current_node( location, node );
}

void
SCOREP_Profile_ParameterInteger( SCOREP_Location*       thread,
                                 SCOREP_ParameterHandle param,
                                 int64_t                value )
{
    scorep_profile_node*       node = NULL;
    scorep_profile_type_data_t node_data;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* If we exceed the maximum callpath depth -> do nothing.
       Do not even increase the depth level, because we do not know how many parameters
       were entered on an exit event. */
    SCOREP_Profile_LocationData* location =
        SCOREP_Location_GetProfileData( thread );

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
                                           scorep_profile_node_parameter_integer,
                                           node_data,
                                           -1 );
        node->next_sibling  = parent->first_child;
        parent->first_child = node;
    }
    else
    {
        node = scorep_profile_find_create_child( location,
                                                 parent,
                                                 scorep_profile_node_parameter_integer,
                                                 node_data, -1 );
    }

    /* Disable profiling if node creation failed */
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Failed to create location" );
        SCOREP_PROFILE_STOP;
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

void
SCOREP_Profile_OnThreadCreation( SCOREP_Location* locationData,
                                 SCOREP_Location* parentLocationData )
{
}

void
SCOREP_Profile_OnThreadActivation( SCOREP_Location* locationData,
                                   SCOREP_Location* parentLocationData )
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
    thread_data = SCOREP_Location_GetProfileData( locationData );
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
        parent_data = SCOREP_Location_GetProfileData( parentLocationData );
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
    scorep_profile_set_current_node( thread_data, node );
}


void
SCOREP_Profile_OnThreadDeactivation( SCOREP_Location* locationData,
                                     SCOREP_Location* parentLocationData )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: Deactivated thread" );

    /* If it is the same location as the parent, do not do anything */
    if ( locationData == parentLocationData )
    {
        return;
    }

    /* Remove the current node. */
    scorep_profile_set_current_node( SCOREP_Location_GetProfileData( locationData ),
                                     NULL );
}


void
SCOREP_Profile_OnLocationCreation( SCOREP_Location* locationData,
                                   SCOREP_Location* parentLocationData )
{
    SCOREP_Profile_LocationData* parent_data = NULL;
    SCOREP_Profile_LocationData* thread_data = NULL;
    scorep_profile_node*         node        = NULL;
    scorep_profile_type_data_t   node_data;
    uint64_t                     thread_id = 0;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: Create Location" );

    /* Initialize type specific data structure */
    thread_data = SCOREP_Location_GetProfileData( locationData );
    thread_id   = SCOREP_Location_GetId( locationData );
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
        parent_data                = SCOREP_Location_GetProfileData( parentLocationData );
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
SCOREP_Profile_OnFork( SCOREP_Location* threadData,
                       size_t           maxChildThreads )
{
    scorep_profile_node*         fork_node = NULL;
    SCOREP_Profile_LocationData* location  = NULL;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: On Fork" );
    SCOREP_PROFILE_ASSURE_INITIALIZED;

    location  = SCOREP_Location_GetProfileData( threadData );
    fork_node = scorep_profile_get_current_node( location );

    /* In case the fork node is a thread start node, this thread started at the same
       node like its parent thread. Thus, transfer the pointer. */
    if ( fork_node->node_type == scorep_profile_node_thread_start )
    {
        fork_node = scorep_profile_type_get_fork_node( fork_node->type_specific_data );
    }

    /* Store current fork node */
    location->fork_node  = fork_node;
    location->fork_depth = location->current_depth;
}
