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
 *  @file
 */

#ifndef SCOREP_CUDA_NVTX_MGMT_H
#define SCOREP_CUDA_NVTX_MGMT_H


#include <wchar.h>

#include <nvToolsExt.h>

#include <SCOREP_Definitions.h>

/*************** Variables ****************************************************/

#define SCOREP_CUDA_NVTX_DEFAULT_DOMAIN NULL

/*************** Functions ****************************************************/

void
scorep_cuda_nvtx_init( void );

const char*
scorep_cuda_nvtx_unicode_to_ascii( const wchar_t* wide );

nvtxDomainHandle_t
scorep_cuda_nvtx_create_domain( const char* name );

nvtxStringHandle_t
scorep_cuda_nvtx_create_string( const char* string );

const char*
scorep_cuda_nvtx_get_name_from_attributes( const nvtxEventAttributes_t* eventAttrib );

SCOREP_RegionHandle
scorep_cuda_nvtx_get_user_region( nvtxDomainHandle_t           domain,
                                  const nvtxEventAttributes_t* eventAttrib );

void
scorep_cuda_nvtx_apply_payload( const nvtxEventAttributes_t* eventAttrib );

void
scorep_cuda_nvtx_name_category( nvtxDomainHandle_t domain,
                                uint32_t           category,
                                const char*        name );

void
scorep_cuda_nvtx_apply_category( nvtxDomainHandle_t           domain,
                                 const nvtxEventAttributes_t* eventAttrib );

void
scorep_cuda_nvtx_set_stream_name( void*       stream,
                                  const char* name );

void
scorep_cuda_nvtx_set_context_name( void*       context,
                                   const char* name );

#endif /* SCOREP_CUDA_NVTX_MGMT_H */
