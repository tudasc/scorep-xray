/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @ file      SILC_Compiler_Init.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Compiler adapter interface support to the measurement system.
 *
 * This file contains compiler adapter initialization and finalization functions
 * which are common for all compiler adapters.
 */

#include <stdio.h>

#include "SILC_Compiler_Init.h"
#include "SILC_Types.h"
#include "SILC_Config.h"
#include "SILC_Error.h"
#include <SILC_Utils.h>

/**
   Contains the name of the executable.
 */
char* silc_compiler_executable = NULL;

/**
   Configuration variables for the compiler adapter.
   Current configuration variables are:
   @li executable: Executable of the application. Preferrably, with full path. It is used
                   by some compiler adapters (GNU) to evaluate the symbol table.
 */
SILC_ConfigVariable silc_compiler_configs[] = {
    {
        "executable",
        SILC_CONFIG_TYPE_STRING,
        &silc_compiler_executable,
        NULL,
        NULL,
        "Executable of the application.",
        "File name, preferrrably with full path, of the application's executable.\n"
        "It is used for evaluating the symbol table of the application, which is\n"
        "required by some compiler adapters."
    },
    SILC_CONFIG_TERMINATOR
};


/**
   The adapter initialize function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern SILC_Error_Code
silc_compiler_init_adapter();

/**
   The adapter finalize function is compiler specific. Thus it is contained in each
   compiler adapter implementation.
 */
extern void
silc_compiler_finalize();

/**
   Registers configuration variables for the compiler adapters.
 */
SILC_Error_Code
silc_compiler_register()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " register compiler adapter!" );

    return SILC_ConfigRegister( NULL, silc_compiler_configs );
}

/**
   Location specific initialization function for compiler adapters. Currently, no
   location specific initialization is performed.
 */
SILC_Error_Code
silc_compiler_init_location()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " compiler adapter init loacation!" );
    return SILC_SUCCESS;
}

/**
   Location specific finalization function for compiler adapters. Currently, no
   location specific finalization is performed.
 */
void
silc_compiler_final_location()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " compiler adapter final loacation!" );
}

/**
   Called on dereigstration of the compiler adapter. Currently, no action is performed
   on deregistration.
 */
void
silc_compiler_deregister()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " compiler adapter deregister!n" );
}


/* Implementation of the compiler adapter initialization/finalization struct */
const SILC_Adapter SILC_Compiler_Adapter =
{
    SILC_ADAPTER_COMPILER,
    "COMPILER",
    &silc_compiler_register,
    &silc_compiler_init_adapter,
    &silc_compiler_init_location,
    &silc_compiler_final_location,
    &silc_compiler_finalize,
    &silc_compiler_deregister
};
