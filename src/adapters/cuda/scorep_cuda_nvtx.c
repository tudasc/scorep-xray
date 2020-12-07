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

#include <SCOREP_InMeasurement.h>
#include <SCOREP_RuntimeManagement.h>

#include <nvToolsExt.h>

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

    // Push/pop region

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxMarkEx( const nvtxEventAttributes_t* eventAttrib )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxMarkA( const char* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxMarkW( const wchar_t* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }
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
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return 0;
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return 0;
}

NVTX_DECLSPEC nvtxRangeId_t NVTX_API
nvtxRangeStartA( const char* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return 0;
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return 0;
}

NVTX_DECLSPEC nvtxRangeId_t NVTX_API
nvtxRangeStartW( const wchar_t* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return 0;
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return 0;
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

    // Enter region

    SCOREP_IN_MEASUREMENT_DECREMENT();

    // Return 0 on success, negative on error (compatible with SCOREP_Error values?)
    return 0;
}

NVTX_DECLSPEC int NVTX_API
nvtxRangePushEx( const nvtxEventAttributes_t* eventAttrib )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return 0;
}

NVTX_DECLSPEC int NVTX_API
nvtxRangePushA( const char* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return 0;
}

NVTX_DECLSPEC int NVTX_API
nvtxRangePushW( const wchar_t* message )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return 0;
}

NVTX_DECLSPEC int NVTX_API
nvtxDomainRangePop( nvtxDomainHandle_t domain )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    // Exit/pop region

    SCOREP_IN_MEASUREMENT_DECREMENT();

    // Return 0 on success, negative on error (compatible with SCOREP_Error values?)
    return 0;
}

NVTX_DECLSPEC int NVTX_API
nvtxRangePop( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return 0;
}

/*************** Domain management ********************************************/

NVTX_DECLSPEC nvtxDomainHandle_t NVTX_API
nvtxDomainCreateA( const char* name )
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

    // Create a domain handle with a name
    // Names here will get used for region group name(?)

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return NULL;
}

NVTX_DECLSPEC nvtxDomainHandle_t NVTX_API
nvtxDomainCreateW( const wchar_t* name )
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

    // Create a domain handle with a name
    // Names here will get used for region group name(?)

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return NULL;
}

NVTX_DECLSPEC void NVTX_API
nvtxDomainDestroy( nvtxDomainHandle_t domain )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    // Remove from list of handles/invalidate/free

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
    // Not implemented
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxDomainNameCategoryW( nvtxDomainHandle_t domain,
                         uint32_t           category,
                         const wchar_t*     name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    // Not implemented
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxNameCategoryA( uint32_t    category,
                   const char* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxNameCategoryW( uint32_t       category,
                   const wchar_t* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*************** Thread naming ************************************************/

NVTX_DECLSPEC void NVTX_API
nvtxNameOsThreadA( uint32_t    threadId,
                   const char* name )
{
    // Not implemented
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

NVTX_DECLSPEC void NVTX_API
nvtxNameOsThreadW( uint32_t       threadId,
                   const wchar_t* name )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*************** String registration ******************************************/

NVTX_DECLSPEC nvtxStringHandle_t NVTX_API
nvtxDomainRegisterStringA( nvtxDomainHandle_t domain,
                           const char*        string )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    // Wrap a SCOREP_StringHandle
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return NULL;
}

NVTX_DECLSPEC nvtxStringHandle_t NVTX_API
nvtxDomainRegisterStringW( nvtxDomainHandle_t domain,
                           const wchar_t*     string )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    // Convert to char* and return from -A version
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return NULL;
}

/* Not implemented API: nvToolsExtCuda.h
 *
 * nvtxNameCuContextA
 * nvtxNameCuContextW
 * nvtxNameCuDeviceA
 * nvtxNameCuDeviceW
 * nvtxNameCuEventA
 * nvtxNameCuEventW
 * nvtxNameCuStreamA
 * nvtxNameCuStreamW
 */

/* Not implemented API: nvToolsExtCudaRt.h
 *
 * nvtxNameCudaDeviceA
 * nvtxNameCudaDeviceW
 * nvtxNameCudaEventA
 * nvtxNameCudaEventW
 * nvtxNameCudaStreamA
 * nvtxNameCudaStreamW
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
