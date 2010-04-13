/*
 * This file is part of the SILC project (http://www.silc.de)
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
 * @file SILC_Profile.c Implementation of functions to construct/destruct the
 * profile tree.
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status ALPHA
 *
 */

#include "SILC_Memory.h"
#include "SILC_Utils.h"
#include "SILC_Profile.h"

#include "silc_thread.h"

#include "silc_profile_node.h"
#include "silc_profile_definition.h"

/* ***************************************************************************************
   Type definitions and variables
*****************************************************************************************/

struct SILC_Profile_LocationData
{
    silc_profile_node* current_node;  // Current callpath of this thread
    silc_profile_node* root_node;     // Root node of this thread
    silc_profile_node* fork_node;     // Last Fork node created by this thread
    silc_profile_node* creation_node; // Node where the thread was created
    uint32_t           current_depth; // Stores the current length of the callpath
};

/* ***************************************************************************************
   internal helper functions
*****************************************************************************************/

/** Returns the current node for a thread */
static inline silc_profile_node*
silc_profile_get_current_node( SILC_Thread_LocationData* thread )
{
    return SILC_Thread_GetProfileLocationData( thread )->current_node;
}

/** Sets the current node for a thread */
static inline void
silc_profile_set_current_node( SILC_Thread_LocationData* thread,
                               silc_profile_node*        node )
{
    SILC_Thread_GetProfileLocationData( thread )->current_node = node;
}

/** Process an enter or parameter call */
static inline silc_profile_node*
silc_profile_find_or_create_child( SILC_Thread_LocationData* thread,
                                   silc_profile_node_type    type,
                                   silc_profile_type_data_t  data,
                                   uint64_t                  timestamp )
{
    silc_profile_node* node  = NULL;
    silc_profile_node* child = NULL;

    /* Get current node */
    node = silc_profile_get_current_node( thread );
    if ( node == NULL )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Current region unknown." );
        SILC_PROFILE_STOP;
        return NULL;
    }

    /* Find or create the child node */

    /* Check wether the node has children */
    child = node->first_child;
    if ( child == NULL )
    {
        node->first_child =
            silc_profile_create_node( node, type, silc_profile_copy_type_data( data, type ),
                                      timestamp );
        node = node->first_child;
    }
    /* At least one child exists: Search if the region was entered before */
    else
    {
        while ( ( child->next_sibling != NULL ) &&
                ( ( child->node_type != type ) ||
                  ( !silc_profile_compare_type_data( data, child->type_specific_data, type ) ) ) )
        {
            child = child->next_sibling;
        }
        /* No matching region found: Create a new sibling */
        if ( ( child->node_type != type ) ||
             ( !silc_profile_compare_type_data( data, child->type_specific_data, type ) ) )
        {
            child->next_sibling =
                silc_profile_create_node( node, type,
                                          silc_profile_copy_type_data( data, type ),
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
silc_profile_setup_start_from_parent( silc_profile_node* node )
{
    silc_profile_node* parent = node->parent;
    int                i;

    /* Correct first start time, on first enter */
    if ( node->first_enter_time == -1 )
    {
        node->first_enter_time = parent->implicit_time.start_value;
    }

    /* Store start values for dense metrics */
    node->count++;
    if ( parent != NULL )
    {
        /* If no enclosing region is present, no dense metric valuies can be associated */
        node->implicit_time.start_value = parent->implicit_time.start_value;
        for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
        {
            node->dense_metrics[ i ].start_value = parent->dense_metrics[ i ].start_value;
        }
    }
}


/* ***************************************************************************************
   Initialization / Finalization
*****************************************************************************************/

void
SILC_Profile_Initialize( uint32_t            maxCallpathDepth,
                         uint32_t            maxCallpathNum,
                         int32_t             numDenseMetrics,
                         SILC_CounterHandle* metrics )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_FUNCTION_ENTRY,
                       "Initialize profile with %d dense metrics\n",
                       numDenseMetrics );

    if ( silc_profile_is_initialized )
    {
        return;
    }

    silc_profile_is_initialized = true;

    silc_profile_init_definition( maxCallpathDepth, maxCallpathNum,
                                  numDenseMetrics, metrics );
}

void
SILC_Profile_Finalize()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_FUNCTION_ENTRY,
                       "Delete profile definition." );

    /* Set the flag to uninitialized */
    silc_profile_is_initialized = false;

    /* Reset profile definition struct */
    silc_profile_delete_definition();

    /* Free all requested memory */
    SILC_Memory_FreeProfileMem();
}

/* Allocate and initialize a valid SILC_Profile_LocationData object. */
SILC_Profile_LocationData*
SILC_Profile_CreateLocationData()
{
    /* Create location data structure */
    SILC_Profile_LocationData* data = malloc( sizeof( SILC_Profile_LocationData ) );
    data->current_node  = NULL;
    data->root_node     = NULL;
    data->fork_node     = NULL;
    data->creation_node = NULL;
    data->current_depth = 0;

    return data;
}


void
SILC_Profile_DeleteLocationData( SILC_Profile_LocationData* profileLocationData )
{
    if ( profileLocationData )
    {
        free( profileLocationData );
    }
}

/* ***************************************************************************************
   Callpath events
*****************************************************************************************/

void
SILC_Profile_Enter( SILC_Thread_LocationData* thread,
                    SILC_RegionHandle         region,
                    SILC_RegionType           type,
                    uint64_t                  timestamp,
                    uint64_t*                 metrics )
{
    silc_profile_node* node = NULL;
    int                i;

    SILC_PROFILE_ASSURE_INITIALIZED;

    node = silc_profile_find_or_create_child( thread, silc_profile_node_regular_region,
                                              SILC_PROFILE_REGION2DATA( region ),
                                              timestamp );

    /* Store start values for dense metrics */
    node->count++;
    node->implicit_time.start_value = timestamp;
    for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
    {
        node->dense_metrics[ i ].start_value = metrics[ i ];
    }

    /* Update current node pointer */
    silc_profile_set_current_node( thread, node );
}

void
SILC_Profile_Exit( SILC_Thread_LocationData* thread,
                   SILC_RegionHandle         region,
                   uint64_t                  timestamp,
                   uint64_t*                 metrics )
{
    int                        i;
    silc_profile_node*         node   = NULL;
    silc_profile_node*         parent = NULL;
    SILC_Profile_LocationData* location;

    SILC_PROFILE_ASSURE_INITIALIZED;

    /* Get current node */
    location = SILC_Thread_GetProfileLocationData( thread );
    SILC_ASSERT( location != NULL );

    node = silc_profile_get_current_node( thread );
    if ( node == NULL )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Exit event occured in a thread which never entered a region" );
        SILC_PROFILE_STOP;
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
        silc_profile_update_dense_metric( &node->implicit_time, timestamp );
        for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
        {
            silc_profile_update_dense_metric( &node->dense_metrics[ i ], metrics[ i ] );
        }
        parent = node->parent;
    }
    while ( ( node->node_type != silc_profile_node_regular_region ) &&
            ( parent != NULL ) );
    /* If this was a parameter node also exit next level node */

    if ( ( node->node_type != silc_profile_node_regular_region ) &&
         ( SILC_PROFILE_DATA2REGION( node->type_specific_data ) != region ) )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Exit event for other than current region occured" );
        SILC_PROFILE_STOP;
        return;
    }

    /* Update current node */
    silc_profile_set_current_node( thread, parent );
}

void
SILC_Profile_TriggerInteger( SILC_Thread_LocationData* thread,
                             SILC_CounterHandle        metric,
                             uint64_t                  value )
{
    silc_profile_node*              node    = NULL;
    silc_profile_sparse_metric_int* current = NULL;
    silc_profile_sparse_metric_int* next    = NULL;

    SILC_PROFILE_ASSURE_INITIALIZED;

    /* Validity check */
    node = silc_profile_get_current_node( thread );
    if ( node == NULL )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Metric triggered outside of a region." );
        SILC_PROFILE_STOP;
        return;
    }

    /* If no sparse metrics are stored so far. */
    next = node->first_int_sparse;
    if ( next == NULL )
    {
        node->first_int_sparse = silc_profile_create_sparse_int( metric, value );
        return;
    }

    /* Iterate all existing sparse metrics */
    do
    {
        current = next;
        if ( current->metric == metric )
        {
            silc_profile_update_sparse_int( current, value );
            return;
        }
        next = current->next_metric;
    }
    while ( next != NULL );

    /* Append new sparse metric */
    current->next_metric = silc_profile_create_sparse_int( metric, value );
}

void
SILC_Profile_TriggerDouble( SILC_Thread_LocationData* thread,
                            SILC_CounterHandle        metric,
                            double                    value )
{
    silc_profile_node*                 node    = NULL;
    silc_profile_sparse_metric_double* current = NULL;
    silc_profile_sparse_metric_double* next    = NULL;

    SILC_PROFILE_ASSURE_INITIALIZED;

    /* Validity check */
    node = silc_profile_get_current_node( thread );
    if ( node == NULL )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Metric triggered outside of a region." );
        SILC_PROFILE_STOP;
        return;
    }

    /* If no sparse metrics are stored so far. */
    next = node->first_double_sparse;
    if ( next == NULL )
    {
        node->first_double_sparse = silc_profile_create_sparse_double( metric, value );
        return;
    }

    /* Iterate all existing sparse metrics */
    do
    {
        current = next;
        if ( current->metric == metric )
        {
            silc_profile_update_sparse_double( current, value );
            return;
        }
        next = current->next_metric;
    }
    while ( next != NULL );

    /* Append new sparse metric */
    current->next_metric = silc_profile_create_sparse_double( metric, value );
}

void
SILC_Profile_ParameterString( SILC_Thread_LocationData* thread,
                              SILC_ParameterHandle      param,
                              SILC_StringHandle         string )
{
    silc_profile_node*            node = NULL;
    silc_profile_string_node_data data;
    int                           i;

    SILC_PROFILE_ASSURE_INITIALIZED;

    /* Initialize type specific data */
    data.handle = param;
    data.value  = string;

    /* Get new callpath node */
    node = silc_profile_find_or_create_child( thread, silc_profile_node_parameter_string,
                                              ( int )&data, -1 );

    /* Store start values for dense metrics */
    silc_profile_setup_start_from_parent( node );

    /* Update current node pointer */
    silc_profile_set_current_node( thread, node );
}

void
SILC_Profile_ParameterInteger( SILC_Thread_LocationData* thread,
                               SILC_ParameterHandle      param,
                               int64_t                   value )
{
    silc_profile_node*             node = NULL;
    silc_profile_integer_node_data data;
    int                            i;

    SILC_PROFILE_ASSURE_INITIALIZED;

    /* Initialize type specific data */
    data.handle = param;
    data.value  = value;

    /* Get new callpath node */
    node = silc_profile_find_or_create_child( thread, silc_profile_node_parameter_string,
                                              ( int )&data, -1 );

    /* Store start values for dense metrics */
    silc_profile_setup_start_from_parent( node );

    /* Update current node pointer */
    silc_profile_set_current_node( thread, node );
}

/* ***************************************************************************************
   Thread events
*****************************************************************************************/

void
SILC_Profile_OnThreadCreation( SILC_Thread_LocationData* locationData,
                               SILC_Thread_LocationData* parentLocationData )
{
}

void
SILC_Profile_OnThreadActivation( SILC_Thread_LocationData* locationData,
                                 SILC_Thread_LocationData* parentLocationData )
{
    SILC_Profile_LocationData* thread_data    = NULL;
    SILC_Profile_LocationData* parent_data    = NULL;
    silc_profile_node*         root           = NULL;
    silc_profile_node*         node           = NULL;
    silc_profile_node*         creation_point = NULL;

    SILC_PROFILE_ASSURE_INITIALIZED;
    SILC_ASSERT( locationData != NULL );

    /* Get root node of the thread */
    thread_data = SILC_Thread_GetProfileLocationData( locationData );
    if ( thread_data == NULL )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Thread activated which was not created." );
        SILC_PROFILE_STOP;
        return;
    }
    root = thread_data->root_node;
    SILC_ASSERT( root != NULL );

    /* Find creation point if available */
    if ( parentLocationData != NULL )
    {
        parent_data = SILC_Thread_GetProfileLocationData( parentLocationData );
        if ( parent_data != NULL )
        {
            creation_point = parent_data->fork_node;
        }
    }

    /* Check wether such an activation node already exists */
    node = root->first_child;
    while ( ( node != NULL ) &&
            ( node->node_type == silc_profile_node_thread_start ) &&
            ( creation_point == ( silc_profile_node* )( int )node->type_specific_data ) )
    {
        node = node->next_sibling;
    }

    /* Create new node if no one exists */
    if ( node == NULL )
    {
        node = silc_profile_create_node( root, silc_profile_node_thread_start,
                                         ( int )creation_point, 0 );
        node->next_sibling = root->first_child;
        root->first_child  = node;
    }

    /* Now node points to the starting point of the thread.
       Make it the current node of the thread. */
    thread_data->current_node = node;
}


void
SILC_Profile_OnThreadDectivation( SILC_Thread_LocationData* locationData,
                                  SILC_Thread_LocationData* parentLocationData )
{
    /* Remove the current node. */
    SILC_Thread_GetProfileLocationData( locationData )->current_node = NULL;
}


void
SILC_Profile_OnLocationCreation( SILC_Thread_LocationData* locationData,
                                 SILC_Thread_LocationData* parentLocationData )
{
    SILC_Profile_LocationData* parent_data = NULL;
    SILC_Profile_LocationData* thread_data = NULL;
    silc_profile_node*         node        = NULL;

    SILC_PROFILE_ASSURE_INITIALIZED;

    /* Create thread root node */
    node = silc_profile_create_node( NULL, silc_profile_node_thread_root,
                                     SILC_Thread_GetLocationId( locationData ), 0 );

    /* Update thread location data */
    thread_data = SILC_Thread_GetProfileLocationData( locationData );
    SILC_ASSERT( thread_data != NULL );
    thread_data->root_node = node;

    if ( parentLocationData != NULL )
    {
        parent_data                = SILC_Thread_GetProfileLocationData( parentLocationData );
        thread_data->creation_node = parent_data->fork_node;
    }

    /* Add it to the profile node list */
    if ( parent_data == NULL )
    {
        /* It is the initial thread. Insert as first new root node */
        node->next_sibling           = silc_profile.first_root_node;
        silc_profile.first_root_node = node;
    }
    else
    {
        /* Append after parent root node */
        node->next_sibling                   = parent_data->root_node->next_sibling;
        parent_data->root_node->next_sibling = node;
    }
}

void
SILC_Profile_OnFork( SILC_Thread_LocationData* threadData,
                     size_t                    maxChildThreads )
{
    silc_profile_node* node  = NULL;
    silc_profile_node* child = NULL;
    int                i;

    SILC_PROFILE_ASSURE_INITIALIZED;

    node = silc_profile_find_or_create_child( threadData, silc_profile_node_thread_fork,
                                              0, -1 );

    /* Store current fork node */
    SILC_Thread_GetProfileLocationData( threadData )->fork_node = node;
}
