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
 * @brief Node handling for the profile tree
 *
 */

#include <stdbool.h>
#include <config.h>

#include "SILC_DefinitionHandles.h"

#include "silc_profile_metric.h"

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
    silc_profile_node_thread_root,
    silc_profile_node_thread_start,
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

   Here is a list which data this field contains:
   <dl>
    <dt>silc_profile_node_regular_region</dt>
    <dd>The region handle</dd>
    <dt>silc_profile_node_parameter_string</dt>
    <dd>A pointer to a @ref silc_profile_string_node_data instance</dd>
    <dt>silc_profile_node_parameter_integer</dt>
    <dd>A pointer to a @ref silc_profile_integer_node_data instance</dd>
    <dt>silc_profile_node_thread_root</dt>
    <dd>A location definition handle</dd>
    <dt>silc_profile_node_thread_start</dt>
    <dd>A pointer to the @ref silc_profile_node instance from which the new thread
        was created</dd>
   </dl>
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

/**
   Contains the type specific data for a node of type
   @a silc_profile_node_thread_root
 */
typedef struct
{
    struct SILC_Profile_LocationData* thread_data;
    uint64_t                          thread_id;
} silc_profile_root_node_data;

/**
   Type of function pointer that must be passed to @ref silc_profile_for_all().
   to be processed for each node.
   @param node  Pointer to the node in the profile which is currently processed.
   @param param Pointer to a parameter that can be given to
                @ref silc_profile_for_all()
 */
typedef void ( silc_profile_process_func_t )( silc_profile_node* node, void* param );

/* ***************************************************************************************
   Defines
*****************************************************************************************/

/**
   @def SILC_PROFILE_POINTER2DATA( handle )
   Casts a pointer to node specific data type.
   @param handle The pointer which is casted to node specific data.
 */
#if SIZEOF_VOID_P == 8
#define SILC_PROFILE_POINTER2DATA( ptr ) ( ( uint64_t )ptr )
#elif SIZEOF_VOID_P == 4
#define SILC_PROFILE_POINTER2DATA( ptr ) ( ( uint32_t )ptr )
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif

/**
   @def SILC_PROFILE_DATA2POINTER( data )
   Casts a node specific data item to a region handle.
   @param data The node specific data that is casted to a pointer.
 */
#if SIZEOF_VOID_P == 8
#define SILC_PROFILE_DATA2POINTER( data )   ( ( void* )( uint64_t )data )
#elif SIZEOF_VOID_P == 4
#define SILC_PROFILE_DATA2POINTER( data )   ( ( void* )( uint32_t )data )
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif

/**
   @def SILC_PROFILE_REGION2DATA( handle )
   Casts a region handle to node specific data type.
   @param handle The region handle which is casted to node specific data.
 */
#define SILC_PROFILE_REGION2DATA( handle ) SILC_PROFILE_POINTER2DATA( handle )

/**
   @def SILC_PROFILE_DATA2REGION( data )
   Casts a node specific data item to a region handle.
   @param data The node specific data that is casted to a region handle.
 */
#define SILC_PROFILE_DATA2REGION( data ) ( ( SILC_RegionHandle ) \
                                           SILC_PROFILE_DATA2POINTER( data ) )

/**
   @def SILC_PROFILE_PARAMSTR2DATA( handle )
   Casts a pointer to a @ref silc_profile_string_node_data instance to node specific
   data type.
   @param handle The pointer to a @ref silc_profile_string_node_data instance which is
                 casted to node specific data.
 */
#define SILC_PROFILE_PARAMSTR2DATA( handle ) SILC_PROFILE_POINTER2DATA( handle )

/**
   @def SILC_PROFILE_DATA2PARAMSTR( data )
   Casts a node specific data item to a pointer to a @ref silc_profile_string_node_data
   instance.
   @param data The node specific data that is casted to a  pointer to a
               @ref silc_profile_string_node_data instance.
 */
#define SILC_PROFILE_DATA2PARAMSTR( data ) ( ( silc_profile_string_node_data* ) \
                                             SILC_PROFILE_DATA2POINTER( data ) )

/**
   @def SILC_PROFILE_PARAMINT2DATA( handle )
   Casts a pointer to a @ref silc_profile_integer_node_data instance to node specific data
   type.
   @param handle The pointer to a @ref silc_profile_integer_node_data instance which is
                 casted to node specific data.
 */
#define SILC_PROFILE_PARAMINT2DATA( handle ) SILC_PROFILE_POINTER2DATA( handle )

/**
   @def SILC_PROFILE_DATA2PARAMINT( data )
   Casts a node specific data item to a pointer to a @ref silc_profile_integer_node_data
   instance.
   @param data The node specific data that is casted to a  pointer to a
               @ref silc_profile_integer_node_data instance.
 */
#define SILC_PROFILE_DATA2PARAMINT( data ) ( ( silc_profile_integer_node_data* ) \
                                             SILC_PROFILE_DATA2POINTER( data ) )

/**
   @def SILC_PROFILE_THREADROOT2DATA( handle )
   Casts a pointer to a @ref silc_profile_root_node_data instance to node specific data
   type.
   @param handle The pointer to a @ref silc_profile_root_node_data instance which is
                 casted to node specific data.
 */
#define SILC_PROFILE_THREADROOT2DATA( handle ) SILC_PROFILE_POINTER2DATA( handle )

/**
   @def SILC_PROFILE_DATA2THREADROOT( data )
   Casts a node specific data item to a pointer to a @ref silc_profile_root_node_data
   instance.
   @param data The node specific data that is casted to a  pointer to a
               @ref silc_profile_root_node_data instance.
 */
#define SILC_PROFILE_DATA2THREADROOT( data ) ( ( silc_profile_root_node_data* ) \
                                               SILC_PROFILE_DATA2POINTER( data ) )



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
   Find or create a child node of @a parent of a specified type. If parent has a child
   which matches @a type this node is returned. Else a copy of @a type without metrics
   is created and added to the children of @a parent.
   @param parent    Pointer to a node which children are searched.
   @param type      Pointer to a node for which a matching node is searched.
   @param timestamp Timestamp for the first enter time in case a new node is created.
   @returns The mathcing node from the children of @a parent. this might be a newly
            created node.
 */
extern silc_profile_node*
silc_profile_find_create_child( silc_profile_node* parent,
                                silc_profile_node* type,
                                uint64_t           timestamp );

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

/**
   Copies all dense metrics and time statistics from @a source to @a destination.
   @param destination Pointer to a node to which the metric values are written.
   @param source      Pointer to a node from which the metric values are read.
 */
extern void
silc_profile_copy_all_dense_metrics( silc_profile_node* destination,
                                     silc_profile_node* source );

/**
   Removes the children from @a source and appens them to the children list of
   @a destination.
   @param destination Pointer to a node to which the children are added. If this
                      is a NULL pointer, the children are added as root nodes
                      to the profile forest.
   @param source      Pointer to a node from which the children are removed. This
                      must not be NULL.
 */
extern void
silc_profile_move_children(  silc_profile_node* destination,
                             silc_profile_node* source );

/**
   Removes the node from the profile sructure. The whole subtree rooted in
   @a node is removed.
   @param node A pointer to a node which is to be removed from the profile. It
               must not be NULL.
 */
extern void
silc_profile_remove_node( silc_profile_node* node );

/**
   Traverse the (sub)tree rooted in @a root_node via depth search and execute the
   function @a func for each node.
   @param root_node Pointer to the root node of the subtree which is processed. Its
                    siblings are nor processed.
   @param func      Function pointer to the function which is called for each node in
                    the (sub)tree.
   @param param     Pointer to a parameter that is passed to @a func.
 */
extern void
silc_profile_for_all( silc_profile_node*           root_node,
                      silc_profile_process_func_t* func,
                      void*                        param );

/**
   Returns the number of children of @a node.
   @param node Pointer to a node, for which the number of child nodes is returned.
   @return the number of child nodes for @a node. If a NULL pointer is given, 0 is
           returned.
 */
extern uint32_t
silc_profile_get_number_of_children( silc_profile_node* node );

/**
   Returns the number of calls to a child node of @a node.
   @param node Pointer to a node, for which the number of child calls is returned.
   @return the number of child calls for @a node. If a NULL pointer is given, 0 is
           returned.
 */
extern uint32_t
silc_profile_get_number_of_child_calls( silc_profile_node* node );

/**
   Returns the exclusive runtime of the callpath represented by @a node.
   @param node Pointer to a node, for which the exclusive runtime is returned.
   @return the exclusive runtime for @a node. If a NULL pointer is given, 0 is
           returned.
 */
extern uint64_t
silc_profile_get_exclusive_time( silc_profile_node* node );

#endif // SILC_PROFILE_NODE_H
