/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_UNWINDING_H
#define SCOREP_UNWINDING_H

/**
 * @file
 *
 */

#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Location.h>
#include <SCOREP_Types.h>
#include <scorep_substrates_definition.h>

#include <stdbool.h>
#include <stdint.h>


/**
 * Predefined types specifying the origin of the unwind request.
 */
typedef enum
{
    /**
     * Unwinding request was triggered by an instrumented function enter event.
     */
    SCOREP_UNWINDING_ORIGIN_INSTRUMENTED_ENTER, /**<< */

    /**
     * Unwinding request was triggered by an instrumented function exit event.
     */
    SCOREP_UNWINDING_ORIGIN_INSTRUMENTED_EXIT,

    /**
     * Unwinding request was triggered by a sample event.
     */
    SCOREP_UNWINDING_ORIGIN_SAMPLE
} SCOREP_Unwinding_Origin;

/**
 * Create the calling context.
 *
 * @param location                    Score-P location
 * @param origin                      From which type of event comes this request
 * @param instrumentedRegionHandle    Region handle of an instrumented functions
 * @param wrappedRegion               Address of the function to be wrapped with the Enter event
 * @param framesToSkip                Number of frames to skipped outside of Score-P
 * @param[out] currentCallingContext  Handle of the current calling context
 * @param[out] previousCallingContext Handle of the last known calling context
 * @param[out] unwindDistance         Number of stack levels changed since the last sample
 *
 */
void
SCOREP_Unwinding_GetCallingContext( SCOREP_Location*             location,
                                    SCOREP_Unwinding_Origin      origin,
                                    SCOREP_RegionHandle          instrumentedRegionHandle,
                                    intptr_t                     wrappedRegion,
                                    size_t                       framesToSkip,
                                    SCOREP_CallingContextHandle* currentCallingContext,
                                    SCOREP_CallingContextHandle* previousCallingContext,
                                    uint32_t*                    unwindDistance );

void
SCOREP_Unwinding_ProcessCallingContext( SCOREP_Location*                location,
                                        uint64_t                        timestamp,
                                        uint64_t*                       metricValues,
                                        SCOREP_CallingContextHandle     currentCallingContext,
                                        SCOREP_CallingContextHandle     previousCallingContext,
                                        uint32_t                        unwindDistance,
                                        SCOREP_Substrates_EnterRegionCb enterCb,
                                        SCOREP_Substrates_ExitRegionCb  exitCb );

#endif /* SCOREP_UNWINDING_H */
