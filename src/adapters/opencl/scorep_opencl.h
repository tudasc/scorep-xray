/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  @brief Definitions for the Score-P OpenCL wrapper.
 */

#ifndef SCOREP_OPENCL_H
#define SCOREP_OPENCL_H

#include <stdint.h>
#include <stdbool.h>

#include <CL/cl.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include <SCOREP_Mutex.h>
#include <SCOREP_Definitions.h>

#define SCOREP_OPENCL_NO_ID          0xFFFFFFFF


/**
 * Internal OpenCL vendor platform.
 */
typedef enum
{
    SCOREP_OPENCL_VENDOR_UNKNOWN = 0, /**< unknown platform */
    SCOREP_OPENCL_VENDOR_INTEL   = 1, /**< Intel platform */
    SCOREP_OPENCL_VENDOR_NVIDIA  = 2, /**< NVIDIA platform */
    SCOREP_OPENCL_VENDOR_AMD     = 3  /**< AMD platform */
} scorep_opencl_vendor;

/**
 * Device/host communication directions
 */
typedef enum
{
    SCOREP_ENQUEUE_BUFFER_DEV2HOST              = 0x00, /**< device to host copy */
    SCOREP_ENQUEUE_BUFFER_HOST2DEV              = 0x01, /**< host to device copy */
    SCOREP_ENQUEUE_BUFFER_DEV2DEV               = 0x02, /**< device to device copy */
    SCOREP_ENQUEUE_BUFFER_HOST2HOST             = 0x04, /**< host to host copy */
    SCOREP_ENQUEUE_BUFFER_COPYDIRECTION_UNKNOWN = 0x08  /**< unknown */
} scorep_enqueue_buffer_kind;

/**
 * Structure for Score-P - OpenCL time synchronization
 */
typedef struct
{
    cl_ulong cl_time;     /**< OpenCL time stamp */
    uint64_t scorep_time; /**< Score-P time stamp */
} scorep_opencl_sync;

/**
 * Enumeration of buffer entry types
 */
typedef enum
{
    SCOREP_OPENCL_BUF_ENTRY_KERNEL, /**< OpenCL kernel */
    SCOREP_OPENCL_BUF_ENTRY_MEMCPY  /**< Memory transfer */
} scorep_opencl_buffer_entry_type;

/**
 * Score-P OpenCL buffer
 */
typedef struct
{
    scorep_opencl_buffer_entry_type type;           /**< type of buffer entry */
    cl_event                        event;          /**< OpenCL event (contains profiling info) */
    bool                            is_enqueued;    /**< Is this entry enqueued? */
    union
    {
        cl_kernel kernel;                           /**< OpenCL kernel (contains name) */
        struct
        {
            scorep_enqueue_buffer_kind kind;        /**< memory copy kind (e.g. host->device) */
            size_t                     bytes;       /**< number of bytes */
        } memcpy;
    } u;
} scorep_opencl_buffer_entry;

/**
 * Score-P OpenCL command queue
 */
typedef struct scorep_opencl_queue
{
    cl_command_queue            queue;                 /**< the OpenCL command queue */
    struct SCOREP_Location*     device_location;       /**< Score-P device location */
    uint32_t                    device_location_id;    /**< internal location ID used for unification */
    struct SCOREP_Location*     host_location;         /**< Score-P host location */
    scorep_opencl_sync          sync;                  /**< Score-P - OpenCL time synchronization */
    uint64_t                    scorep_last_timestamp; /**< last written Score-P timestamp */
    scorep_opencl_buffer_entry* buffer;                /**< OpenCL buffer pointer */
    scorep_opencl_buffer_entry* buf_pos;               /**< current buffer position */
    scorep_opencl_buffer_entry* buf_last;              /**< points to last buffer entry (scorep_opencl_buffer_entry) */
    SCOREP_Mutex                mutex;                 /**< Is queue locked? */
    scorep_opencl_vendor        vendor;                /**< vendor specification */
    struct scorep_opencl_queue* next;                  /**< Pointer to next element in the queue */
} scorep_opencl_queue;


/** number of already existing global location */
extern size_t scorep_opencl_global_location_number;
/** array of global location ids */
extern uint64_t* scorep_opencl_global_location_ids;

// handles for OpenCL communication unification
/** Interim communicator */
extern SCOREP_InterimCommunicatorHandle scorep_opencl_interim_communicator_handle;
/** RMA window */
extern SCOREP_RmaWindowHandle scorep_opencl_window_handle;

/**
 * Initialize OpenCL wrapper handling.
 *
 * We assume that this function is not executed concurrently by multiple
 * threads.
 */
void
scorep_opencl_init( void );

/**
 * Finalize OpenCL wrapper handling.
 */
void
scorep_opencl_finalize( void );

/**
 * Create a Score-P OpenCL command queue.
 *
 * @param clQueue           OpenCL command queue
 * @param clDeviceID        OpenCL device ID
 *
 * @return pointer to created Score-P OpenCL command queue
 */
scorep_opencl_queue*
scorep_opencl_queue_create( cl_command_queue clQueue,
                            cl_device_id     clDeviceID );

/**
 * Retrieves the Score-P command queue structure for a given
 * OpenCL command queue.
 *
 * @param clQueue           OpenCL command queue
 *
 * @return the Score-P command queue structure
 */
scorep_opencl_queue*
scorep_opencl_queue_get( cl_command_queue clQueue );

/**
 * Write OpenCL activities to Score-P OpenCL locations for the given queue.
 *
 * @param queue             pointer to the Score-P OpenCL command queue
 *
 * @return true on success, false on failure (mostly during synchronize due to OpenCL context already released)
 */
bool
scorep_opencl_queue_flush( scorep_opencl_queue* queue );

/**
 * Acquires memory in the Score-P queue buffer to store an activity that will be
 * enqueued in the given OpenCL command queue.
 *
 * @param queue pointer to Score-P OpenCL command queue
 *
 * @return pointer to Score-P buffer entry
 */
scorep_opencl_buffer_entry*
scorep_opencl_get_buffer_entry( scorep_opencl_queue* queue );

/**
 * Retains the OpenCL kernel and the corresponding OpenCL profiling event.
 * Set the type to kernel and mark it as enqueued.
 *
 * @param kernel the Score-P buffer entry to be used as kernel activity
 */
void
scorep_opencl_retain_kernel( scorep_opencl_buffer_entry* kernel );

/**
 * Stores properties of the data transfer, creates RMA windows if necessary and
 * retains the attached OpenCL profiling event.
 *
 * @param mcpy  the Score-P buffer entry to be used as data transfer activity
 * @param kind  kind/direction of memory copy
 * @param count number of bytes for this data transfer
 */
void
scorep_opencl_retain_buffer( scorep_opencl_queue*        queue,
                             scorep_opencl_buffer_entry* mcpy,
                             scorep_enqueue_buffer_kind  kind,
                             size_t                      count );

/**
 * Returns the OpenCL error string for the given error code
 *
 * @param error the error code
 *
 * @return the error string
 */
const char*
scorep_opencl_get_error_string( cl_int error );

/**
 * Define OpenCL locations.
 */
void
scorep_opencl_define_locations( void );

/**
 * Define OpenCL groups.
 */
void
scorep_opencl_define_group( void );

#endif /* SCOREP_OPENCL_H */
