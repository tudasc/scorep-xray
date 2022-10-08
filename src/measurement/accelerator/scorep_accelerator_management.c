/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
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

#include <UTILS_Mutex.h>

#include <scorep_status.h>
#include <scorep_subsystem_management.h>
#include <scorep_substrates_definition.h>
#include <scorep_ipc.h>


static SCOREP_ParameterHandle callsite_parameter;


SCOREP_LocationGroupHandle
SCOREP_AcceleratorMgmt_CreateContext( SCOREP_SystemTreeNodeHandle deviceHandle,
                                      const char*                 name )
{
    SCOREP_Status_OnAccUsage();

    return SCOREP_Definitions_NewLocationGroup(
        name,
        deviceHandle,
        SCOREP_LOCATION_GROUP_TYPE_ACCELERATOR,
        SCOREP_GetProcessLocationGroup() );
}


SCOREP_ParameterHandle
SCOREP_AcceleratorMgmt_GetCallsiteParameter( void )
{
    /* no locking needed, in the worst case the parameter exists twice
     * but all will be unified to just one */
    if ( callsite_parameter == SCOREP_INVALID_PARAMETER )
    {
        callsite_parameter = SCOREP_Definitions_NewParameter( "callsite id", SCOREP_PARAMETER_UINT64 );
    }

    return callsite_parameter;
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


static SCOREP_ErrorCode
accelerator_mgmt_subsystem_pre_unify( void )
{
    /* ensure that the root rank has the local definition, as long as at least
     * one rank has it too */
    int flag = callsite_parameter != SCOREP_INVALID_PARAMETER;
    int flag_max;
    SCOREP_Ipc_Reduce( &flag, &flag_max, 1, SCOREP_IPC_INT, SCOREP_IPC_MAX, 0 );
    if ( 0 == SCOREP_Status_GetRank() && flag_max )
    {
        SCOREP_AcceleratorMgmt_GetCallsiteParameter();
    }

    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
accelerator_mgmt_subsystem_post_unify( void )
{
    if ( 0 != SCOREP_Status_GetRank() || callsite_parameter == SCOREP_INVALID_PARAMETER )
    {
        return SCOREP_SUCCESS;
    }

    SCOREP_ParameterHandle unified_callsite_parameter = SCOREP_LOCAL_HANDLE_DEREF( callsite_parameter, Parameter )->unified;
    uint32_t*              used_callsites             = NULL;
    uint32_t               used_callsites_size        = 0;
    uint32_t               used_callsites_capacity    = 0;
    uint32_t               callsite_hash_collisions   = 0;

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                         Callpath,
                                                         callpath )
    {
        for ( uint32_t i = 0; i < definition->number_of_parameters; i++ )
        {
            /* compare against unified handle */
            if ( definition->parameters[ i ].parameter_handle != unified_callsite_parameter )
            {
                continue;
            }

            /* Data management happens on the launch side as it will always precede the execution side.
             * Check for duplicates only on the launch side where the value is created.
             * Callsite hashes should be unique across paradigms as they are bound to respective
             * launch commands and should be on different callpaths anyhow.
             */
            int64_t parameter_int_value = definition->parameters[ i ].parameter_value.integer_value;
            UTILS_BUG_ON( parameter_int_value  < 0 || parameter_int_value > UINT32_MAX,
                          "Callsite parameter out of range for uint32_t!" );

            if ( SCOREP_RegionHandle_GetType( definition->region_handle ) == SCOREP_REGION_KERNEL_LAUNCH )
            {
                /* kernel launch. */
                bool no_duplicates = true;
                for ( uint32_t j = 0; j < used_callsites_size; j++ )
                {
                    if ( used_callsites[ j ] == ( uint32_t )parameter_int_value )
                    {
                        /* Since every callpath from the definitions should have a distinct callsite (if any)
                         * the appearance of a duplicate indicates a hash collision in the callpath identification
                         * during runtime.
                         * This check provides a warning that some callsites might have been mistakenly unified
                         * and allows a correctness check. */
                        UTILS_WARNING( "Debug information: Callsite hash collision detected"
                                       " (reoccuring call site id hash: %, current callpath: %u) Please report to <support@score-p.org>.",
                                       used_callsites[ j ], definition->sequence_number );
                        no_duplicates = false;
                        callsite_hash_collisions++;
                        break;
                    }
                }
                if ( no_duplicates )
                {
                    if ( used_callsites_size == used_callsites_capacity )
                    {
                        /* Dynamic allocation in batches of 64 */
                        used_callsites_capacity += 64;
                        used_callsites           = realloc( used_callsites, sizeof( uint32_t ) * ( used_callsites_capacity ) );
                        UTILS_ASSERT( used_callsites != NULL );
                    }
                    /* If the callsite is new, which should be the correct default, add the new id to the list. */
                    used_callsites[ used_callsites_size ] = ( uint32_t )parameter_int_value;
                    used_callsites_size++;
                }
            }
            else if ( SCOREP_RegionHandle_GetType( definition->region_handle ) == SCOREP_REGION_KERNEL )
            {
                /* Kernel execution. */
                bool no_match = true;
                for ( uint32_t j = 0; j < used_callsites_size; j++ )
                {
                    if ( used_callsites[ j ] == ( uint32_t )parameter_int_value )
                    {
                        no_match = false;
                        break;
                    }
                }
                if ( no_match )
                {
                    /* This should not be happening, as every callsite hash should have been registered. */
                    UTILS_WARNING( "Call site hash %u could not be matched to any callsite! (current callpath:%u)",
                                   ( uint32_t )parameter_int_value, definition->sequence_number );
                }
            }
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    if ( callsite_hash_collisions > 0 )
    {
        UTILS_WARNING( "Unfortunately, Score-P was not able to generate unique call site identifiers;"
                       " %u collisions were detected. Please report to <support@score-p.org>.",
                       callsite_hash_collisions );
    }

    free( used_callsites );

    return SCOREP_SUCCESS;
}


const SCOREP_Subsystem SCOREP_Subsystem_AcceleratorManagement =
{
    .subsystem_name          = "Accelerator Management",
    .subsystem_register      = &accelerator_mgmt_subsystem_register,
    .subsystem_init          = &accelerator_mgmt_subsystem_init,
    .subsystem_finalize      = &accelerator_mgmt_subsystem_finalize,
    .subsystem_init_location = &accelerator_mgmt_subsystem_init_location,
    .subsystem_pre_unify     = &accelerator_mgmt_subsystem_pre_unify,
    .subsystem_post_unify    = &accelerator_mgmt_subsystem_post_unify,
};
