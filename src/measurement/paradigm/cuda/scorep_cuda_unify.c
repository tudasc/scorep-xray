/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
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

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME CUDA
#include <UTILS_Debug.h>

#include <SCOREP_Types.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Events.h>
#include <SCOREP_Config.h>
#include <scorep_status.h>

#include "scorep_unify_helpers.h"

#include <scorep_cuda.h>

void
scorep_cuda_define_cuda_locations( void )
{
    size_t   i      = 0;
    uint32_t offset = scorep_unify_helper_define_comm_locations(
        SCOREP_GROUP_CUDA_LOCATIONS,
        "CUDA", scorep_cuda_global_location_number,
        scorep_cuda_global_location_ids );

    /* add the offset */
    for ( i = 0; i < scorep_cuda_global_location_number; i++ )
    {
        scorep_cuda_global_location_ids[ i ] = i + offset;
    }

    SCOREP_GroupHandle group_handle = SCOREP_Definitions_NewGroup(
        SCOREP_GROUP_CUDA_GROUP,
        "CUDA_GROUP",
        scorep_cuda_global_location_number,
        scorep_cuda_global_location_ids );

    SCOREP_LOCAL_HANDLE_DEREF( scorep_cuda_interim_communicator_handle,
                               InterimCommunicator )->unified =
        SCOREP_Definitions_NewCommunicator(
            group_handle,
            SCOREP_INVALID_STRING,
            SCOREP_INVALID_COMMUNICATOR,
            0 );
}

void
scorep_cuda_define_cuda_group( void )
{
    if ( 0 == SCOREP_Status_GetRank() )
    {
        /* Count the number of CUDA locations */
        uint32_t total_number_of_cuda_locations = 0;
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                             Location,
                                                             location )
        {
            if ( definition->location_type != SCOREP_LOCATION_TYPE_GPU )
            {
                continue;
            }
            total_number_of_cuda_locations++;
        }
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

        if ( total_number_of_cuda_locations )
        {
            /* collect the global location ids for the CUDA locations */
            uint64_t cuda_locations[ total_number_of_cuda_locations ];
            total_number_of_cuda_locations = 0;
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                                 Location,
                                                                 location )
            {
                if ( definition->location_type != SCOREP_LOCATION_TYPE_GPU )
                {
                    continue;
                }
                cuda_locations[ total_number_of_cuda_locations++ ]
                    = definition->global_location_id;
            }
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

            /* define the group of locations for all CUDA locations */
            SCOREP_Definitions_NewUnifiedGroup( SCOREP_GROUP_LOCATIONS,
                                                "CUDA_GROUP",
                                                total_number_of_cuda_locations,
                                                cuda_locations );
        }
    }
}
