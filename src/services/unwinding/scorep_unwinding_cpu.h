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

#ifndef SCOREP_UNWINDING_CPU_H
#define SCOREP_UNWINDING_CPU_H

#include "scorep_unwinding_mgmt.h"

struct SCOREP_Location;

SCOREP_ErrorCode
scorep_unwinding_cpu_init_location( struct SCOREP_Location* location );

SCOREP_ErrorCode
scorep_unwinding_cpu_handle_enter( struct SCOREP_Location*      location,
                                   SCOREP_RegionHandle          instrumentedRegionHandle,
                                   intptr_t                     wrappedRegion,
                                   size_t                       framesToSkip,
                                   SCOREP_CallingContextHandle* callingContext,
                                   uint32_t*                    unwindDistance,
                                   SCOREP_CallingContextHandle* previousCallingContext );

SCOREP_ErrorCode
scorep_unwinding_cpu_handle_exit( struct SCOREP_Location*      location,
                                  SCOREP_CallingContextHandle* callingContext,
                                  uint32_t*                    unwindDistance,
                                  SCOREP_CallingContextHandle* previousCallingContext );

void
scorep_unwinding_cpu_deactivate( struct SCOREP_Location* location );

#endif /* SCOREP_UNWINDING_CPU_H */
