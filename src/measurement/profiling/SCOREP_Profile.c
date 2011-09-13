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
#include <SCOREP_Definitions.h>
#include <SCOREP_Timing.h>

/* ***************************************************************************************
   Type definitions and variables
*****************************************************************************************/

struct SCOREP_Profile_LocationData
{
    scorep_profile_node* current_node;  /**< Current callpath of this thread */
    scorep_profile_node* root_node;     /**< Root node of this thread */
    scorep_profile_node* fork_node;     /**< Last Fork node created by this thread */
    scorep_profile_node* creation_node; /**< Node where the thread was created */
    uint32_t             current_depth; /**< Stores the current length of the callpath */
    uint32_t             fork_depth;    /**< Depth of last fork node */
};

#define SCOREP_Profile_OutputDefault SCOREP_Profile_OutputCube4

/**
   Allows to limit the depth of the calltree. If the current
   callpath becomes longer than specified by this parameter,
   no further child nodes for this callpath are created.
   This limit allows a reduction of the number of callpathes,
   especially, if the application contains recursive function
   calls.
 */
static uint64_t scorep_profile_max_callpath_depth = 30;

/**
   Allows to limit the number of nodes in the calltree. If the
   number of nodes in the calltree reaches its limit, no further
   callpathes are created. All new callpathes are collapsed into
   a single node. This parameter allows to limit the memory
   usage of the profile.
 */
static uint64_t scorep_profile_max_callpath_num = UINT64_MAX;

/**
   Stores the configuration setting for output format.
 */
static uint64_t scorep_profile_output_format = SCOREP_Profile_OutputDefault;

/**
   Bitset table for output format string configuration.
 */
static const SCOREP_ConfigType_SetEntry scorep_profile_format_table[] = {
    { "NONE",         SCOREP_Profile_OutputNone         },
    { "TAU_SNAPSHOT", SCOREP_Profile_OutputTauSnapshot  },
    { "CUBE4",        SCOREP_Profile_OutputCube4        },
    { "DEFAULT",      SCOREP_Profile_OutputDefault      },
    { NULL,           0                                 }
};

/**
   Configuration variable registration structures for the profiling system.
 */
static SCOREP_ConfigVariable scorep_profile_configs[] = {
    { "max_callpath_depth",
      SCOREP_CONFIG_TYPE_NUMBER,
      &scorep_profile_max_callpath_depth,
      NULL,
      "30",
      "Maximum depth of the calltree",
      "Maximum depth of the calltree" },
    /*{ "max_callpath_num",
       SCOREP_CONFIG_TYPE_NUMBER,
       &scorep_profile_max_callpath_num,
       NULL,
       "1000000000",
       "Maximum number of nodes in the calltree",
       "Maximum number of nodes in the calltree" },*/
    { "base_name",
      SCOREP_CONFIG_TYPE_STRING,
      &scorep_profile_basename,
      NULL,
      "profile",
      "Base for construction of the profile filename",
      "String which is used as based to create the filenames for the profile files" },
    { "format",
      SCOREP_CONFIG_TYPE_BITSET,
      &scorep_profile_output_format,
      ( void* )scorep_profile_format_table,
      "DEFAULT",
      "Profile output format",
      "Sets the output format for the profile.\n"
      "The following formats are supported:\n"
      " NONE: No profile output. This does not disable profile recording.\n"
      " TAU_SNAPSHOT: Tau snapshot format.\n"
      " CUBE4: Cube4 format.\n"
      " DEFAULT: Default format. If Cube4 is supported, Cube4 is the default\n"
      "          else the Tau snapshot format is default.\n", },
    SCOREP_CONFIG_TERMINATOR
};

/**
   Mutex for exclusive execution when adding a new location to the profile.
 */
static SCOREP_Mutex scorep_profile_location_mutex;

/* ***************************************************************************************
   internal helper functions
*****************************************************************************************/

/** Returns the current node for a thread */
static inline scorep_profile_node*
scorep_profile_get_current_node( SCOREP_Thread_LocationData* thread )
{
    return SCOREP_Thread_GetProfileLocationData( thread )->current_node;
}

/** Sets the current node for a thread */
static inline void
scorep_profile_set_current_node( SCOREP_Thread_LocationData* thread,
                                 scorep_profile_node*        node )
{
    SCOREP_Thread_GetProfileLocationData( thread )->current_node = node;
}

/** Process an enter or parameter call */
static inline scorep_profile_node*
scorep_profile_find_or_create_child( SCOREP_Thread_LocationData* thread,
                                     scorep_profile_node_type    type,
                                     scorep_profile_type_data_t  data,
                                     uint64_t                    timestamp )
{
    scorep_profile_node* node  = NULL;
    scorep_profile_node* child = NULL;

    /* Get current node */
    node = scorep_profile_get_current_node( thread );
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Current region unknown." );
        SCOREP_PROFILE_STOP;
        return NULL;
    }

    /* Find or create the child node */

    /* Check wether the node has children */
    child = node->first_child;
    if ( child == NULL )
    {
        node->first_child =
            scorep_profile_create_node( node, type, scorep_profile_copy_type_data( data, type ),
                                        timestamp );
        node = node->first_child;
    }
    /* At least one child exists: Search if the region was entered before */
    else
    {
        while ( ( child->next_sibling != NULL ) &&
                ( ( child->node_type != type ) ||
                  ( !scorep_profile_compare_type_data( data, child->type_specific_data, type ) ) ) )
        {
            child = child->next_sibling;
        }
        /* No matching region found: Create a new sibling */
        if ( ( child->node_type != type ) ||
             ( !scorep_profile_compare_type_data( data, child->type_specific_data, type ) ) )
        {
            child->next_sibling =
                scorep_profile_create_node( node, type,
                                            scorep_profile_copy_type_data( data, type ),
                                            timestamp );
            node = child->next_sibling;
        }
        /* Call path was already visited */
        else
        {
            node = child;
        }
    }
    /* Now node points to the newly entered region */
    return node;
}

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
SCOREP_Profile_Initialize( int32_t               numDenseMetrics,
                           SCOREP_CounterHandle* metrics )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY,
                         "Initialize profile with %d dense metrics",
                         numDenseMetrics );

    if ( scorep_profile_is_initialized )
    {
        return;
    }

    scorep_profile_is_initialized = true;

    SCOREP_MutexCreate( &scorep_profile_location_mutex );

    scorep_profile_init_definition( scorep_profile_max_callpath_depth,
                                    scorep_profile_max_callpath_num,
                                    numDenseMetrics, metrics );

    scorep_profile_param_instance = SCOREP_DefineParameter( "instance", SCOREP_PARAMETER_INT64 );
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
            thread_data = SCOREP_PROFILE_DATA2THREADROOT( current->type_specific_data )
                          ->thread_data;
            thread_data->current_node  = current;
            thread_data->current_depth = 0;
            thread_data->fork_node     = NULL;
        }
        else
        {
            SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT, "Root node of wrong type %d",
                          current->node_type );
        }

        /* Process next */
        current = current->next_sibling;
    }

    /* Set the flag to initialized */
    scorep_profile_is_initialized = true;

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
    data->current_node  = NULL;
    data->root_node     = NULL;
    data->fork_node     = NULL;
    data->creation_node = NULL;
    data->current_depth = 0;

    return data;
}


void
SCOREP_Profile_DeleteLocationData( SCOREP_Profile_LocationData* profileLocationData )
{
    /* Space is freed if the misc memory pages are freed. */
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
                    SCOREP_PROFILE_DATA2REGION( node->type_specific_data );
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

    /* Substitute collapse nodes by normal regio nodes */
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
    if ( scorep_profile_output_format == SCOREP_Profile_OutputNone )
    {
        return;
    }
    else if ( scorep_profile_output_format & SCOREP_Profile_OutputCube4 )
    {
        scorep_profile_write_cube4();
    }
    else if ( scorep_profile_output_format & SCOREP_Profile_OutputTauSnapshot )
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
    scorep_profile_max_callpath_depth = maxCallpathDepth;
    scorep_profile_max_callpath_num   = maxCallpathNum;
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
    scorep_profile_node* node = NULL;
    int                  i;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                         "Enter event of profiling system called" );

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Check wether we excced the depth */
    SCOREP_Profile_LocationData* loc = SCOREP_Thread_GetProfileLocationData( thread );
    loc->current_depth++;

    /* If we are already in a collapse node -> do nothing more */
    if ( ( loc->current_node != NULL ) &&
         ( loc->current_node->node_type == scorep_profile_node_collapse ) )
    {
        if ( scorep_profile.reached_depth <  loc->current_depth )
        {
            scorep_profile.reached_depth = loc->current_depth;
        }
        return;
    }

    /* If we just reached the depth limit */
    if ( loc->current_depth > scorep_profile.max_callpath_depth )
    {
        scorep_profile.has_collapse_node = true;
        if ( scorep_profile.reached_depth <  loc->current_depth )
        {
            scorep_profile.reached_depth = loc->current_depth;
        }
        node = scorep_profile_find_or_create_child( thread,
                                                    scorep_profile_node_collapse,
                                                    loc->current_depth,
                                                    timestamp );
    }
    /* Regular enter */
    else
    {
        node = scorep_profile_find_or_create_child( thread,
                                                    scorep_profile_node_regular_region,
                                                    SCOREP_PROFILE_REGION2DATA( region ),
                                                    timestamp );
    }
    /* Disable profiling if node creation failed */
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Failed to create location. Disable profiling" );
        SCOREP_PROFILE_STOP;
        return;
    }

    /* Store start values for dense metrics */
    node->count++;
    node->inclusive_time.start_value = timestamp;
    for ( i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
    {
        node->dense_metrics[ i ].start_value = metrics[ i ];
    }

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
    int                          i;
    scorep_profile_node*         node   = NULL;
    scorep_profile_node*         parent = NULL;
    SCOREP_Profile_LocationData* location;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                         "Exit event of profiling system called" );

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* Get current node */
    location = SCOREP_Thread_GetProfileLocationData( thread );
    SCOREP_ASSERT( location != NULL );

    node = scorep_profile_get_current_node( thread );
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Exit event occured in a thread which never entered a region" );
        SCOREP_PROFILE_STOP;
        return;
    }

    /* If we are in a collapse node, check whether the current depth is still
       larger than the creation depth of the collapse node */
    location->current_depth--;
    if ( ( node->node_type == scorep_profile_node_collapse ) &&
         ( location->current_depth >= node->type_specific_data ) )
    {
        return;
    }

    /* Exit all parameters and the region itself. Thus, more than one node may be exited.
       Initialize loop: start with this node. Further iterations should work on the
       parent. */
    parent = node;
    do
    {
        node = parent;

        /* Update metrics */
        node->last_exit_time = timestamp;
        scorep_profile_update_dense_metric( &node->inclusive_time, timestamp );
        for ( i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
        {
            scorep_profile_update_dense_metric( &node->dense_metrics[ i ], metrics[ i ] );
        }
        parent = node->parent;
    }
    while ( ( node->node_type != scorep_profile_node_regular_region ) &&
            ( node->node_type != scorep_profile_node_collapse ) &&
            ( parent != NULL ) );
    /* If this was a parameter node also exit next level node */

    if ( ( node->node_type == scorep_profile_node_regular_region ) &&
         ( SCOREP_PROFILE_DATA2REGION( node->type_specific_data ) != region ) )
    {
        SCOREP_PROFILE_STOP;
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Exit event for other than current region occured at "
                      "location %" PRIu64 ": Expected exit for region %s. "
                      "Exited region %s",
                      SCOREP_Thread_GetLocationId( thread ),
                      SCOREP_Region_GetName( SCOREP_PROFILE_DATA2REGION( node->type_specific_data ) ),
                      SCOREP_Region_GetName( region ) );
        return;
    }

    /* Update current node */
    scorep_profile_set_current_node( thread, parent );
}

void
SCOREP_Profile_TriggerInteger( SCOREP_Thread_LocationData* thread,
                               SCOREP_CounterHandle        metric,
                               uint64_t                    value )
{
    scorep_profile_node*              node    = NULL;
    scorep_profile_sparse_metric_int* current = NULL;
    scorep_profile_sparse_metric_int* next    = NULL;

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

    /* If no sparse metrics are stored so far. */
    next = node->first_int_sparse;
    if ( next == NULL )
    {
        node->first_int_sparse = scorep_profile_create_sparse_int( metric, value );
        return;
    }

    /* Iterate all existing sparse metrics */
    do
    {
        current = next;
        if ( current->metric == metric )
        {
            scorep_profile_update_sparse_int( current, value );
            return;
        }
        next = current->next_metric;
    }
    while ( next != NULL );

    /* Append new sparse metric */
    current->next_metric = scorep_profile_create_sparse_int( metric, value );
}

void
SCOREP_Profile_TriggerDouble( SCOREP_Thread_LocationData* thread,
                              SCOREP_CounterHandle        metric,
                              double                      value )
{
    scorep_profile_node*                 node    = NULL;
    scorep_profile_sparse_metric_double* current = NULL;
    scorep_profile_sparse_metric_double* next    = NULL;

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

    /* If no sparse metrics are stored so far. */
    next = node->first_double_sparse;
    if ( next == NULL )
    {
        node->first_double_sparse = scorep_profile_create_sparse_double( metric, value );
        return;
    }

    /* Iterate all existing sparse metrics */
    do
    {
        current = next;
        if ( current->metric == metric )
        {
            scorep_profile_update_sparse_double( current, value );
            return;
        }
        next = current->next_metric;
    }
    while ( next != NULL );

    /* Append new sparse metric */
    current->next_metric = scorep_profile_create_sparse_double( metric, value );
}

void
SCOREP_Profile_ParameterString( SCOREP_Thread_LocationData* thread,
                                SCOREP_ParameterHandle      param,
                                SCOREP_StringHandle         string )
{
    scorep_profile_node*            node = NULL;
    scorep_profile_string_node_data data;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* If we exceed the maximum callpath depth -> do nothing.
       Do not even increase the depth level, because we do not know how many parameters
       were entered on an exit event. */
    SCOREP_Profile_LocationData* loc = SCOREP_Thread_GetProfileLocationData( thread );
    if ( loc->current_depth >= scorep_profile.max_callpath_depth )
    {
        return;
    }
    loc->current_depth++;

    /* Initialize type specific data */
    data.handle = param;
    data.value  = string;

    /* Get new callpath node */
    node = scorep_profile_find_or_create_child( thread, scorep_profile_node_parameter_string,
                                                SCOREP_PROFILE_PARAMSTR2DATA( &data ), -1 );

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
    scorep_profile_node*             node = NULL;
    scorep_profile_integer_node_data data;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    /* If we exceed the maximum callpath depth -> do nothing.
       Do not even increase the depth level, because we do not know how many parameters
       were entered on an exit event. */
    SCOREP_Profile_LocationData* loc = SCOREP_Thread_GetProfileLocationData( thread );
    if ( loc->current_depth >= scorep_profile.max_callpath_depth )
    {
        return;
    }
    loc->current_depth++;

    /* Initialize type specific data */
    data.handle = param;
    data.value  = value;

    /* Set name */

    /* Get new callpath node */
    /* If this parameter is the "instance" type, we will always create a new
     * node */
    if ( param == scorep_profile_param_instance )
    {
        scorep_profile_node* parent = scorep_profile_get_current_node( thread );
        node = scorep_profile_create_node( scorep_profile_get_current_node( thread ),
                                           scorep_profile_node_parameter_integer, SCOREP_PROFILE_PARAMINT2DATA( &data ),
                                           -1 );
        node->next_sibling  = parent->first_child;
        parent->first_child = node;
    }
    else
    {
        node = scorep_profile_find_or_create_child( thread, scorep_profile_node_parameter_integer,
                                                    SCOREP_PROFILE_PARAMINT2DATA( &data ), -1 );
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
              ( creation_point != ( scorep_profile_node* )
                SCOREP_PROFILE_DATA2POINTER( node->type_specific_data ) ) ) )
    {
        node = node->next_sibling;
    }

    /* Create new node if no one exists */
    if ( node == NULL )
    {
        node = scorep_profile_create_node( root, scorep_profile_node_thread_start,
                                           SCOREP_PROFILE_POINTER2DATA( creation_point ), 0 );

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
    //thread_data->current_node = node;
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
    SCOREP_Profile_LocationData*  parent_data = NULL;
    SCOREP_Profile_LocationData*  thread_data = NULL;
    scorep_profile_node*          node        = NULL;
    scorep_profile_root_node_data data;

    SCOREP_PROFILE_ASSURE_INITIALIZED;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE, "Profile: Create Location" );

    /* Initialize type specific data structure */
    data.thread_data = SCOREP_Thread_GetProfileLocationData( locationData );
    data.thread_id   = SCOREP_Thread_GetLocationId( locationData );

    /* Create thread root node */
    node = scorep_profile_create_node( NULL, scorep_profile_node_thread_root,
                                       SCOREP_PROFILE_THREADROOT2DATA( &data ), 0 );

    /* Disable profiling if node creation failed */
    if ( node == NULL )
    {
        SCOREP_ERROR( SCOREP_ERROR_PROFILE_INCONSISTENT,
                      "Failed to create location" );
        SCOREP_PROFILE_STOP;
        return;
    }

    /* Update thread location data */
    thread_data = data.thread_data;
    SCOREP_ASSERT( thread_data != NULL );
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
        fork_node = ( scorep_profile_node* )
                    SCOREP_PROFILE_DATA2POINTER( fork_node->type_specific_data );
    }

    /* Store current fork node */
    thread_data             = SCOREP_Thread_GetProfileLocationData( threadData );
    thread_data->fork_node  = fork_node;
    thread_data->fork_depth = thread_data->current_depth;
}
