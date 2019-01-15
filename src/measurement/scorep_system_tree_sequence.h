/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2015-2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_SYSTEM_TREE_SEQUENCE_H
#define SCOREP_SYSTEM_TREE_SEQUENCE_H

/**
 * @file
 */

#include <SCOREP_Types.h>
#include <inttypes.h>
#include <stdbool.h>

typedef struct scorep_system_tree_seq      scorep_system_tree_seq;

typedef struct scorep_system_tree_seq_name scorep_system_tree_seq_name;


/**
 * Helper type. We want to integrate all system tree definition types in the same
 * sequence data structure. Thus, we need to distinguish the
 * original type.
 */
typedef enum
{
    SCOREP_SYSTEM_TREE_SEQ_TYPE_SYSTEM_TREE_NODE,
    SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION_GROUP,
    SCOREP_SYSTEM_TREE_SEQ_TYPE_LOCATION
} scorep_system_tree_seq_type;

/**
 * The type for the data that the traversal passes from the parent node to its children.
 */
typedef union
{
    uint64_t uint64;
    void*    ptr;
} scorep_system_tree_seq_child_param;

/**
 * function type that is called from scorep_system_tree_seq_traverse_all.
 * @param definition   Pointer to the according sequence definition.
 * @param copy         The index of the copy of this invocation for the definition.
 * @param param        A pointer with parameters that are passes to the
 *                     scorep_system_tree_seq_traverse_all call.
 * @param for_children Data that it passed from a node to its child nodes.
 * @returns Data that is passed to the children of this node in the for_children
 *          parameter.
 */
typedef scorep_system_tree_seq_child_param
(* scorep_system_tree_seq_func)( scorep_system_tree_seq*            definition,
                                 uint64_t                           copy,
                                 void*                              param,
                                 scorep_system_tree_seq_child_param for_children );

/**
 * After Unification, it returns a pointer to the system tree to
 * sequence element. Before unification it returns NULL.
 */
scorep_system_tree_seq*
scorep_system_tree_seq_get_root( void );

/**
 * Unifies the system tree
 */
void
scorep_system_tree_seq_unify( void );

/**
 * Releases data structures that hold the unified system tree.
 */
void
scorep_system_tree_seq_free( void );

/**
 * Returns an communicator which is a permutation of the world communicator
 * where the ranks are sorted after their index of a depth first traversal of
 * the scalable system tree definitions.
 */
struct SCOREP_Ipc_Group*
scorep_system_tree_seq_get_ipc_group( void );

/**
 * Cleans up a depth order communicator.
 */
void
scorep_system_tree_seq_free_ipc_group( struct SCOREP_Ipc_Group* );

/**
 * Returns the mapping of the locations to their index in the
 * new definitions. The i-th value in the returned array is
 * the index of the i-th location in the original definitions.
 * Maps only locations of the current process.
 */
const uint64_t*
scorep_system_tree_seq_get_local_location_order( void );

/**
 * Returns the mapping of the locations to their index in the
 * new definitions. The i-th value in the returned array is
 * the index of the i-th location in the original definitions.
 * Maps locations of all processes. Only valid on rank 0.
 * All other ranks return NULL.
 */
const uint64_t*
scorep_system_tree_seq_get_global_location_order( void );

/**
 * Returns the mappings of ranks to their original index.
 * The i-th value in the returned array is rank number
 * in the original world communicator of this process.
 * Only valid on rank 0. All other ranks return NULL.
 */
const uint32_t*
scorep_system_tree_seq_get_rank_order( void );

/**
 * Returns the list of global event numbers.
 */
const uint64_t*
scorep_system_tree_seq_get_event_numbers( void );

/**
 * Returns whether current MPP implementation supports scalable
 * system tree definitions.
 */
bool
scorep_system_tree_seq_has_support_for( void );

/**
 * Returns the number of copies for a given system tree sequence definition.
 */
uint64_t
scorep_system_tree_seq_get_number_of_copies( scorep_system_tree_seq* definition );

/**
 * Returns the number of children for a given system tree sequence definition.
 */
uint64_t
scorep_system_tree_seq_get_number_of_children( scorep_system_tree_seq* definition );

/**
 * Returns the child definition with a given index for a system
 * tree sequence definition.
 */
scorep_system_tree_seq*
scorep_system_tree_seq_get_child( scorep_system_tree_seq* definition,
                                  uint64_t                index );

/**
 * Returns the type for a given system tree sequence definition.
 */
scorep_system_tree_seq_type
scorep_system_tree_seq_get_type( scorep_system_tree_seq* definition );

/**
 * Returns the sub-type for a given system tree sequence definition.
 */
uint64_t
scorep_system_tree_seq_get_sub_type( scorep_system_tree_seq* definition );

/**
 * Returns the system tree node domains information for a sequence definition.
 * Location groups and locations have no system tree domains.
 */
SCOREP_SystemTreeDomain
scorep_system_tree_seq_get_domains( scorep_system_tree_seq* definition );

/**
 * Traverses the system tree from the sequence definitions in depth-first
 * order and calls @a func for every node in the system tree.
 * @param root      The root sequence definition of the system tree.
 * @param func      The function that is called for every node in the system tree.
 * @param param     A pointer that is passed to every invocation of @a func.
 * @param forRoot   Data that is passed to the root nodes, e.g., the copies
 *                  from @a root only.
 */
void
scorep_system_tree_seq_traverse_all( scorep_system_tree_seq*            root,
                                     scorep_system_tree_seq_func        func,
                                     void*                              param,
                                     scorep_system_tree_seq_child_param forRoot );

/**
 * Creates a data structure that is needed to construct node names
 * from sequence definitions during a traversal of the system tree in
 * depth-first order. The returned data structure need to be released
 * with scorep_system_tree_seq_free_name_data().
 */
scorep_system_tree_seq_name*
scorep_system_tree_seq_create_name_data( void );

/**
 * Releases the data structure for name construction.
 * @param nameData The object that is freed.
 */
void
scorep_system_tree_seq_free_name_data( scorep_system_tree_seq_name* nameData );

/**
 * Constructs a system tree node name, location group name or location name
 * from a sequence definition during a depth-first traversal of the system tree.
 * @param node      The sequence definition of the system tree element to name.
 * @param copy      The index of the current copy of the current system tree element.
 * @param nameData  A data object which keeps the state of during the traveral.
 */
char*
scorep_system_tree_seq_get_name( scorep_system_tree_seq*      node,
                                 uint64_t                     copy,
                                 scorep_system_tree_seq_name* nameData );

/**
 * Constructs a class name for system tree node from a sequence definition.
 * Works only for system tree nodes and not for location group or locations.
 * @param node      The sequence definition of the system tree element to name.
 * @param nameData  A data object which keeps the state during the traversal.
 */
const char*
scorep_system_tree_seq_get_class( scorep_system_tree_seq*      node,
                                  scorep_system_tree_seq_name* nameData );

#endif /* SCOREP_SYSTEM_TREE_SEQUENCE_H */
