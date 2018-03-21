/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2017,
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

#ifndef SCOREP_UNWINDING_CPU_H
#define SCOREP_UNWINDING_CPU_H

#include "scorep_unwinding_mgmt.h"

struct SCOREP_Location;

SCOREP_Unwinding_CpuLocationData*
scorep_unwinding_cpu_get_location_data( struct SCOREP_Location* location );

/** Called by @a SCOREP_Unwinding_PushWrapper for CPU locations. */
void
scorep_unwinding_cpu_push_wrapper( SCOREP_Unwinding_CpuLocationData* unwindData,
                                   SCOREP_RegionHandle               regionHandle,
                                   uint64_t                          wrapperIp,
                                   size_t                            framesToSkip );

/** Called by @a SCOREP_Unwinding_PopWrapper for CPU locations. */
void
scorep_unwinding_cpu_pop_wrapper( SCOREP_Unwinding_CpuLocationData* unwindData,
                                  SCOREP_RegionHandle               regionHandle );

/**
 * Called by @a SCOREP_Unwinding_GetCallingContext for CPU locations and
 * SCOREP_UNWINDING_ORIGIN_INSTRUMENTED_ENTER or SCOREP_UNWINDING_ORIGIN_SAMPLE.
 */
SCOREP_ErrorCode
scorep_unwinding_cpu_handle_enter( SCOREP_Unwinding_CpuLocationData* unwindData,
                                   void*                             contextPtr,
                                   SCOREP_RegionHandle               instrumentedRegionHandle,
                                   SCOREP_CallingContextHandle*      callingContext,
                                   uint32_t*                         unwindDistance,
                                   SCOREP_CallingContextHandle*      previousCallingContext );

/**
 * Called by @a SCOREP_Unwinding_GetCallingContext for CPU locations and
 * SCOREP_UNWINDING_ORIGIN_INSTRUMENTED_EXIT.
 */
SCOREP_ErrorCode
scorep_unwinding_cpu_handle_exit( SCOREP_Unwinding_CpuLocationData* unwindData,
                                  SCOREP_CallingContextHandle*      callingContext,
                                  uint32_t*                         unwindDistance,
                                  SCOREP_CallingContextHandle*      previousCallingContext );

void
scorep_unwinding_cpu_deactivate( SCOREP_Unwinding_CpuLocationData* unwindData );

#endif /* SCOREP_UNWINDING_CPU_H */
