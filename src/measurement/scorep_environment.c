/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2015-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2015, 2017-2018,
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

#include "scorep_environment.h"

#include <UTILS_Error.h>

#include <SCOREP_Types.h>
#include <SCOREP_Config.h>
#include <SCOREP_Profile.h>
#include <tracing/SCOREP_Tracing.h>
#include <SCOREP_OA_RuntimeManagement.h>
#include <SCOREP_Filtering_Management.h>
#include <SCOREP_Timer_Utils.h>
#include "scorep_subsystem_management.h"
#include "scorep_system_tree_sequence.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

static bool env_variables_initialized = false;


// some config variables.

/*
 * General measurement setup
 */
static bool     env_verbose;
static uint64_t env_total_memory;
static uint64_t env_page_size;
static char*    env_experiment_directory;
static bool     env_overwrite_experiment_directory;
static char*    env_machine_name;
static char*    env_executable;
static bool     env_system_tree_sequence;
static bool     force_cfg_files;

/*
 * Tracing setup
 */
static bool env_tracing;

/*
 * Profiling setup
 */
static bool env_profiling;

/*
 * Calling context setup
 */
static bool env_unwinding;


/** @brief Measurement system configure variables */
static const SCOREP_ConfigVariable core_enable_confvars[] = {
    {
        "enable_profiling",
        SCOREP_CONFIG_TYPE_BOOL,
        &env_profiling,
        NULL,
        "true",
        "Enable profiling",
        ""
    },
    {
        "enable_tracing",
        SCOREP_CONFIG_TYPE_BOOL,
        &env_tracing,
        NULL,
        "false",
        "Enable tracing",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};

static const SCOREP_ConfigVariable core_enable_unwinding_confvars[] = {
    {
        "enable_unwinding",
        SCOREP_CONFIG_TYPE_BOOL,
        &env_unwinding,
        NULL,
        "false",
        "Enables recording calling context information for every event",
        "The calling context is the call chain of functions to the current "
        "position in the running program. This call chain will also be "
        "annotated with source code information if possible.\n"
        "This is a prerequisite for sampling but also works with instrumented "
        "applications.\n"
        "Note that when tracing is also enabled, Score-P does not write the "
        "usual Enter/Leave records into the OTF2 trace, but new records.\n"
        "See also SCOREP_TRACING_CONVERT_CALLING_CONTEXT_EVENTS.\n"
        "Note also that this supresses events from the compiler instrumentation."
    },
    SCOREP_CONFIG_TERMINATOR
};

static const SCOREP_ConfigVariable core_confvars[] = {
    {
        "verbose",
        SCOREP_CONFIG_TYPE_BOOL,
        &env_verbose,
        NULL,
        "false",
        "Be verbose",
        ""
    },
    {
        "total_memory",
        SCOREP_CONFIG_TYPE_SIZE,
        &env_total_memory,
        NULL,
        "16000k",
        "Total memory in bytes per process to be consumed by the measurement system",
        "SCOREP_TOTAL_MEMORY will be split into pages of size SCOREP_PAGE_SIZE "
        "(potentially reduced to a multiple of SCOREP_PAGE_SIZE). Maximum size is 4 GB"
        "minus one SCOREP_PAGE_SIZE."
    },
    {
        "page_size",
        SCOREP_CONFIG_TYPE_SIZE,
        &env_page_size,
        NULL,
        "8k",
        "Memory page size in bytes",
        "If not a power of two, SCOREP_PAGE_SIZE will be increased to the next "
        "larger power of two. SCOREP_TOTAL_MEMORY will be split up into pages "
        "of (the adjusted) SCOREP_PAGE_SIZE. Minimum size is 512 bytes."
    },
    {
        "experiment_directory",
        SCOREP_CONFIG_TYPE_PATH,
        &env_experiment_directory,
        NULL,
        "",
        "Name of the experiment directory as child of the current working directory",
        "The experiment directory is created directly under the current working "
        "directory. No parent directories will be created. The experiment "
        "directory is only created if it is requested by at least one substrate. "
        "When no experiment name is given (the default) Score-P names the "
        "experiment directory `scorep-measurement-tmp' and renames this "
        "after a successful measurement to a generated name based on the "
        "current time."
    },
    {
        "overwrite_experiment_directory",
        SCOREP_CONFIG_TYPE_BOOL,
        &env_overwrite_experiment_directory,
        NULL,
        "true",
        "Overwrite an existing experiment directory",
        "If you specified a specific experiment directory name, but this name "
        "is already given, you can force overwriting it with this flag. "
        "The previous experiment directory will be renamed."
    },
    {
        "machine_name",
        SCOREP_CONFIG_TYPE_STRING,
        &env_machine_name,
        NULL,
        SCOREP_DEFAULT_MACHINE_NAME, /* configure-provided default */
        "The machine name used in profile and trace output",
        "We suggest using a unique name, e.g., the fully qualified domain name. "
        "The default machine name was set at configure time (see the INSTALL "
        "file for customization options)."
    },
    {
        "executable",
        SCOREP_CONFIG_TYPE_PATH,
        &env_executable,
        NULL,
        "",
        "Executable of the application",
        "File name, preferably with full path, of the application's executable. "
        "This is a fallback if Score-P cannot determine the exetuable's name "
        "automatically. The name is required by some compiler adapters. They will "
        "complain if this environment variable is needed."
    },
    {
        "enable_system_tree_sequence_definitions",
        SCOREP_CONFIG_TYPE_BOOL,
        &env_system_tree_sequence,
        NULL,
        "false",
        "Use system tree sequence definitions",
        "Enables an internal system tree representation that specifies "
        "a sequence of system tree nodes with one record instead of "
        "creating one record per system tree node, location group or "
        "location. It is more scalable and has less memory requirements "
        "than single-node records. However, it costs inidividual names "
        "of nodes, but simply enumerates them based on types. "
        "Currently, system tree sequence definitions support only MPI "
        "(and trivially single-process) applications."
    },
    {
        "force_cfg_files",
        SCOREP_CONFIG_TYPE_BOOL,
        &force_cfg_files,
        NULL,
        "true",
        "Force the creation of experiment directory and configuration files",
        "If this is set to 'true' (which is the default), the experiment directory "
        " will be created along with some configuration files, even if no substrate "
        "writes data (i.e., profiling and tracing are disabled and no substrate "
        "plugin registered for writing).\n"
        "If this is set to 'false', the directory will only be created if any "
        "substrate actually writes data."
    },
    SCOREP_CONFIG_TERMINATOR
};

#if HAVE( SCOREP_DEBUG )
bool                               scorep_debug_unify;
static const SCOREP_ConfigVariable core_debug_confvars[] = {
    {
        "unify",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_debug_unify,
        NULL,
        "true",
        "Writes the pre-unified definitions also in the local definition trace files",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};
#endif


bool
SCOREP_Env_RunVerbose( void )
{
    assert( env_variables_initialized );
    return env_verbose;
}


bool
SCOREP_Env_DoTracing( void )
{
    assert( env_variables_initialized );
    return env_tracing;
}


bool
SCOREP_Env_DoProfiling( void )
{
    assert( env_variables_initialized );
    return env_profiling;
}


bool
SCOREP_Env_DoUnwinding( void )
{
    assert( env_variables_initialized );
    return env_unwinding;
}


uint64_t
SCOREP_Env_GetTotalMemory( void )
{
    assert( env_variables_initialized );
    return env_total_memory;
}


uint64_t
SCOREP_Env_GetPageSize( void )
{
    assert( env_variables_initialized );
    return env_page_size;
}

const char*
SCOREP_Env_GetExperimentDirectory( void )
{
    assert( env_variables_initialized );
    return env_experiment_directory;
}

bool
SCOREP_Env_DoOverwriteExperimentDirectory( void )
{
    assert( env_variables_initialized );
    return env_overwrite_experiment_directory;
}

const char*
SCOREP_Env_GetMachineName( void )
{
    assert( env_variables_initialized );
    return env_machine_name;
}

const char*
SCOREP_Env_GetExecutable( void )
{
    assert( env_variables_initialized );
    return env_executable;
}

bool
SCOREP_Env_DoForceCfgFiles( void )
{
    assert( env_variables_initialized );
    return force_cfg_files;
}

bool
SCOREP_Env_UseSystemTreeSequence( void )
{
    if ( !scorep_system_tree_seq_has_support_for() &&
         env_system_tree_sequence )
    {
        UTILS_WARNING( "Cannot use the system tree sequence definitions with "
                       "current inter-process communication paradigm. "
                       "Currently, system tree sequence definitions are only "
                       "supported for MPI and single-process applications. "
                       "Disable usage of system tree sequence definitions." );
        env_system_tree_sequence = false;
    }
    return env_system_tree_sequence;
}

void
SCOREP_RegisterAllConfigVariables( void )
{
    if ( env_variables_initialized )
    {
        return;
    }

    env_variables_initialized = true;

    SCOREP_ErrorCode error;
    error = SCOREP_ConfigRegister( "", core_enable_confvars );
    if ( SCOREP_SUCCESS != error )
    {
        UTILS_ERROR( error, "Can't register core environment variables" );
        _Exit( EXIT_FAILURE );
    }

    error = SCOREP_ConfigRegisterCond( "", core_enable_unwinding_confvars,
                                       HAVE_BACKEND_UNWINDING_SUPPORT );
    if ( SCOREP_SUCCESS != error )
    {
        UTILS_ERROR( error, "Can't register core environment variables" );
        _Exit( EXIT_FAILURE );
    }

    error = SCOREP_ConfigRegister( "", core_confvars );
    if ( SCOREP_SUCCESS != error )
    {
        UTILS_ERROR( error, "Can't register core environment variables" );
        _Exit( EXIT_FAILURE );
    }

#if HAVE( SCOREP_DEBUG )
    error = SCOREP_ConfigRegister( "debug", core_debug_confvars );
    if ( SCOREP_SUCCESS != error )
    {
        UTILS_ERROR( error, "Can't register debug environment variables" );
        _Exit( EXIT_FAILURE );
    }
#endif

    SCOREP_Profile_Register();
    SCOREP_Tracing_Register();
    SCOREP_OA_Register();
    SCOREP_Filtering_Register();
    SCOREP_Timer_Register();
    scorep_subsystems_register();
}
