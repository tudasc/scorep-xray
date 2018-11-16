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
#define SCOREP_DEBUG_MODULE_NAME TOPOLOGIES
#include <UTILS_Debug.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Config.h>

#include "scorep_topologies_confvars.inc.c"

static SCOREP_ErrorCode
topologies_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG_ENTRY( "Register Topology config variables" );
    SCOREP_ConfigRegister( "topology", scorep_topology_confvars );
    return SCOREP_SUCCESS;
}

const SCOREP_Subsystem SCOREP_Subsystem_Topologies =
{
    .subsystem_name     = "Topologies (config variables only)",
    .subsystem_register = &topologies_subsystem_register,
};
