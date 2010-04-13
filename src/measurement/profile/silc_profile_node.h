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

#ifndef SILC_PROFILE_NODE_H
#define SILC_PROFILE_NODE_H

/**
 * @file        silc_profile_node.h
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Node handlinf for the profile tree
 *
 */

#include <stdbool.h>

#include "silc_profile_metric.h"

//typedef void* SILC_CallpathHandle;
#include "SILC_Profile.h"
#define SILC_INVALID_CALLPATH NULL

/* ***************************************************************************************
   Type definitions
*****************************************************************************************/

/**
   Type for the type dependent data. Each node has special type and a field where it might
   store type dependent data. Currently, 8 bytes are reserved for type dependent data,
   which should be enough to store a pointer, an handle, an integer value, or an double
   value.
 */
typedef uint64_t silc_profile_type_data_t;

/**
   List of profile node types.  Each node has special type and a field where it might
   store type dependent data. In order to add new node types, add a new entry here, and
   the copy and compare functions to silc_profile_type_data_funcs in silc_profile_node.c.
 */
typedef enum
{
    silc_profile_node_regular_region,
    silc_profile_node_parameter_string,
    silc_profile_node_parameter_integer,
    silc_profile_node_thread_fork,
    silc_profile_node_thread_root,
    silc_profile_node_thread_start,
    silc_profile_node_collapse
} silc_profile_node_type;

/**
   Contains all data for one profile node. Each instance represents a region, or a
   parameter, or other callpath relevant object. The children of a node represent regions,
   paramaters called by this region. A callpath is defined by the path from the
   too to a node. Thus, each node also identies an unique callpath and stored the
   statistics recorded for this callpath.
   The children of a node are stored as single linked list, where @a first_child points to
   the first child of a node and @a next_sibling points to the next sibling, which is the
   next child of the parent.
   Nodes can be of different type, which receive different treatment. The type of the node
   is stored in @a node_type, depending on the type it is possible to store differnt data
   with this node in @a type_specific_data.
 */
typedef struct silc_profile_node_struct
{
    SILC_CallpathHandle                callpath_handle;
    struct silc_profile_node_struct*   parent;
    struct silc_profile_node_struct*   first_child;
    struct silc_profile_node_struct*   next_sibling;
    silc_profile_dense_metric*         dense_metrics;
    silc_profile_sparse_metric_double* first_double_sparse;
    silc_profile_sparse_metric_int*    first_int_sparse;
    silc_profile_dense_metric          implicit_time;
    uint64_t                           count;              // For dense metrics
    uint64_t                           first_enter_time;   // Required by Scalasca
    uint64_t                           last_exit_time;     // Required by Scalasca
    silc_profile_node_type             node_type;
    silc_profile_type_data_t           type_specific_data;
} silc_profile_node;

/**
   Contains the type specific data for a node of type
   @a silc_profile_node_parameter_integer
 */
typedef struct
{
    SILC_ParameterHandle handle;
    int64_t              value;
} silc_profile_integer_node_data;

/**
   Contains the type specific data for a node of type
   @a silc_profile_node_parameter_string
 */
typedef struct
{
    SILC_ParameterHandle handle;
    SILC_StringHandle    value;
} silc_profile_string_node_data;

/* ***************************************************************************************
   Functions
*****************************************************************************************/

/**
   Constructs a node of a given type.
   @param parent Pointer to the parent node. It is NULL is it is a root node. This
                function does not insert the child into the parents child list, but sets
                the parent pointer ot the new node only.
   @param type   The type of the node.
   @param data   The type dependent data for this node.
   @param timestamp The timestamp of its first enter event.
   @return A pointer to the newly created node.
 */
extern silc_profile_node*
silc_profile_create_node( silc_profile_node*       parent,
                          silc_profile_node_type   type,
                          silc_profile_type_data_t data,
                          uint64_t                 timestamp );

/**
   Creates a new node and copies the statistics from @a source to it. The new node is
   not added to a calltree, nor are children copied.
   @param source Pointer to the node which gets copied.
   @return A pointer to the new node.
 */
extern silc_profile_node*
silc_profile_copy_node( silc_profile_node* source );

/**
   Checks wether two nodes represent the same object (region, parameter, thread, ...).
   It does only compare node type and type dependent data. The statistics are not
   compared.
   @param node1 The node that is compared to @a node2
   @param node2 The node that is compared to @a node1
   @return true, if @a node1 and @a node2 are of the same type, and their type dependent
          data match. Else false is returned.
 */
extern bool
silc_profile_compare_nodes( silc_profile_node* node1,
                            silc_profile_node* node2 );

/**
   Compares the type dependent data. Both objects are assumed to be of the same type
   @a type.
   @param data1 The data whch is compared to @a data2.
   @param data2 The data whch is compared to @a data1.
   @param type  Specifies the node type to which @a data1 and @a data2 belong.
   @return true, if @a data1 equals @a data2.
 */
extern bool
silc_profile_compare_type_data( silc_profile_type_data_t data1,
                                silc_profile_type_data_t data2,
                                silc_profile_node_type   type );

/**
   Creates a copy of type dependent data.
   @param data The data which is copied.
   @param type  Specifies the node type of @a data.
 */
extern silc_profile_type_data_t
silc_profile_copy_type_data( silc_profile_type_data_t data,
                             silc_profile_node_type   type );

#endif // SILC_PROFILE_NODE_H
