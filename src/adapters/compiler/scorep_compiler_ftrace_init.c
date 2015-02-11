/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief Initialization of the NEC SX compiler adapter.
 */

#include <config.h>

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include "SCOREP_Compiler_Init.h"
#include "scorep_compiler_data.h"


/* ***************************************************************************************
   Adapter management
*****************************************************************************************/

SCOREP_ErrorCode
scorep_compiler_subsystem_init( void )
{
    if ( !scorep_compiler_initialized )
    {
        UTILS_DEBUG( "inititialize ftrace compiler adapter!" );

        /* Initialize region mutex */
        SCOREP_MutexCreate( &scorep_compiler_region_mutex );

        /* Initialize hash table */
        scorep_compiler_hash_init();

        /* Set flag */
        scorep_compiler_initialized = true;
    }

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_compiler_subsystem_init_location( struct SCOREP_Location* locationData )
{
    UTILS_DEBUG( "ftrace Compiler adapter init location!" );
    return SCOREP_SUCCESS;
}

void
scorep_compiler_subsystem_finalize( void )
{
    /* call only, if previously initialized */
    if ( scorep_compiler_initialized )
    {
        /* Delete hash table */
        scorep_compiler_hash_free();

        scorep_compiler_initialized = false;
        scorep_compiler_finalized   = true;
        UTILS_DEBUG( "finalize ftrace compiler adapter!" );

        /* Delete region mutex */
        SCOREP_MutexDestroy( &scorep_compiler_region_mutex );
    }
}
