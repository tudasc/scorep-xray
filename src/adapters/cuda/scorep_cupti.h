/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 *  @file
 *
 *  This file provides commonly used definitions and functionality for all CUPTI
 *  interface (events, callbacks and activity)
 */

#ifndef SCOREP_CUPTI_H
#define SCOREP_CUPTI_H

#include <stdint.h>
#include <stdbool.h>

#include "SCOREP_Mutex.h"
#include "SCOREP_Definitions.h"
#include "SCOREP_Location.h"
#include "SCOREP_Memory.h"
#include "SCOREP_Events.h"

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Timing.h>

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
            cuptiGetResultString( _status, &msg );                      \
            UTILS_WARNING( "[CUPTI] Call to '%s' failed with message: '%s'",  #fct, msg ); \
        }                                                               \
    }

/*#if defined( HAVE_DEMANGLE )
   #include <demangle.h>
   #define SCOREP_DEMANGLE_CUDA_KERNEL( mangled ) \
    cplus_demangle( mangled, 0 )
 #else*/
#define SCOREP_DEMANGLE_CUDA_KERNEL( mangled ) \
    ( char* )mangled
/*#endif*/

/* the default size for the CUDA kernel name hash table */
#define SCOREP_CUDA_KERNEL_HASHTABLE_SIZE 1024

/* thread (un)locking macros for CUPTI interfaces */
extern SCOREP_Mutex scorep_cupti_mutex;
# define SCOREP_CUPTI_LOCK() SCOREP_MutexLock( scorep_cupti_mutex )
# define SCOREP_CUPTI_UNLOCK() SCOREP_MutexUnlock( scorep_cupti_mutex )

// TODO: define these macros during configure
#define SCOREP_CUPTI_ACTIVITY
#define SCOREP_CUPTI_CALLBACKS

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
typedef struct scorep_cupti_stream_t
{
    CUstream                      cuda_stream;           /**< the CUDA stream */
    uint32_t                      stream_id;             /**< the CUDA stream ID */
    SCOREP_Location*              scorep_location;       /**< Score-P location for this stream (unique) */
    uint32_t                      location_id;           /**< internal location ID used for unification */
    uint64_t                      scorep_last_timestamp; /**< last written Score-P timestamp */
    bool                          destroyed;             /**< Is stream destroyed? Ready for reuse? */
    struct scorep_cupti_stream_t* next;
}scorep_cupti_stream_t;

#if ( defined( SCOREP_CUPTI_CALLBACKS ) || defined( SCOREP_CUPTI_ACTIVITY ) )
/*
 * data structure contains information about allocated CUDA memory
 */
typedef struct scorep_cupti_gpumem_t
{
    uint64_t                      address;  /**< pointer value to allocated memory */
    size_t                        size;     /**< number of bytes allocated */
    uint32_t                      tid;      /**< thread id used with this malloc */
    struct scorep_cupti_gpumem_t* next;
}scorep_cupti_gpumem_t;
#endif /* SCOREP_CUPTI_CALLBACKS || SCOREP_CUPTI_ACTIVITY */

#if defined( SCOREP_CUPTI_CALLBACKS )
/*
 * structure of a Score-P CUPTI CUDA runtime kernel
 */
typedef struct scorep_cupti_kernel_t
{
    CUstream                      cuda_stream;        /**< the CUDA stream */
    uint32_t                      blocks_per_grid;    /**< number of blocks per grid */
    uint32_t                      threads_per_block;  /**< number of threads per block */
    struct scorep_cupti_kernel_t* up;
    struct scorep_cupti_kernel_t* down;
}scorep_cupti_kernel_t;


/*
 * Score-P CUPTI callbacks specific context data.
 */
typedef struct
{
    scorep_cupti_kernel_t* kernel_data;     /**< pointer to top of CUDA runtime kernel
                                               configuration stack */
    uint8_t                streams_created; /**< #streams created for this device */
# if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
    bool                   concurrent_kernels;
# endif
# if !defined( SCOREP_CUPTI_ACTIVITY )
    uint8_t callbacks_enabled;      /**< execute callback function? */
# endif
}scorep_cupti_callbacks_t;
#endif /* SCOREP_CUPTI_CALLBACKS */

#if defined( SCOREP_CUPTI_ACTIVITY )
/*
 * Score-P CUPTI activity synchronization structure
 */
typedef struct
{
    uint64_t host_start; /**< host measurement interval start timestamp */
    uint64_t host_stop;  /**< host measurement interval stop timestamp */
    uint64_t gpu_start;  /**< gpu measurement interval start timestamp */
    double   factor;     /**< synchronization factor for time interval */
}scorep_cupti_sync_t;

/*
 * Score-P CUPTI activity specific context data.
 */
typedef struct
{
    uint32_t            default_strm_id;      /**< CUPTI stream ID of default stream */
    scorep_cupti_sync_t sync;                 /**< store synchronization information */
    uint8_t*            buffer;               /**< CUPTI activity buffer pointer */
    uint64_t            scorep_last_gpu_time; /**< last written Score-P timestamp */
    bool                gpu_idle;             /**< has idle region enter been written last */
}scorep_cupti_activity_t;
#endif /* SCOREP_CUPTI_ACTIVITY */

/*
 * Score-P CUPTI context.
 */
typedef struct scorep_cupti_context_t
{
    CUcontext              cuda_context;         /**< CUDA context handle */
    uint32_t               context_id;           /**< context ID */
    uint32_t               device_id;            /**< device ID */
    CUdevice               cuda_device;          /**< CUDA device handle */
    SCOREP_Location*       scorep_host_location; /**< Score-P context host location */
    uint32_t               location_id;          /**< internal location ID used for unification */
    uint8_t                destroyed;
#if ( defined( SCOREP_CUPTI_ACTIVITY ) || defined( SCOREP_CUPTI_CALLBACKS ) )
    scorep_cupti_stream_t* streams;              /**< list of Score-P CUDA streams */
    scorep_cupti_gpumem_t* cuda_mallocs;         /**< list of allocated GPU memory fields */
    size_t                 gpu_memory_allocated; /**< memory allocated on CUDA device */
#endif
#if defined( SCOREP_CUPTI_ACTIVITY )
    scorep_cupti_activity_t* activity;
#endif
#if defined( SCOREP_CUPTI_CALLBACKS )
    scorep_cupti_callbacks_t*      callbacks;
#endif
    struct scorep_cupti_context_t* next;
}scorep_cupti_context_t;

extern scorep_cupti_context_t* scorep_cupti_context_list;

extern size_t scorep_cupti_location_counter;

/* handle for kernel regions */
extern SCOREP_SourceFileHandle scorep_cupti_kernel_file_handle;

/* handle CUDA idle regions */
extern SCOREP_RegionHandle scorep_cupti_idle_region_handle;

extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_threads_per_kernel;
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_threads_per_block;
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_blocks_per_grid;

#if defined( SCOREP_CUPTI_ACTIVITY )
/* CUPTI activity specific kernel counter IDs */
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_static_shared_mem;
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_dynamic_shared_mem;
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_local_mem_total;
extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_registers_per_thread;
#endif

extern SCOREP_SamplingSetHandle scorep_cupti_sampling_set_gpumemusage;

extern void
scorep_cupti_init( void );

extern void
scorep_cupti_finalize( void );

/*
 * Handles errors returned from CUPTI function calls.
 *
 * @param ecode the CUDA driver API error code
 * @param msg a message to get more detailed information about the error
 * @param the corresponding file
 * @param the line the error occurred
 */
extern void
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
extern scorep_cupti_context_t*
scorep_cupti_context_create( CUcontext cudaContext,
                             CUdevice  cudaDevice,
                             uint32_t  cudaContextId,
                             uint32_t  cudaDeviceId );

/*
 * Prepend the given Score-P CUPTI context to the global context list.
 *
 * @param context pointer to the Score-P CUPTI context to be prepended
 */
extern void
scorep_cupti_context_prepend( scorep_cupti_context_t* context );

/*
 * Get a Score-P CUPTI context by CUDA context.
 *
 * @param cudaContext the CUDA context
 *
 * @return Score-P CUPTI context
 */
extern scorep_cupti_context_t*
scorep_cupti_context_get( CUcontext cudaContext );

/*
 * Get a Score-P CUPTI context by CUDA context without locking.
 *
 * @param cudaContext the CUDA context
 *
 * @return Score-P CUPTI context
 */
extern scorep_cupti_context_t*
scorep_cupti_context_get_nolock( CUcontext cudaContext );

/*
 * Get or if not available create a Score-P CUPTI context by CUDA context.
 *
 * @param cudaContext the CUDA context
 *
 * @return Score-P CUPTI context
 */
extern scorep_cupti_context_t*
scorep_cupti_context_get_create( CUcontext cudaContext );

/*
 * Remove a context from the global context list and return it.
 *
 * @param cudaContext the CUDA context
 *
 * @return the Score-P CUPTI context, which has been removed
 */
extern scorep_cupti_context_t*
scorep_cupti_context_remove( CUcontext cudaContext );

/*
 * Mark the Score-P CUPTI context as destroyed, so that it can be reused later.
 *
 * @param context pointer to the Score-P CUPTI context
 */
extern void
scorep_cupti_context_set_destroyed( scorep_cupti_context_t* context );

/*
 * Finalize the Score-P CUPTI context and free all memory allocated with it.
 *
 * @param context pointer to the Score-P CUPTI context
 */
extern void
scorep_cupti_context_finalize( scorep_cupti_context_t* context );

#if ( defined( SCOREP_CUPTI_ACTIVITY ) || defined( SCOREP_CUPTI_CALLBACKS ) )
/*
 * Create a Score-P CUPTI stream.
 *
 * @param context Score-P CUPTI context
 * @param cudaStream CUDA stream
 * @param streamId ID of the CUDA stream
 *
 * @return pointer to created Score-P CUPTI stream
 */
extern scorep_cupti_stream_t*
scorep_cupti_stream_create(
    scorep_cupti_context_t* context,
    CUstream                cudaStream,
    uint32_t                streamId );

/*
 * Get a Score-P CUPTI stream by CUDA stream without locking.
 *
 * @param context pointer to the Score-P CUPTI context, containing the stream
 * @param streamId the CUPTI stream ID
 *
 * @return Score-P CUPTI stream
 */
extern scorep_cupti_stream_t*
scorep_cupti_stream_get_by_id(
    scorep_cupti_context_t* context,
    uint32_t                streamId );

/*
 * Retrieve a Score-P CUPTI stream object. This function will lookup, if
 * the stream is already available, a stream is reusable or if it has to be
 * created and will return the Score-P CUPTI stream object.
 *
 * @param context Score-P CUPTI Activity context
 * @param cudaStream CUDA stream
 * @param streamId the CUDA stream ID provided by CUPTI callback API
 *
 * @return the Score-P CUPTI stream
 */
extern scorep_cupti_stream_t*
scorep_cupti_stream_get_create(
    scorep_cupti_context_t* context,
    CUstream                cudaStream,
    uint32_t                streamId );

/*
 * Mark a CUDA stream as destroyed, so that it can be reused afterwards.
 *
 * @param cuda_context CUDA context, which contains the stream
 * @param streamId the CUDA stream ID to be marked as destroyed
 *
 */
extern void
scorep_cupti_stream_set_destroyed( CUcontext cudaContext,
                                   uint32_t  streamId );


/*
 * Collect all Score-P locations, which are involved in CUDA communication.
 *
 * @param globalLocationIds an array of global location IDs
 *
 * @return the size of the array
 *
 */
extern size_t
scorep_cupti_create_cuda_comm_group( uint64_t** globalLocationIds );

#endif /* SCOREP_CUPTI_ACTIVITY || SCOREP_CUPTI_CALLBACKS */

extern void*
scorep_cupti_kernel_hash_put( const char*         name,
                              SCOREP_RegionHandle region );

extern void*
scorep_cupti_kernel_hash_get( const char* name );

#endif  /* SCOREP_CUPTI_H */
