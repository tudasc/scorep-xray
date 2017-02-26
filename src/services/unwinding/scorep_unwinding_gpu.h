/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 *
 */

#ifndef SCOREP_UNWINDING_GPU_H
#define SCOREP_UNWINDING_GPU_H

#include "scorep_unwinding_mgmt.h"

struct SCOREP_Location;

SCOREP_Unwinding_GpuLocationData*
scorep_unwinding_gpu_get_location_data( struct SCOREP_Location* location );

SCOREP_ErrorCode
scorep_unwinding_gpu_handle_enter( SCOREP_Unwinding_GpuLocationData* unwindData,
                                   SCOREP_RegionHandle               instrumentedRegionHandle,
                                   SCOREP_CallingContextHandle*      callingContext,
                                   uint32_t*                         unwindDistance,
                                   SCOREP_CallingContextHandle*      previousCallingContext );

SCOREP_ErrorCode
scorep_unwinding_gpu_handle_exit( SCOREP_Unwinding_GpuLocationData* unwindData,
                                  SCOREP_CallingContextHandle*      callingContext,
                                  uint32_t*                         unwindDistance,
                                  SCOREP_CallingContextHandle*      previousCallingContext );


#endif /* SCOREP_UNWINDING_GPU_H */
