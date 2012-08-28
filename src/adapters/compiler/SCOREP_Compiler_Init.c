/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @status     alpha
 * @file       SCOREP_Compiler_Init.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Compiler adapter interface support to the measurement system.
 *
 * This file contains compiler adapter initialization and finalization functions
 * which are common for all compiler adapters.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#include "SCOREP_Compiler_Init.h"
#include "SCOREP_Types.h"
#include <SCOREP_Location.h>
#include "SCOREP_Config.h"
#include <UTILS_Debug.h>

/**
   Contains the name of the executable.
 */
char* scorep_compiler_executable = NULL;

/**
   Configuration variables for the compiler adapter.
   Current configuration variables are:
   @li executable: Executable of the application. Preferrably, with full path. It is used
                   by some compiler adapters (GNU) to evaluate the symbol table.
 */
SCOREP_ConfigVariable scorep_compiler_configs[] = {
    {
        "executable",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_compiler_executable,
        NULL,
        "",
        "Executable of the application.",
        "File name, preferrrably with full path, of the application's executable.\n"
        "It is used for evaluating the symbol table of the application, which is\n"
        "required by some compiler adapters."
    },
    SCOREP_CONFIG_TERMINATOR
};


size_t scorep_compiler_subsystem_id;

/**
   Registers configuration variables for the compiler adapters.
 */
static SCOREP_Error_Code
scorep_compiler_register( size_t subsystem_id )
{
    scorep_compiler_subsystem_id = subsystem_id;

    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " register compiler adapter!" );

    return SCOREP_ConfigRegister( "", scorep_compiler_configs );
}

/**
   The adapter initialize function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern SCOREP_Error_Code
scorep_compiler_init_adapter();

/**
   The location init function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern SCOREP_Error_Code
scorep_compiler_init_location( SCOREP_Location* location );

/**
   The location finalize function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern void
scorep_compiler_finalize_location( SCOREP_Location* location );

/**
   The adapter finalize function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern void
scorep_compiler_finalize();

/**
   Called on dereigstration of the compiler adapter. Currently, no action is performed
   on deregistration.
 */
void
scorep_compiler_deregister()
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " compiler adapter deregister!n" );
    free( scorep_compiler_executable );
}


/* Implementation of the compiler adapter initialization/finalization struct */
const SCOREP_Subsystem SCOREP_Compiler_Adapter =
{
    "COMPILER",
    &scorep_compiler_register,
    &scorep_compiler_init_adapter,
    &scorep_compiler_init_location,
    &scorep_compiler_finalize_location,
    &scorep_compiler_finalize,
    &scorep_compiler_deregister
};
