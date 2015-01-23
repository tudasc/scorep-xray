/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 */

#include <config.h>

#include <SCOREP_Types.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Memory.h>
#include <scorep_status.h>

#include <scorep_unify_helpers.h>

#include <scorep_opencl.h>
#include <scorep_opencl_config.h>

void
scorep_opencl_define_locations( void )
{
    size_t   i      = 0;
    uint32_t offset = scorep_unify_helper_define_comm_locations(
        SCOREP_GROUP_OPENCL_LOCATIONS,
        "OPENCL", scorep_opencl_global_location_number,
        scorep_opencl_global_location_ids );

    /* add the offset */
    for ( i = 0; i < scorep_opencl_global_location_number; i++ )
    {
        scorep_opencl_global_location_ids[ i ] = i + offset;
    }

    SCOREP_GroupHandle group_handle = SCOREP_Definitions_NewGroup(
        SCOREP_GROUP_OPENCL_GROUP,
        "OPENCL_GROUP",
        scorep_opencl_global_location_number,
        scorep_opencl_global_location_ids );

    SCOREP_CommunicatorHandle communicator_handle =
        SCOREP_Definitions_NewCommunicator(
            group_handle,
            "",
            SCOREP_INVALID_COMMUNICATOR );

    SCOREP_RmaWindowHandle window_handle = SCOREP_Definitions_NewRmaWindow(
        "",
        communicator_handle );

    SCOREP_LOCAL_HANDLE_DEREF( scorep_opencl_interim_communicator_handle,
                               InterimCommunicator )->unified =
        communicator_handle;

    SCOREP_LOCAL_HANDLE_DEREF( scorep_opencl_interim_window_handle, InterimRmaWindow )->unified =
        window_handle;
}

void
scorep_opencl_define_group( void )
{
    if ( 0 == SCOREP_Status_GetRank() )
    {
        /* Count the number of OpenCL locations */
        uint32_t total_number_of_opencl_locations = 0;
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                             Location,
                                                             location )
        {
            if ( definition->location_type != SCOREP_LOCATION_TYPE_GPU )
            {
                continue;
            }
            total_number_of_opencl_locations++;
        }
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

        if ( total_number_of_opencl_locations )
        {
            /* collect the global location ids for the OpenCL locations */
            uint64_t opencl_locations[ total_number_of_opencl_locations ];
            total_number_of_opencl_locations = 0;
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                                 Location,
                                                                 location )
            {
                if ( definition->location_type != SCOREP_LOCATION_TYPE_GPU )
                {
                    continue;
                }
                opencl_locations[ total_number_of_opencl_locations++ ]
                    = definition->global_location_id;
            }
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

            /* define the group of locations for all OpenCL locations */
            SCOREP_Definitions_NewUnifiedGroup( SCOREP_GROUP_LOCATIONS,
                                                "OPENCL_GROUP",
                                                total_number_of_opencl_locations,
                                                opencl_locations );
        }
    }

    if ( scorep_opencl_record_memcpy )
    {
        scorep_local_definition_manager.interim_communicator.mapping[
            SCOREP_LOCAL_HANDLE_DEREF( scorep_opencl_interim_communicator_handle,
                                       InterimCommunicator )->sequence_number ] =
            scorep_local_definition_manager.communicator.mapping[
                SCOREP_LOCAL_HANDLE_DEREF( SCOREP_LOCAL_HANDLE_DEREF(
                                               scorep_opencl_interim_communicator_handle,
                                               InterimCommunicator )->unified,
                                           Communicator )->sequence_number ];

        scorep_local_definition_manager.interim_rma_window.mapping[
            SCOREP_LOCAL_HANDLE_DEREF( scorep_opencl_interim_window_handle,
                                       InterimRmaWindow )->sequence_number ] =
            scorep_local_definition_manager.rma_window.mapping[
                SCOREP_LOCAL_HANDLE_DEREF( SCOREP_LOCAL_HANDLE_DEREF(
                                               scorep_opencl_interim_window_handle,
                                               InterimRmaWindow )->unified, RmaWindow )->sequence_number ];
    }
}
