/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @brief Implementation of the CUDA NVTX API for Score-P
 *
 *  @file
 */

#include <config.h>

#include "scorep_cuda_nvtx_mgmt.h"

#include <SCOREP_InMeasurement.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Location.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_Filtering.h>
#include <SCOREP_Task.h>

#include <nvToolsExt.h>

/*************** Macros *******************************************************/

#define SCOREP_CUDA_NVTX_MAKE_ASCII_ATTRIBUTES( message ) \
    { \
        .version       = NVTX_VERSION, \
        .size          = NVTX_EVENT_ATTRIB_STRUCT_SIZE, \
        .messageType   = NVTX_MESSAGE_TYPE_ASCII, \
        .message.ascii = message \
    }

#define SCOREP_CUDA_NVTX_MAKE_UNICODE_ATTRIBUTES( message ) \
    { \
        .version         = NVTX_VERSION, \
        .size            = NVTX_EVENT_ATTRIB_STRUCT_SIZE, \
        .messageType     = NVTX_MESSAGE_TYPE_UNICODE, \
        .message.unicode = message \
    }

/*************** Init functions ***********************************************/

NVTX_DECLSPEC int NVTX_API
nvtxInitialize( const nvtxInitializationAttributes_t* initAttrib )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return 0;
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return 0;
}

/*************** Mark functions ***********************************************/

NVTX_DECLSPEC void NVTX_API
nvtxDomainMarkEx( nvtxDomainHandle_t           domain,
                  const nvtxEventAttributes_t* eventAttrib )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    const char* name = scorep_cuda_nvtx_get_name_from_attributes( eventAttrib );
    if ( SCOREP_Filtering_MatchFunction( name, NULL ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_RegionHandle region_handle = scorep_cuda_nvtx_get_user_region( domain, eventAttrib );

    scorep_cuda_nvtx_apply_payload( eventAttrib );
    SCOREP_EnterRegion( region_handle );
    scorep_cuda_nvtx_apply_category( domain, eventAttrib );
    SCOREP_ExitRegion( region_handle );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxMarkEx( const nvtxEventAttributes_t* eventAttrib )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    nvtxDomainMarkEx( SCOREP_CUDA_NVTX_DEFAULT_DOMAIN, eventAttrib );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxMarkA( const char* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    nvtxEventAttributes_t attributes = SCOREP_CUDA_NVTX_MAKE_ASCII_ATTRIBUTES( message );
    nvtxMarkEx( &attributes );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxMarkW( const wchar_t* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    nvtxEventAttributes_t attributes = SCOREP_CUDA_NVTX_MAKE_UNICODE_ATTRIBUTES( message );
    nvtxMarkEx( &attributes );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*************** Range start/stop functions ***********************************/

NVTX_DECLSPEC nvtxRangeId_t NVTX_API
nvtxDomainRangeStartEx( nvtxDomainHandle_t           domain,
                        const nvtxEventAttributes_t* eventAttrib )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return 0;
    }

    // Look up metric from domain, attributes
    // Increment metric value
    // Return metric ID

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return 0;
}

NVTX_DECLSPEC nvtxRangeId_t NVTX_API
nvtxRangeStartEx( const nvtxEventAttributes_t* eventAttrib )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    nvtxRangeId_t result = nvtxDomainRangeStartEx( SCOREP_CUDA_NVTX_DEFAULT_DOMAIN, eventAttrib );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

NVTX_DECLSPEC nvtxRangeId_t NVTX_API
nvtxRangeStartA( const char* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    nvtxEventAttributes_t attributes = SCOREP_CUDA_NVTX_MAKE_ASCII_ATTRIBUTES( message );
    nvtxRangeId_t         result     = nvtxRangeStartEx( &attributes );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

NVTX_DECLSPEC nvtxRangeId_t NVTX_API
nvtxRangeStartW( const wchar_t* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    nvtxEventAttributes_t attributes = SCOREP_CUDA_NVTX_MAKE_UNICODE_ATTRIBUTES( message );
    nvtxRangeId_t         result     = nvtxRangeStartEx( &attributes );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

NVTX_DECLSPEC void NVTX_API
nvtxDomainRangeEnd( nvtxDomainHandle_t domain,
                    nvtxRangeId_t      id )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    // Decrement metric based on domain/ID

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxRangeEnd( nvtxRangeId_t id )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    nvtxDomainRangeEnd( SCOREP_CUDA_NVTX_DEFAULT_DOMAIN, id );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*************** Range push/pop functions *************************************/

NVTX_DECLSPEC int NVTX_API
nvtxDomainRangePushEx( nvtxDomainHandle_t           domain,
                       const nvtxEventAttributes_t* eventAttrib )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return 0;
    }

    const char* name = scorep_cuda_nvtx_get_name_from_attributes( eventAttrib );
    if ( SCOREP_Filtering_MatchFunction( name, NULL ) )
    {
        SCOREP_Task_Enter( SCOREP_Location_GetCurrentCPULocation(), SCOREP_FILTERED_REGION );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return 0;
    }

    SCOREP_RegionHandle region_handle = scorep_cuda_nvtx_get_user_region( domain, eventAttrib );

    scorep_cuda_nvtx_apply_payload( eventAttrib );
    SCOREP_EnterRegion( region_handle );
    scorep_cuda_nvtx_apply_category( domain, eventAttrib );

    SCOREP_IN_MEASUREMENT_DECREMENT();
    // Return 0 on success, negative on error (compatible with SCOREP_Error values?)
    return 0;
}

NVTX_DECLSPEC int NVTX_API
nvtxRangePushEx( const nvtxEventAttributes_t* eventAttrib )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int result = nvtxDomainRangePushEx( SCOREP_CUDA_NVTX_DEFAULT_DOMAIN, eventAttrib );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

NVTX_DECLSPEC int NVTX_API
nvtxRangePushA( const char* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    nvtxEventAttributes_t attributes = SCOREP_CUDA_NVTX_MAKE_ASCII_ATTRIBUTES( message );
    int                   result     = nvtxRangePushEx( &attributes );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

NVTX_DECLSPEC int NVTX_API
nvtxRangePushW( const wchar_t* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    nvtxEventAttributes_t attributes = SCOREP_CUDA_NVTX_MAKE_UNICODE_ATTRIBUTES( message );
    int                   result     = nvtxRangePushEx( &attributes );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

NVTX_DECLSPEC int NVTX_API
nvtxDomainRangePop( nvtxDomainHandle_t domain )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    UTILS_ASSERT( location != NULL );

    SCOREP_RegionHandle region_handle =
        SCOREP_Task_GetTopRegion( SCOREP_Task_GetCurrentTask( location ) );
    UTILS_ASSERT( region_handle != SCOREP_INVALID_REGION );

    if ( region_handle != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( region_handle );
    }
    else
    {
        SCOREP_Task_Exit( location );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();

    // Return 0 on success, negative on error (compatible with SCOREP_Error values?)
    return 0;
}

NVTX_DECLSPEC int NVTX_API
nvtxRangePop( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int result = nvtxDomainRangePop( SCOREP_CUDA_NVTX_DEFAULT_DOMAIN );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

/*************** Domain management ********************************************/

NVTX_DECLSPEC nvtxDomainHandle_t NVTX_API
nvtxDomainCreateA( const char* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        /* scorep_cuda_nvtx_create_domain needs Score-P memory */
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return NULL;
    }

    nvtxDomainHandle_t result = scorep_cuda_nvtx_create_domain( name );

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

NVTX_DECLSPEC nvtxDomainHandle_t NVTX_API
nvtxDomainCreateW( const wchar_t* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        /* scorep_cuda_nvtx_unicode_to_ascii needs Score-P memory */
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return NULL;
    }

    nvtxDomainHandle_t result = nvtxDomainCreateA( scorep_cuda_nvtx_unicode_to_ascii( name ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

NVTX_DECLSPEC void NVTX_API
nvtxDomainDestroy( nvtxDomainHandle_t domain )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*************** Domain resource management ***********************************/

NVTX_DECLSPEC nvtxResourceHandle_t NVTX_API
nvtxDomainResourceCreate( nvtxDomainHandle_t        domain,
                          nvtxResourceAttributes_t* attribs )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return NULL;
    }

    // Not implemented

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return NULL;
}

NVTX_DECLSPEC void NVTX_API
nvtxDomainResourceDestroy( nvtxResourceHandle_t resource )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    // Not implemented
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*************** Category naming **********************************************/

NVTX_DECLSPEC void NVTX_API
nvtxDomainNameCategoryA( nvtxDomainHandle_t domain,
                         uint32_t           category,
                         const char*        name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        /* scorep_cuda_nvtx_name_category needs Score-P memory */
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    scorep_cuda_nvtx_name_category( domain, category, name );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxDomainNameCategoryW( nvtxDomainHandle_t domain,
                         uint32_t           category,
                         const wchar_t*     name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        /* scorep_cuda_nvtx_unicode_to_ascii needs Score-P memory */
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    nvtxDomainNameCategoryA( domain, category, scorep_cuda_nvtx_unicode_to_ascii( name ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxNameCategoryA( uint32_t    category,
                   const char* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    nvtxDomainNameCategoryA( SCOREP_CUDA_NVTX_DEFAULT_DOMAIN, category, name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxNameCategoryW( uint32_t       category,
                   const wchar_t* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    nvtxDomainNameCategoryW( SCOREP_CUDA_NVTX_DEFAULT_DOMAIN, category, name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*************** Thread naming ************************************************/

/* NVTX documentation:
 *
 * Annotate an OS thread.
 *
 * Allows the user to name an active thread of the current process. If an
 * invalid thread ID is provided or a thread ID from a different process is
 * used the behavior of the tool is implementation dependent.
 *
 * The thread name is associated to the default domain.  To support domains
 * use resource objects via ::nvtxDomainResourceCreate.
 *
 * \param threadId - The ID of the thread to name.
 * \param name     - The name of the thread.
 *
 * \par Example:
 * \code
 * nvtxNameOsThread(GetCurrentThreadId(), "MAIN_THREAD");
 * \endcode
 */

NVTX_DECLSPEC void NVTX_API
nvtxNameOsThreadA( uint32_t    threadId,
                   const char* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_Location_SetNameByThreadId( threadId, name );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxNameOsThreadW( uint32_t       threadId,
                   const wchar_t* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        /* scorep_cuda_nvtx_unicode_to_ascii needs Score-P memory */
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    nvtxNameOsThreadA( threadId, scorep_cuda_nvtx_unicode_to_ascii( name ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*************** Stream naming ************************************************/

/*
 * From cuda_runtime_api.h:
 * > The types ::CUstream and ::cudaStream_t are identical and may be used interchangeably.
 * They are just pointers, thus we can use `void*` to avoid additional includes
 */

NVTX_DECLSPEC void NVTX_API
nvtxNameCuStreamA( void*       stream,
                   const char* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    scorep_cuda_nvtx_set_stream_name( stream, name );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxNameCuStreamW( void*          stream,
                   const wchar_t* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        /* scorep_cuda_nvtx_unicode_to_ascii needs Score-P memory */
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    nvtxNameCuStreamA( stream, scorep_cuda_nvtx_unicode_to_ascii( name ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxNameCudaStreamA( void*       stream,
                     const char* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    scorep_cuda_nvtx_set_stream_name( stream, name );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxNameCudaStreamW( void*          stream,
                     const wchar_t* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        /* scorep_cuda_nvtx_unicode_to_ascii needs Score-P memory */
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    nvtxNameCudaStreamA( stream, scorep_cuda_nvtx_unicode_to_ascii( name ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*************** Context naming ************************************************/

/*
 * CUcontext is just a pointer, thus we can use `void*` to avoid additional includes
 */

NVTX_DECLSPEC void NVTX_API
nvtxNameCuContextA( void*       context,
                    const char* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    scorep_cuda_nvtx_set_context_name( context, name );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxNameCuContextW( void*          context,
                    const wchar_t* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        /* scorep_cuda_nvtx_unicode_to_ascii needs Score-P memory */
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    nvtxNameCuContextA( context, scorep_cuda_nvtx_unicode_to_ascii( name ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*************** String registration ******************************************/

NVTX_DECLSPEC nvtxStringHandle_t NVTX_API
nvtxDomainRegisterStringA( nvtxDomainHandle_t domain,
                           const char*        string )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        /* scorep_cuda_nvtx_create_string needs Score-P memory */
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return NULL;
    }

    nvtxStringHandle_t result = scorep_cuda_nvtx_create_string( string );

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

NVTX_DECLSPEC nvtxStringHandle_t NVTX_API
nvtxDomainRegisterStringW( nvtxDomainHandle_t domain,
                           const wchar_t*     string )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        /* scorep_cuda_nvtx_unicode_to_ascii needs Score-P memory */
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return NULL;
    }

    nvtxStringHandle_t result =
        nvtxDomainRegisterStringA( domain, scorep_cuda_nvtx_unicode_to_ascii( string ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

/* Not implemented API: nvToolsExtCuda.h
 *
 * nvtxNameCuDeviceA
 * nvtxNameCuDeviceW
 * nvtxNameCuEventA
 * nvtxNameCuEventW
 */

/* Not implemented API: nvToolsExtCudaRt.h
 *
 * nvtxNameCudaDeviceA
 * nvtxNameCudaDeviceW
 * nvtxNameCudaEventA
 * nvtxNameCudaEventW
 */

/* Not implemented API: nvToolsExtOpenCL.h
 *
 * nvtxNameClCommandQueueA
 * nvtxNameClCommandQueueW
 * nvtxNameClContextA
 * nvtxNameClContextW
 * nvtxNameClDeviceA
 * nvtxNameClDeviceW
 * nvtxNameClEventA
 * nvtxNameClEventW
 * nvtxNameClMemObjectA
 * nvtxNameClMemObjectW
 * nvtxNameClProgramA
 * nvtxNameClProgramW
 * nvtxNameClSamplerA
 * nvtxNameClSamplerW
 */

/* Not implemented API: nvToolsExtSync.h
 *
 * nvtxDomainSyncUserAcquireFailed
 * nvtxDomainSyncUserAcquireStart
 * nvtxDomainSyncUserAcquireSuccess
 * nvtxDomainSyncUserCreate
 * nvtxDomainSyncUserDestroy
 * nvtxDomainSyncUserReleasing
 * nvtxGetExportTable
 */
