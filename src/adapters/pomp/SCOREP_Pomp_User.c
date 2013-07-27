/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
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
 */

/**
 * @file       src/adapters/pomp/SCOREP_Pomp_User.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 user adapter functions.
 */

#include <config.h>

#include "SCOREP_Pomp_Common.h"
#include "SCOREP_Pomp_RegionInfo.h"

#include <SCOREP_Events.h>
#include <SCOREP_RuntimeManagement.h>

#define SCOREP_DEBUG_MODULE_NAME OPENMP
#include <UTILS_Debug.h>

#include <opari2/pomp2_lib.h>

/** @ingroup POMP2
    @{
 */

/* **************************************************************************************
 *                                                                   POMP event functions
 ***************************************************************************************/

void
POMP2_Begin( POMP2_Region_handle* pomp_handle,
             const char           ctc_string[] )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }
}

void
POMP2_End( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_End" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }
}

void
POMP2_Init( void )
{
    UTILS_DEBUG_ENTRY();

    /* If adapter is not initialized, it means that the measurement
       system is not initialized. */
    SCOREP_InitMeasurement();

    UTILS_DEBUG_EXIT();
}

void
POMP2_Finalize( void )
{
    UTILS_DEBUG_ENTRY();
    UTILS_DEBUG_EXIT();
}

void
POMP2_On( void )
{
    SCOREP_POMP2_ENSURE_INITIALIZED;
    scorep_pomp_is_tracing_on = true;
}

void
POMP2_Off( void )
{
    SCOREP_POMP2_ENSURE_INITIALIZED;
    scorep_pomp_is_tracing_on = false;
}

/** @} */
