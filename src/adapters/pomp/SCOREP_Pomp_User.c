/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       SCOREP_Pomp_User.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 user adapter functions and initialization.
 */

#include <config.h>

#include "SCOREP_Pomp_Common.h"
#include "SCOREP_Pomp_RegionInfo.h"

#include <SCOREP_Events.h>
#include <SCOREP_Subsystem.h>

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

/** @} */

/* *****************************************************************************
 *                                                         POMP User subsystem *
 ******************************************************************************/

static size_t scorep_pomp_user_subsystem_id;

/** Adapter initialization function to allow registering configuration
    variables. No variables are regstered.
 */
static SCOREP_ErrorCode
scorep_pomp_user_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    scorep_pomp_user_subsystem_id = subsystem_id;

    return SCOREP_SUCCESS;
}

/** Adapter initialization function.
 */
static SCOREP_ErrorCode
scorep_pomp_user_init( void )
{
    UTILS_DEBUG_ENTRY();

    /* Initialize the common POMP adapter */
    scorep_pomp_adapter_init();

    UTILS_DEBUG_EXIT();

    return SCOREP_SUCCESS;
}

/** Adapter finalialization function.
 */
static void
scorep_pomp_user_finalize( void )
{
    UTILS_DEBUG_ENTRY();

    scorep_pomp_adapter_finalize();

    UTILS_DEBUG_ENTRY();
}

/** Struct which contains the adapter iniitialization and finalization
    functions for the POMP2 adapter.
 */
const SCOREP_Subsystem SCOREP_Subsystem_PompUserAdapter =
{
    .subsystem_name              = "POMP2 User Adapter / Version 1.0",
    .subsystem_register          = &scorep_pomp_user_register,
    .subsystem_init              = &scorep_pomp_user_init,
    .subsystem_init_location     = NULL,
    .subsystem_finalize_location = NULL,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = &scorep_pomp_user_finalize,
    .subsystem_deregister        = NULL,
    .subsystem_control           = NULL
};
