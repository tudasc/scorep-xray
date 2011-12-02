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
 * @file scorep_profile_node.c
 * Implementation for managing profile node trees.
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <stddef.h>

#include "SCOREP_Memory.h"
#include "scorep_utility/SCOREP_Utils.h"

#include "scorep_profile_node.h"
#include "scorep_profile_definition.h"


/* ***************************************************************************************
   Prototypes of internal comparison and copy functions for type dependent data.
*****************************************************************************************/
static scorep_profile_type_data_t
scorep_profile_copy_regular_region_data( scorep_profile_type_data_t data );

static scorep_profile_type_data_t
scorep_profile_copy_parameter_string_data( scorep_profile_type_data_t data );

static scorep_profile_type_data_t
scorep_profile_copy_parameter_integer_data( scorep_profile_type_data_t data );

static scorep_profile_type_data_t
scorep_profile_copy_thread_root_data( scorep_profile_type_data_t data );

static scorep_profile_type_data_t
scorep_profile_copy_thread_start_data( scorep_profile_type_data_t data );

static scorep_profile_type_data_t
scorep_profile_copy_collapse_data( scorep_profile_type_data_t data );

static bool
scorep_profile_compare_regular_region_data( scorep_profile_type_data_t data1,
                                            scorep_profile_type_data_t data2 );

static bool
scorep_profile_compare_parameter_string_data( scorep_profile_type_data_t data1,
                                              scorep_profile_type_data_t data2 );

static bool
scorep_profile_compare_parameter_integer_data( scorep_profile_type_data_t data1,
                                               scorep_profile_type_data_t data2 );

static bool
scorep_profile_compare_thread_root_data( scorep_profile_type_data_t data1,
                                         scorep_profile_type_data_t data2 );

static bool
scorep_profile_compare_thread_start_data( scorep_profile_type_data_t data1,
                                          scorep_profile_type_data_t data2 );

static bool
scorep_profile_compare_collapse_data( scorep_profile_type_data_t data1,
                                      scorep_profile_type_data_t data2 );

/* ***************************************************************************************
   Type dependent data handling types and variables
*****************************************************************************************/

/**
   Type for entries in the functions table list @a scorep_profile_type_data_funcs, which
   contains functions for handling type dependent data. Currently, it contains a
   pointer to a comparison function and a pointer to a copy function.
 */
typedef struct
{
    bool ( * comp_func )( scorep_profile_type_data_t data1,
                          scorep_profile_type_data_t data2 );
    scorep_profile_type_data_t ( * copy_func )( scorep_profile_type_data_t data );
} scorep_profile_type_data_func_t;

/* *INDENT-OFF* */

/**
  Functions table list for handling type dependent data. All entries must occur in the
  same order like in @a scorep_profile_node_type.
 */
scorep_profile_type_data_func_t scorep_profile_type_data_funcs[] = {
  { &scorep_profile_compare_regular_region_data,    &scorep_profile_copy_regular_region_data    },
  { &scorep_profile_compare_parameter_string_data,  &scorep_profile_copy_parameter_string_data  },
  { &scorep_profile_compare_parameter_integer_data, &scorep_profile_copy_parameter_integer_data },
  { &scorep_profile_compare_thread_root_data,       &scorep_profile_copy_thread_root_data       },
  { &scorep_profile_compare_thread_start_data,      &scorep_profile_copy_thread_start_data      },
  { &scorep_profile_compare_collapse_data,          &scorep_profile_copy_collapse_data          }
};

/* *INDENT-ON* */

/* ***************************************************************************************
   Implementation of comparison ond copy functions for type dependent data.
*****************************************************************************************/

static scorep_profile_type_data_t
scorep_profile_copy_regular_region_data( scorep_profile_type_data_t data )
{
    return data;
}

static scorep_profile_type_data_t
scorep_profile_copy_parameter_string_data( scorep_profile_type_data_t data )
{
    scorep_profile_string_node_data* old_data = SCOREP_PROFILE_DATA2PARAMSTR( data );
    scorep_profile_string_node_data* new_data =
        SCOREP_Memory_AllocForProfile( sizeof( scorep_profile_string_node_data ) );
    new_data->handle = old_data->handle;
    new_data->value  = old_data->value;
    return SCOREP_PROFILE_PARAMSTR2DATA( new_data );
}

static scorep_profile_type_data_t
scorep_profile_copy_parameter_integer_data( scorep_profile_type_data_t data )
{
    scorep_profile_integer_node_data* old_data = SCOREP_PROFILE_DATA2PARAMINT( data );
    scorep_profile_integer_node_data* new_data =
        SCOREP_Memory_AllocForProfile( sizeof( scorep_profile_integer_node_data ) );
    new_data->handle = old_data->handle;
    new_data->value  = old_data->value;
    return SCOREP_PROFILE_PARAMINT2DATA( new_data );
}

static scorep_profile_type_data_t
scorep_profile_copy_thread_fork_data( scorep_profile_type_data_t data )
{
    return data;
}

static scorep_profile_type_data_t
scorep_profile_copy_thread_root_data( scorep_profile_type_data_t data )
{
    scorep_profile_root_node_data* old_data = SCOREP_PROFILE_DATA2THREADROOT( data );
    scorep_profile_root_node_data* new_data =
        SCOREP_Memory_AllocForMisc( sizeof( scorep_profile_root_node_data ) );
    new_data->thread_data = old_data->thread_data;
    new_data->thread_id   = old_data->thread_id;
    return SCOREP_PROFILE_THREADROOT2DATA( new_data );
}

static scorep_profile_type_data_t
scorep_profile_copy_thread_start_data( scorep_profile_type_data_t data )
{
    return data;
}

static scorep_profile_type_data_t
scorep_profile_copy_collapse_data( scorep_profile_type_data_t data )
{
    return data;
}

static bool
scorep_profile_compare_regular_region_data( scorep_profile_type_data_t data1,
                                            scorep_profile_type_data_t data2 )
{
    return data1 == data2;
}

static bool
scorep_profile_compare_parameter_string_data( scorep_profile_type_data_t data1,
                                              scorep_profile_type_data_t data2 )
{
    scorep_profile_string_node_data* string_data1 = SCOREP_PROFILE_DATA2PARAMSTR( data1 );
    scorep_profile_string_node_data* string_data2 = SCOREP_PROFILE_DATA2PARAMSTR( data2 );
    return ( string_data1->handle == string_data2->handle ) &&
           ( string_data1->value  == string_data2->value );
}

static bool
scorep_profile_compare_parameter_integer_data( scorep_profile_type_data_t data1,
                                               scorep_profile_type_data_t data2 )
{
    scorep_profile_integer_node_data* string_data1 = SCOREP_PROFILE_DATA2PARAMINT( data1 );
    scorep_profile_integer_node_data* string_data2 = SCOREP_PROFILE_DATA2PARAMINT( data2 );
    return ( string_data1->handle == string_data2->handle ) &&
           ( string_data1->value  == string_data2->value );
}

static bool
scorep_profile_compare_thread_fork_data( scorep_profile_type_data_t data1,
                                         scorep_profile_type_data_t data2 )
{
    return true;
}

static bool
scorep_profile_compare_thread_root_data( scorep_profile_type_data_t data1,
                                         scorep_profile_type_data_t data2 )
{
    scorep_profile_root_node_data* root1 = SCOREP_PROFILE_DATA2THREADROOT( data1 );
    scorep_profile_root_node_data* root2 = SCOREP_PROFILE_DATA2THREADROOT( data2 );
    return root1->thread_id == root2->thread_id;
}

static bool
scorep_profile_compare_thread_start_data( scorep_profile_type_data_t data1,
                                          scorep_profile_type_data_t data2 )
{
    return data1 == data2;
}

static bool
scorep_profile_compare_collapse_data( scorep_profile_type_data_t data1,
                                      scorep_profile_type_data_t data2 )
{
    return true;
}

/* ***************************************************************************************
   Type dependent functions
*****************************************************************************************/

/* Compares to data sets */
bool
scorep_profile_compare_type_data( scorep_profile_type_data_t data1,
                                  scorep_profile_type_data_t data2,
                                  scorep_profile_node_type   type )
{
    return ( *scorep_profile_type_data_funcs[ type ].comp_func )( data1, data2 );
}

/* Copies a data set */
scorep_profile_type_data_t
scorep_profile_copy_type_data( scorep_profile_type_data_t data,
                               scorep_profile_node_type   type )
{
    return ( *scorep_profile_type_data_funcs[ type ].copy_func )( data );
}



/* ***************************************************************************************
   Creation / Destruction
*****************************************************************************************/

/** Creates a new child node of given type and data */
scorep_profile_node*
scorep_profile_create_node( scorep_profile_node*       parent,
                            scorep_profile_node_type   type,
                            scorep_profile_type_data_t data,
                            uint64_t                   timestamp )
{
    int                  i;
    scorep_profile_node* node = NULL;

    /* Size of the allocated memory for dense metrics */
    int size = scorep_profile.num_of_dense_metrics * sizeof( scorep_profile_dense_metric );

    /* Reserve space for the node record and dense metrics.
     *  Thread root nodes must not be deleted in Persicope phases, while all other
     *  nodes are. The profile memory pages are deleted in Persicope phases.
     *  Thus, space for thread root nodes must not be allocated
     *  from profile memory.
     */
    if ( type == scorep_profile_node_thread_root )
    {
        node = ( scorep_profile_node* )
               SCOREP_Memory_AllocForMisc( sizeof( scorep_profile_node ) );
    }
    else
    {
        node = ( scorep_profile_node* )
               SCOREP_Memory_AllocForProfile( sizeof( scorep_profile_node ) );
    }
    if ( !node )
    {
        SCOREP_ERROR( SCOREP_ERROR_MEM_FAULT, "Unable to allocate memory for new node" );
        return NULL;
    }

    /* Reserve space for dense metrics */
    if ( scorep_profile.num_of_dense_metrics > 0 )
    {
        if ( type == scorep_profile_node_thread_root )
        {
            node->dense_metrics = ( scorep_profile_dense_metric* )
                                  SCOREP_Memory_AllocForMisc( size );
        }
        else
        {
            node->dense_metrics = ( scorep_profile_dense_metric* )
                                  SCOREP_Memory_AllocForProfile( size );
        }
        if ( !node->dense_metrics )
        {
            SCOREP_ERROR( SCOREP_ERROR_MEM_FAULT,
                          "Unable to allocate memory for dense metrics" );
            return NULL;
        }
    }
    else
    {
        node->dense_metrics = NULL;
    }

    /* Initialize values */
    node->callpath_handle     = SCOREP_INVALID_CALLPATH;
    node->parent              = parent;
    node->first_child         = NULL;
    node->next_sibling        = NULL;
    node->first_double_sparse = NULL;
    node->first_int_sparse    = NULL;
    node->count               = 0; /* Is increased to one during SCOREP_Profile_Enter() */
    node->first_enter_time    = timestamp;
    node->last_exit_time      = timestamp;
    node->node_type           = type;
    node->type_specific_data  = scorep_profile_copy_type_data( data, type );

    /* Initialize dense metric values */
    scorep_profile_init_dense_metric( &node->inclusive_time );
    for ( i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
    {
        scorep_profile_init_dense_metric( &node->dense_metrics[ i ] );
    }

    return node;
}

/* Copies a node */
scorep_profile_node*
scorep_profile_copy_node( scorep_profile_node* source )
{
    scorep_profile_sparse_metric_int*    dest_sparse_int      = NULL;
    scorep_profile_sparse_metric_int*    source_sparse_int    = source->first_int_sparse;
    scorep_profile_sparse_metric_double* dest_sparse_double   = NULL;
    scorep_profile_sparse_metric_double* source_sparse_double = source->first_double_sparse;

    /* Create node */
    scorep_profile_node* node = scorep_profile_create_node( NULL, source->node_type,
                                                            source->type_specific_data, 0 );

    /* Copy dense metric values */
    scorep_profile_copy_all_dense_metrics( node, source );

    /* Copy sparse integer metrics */
    while ( source_sparse_int != NULL )
    {
        dest_sparse_int              = scorep_profile_copy_sparse_int( source_sparse_int );
        dest_sparse_int->next_metric = node->first_int_sparse;
        node->first_int_sparse       = dest_sparse_int;

        source_sparse_int = source_sparse_int->next_metric;
    }

    /* Copy sparse double metrics */
    while ( source_sparse_double != NULL )
    {
        dest_sparse_double              = scorep_profile_copy_sparse_double( source_sparse_double );
        dest_sparse_double->next_metric = node->first_double_sparse;
        node->first_double_sparse       = dest_sparse_double;

        source_sparse_double = source_sparse_double->next_metric;
    }

    return node;
}

/* ***************************************************************************************
   Node operation
*****************************************************************************************/

void
scorep_profile_add_child( scorep_profile_node* parent,
                          scorep_profile_node* child )
{
    child->next_sibling = parent->first_child;
    parent->first_child = child;
    child->parent       = parent;
}

bool
scorep_profile_compare_nodes( scorep_profile_node* node1,
                              scorep_profile_node* node2 )
{
    if ( node1->node_type != node2->node_type )
    {
        return false;
    }
    return scorep_profile_compare_type_data( node1->type_specific_data,
                                             node2->type_specific_data,
                                             node1->node_type );
}

/* Copies all dense metrics from source to destination */
void
scorep_profile_copy_all_dense_metrics( scorep_profile_node* destination,
                                       scorep_profile_node* source )
{
    int i;

    destination->count            = source->count;
    destination->first_enter_time = source->first_enter_time;
    destination->last_exit_time   = source->last_exit_time;

    /* Copy dense metric values */
    scorep_profile_copy_dense_metric( &destination->inclusive_time, &source->inclusive_time );
    for ( i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
    {
        scorep_profile_copy_dense_metric( &destination->dense_metrics[ i ],
                                          &source->dense_metrics[ i ] );
    }
}

/* Moves the children of a node to another node */
void
scorep_profile_move_children(  scorep_profile_node* destination,
                               scorep_profile_node* source )
{
    scorep_profile_node* child = NULL;

    SCOREP_ASSERT( source != NULL );

    /* If source has no child -> nothing to do */
    child = source->first_child;
    if ( child == NULL )
    {
        return;
    }

    /* Set new parent of all children of source. */
    while ( child != NULL )
    {
        child->parent = destination;
        child         = child->next_sibling;
    }

    /* If destination is NULL -> append all children as root nodes */
    if ( destination == NULL )
    {
        child = scorep_profile.first_root_node;

        /* If profile is empty */
        if ( child == NULL )
        {
            scorep_profile.first_root_node = source->first_child;
            source->first_child            = NULL;
            return;
        }

        /* Append at end of root nodes, see below */
    }
    else
    {
        /* If destination has no children */
        child = destination->first_child;
        if ( child == NULL )
        {
            destination->first_child = source->first_child;
            source->first_child      = NULL;
            return;
        }
        /* Else append list of source to end of children list of destination, see below */
    }

    /* Search end of sibling list and append children */
    while ( child->next_sibling != NULL )
    {
        child = child->next_sibling;
    }
    child->next_sibling = source->first_child;
    source->first_child = NULL;
}

/* Removes a node with it subtree from its parents children */
void
scorep_profile_remove_node( scorep_profile_node* node )
{
    SCOREP_ASSERT( node != NULL );

    scorep_profile_node* parent = node->parent;
    scorep_profile_node* before = NULL;

    /* Obtain start of the siblings list of node */
    if ( parent == NULL )
    {
        before = scorep_profile.first_root_node;
    }
    else
    {
        before = parent->first_child;
    }

    /* If node is the first entry */
    if ( before == node )
    {
        if ( parent == NULL )
        {
            scorep_profile.first_root_node = node->next_sibling;
        }
        else
        {
            parent->first_child = node->next_sibling;
        }
        node->parent       = NULL;
        node->next_sibling = NULL;
        return;
    }

    /* Else search for the sibling before node. */
    while ( ( before != NULL ) && ( before->next_sibling != node ) )
    {
        before = before->next_sibling;
    }

    /* Node is already removed */
    if ( before == NULL )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_PROFILE,
                             "Trying to remove a node which is not contained in the siblings "
                             "list.\nMaybe an inconsistent profile." );
        node->parent       = NULL;
        node->next_sibling = NULL;
        return;
    }

    /* Remove node from list */
    before->next_sibling = node->next_sibling;
    node->parent         = NULL;
    node->next_sibling   = NULL;
}

/* Traverse a subtree (depth search) and execute a given function on each node */
void
scorep_profile_for_all( scorep_profile_node*           root_node,
                        scorep_profile_process_func_t* func,
                        void*                          param )
{
    scorep_profile_node* current = root_node;

    /* Process root node */
    if ( current == NULL )
    {
        return;
    }
    ( *func )( current, param );

    /* Process children */
    current = current->first_child;
    if ( current == NULL )
    {
        return;
    }

    while ( current != root_node )
    {
        ( *func )( current, param );

        /* Find next node */
        if ( current->first_child != NULL )
        {
            current = current->first_child;
        }
        else
        {
            do
            {
                if ( current->next_sibling != NULL )
                {
                    current = current->next_sibling;
                    break;
                }
                current = current->parent;
            }
            while ( current != root_node );
        }
    }
}

/* Finds a child node of a specified type */
scorep_profile_node*
scorep_profile_find_child( scorep_profile_node* parent,
                           scorep_profile_node* type )
{
    /* Search matching node */
    SCOREP_ASSERT( parent != NULL );
    scorep_profile_node* child = parent->first_child;
    while ( ( child != NULL ) &&
            !scorep_profile_compare_nodes( child, type ) )
    {
        child = child->next_sibling;
    }

    return child;
}

/* Find or create a child node of a specified type */
scorep_profile_node*
scorep_profile_find_create_child( scorep_profile_node* parent,
                                  scorep_profile_node* type,
                                  uint64_t             timestamp )
{
    /* Search matching node */
    SCOREP_ASSERT( parent != NULL );
    scorep_profile_node* child = parent->first_child;
    while ( ( child != NULL ) &&
            !scorep_profile_compare_nodes( child, type ) )
    {
        child = child->next_sibling;
    }

    /* If not found -> create new node */
    if ( child == NULL )
    {
        child = scorep_profile_create_node( parent, type->node_type,
                                            type->type_specific_data,
                                            timestamp );
        child->next_sibling = parent->first_child;
        parent->first_child = child;
    }

    return child;
}

uint64_t
scorep_profile_get_number_of_children( scorep_profile_node* node )
{
    uint64_t             count = 0;
    scorep_profile_node* child = NULL;

    if ( node == NULL )
    {
        return 0;
    }

    child = node->first_child;
    while ( child != NULL )
    {
        count++;
        child = child->next_sibling;
    }
    return count;
}

uint64_t
scorep_profile_get_number_of_child_calls( scorep_profile_node* node )
{
    uint64_t             count = 0;
    scorep_profile_node* child = NULL;

    if ( node == NULL )
    {
        return 0;
    }

    child = node->first_child;
    while ( child != NULL )
    {
        count += child->count;
        child  = child->next_sibling;
    }
    return count;
}

uint64_t
scorep_profile_get_exclusive_time( scorep_profile_node* node )
{
    uint64_t             exclusive_time = node->inclusive_time.sum;
    scorep_profile_node* child          = NULL;

    if ( node == NULL )
    {
        return 0;
    }

    child = node->first_child;
    while ( child != NULL )
    {
        exclusive_time -= child->inclusive_time.sum;
        child           = child->next_sibling;
    }
    return exclusive_time;
}

void
scorep_profile_merge_node_dense( scorep_profile_node* destination,
                                 scorep_profile_node* source )
{
    int i;

    /* Merge static values */
    destination->count += source->count;
    if ( destination->first_enter_time > source->first_enter_time )
    {
        destination->first_enter_time = source->first_enter_time;
    }
    if ( destination->last_exit_time < source->last_exit_time )
    {
        destination->last_exit_time = source->last_exit_time;
    }

    /* Merge dense metrics */
    scorep_profile_merge_dense_metric( &destination->inclusive_time, &source->inclusive_time );
    for ( i = 0; i < scorep_profile.num_of_dense_metrics; i++ )
    {
        scorep_profile_merge_dense_metric( &destination->dense_metrics[ i ],
                                           &source->dense_metrics[ i ] );
    }
}

void
scorep_profile_merge_node_sparse( scorep_profile_node* destination,
                                  scorep_profile_node* source )
{
    scorep_profile_sparse_metric_int*    dest_sparse_int      = NULL;
    scorep_profile_sparse_metric_int*    source_sparse_int    = source->first_int_sparse;
    scorep_profile_sparse_metric_double* dest_sparse_double   = NULL;
    scorep_profile_sparse_metric_double* source_sparse_double = source->first_double_sparse;

    /* Merge sparse integer metrics */
    while ( source_sparse_int != NULL )
    {
        dest_sparse_int = destination->first_int_sparse;
        while ( ( dest_sparse_int != NULL ) &&
                ( dest_sparse_int->metric != source_sparse_int->metric ) )
        {
            dest_sparse_int = dest_sparse_int->next_metric;
        }
        if ( dest_sparse_int == NULL )
        {
            dest_sparse_int = scorep_profile_copy_sparse_int( source_sparse_int );
        }
        else
        {
            scorep_profile_merge_sparse_metric_int( dest_sparse_int, source_sparse_int );
            dest_sparse_int->next_metric  = destination->first_int_sparse;
            destination->first_int_sparse = dest_sparse_int;
        }

        source_sparse_int = source_sparse_int->next_metric;
    }

    /* Merge sparse double metrics */
    while ( source_sparse_double != NULL )
    {
        dest_sparse_double = destination->first_double_sparse;
        while ( ( dest_sparse_double != NULL ) &&
                ( dest_sparse_double->metric != source_sparse_double->metric ) )
        {
            dest_sparse_double = dest_sparse_double->next_metric;
        }
        if ( dest_sparse_double == NULL )
        {
            dest_sparse_double               = scorep_profile_copy_sparse_double( source_sparse_double );
            dest_sparse_double->next_metric  = destination->first_double_sparse;
            destination->first_double_sparse = dest_sparse_double;
        }
        else
        {
            scorep_profile_merge_sparse_metric_double( dest_sparse_double, source_sparse_double );
        }

        source_sparse_double = source_sparse_double->next_metric;
    }
}

void
scorep_profile_merge_subtree( scorep_profile_node* destination,
                              scorep_profile_node* source )
{
    assert( destination );
    assert( source );

    scorep_profile_merge_node_dense( destination, source );
    scorep_profile_merge_node_sparse( destination, source );

    scorep_profile_node* child = source->first_child;
    scorep_profile_node* next  = NULL;
    scorep_profile_node* match = NULL;
    while ( child != NULL )
    {
        next  = child->next_sibling;
        match = scorep_profile_find_child( destination, child );

        /* If no equal child node of destination exists, insert the child of source as
           first child of destination. */
        if ( match == NULL )
        {
            child->next_sibling      = destination->first_child;
            destination->first_child = child;
        }
        /* If a matching node exists, merge the subtree recursively */
        else
        {
            scorep_profile_merge_subtree( match, child );
        }

        child = next;
    }
}
