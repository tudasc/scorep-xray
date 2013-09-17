/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
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


#include <config.h>

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <UTILS_Error.h>

#include <SCOREP_Types.h>
#include <SCOREP_Config.h>
#include <SCOREP_Profile.h>
#include <tracing/SCOREP_Tracing.h>
#include <SCOREP_OA_RuntimeManagement.h>
#include <SCOREP_Filter_Init.h>
#include "scorep_subsystem.h"

#include "scorep_environment.h"

static bool scorep_env_core_environment_variables_initialized = false;


// some config variables.
static bool     scorep_env_verbose;
static bool     scorep_env_tracing;
static bool     scorep_env_profiling;
static uint64_t scorep_env_total_memory;
static uint64_t scorep_env_page_size;
static char*    scorep_env_experiment_directory;
static bool     scorep_env_overwrite_experiment_directory;
static char*    scorep_env_machine_name;

/** @brief Measurement system configure variables */
static SCOREP_ConfigVariable scorep_env_core_environment_variables[] = {
    {
        "enable_profiling",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_env_profiling,
        NULL,
        "true",
        "Enable profiling",
        ""
    },
    {
        "enable_tracing",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_env_tracing,
        NULL,
        "false",
        "Enable tracing",
        ""
    },
    {
        "verbose",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_env_verbose,
        NULL,
        "false",
        "Be verbose",
        ""
    },
    {
        "total_memory",
        SCOREP_CONFIG_TYPE_SIZE,
        &scorep_env_total_memory,
        NULL,
        "16000k",
        "Total memory in bytes for the measurement system",
        ""
    },
    {
        "page_size",
        SCOREP_CONFIG_TYPE_SIZE,
        &scorep_env_page_size,
        NULL,
        "8k", // with 1200k total memory this means 150 pages
        "Memory page size in bytes",
        "TOTAL_MEMORY will be split up into pages of size PAGE_SIZE."
    },
    {
        "experiment_directory",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_env_experiment_directory,
        NULL,
        "",
        "Name of the experiment directory",
        "When no experiment name is given (the default)Score-P names the\n"
        "experiment directory `scorep-measurement-tmp' and renames this\n"
        "after a successful measurement to a generated name based on the\n"
        "current time."
    },
    {
        "overwrite_experiment_directory",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_env_overwrite_experiment_directory,
        NULL,
        "true",
        "Overwrite an existing experiment directory",
        "If you specified a specific experiment directory name, but this name\n"
        "is already given, you can force overwriting it with this flag.\n"
        "The previous experiment directory will be renamed."
    },
    {
        "machine_name",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_env_machine_name,
        NULL,
        SCOREP_DEFAULT_MACHINE_NAME, /* configure-provided default */
        "The machine name used in profile and trace output.",
        "We suggest using a unique name, e.g., the fully qualified domain name.\n"
        "The default machine name was set at configure time (see the INSTALL\n"
        "file for customization options)."
    },
    SCOREP_CONFIG_TERMINATOR
};


#if HAVE( SCOREP_DEBUG )
bool                  scorep_debug_unify;
SCOREP_ConfigVariable scorep_debug_environment_variables[] = {
    {
        "unify",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_debug_unify,
        NULL,
        "true",
        "Writes the pre-unified definitions also in the local definition trace files.",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};
#endif


bool
SCOREP_Env_RunVerbose( void )
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_verbose;
}


bool
SCOREP_Env_DoTracing( void )
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_tracing;
}


bool
SCOREP_Env_DoProfiling( void )
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_profiling;
}


uint64_t
SCOREP_Env_GetTotalMemory( void )
{
    assert( scorep_env_core_environment_variables_initialized );
    assert( scorep_env_total_memory > scorep_env_page_size );
    return scorep_env_total_memory;
}


uint64_t
SCOREP_Env_GetPageSize( void )
{
    assert( scorep_env_core_environment_variables_initialized );
    assert( scorep_env_total_memory > scorep_env_page_size );
    return scorep_env_page_size;
}

const char*
SCOREP_Env_GetExperimentDirectory( void )
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_experiment_directory;
}

bool
SCOREP_Env_DoOverwriteExperimentDirectory( void )
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_overwrite_experiment_directory;
}

const char*
SCOREP_Env_GetMachineName( void )
{
    assert( scorep_env_core_environment_variables_initialized );
    return scorep_env_machine_name;
}

void
SCOREP_RegisterAllConfigVariables( void )
{
    if ( scorep_env_core_environment_variables_initialized )
    {
        return;
    }

    scorep_env_core_environment_variables_initialized = true;

    SCOREP_ErrorCode error;
    error = SCOREP_ConfigRegister( "", scorep_env_core_environment_variables );
    if ( SCOREP_SUCCESS != error )
    {
        UTILS_ERROR( error, "Can't register core environment variables" );
        _Exit( EXIT_FAILURE );
    }

#if HAVE( SCOREP_DEBUG )
    error = SCOREP_ConfigRegister( "debug", scorep_debug_environment_variables );
    if ( SCOREP_SUCCESS != error )
    {
        UTILS_ERROR( error, "Can't register debug environment variables" );
        _Exit( EXIT_FAILURE );
    }
#endif

    SCOREP_Profile_Register();
    SCOREP_Tracing_Register();
    SCOREP_OA_Register();
    SCOREP_Filter_Register();
    scorep_subsystems_register();
}
