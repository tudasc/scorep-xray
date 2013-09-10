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
 * @file
 *
 * @brief Compiler adapter interface support to the measurement system.
 *
 * This file contains compiler adapter initialization and finalization functions
 * which are common for all compiler adapters.
 */

#include <config.h>
#include "SCOREP_Compiler_Init.h"

#include <stdlib.h>

#include "SCOREP_Config.h"
#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>


#include "scorep_compiler_confvars.inc.c"

bool         scorep_compiler_initialized = false;
bool         scorep_compiler_finalized   = false;
SCOREP_Mutex scorep_compiler_region_mutex;

/**
   The adapter initialize function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern SCOREP_ErrorCode
scorep_compiler_init_adapter( void );

/**
   The location init function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern SCOREP_ErrorCode
scorep_compiler_init_location( struct SCOREP_Location* location );

/**
   The location finalize function.
 */
static void
scorep_compiler_finalize_location( struct SCOREP_Location* location );

/**
   The adapter finalize function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern void
scorep_compiler_finalize( void );

/* Implementation of the compiler adapter initialization/finalization struct */
const SCOREP_Subsystem SCOREP_Subsystem_CompilerAdapter =
{
    .subsystem_name              = "COMPILER",
    .subsystem_register          = &scorep_compiler_register,
    .subsystem_init              = &scorep_compiler_init_adapter,
    .subsystem_init_location     = &scorep_compiler_init_location,
    .subsystem_finalize_location = &scorep_compiler_finalize_location,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = &scorep_compiler_finalize,
    .subsystem_deregister        = &scorep_compiler_deregister,
    .subsystem_control           = NULL
};

/* Location finalization */
static void
scorep_compiler_finalize_location( struct SCOREP_Location* locationData )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "compiler adapter finalize location!" );
}
