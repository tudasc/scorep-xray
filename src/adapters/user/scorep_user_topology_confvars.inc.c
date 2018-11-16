/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Forschungszentrum Juelich GmbH, Germany
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

#include <SCOREP_Config.h>

/**
   Stores whether recording of User defined topologies is enabled
 */
bool scorep_user_enable_topologies;


static const SCOREP_ConfigVariable scorep_user_topology_confvars[] = {
    {
        "user",
        SCOREP_CONFIG_TYPE_BOOL,
        &scorep_user_enable_topologies,
        NULL,
        "true",
        "Record topologies provided by user instrumentation",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};
