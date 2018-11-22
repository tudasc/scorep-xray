/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2014-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
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
 *
 */


#include <SCOREP_Config.h>

#include <stdlib.h>

/**
   Filename of file containing nm -l output for compiler instrumentation.
 */
char* scorep_compiler_nm_symbols = NULL;

/**
   Configuration variables for the compiler adapter.
   Current configuration variables are:
   @li executable: Executable of the application. Preferably, with full path. It is used
                   by some compiler adapters (GNU) to evaluate the symbol table.
 */
static const SCOREP_ConfigVariable scorep_compiler_confvars[] = {
    {
        "nm_symbols",
        SCOREP_CONFIG_TYPE_PATH,
        &scorep_compiler_nm_symbols,
        NULL,
        "",
        "Application's symbol table obtained via \'nm -l\' for compiler instrumentation",
        "File name, preferably with full path, of <file> that contains the "
        "<application>'s symbol table that was obtained by the command:\n"
        "\n"
        "$ nm -l <application> 2> /dev/null > <file>\n"
        "\n"
        "Only needed if generating the file at measurement initialization time fails, e.g., "
        "if using the 'system()' command from the compute nodes isn't possible."
    },
    SCOREP_CONFIG_TERMINATOR
};


size_t scorep_compiler_subsystem_id;

/**
   Registers configuration variables for the compiler adapters.
 */
static SCOREP_ErrorCode
compiler_subsystem_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    scorep_compiler_subsystem_id = subsystem_id;

    return SCOREP_ConfigRegisterCond( "",
                                      scorep_compiler_confvars,
                                      HAVE_BACKEND_COMPILER_INSTRUMENTATION_NEEDS_SYMBOL_TABLE );
}

/**
   Called on deregistration of the compiler adapter. Currently, no action is performed
   on deregistration.
 */
static void
compiler_subsystem_deregister( void )
{
    UTILS_DEBUG_ENTRY();

    free( scorep_compiler_nm_symbols );
}
