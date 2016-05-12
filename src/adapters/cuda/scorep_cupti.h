/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  This file provides commonly used definitions and functionality for all CUPTI
 *  interface (events, callbacks and activity)
 */

#ifndef SCOREP_CUPTI_H
#define SCOREP_CUPTI_H

#include <stdint.h>
#include <stdbool.h>

#include <SCOREP_Mutex.h>
#include "SCOREP_Definitions.h"
#include "SCOREP_Location.h"
#include "SCOREP_Memory.h"
#include "SCOREP_Events.h"

#include <SCOREP_RuntimeManagement.h>

#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include <cupti.h>

#define SCOREP_CUPTI_NO_ID          0xFFFFFFFF
#define SCOREP_CUPTI_NO_DEVICE      ( CUdevice )0xFFFFFFFF
#define SCOREP_CUPTI_NO_DEVICE_ID   0xFFFFFFFF
#define SCOREP_CUPTI_NO_CONTEXT_ID  0xFFFFFFFF
#define SCOREP_CUPTI_NO_STREAM      ( CUstream )0xFFFFFFFF
#define SCOREP_CUPTI_NO_STREAM_ID   0xFFFFFFFF

#define SCOREP_CUDA_DRIVER_CALL( fct ) \
    if ( fct != CUDA_SUCCESS ) { \
        UTILS_WARNING( "[CUDA] Call to '%s' failed.", #fct ); \
    }

#define SCOREP_CUPTI_CALL( fct )                                        \
    {                                                                   \
        CUptiResult _status = fct;                                      \
        if ( _status != CUPTI_SUCCESS )                                 \
        {                                                               \
            const char* msg;                                            \
            cuptiGetResultString( _status, & msg );                      \
            UTILS_WARNING( "[CUPTI] Call to '%s' failed with message: '%s'",  #fct, msg ); \
        }                                                               \
    }

#if HAVE( DEMANGLE )
extern char*
cplus_demangle( const char* mangled,
                int         options );

   #define SCOREP_DEMANGLE_CUDA_KERNEL( mangled ) \
    cplus_demangle( mangled, 0 )
#else
#define SCOREP_DEMANGLE_CUDA_KERNEL( mangled ) \
    ( char* )mangled
#endif

/* the default size for the CUDA kernel name hash table */
#define SCOREP_CUDA_KERNEL_HASHTABLE_SIZE 1024

/* thread (un)locking macros for CUPTI interfaces */
extern SCOREP_Mutex scorep_cupti_mutex;
# define SCOREP_CUPTI_LOCK() SCOREP_MutexLock( scorep_cupti_mutex )
# define SCOREP_CUPTI_UNLOCK() SCOREP_MutexUnlock( scorep_cupti_mutex )

typedef struct scorep_cuda_location_data
{
    uint8_t callbacksState;    //!< combination of SCOREP_CUPTI_CALLBACKS_STATE_*
    bool    rma_window_active; //!< RMA window active for this location?
} scorep_cuda_location_data;

/*
 * The key of the hash node is a string, the value the corresponding region handle.
 * It is used to store region names with its corresponding region handles.
 */
typedef struct scorep_cuda_kernel_hash_node
{
    char*                                name;   /**< name of the symbol */
    SCOREP_RegionHandle                  region; /**< associated region handle */
    struct scorep_cuda_kernel_hash_node* next;   /**< bucket for collision */
} scorep_cuda_kernel_hash_node;

/*
 * Score-P CUPTI stream
 */
typedef struct scorep_cupti_stream
{
    uint32_t                    stream_id;               /**< the CUDA stream ID */
    SCOREP_Location*            scorep_location;         /**< Score-P location for this stream (unique) */
    uint32_t                    location_id;             /**< internal location ID used for unification */
    uint64_t                    scorep_last_timestamp;   /**< last written Score-P timestamp */
    struct scorep_cupti_stream* next;
} scorep_cupti_stream;

/*
 * data structure contains information about allocated CUDA memory
 */
typedef struct scorep_cupti_gpumem
{
    uint64_t                    address;    /**< pointer value to allocated memory */
    size_t                      size;       /**< number of bytes allocated */
    uint32_t                    tid;        /**< thread id used with this malloc */
    struct scorep_cupti_gpumem* next;
} scorep_cupti_gpumem;

/*
 * Score-P CUPTI activity synchronization structure
 */
typedef struct
{
    uint64_t host_start; /**< host measurement interval start timestamp */
    uint64_t host_stop;  /**< host measurement interval stop timestamp */
    uint64_t gpu_start;  /**< gpu measurement interval start timestamp */
    uint64_t gpu_stop;   /**< gpu measurement interval stop timestamp */
    double   factor;     /**< synchronization factor for time interval */
} scorep_cupti_sync;

#if HAVE( CUPTI_ASYNC_SUPPORT )
/**
 * Score-P CUPTI activity buffers list entry
 */
typedef struct scorep_cupti_buffer
{
    uint8_t*                    buffer;     /**< activity records buffer */
    uint32_t                    stream_id;  /**< CUDA stream id */
    size_t                      valid_size; /**< valid bytes in buffer */
    size_t                      size;       /**< total size (bytes) in buffer */
    bool                        committed;  /**< set if buffer is controlled by CUPTI */
    bool                        pending;    /**< set if buffer has been filled by CUPTI */
    struct scorep_cupti_buffer* next;       /**< ptr to next entry in list */
} scorep_cupti_buffer;
#endif

/*
 * Score-P CUPTI activity specific context data.
 */
typedef struct
{
    uint32_t             default_strm_id;          /**< CUPTI stream ID of default stream */
    scorep_cupti_sync    sync;                     /**< store synchronization information */
#if HAVE( CUPTI_ASYNC_SUPPORT )
    scorep_cupti_buffer* buffers;                  /**< CUPTI activity buffers list */
    bool                 max_buffer_size_exceeded; /**< flag if the maximum allowed buffer size is reached */
#else
    uint8_t*             buffer;                   /**< CUPTI activity buffer pointer */
#endif
    uint64_t             scorep_last_gpu_time;     /**< last written Score-P timestamp */
    bool                 gpu_idle;                 /**< has idle region enter been written last */
} scorep_cupti_activity;

/*
 * Score-P CUPTI context.
 */
typedef struct scorep_cupti_context
{
    CUcontext                    cuda_context;         /**< CUDA context handle */
    uint32_t                     context_id;           /**< context ID */
    uint32_t                     device_id;            /**< device ID */
    CUdevice                     cuda_device;          /**< CUDA device handle */
    SCOREP_Location*             scorep_host_location; /**< Score-P context host location */
    uint32_t                     location_id;          /**< internal location ID used for unification */
    scorep_cupti_stream*         streams;              /**< list of Score-P CUDA streams */
    scorep_cupti_gpumem*         cuda_mallocs;         /**< list of allocated GPU memory fields */
    scorep_cupti_gpumem*         free_cuda_mallocs;    /**< free-list of allocated GPU memory fields */
    size_t                       gpu_memory_allocated; /**< memory allocated on CUDA device */
    scorep_cupti_activity*       activity;
    struct scorep_cupti_context* next;
} scorep_cupti_context;

/*
 * Score-P CUPTI attribute handles
 */
typedef struct
{
    SCOREP_AttributeHandle stream_ref;                 /**< CUDA DRV stream references */
    SCOREP_AttributeHandle event_ref;                  /**< CUDA DRV events */
    SCOREP_AttributeHandle result_ref;                 /**< CUDA DRV API function results */
} scorep_cupti_attribute_handles;

extern scorep_cupti_attribute_handles scorep_cupti_attributes;

extern scorep_cupti_context* scorep_cupti_context_list;

extern size_t scorep_cupti_location_counter;

/* handle for kernel regions */
extern SCOREP_SourceFileHandle scorep_cupti_kernel_file_handle;

/* handle CUDA idle regions */
extern SCOREP_RegionHandle scorep_cupti_idle_region_handle;

extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_threads_per_kernel;
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_threads_per_block;
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_blocks_per_grid;

/* CUPTI activity specific kernel counter IDs */
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_static_shared_mem;
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_dynamic_shared_mem;
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_local_mem_total;
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_registers_per_thread;

extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_gpumemusage;

/*
 * Initialization of common variables for the Score-P CUPTI implementation.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_cupti_init( void );

/*
 * Finalization of common variables in the Score-P CUPTI implementation.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_cupti_finalize( void );

/*
 * Handles errors returned from CUPTI function calls.
 *
 * @param err the CUDA driver API error code
 * @param msg a message to get more detailed information about the error
 * @param file pointer to the corresponding file name
 * @param line the line the error occurred
 */
void
scorep_cupti_handle_error( CUptiResult err,
                           const char* msg,
                           const char* file,
                           const int   line );

/*
 * Create a Score-P CUPTI context.
 *
 * @param cudaContext CUDA context
 * @param cudaDevice CUDA device
 * @param cudaContextId ID of the CUDA context
 * @param cudaDeviceId ID of the CUDA device
 *
 * @return pointer to created Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_create( CUcontext cudaContext,
                             CUdevice  cudaDevice,
                             uint32_t  cudaContextId,
                             uint32_t  cudaDeviceId );

/*
 * Get a Score-P CUPTI context by CUDA context.
 *
 * @param cudaContext the CUDA context
 *
 * @return Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_get( CUcontext cudaContext );

/*
 * Get a Score-P CUPTI context by CUDA context ID
 *
 * @param contextId the CUDA context ID
 *
 * @return Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_get_by_id( uint32_t contextId );

/*
 * Get or if not available create a Score-P CUPTI context by CUDA context.
 *
 * @param cudaContext the CUDA context
 *
 * @return Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_get_create( CUcontext cudaContext );

/*
 * Remove a context from the global context list and return it.
 *
 * @param cudaContext the CUDA context
 *
 * @return the Score-P CUPTI context, which has been removed
 */
scorep_cupti_context*
scorep_cupti_context_remove( CUcontext cudaContext );

/*
 * Finalize the Score-P CUPTI context and free all memory allocated with it.
 *
 * @param context pointer to the Score-P CUPTI context
 */
void
scorep_cupti_context_finalize( scorep_cupti_context* context );

/*
 * Create a Score-P CUPTI stream.
 *
 * @param context Score-P CUPTI context
 * @param streamId ID of the CUDA stream
 *
 * @return pointer to created Score-P CUPTI stream
 */
scorep_cupti_stream*
scorep_cupti_stream_create(
    scorep_cupti_context* context,
    uint32_t              streamId );

/*
 * Get a Score-P CUPTI stream by CUDA stream without locking.
 *
 * @param context pointer to the Score-P CUPTI context, containing the stream
 * @param streamId the CUPTI stream ID
 *
 * @return Score-P CUPTI stream
 */
scorep_cupti_stream*
scorep_cupti_stream_get_by_id(
    scorep_cupti_context* context,
    uint32_t              streamId );

/*
 * Retrieve a Score-P CUPTI stream object. This function will lookup, if
 * the stream is already available, a stream is reusable or if it has to be
 * created and will return the Score-P CUPTI stream object.
 *
 * @param context Score-P CUPTI Activity context
 * @param streamId the CUDA stream ID provided by CUPTI callback API
 *
 * @return the Score-P CUPTI stream
 */
scorep_cupti_stream*
scorep_cupti_stream_get_create(
    scorep_cupti_context* context,
    uint32_t              streamId );

/*
 * Collect all Score-P locations, which are involved in CUDA communication.
 *
 * @param globalLocationIds an array of global location IDs
 *
 * @return the size of the array
 *
 */
size_t
scorep_cupti_create_cuda_comm_group( uint64_t** globalLocationIds );

void*
scorep_cupti_kernel_hash_put( const char*         name,
                              SCOREP_RegionHandle region );

void*
scorep_cupti_kernel_hash_get( const char* name );

#endif /* SCOREP_CUPTI_H */
