/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include <config.h>
#include <SCOREP_Definitions.h>


/**
   Stores whether recording of platform/hardware topologies is enabled
 */
bool scorep_topologies_enable_platform;

/**
   Stores whether recording of the Process x Threads topology is enabled
 */
bool scorep_topologies_enable_process;


static const SCOREP_ConfigVariable scorep_topology_confvars[] = {
    {
        "platform",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_topologies_enable_platform,
        NULL,
        "true",
        "Record hardware topology information for this platform, if available.",
        ""
    },
    {
        "process",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_topologies_enable_process,
        NULL,
        "true",
        "Record the Process x Thread topology.",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};
