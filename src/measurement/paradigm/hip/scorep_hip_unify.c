/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  Implementation of HIP unification routines.
 */

#include <config.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME HIP
#include <UTILS_Debug.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_RuntimeManagement.h>
#include <scorep_status.h>

#include <string.h>
#include <stdbool.h>

#include "scorep_hip.h"
#include "scorep_unify_helpers.h"

void
scorep_hip_unify_pre( void )
{
    UTILS_BUG_ON( scorep_hip_global_location_count != 0 && scorep_hip_global_location_ids == NULL,
                  "Invalid parameters for HIP unification." );

    /* This is a collective, all processes need to participate */
    uint32_t offset = scorep_unify_helper_define_comm_locations(
        SCOREP_GROUP_HIP_LOCATIONS,
        "HIP", scorep_hip_global_location_count,
        scorep_hip_global_location_ids );

    if ( scorep_hip_global_location_count == 0 )
    {
        return;
    }

    /* add the offset */
    for ( size_t i = 0; i < scorep_hip_global_location_count; i++ )
    {
        scorep_hip_global_location_ids[ i ] = i + offset;
    }

    SCOREP_GroupHandle group_handle = SCOREP_Definitions_NewGroup(
        SCOREP_GROUP_HIP_GROUP,
        "HIP_GROUP",
        scorep_hip_global_location_count,
        scorep_hip_global_location_ids );

    SCOREP_LOCAL_HANDLE_DEREF( scorep_hip_interim_communicator_handle,
                               InterimCommunicator )->unified =
        SCOREP_Definitions_NewCommunicator(
            group_handle,
            SCOREP_INVALID_STRING,
            SCOREP_INVALID_COMMUNICATOR,
            0,
            SCOREP_COMMUNICATOR_FLAG_NONE );
}

void
scorep_hip_unify_post( void )
{
    if ( 0 == SCOREP_Status_GetRank() )
    {
        /* Count the number of HIP locations */
        uint32_t total_number_of_hip_locations = 0;
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                             Location,
                                                             location )
        {
            if ( definition->location_type != SCOREP_LOCATION_TYPE_GPU ||
                 definition->paradigm      != SCOREP_PARADIGM_HIP )
            {
                continue;
            }

            total_number_of_hip_locations++;
        }
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

        if ( total_number_of_hip_locations )
        {
            /* collect the global location ids for the HIP locations */
            uint64_t hip_locations[ total_number_of_hip_locations ];
            total_number_of_hip_locations = 0;
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                                 Location,
                                                                 location )
            {
                if ( definition->location_type != SCOREP_LOCATION_TYPE_GPU ||
                     definition->paradigm      != SCOREP_PARADIGM_HIP )
                {
                    continue;
                }
                hip_locations[ total_number_of_hip_locations++ ]
                    = definition->global_location_id;
            }
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

            UTILS_DEBUG( "Created HIP group with %u locations", total_number_of_hip_locations );
            /* define the group of locations for all HIP locations */
            SCOREP_Definitions_NewUnifiedGroup( SCOREP_GROUP_LOCATIONS,
                                                "HIP_GROUP",
                                                total_number_of_hip_locations,
                                                hip_locations );
        }
    }
}
