/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include <SCOREP_Unwinding.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Location.h>
#include <SCOREP_Config.h>

#include "scorep_unwinding_confvars.inc.c"

void
SCOREP_Unwinding_PushWrapper( SCOREP_Location*    location,
                              SCOREP_RegionHandle regionHandle,
                              uint64_t            wrapperIp,
                              size_t              framesToSkip )
{
    UTILS_BUG( "This function should not have been called." );
}

void
SCOREP_Unwinding_PopWrapper( SCOREP_Location*    location,
                             SCOREP_RegionHandle regionHandle )
{
    UTILS_BUG( "This function should not have been called." );
}

void
SCOREP_Unwinding_GetCallingContext( SCOREP_Location*             location,
                                    void*                        contextPtr,
                                    SCOREP_Unwinding_Origin      origin,
                                    SCOREP_RegionHandle          instrumentedRegionHandle,
                                    SCOREP_CallingContextHandle* currentCallingContext,
                                    SCOREP_CallingContextHandle* previousCallingContext,
                                    uint32_t*                    unwindDistance )
{
    UTILS_BUG( "This function should not have been called." );
}

void
SCOREP_Unwinding_ProcessCallingContext( SCOREP_Location*                location,
                                        uint64_t                        timestamp,
                                        uint64_t*                       metricValues,
                                        SCOREP_CallingContextHandle     currentCallingContext,
                                        SCOREP_CallingContextHandle     previousCallingContext,
                                        uint32_t                        unwindDistance,
                                        SCOREP_Substrates_EnterRegionCb enterCb,
                                        SCOREP_Substrates_ExitRegionCb  exitCb )
{
    UTILS_BUG( "This function should not have been called." );
}

/* *********************************************************************
 * Subsystem declaration
 **********************************************************************/

/**
 * Implementation of the unwinding service initialization/finalization struct
 */
const SCOREP_Subsystem SCOREP_Subsystem_UnwindingService =
{
    .subsystem_name = "UNWINDING",
};
