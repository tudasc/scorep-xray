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

/*************** Functions ****************************************************/

void
scorep_cuda_nvtx_init( void );

const char*
scorep_cuda_nvtx_unicode_to_ascii( const wchar_t* wide );

#endif /* SCOREP_CUDA_NVTX_MGMT_H */
