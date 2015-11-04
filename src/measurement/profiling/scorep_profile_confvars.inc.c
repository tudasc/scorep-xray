/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012, 2014
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Darmstadt, Germany
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
#include <SCOREP_Profile.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/**
   Number of foreign task objects that are collected before they are put into
   the common exchange buffer.
 */
uint64_t scorep_profile_task_exchange_num;

/**
   Initial value for scorep_profile.max_callpath_depth, which may change over
   time.
 */
uint64_t scorep_profile_max_callpath_depth;

/**
   Contains the basename for profile files.
 */
char* scorep_profile_basename = NULL;

/**
   Stores the configuration setting for output format.
 */
uint64_t scorep_profile_output_format;

/**
   Stores the number of clusters
 */
uint64_t scorep_profile_cluster_count;

/**
   Stores the cluster mode
 */
uint64_t scorep_profile_cluster_mode;

/**
   Stores the name of the cluster region-
 */
char* scorep_profile_clustered_region;

/**
   Stores whether clustering is enabled
 */
bool scorep_profile_enable_clustering;

/**
   Stores whether the profile shall write profile.core files on errors
 */
bool scorep_profile_enable_core_files;


/**
   Bitset table for output format string configuration.
 */
static const SCOREP_ConfigType_SetEntry scorep_profile_format_table[] = {
    { "tau_snapshot", SCOREP_PROFILE_OUTPUT_TAU_SNAPSHOT },
    { "cube4",        SCOREP_PROFILE_OUTPUT_CUBE4        },
    { "cube_tuple",   SCOREP_PROFILE_OUTPUT_CUBE_TUPLE   },
    { "default",      SCOREP_PROFILE_OUTPUT_DEFAULT      },
    { NULL,           0                                  }
};

/**
   Configuration variable registration structures for the profiling system.
 */
static SCOREP_ConfigVariable scorep_profile_configs[] = {
    {
        "task_exchange_num",
        SCOREP_CONFIG_TYPE_NUMBER,
        &scorep_profile_task_exchange_num,
        NULL,
        "1024",
        "Number of foreign task objects that are collected before they are put into "
        "the common task object exchange buffer.",
        "The profiling creates a record for every task instance that is running. "
        "To avoid locking, the required memory is taken from a preallocated memory "
        "block. Each thread has its own memory block. On task completion, the created "
        "object can be reused by other tasks. However, if tasks migrate, the data "
        "structure migrates with them. Thus, if there is an imbalance in the migration "
        "from a source thread that starts the execution of tasks towards a sink thread "
        "that completes the tasks, the source thread may continually creating new "
        "task objects while in the sink, released task objects are collected. Thus, "
        "if the sink collected a certain number of tasks it should trigger a backflow "
        "of its collected task objects. However, this requires locking which should be "
        "avoided as much as possible. Thus, we do not want the locking happen on every "
        "migrated task, but only if a certain imbalance occurs. This environment "
        "variable determines the number of igrated task instances that must be "
        "collected before the backflow is triggered."
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
        "  cube4:        Stores the sum for every metric per callpath in Cube4 format.\n"
        "  cube_tuple:   Stores an extended set of statistics in Cube4 format.\n"
        "  default:      Default format. If Cube4 is supported, Cube4 is the default\n"
        "                else the Tau snapshot format is default"
    },
    {
        "enable_clustering",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_profile_enable_clustering,
        NULL,
        "true",
        "Enable clustering",
        ""
    },
    {
        "cluster_count",
        SCOREP_CONFIG_TYPE_SIZE,
        &scorep_profile_cluster_count,
        NULL,
        "64",
        "maximum cluster count for iteration clustering.",
        "maximum cluster count for iteration clustering."
    },
    {
        "clustering_mode",
        SCOREP_CONFIG_TYPE_SIZE,
        &scorep_profile_cluster_mode,
        NULL,
        "1",
        "Specifies the level of strictness when comparing call trees for equivalence.",
        "Specifies the level of strictness when comparing call trees for equivalence."
    },
    {
        "clustered_region",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_profile_clustered_region,
        NULL,
        "",
        "Name of the clustered region",
        "The clustering can only cluster one dynamic region. If more than one "
        "dynamic region are defined by the user, the region is clustered "
        "which is exited first. If another region should be clustered instead "
        "you can specify the region name in this variable. If the variable is "
        "unset or empty, the first exited dynamic region is clustered."
    },
    {
        "enable_core_files",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_profile_enable_core_files,
        NULL,
        "false",
        "Write .core files if an error occured.",
        "If an error occures inside the profiling system, the profiling is disabled. "
        "For debugging reasons, it might be feasible to get the state of the local "
        "stack at these points. It is not recommended to enable this feature for "
        "large scale measurements."
    },
    SCOREP_CONFIG_TERMINATOR
};

void
SCOREP_Profile_Register( void )
{
    SCOREP_ConfigRegister( "profiling", scorep_profile_configs );
}
