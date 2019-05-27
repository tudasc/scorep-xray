/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * This file contains the subsystem for topologies.
 *
 * Recording of topologies is enabled by default for all possible types. The
 * following set of environment variables allows each type to be disabled:
 * SCOREP_TOPOLOGY_MPI:      MPI cartesian topologies created in the MPI adapter.
 * SCOREP_TOPOLOGY_USER:     User defined topologies created through user
 *                           instrumentation API.
 * SCOREP_TOPOLOGY_PLATFORM: Platform specific topologies, if available.
 * SCOREP_TOPOLOGY_PROCESS:  A process x threads topology
 *
 * The main task of this subsystem is the creation of the unique platform and process
 * topologies
 * Both have distinct peculiarities that differ from the standard adapter generated MPI
 * and user topologies
 *
 * platform / hardware topology:
 *   Coordinates have to be provided for every location. Depending on the platform the
 *   method to acquire those may vary. Examples:
 *    - on BGQ the call is required to be run on each location as it also provides thread
 *      information
 *    - on the K Computer the respective call only provides information on process level
 *      requiring MPI to be initialized and the thread information has to be provided by
 *      a secondary source
 *   Given these two examples, the subsystem provides multiple opportunities to provide
 *   the necessary information (on subsystem_init_location and on subsystem_end going over
 *   all locations on a process).
 *
 * process x thread / software topology:
 *   Full information about processes and threads can only be gathered after unification
 *   of the locations and locationgroups. Therefore, coordinates can only be created
 *   after the general unification.
 *   On the other hand, a topology has to exist before unification to exist after
 *   unification, which requires creation with placeholder values and filling in of
 *   details after the fact.
 */

#include <config.h>


#include <SCOREP_Config.h>

#include <SCOREP_Subsystem.h>
#include <scorep_status.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Platform.h>
#include <SCOREP_Location.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME TOPOLOGIES
#include <UTILS_Debug.h>

#include <inttypes.h>
#include <string.h>
#include <scorep_ipc.h>
#include "scorep_unify_helpers.h"

/* *INDENT-OFF* */
static SCOREP_ErrorCode topologies_subsystem_register(size_t);
static SCOREP_ErrorCode topologies_subsystem_init(void);
static SCOREP_ErrorCode topologies_subsystem_init_location(struct SCOREP_Location*, struct SCOREP_Location*);
static void topologies_subsystem_end( void );
static SCOREP_ErrorCode topologies_subsystem_pre_unify(void);
static SCOREP_ErrorCode topologies_subsystem_post_unify(void);
/* *INDENT-ON* */

#include "scorep_topologies_confvars.inc.c"

static SCOREP_CartesianTopologyHandle process_topo_id = SCOREP_INVALID_CART_TOPOLOGY;
static int max_locations_per_rank = 0;

/* Gather in pre unify which types actual are present */
static bool                             have_process                         = false;
static bool                             have_platform                        = false;
static bool                             have_user                            = false;
static SCOREP_InterimCommunicatorHandle platform_interim_communicator_handle = SCOREP_INVALID_INTERIM_COMMUNICATOR;
static SCOREP_InterimCommunicatorHandle process_interim_communicator_handle  = SCOREP_INVALID_INTERIM_COMMUNICATOR;
static SCOREP_GroupHandle               process_comm_locations_handle        = SCOREP_INVALID_GROUP;
static SCOREP_GroupHandle               user_comm_locations_handle           = SCOREP_INVALID_GROUP;
static SCOREP_GroupHandle               platform_comm_locations_handle       = SCOREP_INVALID_GROUP;

/* ************************************** subsystem struct */

const SCOREP_Subsystem SCOREP_Subsystem_Topologies =
{
    .subsystem_name          = "Topologies",
    .subsystem_register      = &topologies_subsystem_register,
    .subsystem_init          = &topologies_subsystem_init,
    .subsystem_init_location = &topologies_subsystem_init_location,
    .subsystem_end           = &topologies_subsystem_end,
    .subsystem_pre_unify     = &topologies_subsystem_pre_unify,
    .subsystem_post_unify    = &topologies_subsystem_post_unify,
};

/* ********************************************* static functions */
static size_t subsystem_id;

static void
create_and_store_platform_topology( struct SCOREP_Location* location )
{
#if HAVE( SCOREP_DEBUG )
    int thread = SCOREP_Location_GetId( location );
    int rank   = -1;
    if ( SCOREP_Status_IsMpp() && SCOREP_Status_IsMppInitialized() )
    {
        rank = SCOREP_Status_GetRank();
    }
    UTILS_DEBUG( "Trying to create topology for rank_%d/thread_%d.", rank, thread );
#endif /* HAVE( SCOREP_DEBUG ) */

    static void* hardware_topology_created = ( void* )1; /* any value != 0 suffices */

    void* subsystem_data = SCOREP_Location_GetSubsystemData( location, subsystem_id );
    if ( subsystem_data == NULL )
    {
        char const* topo_name;

        uint32_t num_dims   = 0;
        uint32_t coord_rank = UINT32_MAX;

        if ( SCOREP_Status_IsMpp() && SCOREP_Status_IsMppInitialized() )
        {
            coord_rank = SCOREP_Status_GetRank();
        }

        num_dims = SCOREP_Platform_GetHardwareTopologyNumberOfDimensions();
        if ( num_dims == 0 )
        {
            return;
        }
        int   coords[ num_dims ];
        int   procs_per_dim[ num_dims ];
        int   periodicity_per_dim[ num_dims ];
        char* dim_names[ num_dims ];

        /* only create communicator on thread 0, as threads >0 won't be covered by the communicator
         * unification in the preunify step */
        if ( SCOREP_Location_GetId( location ) == 0 &&
             platform_interim_communicator_handle == SCOREP_INVALID_INTERIM_COMMUNICATOR )
        {
            platform_interim_communicator_handle =
                SCOREP_Definitions_NewInterimCommunicator(
                    SCOREP_INVALID_INTERIM_COMMUNICATOR,
                    SCOREP_PARADIGM_MEASUREMENT,
                    0,
                    NULL );
        }

        /* SCOREP_Platform_GetHardwareTopologyInformation expects arrays in the size of num_dims for
           the dimension informations. */
        if ( SCOREP_SUCCESS == SCOREP_Platform_GetHardwareTopologyInformation( &topo_name,
                                                                               num_dims,
                                                                               procs_per_dim,
                                                                               periodicity_per_dim,
                                                                               dim_names ) )
        {
            if ( SCOREP_SUCCESS != SCOREP_Platform_GetCartCoords( num_dims, coords, location ) )
            {
                /* if we can't get coords successfully, we don't need to create the topology def either
                   even if it is technically possible. */
                return;
            }
            SCOREP_CartesianTopologyHandle handle =
                SCOREP_Definitions_NewCartesianTopology( topo_name,
                                                         platform_interim_communicator_handle,
                                                         num_dims,
                                                         procs_per_dim,
                                                         periodicity_per_dim,
                                                         ( const char** )dim_names,
                                                         SCOREP_TOPOLOGIES_PLATFORM );

            SCOREP_Definitions_NewCartesianCoords( handle, coord_rank, SCOREP_Location_GetId( location ), num_dims, coords );

            SCOREP_Location_SetSubsystemData( location, subsystem_id, hardware_topology_created );
            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_TOPOLOGIES, "Created topology for rank_%d/thread_%d.", rank, thread );
        }
        else
        {
            UTILS_DEBUG_PRINTF( SCOREP_DEBUG_TOPOLOGIES, "Failed creating topology for rank_%d/thread_%d.", rank, thread );
        }
    }
    else
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_TOPOLOGIES, "Topology already created for rank_%d/thread_%d.", rank, thread );
    }
}


static SCOREP_ErrorCode
topologies_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG_ENTRY();
    subsystem_id = subsystemId;
    SCOREP_ConfigRegister( "topology", scorep_topology_confvars );
    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
topologies_subsystem_init( void )
{
    UTILS_DEBUG_ENTRY();

    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
topologies_subsystem_init_location( struct SCOREP_Location* location,
                                    struct SCOREP_Location* parent )
{
    UTILS_DEBUG_ENTRY();

    /* The first check is dependent on the user choice, the second is dependent
       on the platform and the current Score-P running mode, e.g., MPP or not. */
    if ( !scorep_topologies_enable_platform || !SCOREP_Platform_GenerateTopology() )
    {
        return SCOREP_SUCCESS;
    }

    if ( SCOREP_Location_GetType( location ) == SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        create_and_store_platform_topology( location );
    }

    return SCOREP_SUCCESS;
}

static bool
add_missing_platform_entries( SCOREP_Location* location,
                              void*            arg )
{
    UTILS_DEBUG_ENTRY();

    if ( SCOREP_Location_GetType( location ) != SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        return false;
    }

    create_and_store_platform_topology( location );

    return false;
}

static void
topologies_subsystem_end( void )
{
    UTILS_DEBUG_ENTRY();
    if ( scorep_topologies_enable_platform && SCOREP_Platform_GenerateTopology() )
    {
        SCOREP_Location_ForAll( add_missing_platform_entries, NULL );
    }
}

static int
compare_uint64( const void* a, const void* b )
{
    const uint64_t* aptr = ( const uint64_t* )a;
    const uint64_t* bptr = ( const uint64_t* )b;

    if ( *aptr <  *bptr )
    {
        return -1;
    }
    if (  *aptr == *bptr )
    {
        return 0;
    }
    if (  *aptr >  *bptr )
    {
        return 1;
    }
}

static void
define_topology_locations_pre_unify_create_groups( void )
{
    /* check which types of topologies are used, process x threads is always available if enabled */
    uint32_t have_user_local = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         CartesianTopology,
                                                         cartesian_topology )
    {
        switch ( definition->topology_type )
        {
            case SCOREP_TOPOLOGIES_PLATFORM:
                have_platform = true;
                break;
            case SCOREP_TOPOLOGIES_USER:
                have_user_local = 1;
                break;
            default:
                break;
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    // In edge cases some processes might not participate in user topologies at all while others do.
    // Ensure that those processes will partake in the required global comm calls.
    // Only required for user topologies.
    uint32_t have_user_global = have_user_local;
    SCOREP_Ipc_Allreduce( &have_user_local,
                          &have_user_global,
                          1,
                          SCOREP_IPC_UINT32_T,
                          SCOREP_IPC_MAX );
    have_user = ( bool )have_user_global;

    /* we need group(s) for non-MPI topology communicators */
    if ( have_process || have_platform || have_user )
    {
        uint32_t total_number_of_locations    = 0;
        int*     number_of_locations_per_rank =  calloc( SCOREP_Ipc_GetSize(), sizeof( int ) );

        /* Currently process and platform have the same locations,
           all CPU locations, so just one gathering step */
        scorep_unify_helper_get_number_of_cpu_locations( number_of_locations_per_rank, &total_number_of_locations, &max_locations_per_rank );

        uint64_t* all_locations = calloc( total_number_of_locations,  sizeof( uint64_t ) );

        scorep_unify_helper_exchange_all_cpu_locations( all_locations, total_number_of_locations, number_of_locations_per_rank );

        // local_locations_per_rank=number_of_locations_per_rank[SCOREP_Ipc_GetRank()];
        free( number_of_locations_per_rank );
        /* COMM_LOCATION group creation dependent on found topology types */
        if ( have_user )
        {
            user_comm_locations_handle = SCOREP_Definitions_NewGroup( SCOREP_GROUP_TOPOLOGY_USER_LOCATIONS,
                                                                      "USER_GROUP_LOCATIONS",
                                                                      total_number_of_locations,
                                                                      all_locations );
        }
        if ( have_platform )
        {
            platform_comm_locations_handle = SCOREP_Definitions_NewGroup( SCOREP_GROUP_TOPOLOGY_HARDWARE_LOCATIONS,
                                                                          "HARDWARE_GROUP_LOCATIONS",
                                                                          total_number_of_locations,
                                                                          all_locations );
        }
        if ( have_process )
        {
            process_comm_locations_handle = SCOREP_Definitions_NewGroup( SCOREP_GROUP_TOPOLOGY_PROCESS_LOCATIONS,
                                                                         "PROCESSXTHREADS_GROUP_LOCATIONS",
                                                                         total_number_of_locations,
                                                                         all_locations );
        }

        /* Create identity map */
        for ( uint64_t i = 0; i < total_number_of_locations; i++ )
        {
            all_locations[ i ] = i;
        }

        /* create the respective COMM_GROUPS and unified communicators as needed */
        if ( have_user )
        {
            // Creating COMM_GROUPS specific for the ranks used in coordinates
            uint32_t total_number_of_local_topologies  = scorep_local_definition_manager.cartesian_topology.counter;
            uint32_t unique_number_of_local_topologies = 0;


            uint32_t unique_local_topology_name_handles[ total_number_of_local_topologies ];
            // Index in unique_local_topology_name_handles
            uint32_t map_to_local_unique_topos[ total_number_of_local_topologies ];

            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                                 CartesianTopology,
                                                                 cartesian_topology )
            {
                if ( definition->topology_type == SCOREP_TOPOLOGIES_USER )
                {
                    bool new_entry = true;
                    for ( uint32_t i = 0; i < unique_number_of_local_topologies; i++ )
                    {
                        if ( definition->topology_name == unique_local_topology_name_handles[ i ] )
                        {
                            // already in list
                            new_entry                                                = false;
                            map_to_local_unique_topos[ definition->sequence_number ] = i;
                            break;
                        }
                    }
                    if ( new_entry )
                    {
                        unique_local_topology_name_handles[ unique_number_of_local_topologies ] = definition->topology_name;
                        map_to_local_unique_topos[ definition->sequence_number ]                = unique_number_of_local_topologies;
                        unique_number_of_local_topologies++;
                    }
                }
            }
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
            // --> we have a local list of unique topo names and a map from each user topo seq. number to its index in the name list aka group index


            // Create local groups
            // reserve group of maximal possible locations for each unique topo, as we don't know yet how they are distributed
            uint64_t unique_local_group_members[ unique_number_of_local_topologies ][ scorep_local_definition_manager.location.counter ];

            uint32_t unique_local_group_members_count[ unique_number_of_local_topologies ];
            memset( unique_local_group_members_count, 0, unique_number_of_local_topologies * sizeof( uint32_t ) );

            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                                 CartesianCoords,
                                                                 cartesian_coords )
            {
                SCOREP_CartesianTopologyDef* cart_topo = SCOREP_LOCAL_HANDLE_DEREF( definition->topology_handle,
                                                                                    CartesianTopology );
                if ( cart_topo->topology_type == SCOREP_TOPOLOGIES_USER  )
                {
                    uint32_t  unique_topo_index   = map_to_local_unique_topos[ cart_topo->sequence_number ];
                    uint64_t  global_location_id  = SCOREP_Location_CalculateGlobalId( definition->rank, definition->thread );
                    uint64_t* comm_locations_user = SCOREP_LOCAL_HANDLE_DEREF( user_comm_locations_handle,
                                                                               Group )->members;
                    uint64_t comm_locations_user_count = SCOREP_LOCAL_HANDLE_DEREF( user_comm_locations_handle,
                                                                                    Group )->number_of_members;
                    uint64_t start_pos = ( uint64_t )( ( definition->rank * max_locations_per_rank ) +  definition->thread );
                    if ( start_pos > ( comm_locations_user_count - 1 ) )
                    {
                        // maximum start position is the last index
                        start_pos =  comm_locations_user_count - 1;
                    }
                    // Storing respective index to COMM_LOCATIONS group in unique_local_group_members
                    uint64_t i = start_pos;
                    while ( true )
                    {
                        if ( comm_locations_user[ i ] == global_location_id )
                        {
                            bool new_entry = true;
                            for ( uint32_t j = 0; j < unique_local_group_members_count[ unique_topo_index ]; j++ )
                            {
                                if ( unique_local_group_members[ unique_topo_index ][ j ] == i )
                                {
                                    //already in group, case of muliple coords per location
                                    new_entry = false;
                                    break;
                                }
                            }
                            if ( new_entry )
                            {
                                unique_local_group_members[ unique_topo_index ][ unique_local_group_members_count[ unique_topo_index ] ] = i;
                                unique_local_group_members_count[ unique_topo_index ]++;
                            }
                            break;
                        }
                        if ( i == 0 )
                        {
                            // if we are still here and didn't break in any of the other cases, we haven't found
                            // a match in the COMM_LOCATIONS definition for this coordinate. That should never happen
                            UTILS_FATAL( "No location match found for this coordinate!" );
                        }
                        i--;
                    }
                }
            }
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
            // --> we have groups for all unique local groups, and an index for all user topos to its group


            // Determine the length of the topology names in the unique list
            uint32_t unique_local_topology_name_lengths[ unique_number_of_local_topologies ];

            uint32_t total_local_name_length = 0;
            uint32_t max_local_name_length   = 0;
            for ( uint32_t i = 0; i < unique_number_of_local_topologies; i++ )
            {
                const char* name   = SCOREP_StringHandle_Get( unique_local_topology_name_handles[ i ] );
                uint32_t    length = strlen( name );

                total_local_name_length                += length + 1;
                unique_local_topology_name_lengths[ i ] = length;
                if ( length > max_local_name_length )
                {
                    max_local_name_length = length;
                }
            }

            char total_name_string[ total_local_name_length ];
            int  offset = 0;
            for ( uint32_t i = 0; i < unique_number_of_local_topologies; i++ )
            {
                memcpy( total_name_string + offset, SCOREP_StringHandle_Get( unique_local_topology_name_handles[ i ] ), ( unique_local_topology_name_lengths[ i ] ) );
                offset                         += unique_local_topology_name_lengths[ i ];
                total_name_string[ offset - 1 ] = '\0';
            }

            int num_procs = SCOREP_Ipc_GetSize();
            int rank      = SCOREP_Ipc_GetRank();

            // find the maximum process local string length
            uint32_t max_all_local_lengths = 0;
            SCOREP_Ipc_Allreduce( &total_local_name_length,
                                  &max_all_local_lengths,
                                  1,
                                  SCOREP_IPC_UINT32_T,
                                  SCOREP_IPC_MAX );


            char all_names[ num_procs ][ max_all_local_lengths ];
            for ( int i = 0; i < num_procs; i++ )
            {
                all_names[ i ][ 0 ] = '\0';
            }

            // find the maximum number of topologies over the processes
            uint32_t max_number_local_topologies = 0;

            SCOREP_Ipc_Allreduce( &unique_number_of_local_topologies,
                                  &max_number_local_topologies,
                                  1,
                                  SCOREP_IPC_UINT32_T,
                                  SCOREP_IPC_MAX );

            int all_local_single_string_lengths[ num_procs ][ max_number_local_topologies ];
            memset( all_local_single_string_lengths, 0, num_procs * max_number_local_topologies * sizeof( int ) );
            int local_single_string_lengths_buffer[ max_number_local_topologies ];
            memset( local_single_string_lengths_buffer, 0, max_number_local_topologies * sizeof( int ) );
            for ( uint32_t i = 0; i < unique_number_of_local_topologies; i++ )
            {
                local_single_string_lengths_buffer[ i ] = ( int )unique_local_topology_name_lengths[ i ];
            }
            // gather local string lengths
            SCOREP_Ipc_Allgather( local_single_string_lengths_buffer,
                                  all_local_single_string_lengths,
                                  max_number_local_topologies,
                                  SCOREP_IPC_UINT32_T );
            // gather the string names
            SCOREP_Ipc_Allgather(   total_name_string,
                                    all_names,
                                    max_all_local_lengths,
                                    SCOREP_IPC_CHAR );


            // mapping local indices to the global list
            int      map_to_global[ num_procs ][ max_number_local_topologies ];
            char*    global_topology_name_strings[ max_number_local_topologies * num_procs ];
            uint32_t number_global_topology_name_strings = 0;

            for ( int i = 0; i < num_procs; i++ )
            {
                for ( uint32_t j = 0; j < max_number_local_topologies; j++ )
                {
                    // using -1 as indicator that this not mapping to any other topo
                    // in case of the local set of topologies is smaller than the global.
                    map_to_global[ i ][ j ] = -1;
                }

                uint32_t offset = 0;
                for ( uint32_t j = 0; j < max_number_local_topologies; j++ )
                {
                    if ( all_local_single_string_lengths[ i ][ j ] == 0 )
                    {
                        // no more topologies (fewer than max), ensured by initializing with 0 and
                        // requiring non-zero names from user topologies at creation
                        break;
                    }
                    //pick name based on all_local_single_string_lengths[i][j]
                    char name[ all_local_single_string_lengths[ i ][ j ] ];
                    sprintf( name, "%s", all_names[ i ] + offset );
                    offset += all_local_single_string_lengths[ i ][ j ];

                    bool new_entry = true;
                    for ( int k = 0; k < number_global_topology_name_strings; k++ )
                    {
                        if ( strcmp( name, global_topology_name_strings[ k ] ) == 0 )
                        {
                            //already added to the global list, just add k as map entry for this process and his ith topology
                            map_to_global[ i ][ j ] = k;
                            new_entry               = false;
                            break; // unique local topologies ensure: no multiple matches
                        }
                    }
                    if ( new_entry )
                    {
                        //add this to the global list
                        global_topology_name_strings[ number_global_topology_name_strings ] = calloc( all_local_single_string_lengths[ i ][ j ], sizeof( char ) );
                        sprintf( global_topology_name_strings[ number_global_topology_name_strings ], "%s", name );
                        map_to_global[ i ][ j ] = number_global_topology_name_strings;
                        number_global_topology_name_strings++;
                    }
                }
            }


            // group exchange allgather
            uint64_t global_location_ids_groups[ num_procs ][ number_global_topology_name_strings ][ max_locations_per_rank ];

            for ( uint64_t i = 0; i < num_procs; i++ )
            {
                for ( uint64_t j = 0; j < number_global_topology_name_strings; j++ )
                {
                    for ( uint64_t k = 0; k < max_locations_per_rank; k++ )
                    {
                        global_location_ids_groups[ i ][ j ][ k ] = UINT64_MAX;
                    }
                }
            }
            // init global array with the local group sets
            for ( uint64_t i = 0; i < unique_number_of_local_topologies; i++ )
            {
                for ( uint64_t j = 0; j < unique_local_group_members_count[ i ]; j++ )
                {
                    global_location_ids_groups[ rank ][ map_to_global[ rank ][ i ] ][ j ] = unique_local_group_members[ i ][ j ];
                }
            }

            SCOREP_Ipc_Allgather( global_location_ids_groups[ rank ],
                                  global_location_ids_groups,
                                  number_global_topology_name_strings * max_locations_per_rank,
                                  SCOREP_IPC_UINT64_T );

            // check which indices have to be added as we need only unique entries of location ids
            uint64_t final_groups[ unique_number_of_local_topologies ][ total_number_of_locations ];
            uint64_t final_groups_element_count[ unique_number_of_local_topologies ];
            memset( final_groups_element_count, 0, unique_number_of_local_topologies * sizeof( uint64_t ) );

            for ( uint32_t i = 0; i < num_procs; i++ )
            {
                for ( uint64_t j = 0; j < unique_number_of_local_topologies; j++ )
                {
                    // filling our topologies
                    int global_topo_index = map_to_global[ rank ][ j ];
                    for ( uint64_t k = 0; k < max_locations_per_rank; k++ )
                    {
                        if ( global_location_ids_groups[ i ][ global_topo_index ][ k ] == UINT64_MAX )
                        {
                            // either not taking part in this topology or last location id already passed
                            break;
                        }

                        // we have a valid location id, check if we need to add the value.
                        bool found_new_element = true;
                        for ( uint32_t n = 0; n < final_groups_element_count[ j ]; n++ )
                        {
                            // check in final list
                            if ( global_location_ids_groups[ i ][ global_topo_index ][ k ] == final_groups[ j ][ n ] )
                            {
                                // existing location id, no need to add
                                found_new_element = false;
                                break;
                            }
                        }
                        if ( found_new_element )
                        {
                            final_groups[ j ][ final_groups_element_count[ j ] ] = global_location_ids_groups[ i ][ global_topo_index ][ k ];
                            final_groups_element_count[ j ]++;
                        }
                    }
                }
            }

            // qsort groups to make the order ascending, not strictly necessary if order of indeces isn't a priority
            // as the groups are build on each process the same way even if they are unordered, e.g., hybrid cases.

            for ( int i = 0; i < unique_number_of_local_topologies; i++ )
            {
                qsort( final_groups[ i ], final_groups_element_count[ i ], sizeof( uint64_t ), compare_uint64 );
            }


            // create groups and unified comms for the topologies
            SCOREP_GroupHandle        topology_group_handles[ unique_number_of_local_topologies ];
            SCOREP_CommunicatorHandle topology_comm_handles[ unique_number_of_local_topologies ];
            for ( uint64_t i = 0; i < unique_number_of_local_topologies; i++ )
            {
                topology_group_handles[ i ] = SCOREP_INVALID_GROUP;
                topology_comm_handles[ i ]  = SCOREP_INVALID_COMMUNICATOR;
            }


            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                                 CartesianTopology,
                                                                 cartesian_topology )
            {
                if ( definition->topology_type == SCOREP_TOPOLOGIES_USER )
                {
                    uint32_t topo_id        = definition->sequence_number;
                    uint64_t unique_topo_id = map_to_local_unique_topos[ topo_id ];

                    if ( topology_group_handles[ unique_topo_id ] == SCOREP_INVALID_GROUP )
                    {
                        // first appearance of this unique topo: new group and new comm
                        topology_group_handles[ unique_topo_id ] = SCOREP_Definitions_NewGroup(
                            SCOREP_GROUP_TOPOLOGY_USER_GROUP,
                            SCOREP_StringHandle_Get( definition->topology_name ),
                            final_groups_element_count[ unique_topo_id ],
                            final_groups[ unique_topo_id ] );

                        // as we set them  together, comm also not yet defined
                        topology_comm_handles[ unique_topo_id ] = SCOREP_Definitions_NewCommunicator(
                            topology_group_handles[ unique_topo_id ],
                            definition->topology_name,
                            SCOREP_INVALID_COMMUNICATOR,
                            0 );
                    }
                    // set comm for this topo
                    SCOREP_LOCAL_HANDLE_DEREF( definition->communicator_handle,
                                               InterimCommunicator )->unified = topology_comm_handles[ unique_topo_id ];
                }
            }
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
        }
        if ( have_platform )
        {
            SCOREP_GroupHandle group_handle = SCOREP_Definitions_NewGroup(
                SCOREP_GROUP_TOPOLOGY_HARDWARE_GROUP,
                "HARDWARE_GROUP",
                total_number_of_locations,
                all_locations );

            SCOREP_LOCAL_HANDLE_DEREF( platform_interim_communicator_handle,
                                       InterimCommunicator )->unified =
                SCOREP_Definitions_NewCommunicator(
                    group_handle,
                    scorep_definitions_new_string(
                        &scorep_local_definition_manager,
                        "Hardware CPU Locations", NULL ),
                    SCOREP_INVALID_COMMUNICATOR,
                    0 );
        }
        if ( have_process )
        {
            SCOREP_GroupHandle group_handle = SCOREP_Definitions_NewGroup(
                SCOREP_GROUP_TOPOLOGY_PROCESS_GROUP,
                "PROCESSXTHREADS_GROUP",
                total_number_of_locations,
                all_locations );

            SCOREP_LOCAL_HANDLE_DEREF( process_interim_communicator_handle,
                                       InterimCommunicator )->unified =
                SCOREP_Definitions_NewCommunicator(
                    group_handle,
                    scorep_definitions_new_string(
                        &scorep_local_definition_manager,
                        "Process x Threads CPU Locations", NULL ),
                    SCOREP_INVALID_COMMUNICATOR,
                    0 );
        }

        free( all_locations );
    }
}

static SCOREP_ErrorCode
topologies_subsystem_pre_unify( void )
{
    UTILS_DEBUG_ENTRY();

    if ( scorep_topologies_enable_platform )
    {
        /* correction of the rank information in the coords provided on init_location
           since rank isn't necessarily available there (only for platform topologies)
         */
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                             CartesianCoords,
                                                             cartesian_coords )
        {
            SCOREP_CartesianTopologyDef* cart_topo = SCOREP_LOCAL_HANDLE_DEREF( definition->topology_handle,
                                                                                CartesianTopology );
            if ( cart_topo->topology_type == SCOREP_TOPOLOGIES_PLATFORM )
            {
                definition->rank = SCOREP_Status_GetRank();
            }
        }
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    }

    if ( scorep_topologies_enable_process )
    {
        /* first part creating the comm. */
        have_process                        = true; //always there if enabled
        process_interim_communicator_handle =
            SCOREP_Definitions_NewInterimCommunicator(
                SCOREP_INVALID_INTERIM_COMMUNICATOR,
                SCOREP_PARADIGM_MEASUREMENT,
                0,
                NULL );
    }
    define_topology_locations_pre_unify_create_groups();

    if ( scorep_topologies_enable_process )
    {
        /* second part creating the topo after max_locations_per_rank is gathered */
        int         dims[ 2 ]      = { SCOREP_Status_GetSize(),  max_locations_per_rank };
        int         periodic[ 2 ]  = { 0, 0 };
        const char* dim_names[ 2 ] = { "Process", "Thread" };
        process_topo_id =  SCOREP_Definitions_NewCartesianTopology( "Process x Thread",
                                                                    process_interim_communicator_handle,
                                                                    2,
                                                                    dims,
                                                                    periodic,
                                                                    dim_names,
                                                                    SCOREP_TOPOLOGIES_PROCESS );

        // creating coordinates for each CPU location on each rank
        uint32_t local_id_counter = 0;
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                             Location,
                                                             location )
        {
            if ( definition->location_type != SCOREP_LOCATION_TYPE_CPU_THREAD )
            {
                continue;
            }

            int coords[ 2 ] = { SCOREP_Ipc_GetRank(), local_id_counter };
            SCOREP_Definitions_NewCartesianCoords( process_topo_id, SCOREP_Ipc_GetRank(), definition->sequence_number, 2, coords );
            local_id_counter++;
        }
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    }

    UTILS_DEBUG_EXIT();

    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
topologies_subsystem_post_unify( void )
{
    UTILS_DEBUG_ENTRY();

    if ( SCOREP_Status_GetRank() == 0 )
    {
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                             CartesianTopology,
                                                             cartesian_topology )
        {
            /* In case MPI_Comm_set_name was set for any of the MPI topology comms, use its name as the topology name. */
            if ( definition->topology_type == SCOREP_TOPOLOGIES_MPI )
            {
                SCOREP_StringHandle unified_comm_string_handle = SCOREP_LOCAL_HANDLE_DEREF( definition->communicator_handle, Communicator )->name_handle;
                if ( unified_comm_string_handle )
                {
                    char* comm_name = SCOREP_UNIFIED_HANDLE_DEREF( unified_comm_string_handle, String )->string_data;

                    SCOREP_StringHandle new_name_handle = scorep_definitions_new_string( scorep_unified_definition_manager,
                                                                                         comm_name,
                                                                                         NULL );
                    definition->topology_name = new_name_handle;
                }
            }
        }
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

        if ( have_process || have_platform || have_user )
        {
            /* Up to this point the rank identifies the MPI rank.
               Use this rank and the thread information to create the mapping for the
               location group holding all CPU thread locations to conform to the OTF2
               definition.
             */

            /* Arrays of location ids in the comm location group as reference for the rank mapping. */
            uint64_t* locations_platform;
            uint64_t* locations_process;
            uint64_t* locations_user;
            // set by either topology type
            uint64_t comm_locations_member_count = 0;

            if ( have_platform )
            {
                locations_platform = SCOREP_UNIFIED_HANDLE_DEREF( platform_comm_locations_handle,
                                                                  Group )->members;
                uint64_t count =  SCOREP_UNIFIED_HANDLE_DEREF( platform_comm_locations_handle,
                                                               Group )->number_of_members;
                if ( comm_locations_member_count != 0 && comm_locations_member_count != count )
                {
                    UTILS_FATAL( "COMM_LOCATIONS groups should have the same number of elements!" );
                }
                comm_locations_member_count = count;
            }
            if ( have_process )
            {
                locations_process = SCOREP_UNIFIED_HANDLE_DEREF( process_comm_locations_handle,
                                                                 Group )->members;
                uint64_t count =  SCOREP_UNIFIED_HANDLE_DEREF( process_comm_locations_handle,
                                                               Group )->number_of_members;
                if ( comm_locations_member_count != 0 && comm_locations_member_count != count )
                {
                    UTILS_FATAL( "COMM_LOCATIONS groups should have the same number of elements!" );
                }
                comm_locations_member_count = count;
            }
            if ( have_user )
            {
                locations_user = SCOREP_UNIFIED_HANDLE_DEREF( user_comm_locations_handle,
                                                              Group )->members;
                uint64_t count =  SCOREP_UNIFIED_HANDLE_DEREF( user_comm_locations_handle,
                                                               Group )->number_of_members;
                if ( comm_locations_member_count != 0 && comm_locations_member_count != count )
                {
                    UTILS_FATAL( "COMM_LOCATIONS groups should have the same number of elements!" );
                }
                comm_locations_member_count = count;
            }
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                                 CartesianCoords,
                                                                 cartesian_coords )
            {
                SCOREP_Topology_Type cart_topo_type = SCOREP_UNIFIED_HANDLE_DEREF( definition->topology_handle,
                                                                                   CartesianTopology )->topology_type;
                if ( cart_topo_type  != SCOREP_TOPOLOGIES_MPI )
                {
                    /* use rank and thread to get global id and identify the final rank inside the comm */
                    uint64_t coord_rank         = definition->rank;
                    uint64_t coord_thread       = definition->thread;
                    uint64_t global_location_id = SCOREP_Location_CalculateGlobalId( coord_rank, coord_thread );

                    /* For the lookup "rank * max_threads + thread" is used. Since the locations are
                       ordered by rank and thread this provides an useful starting point.
                       For symmetric threaded programs, where the thread number is constant, the lookup
                       for all coords is in O(n). The lookup becomes more costly with higher deviation
                       from maxThreads and higher rank.
                       This is based on the current fact, that all global COMM_LOCATION groups have the
                       same number of elements and one loop can be used to address all non MPI topology types.
                       If that changes, the loop has to be splitted.
                     */
                    uint64_t start_pos = ( uint64_t )( ( definition->rank * max_locations_per_rank ) + definition->thread );
                    if ( start_pos > ( comm_locations_member_count - 1 ) )
                    {
                        // maximum start position is the last index
                        start_pos =  comm_locations_member_count - 1;
                    }
                    uint64_t i = start_pos;
                    while ( true )
                    {
                        if ( cart_topo_type == SCOREP_TOPOLOGIES_PROCESS )
                        {
                            if ( locations_process[ i ] == global_location_id )
                            {
                                definition->rank = ( uint32_t )i; // set to index in the group
                                break;
                            }
                        }
                        if ( cart_topo_type == SCOREP_TOPOLOGIES_PLATFORM )
                        {
                            if ( locations_platform[ i ] == global_location_id )
                            {
                                definition->rank = ( uint32_t )i; // set to index in the group
                                break;
                            }
                        }
                        if ( cart_topo_type == SCOREP_TOPOLOGIES_USER )
                        {
                            if ( locations_user[ i ] == global_location_id )
                            {
                                SCOREP_CommunicatorHandle topo_comm =  SCOREP_UNIFIED_HANDLE_DEREF( definition->topology_handle,
                                                                                                    CartesianTopology )->communicator_handle;

                                SCOREP_GroupHandle group_handle         = SCOREP_UNIFIED_HANDLE_DEREF( topo_comm, Communicator )->group_handle;
                                uint64_t*          locations_user_group = SCOREP_UNIFIED_HANDLE_DEREF( group_handle,
                                                                                                       Group )->members;
                                uint64_t count =  SCOREP_UNIFIED_HANDLE_DEREF( group_handle,
                                                                               Group )->number_of_members;

                                for ( uint64_t j = 0; j < count; j++ )
                                {
                                    if ( i == locations_user_group[ j ] )
                                    {
                                        definition->rank = ( uint32_t )j; // set to index in the group
                                    }
                                }
                                break;
                            }
                        }
                        if ( i == 0 )
                        {
                            // if we are still here and didn't break in any of the other cases, we haven't found
                            // a match in the COMM_LOCATIONS definition for this coordinate. That should never happen
                            UTILS_FATAL( "No location match found for this coordinate!" );
                        }
                        i--;
                    }
                }
            }
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
        }
    }
    UTILS_DEBUG_EXIT();
    return SCOREP_SUCCESS;
}


uint32_t
SCOREP_Topologies_CoordRankFromGroupIndex( uint32_t                       index,
                                           SCOREP_CartesianTopologyHandle topoHandle )
{
    SCOREP_GroupHandle   comm_locations_handle = SCOREP_INVALID_GROUP;
    SCOREP_Topology_Type topo_type             = SCOREP_UNIFIED_HANDLE_DEREF( topoHandle,
                                                                              CartesianTopology )->topology_type;

    switch ( topo_type )
    {
        case  SCOREP_TOPOLOGIES_PROCESS:
            comm_locations_handle = process_comm_locations_handle;
            break;
        case SCOREP_TOPOLOGIES_PLATFORM:
            comm_locations_handle = platform_comm_locations_handle;
            break;
        case SCOREP_TOPOLOGIES_USER:
        {
            comm_locations_handle = user_comm_locations_handle;
            uint64_t* locations_user_group =
                SCOREP_UNIFIED_HANDLE_DEREF(
                    SCOREP_UNIFIED_HANDLE_DEREF(
                        SCOREP_UNIFIED_HANDLE_DEREF( topoHandle,
                                                     CartesianTopology )->communicator_handle,
                        Communicator )->group_handle,
                    Group )->members;
            index = locations_user_group[ index ];
            break;
        }
        default:
            UTILS_FATAL( "This topology type is not usable here." );
    }

    uint64_t* locations = SCOREP_UNIFIED_HANDLE_DEREF( comm_locations_handle,
                                                       Group )->members;
    return SCOREP_Location_CalculateRank( locations[ index ] );
}

uint32_t
SCOREP_Topologies_CoordThreadFromGroupIndex( uint32_t                       index,
                                             SCOREP_CartesianTopologyHandle topoHandle )
{
    SCOREP_GroupHandle   comm_locations_handle = SCOREP_INVALID_GROUP;
    SCOREP_Topology_Type topo_type             = SCOREP_UNIFIED_HANDLE_DEREF( topoHandle,
                                                                              CartesianTopology )->topology_type;

    switch ( topo_type )
    {
        case  SCOREP_TOPOLOGIES_PROCESS:
            comm_locations_handle = process_comm_locations_handle;
            break;
        case SCOREP_TOPOLOGIES_PLATFORM:
            comm_locations_handle = platform_comm_locations_handle;
            break;
        case SCOREP_TOPOLOGIES_USER:
        {
            comm_locations_handle = user_comm_locations_handle;
            uint64_t* locations_user_group =
                SCOREP_UNIFIED_HANDLE_DEREF(
                    SCOREP_UNIFIED_HANDLE_DEREF(
                        SCOREP_UNIFIED_HANDLE_DEREF( topoHandle,
                                                     CartesianTopology )->communicator_handle,
                        Communicator )->group_handle,
                    Group )->members;
            index = locations_user_group[ index ];
            break;
        }
        default:
            UTILS_FATAL( "This topology type is not usable here." );
    }

    uint64_t* locations = SCOREP_UNIFIED_HANDLE_DEREF( comm_locations_handle,
                                                       Group )->members;
    return SCOREP_Location_CalculateLocalId(  locations[ index ] );
}
