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
 * @brief Compiler adapter interface support to the measurement system.
 *
 * This file contains compiler adapter initialization and finalization functions
 * which are common for all compiler adapters.
 */

#include <config.h>

#include "SCOREP_Compiler_Init.h"

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include "scorep_compiler_confvars.inc.c"

SCOREP_Mutex scorep_compiler_region_mutex = SCOREP_INVALID_MUTEX;

/**
   The adapter initialize function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern SCOREP_ErrorCode
scorep_compiler_subsystem_init( void );

extern SCOREP_ErrorCode
scorep_compiler_subsystem_begin( void );

extern void
scorep_compiler_subsystem_end( void );

/**
   The adapter finalize function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern void
scorep_compiler_subsystem_finalize( void );

/**
   The location init function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern SCOREP_ErrorCode
scorep_compiler_subsystem_init_location( struct SCOREP_Location* location,
                                         struct SCOREP_Location* parent );

/**
   The location finalize function.
 */
static void
compiler_subsystem_finalize_location( struct SCOREP_Location* location )
{
    UTILS_DEBUG( "compiler adapter finalize location!" );
}

/* Implementation of the compiler adapter initialization/finalization struct */
const SCOREP_Subsystem SCOREP_Subsystem_CompilerAdapter =
{
    .subsystem_name              = "COMPILER",
    .subsystem_register          = &compiler_subsystem_register,
    .subsystem_init              = &scorep_compiler_subsystem_init,
    .subsystem_begin             = &scorep_compiler_subsystem_begin,
    .subsystem_end               = &scorep_compiler_subsystem_end,
    .subsystem_finalize          = &scorep_compiler_subsystem_finalize,
    .subsystem_init_location     = &scorep_compiler_subsystem_init_location,
    .subsystem_finalize_location = &compiler_subsystem_finalize_location,
    .subsystem_deregister        = &compiler_subsystem_deregister,
};
