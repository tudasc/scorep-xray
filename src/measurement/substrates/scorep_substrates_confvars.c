/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */

#include <config.h>

#include <SCOREP_Subsystem.h>

#include "scorep_substrates_plugins_confvars.inc.c"


static SCOREP_ErrorCode
substrates_subsystem_register( size_t subsystem_id )
{
    SCOREP_ConfigRegisterCond( "substrate",
                               scorep_substrates_plugins_confvars,
                               HAVE_BACKEND_DLFCN_SUPPORT );
    return SCOREP_SUCCESS;
}

/**
 * Implementation of the metric service initialization/finalization struct
 */
const SCOREP_Subsystem SCOREP_Subsystem_Substrates =
{
    .subsystem_name     = "SUBSTRATE (config variables only)",
    .subsystem_register = &substrates_subsystem_register,
};
