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
 * @file       src/adapters/compiler/scorep_compiler_confvars.inc.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


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
        "Executable of the application",
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
    UTILS_DEBUG_ENTRY();

    scorep_compiler_subsystem_id = subsystem_id;

    return SCOREP_ConfigRegister( "", scorep_compiler_configs );
}

/**
   Called on dereigstration of the compiler adapter. Currently, no action is performed
   on deregistration.
 */
static void
scorep_compiler_deregister()
{
    UTILS_DEBUG_ENTRY();

    free( scorep_compiler_executable );
}
