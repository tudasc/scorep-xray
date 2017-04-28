/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
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
 * @file
 *
 * @brief Initialization of the PGI compiler adapter.
 */

#include <config.h>

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include <SCOREP_Mutex.h>

#include "SCOREP_Compiler_Init.h"


/* **************************************************************************************
 * Initialization / Finalization
 ***************************************************************************************/

/* Adapter initialization */
SCOREP_ErrorCode
scorep_compiler_subsystem_init( void )
{
    UTILS_DEBUG( "initialize PGI compiler adapter!" );

    /* Initialize region mutex */
    SCOREP_MutexCreate( &scorep_compiler_region_mutex );

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_compiler_subsystem_begin( void )
{
    return SCOREP_SUCCESS;
}

void
scorep_compiler_subsystem_end( void )
{
}

/* Adapter finalization */
void
scorep_compiler_subsystem_finalize( void )
{
    /* Delete region mutex */
    SCOREP_MutexDestroy( &scorep_compiler_region_mutex );

    UTILS_DEBUG( "finalize PGI compiler adapter!" );
}

/* Location initialization */
SCOREP_ErrorCode
scorep_compiler_subsystem_init_location( struct SCOREP_Location* locationData,
                                         struct SCOREP_Location* parent )
{
    UTILS_DEBUG( "PGI compiler adapter init location!" );
    return SCOREP_SUCCESS;
}
