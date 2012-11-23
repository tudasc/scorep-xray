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
 * @file       src/measurement/profiling/scorep_profile_confvars.inc.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

/**
   Initial value for scorep_profile.max_callpath_depth, which may change over
   time.
 */
uint64_t scorep_profile_max_callpath_depth;

/**
   Stores the configuration of the hash table size.
 */
uint64_t scorep_profile_task_table_size;

/**
   Contains the basename for profile files.
 */
char* scorep_profile_basename = NULL;

/**
   Stores the configuration setting for output format.
 */
uint64_t scorep_profile_output_format;

/**
   Bitset table for output format string configuration.
 */
static const SCOREP_ConfigType_SetEntry scorep_profile_format_table[] = {
    { "none",         SCOREP_Profile_OutputNone         },
    { "tau_snapshot", SCOREP_Profile_OutputTauSnapshot  },
    { "cube4",        SCOREP_Profile_OutputCube4        },
    { "default",      SCOREP_Profile_OutputDefault      },
    { NULL,           0                                 }
};

/**
   Configuration variable registration structures for the profiling system.
 */
static SCOREP_ConfigVariable scorep_profile_configs[] = {
    {
        "task_table_size",
        SCOREP_CONFIG_TYPE_NUMBER,
        &scorep_profile_task_table_size,
        NULL,
        "64",
        "Size of the task tracing table",
        "Size of the task tracing table. Best performance is obtained, if the\n"
        "table size matches the number of tasks that are active at once. If\n"
        "your program does not use tasks, you may set this value to zero, to\n"
        "save some memory."
    },
    {
        "max_callpath_depth",
        SCOREP_CONFIG_TYPE_NUMBER,
        &scorep_profile_max_callpath_depth,
        NULL,
        "30",
        "Maximum depth of the calltree",
        ""
    },
/*
    {
        "max_callpath_num",
        SCOREP_CONFIG_TYPE_NUMBER,
        &scorep_profile_max_callpath_num,
        NULL,
        "1000000000",
        "Maximum number of nodes in the calltree",
        ""
    },
 */
    {
        "base_name",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_profile_basename,
        NULL,
        "profile",
        "Base for construction of the profile filename",
        "String which is used as based to create the filenames for the profile files."
    },
    {
        "format",
        SCOREP_CONFIG_TYPE_BITSET,
        &scorep_profile_output_format,
        ( void* )scorep_profile_format_table,
        "default",
        "Profile output format",
        "Sets the output format for the profile.\n"
        "The following formats are supported:\n"
        "  none:         No profile output. This does not disable profile recording.\n"
        "  tau_snapshot: Tau snapshot format\n"
        "  cube4:        Cube4 format\n"
        "  default:      Default format. If Cube4 is supported, Cube4 is the default\n"
        "                else the Tau snapshot format is default"
    },
    SCOREP_CONFIG_TERMINATOR
};

void
SCOREP_Profile_Register( void )
{
    SCOREP_ConfigRegister( "profiling", scorep_profile_configs );
}
