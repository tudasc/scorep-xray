/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2016, 2020, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015, 2019-2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  Implementation of common functionality between CUPTI activity, callbacks
 *  and events.
 */

#include <config.h>
#include "scorep_cupti.h"
#include "scorep_cuda.h"     /* CUPTI common structures, functions, etc. */
#include "scorep_cupti_callbacks.h"

#include <SCOREP_AcceleratorManagement.h>
#include <scorep_system_tree.h>

#include <SCOREP_Timer_Ticks.h>

#include <UTILS_CStr.h>

#include <stdio.h>

/* String constants for CUDA attribute references */
#define SCOREP_CUPTI_CUDA_STREAMREF_KEY     "CUDA_STREAM_REF"
#define SCOREP_CUPTI_CUDA_EVENTREF_KEY      "CUDA_EVENT_REF"
#define SCOREP_CUPTI_CUDA_CURESULT_KEY      "CUDA_DRV_API_RESULT"

/* String constants for CUDA location properties */
#define SCOREP_CUPTI_LOCATION_NULL_STREAM   "CUDA_NULL_STREAM"

/* hash table for CUDA kernels */
static scorep_cuda_kernel_hash_node* scorep_cuda_kernel_hashtab[ SCOREP_CUDA_KERNEL_HASHTABLE_SIZE ];

/* mutex for locking the CUPTI environment */
UTILS_Mutex scorep_cupti_mutex = UTILS_MUTEX_INIT;

/* attribute handles for CUDA references */
scorep_cupti_attribute_handles scorep_cupti_attributes;

/* List of CUPTI devices */
static scorep_cupti_device*  cupti_device_list;
static scorep_cupti_device** cupti_device_list_tail = &cupti_device_list;

/* parameter handle for CUDA callsites */
SCOREP_ParameterHandle scorep_cupti_parameter_callsite_id = SCOREP_INVALID_PARAMETER;

/* set the list of CUPTI contexts to 'empty' */
scorep_cupti_context* scorep_cupti_context_list = NULL;

/* set the location counter to zero */
size_t scorep_cupti_location_counter = 0;

/* handle for kernel regions */
SCOREP_SourceFileHandle scorep_cupti_kernel_file_handle = SCOREP_INVALID_SOURCE_FILE;

/* handle CUDA idle regions */
SCOREP_RegionHandle scorep_cupti_idle_region_handle = SCOREP_INVALID_REGION;

static scorep_cupti_device*
cupti_device_get( CUdevice cudaDevice );
static scorep_cupti_device*
cupti_device_create( CUdevice cudaDevice );
static scorep_cupti_device*
cupti_device_get_create( CUdevice cudaDevice );

static bool scorep_cupti_initialized = 0;
static bool scorep_cupti_finalized   = 0;

void
scorep_cupti_init( void )
{
    if ( !scorep_cupti_initialized )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI] Initializing ..." );

        /* GPU idle time */
        if ( scorep_cuda_record_idle )
        {
            if ( scorep_cuda_record_idle == SCOREP_CUDA_PURE_IDLE &&
                 scorep_cuda_record_memcpy )
            {
                SCOREP_SourceFileHandle file_handle =
                    SCOREP_Definitions_NewSourceFile( "CUDA_IDLE" );
                scorep_cupti_idle_region_handle = SCOREP_Definitions_NewRegion(
                    "GPU IDLE", NULL, file_handle,
                    0, 0, SCOREP_PARADIGM_CUDA, SCOREP_REGION_ARTIFICIAL );
            }
            else if ( scorep_cuda_record_kernels )
            {
                SCOREP_SourceFileHandle file_handle =
                    SCOREP_Definitions_NewSourceFile( "CUDA_IDLE" );
                scorep_cupti_idle_region_handle = SCOREP_Definitions_NewRegion(
                    "COMPUTE IDLE", NULL, file_handle,
                    0, 0, SCOREP_PARADIGM_CUDA, SCOREP_REGION_ARTIFICIAL );
            }
            else
            {
                scorep_cuda_record_idle = SCOREP_CUDA_NO_IDLE;
            }
        }

        if ( scorep_cuda_record_references )
        {
            scorep_cupti_attributes.stream_ref = SCOREP_Definitions_NewAttribute(
                SCOREP_CUPTI_CUDA_STREAMREF_KEY,
                "Referenced CUDA stream",
                SCOREP_ATTRIBUTE_TYPE_LOCATION );
            scorep_cupti_attributes.event_ref = SCOREP_Definitions_NewAttribute(
                SCOREP_CUPTI_CUDA_EVENTREF_KEY,
                "ID (address) of referenced CUDA event",
                SCOREP_ATTRIBUTE_TYPE_UINT64 );
            scorep_cupti_attributes.result_ref = SCOREP_Definitions_NewAttribute(
                SCOREP_CUPTI_CUDA_CURESULT_KEY,
                "CUDA driver API function result",
                SCOREP_ATTRIBUTE_TYPE_UINT32 );
        }

        if ( scorep_cuda_record_callsites )
        {
            scorep_cupti_parameter_callsite_id = SCOREP_AcceleratorMgmt_GetCallsiteParameter();
        }

        scorep_cupti_initialized = true;
    }
}

/*
 * Finalize the CUPTI common interface.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_cupti_finalize( void )
{
    if ( !scorep_cupti_finalized && scorep_cupti_initialized )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA, "[CUPTI] Finalizing ..." );

        /* free Score-P CUPTI context structures */
        while ( scorep_cupti_context_list != NULL )
        {
            scorep_cupti_context* tmp =  scorep_cupti_context_list;

            scorep_cupti_context_list = scorep_cupti_context_list->next;

            scorep_cupti_context_finalize( tmp );
            tmp = NULL;
        }

        scorep_cupti_finalized = true;
    }
}

/*
 * Create a Score-P CUPTI stream.
 *
 * @param context Score-P CUPTI context
 * @param streamId ID of the CUDA stream
 *
 * @return pointer to created Score-P CUPTI stream
 */
scorep_cupti_stream*
scorep_cupti_stream_create( scorep_cupti_context* context,
                            uint32_t              streamId )
{
    scorep_cupti_stream* stream = NULL;

    if ( context == NULL )
    {
        UTILS_WARNING( "[CUPTI] Cannot create stream without Score-P CUPTI context" );
        return NULL;
    }

    /* create stream by Score-P CUPTI callbacks implementation (CUstream is given) */
    if ( streamId == SCOREP_CUPTI_NO_STREAM_ID )
    {
        UTILS_WARNING( "[CUPTI] No CUDA stream ID given!" );
        return NULL;
    }

    stream = ( scorep_cupti_stream* )SCOREP_Memory_AllocForMisc(
        sizeof( scorep_cupti_stream ) );

    stream->scorep_last_timestamp = SCOREP_GetBeginEpoch();
    stream->stream_id             = streamId;
    stream->next                  = NULL;

    /* create Score-P thread */
    {
        char thread_name[ 16 ] = "CUDA";

        if ( context->device->device_id == SCOREP_CUPTI_NO_DEVICE_ID )
        {
            if ( -1 == snprintf( thread_name + 4, 12, "[?:%d]", streamId ) )
            {
                UTILS_WARNING( "[CUPTI] Could not create thread name for CUDA thread!" );
            }
        }
        else
        {
            if ( -1 == snprintf( thread_name + 4, 12, "[%d:%d]", context->device->device_id, streamId ) )
            {
                UTILS_WARNING( "[CUPTI] Could not create thread name for CUDA thread!" );
            }
        }

        stream->scorep_location =
            SCOREP_Location_CreateNonCPULocation( context->host_location,
                                                  SCOREP_LOCATION_TYPE_GPU,
                                                  SCOREP_PARADIGM_CUDA,
                                                  thread_name,
                                                  context->location_group );

        if ( context->activity && ( stream->stream_id == context->activity->default_strm_id ) )
        {
            /* add a location property marking CUDA NULL streams */
            SCOREP_Location_AddLocationProperty( stream->scorep_location,
                                                 SCOREP_CUPTI_LOCATION_NULL_STREAM,
                                                 0, "yes" );
        }

        stream->location_id = SCOREP_CUPTI_NO_ID;
    }

    /* for the first stream created for this context */
    if ( context->streams == NULL )
    {
        if ( scorep_cuda_record_idle )
        {
            /* write enter event for GPU_IDLE on first stream */
            SCOREP_Location_EnterRegion( stream->scorep_location,
                                         stream->scorep_last_timestamp,
                                         scorep_cupti_idle_region_handle );

            if ( context->activity != NULL )
            {
                context->activity->gpu_idle = 1;
            }
        }
    }

    return stream;
}

/*
 * Retrieve a Score-P CUPTI stream object. This function will lookup if
 * the stream is already available or if it has to be
 * created and will return the Score-P CUPTI stream object.
 *
 * Note: This function must be locked with a Score-P CUPTI lock
 *
 * @param context Score-P CUPTI Activity context
 * @param streamId the CUDA stream ID provided by CUPTI callback API
 *
 * @return the Score-P CUPTI stream
 */
scorep_cupti_stream*
scorep_cupti_stream_get_create( scorep_cupti_context* context, uint32_t streamId )
{
    scorep_cupti_stream* stream      = NULL;
    scorep_cupti_stream* last_stream = NULL;

    if ( context == NULL )
    {
        UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                     "[CUPTI] Stream creation: No context given!" );
        return NULL;
    }

    if ( streamId == SCOREP_CUPTI_NO_STREAM_ID )
    {
        UTILS_ERROR( SCOREP_ERROR_UNKNOWN_TYPE,
                     "[CUPTI] No stream information given!" );
        return NULL;
    }

    /*** lookup stream ***/
    stream      = context->streams;
    last_stream = context->streams;
    while ( stream != NULL )
    {
        /* check for existing stream */
        if ( streamId != SCOREP_CUPTI_NO_STREAM_ID && stream->stream_id == streamId )
        {
            return stream;
        }

        /* remember last stream to append new created stream later */
        last_stream = stream;

        /* check next stream */
        stream = stream->next;
    }

    /*
     * If stream list is empty, the stream to be created is not the default
     * stream and GPU idle and memory copy tracing is enabled, then create
     * a default stream.
     * This is necessary to preserve increasing event time stamps!
     */
    if ( context->streams == NULL && context->activity != NULL &&
         streamId != context->activity->default_strm_id &&
         scorep_cuda_record_idle && scorep_cuda_record_memcpy )
    {
        context->streams = scorep_cupti_stream_create( context,
                                                       context->activity->default_strm_id );
        last_stream = context->streams;
    }

    /* create the stream, which has not been created yet */
    stream = scorep_cupti_stream_create( context, streamId );

    /* append the newly created stream */
    if ( NULL != last_stream )
    {
        last_stream->next = stream;
    }
    else
    {
        context->streams = stream;
    }

    return stream;
}

/*
 * Get a Score-P CUPTI stream by CUDA stream without locking.
 *
 * @param context pointer to the Score-P CUPTI context, containing the stream
 * @param streamId the CUPTI stream ID
 *
 * @return Score-P CUPTI stream
 */
scorep_cupti_stream*
scorep_cupti_stream_get_by_id( scorep_cupti_context* context,
                               uint32_t              streamId )
{
    scorep_cupti_stream* stream = NULL;

    stream = context->streams;
    while ( stream != NULL )
    {
        if ( stream->stream_id == streamId )
        {
            return stream;
        }
        stream = stream->next;
    }

    return NULL;
}

scorep_cupti_stream*
scorep_cupti_stream_get( CUstream cudaStream )
{
    CUcontext cuda_context;
    SCOREP_CUDA_DRIVER_CALL( cuStreamGetCtx( cudaStream, &cuda_context ) );

    scorep_cupti_context* scorep_context = scorep_cupti_context_get( cuda_context );

    uint32_t stream_id;
    SCOREP_CUPTI_CALL( cuptiGetStreamId( cuda_context, cudaStream, &stream_id ) );

    SCOREP_CUPTI_LOCK();
    scorep_cupti_stream* scorep_stream = scorep_cupti_stream_get_by_id( scorep_context, stream_id );
    SCOREP_CUPTI_UNLOCK();

    return scorep_stream;
}

void
scorep_cupti_stream_set_name( scorep_cupti_stream* stream,
                              const char*          name )
{
    SCOREP_CUPTI_LOCK();
    SCOREP_Location_SetName( stream->scorep_location, name );
    SCOREP_CUPTI_UNLOCK();
}

/*
 * Create a Score-P CUPTI context. If the CUDA context is not given, the
 * current context will be requested and used.
 *
 * @param cudaContext CUDA context
 *
 * @return pointer to created Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_create( CUcontext cudaContext )
{
    SCOREP_SUSPEND_CUDRV_CALLBACKS();

    /* get the current CUDA context, if it is not given */
    if ( cudaContext == NULL )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                            "[CUPTI] Creating context for current context" );
        SCOREP_CUDA_DRIVER_CALL( cuCtxGetCurrent( &cudaContext ) );
    }

    CUcontext current_context;
    SCOREP_CUDA_DRIVER_CALL( cuCtxGetCurrent( &current_context ) );

    /* if given context does not match the current one, get the device for
       the given one */
    if ( cudaContext != current_context )
    {
        SCOREP_CUDA_DRIVER_CALL( cuCtxSetCurrent( cudaContext ) );
    }

    CUdevice cuda_device;
    CUresult result = cuCtxGetDevice( &cuda_device );

    /* reset the active context */
    if ( cudaContext != current_context )
    {
        SCOREP_CUDA_DRIVER_CALL( cuCtxSetCurrent( current_context ) );
    }

    if ( CUDA_SUCCESS != result )
    {
        /* This might be a fatal error */
        UTILS_WARNING( "[CUPTI] Could not get CUDA device for context" );
        SCOREP_RESUME_CUDRV_CALLBACKS();
        return NULL;
    }

    /* create new context */
    scorep_cupti_context* context = SCOREP_Memory_AllocForMisc( sizeof( *context ) );
    memset( context, 0, sizeof( *context ) );

    SCOREP_CUPTI_CALL( cuptiGetContextId( cudaContext, &context->context_id ) );

    context->device = cupti_device_get_create( cuda_device );

    context->streams = NULL;
    context->next    = NULL;

    context->host_location = SCOREP_Location_GetCurrentCPULocation();
    context->location_id   = SCOREP_CUPTI_NO_ID;

    /* set the CUDA context */
    context->cuda_context = cudaContext;

    context->activity = NULL;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                        "[CUPTI] Created context for CUcontext %p, CUdevice %d",
                        cudaContext, context->device->cuda_device );

    SCOREP_RESUME_CUDRV_CALLBACKS();

    char context_name_buffer[ 64 ];
    snprintf( context_name_buffer, sizeof( context_name_buffer ),
              "CUDA Context %u", context->context_id );
    context->location_group = SCOREP_AcceleratorMgmt_CreateContext(
        context->device->system_tree_node,
        context_name_buffer );

    if ( scorep_cuda_record_gpumemusage )
    {
        snprintf( context_name_buffer, sizeof( context_name_buffer ),
                  "CUDA Context %u Memory", context->context_id );
        SCOREP_AllocMetric_NewScoped( context_name_buffer,
                                      context->location_group,
                                      &context->alloc_metric );
    }

    return context;
}

/*
 * Get a Score-P CUPTI context by CUDA context
 *
 * @param cudaContext the CUDA context
 *
 * @return Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_get( CUcontext cudaContext )
{
    scorep_cupti_context* context = NULL;

    uint32_t cuda_context_id = SCOREP_CUPTI_NO_CONTEXT_ID;
    SCOREP_CUPTI_CALL( cuptiGetContextId( cudaContext, &cuda_context_id ) );

    /* lookup context */
    context = scorep_cupti_context_list;
    while ( context != NULL )
    {
        if ( context->cuda_context == cudaContext )
        {
            if ( context->context_id == cuda_context_id )
            {
                return context;
            }
        }

        context = context->next;
    }

    return NULL;
}

/*
 * Get a Score-P CUPTI context by CUDA context ID
 *
 * @param contextId the CUDA context ID
 *
 * @return Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_get_by_id( uint32_t contextId )
{
    scorep_cupti_context* context = NULL;

    /* lookup context */
    context = scorep_cupti_context_list;
    while ( context != NULL )
    {
        if ( context->context_id == contextId )
        {
            return context;
        }

        context = context->next;
    }

    return NULL;
}


/*
 * Get or if not available create a Score-P CUPTI context by CUDA context.
 *
 * @param cudaContext the CUDA context
 *
 * @return Score-P CUPTI context
 */
scorep_cupti_context*
scorep_cupti_context_get_create( CUcontext cudaContext )
{
    /* try to find CUPTI context without locking */
    scorep_cupti_context* context = scorep_cupti_context_get( cudaContext );

    if ( context == NULL )
    {
        /* securely insert context into global context list */
        SCOREP_CUPTI_LOCK();
        context = scorep_cupti_context_get( cudaContext );

        if ( context == NULL )
        {
            context = scorep_cupti_context_create( cudaContext );

            /* prepend context to global context list */
            context->next             = scorep_cupti_context_list;
            scorep_cupti_context_list = context;
        }
        SCOREP_CUPTI_UNLOCK();
    }

    return context;
}

/*
 * Remove a context from the global context list and return it.
 *
 * @param cudaContext pointer to the CUDA context
 * @return the Score-P CUPTI context, which has been removed
 */
scorep_cupti_context*
scorep_cupti_context_remove( CUcontext cudaContext )
{
    scorep_cupti_context* currCtx = NULL;
    scorep_cupti_context* lastCtx = NULL;

    SCOREP_CUPTI_LOCK();
    currCtx = scorep_cupti_context_list;
    lastCtx = scorep_cupti_context_list;
    while ( currCtx != NULL )
    {
        if ( currCtx->cuda_context == cudaContext )
        {
            /* if first element in list */
            if ( currCtx == scorep_cupti_context_list )
            {
                scorep_cupti_context_list = scorep_cupti_context_list->next;
            }
            else
            {
                lastCtx->next = currCtx->next;
            }
            SCOREP_CUPTI_UNLOCK();
            return currCtx;
        }
        lastCtx = currCtx;
        currCtx = currCtx->next;
    }
    SCOREP_CUPTI_UNLOCK();

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_CUDA,
                        "[CUPTI] Could not remove context (CUDA Context not found)!" );

    return NULL;
}

void
scorep_cupti_context_set_name( scorep_cupti_context* context,
                               const char*           name )
{
    SCOREP_CUPTI_LOCK();
    SCOREP_LocationGroupHandle_SetName( context->location_group, name );
    SCOREP_CUPTI_UNLOCK();
}

/*
 * Finalize the Score-P CUPTI context and free all memory allocated with it.
 *
 * @param scorepCtx pointer to the Score-P CUPTI context
 */
void
scorep_cupti_context_finalize( scorep_cupti_context* context )
{
    if ( context == NULL )
    {
        return;
    }

    /* write exit event for GPU idle time */
    if ( scorep_cuda_record_idle && context->streams != NULL
         && context->activity != NULL && context->activity->gpu_idle == 1 )
    {
        uint64_t idle_end = SCOREP_Timer_GetClockTicks();

        SCOREP_Location_ExitRegion( context->streams->scorep_location, idle_end,
                                    scorep_cupti_idle_region_handle );
    }

    context->streams = NULL;

    if ( scorep_cuda_record_gpumemusage )
    {
        SCOREP_AllocMetric_ReportLeaked( context->alloc_metric );
    }

    if ( context->activity != NULL )
    {
        context->activity = NULL;
    }

    context = NULL;
}


size_t
scorep_cupti_create_cuda_comm_group( uint64_t** globalLocationIds )
{
    size_t                count   = 0;
    scorep_cupti_context* context = NULL;

    /* get the number of CUDA communication partners */
    context = scorep_cupti_context_list;
    while ( context != NULL )
    {
        scorep_cupti_stream* stream = context->streams;

        while ( NULL != stream )
        {
            if ( SCOREP_CUPTI_NO_ID != stream->location_id )
            {
                count++;
            }

            stream = stream->next;
        }

        /* get an array element for the context location */
        if ( scorep_cuda_record_memcpy )
        {
            count++;
        }

        context = context->next;
    }

    if ( count == 0 )
    {
        return count;
    }

    /* allocate the CUDA communication group array */
    *globalLocationIds = ( uint64_t* )SCOREP_Memory_AllocForMisc( count * sizeof( uint64_t ) );

    /* add the communication partners allocated array */
    context = scorep_cupti_context_list;
    while ( context != NULL )
    {
        scorep_cupti_stream* stream = context->streams;

        while ( NULL != stream )
        {
            if ( SCOREP_CUPTI_NO_ID != stream->location_id )
            {
                ( *globalLocationIds )[ stream->location_id ] =
                    SCOREP_Location_GetGlobalId( stream->scorep_location );
            }

            stream = stream->next;
        }

        /* add the context location */
        if ( SCOREP_CUPTI_NO_ID != context->location_id )
        {
            ( *globalLocationIds )[ context->location_id ] =
                SCOREP_Location_GetGlobalId( context->host_location );
        }

        context = context->next;
    }

    return count;
}


/** @brief
 * SDBM hash function. (better than DJB2 for table size 2^10)
 *
 * @param str               Pointer to a string to be hashed
 *
 * @return Returns hash code of @ str.
 */
static unsigned int
hash_string( const char* str )
{
    unsigned int hash = 0;
    int          c;

    while ( ( c = *str++ ) )
    {
        hash = c + ( hash << 6 ) + ( hash << 16 ) - hash;
    }

    return hash;
}

/** @brief
 * Puts a string into the CUPTI activity hash table
 *
 * @param name              Pointer to a string to be stored in the hash table.
 * @param region            Region handle.
 *
 * @return Return pointer to the created hash node.
 */
void*
scorep_cupti_kernel_hash_put( const char*         name,
                              SCOREP_RegionHandle region )
{
    uint32_t id = ( uint32_t )hash_string( name ) % SCOREP_CUDA_KERNEL_HASHTABLE_SIZE;

    scorep_cuda_kernel_hash_node* add =
        ( scorep_cuda_kernel_hash_node* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cuda_kernel_hash_node ) );

    add->name                        = UTILS_CStr_dup( name );           /* does an implicit malloc */
    add->region                      = region;
    add->next                        = scorep_cuda_kernel_hashtab[ id ];
    scorep_cuda_kernel_hashtab[ id ] = add;

    return add;
}

/** @brief
 * Get a string from the CUPTI Activity hash table
 *
 * @param name              Pointer to a string to be retrieved from the hash table.
 *
 * @return Return pointer to the retrieved hash node.
 */
void*
scorep_cupti_kernel_hash_get( const char* name )
{
    uint32_t id = ( uint32_t )hash_string( name )  % SCOREP_CUDA_KERNEL_HASHTABLE_SIZE;

    scorep_cuda_kernel_hash_node* curr = scorep_cuda_kernel_hashtab[ id ];

    while ( curr )
    {
        if ( strcmp( curr->name, name ) == 0 )
        {
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}

/*
 * Get a Score-P device by CUDA device
 *
 * @param cudaDevice the CUDA context
 *
 * @return Score-P CUPTI context
 */
scorep_cupti_device*
cupti_device_get( CUdevice cudaDevice )
{
    scorep_cupti_device* device = NULL;

    /* lookup device */
    device = cupti_device_list;
    while ( device != NULL )
    {
        if ( device->cuda_device == cudaDevice )
        {
            return device;
        }

        device = device->next;
    }

    return NULL;
}

/*
 * Create a Score-P device by CUDA device
 *
 * @param cudaDevice the CUDA device
 *
 * @return Score-P CUPTI device
 */
scorep_cupti_device*
cupti_device_create( CUdevice cudaDevice )
{
    scorep_cupti_device* device = SCOREP_Memory_AllocForMisc( sizeof( *device ) );

    device->cuda_device = cudaDevice;
    device->device_id   = ( uint32_t )cudaDevice;

    /* CUDA_VISIBLE_DEVICES may be used to change the order and thus
     * the device ID visible from the outside, use it to map the device ID
     */
    if ( device->device_id < scorep_cuda_visible_devices_len )
    {
        device->device_id = scorep_cuda_visible_devices_map[ device->device_id ];
    }

    SCOREP_CUDA_DRIVER_CALL( cudaDeviceGetAttribute( &device->pci_domain_id, cudaDevAttrPciDomainId, cudaDevice ) );
    UTILS_BUG_ON( device->pci_domain_id < 0 || device->pci_domain_id >= UINT16_MAX, "Invalid PCI domain ID: %d", device->pci_domain_id );
    SCOREP_CUDA_DRIVER_CALL( cudaDeviceGetAttribute( &device->pci_bus_id, cudaDevAttrPciBusId, cudaDevice ) );
    UTILS_BUG_ON( device->pci_bus_id < 0 || device->pci_bus_id >= UINT8_MAX, "Invalid PCI bus ID: %d", device->pci_bus_id );
    SCOREP_CUDA_DRIVER_CALL( cudaDeviceGetAttribute( &device->pci_device_id, cudaDevAttrPciDeviceId, cudaDevice ) );
    UTILS_BUG_ON( device->pci_device_id < 0 || device->pci_device_id >= UINT8_MAX, "Invalid PCI device ID: %d", device->pci_device_id );

    char device_name_buffer[ 32 ];
    sprintf( device_name_buffer, "%u", device->device_id );
    device->system_tree_node = SCOREP_Definitions_NewSystemTreeNode(
        SCOREP_GetSystemTreeNodeHandleForSharedMemory(),
        SCOREP_SYSTEM_TREE_DOMAIN_ACCELERATOR_DEVICE,
        "CUDA Device",
        device_name_buffer );

    SCOREP_SystemTreeNode_AddPCIProperties( device->system_tree_node,
                                            ( uint16_t )device->pci_domain_id,
                                            ( uint8_t )device->pci_bus_id,
                                            ( uint8_t )device->pci_device_id,
                                            UINT8_MAX );

    device->next            = NULL;
    *cupti_device_list_tail = device;
    cupti_device_list_tail  = &device->next;

    return device;
}

scorep_cupti_device*
cupti_device_get_create( CUdevice cudaDevice )
{
    scorep_cupti_device* device = cupti_device_get( cudaDevice );
    if ( device != NULL )
    {
        return device;
    }

    return cupti_device_create( cudaDevice );
}
