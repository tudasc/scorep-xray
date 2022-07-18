/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2022,
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
 * @file
 *
 *
 */


#include <config.h>

#include "scorep_unify_helpers.h"

#include <stdlib.h>
#include <stdio.h>

#include <UTILS_Error.h>

#include <SCOREP_Definitions.h>
#include <scorep_ipc.h>

uint32_t
scorep_unify_helper_define_comm_locations( SCOREP_GroupType type,
                                           const char*      name,
                                           uint64_t         numberOfMembers,
                                           const uint64_t*  localMembers )
{
    uint32_t size                         = SCOREP_Ipc_GetSize();
    uint32_t rank                         = SCOREP_Ipc_GetRank();
    int*     number_of_locations_per_rank = NULL;
    int*     offsets                      = NULL;

    if ( rank == 0 )
    {
        number_of_locations_per_rank =
            calloc( 2 * size, sizeof( *number_of_locations_per_rank ) );
        UTILS_ASSERT( number_of_locations_per_rank );
        offsets = number_of_locations_per_rank + size;
    }

    /* Collect the number of locations per rank into rank 0 */
    int number_of_members = numberOfMembers;
    SCOREP_Ipc_Gather( &number_of_members,
                       number_of_locations_per_rank,
                       1,
                       SCOREP_IPC_INT,
                       0 );

    /* Calculate the exclusive prefix sum for each tank */
    uint32_t total_number_of_locations = 0;
    if ( rank == 0 )
    {
        for ( uint32_t i = 0; i < size; i++ )
        {
            offsets[ i ]               = total_number_of_locations;
            total_number_of_locations += number_of_locations_per_rank[ i ];
        }
    }

    /* Distribute the per-rank offsets */
    int offset_to_global;
    SCOREP_Ipc_Scatter( offsets,
                        &offset_to_global,
                        1,
                        SCOREP_IPC_INT,
                        0 );

    /* Gather all location ids from the ranks. */
    uint64_t* all_locations = NULL;
    if ( 0 == rank )
    {
        all_locations = calloc( total_number_of_locations,
                                sizeof( *all_locations ) );
        UTILS_ASSERT( all_locations );
    }

    SCOREP_Ipc_Gatherv( ( void* )localMembers,
                        number_of_members,
                        all_locations,
                        number_of_locations_per_rank,
                        SCOREP_IPC_UINT64_T,
                        0 );
    free( number_of_locations_per_rank );

    if ( 0 == rank )
    {
        if ( total_number_of_locations != 0 )
        {
            /*
             * Don't define this in the unified manager, it disturbs greatly
             * the string mapping
             */
            SCOREP_Definitions_NewGroup( type,
                                         name,
                                         total_number_of_locations,
                                         all_locations );
        }
        free( all_locations );
    }

    return offset_to_global;
}

void
scorep_unify_helper_get_number_of_cpu_locations( int*      numberOfLocationsPerRank,
                                                 uint32_t* totalNumberOfLocations,
                                                 int*      maxLocationsPerRank )
{
    UTILS_ASSERT( totalNumberOfLocations );
    UTILS_ASSERT( maxLocationsPerRank );
    UTILS_ASSERT( numberOfLocationsPerRank );

    int local_number_of_cpu_locations = 0;

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         Location,
                                                         location )
    {
        if ( definition->location_type != SCOREP_LOCATION_TYPE_CPU_THREAD )
        {
            continue;
        }
        local_number_of_cpu_locations++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    uint32_t size = SCOREP_Ipc_GetSize();

    SCOREP_Ipc_Allgather( &local_number_of_cpu_locations,
                          numberOfLocationsPerRank,
                          1,
                          SCOREP_IPC_INT );

    *totalNumberOfLocations = 0;
    *maxLocationsPerRank    = 0;
    /* calculate the total sum and the maximum of locations per rank
       required for cases with different numbers of threads per rank */
    for ( uint32_t i = 0; i < size; i++ )
    {
        *totalNumberOfLocations += ( uint32_t )numberOfLocationsPerRank[ i ];
        if ( *maxLocationsPerRank < numberOfLocationsPerRank[ i ] )
        {
            *maxLocationsPerRank = numberOfLocationsPerRank[ i ];
        }
    }
}

void
scorep_unify_helper_exchange_all_cpu_locations( uint64_t* allLocations,
                                                uint32_t  totalNumberOfLocations,
                                                int*      numberOfLocationsPerRank )
{
    UTILS_ASSERT( allLocations );
    UTILS_ASSERT( numberOfLocationsPerRank );

    uint64_t cpu_locations[ numberOfLocationsPerRank[ SCOREP_Ipc_GetRank() ] ];

    /* collect the global location ids for the cpu locations */
    uint32_t local_number_of_cpu_locations = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         Location,
                                                         location )
    {
        if ( definition->location_type != SCOREP_LOCATION_TYPE_CPU_THREAD )
        {
            continue;
        }
        cpu_locations[ local_number_of_cpu_locations++ ]
            = definition->global_location_id;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    /* Distribute all location ids to all ranks for group creation */
    SCOREP_Ipc_Gatherv( cpu_locations,
                        numberOfLocationsPerRank[ SCOREP_Ipc_GetRank() ],
                        allLocations,
                        numberOfLocationsPerRank,
                        SCOREP_IPC_UINT64_T,
                        0 );
    SCOREP_Ipc_Bcast( ( void* )allLocations,
                      ( int )totalNumberOfLocations,
                      SCOREP_IPC_UINT64_T,
                      0 );
}

void
scorep_unify_helper_create_interim_comm_mapping( scorep_definitions_manager_entry* entry )
{
    uint32_t* interim_comm_mapping = scorep_local_definition_manager.interim_communicator.mapping;
    uint32_t* comm_mapping         = scorep_local_definition_manager.communicator.mapping;
    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_BEGIN(
        entry,
        InterimCommunicator,
        SCOREP_Memory_GetLocalDefinitionPageManager() )
    {
        UTILS_BUG_ON( definition->unified == SCOREP_INVALID_COMMUNICATOR,
                      "InterimComm not unified" );
        SCOREP_CommunicatorDef* comm_definition = SCOREP_LOCAL_HANDLE_DEREF( definition->unified, Communicator );

        interim_comm_mapping[ definition->sequence_number ] = comm_mapping[ comm_definition->sequence_number ];
    }
    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_END();
}

void
scorep_unify_helper_check_callsite_hashes( void )
{
    uint32_t* used_callsites           = NULL;
    uint32_t  used_callsites_size      = 0;
    uint32_t  used_callsites_capacity  = 0;
    uint32_t  callsite_hash_collisions = 0;

    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                         Callpath,
                                                         callpath )
    {
        for ( uint32_t i = 0; i < definition->number_of_parameters; i++ )
        {
            if ( strcmp( SCOREP_ParameterHandle_GetName( definition->parameters[ i ].parameter_handle ), "callsite id" ) == 0 )
            {
                /* Data management happens on the launch side as it will always precede the execution side.
                 * Check for duplicates only on the launch side where the value is created.
                 * Callsite hashes should be unique across paradigms as they are bound to respective
                 * launch commands and should be on different callpaths anyhow.
                 */
                UTILS_BUG_ON( SCOREP_ParameterHandle_GetType( definition->parameters[ i ].parameter_handle ) == SCOREP_PARAMETER_STRING,
                              "Callsite parameter has to be an integer parameter!" );
                int64_t parameter_int_value = definition->parameters[ i ].parameter_value.integer_value;
                UTILS_BUG_ON( parameter_int_value  < 0 || parameter_int_value > UINT32_MAX,
                              "Callsite parameter out of range for uint32_t!" );
                if ( SCOREP_RegionHandle_GetParadigmType( definition->region_handle ) == SCOREP_PARADIGM_CUDA )
                {
                    /* Dealing with launch and execution side for the CUDA case. */
                    if ( SCOREP_RegionHandle_GetType( definition->region_handle ) == SCOREP_REGION_WRAPPER )
                    {
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
                                /* Dynamic allocation in batches of 100 */
                                used_callsites_capacity += 100;
                                used_callsites           = realloc( used_callsites, sizeof( uint32_t ) * ( used_callsites_capacity ) );
                                UTILS_ASSERT( used_callsites != NULL );
                            }
                            /* If the callsite is new, which should be the correct default, add the new id to the list. */
                            used_callsites[ used_callsites_size ] = ( uint32_t )parameter_int_value;
                            used_callsites_size++;
                        }
                    }
                    else
                    {
                        /* Kernel execution - the only non "wrapper" carrier of the callsite parameter for CUDA. */
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
}

/* fool linker, so that this unit is always linked into the library/binary. */
UTILS_FOOL_LINKER_DECLARE( scorep_unify_helpers );
