/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief
 */

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include <SCOREP_AllocMetric.h>

#include <scorep/SCOREP_PublicTypes.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Location.h>
#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Events.h>
#include <SCOREP_Memory.h>
#include <scorep_substrates_definition.h>

#define SCOREP_DEBUG_MODULE_NAME MEMORY
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

/*
 * The key is a pointer address of an allocation,
 * the value the corresponding size of the allocated memory
 * and an array where substrates may maintain substrate local
 * data in SCOREP_TrackAlloc/Free events.
 */
typedef struct allocation_item
{
    struct allocation_item* left;
    struct allocation_item* right;
    uint64_t                address; /**< pointer address of allocated memory */
    size_t                  size;    /**< allocated memory */
    void*                   substrate_data[ SCOREP_SUBSTRATES_NUM_SUBSTRATES ];
} allocation_item;

typedef struct free_list_item
{
    struct free_list_item* next;
} free_list_item;

struct SCOREP_AllocMetric
{
    SCOREP_Mutex             mutex;

    allocation_item*         allocations;
    free_list_item*          free_list;

    SCOREP_SamplingSetHandle sampling_set;
    uint64_t                 total_allocated_memory;
};


/* Splay tree based on: */
/*
                An implementation of top-down splaying
                    D. Sleator <sleator@cs.cmu.edu>
                            March 1992

   "Splay trees", or "self-adjusting search trees" are a simple and
   efficient data structure for storing an ordered set.  The data
   structure consists of a binary tree, without parent pointers, and no
   additional fields.  It allows searching, insertion, deletion,
   deletemin, deletemax, splitting, joining, and many other operations,
   all with amortized logarithmic performance.  Since the trees adapt to
   the sequence of requests, their performance on real access patterns is
   typically even better.  Splay trees are described in a number of texts
   and papers [1,2,3,4,5].

   The code here is adapted from simple top-down splay, at the bottom of
   page 669 of [3].  It can be obtained via anonymous ftp from
   spade.pc.cs.cmu.edu in directory /usr/sleator/public.

   The chief modification here is that the splay operation works even if the
   item being splayed is not in the tree, and even if the tree root of the
   tree is NULL.  So the line:

                              t = splay(i, t);

   causes it to search for item with key i in the tree rooted at t.  If it's
   there, it is splayed to the root.  If it isn't there, then the node put
   at the root is the last one before NULL that would have been reached in a
   normal binary search for i.  (It's a neighbor of i in the tree.)  This
   allows many other operations to be easily implemented, as shown below.

   [1] "Fundamentals of data structures in C", Horowitz, Sahni,
       and Anderson-Freed, Computer Science Press, pp 542-547.
   [2] "Data Structures and Their Algorithms", Lewis and Denenberg,
       Harper Collins, 1991, pp 243-251.
   [3] "Self-adjusting Binary Search Trees" Sleator and Tarjan,
       JACM Volume 32, No 3, July 1985, pp 652-686.
   [4] "Data Structure and Algorithm Analysis", Mark Weiss,
       Benjamin Cummins, 1992, pp 119-130.
   [5] "Data Structures, Algorithms, and Performance", Derick Wood,
       Addison-Wesley, 1993, pp 367-375.
 */

static allocation_item*
splay( allocation_item* root,
       uint64_t         key )
{
    allocation_item  sentinel;
    allocation_item* left;
    allocation_item* right;
    allocation_item* node;

    if ( root == NULL )
    {
        return root;
    }

    sentinel.left = sentinel.right = NULL;
    left          = right = &sentinel;

    for (;; )
    {
        if ( key < root->address )
        {
            if ( root->left == NULL )
            {
                break;
            }
            if ( key < root->left->address )
            {
                node        = root->left;                    /* rotate right */
                root->left  = node->right;
                node->right = root;
                root        = node;
                if ( root->left == NULL )
                {
                    break;
                }
            }
            right->left = root;                               /* link right */
            right       = root;
            root        = root->left;
        }
        else if ( key > root->address )
        {
            if ( root->right == NULL )
            {
                break;
            }
            if ( key > root->right->address )
            {
                node        = root->right;                   /* rotate left */
                root->right = node->left;
                node->left  = root;
                root        = node;
                if ( root->right == NULL )
                {
                    break;
                }
            }
            left->right = root;                              /* link left */
            left        = root;
            root        = root->right;
        }
        else
        {
            break;
        }
    }

    left->right = root->left;                                /* assemble */
    right->left = root->right;
    root->left  = sentinel.right;
    root->right = sentinel.left;

    return root;
}


static void
insert_memory_allocation( SCOREP_AllocMetric* allocMetric,
                          allocation_item*    allocation )
{
    if ( allocMetric->allocations )
    {
        allocMetric->allocations = splay( allocMetric->allocations, allocation->address );
        if ( allocation->address < allocMetric->allocations->address )
        {
            allocation->right       = allocMetric->allocations;
            allocation->left        = allocation->right->left;
            allocation->right->left = NULL;
        }
        else if ( allocation->address > allocMetric->allocations->address )
        {
            allocation->left        = allocMetric->allocations;
            allocation->right       = allocation->left->right;
            allocation->left->right = NULL;
        }
        else
        {
            UTILS_WARNING( "Allocation already known: 0x%" PRIx64, allocation->address );
        }
    }
    allocMetric->allocations = allocation;
}

static allocation_item*
add_memory_allocation( SCOREP_AllocMetric* allocMetric,
                       uint64_t            addr,
                       size_t              size )
{
    if ( allocMetric == NULL )
    {
        return NULL;
    }

    allocation_item* new_item = ( allocation_item* )allocMetric->free_list;
    if ( new_item )
    {
        allocMetric->free_list = allocMetric->free_list->next;
    }
    else
    {
        new_item = SCOREP_Memory_AllocForMisc( sizeof( *new_item ) );
    }
    memset( new_item, 0, sizeof( *new_item ) );
    new_item->address = addr;
    new_item->size    = size;

    insert_memory_allocation( allocMetric, new_item );

    return new_item;
}

static allocation_item*
find_memory_allocation( SCOREP_AllocMetric* allocMetric,
                        uint64_t            addr )
{
    if ( allocMetric == NULL || allocMetric->allocations == NULL )
    {
        return NULL;
    }

    allocMetric->allocations = splay( allocMetric->allocations, addr );
    if ( addr == allocMetric->allocations->address )
    {
        return allocMetric->allocations;
    }

    return NULL;
}


static void
remove_memory_allocation( SCOREP_AllocMetric* allocMetric,
                          allocation_item*    allocation )
{
    if ( allocMetric == NULL
         || allocMetric->allocations == NULL
         || allocMetric->allocations != allocation )
    {
        return;
    }

    if ( allocation->left == NULL )
    {
        allocMetric->allocations = allocation->right;
    }
    else
    {
        /* Serach in the sub-tree where all entries are smaller than the allocation
         * to delete, the bigest, for this, the right child must be NULL */
        allocMetric->allocations        = splay( allocation->left, allocation->address );
        allocMetric->allocations->right = allocation->right;
    }

    allocation->right = NULL;
    allocation->left  = NULL;
}

static void
free_memory_allocation( SCOREP_AllocMetric* allocMetric,
                        allocation_item*    allocation )
{
    free_list_item* next = allocMetric->free_list;
    allocMetric->free_list       = ( free_list_item* )allocation;
    allocMetric->free_list->next = next;
}

static void
delete_memory_allocation( SCOREP_AllocMetric* allocMetric,
                          allocation_item*    allocation )
{
    remove_memory_allocation( allocMetric, allocation );
    free_memory_allocation( allocMetric, allocation );
}

/* Keep track of the allocated memory per process, not only per SCOREP_AllocMetric */
static size_t       process_allocated_memory;
static SCOREP_Mutex process_allocated_memory_mutex;
static int          n_alloc_metric_objects;

SCOREP_ErrorCode
SCOREP_AllocMetric_New( const char*          name,
                        SCOREP_AllocMetric** allocMetric )
{
    /* not so nice. Make AllocMetric a subsystem? */
    if ( n_alloc_metric_objects == 0 )
    {
        SCOREP_MutexCreate( &process_allocated_memory_mutex );
    }
    n_alloc_metric_objects += 1;

    *allocMetric = SCOREP_Memory_AllocForMisc( sizeof( **allocMetric ) );
    memset( *allocMetric, 0, sizeof( **allocMetric ) );

    SCOREP_MutexCreate( &( *allocMetric )->mutex );

    SCOREP_MetricHandle metric_handle =
        SCOREP_Definitions_NewMetric( name,
                                      name,
                                      SCOREP_METRIC_SOURCE_TYPE_OTHER,
                                      SCOREP_METRIC_MODE_ABSOLUTE_NEXT,
                                      SCOREP_METRIC_VALUE_UINT64,
                                      SCOREP_METRIC_BASE_DECIMAL,
                                      0,
                                      "bytes",
                                      SCOREP_METRIC_PROFILING_TYPE_MAX,
                                      SCOREP_INVALID_METRIC );

    SCOREP_SamplingSetHandle sampling_set_handle =
        SCOREP_Definitions_NewSamplingSet( 1,
                                           &metric_handle,
                                           SCOREP_METRIC_OCCURRENCE_ASYNCHRONOUS,
                                           SCOREP_SAMPLING_SET_ABSTRACT );

    SCOREP_Location* per_process_metric_location =
        SCOREP_Location_AcquirePerProcessMetricsLocation( NULL );
    ( *allocMetric )->sampling_set =
        SCOREP_Definitions_NewScopedSamplingSet( sampling_set_handle,
                                                 SCOREP_Location_GetLocationHandle( per_process_metric_location ),
                                                 SCOREP_METRIC_SCOPE_LOCATION_GROUP,
                                                 SCOREP_GetLocationGroup() );
    SCOREP_Location_ReleasePerProcessMetricsLocation();

    return SCOREP_SUCCESS;
}


void
SCOREP_AllocMetric_Destroy( SCOREP_AllocMetric* allocMetric )
{
    SCOREP_MutexDestroy( &allocMetric->mutex );
    /* not so nice. Make AllocMetric a subsystem? */
    n_alloc_metric_objects -= 1;
    if ( n_alloc_metric_objects == 0 )
    {
        SCOREP_MutexDestroy( &process_allocated_memory_mutex );
    }
}


void
SCOREP_AllocMetric_AcquireAlloc( SCOREP_AllocMetric* allocMetric,
                                 uint64_t            addr,
                                 void**              allocation )
{
    SCOREP_MutexLock( allocMetric->mutex );

    UTILS_DEBUG_ENTRY( "%p", ( void* )addr );

    UTILS_BUG_ON( addr == 0, "Can't acquire allocation for NULL pointers." );

    *allocation = find_memory_allocation( allocMetric, addr );
    if ( *allocation )
    {
        remove_memory_allocation( allocMetric, *allocation );
    }
    else
    {
        UTILS_WARNING( "Could not find allocation %p.",
                       ( void* )addr );
    }

    UTILS_DEBUG_EXIT( "Total Memory: %" PRIu64, allocMetric->total_allocated_memory );

    SCOREP_MutexUnlock( allocMetric->mutex );
}


void
SCOREP_AllocMetric_HandleAlloc( SCOREP_AllocMetric* allocMetric,
                                uint64_t            resultAddr,
                                size_t              size )
{
    SCOREP_MutexLock( allocMetric->mutex );

    UTILS_DEBUG_ENTRY( "%p , %zu", ( void* )resultAddr, size );

    SCOREP_MutexLock( process_allocated_memory_mutex );
    process_allocated_memory += size;
    uint64_t process_allocated_memory_save = process_allocated_memory;
    SCOREP_MutexUnlock( process_allocated_memory_mutex );

    allocMetric->total_allocated_memory += size;
    allocation_item* allocation =
        add_memory_allocation( allocMetric, resultAddr, size );

    /* We need to ensure, that we take the timestamp  *after* we acquired
       the metric location, else we may end up with an invalid timestamp order */
    uint64_t         timestamp;
    SCOREP_Location* per_process_metric_location =
        SCOREP_Location_AcquirePerProcessMetricsLocation( &timestamp );
    SCOREP_Location_TriggerCounterUint64( per_process_metric_location,
                                          timestamp,
                                          allocMetric->sampling_set,
                                          allocMetric->total_allocated_memory );
    SCOREP_Location_ReleasePerProcessMetricsLocation();

    SCOREP_TrackAlloc( resultAddr, size, allocation->substrate_data,
                       allocMetric->total_allocated_memory,
                       process_allocated_memory_save );

    UTILS_DEBUG_EXIT( "Total Memory: %" PRIu64, allocMetric->total_allocated_memory );

    SCOREP_MutexUnlock( allocMetric->mutex );
}


void
SCOREP_AllocMetric_HandleRealloc( SCOREP_AllocMetric* allocMetric,
                                  uint64_t            resultAddr,
                                  size_t              size,
                                  void*               prevAllocation,
                                  uint64_t*           prevSize )
{
    SCOREP_MutexLock( allocMetric->mutex );

    UTILS_DEBUG_ENTRY( "%p , %zu, %p", ( void* )resultAddr, size, prevAllocation );

    uint64_t total_allocated_memory_save;
    uint64_t process_allocated_memory_save;

    /* get the handle of the previously allocated memory */
    allocation_item* allocation = prevAllocation;
    if ( allocation )
    {
        if ( prevSize )
        {
            /* report previous size to caller. */
            *prevSize = allocation->size;
        }

        /*
         * If the allocation did not resulted in a new address, than assign
         * the new size to the handle. */
        if ( allocation->address == resultAddr )
        {
            SCOREP_MutexLock( process_allocated_memory_mutex );
            process_allocated_memory     += ( size - allocation->size );
            process_allocated_memory_save = process_allocated_memory;
            SCOREP_MutexUnlock( process_allocated_memory_mutex );

            allocMetric->total_allocated_memory += ( size - allocation->size );
            total_allocated_memory_save          = allocMetric->total_allocated_memory;

            SCOREP_TrackRealloc( allocation->address, allocation->size, allocation->substrate_data,
                                 resultAddr, size, allocation->substrate_data,
                                 total_allocated_memory_save,
                                 process_allocated_memory_save );

            allocation->size = size;
            insert_memory_allocation( allocMetric, allocation );
        }
        /* System allocates size before freeing allocation->size (actually,
         * a free(prevAddr) is done), report the memory usage after the allocation
         * but before the free as total_allocated_memory, but reduce afterwards
         * about freed_mem. */
        else
        {
            SCOREP_MutexLock( process_allocated_memory_mutex );
            process_allocated_memory     += size;
            process_allocated_memory_save = process_allocated_memory;
            process_allocated_memory     -= allocation->size;
            SCOREP_MutexUnlock( process_allocated_memory_mutex );

            allocMetric->total_allocated_memory += size;
            total_allocated_memory_save          = allocMetric->total_allocated_memory;
            allocMetric->total_allocated_memory -= allocation->size;

            SCOREP_TrackRealloc( allocation->address, allocation->size, allocation->substrate_data,
                                 resultAddr, size, allocation->substrate_data,
                                 total_allocated_memory_save,
                                 process_allocated_memory_save );

            allocation->address = resultAddr;
            allocation->size    = size;
            insert_memory_allocation( allocMetric, allocation );
        }
    }
    else
    {
        UTILS_WARNING( "Could not find previous allocation." );

        if ( prevSize )
        {
            *prevSize = 0;
        }

        SCOREP_MutexLock( process_allocated_memory_mutex );
        process_allocated_memory     += size;
        process_allocated_memory_save = process_allocated_memory;
        SCOREP_MutexUnlock( process_allocated_memory_mutex );

        allocMetric->total_allocated_memory += size;
        total_allocated_memory_save          = allocMetric->total_allocated_memory;

        allocation = add_memory_allocation( allocMetric, resultAddr, size );
        SCOREP_TrackAlloc( resultAddr, size, allocation->substrate_data,
                           total_allocated_memory_save,
                           process_allocated_memory_save );
    }

    /* We need to ensure, that we take the timestamp  *after* we acquired
       the metric location, else we may end up with an invalid timestamp order */
    uint64_t         timestamp;
    SCOREP_Location* per_process_metric_location =
        SCOREP_Location_AcquirePerProcessMetricsLocation( &timestamp );
    SCOREP_Location_TriggerCounterUint64( per_process_metric_location,
                                          timestamp,
                                          allocMetric->sampling_set,
                                          total_allocated_memory_save );
    SCOREP_Location_ReleasePerProcessMetricsLocation();

    UTILS_DEBUG_EXIT( "Total Memory: %" PRIu64, allocMetric->total_allocated_memory );

    SCOREP_MutexUnlock( allocMetric->mutex );
}


void
SCOREP_AllocMetric_HandleFree( SCOREP_AllocMetric* allocMetric,
                               void*               allocation_,
                               uint64_t*           size )
{
    SCOREP_MutexLock( allocMetric->mutex );

    UTILS_DEBUG_ENTRY( "%p", allocation_ );

    allocation_item* allocation = allocation_;
    if ( !allocation )
    {
        UTILS_WARNING( "Could not find previous allocation, ignoring event." );

        if ( size )
        {
            *size = 0;
        }

        SCOREP_MutexUnlock( allocMetric->mutex );
        return;
    }

    uint64_t allocation_addr   = allocation->address;
    uint64_t deallocation_size = allocation->size;

    SCOREP_MutexLock( process_allocated_memory_mutex );
    process_allocated_memory -= deallocation_size;
    uint64_t process_allocated_memory_save = process_allocated_memory;
    SCOREP_MutexUnlock( process_allocated_memory_mutex );

    allocMetric->total_allocated_memory -= deallocation_size;

    void* substrate_data[ SCOREP_SUBSTRATES_NUM_SUBSTRATES ];
    memcpy( substrate_data, allocation->substrate_data,
            SCOREP_SUBSTRATES_NUM_SUBSTRATES * sizeof( void* ) );
    free_memory_allocation( allocMetric, allocation );

    /* We need to ensure, that we take the timestamp  *after* we acquired
       the metric location, else we may end up with an invalid timestamp order */
    uint64_t         timestamp;
    SCOREP_Location* per_process_metric_location =
        SCOREP_Location_AcquirePerProcessMetricsLocation( &timestamp );
    SCOREP_Location_TriggerCounterUint64( per_process_metric_location,
                                          timestamp,
                                          allocMetric->sampling_set,
                                          allocMetric->total_allocated_memory );
    SCOREP_Location_ReleasePerProcessMetricsLocation();

    if ( size )
    {
        *size = deallocation_size;
    }

    SCOREP_TrackFree( allocation_addr, deallocation_size, substrate_data,
                      allocMetric->total_allocated_memory,
                      process_allocated_memory_save );

    UTILS_DEBUG_EXIT( "Total Memory: %" PRIu64, allocMetric->total_allocated_memory );

    SCOREP_MutexUnlock( allocMetric->mutex );
}


void
SCOREP_AllocMetric_ReportLeaked( SCOREP_AllocMetric* allocMetric )
{
    /* walk through tree, every item represents leaked memory */
    while ( allocMetric->allocations != NULL )
    {
        allocation_item* node = allocMetric->allocations;

        UTILS_DEBUG( "[leaked] ptr %p, size %zu",
                     ( void* )( node->address ), node->size );

        SCOREP_LeakedMemory( node->address,
                             node->size,
                             node->substrate_data );
        delete_memory_allocation( allocMetric, node );
    }
}

SCOREP_AttributeHandle
SCOREP_AllocMetric_GetAllocationSizeAttribute( void )
{
    /* The definition component takes care of locking and duplicates */
    /* The SCOREP_MetricProfilingType argument is 'INVALID' as passing the metric
     * as an attribute to the profiling substrates prevents parameter events to
     * work as expected. The 'why' needs to be investigated. */
    return SCOREP_Definitions_NewAttribute(
        "ALLOCATION_SIZE",
        "Size of memory to allocate",
        SCOREP_ATTRIBUTE_TYPE_UINT64 );
}

SCOREP_AttributeHandle
SCOREP_AllocMetric_GetDeallocationSizeAttribute( void )
{
    /* The definition component takes care of locking and duplicates */
    /* The SCOREP_MetricProfilingType argument is 'INVALID' as passing the metric
     * as an attribute to the profiling substrates prevents parameter events to
     * work as expected. The 'why' needs to be investigated. */
    return SCOREP_Definitions_NewAttribute(
        "DEALLOCATION_SIZE",
        "Size of memory to deallocate",
        SCOREP_ATTRIBUTE_TYPE_UINT64 );
}
