/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015-2016,
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
 * This file contains the a mockup of the low-level substrate plugins
 *
 */
#include <config.h>
#include <string.h>

#include <UTILS_Debug.h>
#include <UTILS_Error.h>
#include <UTILS_CStr.h>

#include <SCOREP_Substrates_Management.h>
#include <scorep_substrates_definition.h>

/* There is no support for dynamic loading, so we provide stub functions */
uint32_t
SCOREP_Substrate_Plugins_GetSubstrateCallbacks( SCOREP_Substrates_Mode mode, int current_plugin, SCOREP_Substrates_Callback** returned_callbacks, uint32_t* current_array_length )
{
    return 0;
}

uint32_t
SCOREP_Substrate_Plugins_GetSubstrateMgmtCallbacks( int current_plugin, SCOREP_Substrates_Callback** returned_callbacks )
{
    return 0;
}

void
SCOREP_Substrate_Plugins_EarlyInit( void )
{
}

void
SCOREP_Substrate_Plugins_Register( void )
{
}

int
SCOREP_Substrate_Plugins_GetNumberRegisteredPlugins( void )
{
    return 0;
}
void
SCOREP_Substrate_Plugins_InitLocationData( SCOREP_Location* location )
{
}
