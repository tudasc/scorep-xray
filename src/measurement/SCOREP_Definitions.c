/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


#include <config.h>
#include <SCOREP_Definitions.h>


/**
 * @status     alpha
 * @file       SCOREP_Definitions.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */



#include <scorep_utility/SCOREP_Debug.h>
#include <scorep_utility/SCOREP_Omp.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Timing.h>
#include <jenkins_hash.h>

#include "scorep_definition_structs.h"
#include "scorep_definitions.h"
#include "scorep_types.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/stat.h>

extern SCOREP_DefinitionManager  scorep_local_definition_manager;
extern SCOREP_DefinitionManager* scorep_unified_definition_manager;


/////////////////////////////////////////////////////////////////////////////
// StringDefinitions ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


SCOREP_StringHandle
SCOREP_DefineString( const char* str )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new string \"%s\":", str );

    SCOREP_Definitions_Lock();

    SCOREP_StringHandle new_handle = scorep_string_definition_define(
        &scorep_local_definition_manager,
        str );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopyStringDefinitionToUnified( SCOREP_String_Definition*     definition,
                                      SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( !omp_in_parallel() );
    assert( definition );
    assert( handlesPageManager );
    definition->unified = scorep_string_definition_define( scorep_unified_definition_manager,
                                                           definition->string_data );
}


bool
scorep_string_definitions_equal( const SCOREP_String_Definition* existingDefinition,
                                 const SCOREP_String_Definition* newDefinition )
{
    return existingDefinition->hash_value == newDefinition->hash_value
           && existingDefinition->string_length == newDefinition->string_length
           && 0 == strcmp( existingDefinition->string_data, newDefinition->string_data );
}


SCOREP_StringHandle
scorep_string_definition_define( SCOREP_DefinitionManager* definition_manager,
                                 const char*               str )
{
    assert( definition_manager );

    SCOREP_String_Definition* new_definition = NULL;
    SCOREP_StringHandle       new_handle     = SCOREP_INVALID_STRING;

    /* 1) Get storage for new definition */
    size_t string_length = strlen( str );
    SCOREP_DEFINITION_ALLOC_VARIABLE_ARRAY( String,
                                            char,
                                            string_length + 1 );

    /* 2) populate definitions attributes */

    /* we know the length of the string already, therefore we can use the
     * faster memcpy
     */
    memcpy( new_definition->string_data, str, string_length + 1 );
    new_definition->string_length = string_length;
    new_definition->hash_value    = hash( str, string_length, 0 );

    /*
     * 3) search in existing definitions and return found
     *    - discard new if an old one was found
     *    - if not, link new one into the hash chain and into definition list
     */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( String, string );
}


/////////////////////////////////////////////////////////////////////////////
// SourceFileDefinitions ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/* *INDENT-OFF* */
static bool scorep_source_file_definitions_equal( const SCOREP_SourceFile_Definition* existingDefinition, const SCOREP_SourceFile_Definition* tmpDefinition );
/* *INDENT-ON* */


/**
 * Associate a file name with a process unique file handle.
 */
SCOREP_SourceFileHandle
SCOREP_DefineSourceFile( const char* fileName )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new source file: \"%s\"", fileName );

    SCOREP_Definitions_Lock();

    SCOREP_SourceFileHandle new_handle = scorep_source_file_definition_define(
        &scorep_local_definition_manager,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            fileName ? fileName : "<unknown source file>" ) );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopySourceFileDefinitionToUnified( SCOREP_SourceFile_Definition* definition,
                                          SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( !omp_in_parallel() );
    assert( definition );
    assert( handlesPageManager );

    definition->unified = scorep_source_file_definition_define(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED( definition->name_handle,
                                   String,
                                   handlesPageManager ) );
}


SCOREP_SourceFileHandle
scorep_source_file_definition_define( SCOREP_DefinitionManager* definition_manager,
                                      SCOREP_StringHandle       fileNameHandle )
{
    assert( definition_manager );

    SCOREP_SourceFile_Definition* new_definition = NULL;
    SCOREP_SourceFileHandle       new_handle     = SCOREP_INVALID_SOURCE_FILE;

    SCOREP_DEFINITION_ALLOC( SourceFile );

    new_definition->name_handle = fileNameHandle;
    new_definition->hash_value  = SCOREP_GET_HASH_OF_LOCAL_HANDLE( new_definition->name_handle, String );

    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( SourceFile, source_file );
}


bool
scorep_source_file_definitions_equal( const SCOREP_SourceFile_Definition* existingDefinition,
                                      const SCOREP_SourceFile_Definition* tmpDefinition )
{
    return existingDefinition->hash_value == tmpDefinition->hash_value
           && existingDefinition->name_handle == tmpDefinition->name_handle;
}


/////////////////////////////////////////////////////////////////////////////
// LocationDefinitions //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

static SCOREP_LocationHandle
scorep_location_definition_define( SCOREP_DefinitionManager* definition_manager,
                                   uint64_t                  globalLocationId,
                                   SCOREP_LocationHandle     parent,
                                   SCOREP_StringHandle       nameHandle,
                                   SCOREP_LocationType       locationType,
                                   uint64_t                  numberOfEvents,
                                   uint64_t                  numberOfDefinitions,
                                   uint64_t                  timerResolution );

/**
 * Registers a new local location into the definitions.
 *
 * @note No locking needed, will be done by the caller.
 *
 * @in internal
 */
SCOREP_LocationHandle
SCOREP_DefineLocation( uint64_t              globalLocationId,
                       SCOREP_LocationHandle parent,
                       const char*           name )
{
    SCOREP_Definitions_Lock();

    /** @todo: location_type: this needs clarification after the location hierarchy
               has settled */
    SCOREP_SourceFileHandle new_handle = scorep_location_definition_define(
        &scorep_local_definition_manager,
        globalLocationId,
        parent,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            name ? name : "" ),
        SCOREP_LOCATION_OMP_THREAD,
        0,
        0,
        SCOREP_GetClockResolution() );

    SCOREP_Definitions_Unlock();

    return new_handle;
}

void
SCOREP_CopyLocationDefinitionToUnified( SCOREP_Location_Definition*   definition,
                                        SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( !omp_in_parallel() );
    assert( definition );
    assert( handlesPageManager );

    SCOREP_LocationHandle unified_parent_location_handle = SCOREP_INVALID_LOCATION;
    if ( definition->parent != SCOREP_INVALID_LOCATION )
    {
        unified_parent_location_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent,
            Location,
            handlesPageManager );
        assert( unified_parent_location_handle != SCOREP_MOVABLE_NULL );
    }

    definition->unified = scorep_location_definition_define(
        scorep_unified_definition_manager,
        definition->global_location_id,
        unified_parent_location_handle,
        SCOREP_HANDLE_GET_UNIFIED( definition->name_handle, String, handlesPageManager ),
        definition->location_type,
        definition->number_of_events,
        definition->number_of_definitions,
        definition->timer_resolution );
}

static inline bool
scorep_location_definitions_equal( const SCOREP_Location_Definition* existingDefinition,
                                   const SCOREP_Location_Definition* tmpDefinition )
{
    return false;
}


SCOREP_LocationHandle
scorep_location_definition_define( SCOREP_DefinitionManager* definition_manager,
                                   uint64_t                  globalLocationId,
                                   SCOREP_LocationHandle     parent,
                                   SCOREP_StringHandle       nameHandle,
                                   SCOREP_LocationType       locationType,
                                   uint64_t                  numberOfEvents,
                                   uint64_t                  numberOfDefinitions,
                                   uint64_t                  timerResolution )
{
    assert( definition_manager );

    SCOREP_Location_Definition* new_definition = NULL;
    SCOREP_LocationHandle       new_handle     = SCOREP_INVALID_LOCATION;

    SCOREP_DEFINITION_ALLOC( Location );

    /* locations wont be unfied, therefore no hash value needed, yet? */
    new_definition->global_location_id    = globalLocationId;
    new_definition->parent                = parent;
    new_definition->name_handle           = nameHandle;
    new_definition->location_type         = locationType;
    new_definition->number_of_events      = numberOfEvents;
    new_definition->number_of_definitions = numberOfDefinitions;
    new_definition->timer_resolution      = timerResolution;

    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Location, location );
}


/////////////////////////////////////////////////////////////////////////////
// RegionDefinitions ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


static void
scorep_region_definition_initialize( SCOREP_Region_Definition* definition,
                                     SCOREP_DefinitionManager* definition_manager,
                                     SCOREP_StringHandle       regionNameHandle,
                                     SCOREP_StringHandle       descriptionNameHandle,
                                     SCOREP_StringHandle       fileNameHandle,
                                     SCOREP_LineNo             beginLine,
                                     SCOREP_LineNo             endLine,
                                     SCOREP_AdapterType        adapter,
                                     SCOREP_RegionType         regionType );

/* *INDENT-OFF* */
static bool scorep_region_definitions_equal( const SCOREP_Region_Definition* existingDefinition, const SCOREP_Region_Definition* tmpDefinition );
/* *INDENT-ON* */


SCOREP_RegionHandle
SCOREP_DefineRegion( const char*             regionName,
                     SCOREP_SourceFileHandle fileHandle,
                     SCOREP_LineNo           beginLine,
                     SCOREP_LineNo           endLine,
                     SCOREP_AdapterType      adapter,
                     SCOREP_RegionType       regionType )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new region: %s", regionName );

    /* resolve the file name early */
    SCOREP_StringHandle file_name_handle = SCOREP_INVALID_STRING;
    if ( fileHandle != SCOREP_INVALID_SOURCE_FILE )
    {
        file_name_handle = SCOREP_LOCAL_HANDLE_DEREF( fileHandle, SourceFile )->name_handle;
    }

    SCOREP_Definitions_Lock();

    SCOREP_RegionHandle new_handle = scorep_region_definition_define(
        &scorep_local_definition_manager,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            regionName ? regionName : "<unknown region>" ),
        /* description currently not used */
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            "" ),
        file_name_handle,
        beginLine,
        endLine,
        adapter,
        regionType );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopyRegionDefinitionToUnified( SCOREP_Region_Definition*     definition,
                                      SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( !omp_in_parallel() );
    assert( definition );
    assert( handlesPageManager );

    SCOREP_StringHandle unified_file_name_handle = SCOREP_INVALID_STRING;
    if ( definition->file_name_handle != SCOREP_INVALID_STRING )
    {
        unified_file_name_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->file_name_handle,
            String,
            handlesPageManager );
        assert( unified_file_name_handle != SCOREP_MOVABLE_NULL );
    }


    definition->unified = scorep_region_definition_define(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED( definition->name_handle, String, handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED( definition->description_handle, String, handlesPageManager ),
        unified_file_name_handle,
        definition->begin_line,
        definition->end_line,
        definition->adapter_type,
        definition->region_type );
}


SCOREP_RegionHandle
scorep_region_definition_define( SCOREP_DefinitionManager* definition_manager,
                                 SCOREP_StringHandle       regionNameHandle,
                                 SCOREP_StringHandle       descriptionNameHandle,
                                 SCOREP_StringHandle       fileNameHandle,
                                 SCOREP_LineNo             beginLine,
                                 SCOREP_LineNo             endLine,
                                 SCOREP_AdapterType        adapter,
                                 SCOREP_RegionType         regionType )
{
    assert( definition_manager );

    SCOREP_Region_Definition* new_definition = NULL;
    SCOREP_RegionHandle       new_handle     = SCOREP_INVALID_REGION;

    SCOREP_DEFINITION_ALLOC( Region );
    scorep_region_definition_initialize( new_definition,
                                         definition_manager,
                                         regionNameHandle,
                                         descriptionNameHandle,
                                         fileNameHandle,
                                         beginLine,
                                         endLine,
                                         adapter,
                                         regionType );

    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Region, region );
}


void
scorep_region_definition_initialize( SCOREP_Region_Definition* definition,
                                     SCOREP_DefinitionManager* definition_manager,
                                     SCOREP_StringHandle       regionNameHandle,
                                     SCOREP_StringHandle       descriptionNameHandle,
                                     SCOREP_StringHandle       fileNameHandle,
                                     SCOREP_LineNo             beginLine,
                                     SCOREP_LineNo             endLine,
                                     SCOREP_AdapterType        adapter,
                                     SCOREP_RegionType         regionType )
{
    definition->name_handle = regionNameHandle;
    HASH_ADD_HANDLE( definition, name_handle, String );

    definition->description_handle = descriptionNameHandle;
    HASH_ADD_HANDLE( definition, description_handle, String );

    definition->region_type = regionType; // maps to OTF2_RegionType
    HASH_ADD_POD( definition, region_type );

    definition->file_name_handle = fileNameHandle;
    if ( fileNameHandle != SCOREP_INVALID_STRING )
    {
        HASH_ADD_HANDLE( definition, file_name_handle, String );
    }

    definition->begin_line = beginLine;
    HASH_ADD_POD( definition, begin_line );
    definition->end_line = endLine;
    HASH_ADD_POD( definition, end_line );
    definition->adapter_type = adapter;       // currently not used
    HASH_ADD_POD( definition, adapter_type );
}


bool
scorep_region_definitions_equal( const SCOREP_Region_Definition* existingDefinition,
                                 const SCOREP_Region_Definition* tmpDefinition )
{
    return existingDefinition->hash_value         == tmpDefinition->hash_value &&
           existingDefinition->name_handle        == tmpDefinition->name_handle &&
           existingDefinition->description_handle == tmpDefinition->description_handle &&
           existingDefinition->region_type        == tmpDefinition->region_type &&
           existingDefinition->file_name_handle   == tmpDefinition->file_name_handle &&
           existingDefinition->begin_line         == tmpDefinition->begin_line &&
           existingDefinition->end_line           == tmpDefinition->end_line &&
           existingDefinition->adapter_type       == tmpDefinition->adapter_type;
}


/////////////////////////////////////////////////////////////////////////////
// CommunicatorDefinitions //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

uint32_t scorep_number_of_self_comms = 0;
uint32_t scorep_number_of_root_comms = 0;

static SCOREP_MPICommunicatorHandle
scorep_mpi_communicator_definition_define( SCOREP_DefinitionManager* definition_manager,
                                           const uint64_t            numberOfRanks,
                                           const uint64_t            localRank,
                                           const uint64_t            globalRootRank,
                                           const uint64_t            id );

bool
scorep_mpi_communicator_definitions_equal
(
    const SCOREP_MPICommunicator_Definition* existingDefinition,
    const SCOREP_MPICommunicator_Definition* newDefinition
);

/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
SCOREP_MPICommunicatorHandle
SCOREP_DefineMPICommunicator( const uint64_t numberOfRanks,
                              const uint64_t localRank,
                              const uint64_t globalRootRank,
                              const uint64_t id )
{
    printf( "Local Rank %" PRIu64 ": Define Communicator\n"
            "  size     : %" PRIu64 "\n"
            "  root:      %" PRIu64 "\n"
            "  id:        %" PRIu64 "\n\n", localRank, numberOfRanks, globalRootRank,
            id );

    SCOREP_Definitions_Lock();
    SCOREP_MPICommunicatorHandle new_handle
        = scorep_mpi_communicator_definition_define( &scorep_local_definition_manager,
                                                     numberOfRanks,
                                                     localRank,
                                                     globalRootRank,
                                                     id );

    /* Count the number of comm self instances and communicators where this process
       is rank 0.
     */
    if ( localRank == 0 )
    {
        if ( numberOfRanks > 1 )
        {
            scorep_number_of_root_comms++;
        }
        else
        {
            scorep_number_of_self_comms++;
        }
    }

    SCOREP_Definitions_Unlock();

    return new_handle;
}

/**
 * Associate a MPI communicator with a process unique communicator handle.
 * The number of ranks information is obtained if the process group is
 * associated. The local rank is ignored.
 */
SCOREP_MPICommunicatorHandle
SCOREP_DefineUnifiedMPICommunicator( const uint64_t globalRootRank,
                                     const uint64_t local_id )
{
    printf( "Define unified Communicator\n"
            "  root:      %" PRIu64 "\n"
            "  id:        %" PRIu64 "\n\n", globalRootRank, local_id );

    SCOREP_Definitions_Lock();
    SCOREP_MPICommunicatorHandle new_handle
        = scorep_mpi_communicator_definition_define( scorep_unified_definition_manager,
                                                     1,  // Use the default for self communcators.
                                                     -1, // Local rank is ignored for unified definitions
                                                     globalRootRank,
                                                     local_id );
    SCOREP_Definitions_Unlock();

    return new_handle;
}

SCOREP_MPICommunicatorHandle
scorep_mpi_communicator_definition_define( SCOREP_DefinitionManager* definition_manager,
                                           const uint64_t            numberOfRanks,
                                           const uint64_t            localRank,
                                           const uint64_t            globalRootRank,
                                           const uint64_t            id )
{
    SCOREP_MPICommunicator_Definition* new_definition = NULL;
    SCOREP_MPICommunicatorHandle       new_handle     = SCOREP_INVALID_MPI_COMMUNICATOR;

    SCOREP_DEFINITION_ALLOC( MPICommunicator );

    // Init new_definition
    new_definition->number_of_ranks  = numberOfRanks;
    new_definition->local_rank       = localRank;
    new_definition->global_root_rank = globalRootRank;
    new_definition->root_id          = id;
    new_definition->group            = SCOREP_INVALID_GROUP;

    // The (globalRootRank, id) pair is unique
    HASH_ADD_POD( new_definition, global_root_rank );
    HASH_ADD_POD( new_definition, root_id );

    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( MPICommunicator, mpi_communicator );
}

bool
scorep_mpi_communicator_definitions_equal
(
    const SCOREP_MPICommunicator_Definition* existingDefinition,
    const SCOREP_MPICommunicator_Definition* newDefinition
)
{
    return ( existingDefinition->global_root_rank == newDefinition->global_root_rank ) &&
           ( existingDefinition->root_id          == newDefinition->root_id );
}

/////////////////////////////////////////////////////////////////////////////
// GroupDefinitions /////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


static SCOREP_GroupHandle
scorep_group_definition_define( SCOREP_DefinitionManager* definition_manager,
                                SCOREP_GroupType          groupType,
                                uint64_t                  numberOfMembers,
                                const uint64_t*           members,
                                SCOREP_StringHandle       groupNameHandle,
                                bool                      convertRanks );

static bool
scorep_group_definitions_equal( const SCOREP_Group_Definition* existingDefinition,
                                const SCOREP_Group_Definition* newDefinition );


static void
scorep_group_definition_debug( void );


/**
 * Associate a MPI group with a process unique group handle.
 */
SCOREP_GroupHandle
SCOREP_DefineMPIGroup( const int32_t  numberOfRanks,
                       const int32_t* ranks )
{
    SCOREP_Definitions_Lock();

    SCOREP_GroupHandle new_handle = scorep_group_definition_define(
        &scorep_local_definition_manager,
        SCOREP_GROUP_COMMUNICATOR,
        numberOfRanks,
        ( const uint64_t* )ranks,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            "<unknown MPI group>" ),
        true /* need to convert to global ids */ );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/**
 * Associate a MPI group with a process unique group handle.
 * Used to add groups from the communicator unifiaction after
 * group unification was done.
 */
SCOREP_GroupHandle
SCOREP_DefineUnifiedMPIGroup( int32_t  numberOfRanks,
                              int32_t* ranks )
{
    SCOREP_GroupType type = SCOREP_GROUP_COMMUNICATOR;
    SCOREP_Definitions_Lock();

    if ( ( ranks == NULL ) || ( numberOfRanks <= 1 ) )
    {
        type          = SCOREP_GROUP_COMM_SELF;
        numberOfRanks = 0;
        ranks         = NULL;
    }

    SCOREP_GroupHandle new_handle = scorep_group_definition_define(
        scorep_unified_definition_manager,
        type,
        numberOfRanks,
        ( const uint64_t* )ranks,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            "<unknown MPI group>" ),
        true /* need to convert to global ids */ );

    SCOREP_Definitions_Unlock();

    return new_handle;
}

void
SCOREP_CopyGroupDefinitionToUnified( SCOREP_Group_Definition*      definition,
                                     SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( !omp_in_parallel() );
    assert( definition );
    assert( handlesPageManager );
    definition->unified = scorep_group_definition_define(
        scorep_unified_definition_manager,
        definition->group_type,
        definition->number_of_members,
        definition->members,
        SCOREP_HANDLE_GET_UNIFIED( definition->name_handle, String, handlesPageManager ),
        false );
}


SCOREP_GroupHandle
scorep_group_definition_define( SCOREP_DefinitionManager* definition_manager,
                                SCOREP_GroupType          groupType,
                                uint64_t                  numberOfMembers,
                                const uint64_t*           members,
                                SCOREP_StringHandle       groupNameHandle,
                                bool                      convertFromRanks )
{
    SCOREP_Group_Definition* new_definition = NULL;
    SCOREP_GroupHandle       new_handle     = SCOREP_INVALID_MPI_COMMUNICATOR;
    SCOREP_DEFINITION_ALLOC_VARIABLE_ARRAY( Group,
                                            uint64_t,
                                            numberOfMembers );

    // Init new_definition
    new_definition->group_type = groupType;
    HASH_ADD_POD( new_definition, group_type );

    new_definition->name_handle = groupNameHandle;
    HASH_ADD_HANDLE( new_definition, name_handle, String );

    new_definition->number_of_members = numberOfMembers;
    HASH_ADD_POD( new_definition, number_of_members );

    if ( convertFromRanks )
    {
        const int32_t* ranks = ( const int32_t* )members;
        for ( uint64_t i = 0; i < numberOfMembers; i++ )
        {
            /* convert ranks to global location ids */
            new_definition->members[ i ] = ( uint64_t )ranks[ i ];
        }
    }
    else
    {
        /* Just copy array */
        memcpy( new_definition->members,
                members,
                sizeof( new_definition->members[ 0 ] ) * numberOfMembers );
    }
    HASH_ADD_ARRAY( new_definition, members, number_of_members );

    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Group, group );
}

bool
scorep_group_definitions_equal( const SCOREP_Group_Definition* existingDefinition,
                                const SCOREP_Group_Definition* newDefinition )
{
    return existingDefinition->hash_value == newDefinition->hash_value
           && existingDefinition->group_type == newDefinition->group_type
           && existingDefinition->name_handle == newDefinition->name_handle
           && existingDefinition->number_of_members == newDefinition->number_of_members
           && 0 == memcmp( existingDefinition->members,
                           newDefinition->members,
                           sizeof( existingDefinition->members[ 0 ]
                                   * existingDefinition->number_of_members ) );
}


static void
scorep_group_definition_debug( void )
{
#if 0
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    Handle ID:   %x", new_definition->sequence_number );
    SCOREP_DEBUG_PREFIX( SCOREP_DEBUG_DEFINITIONS );
    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                             "    World ranks:" );

    uint32_t ranks_in_line = 0;
    for ( int32_t i = 0; i < numberOfRanks; ++i )
    {
        if ( ranks_in_line && ranks_in_line % 16 == 0 )
        {
            SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_DEFINITIONS, "\n" );
            SCOREP_DEBUG_PREFIX( SCOREP_DEBUG_DEFINITIONS );
            SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_DEFINITIONS, "%*s",
                                     ( int )strlen( "    World ranks:" ),
                                     "" );
        }

        SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_DEFINITIONS, " %u", ranks[ i ] );

        ranks_in_line++;
    }
    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_DEFINITIONS, "\n" );
#endif
}


/////////////////////////////////////////////////////////////////////////////
// MPIWindowDefinitions /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Associate a MPI window with a process unique window handle.
 */
SCOREP_MPIWindowHandle
SCOREP_DefineMPIWindow( SCOREP_MPICommunicatorHandle communicatorHandle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "Define new MPI Window:" );

    SCOREP_Definitions_Lock();

    SCOREP_MPIWindow_Definition* new_definition = NULL;
    SCOREP_MPIWindowHandle       new_handle     = SCOREP_INVALID_MPI_WINDOW;
    SCOREP_ALLOC_NEW_DEFINITION_OLD( MPIWindow, mpi_window, &scorep_local_definition_manager );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    Handle ID: %x", new_definition->sequence_number );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/////////////////////////////////////////////////////////////////////////////
// MPICartesianTopologyDefinitions //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Associate a MPI cartesian topology with a process unique topology handle.
 */
SCOREP_MPICartesianTopologyHandle
SCOREP_DefineMPICartesianTopology( const char*                  topologyName,
                                   SCOREP_MPICommunicatorHandle communicatorHandle,
                                   uint32_t                     nDimensions,
                                   const uint32_t               nProcessesPerDimension[],
                                   const uint8_t                periodicityPerDimension[] )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new MPI cartesian topology %s:",
                         topologyName );

    SCOREP_Definitions_Lock();

    SCOREP_MPICartesianTopology_Definition* new_definition = NULL;
    SCOREP_MPICartesianTopologyHandle       new_handle     = SCOREP_INVALID_CART_TOPOLOGY;
    SCOREP_ALLOC_NEW_DEFINITION_OLD( MPICartesianTopology, mpi_cartesian_topology, &scorep_local_definition_manager );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

// TODO: make this to a scorep_debug_dump_*_definition
#ifdef SCOREP_DEBUG
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    Handle ID:  %x", new_definition->sequence_number );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    Dimensions: %u", nDimensions );

    for ( uint32_t i = 0; i < nDimensions; ++i )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                             "    Dimension %u:", i );
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                             "        #processes  %u:",
                             nProcessesPerDimension[ i ] );
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                             "        periodicity %hhu:",
                             periodicityPerDimension[ i ] );
    }
#endif

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/////////////////////////////////////////////////////////////////////////////
// MPICartesianCoordsDefinitions ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Define the coordinates of the current rank in the cartesian topology
 * referenced by @a cartesianTopologyHandle.
 */
void
SCOREP_DefineMPICartesianCoords(
    SCOREP_MPICartesianTopologyHandle cartesianTopologyHandle,
    uint32_t                          nCoords,
    const uint32_t                    coordsOfCurrentRank[] )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new MPI cartesian coordinates:" );

    SCOREP_Definitions_Lock();

    SCOREP_MPICartesianCoords_Definition* new_definition = NULL;
    SCOREP_MPICartesianCoordsHandle       new_handle     = SCOREP_INVALID_CART_COORDS;
    SCOREP_ALLOC_NEW_DEFINITION_OLD( MPICartesianCoords, mpi_cartesian_coords, &scorep_local_definition_manager );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

// TODO: make this into a scorep_debug_dump_*_definition function
#ifdef SCOREP_DEBUG
    char stringBuffer[ 16 ];

    SCOREP_DEBUG_PREFIX( SCOREP_DEBUG_DEFINITIONS );
    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_DEFINITIONS, "    Coordinates:" );
    for ( uint32_t i = 0; i < nCoords; ++i )
    {
        SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                                 " %u", coordsOfCurrentRank[ i ] );
    }
    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_DEFINITIONS, "\n" );
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CounterGroupDefinitions //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Associate a name with a process unique counter group handle.
 */
SCOREP_CounterGroupHandle
SCOREP_DefineCounterGroup( const char* name )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new counter group: %s", name );

    SCOREP_Definitions_Lock();

    SCOREP_CounterGroup_Definition* new_definition = NULL;
    SCOREP_CounterGroupHandle       new_handle     = SCOREP_INVALID_COUNTER_GROUP;
    SCOREP_ALLOC_NEW_DEFINITION_OLD( CounterGroup, counter_group, &scorep_local_definition_manager );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/////////////////////////////////////////////////////////////////////////////
// CounterDefinitions ///////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Associate the parameter tuple with a process unique counter handle.
 */
SCOREP_CounterHandle
SCOREP_DefineCounter( const char*               name,
                      SCOREP_CounterType        counterType,
                      SCOREP_CounterGroupHandle counterGroupHandle,
                      const char*               unit )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new counter: %s", name );

    SCOREP_Definitions_Lock();

    SCOREP_Counter_Definition* new_definition = NULL;
    SCOREP_CounterHandle       new_handle     = SCOREP_INVALID_COUNTER;
    SCOREP_ALLOC_NEW_DEFINITION_OLD( Counter, counter, &scorep_local_definition_manager );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/////////////////////////////////////////////////////////////////////////////
// IOFileGroupDefinitions ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Associate a name with a process unique I/O file group handle.
 */
SCOREP_IOFileGroupHandle
SCOREP_DefineIOFileGroup( const char* name )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new I/O file group: %s", name );

    SCOREP_Definitions_Lock();

    SCOREP_IOFileGroup_Definition* new_definition = NULL;
    SCOREP_IOFileGroupHandle       new_handle     = SCOREP_INVALID_IOFILE_GROUP;
    SCOREP_ALLOC_NEW_DEFINITION_OLD( IOFileGroup, io_file_group, &scorep_local_definition_manager );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/////////////////////////////////////////////////////////////////////////////
// IOFileDefinitions ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Associate a name and a group handle with a process unique I/O file handle.
 */
SCOREP_IOFileHandle
SCOREP_DefineIOFile( const char*              name,
                     SCOREP_IOFileGroupHandle ioFileGroup )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new I/O file: %s", name );

    SCOREP_Definitions_Lock();

    SCOREP_IOFile_Definition* new_definition = NULL;
    SCOREP_IOFileHandle       new_handle     = SCOREP_INVALID_IOFILE;
    SCOREP_ALLOC_NEW_DEFINITION_OLD( IOFile, io_file, &scorep_local_definition_manager );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/////////////////////////////////////////////////////////////////////////////
// MarkerGroupDefinitions ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Associate a name with a process unique marker group handle.
 */
SCOREP_MarkerGroupHandle
SCOREP_DefineMarkerGroup( const char* name )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new marker group: %s", name );

    SCOREP_Definitions_Lock();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "" );

    SCOREP_MarkerGroup_Definition* new_definition = NULL;
    SCOREP_MarkerGroupHandle       new_handle     = SCOREP_INVALID_MARKER_GROUP;
    SCOREP_ALLOC_NEW_DEFINITION_OLD( MarkerGroup, marker_group, &scorep_local_definition_manager );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/////////////////////////////////////////////////////////////////////////////
// MarkerDefinitions ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Associate a name and a group handle with a process unique marker handle.
 */
SCOREP_MarkerHandle
SCOREP_DefineMarker( const char*              name,
                     SCOREP_MarkerGroupHandle markerGroup )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new marker: %s", name );

    SCOREP_Definitions_Lock();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "" );

    SCOREP_Marker_Definition* new_definition = NULL;
    SCOREP_MarkerHandle       new_handle     = SCOREP_INVALID_MARKER;
    SCOREP_ALLOC_NEW_DEFINITION_OLD( Marker, marker, &scorep_local_definition_manager );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/////////////////////////////////////////////////////////////////////////////
// ParameterDefinitions /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


static SCOREP_ParameterHandle
scorep_parameter_definition_define( SCOREP_DefinitionManager* definition_manager,
                                    SCOREP_StringHandle       nameHandle,
                                    SCOREP_ParameterType      type );


static bool
scorep_parameter_definitions_equal( const SCOREP_Parameter_Definition* existingDefinition,
                                    const SCOREP_Parameter_Definition* tmpDefinition );


/**
 * Associate a name and a type with a process unique parameter handle.
 */
SCOREP_ParameterHandle
SCOREP_DefineParameter( const char*          name,
                        SCOREP_ParameterType type )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new parameter: %s", name );

    SCOREP_Definitions_Lock();

    SCOREP_ParameterHandle new_handle = scorep_parameter_definition_define(
        &scorep_local_definition_manager,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            name ? name : "<unknown parameter>" ),
        type );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopyParameterDefinitionToUnified( SCOREP_Parameter_Definition*  definition,
                                         SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( !omp_in_parallel() );
    assert( definition );
    assert( handlesPageManager );

    definition->unified = scorep_parameter_definition_define(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED( definition->name_handle, String, handlesPageManager ),
        definition->parameter_type );
}


SCOREP_ParameterHandle
scorep_parameter_definition_define( SCOREP_DefinitionManager* definition_manager,
                                    SCOREP_StringHandle       nameHandle,
                                    SCOREP_ParameterType      type )
{
    assert( definition_manager );

    SCOREP_Parameter_Definition* new_definition = NULL;
    SCOREP_ParameterHandle       new_handle     = SCOREP_INVALID_PARAMETER;

    SCOREP_DEFINITION_ALLOC( Parameter );
    new_definition->name_handle = nameHandle;
    HASH_ADD_HANDLE( new_definition, name_handle, String );
    new_definition->parameter_type = type;
    HASH_ADD_POD( new_definition, parameter_type );

    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Parameter, parameter );
}


bool
scorep_parameter_definitions_equal( const SCOREP_Parameter_Definition* existingDefinition,
                                    const SCOREP_Parameter_Definition* tmpDefinition )
{
    return existingDefinition->hash_value == tmpDefinition->hash_value
           && existingDefinition->name_handle == tmpDefinition->name_handle
           && existingDefinition->parameter_type == tmpDefinition->parameter_type;
}


/////////////////////////////////////////////////////////////////////////////
// CallpathDefinitions //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


static SCOREP_CallpathHandle
scorep_callpath_definition_define( SCOREP_DefinitionManager* definition_manager,
                                   SCOREP_CallpathHandle     parentCallpath,
                                   bool                      withParameter,
                                   SCOREP_RegionHandle       regionHandle,
                                   SCOREP_ParameterHandle    parameterHandle,
                                   int64_t                   integerValue,
                                   SCOREP_StringHandle       stringHandle );


static void
scorep_callpath_definition_initialize( SCOREP_Callpath_Definition* definition,
                                       SCOREP_DefinitionManager*   definition_manager,
                                       SCOREP_CallpathHandle       parentCallpath,
                                       bool                        withParameter,
                                       SCOREP_RegionHandle         regionHandle,
                                       SCOREP_ParameterHandle      parameterHandle,
                                       int64_t                     integerValue,
                                       SCOREP_StringHandle         stringHandle );


static bool
scorep_callpath_definitions_equal( const SCOREP_Callpath_Definition* existingDefinition,
                                   const SCOREP_Callpath_Definition* tmpDefinition );


SCOREP_CallpathHandle
SCOREP_DefineCallpath( SCOREP_CallpathHandle parentCallpath,
                       SCOREP_RegionHandle   region )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new callpath" );

    SCOREP_Definitions_Lock();

    SCOREP_CallpathHandle new_handle = scorep_callpath_definition_define(
        &scorep_local_definition_manager,
        parentCallpath,
        false,
        region,
        SCOREP_INVALID_PARAMETER,
        0,
        SCOREP_INVALID_STRING );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


SCOREP_CallpathHandle
SCOREP_DefineCallpathParameterInteger( SCOREP_CallpathHandle  parentCallpath,
                                       SCOREP_ParameterHandle callpathParameter,
                                       int64_t                integerValue )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new integer callpath" );

    SCOREP_Definitions_Lock();

    SCOREP_CallpathHandle new_handle = scorep_callpath_definition_define(
        &scorep_local_definition_manager,
        parentCallpath,
        true,
        SCOREP_INVALID_REGION,
        callpathParameter,
        integerValue,
        SCOREP_INVALID_STRING );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


SCOREP_CallpathHandle
SCOREP_DefineCallpathParameterString( SCOREP_CallpathHandle  parentCallpath,
                                      SCOREP_ParameterHandle callpathParameter,
                                      SCOREP_StringHandle    stringHandle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new string callpath" );

    SCOREP_Definitions_Lock();

    SCOREP_CallpathHandle new_handle = scorep_callpath_definition_define(
        &scorep_local_definition_manager,
        parentCallpath,
        true,
        SCOREP_INVALID_REGION,
        callpathParameter,
        0,
        stringHandle );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopyCallpathDefinitionToUnified( SCOREP_Callpath_Definition*   definition,
                                        SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( !omp_in_parallel() );
    assert( definition );
    assert( handlesPageManager );

    SCOREP_CallpathHandle unified_parent_callpath_handle = SCOREP_INVALID_CALLPATH;
    if ( definition->parent_callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        unified_parent_callpath_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent_callpath_handle,
            Callpath,
            handlesPageManager );
        assert( unified_parent_callpath_handle != SCOREP_MOVABLE_NULL );
    }

    SCOREP_RegionHandle    unified_region_handle    = SCOREP_INVALID_REGION;
    SCOREP_ParameterHandle unified_parameter_handle = SCOREP_INVALID_PARAMETER;
    int64_t                integer_value            = 0;
    SCOREP_StringHandle    unified_string_handle    = SCOREP_INVALID_STRING;

    if ( !definition->with_parameter )
    {
        if ( definition->callpath_argument.region_handle != SCOREP_INVALID_REGION )
        {
            unified_region_handle = SCOREP_HANDLE_GET_UNIFIED(
                definition->callpath_argument.region_handle,
                Callpath,
                handlesPageManager );
            assert( unified_region_handle != SCOREP_MOVABLE_NULL );
        }
    }
    else
    {
        if ( definition->callpath_argument.parameter_handle
             != SCOREP_INVALID_PARAMETER )
        {
            SCOREP_Parameter_Definition* parameter = SCOREP_HANDLE_DEREF(
                definition->callpath_argument.parameter_handle,
                Parameter,
                handlesPageManager );

            unified_parameter_handle = parameter->unified;
            assert( unified_parameter_handle != SCOREP_MOVABLE_NULL );

            if ( parameter->parameter_type == SCOREP_PARAMETER_INT64 )
            {
                integer_value = definition->parameter_value.integer_value;
            }
            else if ( parameter->parameter_type == SCOREP_PARAMETER_STRING )
            {
                if ( definition->parameter_value.string_handle != SCOREP_INVALID_STRING )
                {
                    unified_string_handle = SCOREP_HANDLE_GET_UNIFIED(
                        definition->parameter_value.string_handle,
                        String,
                        handlesPageManager );
                    assert( unified_string_handle != SCOREP_MOVABLE_NULL );
                }
            }
            else
            {
                assert( !"Valid parameter type." );
            }
        }
    }

    definition->unified = scorep_callpath_definition_define(
        scorep_unified_definition_manager,
        unified_parent_callpath_handle,
        definition->with_parameter,
        unified_region_handle,
        unified_parameter_handle,
        integer_value,
        unified_string_handle );
}


SCOREP_CallpathHandle
scorep_callpath_definition_define( SCOREP_DefinitionManager* definition_manager,
                                   SCOREP_CallpathHandle     parentCallpathHandle,
                                   bool                      withParameter,
                                   SCOREP_RegionHandle       regionHandle,
                                   SCOREP_ParameterHandle    parameterHandle,
                                   int64_t                   integerValue,
                                   SCOREP_StringHandle       stringHandle )
{
    assert( definition_manager );

    SCOREP_Callpath_Definition* new_definition = NULL;
    SCOREP_CallpathHandle       new_handle     = SCOREP_INVALID_CALLPATH;
    SCOREP_DEFINITION_ALLOC( Callpath );
    scorep_callpath_definition_initialize( new_definition,
                                           definition_manager,
                                           parentCallpathHandle,
                                           withParameter,
                                           regionHandle,
                                           parameterHandle,
                                           integerValue,
                                           stringHandle );

    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Callpath, callpath );
}


void
scorep_callpath_definition_initialize( SCOREP_Callpath_Definition* definition,
                                       SCOREP_DefinitionManager*   definition_manager,
                                       SCOREP_CallpathHandle       parentCallpathHandle,
                                       bool                        withParameter,
                                       SCOREP_RegionHandle         regionHandle,
                                       SCOREP_ParameterHandle      parameterHandle,
                                       int64_t                     integerValue,
                                       SCOREP_StringHandle         stringHandle )
{
    definition->parent_callpath_handle = parentCallpathHandle;
    if ( definition->parent_callpath_handle != SCOREP_INVALID_CALLPATH )
    {
        HASH_ADD_HANDLE( definition, parent_callpath_handle, Callpath );
    }

    definition->with_parameter = withParameter;
    HASH_ADD_POD( definition, with_parameter );

    if ( !definition->with_parameter )
    {
        definition->callpath_argument.region_handle = regionHandle;
        if ( definition->callpath_argument.region_handle
             != SCOREP_INVALID_REGION )
        {
            HASH_ADD_HANDLE( definition, callpath_argument.region_handle, Region );
        }
    }
    else
    {
        definition->callpath_argument.parameter_handle = parameterHandle;
        if ( definition->callpath_argument.parameter_handle
             != SCOREP_INVALID_PARAMETER )
        {
            HASH_ADD_HANDLE( definition, callpath_argument.parameter_handle, Parameter );

            SCOREP_Parameter_Definition* parameter = SCOREP_HANDLE_DEREF(
                definition->callpath_argument.parameter_handle,
                Parameter,
                definition_manager->page_manager );

            if ( parameter->parameter_type == SCOREP_PARAMETER_INT64 )
            {
                definition->parameter_value.integer_value = integerValue;
                HASH_ADD_POD( definition, parameter_value.integer_value );
            }
            else if ( parameter->parameter_type == SCOREP_PARAMETER_STRING )
            {
                definition->parameter_value.string_handle = stringHandle;
                if ( definition->parameter_value.string_handle
                     != SCOREP_INVALID_STRING )
                {
                    HASH_ADD_HANDLE( definition,
                                     parameter_value.string_handle,
                                     String );
                }
            }
            else
            {
                assert( !"Valid parameter type." );
            }
        }
    }
}


bool
scorep_callpath_definitions_equal( const SCOREP_Callpath_Definition* existingDefinition,
                                   const SCOREP_Callpath_Definition* tmpDefinition )
{
    if ( existingDefinition->hash_value != tmpDefinition->hash_value
         || existingDefinition->parent_callpath_handle != tmpDefinition->parent_callpath_handle
         || existingDefinition->with_parameter != tmpDefinition->with_parameter )
    {
        return false;
    }

    if ( !existingDefinition->with_parameter )
    {
        return existingDefinition->callpath_argument.region_handle == tmpDefinition->callpath_argument.region_handle;
    }

    if ( existingDefinition->callpath_argument.parameter_handle
         != tmpDefinition->callpath_argument.parameter_handle )
    {
        return false;
    }

    /** @todo: we would need to deref parameter_handle, to get to the type here
     *         but we don't have the associated page manager
     *         we currently know, that sizeof(integer_value) == sizeof(string_handle)
     */
    return existingDefinition->parameter_value.integer_value == tmpDefinition->parameter_value.integer_value;
}


/////////////////////////////////////////////////////////////////////////////
// misc definition stuff ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Region accessor functions for user adapters.
 * @{
 */


/**
 * Gets read-only access to the name of the region.
 *
 * @param handle A handle to the region.
 *
 * @return region name.
 */
const char*
SCOREP_Region_GetName( SCOREP_RegionHandle handle )
{
    SCOREP_Region_Definition* region = SCOREP_LOCAL_HANDLE_DEREF( handle, Region );

    return SCOREP_LOCAL_HANDLE_DEREF( region->name_handle, String )->string_data;
}


/**
 * Gets read-only access to the file name of the region.
 *
 * @param handle A handle to the region.
 *
 * @return region file name.
 */
const char*
SCOREP_Region_GetFileName( SCOREP_RegionHandle handle )
{
    SCOREP_Region_Definition* region = SCOREP_LOCAL_HANDLE_DEREF( handle, Region );

    return SCOREP_LOCAL_HANDLE_DEREF( region->file_name_handle, String )->string_data;
}


/**
 * Gets the type of the region.
 *
 * @param handle A handle to the region.
 *
 * @return region type.
 */
SCOREP_RegionType
SCOREP_Region_GetType( SCOREP_RegionHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Region )->region_type;
}


/**
 * @}
 */


/**
 * Parameter accessor functions for user adapters.
 * @{
 */


/**
 * Gets read-only access to the name of the parameter.
 *
 * @param handle A handle to the parameter.
 *
 * @return region name.
 */
const char*
SCOREP_Parameter_GetName( SCOREP_ParameterHandle handle )
{
    SCOREP_Parameter_Definition* param = SCOREP_LOCAL_HANDLE_DEREF( handle, Parameter );

    return SCOREP_LOCAL_HANDLE_DEREF( param->name_handle, String )->string_data;
}

/**
 * Returns the sequence number of the unified defintions for a local callpath handle from
 * the mappings.
 * @param handle handle to local callpath handle.
 */
uint32_t
SCOREP_Callpath_GetUnifiedSequenceNumber( SCOREP_CallpathHandle handle )
{
    uint32_t local_id = SCOREP_LOCAL_HANDLE_TO_ID( handle, Callpath );
    return scorep_local_definition_manager.mappings->callpath_mappings[ local_id ];
}

/**
 * Returns the unified handle from a local handle.
 */
SCOREP_CallpathHandle
SCOREP_Callpath_GetUnifiedHandle( SCOREP_CallpathHandle handle )
{
    return SCOREP_HANDLE_GET_UNIFIED( handle, Callpath,
                                      SCOREP_Memory_GetLocalDefinitionPageManager() );
}

/**
 * Returns the number of unified callpath definitions.
 */
uint32_t
SCOREP_Callpath_GetNumberOfUnifiedDefinitions()
{
    return scorep_unified_definition_manager->callpath_definition_counter;
}

/**
 * @}
 */
