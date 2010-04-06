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
 * @file silc_profile_tree.c Implementation of functions to construct/destruct the
 * profile tree.
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status ALPHA
 *
 */

#include "SILC_Memory.h"
#include "silc_profile_types.h"
#include "silc_profile_tree.h"
#include "silc_profile_thread_interaction.h"
#include "SILC_Utils.h"
#include "silc_thread.h"

/*----------------------------------------------------------------------------------------
   gobal variables
   ----------------------------------------------------------------------------------------*/
silc_profile_definition silc_profile;

bool                    silc_profile_is_initialized = false;

/*----------------------------------------------------------------------------------------
   helper for thread handling
   ----------------------------------------------------------------------------------------*/
/** Returns the current node for a thread */
silc_profile_node*
silc_profile_get_current_node( SILC_Thread_LocationData* thread )
{
    return SILC_Thread_GetProfileLocationData( thread )->current_node;
}

/** Sets the current node for a thread */
inline void
silc_profile_set_current_node( SILC_Thread_LocationData* thread,
                               silc_profile_node*        node )
{
    SILC_Thread_GetProfileLocationData( thread )->current_node = node;
}

/*----------------------------------------------------------------------------------------
   Constructors / destriuctors
   ----------------------------------------------------------------------------------------*/

/** Initializes the profile definition struct
 */
void
silc_profile_init_definition( int32_t             numDenseMetrics,
                              SILC_CounterHandle* metrics )
{
    int i;
    silc_profile.first_root_node      = NULL;
    silc_profile.num_of_dense_metrics = numDenseMetrics;
    silc_profile.dense_metrics        = SILC_Memory_AllocForProfile( numDenseMetrics * sizeof( SILC_CounterHandle ) );

    for ( i = 0; i < numDenseMetrics; i++ )
    {
        silc_profile.dense_metrics[ i ] = metrics[ i ];
    }
}

/** Resets the profile definition struct
 */
void
silc_profile_delete_definition()
{
    silc_profile.first_root_node      = NULL;
    silc_profile.num_of_dense_metrics = 0;
    if ( silc_profile.dense_metrics )
    {
        silc_profile.dense_metrics = NULL;
    }
}

/** Initializes a dense metric */
void
silc_profile_init_dense_metric( silc_profile_dense_metric* metric )
{
    metric->sum         = 0;
    metric->min         = UINT64_MAX;
    metric->max         = 0;
    metric->squares     = 0;
    metric->start_value = 0;
}

/** Creates a new child node of given type and data */
silc_profile_node*
silc_profile_create_node( silc_profile_node*     parent,
                          silc_profile_node_type type,
                          uint64_t               data,
                          uint64_t               timestamp )
{
    int i;

    /* Reserve space for the node record and dense metrics */
    silc_profile_node* node = ( silc_profile_node* )
                              SILC_Memory_AllocForProfile( sizeof( silc_profile_node )
                                                           + silc_profile.num_of_dense_metrics
                                                           * sizeof( silc_profile_dense_metric ) );
    if ( !node )
    {
        SILC_ERROR_POSIX();
        return NULL;
    }

    /* Space for dense metrics are reserved after the node struct */
    node->dense_metrics =
        ( silc_profile_dense_metric* )( ( void* )node + sizeof( silc_profile_node ) );

    /* Initialize values */
    node->callpath_handle     = SILC_INVALID_CALLPATH;
    node->parent              = parent;
    node->first_child         = NULL;
    node->next_sibling        = NULL;
    node->first_double_sparse = NULL;
    node->first_int_sparse    = NULL;
    node->count               = 0; /* Is increased to one during SILC_Profile_Enter() */
    node->first_enter_time    = timestamp;
    node->last_exit_time      = timestamp;
    node->node_type           = type;
    node->type_specific_data  = data;

    /* Initialize dense metric values */
    silc_profile_init_dense_metric( &node->implicit_time );
    for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
    {
        silc_profile_init_dense_metric( &node->dense_metrics[ i ] );
    }

    return node;
}


/** Creates a new child node of type 'regular region' */
inline silc_profile_node*
silc_profile_create_region_node( silc_profile_node* parent,
                                 SILC_RegionHandle  region,
                                 uint64_t           timestamp )
{
    return silc_profile_create_node( parent, silc_profile_node_regular_region, region,
                                     timestamp );
}

/** Creates a new child node of type 'parameter integer' */
inline silc_profile_node*
silc_profile_create_integer_node( silc_profile_node*   parent,
                                  SILC_ParameterHandle handle,
                                  int64_t              value )
{
    silc_profile_integer_node_data* data =
        SILC_Memory_AllocForProfile( sizeof( silc_profile_integer_node_data ) );
    data->handle = handle;
    data->value  = value;
    return silc_profile_create_node( parent, silc_profile_node_parameter_integer,
                                     ( int )data, parent->implicit_time.start_value );
}

/** Creates a new child node of type 'parameter string' */
inline silc_profile_node*
silc_profile_create_string_node( silc_profile_node*   parent,
                                 SILC_ParameterHandle handle,
                                 SILC_StringHandle    value )
{
    silc_profile_string_node_data* data =
        SILC_Memory_AllocForProfile( sizeof( silc_profile_string_node_data ) );
    data->handle = handle;
    data->value  = value;
    return silc_profile_create_node( parent, silc_profile_node_parameter_string,
                                     ( int )data, parent->implicit_time.start_value );
}

/** Creates a new sparse metric struct instance for integer values */
silc_profile_sparse_metric_int*
silc_profile_create_sparse_int(
    SILC_CounterHandle metric,
    uint64_t           value )
{
    silc_profile_sparse_metric_int* new_sparse =
        SILC_Memory_AllocForProfile( sizeof( silc_profile_sparse_metric_int ) );
    new_sparse->metric      = metric;
    new_sparse->count       = 1;
    new_sparse->sum         = value;
    new_sparse->min         = value;
    new_sparse->max         = value;
    new_sparse->squares     = value * value;
    new_sparse->next_metric = NULL;
    return new_sparse;
}

/** Creates a new sparse metric struct instance for double values */
silc_profile_sparse_metric_double*
silc_profile_create_sparse_double(
    SILC_CounterHandle metric,
    double             value )
{
    silc_profile_sparse_metric_double* new_sparse =
        SILC_Memory_AllocForProfile( sizeof( silc_profile_sparse_metric_double ) );
    new_sparse->metric      = metric;
    new_sparse->count       = 1;
    new_sparse->sum         = value;
    new_sparse->min         = value;
    new_sparse->max         = value;
    new_sparse->squares     = value * value;
    new_sparse->next_metric = NULL;
    return new_sparse;
}

/*----------------------------------------------------------------------------------------
   Helper functions
   ----------------------------------------------------------------------------------------*/

/** Updates a sparse metric struct instance for integer values */
void
silc_profile_update_sparse_int( silc_profile_sparse_metric_int* metric,
                                uint64_t                        value )
{
    metric->count++;
    metric->sum += value;
    if ( metric->min > value )
    {
        metric->min = value;
    }
    if ( metric->max < value )
    {
        metric->max = value;
    }
    metric->squares += value * value;
}

/** Updates a sparse metric struct instance for double values */
void
silc_profile_update_sparse_double( silc_profile_sparse_metric_double* metric,
                                   uint64_t                           value )
{
    metric->count++;
    metric->sum += value;
    if ( metric->min > value )
    {
        metric->min = value;
    }
    if ( metric->max < value )
    {
        metric->max = value;
    }
    metric->squares += value * value;
}


/** Updates the statistics of one dense metric on an exit event. */
inline void
silc_profile_update_dense_metric( silc_profile_dense_metric* metric,
                                  uint64_t                   end_value )
{
    double diff = end_value - metric->start_value;
    metric->sum += diff;
    if ( metric->min > diff )
    {
        metric->min = diff;
    }
    if ( metric->max < diff )
    {
        metric->max = diff;
    }
    metric->squares += diff * diff;
}

/** Compares if a node equals a given string parameter */
inline bool
silc_profile_compare_parameter_string( silc_profile_node*   node,
                                       SILC_ParameterHandle handle,
                                       SILC_StringHandle    value )
{
    silc_profile_string_node_data* data;
    if ( node->node_type != silc_profile_node_parameter_string )
    {
        return false;
    }
    data = ( silc_profile_string_node_data* )( int )node->type_specific_data;
    return ( data->handle == handle ) && ( data->value == value );
}

/** Compares if a node equals a given integer parameter */
inline bool
silc_profile_compare_parameter_int( silc_profile_node*   node,
                                    SILC_ParameterHandle handle,
                                    uint64_t             value )
{
    silc_profile_integer_node_data* data;
    if ( node->node_type != silc_profile_node_parameter_integer )
    {
        return false;
    }
    data = ( silc_profile_integer_node_data* )( int )node->type_specific_data;
    return ( data->handle == handle ) && ( data->value == value );
}

/*----------------------------------------------------------------------------------------
   Debug
   -------------------------------------------------------------------------------------*/
void
silc_profile_dump_node( silc_profile_node* node )
{
    printf( "\nNode: %p\n", node );
    if ( node == NULL )
    {
        return;
    }
    printf( "node type: %d\n", node->node_type );
    printf( "Parent: %p\n", node->parent );
    printf( "first child: %p\n", node->first_child );
    printf( "next_sibling: %p\n", node->next_sibling );
    printf( "visit count: %d\n", node->count );
    printf( "implicit time: %d\n\n", ( int )node->implicit_time.sum );
}

void
silc_profile_dump_subtree( silc_profile_node* node )
{
    silc_profile_dump_node( node );
    if ( node == NULL )
    {
        return;
    }
    if ( node->first_child != NULL )
    {
        silc_profile_dump_subtree( node->first_child );
    }
    if ( node->next_sibling != NULL )
    {
        silc_profile_dump_subtree( node->next_sibling );
    }
}

void
silc_profile_dump()
{
    silc_profile_dump_subtree( silc_profile.first_root_node );
}

/*----------------------------------------------------------------------------------------
   Interface implementation
   -------------------------------------------------------------------------------------*/

void
SILC_Profile_Initialize( int32_t             numDenseMetrics,
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

    silc_profile_init_definition( numDenseMetrics, metrics );
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

void
SILC_Profile_Enter( SILC_Thread_LocationData* thread,
                    SILC_RegionHandle         region,
                    SILC_RegionType           type,
                    uint64_t                  timestamp,
                    uint64_t*                 metrics )
{
    silc_profile_node* node  = NULL;
    silc_profile_node* child = NULL;
    int                i;

    SILC_PROFILE_ASSURE_INITIALIZED;

    node = silc_profile_get_current_node( thread );

    /* Find or create child node:
       Test if it is the first region of the profile or thread */
    if ( node == NULL )
    {
        node = silc_profile_create_region_node( NULL, region, timestamp );
        if ( silc_profile.first_root_node == NULL )
        {
            silc_profile.first_root_node = node;
        }
    }
    /* It is a subnode of an existing tree: Traverse childen */
    else
    {
        child = node->first_child;
        /* It is the first child */
        if ( child == NULL )
        {
            node->first_child = silc_profile_create_region_node( node, region, timestamp );
            node              = node->first_child;
        }
        /* At least one child exists: Search if the region was entered before */
        else
        {
            while ( ( child->next_sibling != NULL ) &&
                    ( ( child->node_type != silc_profile_node_regular_region ) ||
                      ( child->type_specific_data != region ) ) )
            {
                child = child->next_sibling;
            }
            /* No matching region found: Create a new sibling */
            if ( ( child->node_type != silc_profile_node_regular_region ) ||
                 ( child->type_specific_data != region ) )
            {
                child->next_sibling = silc_profile_create_region_node( node, region, timestamp );
                node                = child->next_sibling;
            }
            /* Call path was already visited */
            else
            {
                node = child;
            }
        }
    }
    /* Now node points to the newly entered region */

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
    int                i;
    silc_profile_node* node   = NULL;
    silc_profile_node* parent = NULL;

    SILC_PROFILE_ASSURE_INITIALIZED;

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
        //node->count++;
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
         ( node->type_specific_data != region ) )
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
    silc_profile_node* node   = NULL;
    silc_profile_node* child  = NULL;
    silc_profile_node* parent = NULL;
    int                i;

    SILC_PROFILE_ASSURE_INITIALIZED;

    /* Get current node */
    node = silc_profile_get_current_node( thread );
    if ( node == NULL )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Current region unknown." );
        SILC_PROFILE_STOP;
        return;
    }

    /* Find or create the child node */

    /* Check wether the node has children */
    child = node->first_child;
    if ( child == NULL )
    {
        node->first_child = silc_profile_create_string_node( node, param, string );
        node              = node->first_child;
    }
    /* At least one child exists: Search if the region was entered before */
    else
    {
        while ( ( child->next_sibling != NULL ) &&
                !silc_profile_compare_parameter_string( child, param, string ) )
        {
            child = child->next_sibling;
        }
        /* No matching region found: Create a new sibling */
        if ( !silc_profile_compare_parameter_string( child, param, string ) )
        {
            child->next_sibling = silc_profile_create_string_node( node, param,
                                                                   string );
            node = child->next_sibling;
        }
        /* Call path was already visited */
        else
        {
            node = child;
        }
    }
    /* Now node points to the newly entered region */

    /* Store start values for dense metrics */
    parent = node->parent;
    node->count++;
    node->implicit_time.start_value = parent->implicit_time.start_value;
    for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
    {
        node->dense_metrics[ i ].start_value = parent->dense_metrics[ i ].start_value;
    }

    /* Update current node pointer */
    silc_profile_set_current_node( thread, node );
}

void
SILC_Profile_ParameterInteger( SILC_Thread_LocationData* thread,
                               SILC_ParameterHandle      param,
                               int64_t                   value )
{
    silc_profile_node* node   = NULL;
    silc_profile_node* child  = NULL;
    silc_profile_node* parent = NULL;
    int                i;

    SILC_PROFILE_ASSURE_INITIALIZED;

    /* Get current node */
    node = silc_profile_get_current_node( thread );
    if ( node == NULL )
    {
        SILC_ERROR( SILC_ERROR_PROFILE_INCONSISTENT,
                    "Current region unknown." );
        SILC_PROFILE_STOP;
        return;
    }

    /* Find or create the child node */

    /* Check wether the node has children */
    child = node->first_child;
    if ( child == NULL )
    {
        node->first_child = silc_profile_create_integer_node( node, param, value );
        node              = node->first_child;
    }
    /* At least one child exists: Search if the region was entered before */
    else
    {
        while ( ( child->next_sibling != NULL ) &&
                ( !silc_profile_compare_parameter_int( child, param, value ) ) )
        {
            child = child->next_sibling;
        }
        /* No matching region found: Create a new sibling */
        if ( !silc_profile_compare_parameter_int( child, param, value ) )
        {
            child->next_sibling = silc_profile_create_integer_node( node, param,
                                                                    value );
            node = child->next_sibling;
        }
        /* Call path was already visited */
        else
        {
            node = child;
        }
    }
    /* Now node points to the newly entered region */

    /* Store start values for dense metrics */
    parent = node->parent;
    node->count++;
    node->implicit_time.start_value = parent->implicit_time.start_value;
    for ( i = 0; i < silc_profile.num_of_dense_metrics; i++ )
    {
        node->dense_metrics[ i ].start_value = parent->dense_metrics[ i ].start_value;
    }

    /* Update current node pointer */
    silc_profile_set_current_node( thread, node );
}
