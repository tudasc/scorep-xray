/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
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

#ifndef SCOREP_PROFILE_NODE_H
#define SCOREP_PROFILE_NODE_H

/**
 * @file
 *
 * @brief Node handling for the profile tree
 *
 */

#include <stdbool.h>
#include <SCOREP_Definitions.h>
#include <scorep_profile_metric.h>

/* ***************************************************************************************
   Type definitions
*****************************************************************************************/

/**
   Type for the type dependent data. Each node has special type and a field where it might
   store type dependent data. Currently, 8 bytes are reserved for type dependent data,
   which should be enough to store a pointer, an handle, an integer value, or an double
   value.
 */
typedef struct
{
    uint64_t handle;
    uint64_t value;
} scorep_profile_type_data_t;

/**
   Type to store the parameter list in callpath representation after resolving
   the parameter sub-trees.
 */
typedef struct
{
    uint32_t                              number;
    scorep_definitions_callpath_parameter parameters[];
} scorep_profile_callpath_parameters_t;

/**
   List of profile node types.  Each node has special type and a field where it might
   store type dependent data. In order to add new node types, add a new entry here, and
   the copy and compare functions to scorep_profile_type_data_funcs in scorep_profile_node.c.
 */
typedef enum
{
    SCOREP_PROFILE_NODE_REGULAR_REGION,
    SCOREP_PROFILE_NODE_PARAMETER_STRING,
    SCOREP_PROFILE_NODE_PARAMETER_INTEGER,
    SCOREP_PROFILE_NODE_THREAD_ROOT,
    SCOREP_PROFILE_NODE_THREAD_START,
    SCOREP_PROFILE_NODE_COLLAPSE,
    SCOREP_PROFILE_NODE_TASK_ROOT
} scorep_profile_node_type;

/**
   Speciefies whether the current call happens in the context of an untied or tied task.
 */
typedef enum
{
    SCOREP_PROFILE_TASK_CONTEXT_UNTIED,
    SCOREP_PROFILE_TASK_CONTEXT_TIED
} scorep_profile_task_context;

/**
   Contains all data for one profile node. Each instance represents a region, or a
   parameter, or other callpath relevant object. The children of a node represent regions,
   parameters called by this region. A callpath is defined by the path from the
   root to a node. Thus, each node also identifies an unique callpath and stored the
   statistics recorded for this callpath.
   The children of a node are stored as single linked list, where @a first_child points to
   the first child of a node and @a next_sibling points to the next sibling, which is the
   next child of the parent.
   Nodes can be of different type, which receive different treatment. The type of the node
   is stored in @a node_type, depending on the type it is possible to store different data
   with this node in @a type_specific_data.

   Here is a list which data this field contains:
   <dl>
    <dt>SCOREP_PROFILE_NODE_REGULAR_REGION</dt>
    <dd>The region handle and possible parameters as value (as pointer to
        scorep_profile_callpath_parameters_t)</dd>
    <dt>SCOREP_PROFILE_NODE_PARAMETER_STRING</dt>
    <dd>The parameter handle and a handle for the string value</dd>
    <dt>SCOREP_PROFILE_NODE_PARAMETER_INTEGER</dt>
    <dd>The parameter handle and an integer value</dd>
    <dt>SCOREP_PROFILE_NODE_THREAD_ROOT</dt>
    <dd>A pointer to a @ref scorep_profile_root_node_data instance and the thread id</dd>
    <dt>SCOREP_PROFILE_NODE_THREAD_START</dt>
    <dd>A pointer to the @ref scorep_profile_node instance from which the new thread
        was created</dd>
    <dt>SCOREP_PROFILE_NODE_COLLAPSE</dt>
    <dd>The depth level of the node</dd>
    <dt>SCOREP_PROFILE_NODE_TASK_ROOT</dt>
    <dd>The region handle</dd>
   </dl>

   The  field @a flag is an bitstring to set various flags. Possible flags are defined
   in @a scorep_profile_node_flag.
 */
typedef struct scorep_profile_node_struct
{
    SCOREP_CallpathHandle                callpath_handle;
    struct scorep_profile_node_struct*   parent;
    struct scorep_profile_node_struct*   first_child;
    struct scorep_profile_node_struct*   next_sibling;
    scorep_profile_dense_metric*         dense_metrics;
    scorep_profile_sparse_metric_double* first_double_sparse;
    scorep_profile_sparse_metric_int*    first_int_sparse;
    scorep_profile_dense_metric          inclusive_time;
    uint64_t                             count;            // For dense metrics
    uint64_t                             hits;             // For samples
    uint64_t                             first_enter_time; // Required by Scalasca
    uint64_t                             last_exit_time;   // Required by Scalasca
    scorep_profile_node_type             node_type;
    scorep_profile_type_data_t           type_specific_data;
    uint8_t                              flags;
} scorep_profile_node;

/**
   A type which specifies possible flags for the scorep_profile_node::flags field.
 */
typedef enum
{
    SCOREP_PROFILE_FLAG_MPI_IN_SUBTREE = 1, /**< Set if the subtree contains MPI calls */
    SCOREP_PROFILE_FLAG_IS_FORK_NODE   = 2, /**< Set if another thread was forked here */
    SCOREP_PROFILE_FLAG_IN_UNTIED_TASK = 4  /**< Set if in untied task */
} scorep_profile_node_flag;

/**
   Type of function pointer that must be passed to @ref scorep_profile_for_all().
   to be processed for each node.
   @param node  Pointer to the node in the profile which is currently processed.
   @param param Pointer to a parameter that can be given to
                @ref scorep_profile_for_all()
 */
typedef void ( scorep_profile_process_func_t )( scorep_profile_node* node,
                                                void*                param );

/**
   Type of the comparison function pointer that must be passed to
   @red scorep_profile_sort_subtree(). It should compare two profile nodes.
   @param node_a  Pointer to the first profile node to be compared.
   @param node_b  Pointer to the second profile node.
   @return The function should return true if @a node_a is greater than @a node_b.
           Else it returns false.
 */
typedef bool ( scorep_profile_compare_node_t )( scorep_profile_node* node_a,
                                                scorep_profile_node* node_b );

/* ***************************************************************************************
   Node creation/destruction
*****************************************************************************************/
/**
   Constructs a node of a given type.
   @param location Pointer to the location data associated with the subtrees memory
                   page.
   @param parent   Pointer to the parent node. It is NULL is it is a root node. This
                   function does not insert the child into the parents child list, but sets
                   the parent pointer of the new node only.
   @param type     The type of the node.
   @param data     The type dependent data for this node.
   @param timestamp The timestamp of its first enter event.
   @param context  Specifies whether this call happens in the context of a tied or untied task.
   @return A pointer to the newly created node.
 */
scorep_profile_node*
scorep_profile_create_node( SCOREP_Profile_LocationData* location,
                            scorep_profile_node*         parent,
                            scorep_profile_node_type     type,
                            scorep_profile_type_data_t   data,
                            uint64_t                     timestamp,
                            scorep_profile_task_context  context );

/**
   Creates a new node and copies the statistics from @a source to it. The new node is
   not added to a calltree, nor are children copied.
   @param location Pointer to the location data associated with the subtrees memory
                   page.
   @param source Pointer to the node which gets copied.
   @return A pointer to the new node.
 */
scorep_profile_node*
scorep_profile_copy_node( SCOREP_Profile_LocationData* location,
                          scorep_profile_node*         source );

/**
   Releases the subtree, rooted in @a root.
   @param location Pointer to the location data associated with the subtrees memory
                   page.
   @param root     Pointer to the root node of a subtree, whose memory can be reused.
 */
void
scorep_profile_release_subtree( SCOREP_Profile_LocationData* location,
                                scorep_profile_node*         root );

/**
   Allocates or recycles memory for a new profile node.
   @param location Pointer to the location data associated with the subtrees memory
                   page.
   @param type     Node type of the newly allocated node. The node type is not set, but
                   used to determine whether the memory must be taken from miscellaneous
                   of from the profile pool. Because some node must not be released on
                   reconfiguration.
   @param context  Specifies whether this call happens in the context of e tied or untied task.
 */
scorep_profile_node*
scorep_profile_alloc_node( SCOREP_Profile_LocationData* location,
                           scorep_profile_node_type     type,
                           scorep_profile_task_context  context );

/**
   Find a child node of @a parent of a specified type. If parent has a child
   which matches @a type this node is returned.
   @param parent    Pointer to a node which children are searched.
   @param type      Pointer to a node for which a matching node is searched.
   @returns The matching node from the children of @a parent. If no matching node is
            found, it returns NULL.
 */

/**
   Find or create a child node of @a parent of a specified type. If parent has a child
   which matches @a type this node is returned. Else it creates a node of @a node_type
   and the type specific data and adds it to the children of @a parent.
   @param location  Pointer to the location data.
   @param parent    Pointer to a node which children are searched.
   @param nodeType  The node type of the searched or created node.
   @param specific  The type specific data of the searched or created node.
   @param timestamp Timestamp for the first enter time in case a new node is created.
   @returns The matching node from the children of @a parent. This might be a newly
            created node.
 */
scorep_profile_node*
scorep_profile_find_create_child( SCOREP_Profile_LocationData* location,
                                  scorep_profile_node*         parent,
                                  scorep_profile_node_type     nodeType,
                                  scorep_profile_type_data_t   specific,
                                  uint64_t                     timestamp );


/* ***************************************************************************************
   Tree manipulation and traversal functions
*****************************************************************************************/
/**
   Adds @a child to the children of @a parent. No check whether a matching node
   already exists is performed. If @a child is the root node to a sub-tree, the whole
   sub-tree is added. If child is still child of another node, the tree ill become
   inconsistent.
   @param parent Pointer to the node to which the @a child is added.
   @param child  Pointer to the node which is added to @a parent.
 */
void
scorep_profile_add_child( scorep_profile_node* parent,
                          scorep_profile_node* child );

/**
   Find a child node of @a parent of a specified type. If parent has a child
   which matches @a type this node is returned.
   @param parent    Pointer to a node which children are searched.
   @param type      Pointer to a node for which a matching node is searched.
   @returns The matching node from the children of @a parent. If no matching node is
            found, it returns NULL.
 */
scorep_profile_node*
scorep_profile_find_child( scorep_profile_node* parent,
                           scorep_profile_node* type );

/**
   Checks whether two nodes represent the same object (region, parameter, thread, ...).
   It does only compare node type and type dependent data. The statistics are not
   compared.
   @param node1 The node that is compared to @a node2
   @param node2 The node that is compared to @a node1
   @return true, if @a node1 and @a node2 are of the same type, and their type dependent
          data match. Else false is returned.
 */
bool
scorep_profile_compare_nodes( scorep_profile_node* node1,
                              scorep_profile_node* node2 );

/**
   Provides a hash value for a single node (without its subtree).
   @param node The node to be hashed.
   @return a 64-bit hash value.
 */
uint64_t
scorep_profile_node_hash( scorep_profile_node* node );

/**
   Provides an ordering for two nodes based on their type-specific data.
   @param node1 The node that is compared to @a node2
   @param node2 The node that is compared to @a node1
   @return true, iff @a node1 is less than @a node2. If their types are
   different, an arbitrary (but well-defined) ordering is applied based on
   their types.
 */
bool
scorep_profile_node_less_than( scorep_profile_node* a,
                               scorep_profile_node* b );

/**
   Removes the children from @a source and appends them to the children list of
   @a destination.
   @param destination Pointer to a node to which the children are added. If this
                      is a NULL pointer, the children are added as root nodes
                      to the profile forest.
   @param source      Pointer to a node from which the children are removed. This
                      must not be NULL.
 */
void
scorep_profile_move_children(  scorep_profile_node* destination,
                               scorep_profile_node* source );

/**
   Removes the node from the profile structure. The whole subtree rooted in
   @a node is removed.
   @param node A pointer to a node which is to be removed from the profile. It
               must not be NULL.
 */
void
scorep_profile_remove_node( scorep_profile_node* node );

/**
   Traverse the (sub)tree rooted in @a root_node via depth search and execute the
   function @a func for each node.
   @param rootNode  Pointer to the root node of the subtree which is processed. Its
                    siblings are nor processed.
   @param func      Function pointer to the function which is called for each node in
                    the (sub)tree.
   @param param     Pointer to a parameter that is passed to @a func.
 */
void
scorep_profile_for_all( scorep_profile_node*           rootNode,
                        scorep_profile_process_func_t* func,
                        void*                          param );

/**
   Merges the subtree rooted in @a source into the subtree rooted in @a destination.
   Adds all statistics of @a source to @a destination and processes the children
   recursively.
   @param location    A pointer to the location data structure which is associated to
                      the memory page which is used to contribute released memory or
                      create new records.
   @param destination A pointer to the node which is the root of the target subtree.
   @param source      A pointer to the node of the subtree which is merged into
                      @a destination.
 */
void
scorep_profile_merge_subtree( SCOREP_Profile_LocationData* location,
                              scorep_profile_node*         destination,
                              scorep_profile_node*         source );

/**
   Sorts the children of all nodes in the subtree according to the given comparison
   function @a comparison_func.
   @param root            Pointer to the root of the subtree to be sorted.
   @param comparisonFunc  Pointer to the comparison function.
 */
void
scorep_profile_sort_subtree( scorep_profile_node*           root,
                             scorep_profile_compare_node_t* comparisionFunc );

/**
   Replaces all occurrences of @a old in thread start nodes by @a substitute.
   @param old        A pointer to a fork node, that is replaced in the type specific data
                     of all thread start nodes.
   @param substitute A pointer to a fork node, that will replace the occurrences of
                     @a old.
 */
void
scorep_profile_substitute_thread_starts( scorep_profile_node* old,
                                         scorep_profile_node* substitute );

/* ***************************************************************************************
   Data accesss and evaluation
*****************************************************************************************/

/**
   Copies all dense metrics and time statistics from @a source to @a destination.
   @param destination Pointer to a node to which the metric values are written.
   @param source      Pointer to a node from which the metric values are read.
 */
void
scorep_profile_copy_all_dense_metrics( scorep_profile_node* destination,
                                       scorep_profile_node* source );


/**
   Returns the location data of the location of @a node.
   @param node  Pointer to the node for which the location data is returned.
 */
SCOREP_Profile_LocationData*
scorep_profile_get_location_of_node( scorep_profile_node* node );

/**
   Returns the number of children of @a node.
   @param node Pointer to a node, for which the number of child nodes is returned.
   @return the number of child nodes for @a node. If a NULL pointer is given, 0 is
           returned.
 */
uint64_t
scorep_profile_get_number_of_children( scorep_profile_node* node );

/**
   Returns the number of calls to a child node of @a node.
   @param node Pointer to a node, for which the number of child calls is returned.
   @return the number of child calls for @a node. If a NULL pointer is given, 0 is
           returned.
 */
uint64_t
scorep_profile_get_number_of_child_calls( scorep_profile_node* node );

/**
   Returns the exclusive runtime of the callpath represented by @a node.
   @param node Pointer to a node, for which the exclusive runtime is returned.
   @return the exclusive runtime for @a node. If a NULL pointer is given, 0 is
           returned.
 */
uint64_t
scorep_profile_get_exclusive_time( scorep_profile_node* node );

/**
   Add inclusive dense metrics from source to destination. Does not add the visit count,
   because it is an exclusive metric.
   @param destination A pointer to a node to which the metrics are added.
   @param source      A pointer to a node which metrics are added to @a destination.
 */
void
scorep_profile_merge_node_inclusive( scorep_profile_node* destination,
                                     scorep_profile_node* source );

/**
   Add dense metrics from source to destination. In difference to
   scorep_profile_merge_node_inclusive, it also adds the visit count from source
   to destination.
   @param destination A pointer to a node to which the metrics are added.
   @param source      A pointer to a node which metrics are added to @a destination.
 */
void
scorep_profile_merge_node_dense( scorep_profile_node* destination,
                                 scorep_profile_node* source );

/**
   Add sparse metric from source to destination.
   @param location    A pointer to the location data structure which is associated to
                      the memory page which is used to contribute released memory or
                      create new records.
   @param destination A pointer to a node to which the metrics are added.
   @param source      A pointer to a node which metrics are added to @a destination.
 */
void
scorep_profile_merge_node_sparse( SCOREP_Profile_LocationData* location,
                                  scorep_profile_node*         destination,
                                  scorep_profile_node*         source );

/**
   Returns whether the SCOREP_PROFILE_FLAG_MPI_IN_SUBTREE flag is set for @a node.
   @param node         Pointer to the node which flag is requested.
 */
bool
scorep_profile_is_mpi_in_subtree( scorep_profile_node* node );

/**
   Sets the value of the SCOREP_PROFILE_FLAG_MPI_IN_SUBTREE flag in @a node.
   @param node         Pointer to the node which flag is set.
   @param mpiInSubtree Specifies the value of the
                       SCOREP_PROFILE_FLAG_MPI_IN_SUBTREE flag.
 */
void
scorep_profile_set_mpi_in_subtree( scorep_profile_node* node,
                                   bool                 mpiInSubtree );

/**
   Returns whether the SCOREP_PROFILE_FLAG_IS_FORK_NODE flag is set for @a node.
   @param node         Pointer to the node which flag is requested.
 */
bool
scorep_profile_is_fork_node( scorep_profile_node* node );

/**
   Sets the value of the SCOREP_PROFILE_FLAG_IS_FORK_NODE flag in @a node.
   @param node         Pointer to the node which flag is set.
   @param isForkNode   Specifies the value of the SCOREP_PROFILE_FLAG_IS_FORK_NODE flag.
 */
void
scorep_profile_set_fork_node( scorep_profile_node* node,
                              bool                 isForkNode );

/**
   Returns the context of the node.
   @param node         Pointer to the node which flag is requested.
   @returns whether the SCOREP_PROFILE_FLAG_IN_UNTIED_TASK flag is set for @a node.
            If it is set, it returns scorep_profile_task_context_untied. Else it
            returns scorep_profile_task_context_tied.
 */
scorep_profile_task_context
scorep_profile_get_task_context( scorep_profile_node* node );

/**
   Sets the value of the SCOREP_PROFILE_FLAG_IN_UNTIED_TASK flag in @a node.
   @param node     Pointer to the node which flag is set.
   @param context  Specifies whether this call happens in the context of e
                   tied or untied task.
 */
void
scorep_profile_set_task_context( scorep_profile_node*        node,
                                 scorep_profile_task_context context );

/**
   Subtracts the subtrahend's sum value and the sum2 value of all dense metrics
   from the minuend.
   @param minuend    The node from which the values are subtracted.
   @param subtrahend The node that is subtracted from minuend.
 */
void
scorep_profile_subtract_node( scorep_profile_node* minuend,
                              scorep_profile_node* subtrahend );

/* ***************************************************************************************
   Getter / Setter functions for type dependent data
*****************************************************************************************/

/**
   Stores the region handle in the type specific data of a regular region node.
 */
void
scorep_profile_type_set_region_handle( scorep_profile_type_data_t* data,
                                       SCOREP_RegionHandle         handle );

/**
   Retrieves the region handle from the type specific data of a regular region node.
 */
SCOREP_RegionHandle
scorep_profile_type_get_region_handle( scorep_profile_type_data_t data );

/**
   Stores the parameter handle in the type specific data of a parameter node.
 */
void
scorep_profile_type_set_parameter_handle( scorep_profile_type_data_t* data,
                                          SCOREP_ParameterHandle      handle );

/**
   Retrieves the parameter handle from the type specific data of a parameter node.
 */
SCOREP_ParameterHandle
scorep_profile_type_get_parameter_handle( scorep_profile_type_data_t data );

/**
   Stores the string handle in the type specific data of a parameter string node.
 */
void
scorep_profile_type_set_string_handle( scorep_profile_type_data_t* data,
                                       SCOREP_StringHandle         handle );

/**
   Retrieves the string handle from the type specific data of a parameter string node.
 */
SCOREP_StringHandle
scorep_profile_type_get_string_handle( scorep_profile_type_data_t data );

/**
   Stores the fork node where the thread was created in the type specific data of
   a thread start node.
 */
void
scorep_profile_type_set_fork_node( scorep_profile_type_data_t* data,
                                   scorep_profile_node*        node );

/**
   Retrieves the fork node where the thread was created from the type specific data of
   a thread start node.
 */
scorep_profile_node*
scorep_profile_type_get_fork_node( scorep_profile_type_data_t data );

/**
   Stores the profile location data in the type specific data of a thread root node.
 */
void
scorep_profile_type_set_location_data( scorep_profile_type_data_t*  data,
                                       SCOREP_Profile_LocationData* location );

/**
   Retrieves the profile location data from the type specific data of a thread root node.
 */
SCOREP_Profile_LocationData*
scorep_profile_type_get_location_data( scorep_profile_type_data_t data );

/**
   Retrieves the depth from the type specific data of a collapse node.
 */
uint64_t
scorep_profile_type_get_depth( scorep_profile_type_data_t data );

/**
   Stores the depth in the type specific data of a collapse node.
 */
void
scorep_profile_type_set_depth( scorep_profile_type_data_t* data,
                               uint64_t                    depth );

/**
   Retrieves the integer value from the type specific data of a parameter integer  node.
 */
uint64_t
scorep_profile_type_get_int_value( scorep_profile_type_data_t data );

/**
   Stores the integer value in the type specific data of a parameter integer node.
 */
void
scorep_profile_type_set_int_value( scorep_profile_type_data_t* data,
                                   uint64_t                    value );

/**
   Retrieves the pointer value from the type specific data.
 */
void*
scorep_profile_type_get_ptr_value( scorep_profile_type_data_t data );

/**
   Stores the pointer value in the type specific data.
 */
void
scorep_profile_type_set_ptr_value( scorep_profile_type_data_t* data,
                                   void*                       value );

/**
   Provides a hash value for type-dependent data.
   @param data The data to provide a hash value for.
   @param type Specifies the node type in which @a data belongs.
   @return a 64-bit hash value.
 */
uint64_t
scorep_profile_hash_for_type_data( scorep_profile_type_data_t data,
                                   scorep_profile_node_type   type );

/**
   Provides an ordering for type-dependent data. Both objects are assumed to
   be of the same type @a type.
   @param data1 The data which is compared to @a data2.
   @param data2 The data which is compared to @a data1.
   @param type Specifies the node type in which @a data1 and @a data2 belong.
   @return true, if @a data1 is less than @a data2.
 */
bool
scorep_profile_less_than_for_type_data( scorep_profile_type_data_t data1,
                                        scorep_profile_type_data_t data2,
                                        scorep_profile_node_type   type );

/**
   Compares the type dependent data. Both objects are assumed to be of the same type
   @a type.
   @param data1 The data which is compared to @a data2.
   @param data2 The data which is compared to @a data1.
   @param type  Specifies the node type to which @a data1 and @a data2 belong.
   @return true, if @a data1 equals @a data2.
 */
bool
scorep_profile_compare_type_data( scorep_profile_type_data_t data1,
                                  scorep_profile_type_data_t data2,
                                  scorep_profile_node_type   type );

/**
   Creates a copy of type dependent data.
   @param data The data which is copied.
   @param type  Specifies the node type of @a data.
 */
void
scorep_profile_copy_type_data( scorep_profile_type_data_t* destination,
                               scorep_profile_type_data_t  source,
                               scorep_profile_node_type    type );


#endif /* SCOREP_PROFILE_NODE_H */
