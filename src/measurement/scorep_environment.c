/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 * @file       scorep_environment.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include "scorep_environment.h"

#include <scorep_utility/SCOREP_Error.h>
#include <stdlib.h>
#include <SCOREP_Types.h>
#include <SCOREP_Config.h>
#include <assert.h>


static bool scorep_env_core_environment_variables_initialized = false;


// some config variables.
static bool     scorep_env_verbose;
static bool     scorep_env_unify;
static bool     scorep_env_tracing;
static bool     scorep_env_profiling;
static uint64_t scorep_env_total_memory;
static uint64_t scorep_env_page_size;


/** @brief Measurement system configure variables */
SCOREP_ConfigVariable scorep_env_core_environment_variables[] = {
    {
        "ENABLE_PROFILING",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_env_profiling,
        NULL,
        "false",
        "enable profiling",
        "enable profiling"
    },
    {
        "ENABLE_TRACING",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_env_tracing,
        NULL,
        "true",
        "enable tracing",
        "enable tracing"
    },
    {
        "VERBOSE",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_env_verbose,
        NULL,
        "false",
        "Be verbose",
        "Long help"
    },
    {
        "UNIFY",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_env_unify,
        NULL,
        "false",
        "do unification step",
        ""
    },
    {
        "TOTAL_MEMORY",
        SCOREP_CONFIG_TYPE_SIZE,
        &scorep_env_total_memory,
        NULL,
        "1200k",
        "total memory in bytes for the measurement system, excluding trace memory",
        ""
    },
    {
        "PAGE_SIZE",
        SCOREP_CONFIG_TYPE_SIZE,
        &scorep_env_page_size,
        NULL,
        "8k", // with 1200k total memory this means 150 pages
        "memory page size in bytes",
        "TOTAL_MEMORY will be split up into pages of size PAGE_SIZE."
    },
    SCOREP_CONFIG_TERMINATOR
};


void
SCOREP_Env_InitializeCoreEnvironmentVariables()
{
    if ( scorep_env_core_environment_variables_initialized )
    {
        return;
    }

    SCOREP_Error_Code error = SCOREP_ConfigRegister( NULL, scorep_env_core_environment_variables );

    if ( SCOREP_SUCCESS != error )
    {
        SCOREP_ERROR( error, "Can't register core environment variables" );
        _Exit( EXIT_FAILURE );
    }

    assert( scorep_env_total_memory > scorep_env_page_size );

    scorep_env_core_environment_variables_initialized = true;
}


//bool
//SCOREP_Env_CoreEnvironmentVariablesInitialized()
//{
//    return scorep_env_core_environment_variables_initialized;
//}


bool
SCOREP_Env_RunVerbose()
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_verbose;
}


bool
SCOREP_Env_DoUnification()
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_unify;
}

bool
SCOREP_Env_DoTracing()
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_tracing;
}


bool
SCOREP_Env_DoProfiling()
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_profiling;
}


uint64_t
SCOREP_Env_GetTotalMemory()
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_total_memory;
}


uint64_t
SCOREP_Env_GetPageSize()
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_page_size;
}
