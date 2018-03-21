/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 */


#ifndef SCOREP_UNWINDING_INTERNAL_H
#define SCOREP_UNWINDING_INTERNAL_H

#include <UTILS_Error.h>

#include <SCOREP_DefinitionHandles.h>

#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <stdbool.h>


/** Number of buckets in the region hash table */
#define HASH_SIZE            2048

/** Size of the region name buffer passed to libunwind. */
#define  MAX_FUNC_NAME_LENGTH 1024


/** Our subsystem id, used to address our per-location unwinding data */
extern size_t scorep_unwinding_subsystem_id;


/**
 * A record of this type associates the start instruction of a function
 * with Score-P related data for that function.
 */
typedef struct scorep_unwinding_region
{
    /** left and right child in the splay tree */
    struct scorep_unwinding_region* left;
    struct scorep_unwinding_region* right;

    /** Start IP of a function */
    uint64_t            start;
    /** End IP of a function (exclusive) */
    uint64_t            end;
    /** Handle to region definition */
    SCOREP_RegionHandle handle;

    /** Indicates whether this function should be skipped */
    bool skip;
    /** True if this region represents main */
    bool is_main;
    /** True if this region represents a fork event spawning additional threads */
    bool is_fork;

    /** Region name */
    char name[ 1 ];
} scorep_unwinding_region;


/**
 * Data structure of a node on the calling context tree (CCT)
 */
typedef struct scorep_unwinding_calling_context_tree_node
{
    /** Definition handle */
    SCOREP_CallingContextHandle                        handle;
    /** Instruction pointer */
    uint64_t                                           ip;
    /** Region handle */
    SCOREP_RegionHandle                                region;
    /** Splay-tree of children nodes */
    struct scorep_unwinding_calling_context_tree_node* children;
    /** Pointer to left sibling element */
    struct scorep_unwinding_calling_context_tree_node* left;
    /** Pointer to right sibling element */
    struct scorep_unwinding_calling_context_tree_node* right;
} scorep_unwinding_calling_context_tree_node;


/**
 * Object for a frame in the current unwinding stack.
 */
typedef struct scorep_unwinding_frame
{
    /** Next frame (i.e., the one who was called by this frame) */
    struct scorep_unwinding_frame* next;
    /** The instruction address for this frame */
    uint64_t                       ip;
    /** The region for this frame */
    scorep_unwinding_region*       region;
} scorep_unwinding_frame;


/**
 * Object for a replaced region in the augmented stack.
 */
typedef struct scorep_unwinding_surrogate
{
    /** The previous surrogate */
    struct scorep_unwinding_surrogate*          prev;
    /** The instruction address for this surrogate */
    uint64_t                                    ip;
    /** The region handle for this surrogate */
    SCOREP_RegionHandle                         region_handle;
    /** The unwind context of this surrogate */
    scorep_unwinding_calling_context_tree_node* unwind_context;
    /** is this a wrapped region */
    bool                                        is_wrapped;
} scorep_unwinding_surrogate;


/**
 * Stack frame in the augmented stack.
 *
 * If @p ::surrogates is NULL, than this frame represents a frame function
 *
 * Else
 *   @p ::surrogates->region is the instrumented region of this frame
 *   @p ::surrogates->unwind_context is the unwind context
 */
typedef struct scorep_unwinding_augmented_frame
{
    /** double linked list: the parent */
    struct scorep_unwinding_augmented_frame* next;
    /** double linked list: the child */
    struct scorep_unwinding_augmented_frame* prev;

    /** The orignal region for this frame, wont changed by surrogate */
    scorep_unwinding_region* region;

    /** The current instruction address for this frame, may change by surrogate */
    uint64_t ip;

    /** Surrogates of this frame */
    scorep_unwinding_surrogate* surrogates;
} scorep_unwinding_augmented_frame;

/**
 * Maintaines unhandled PushWrapper
 */
typedef struct scorep_unwinding_unhandled_wrapper
{
    struct scorep_unwinding_unhandled_wrapper* next;

    /** An address inside the wrapper region, i.e., the one who called the original function */
    uint64_t            wrapper_ip;
    /** The number of wrappers involved in this wrapping (i.e., in-measurement) */
    size_t              n_wrapper_frames;
    /** The handle for the wrapped region */
    SCOREP_RegionHandle wrappee_handle;
} scorep_unwinding_unhandled_wrapper;

typedef union scorep_unwinding_unused_object
{
    union scorep_unwinding_unused_object* next;
    scorep_unwinding_frame                frame;
    scorep_unwinding_surrogate            surrogate;
    scorep_unwinding_augmented_frame      augmented_frame;
    scorep_unwinding_unhandled_wrapper    unhandled_wrapper;
} scorep_unwinding_unused_object;

/** Per-location based data related to unwinding for all CPU locations. */
typedef struct SCOREP_Unwinding_CpuLocationData
{
    struct SCOREP_Location* location;

    /** Unused objects */
    scorep_unwinding_unused_object* unused_objects;

    /** The augumented stack with instrumented regions, NULL if no instrumented
     *  region is on the stack, points to the last element entered instrumented region */
    scorep_unwinding_augmented_frame* augmented_stack;

    /** Wrapper meta data, before they re used in actual unwinding */
    scorep_unwinding_unhandled_wrapper* unhandled_wrappers;

    /** The address of the main function */
    uint64_t start_ip_of_main;
    /** Instruction pointer of the fork event */
    uint64_t start_ip_of_fork;

    /** Splay tree containing already known regions */
    scorep_unwinding_region* known_regions;

    /** Stack unwinding management data, keep it here instead of on the stack because they tend to be large */
    unw_context_t context;
    /** Current cursor position in the stack, used to unwind the stack */
    unw_cursor_t  cursor;

    /** Root calling context node for all generated nodes,
        i.e., the SCOREP_INVALID_CALLING_CONTEXT node. */
    scorep_unwinding_calling_context_tree_node calling_context_root;

    /** Last known calling context */
    SCOREP_CallingContextHandle previous_calling_context;

    /* Below is storage normally allocated on the stack.
       As they are rather big, we allocate them in the per-location data.
       None of these variables are used in recursive calls. */

    /** Buffer to retrieve the region name from libunwind. */
    char region_name_buffer[ MAX_FUNC_NAME_LENGTH ];
} SCOREP_Unwinding_CpuLocationData;


/**
 * Element in the list of instrumented regions.
 */
typedef struct scorep_unwinding_instrumented_region
{
    /** Corresponding region handle */
    SCOREP_RegionHandle                          region_handle;
    /** Pointer to corresponding node in the calling context tree */
    scorep_unwinding_calling_context_tree_node*  unwind_context;
    /** Pointer to previous instrumented function in the stack */
    struct scorep_unwinding_instrumented_region* prev;
} scorep_unwinding_instrumented_region;

/** Per-location based data related to unwinding for all GPU locations. */
typedef struct SCOREP_Unwinding_GpuLocationData
{
    struct SCOREP_Location* location;

    /** List of regions that were entered through instrumentation */
    scorep_unwinding_instrumented_region* instrumented_regions;
    /** List with free elements for the inst_list */
    scorep_unwinding_instrumented_region* unused_instrumented_regions;
    /** Root calling context node for all generated nodes,
        i.e., the SCOREP_INVALID_CALLING_CONTEXT node. */
    scorep_unwinding_calling_context_tree_node calling_context_root;
    /** Last known calling context */
    SCOREP_CallingContextHandle                previous_calling_context;
} SCOREP_Unwinding_GpuLocationData;


SCOREP_ErrorCode
scorep_unwinding_unify( void );


#endif /* SCOREP_UNWINDING_INTERNAL_H */
