/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014,
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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include <SCOREP_Shmem.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Memory.h>
#include <scorep_status.h>
#include <scorep_ipc.h>
#include <scorep_unify_helpers.h>

#include <UTILS_Debug.h>

#define WIN_WORLD_NAME "All PEs"
#define WIN_SELF_NAME  "Self PE"

/**
 * Define group of all SHMEM locations.
 */
void
scorep_shmem_define_shmem_locations( void )
{
    /* Array of members used for all group definitions */
    uint32_t* members =
        calloc( scorep_shmem_number_of_pes, sizeof( *members ) );
    UTILS_ASSERT( members );

    /*
     * Define the list of locations which are SHMEM processing elements.
     *
     * This needs to be called early, so that the resulting definition
     * is before any other group definition of type SCOREP_GROUP_SHMEM_GROUP.
     */
    for ( uint32_t i = 0; i < scorep_shmem_number_of_pes; i++ )
    {
        members[ i ] = i;
    }
    SCOREP_Definitions_NewGroupFrom32( SCOREP_GROUP_SHMEM_LOCATIONS,
                                       "",
                                       scorep_shmem_number_of_pes,
                                       members );
    free( members );
}


/* *********************************************************************
 * Post-unify actions
 * ****************************************************************** */

/**
 * This is a pre-unify function and contains definition and
 * mapping of RMA windows and communicator related to SHMEM.
 */
void
scorep_shmem_define_shmem_group( void )
{
    uint32_t* members =
        calloc( scorep_shmem_number_of_pes, sizeof( *members ) );
    UTILS_ASSERT( members );

    /* Create SHMEM group for self-like communicator */
    SCOREP_GroupHandle self_group =
        SCOREP_Definitions_NewGroup( SCOREP_GROUP_SHMEM_SELF,
                                     "",
                                     0,
                                     NULL );

    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_BEGIN(
        &scorep_shmem_pe_groups,
        InterimCommunicator,
        SCOREP_Memory_GetLocalDefinitionPageManager() )
    {
        if ( definition->paradigm_type != SCOREP_PARADIGM_SHMEM )
        {
            /* Unlikely, as this is our own definition manager entry */
            continue;
        }

        scorep_shmem_comm_definition_payload* payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( handle );

        if ( payload->pe_size == 1 )
        {
            continue;
        }

        /* Create group for this PE group */
        for ( uint32_t i = 0; i < payload->pe_size; i++ )
        {
            /*
             * Generate array of PE ids which are member of this group
             */
            members[ i ] = payload->pe_start + ( i * ( 1 << payload->log_pe_stride ) );
        }
        SCOREP_GroupHandle group =
            SCOREP_Definitions_NewGroupFrom32( SCOREP_GROUP_SHMEM_GROUP,
                                               "",
                                               payload->pe_size,
                                               members );

        definition->unified =
            SCOREP_Definitions_NewCommunicator( group,
                                                SCOREP_INVALID_STRING,
                                                SCOREP_INVALID_COMMUNICATOR,
                                                0 );
    }
    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_END();
    free( members );

    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_BEGIN(
        &scorep_shmem_pe_groups,
        InterimCommunicator,
        SCOREP_Memory_GetLocalDefinitionPageManager() )
    {
        if ( definition->paradigm_type != SCOREP_PARADIGM_SHMEM )
        {
            /* Unlikely, as this is our own definition manager entry */
            continue;
        }

        scorep_shmem_comm_definition_payload* payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( handle );

        if ( payload->pe_size != 1 )
        {
            continue;
        }

        /* Create the SHMEM communicator with this group */
        definition->unified =
            SCOREP_Definitions_NewCommunicator( self_group,
                                                SCOREP_INVALID_STRING,
                                                SCOREP_INVALID_COMMUNICATOR,
                                                0 );
    }
    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_END();
}

void
scorep_shmem_finalize_shmem_mapping( void )
{
    scorep_unify_helper_create_interim_comm_mapping( &scorep_shmem_pe_groups );
}
