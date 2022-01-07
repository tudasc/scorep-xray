/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @brief
 */

#include <config.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Hashtab.h>
#include <SCOREP_Mutex.h>
#include <scorep_subsystem_management.h>
#include <scorep_substrates_definition.h>

SCOREP_LocationGroupHandle
SCOREP_AcceleratorMgmt_CreateContext( SCOREP_SystemTreeNodeHandle deviceHandle,
                                      const char*                 name )
{
    return SCOREP_Definitions_NewLocationGroup(
        name,
        deviceHandle,
        SCOREP_LOCATION_GROUP_TYPE_ACCELERATOR,
        SCOREP_GetProcessLocationGroup() );
}


static SCOREP_ErrorCode
accelerator_mgmt_subsystem_register( size_t subsystemId )
{
    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
accelerator_mgmt_subsystem_init( void )
{
    return SCOREP_SUCCESS;
}


static void
accelerator_mgmt_subsystem_finalize( void )
{
}


/** Initializes the location specific data */
static SCOREP_ErrorCode
accelerator_mgmt_subsystem_init_location( SCOREP_Location* location,
                                          SCOREP_Location* parent )
{
    if ( SCOREP_Location_GetType( location ) != SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        return SCOREP_SUCCESS;
    }

    return SCOREP_SUCCESS;
}


const SCOREP_Subsystem SCOREP_Subsystem_AcceleratorManagement =
{
    .subsystem_name          = "Accelerator Management",
    .subsystem_register      = &accelerator_mgmt_subsystem_register,
    .subsystem_init          = &accelerator_mgmt_subsystem_init,
    .subsystem_finalize      = &accelerator_mgmt_subsystem_finalize,
    .subsystem_init_location = &accelerator_mgmt_subsystem_init_location,
};
