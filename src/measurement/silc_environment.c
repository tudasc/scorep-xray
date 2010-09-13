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
 * @file       silc_environment.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>
#include "silc_environment.h"

#include <silc_utility/SILC_Error.h>
#include <stdlib.h>
#include <SILC_Types.h>
#include <SILC_Config.h>
#include <assert.h>


static bool silc_env_core_environment_variables_initialized = false;


// some config variables.
static bool     silc_env_verbose;
static bool     silc_env_unify;
static bool     silc_env_tracing;
static bool     silc_env_profiling;
static uint64_t silc_env_total_memory;
static uint64_t silc_env_page_size;


/** @brief Measurement system configure variables */
SILC_ConfigVariable silc_env_core_environment_variables[] = {
    {
        "ENABLE_PROFILING",
        SILC_CONFIG_TYPE_BOOL,
        &silc_env_profiling,
        NULL,
        "false",
        "enable profiling",
        "enable profiling"
    },
    {
        "ENABLE_TRACING",
        SILC_CONFIG_TYPE_BOOL,
        &silc_env_tracing,
        NULL,
        "true",
        "enable tracing",
        "enable tracing"
    },
    {
        "VERBOSE",
        SILC_CONFIG_TYPE_BOOL,
        &silc_env_verbose,
        NULL,
        "false",
        "Be verbose",
        "Long help"
    },
    {
        "UNIFY",
        SILC_CONFIG_TYPE_BOOL,
        &silc_env_unify,
        NULL,
        "false",
        "do unification step",
        ""
    },
    {
        "TOTAL_MEMORY",
        SILC_CONFIG_TYPE_SIZE,
        &silc_env_total_memory,
        NULL,
        "1200k",
        "total memory in bytes for the measurement system, excluding trace memory",
        ""
    },
    {
        "PAGE_SIZE",
        SILC_CONFIG_TYPE_SIZE,
        &silc_env_page_size,
        NULL,
        "8k", // with 1200k total memory this means 150 pages
        "memory page size in bytes",
        "TOTAL_MEMORY will be split up into pages of size PAGE_SIZE."
    },
    SILC_CONFIG_TERMINATOR
};


void
SILC_Env_InitializeCoreEnvironmentVariables()
{
    if ( silc_env_core_environment_variables_initialized )
    {
        return;
    }

    SILC_Error_Code error = SILC_ConfigRegister( NULL, silc_env_core_environment_variables );

    if ( SILC_SUCCESS != error )
    {
        SILC_ERROR( error, "Can't register core environment variables" );
        _Exit( EXIT_FAILURE );
    }

    assert( silc_env_total_memory > silc_env_page_size );

    silc_env_core_environment_variables_initialized = true;
}


//bool
//SILC_Env_CoreEnvironmentVariablesInitialized()
//{
//    return silc_env_core_environment_variables_initialized;
//}


bool
SILC_Env_RunVerbose()
{
    assert( silc_env_core_environment_variables_initialized );
    return silc_env_verbose;
}


bool
SILC_Env_DoUnification()
{
    assert( silc_env_core_environment_variables_initialized );
    return silc_env_unify;
}

bool
SILC_Env_DoTracing()
{
    assert( silc_env_core_environment_variables_initialized );
    return silc_env_tracing;
}


bool
SILC_Env_DoProfiling()
{
    assert( silc_env_core_environment_variables_initialized );
    return silc_env_profiling;
}


uint64_t
SILC_Env_GetTotalMemory()
{
    assert( silc_env_core_environment_variables_initialized );
    return silc_env_total_memory;
}


uint64_t
SILC_Env_GetPageSize()
{
    assert( silc_env_core_environment_variables_initialized );
    return silc_env_page_size;
}
