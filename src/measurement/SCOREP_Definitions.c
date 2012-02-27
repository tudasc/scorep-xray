/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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


/**
 * @status     alpha
 * @file       SCOREP_Definitions.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/stat.h>


#include <scorep_utility/SCOREP_Debug.h>


#include <jenkins_hash.h>


#include <SCOREP_Definitions.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Timing.h>
#include <scorep_openmp.h>
#include <scorep_definition_structs.h>
#include <scorep_definitions.h>
#include <scorep_types.h>


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
    assert( !SCOREP_Omp_InParallel() );
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
    /* Does return */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( String, string );
}


/////////////////////////////////////////////////////////////////////////////
// SourceFileDefinitions ////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/* *INDENT-OFF* */
static bool scorep_source_file_definitions_equal( const SCOREP_SourceFile_Definition* existingDefinition, const SCOREP_SourceFile_Definition* newDefinition );
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
    assert( !SCOREP_Omp_InParallel() );
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

    /* Does return */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( SourceFile, source_file );
}


bool
scorep_source_file_definitions_equal( const SCOREP_SourceFile_Definition* existingDefinition,
                                      const SCOREP_SourceFile_Definition* newDefinition )
{
    return existingDefinition->hash_value == newDefinition->hash_value
           && existingDefinition->name_handle == newDefinition->name_handle;
}


/////////////////////////////////////////////////////////////////////////////
// LocationGroupDefinitions /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

static SCOREP_LocationGroupHandle
scorep_location_group_definition_define( SCOREP_DefinitionManager*   definition_manager,
                                         uint64_t                    globalLocationGroupId,
                                         SCOREP_SystemTreeNodeHandle parent,
                                         SCOREP_StringHandle         nameHandle,
                                         SCOREP_LocationGroupType    locationType );

/**
 * Registers a new local location group into the definitions.
 *
 * @in internal
 */
SCOREP_LocationGroupHandle
SCOREP_DefineLocationGroup( uint64_t                    globalLocationGroupId,
                            SCOREP_SystemTreeNodeHandle parent,
                            const char*                 name )
{
    SCOREP_Definitions_Lock();

    SCOREP_LocationGroupHandle new_handle = scorep_location_group_definition_define(
        &scorep_local_definition_manager,
        globalLocationGroupId,
        parent,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            name ? name : "" ),
        SCOREP_LOCATION_GROUP_TYPE_PROCESS );

    SCOREP_Definitions_Unlock();

    return new_handle;
}

void
SCOREP_CopyLocationGroupDefinitionToUnified( SCOREP_LocationGroup_Definition* definition,
                                             SCOREP_Allocator_PageManager*    handlesPageManager )
{
    assert( !SCOREP_Omp_InParallel() );
    assert( definition );
    assert( handlesPageManager );

    SCOREP_SystemTreeNodeHandle unified_parent_handle = SCOREP_INVALID_SYSTEM_TREE_NODE;
    if ( definition->parent != SCOREP_INVALID_SYSTEM_TREE_NODE )
    {
        unified_parent_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent,
            SystemTreeNode,
            handlesPageManager );
        assert( unified_parent_handle != SCOREP_MOVABLE_NULL );
    }

    definition->unified = scorep_location_group_definition_define(
        scorep_unified_definition_manager,
        definition->global_location_group_id,
        unified_parent_handle,
        SCOREP_HANDLE_GET_UNIFIED( definition->name_handle, String, handlesPageManager ),
        definition->location_group_type );
}

static inline bool
scorep_location_group_definitions_equal( const SCOREP_LocationGroup_Definition* existingDefinition,
                                         const SCOREP_LocationGroup_Definition* newDefinition )
{
    return false;
}


SCOREP_LocationGroupHandle
scorep_location_group_definition_define( SCOREP_DefinitionManager*   definition_manager,
                                         uint64_t                    globalLocationGroupId,
                                         SCOREP_SystemTreeNodeHandle parent,
                                         SCOREP_StringHandle         nameHandle,
                                         SCOREP_LocationGroupType    locationGroupType )
{
    assert( definition_manager );

    SCOREP_LocationGroup_Definition* new_definition = NULL;
    SCOREP_LocationGroupHandle       new_handle     = SCOREP_INVALID_LOCATION_GROUP;

    SCOREP_DEFINITION_ALLOC( LocationGroup );

    /* location groups wont be unfied, therefore no hash value needed */
    new_definition->global_location_group_id = globalLocationGroupId;
    new_definition->parent                   = parent;
    new_definition->name_handle              = nameHandle;
    new_definition->location_group_type      = locationGroupType;

    /* Does return */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( LocationGroup, location_group );
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
                                   uint64_t                  locationGroupId );

/**
 * Registers a new location into the definitions.
 *
 * @in internal
 */
SCOREP_LocationHandle
SCOREP_DefineLocation( uint64_t              globalLocationId,
                       SCOREP_LocationHandle parent,
                       const char*           name )
{
    SCOREP_Definitions_Lock();

    SCOREP_SourceFileHandle new_handle = scorep_location_definition_define(
        &scorep_local_definition_manager,
        globalLocationId,
        parent,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            name ? name : "" ),
        SCOREP_LOCATION_TYPE_CPU_THREAD,
        0, 0 );

    SCOREP_Definitions_Unlock();

    return new_handle;
}

void
SCOREP_CopyLocationDefinitionToUnified( SCOREP_Location_Definition*   definition,
                                        SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( !SCOREP_Omp_InParallel() );
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
        definition->location_group_id );
}

static inline bool
scorep_location_definitions_equal( const SCOREP_Location_Definition* existingDefinition,
                                   const SCOREP_Location_Definition* newDefinition )
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
                                   uint64_t                  locationGroupId )
{
    assert( definition_manager );

    SCOREP_Location_Definition* new_definition = NULL;
    SCOREP_LocationHandle       new_handle     = SCOREP_INVALID_LOCATION;

    SCOREP_DEFINITION_ALLOC( Location );

    /* locations wont be unfied, therefore no hash value needed */
    new_definition->global_location_id = globalLocationId;
    new_definition->parent             = parent;
    new_definition->name_handle        = nameHandle;
    new_definition->location_type      = locationType;
    new_definition->number_of_events   = numberOfEvents;
    new_definition->location_group_id  = locationGroupId;

    /* Does return */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Location, location );
}

/////////////////////////////////////////////////////////////////////////////
// SystemTreeNodeDefinitions ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

SCOREP_SystemTreeNodeHandle
scorep_system_tree_node_definition_define( SCOREP_DefinitionManager*   definition_manager,
                                           SCOREP_SystemTreeNodeHandle parent,
                                           SCOREP_StringHandle         name,
                                           SCOREP_StringHandle         class );

bool
scorep_system_tree_node_definitions_equal( const SCOREP_SystemTreeNode_Definition* existingDefinition,
                                           const SCOREP_SystemTreeNode_Definition* newDefinition );


SCOREP_SystemTreeNodeHandle
SCOREP_DefineSystemTreeNode( SCOREP_SystemTreeNodeHandle parent,
                             const char*                 name,
                             const char*                 class )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new system tree node %s of class %s",
                         name, class );

    SCOREP_Definitions_Lock();

    SCOREP_SystemTreeNodeHandle new_handle
        = scorep_system_tree_node_definition_define(
        &scorep_local_definition_manager,
        parent,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            name ? name : "<unnamed system tree node>" ),
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            class ? class : "<unnamed system tree class>" ) );

    SCOREP_Definitions_Unlock();

    return new_handle;
}

void
SCOREP_CopySystemTreeNodeDefinitionToUnified( SCOREP_SystemTreeNode_Definition* definition,
                                              SCOREP_Allocator_PageManager*     handlesPageManager )
{
    assert( !SCOREP_Omp_InParallel() );
    assert( definition );
    assert( handlesPageManager );

    SCOREP_SystemTreeNodeHandle unified_parent_handle = SCOREP_INVALID_SYSTEM_TREE_NODE;
    if ( definition->parent_handle != SCOREP_INVALID_SYSTEM_TREE_NODE )
    {
        unified_parent_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent_handle,
            SystemTreeNode,
            handlesPageManager );
        assert( unified_parent_handle != SCOREP_MOVABLE_NULL );
    }

    definition->unified = scorep_system_tree_node_definition_define(
        scorep_unified_definition_manager,
        unified_parent_handle,
        SCOREP_HANDLE_GET_UNIFIED( definition->name_handle, String, handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED( definition->class_handle, String, handlesPageManager )
        );
}

bool
scorep_system_tree_node_definitions_equal( const SCOREP_SystemTreeNode_Definition* existingDefinition,
                                           const SCOREP_SystemTreeNode_Definition* newDefinition )
{
    return existingDefinition->hash_value    == newDefinition->hash_value &&
           existingDefinition->parent_handle == newDefinition->parent_handle &&
           existingDefinition->class_handle  == newDefinition->class_handle &&
           existingDefinition->name_handle   == newDefinition->name_handle;
}

SCOREP_SystemTreeNodeHandle
scorep_system_tree_node_definition_define( SCOREP_DefinitionManager*   definition_manager,
                                           SCOREP_SystemTreeNodeHandle parent,
                                           SCOREP_StringHandle         name,
                                           SCOREP_StringHandle         class )
{
    assert( definition_manager );

    SCOREP_SystemTreeNode_Definition* new_definition = NULL;
    SCOREP_SystemTreeNodeHandle       new_handle     = SCOREP_INVALID_SYSTEM_TREE_NODE;

    SCOREP_DEFINITION_ALLOC( SystemTreeNode );

    new_definition->parent_handle = parent;
    if ( new_definition->parent_handle != SCOREP_INVALID_SYSTEM_TREE_NODE )
    {
        HASH_ADD_HANDLE( new_definition, parent_handle, SystemTreeNode );
    }

    new_definition->name_handle = name;
    HASH_ADD_HANDLE( new_definition, name_handle, String );

    new_definition->class_handle = class;
    HASH_ADD_HANDLE( new_definition, class_handle, String );

    /* Does return */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( SystemTreeNode, system_tree_node );
}

/////////////////////////////////////////////////////////////////////////////
// RegionDefinitions ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


static SCOREP_RegionHandle
scorep_region_definition_define( SCOREP_DefinitionManager* definition_manager,
                                 SCOREP_StringHandle       regionNameHandle,
                                 SCOREP_StringHandle       descriptionNameHandle,
                                 SCOREP_StringHandle       fileNameHandle,
                                 SCOREP_LineNo             beginLine,
                                 SCOREP_LineNo             endLine,
                                 SCOREP_AdapterType        adapter,
                                 SCOREP_RegionType         regionType );



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
static bool scorep_region_definitions_equal( const SCOREP_Region_Definition* existingDefinition, const SCOREP_Region_Definition* newDefinition );
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
    assert( !SCOREP_Omp_InParallel() );
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

    /* Does return */
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

    definition->region_type = regionType;
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
                                 const SCOREP_Region_Definition* newDefinition )
{
    return existingDefinition->hash_value         == newDefinition->hash_value &&
           existingDefinition->name_handle        == newDefinition->name_handle &&
           existingDefinition->description_handle == newDefinition->description_handle &&
           existingDefinition->region_type        == newDefinition->region_type &&
           existingDefinition->file_name_handle   == newDefinition->file_name_handle &&
           existingDefinition->begin_line         == newDefinition->begin_line &&
           existingDefinition->end_line           == newDefinition->end_line &&
           existingDefinition->adapter_type       == newDefinition->adapter_type;
}


/////////////////////////////////////////////////////////////////////////////
// CommunicatorDefinitions //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

uint32_t scorep_number_of_self_comms = 0;
uint32_t scorep_number_of_root_comms = 0;

static SCOREP_LocalMPICommunicatorHandle
scorep_local_mpi_communicator_definitions_define(
    SCOREP_DefinitionManager* definition_manager,
    bool                      isSelfLike,
    uint32_t                  localRank,
    uint32_t                  globalRootRank,
    uint32_t                  id );

static bool
scorep_local_mpi_communicator_definitions_equal(
    const SCOREP_LocalMPICommunicator_Definition* existingDefinition,
    const SCOREP_LocalMPICommunicator_Definition* newDefinition );

/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
SCOREP_LocalMPICommunicatorHandle
SCOREP_DefineLocalMPICommunicator( uint32_t numberOfRanks,
                                   uint32_t localRank,
                                   uint32_t globalRootRank,
                                   uint32_t id )
{
    SCOREP_LocalMPICommunicatorHandle new_handle = SCOREP_INVALID_LOCAL_MPI_COMMUNICATOR;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Local Rank %" PRIu32 ": Define local Communicator:"
                         "  size: %" PRIu32
                         "  root: %" PRIu32
                         "  id: %" PRIu32,
                         localRank,
                         numberOfRanks,
                         globalRootRank,
                         id );

    SCOREP_Definitions_Lock();

    new_handle = scorep_local_mpi_communicator_definitions_define(
        &scorep_local_definition_manager,
        numberOfRanks == 1,
        localRank,
        globalRootRank,
        id );

    /*
     * Count the number of comm self instances and communicators where this
     * process is rank 0.
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

static SCOREP_LocalMPICommunicatorHandle
scorep_local_mpi_communicator_definitions_define(
    SCOREP_DefinitionManager* definition_manager,
    bool                      isSelfLike,
    uint32_t                  localRank,
    uint32_t                  globalRootRank,
    uint32_t                  id )
{
    SCOREP_LocalMPICommunicator_Definition* new_definition = NULL;
    SCOREP_LocalMPICommunicatorHandle       new_handle     = SCOREP_INVALID_LOCAL_MPI_COMMUNICATOR;

    SCOREP_DEFINITION_ALLOC( LocalMPICommunicator );

    // Init new_definition
    new_definition->is_self_like     = isSelfLike;
    new_definition->local_rank       = localRank;
    new_definition->global_root_rank = globalRootRank;
    new_definition->root_id          = id;

    /* Does return */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( LocalMPICommunicator,
                                              local_mpi_communicator );
}

bool
scorep_local_mpi_communicator_definitions_equal(
    const SCOREP_LocalMPICommunicator_Definition* existingDefinition,
    const SCOREP_LocalMPICommunicator_Definition* newDefinition )
{
    return false;
}

static bool
scorep_mpi_communicator_definitions_equal(
    const SCOREP_MPICommunicator_Definition* existingDefinition,
    const SCOREP_MPICommunicator_Definition* newDefinition );

/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
SCOREP_MPICommunicatorHandle
SCOREP_DefineUnifiedMPICommunicator( SCOREP_GroupHandle group_handle )
{
    SCOREP_MPICommunicator_Definition* new_definition     = NULL;
    SCOREP_MPICommunicatorHandle       new_handle         = SCOREP_INVALID_MPI_COMMUNICATOR;
    SCOREP_DefinitionManager*          definition_manager = scorep_unified_definition_manager;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define unified Communicator" );

    assert( !SCOREP_Omp_InParallel() );
    assert( scorep_unified_definition_manager );

    SCOREP_DEFINITION_ALLOC( MPICommunicator );

    // Init new_definition
    new_definition->group = group_handle;

    /* Does return */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( MPICommunicator,
                                              mpi_communicator );
}

bool
scorep_mpi_communicator_definitions_equal(
    const SCOREP_MPICommunicator_Definition* existingDefinition,
    const SCOREP_MPICommunicator_Definition* newDefinition )
{
    return false;
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
                                bool                      convertFromUint32 );

static bool
scorep_group_definitions_equal( const SCOREP_Group_Definition* existingDefinition,
                                const SCOREP_Group_Definition* newDefinition );


/* Used to protect defining a MPI group before the list of MPI locations */
static bool scorep_mpi_locations_defined;


/**
 * Associate a MPI group with a process unique group handle.
 */
SCOREP_GroupHandle
SCOREP_DefineMPIGroup( int32_t        numberOfRanks,
                       const int32_t* ranks )
{
    SCOREP_Definitions_Lock();

    /* we should also be called only once */
    SCOREP_BUG_ON( scorep_mpi_locations_defined == false,
                   "Called before SCOREP_DefineMPILocations" );

    SCOREP_GroupHandle new_handle = scorep_group_definition_define(
        &scorep_local_definition_manager,
        SCOREP_GROUP_MPI_GROUP,
        numberOfRanks,
        ( const uint64_t* )ranks,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            "" ),
        true /* need to be converted from uint32_t */ );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/**
 * Define the MPI locations
 *
 * needs only be called by rank 0
 */
void
SCOREP_DefineMPILocations( int32_t        numberOfRanks,
                           const int32_t* locations )
{
    SCOREP_Definitions_Lock();

    SCOREP_BUG_ON( scorep_mpi_locations_defined == true,
                   "We should be called only once" );

    SCOREP_GroupHandle new_handle = scorep_group_definition_define(
        &scorep_local_definition_manager,
        SCOREP_GROUP_MPI_LOCATIONS,
        numberOfRanks,
        ( const uint64_t* )locations,
        scorep_string_definition_define(
            &scorep_local_definition_manager,
            "" ),
        true /* need to be converted from uint32_t */ );

    /* Its now semantically correct to define MPI groups */
    scorep_mpi_locations_defined = true;

    SCOREP_Definitions_Unlock();
}


/**
 * Associate a MPI group with a process unique group handle.
 * Used to add groups from the communicator unifiaction after
 * group unification was done.
 */
SCOREP_GroupHandle
SCOREP_DefineUnifiedMPIGroup( SCOREP_GroupType type,
                              int32_t          numberOfRanks,
                              int32_t*         ranks )
{
    assert( !SCOREP_Omp_InParallel() );
    assert( scorep_unified_definition_manager );

    SCOREP_GroupHandle new_handle = scorep_group_definition_define(
        scorep_unified_definition_manager,
        type,
        numberOfRanks,
        ( const uint64_t* )ranks,
        scorep_string_definition_define(
            scorep_unified_definition_manager,
            "" ),
        true /* need to be converted from uint32_t */ );

    return new_handle;
}

void
SCOREP_CopyGroupDefinitionToUnified( SCOREP_Group_Definition*      definition,
                                     SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( !SCOREP_Omp_InParallel() );
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
                                bool                      convertFromUint32 )
{
    SCOREP_Group_Definition* new_definition = NULL;
    SCOREP_GroupHandle       new_handle     = SCOREP_INVALID_GROUP;
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

    if ( convertFromUint32 )
    {
        const int32_t* ranks = ( const int32_t* )members;
        for ( uint64_t i = 0; i < numberOfMembers; i++ )
        {
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

    /* Does return */
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


/////////////////////////////////////////////////////////////////////////////
// MPIWindowDefinitions /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Associate a MPI window with a process unique window handle.
 */
SCOREP_MPIWindowHandle
SCOREP_DefineMPIWindow( SCOREP_LocalMPICommunicatorHandle communicatorHandle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "Define new MPI Window:" );

    SCOREP_Definitions_Lock();

    SCOREP_MPIWindow_Definition* new_definition = NULL;
    SCOREP_MPIWindowHandle       new_handle     = SCOREP_INVALID_MPI_WINDOW;

    // Init new_definition
    // see ticket:423
    //SCOREP_DEBUG_NOT_YET_IMPLEMENTED();

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
SCOREP_DefineMPICartesianTopology( const char*                       topologyName,
                                   SCOREP_LocalMPICommunicatorHandle communicatorHandle,
                                   uint32_t                          nDimensions,
                                   const uint32_t                    nProcessesPerDimension[],
                                   const uint8_t                     periodicityPerDimension[] )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new MPI cartesian topology %s:",
                         topologyName );

    SCOREP_Definitions_Lock();

    SCOREP_MPICartesianTopology_Definition* new_definition = NULL;
    SCOREP_MPICartesianTopologyHandle       new_handle     = SCOREP_INVALID_CART_TOPOLOGY;

    // Init new_definition
    // see ticket:423
    //SCOREP_DEBUG_NOT_YET_IMPLEMENTED();

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

    // Init new_definition
    // see ticket:423
    //SCOREP_DEBUG_NOT_YET_IMPLEMENTED();

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
// MetricDefinitions ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


static SCOREP_MetricHandle
scorep_metric_definition_define(
    SCOREP_DefinitionManager*  definition_manager,
    SCOREP_StringHandle        metricNameHandle,
    SCOREP_StringHandle        descriptionNameHandle,
    SCOREP_MetricSourceType    sourceType,
    SCOREP_MetricMode          mode,
    SCOREP_MetricValueType     valueType,
    SCOREP_MetricBase          base,
    int64_t                    exponent,
    SCOREP_StringHandle        unitNameHandle,
    SCOREP_MetricProfilingType profilingType );

static void
scorep_metric_definition_initialize(
    SCOREP_Metric_Definition*  definition,
    SCOREP_DefinitionManager*  definition_manager,
    SCOREP_StringHandle        metricNameHandle,
    SCOREP_StringHandle        descriptionNameHandle,
    SCOREP_MetricSourceType    sourceType,
    SCOREP_MetricMode          mode,
    SCOREP_MetricValueType     valueType,
    SCOREP_MetricBase          base,
    int64_t                    exponent,
    SCOREP_StringHandle        unitNameHandle,
    SCOREP_MetricProfilingType profilingType );

static bool
scorep_metric_definitions_equal( const SCOREP_Metric_Definition* existingDefinition,
                                 const SCOREP_Metric_Definition* newDefinition );


/**
 * Associate the parameter tuple with a process unique counter handle.
 */
SCOREP_MetricHandle
SCOREP_DefineMetric( const char*                name,
                     const char*                description,
                     SCOREP_MetricSourceType    sourceType,
                     SCOREP_MetricMode          mode,
                     SCOREP_MetricValueType     valueType,
                     SCOREP_MetricBase          base,
                     int64_t                    exponent,
                     const char*                unit,
                     SCOREP_MetricProfilingType profilingType )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new metric member: %s", name );

    SCOREP_Definitions_Lock();

    SCOREP_Metric_Definition* new_definition = NULL;
    SCOREP_MetricHandle       new_handle     = scorep_metric_definition_define(
        &scorep_local_definition_manager,
        scorep_string_definition_define( &scorep_local_definition_manager,
                                         name ? name : "<unknown metric>" ),
        scorep_string_definition_define( &scorep_local_definition_manager,
                                         description ? description : "" ),
        sourceType,
        mode,
        valueType,
        base,
        exponent,
        scorep_string_definition_define( &scorep_local_definition_manager,
                                         unit ? unit : "#" ),
        profilingType );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopyMetricDefinitionToUnified( SCOREP_Metric_Definition*     definition,
                                      SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( !SCOREP_Omp_InParallel() );
    assert( definition );
    assert( handlesPageManager );

    definition->unified = scorep_metric_definition_define(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED( definition->name_handle,
                                   String,
                                   handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED( definition->description_handle,
                                   String,
                                   handlesPageManager ),
        definition->source_type,
        definition->mode,
        definition->value_type,
        definition->base,
        definition->exponent,
        SCOREP_HANDLE_GET_UNIFIED( definition->unit_handle,
                                   String,
                                   handlesPageManager ),
        definition->profiling_type );
}


SCOREP_MetricHandle
scorep_metric_definition_define( SCOREP_DefinitionManager*  definition_manager,
                                 SCOREP_StringHandle        metricNameHandle,
                                 SCOREP_StringHandle        descriptionNameHandle,
                                 SCOREP_MetricSourceType    sourceType,
                                 SCOREP_MetricMode          mode,
                                 SCOREP_MetricValueType     valueType,
                                 SCOREP_MetricBase          base,
                                 int64_t                    exponent,
                                 SCOREP_StringHandle        unitNameHandle,
                                 SCOREP_MetricProfilingType profilingType )
{
    assert( definition_manager );

    SCOREP_Metric_Definition* new_definition = NULL;
    SCOREP_MetricHandle       new_handle     = SCOREP_INVALID_METRIC;

    SCOREP_DEFINITION_ALLOC( Metric );
    scorep_metric_definition_initialize( new_definition,
                                         definition_manager,
                                         metricNameHandle,
                                         descriptionNameHandle,
                                         sourceType,
                                         mode,
                                         valueType,
                                         base,
                                         exponent,
                                         unitNameHandle,
                                         profilingType );

    /* Does return */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Metric, metric );
}


void
scorep_metric_definition_initialize(
    SCOREP_Metric_Definition*  definition,
    SCOREP_DefinitionManager*  definition_manager,
    SCOREP_StringHandle        metricNameHandle,
    SCOREP_StringHandle        descriptionNameHandle,
    SCOREP_MetricSourceType    sourceType,
    SCOREP_MetricMode          mode,
    SCOREP_MetricValueType     valueType,
    SCOREP_MetricBase          base,
    int64_t                    exponent,
    SCOREP_StringHandle        unitNameHandle,
    SCOREP_MetricProfilingType profilingType )
{
    definition->name_handle = metricNameHandle;
    HASH_ADD_HANDLE( definition, name_handle, String );

    definition->description_handle = descriptionNameHandle;
    HASH_ADD_HANDLE( definition, description_handle, String );

    definition->source_type = sourceType;
    HASH_ADD_POD( definition, source_type );

    definition->mode = mode;
    HASH_ADD_POD( definition, mode );

    definition->value_type = valueType;
    HASH_ADD_POD( definition, value_type );

    definition->base = base;
    HASH_ADD_POD( definition, base );

    definition->exponent = exponent;
    HASH_ADD_POD( definition, exponent );

    definition->unit_handle = unitNameHandle;
    HASH_ADD_HANDLE( definition, unit_handle, String );

    definition->profiling_type = profilingType;
    HASH_ADD_POD( definition, profiling_type );
}


bool
scorep_metric_definitions_equal( const SCOREP_Metric_Definition* existingDefinition,
                                 const SCOREP_Metric_Definition* newDefinition )
{
    return existingDefinition->hash_value == newDefinition->hash_value
           && existingDefinition->name_handle == newDefinition->name_handle
           && existingDefinition->description_handle == newDefinition->description_handle
           && existingDefinition->source_type == newDefinition->source_type
           && existingDefinition->mode == newDefinition->mode
           && existingDefinition->value_type == newDefinition->value_type
           && existingDefinition->base == newDefinition->base
           && existingDefinition->exponent == newDefinition->exponent
           && existingDefinition->unit_handle == newDefinition->unit_handle;
}


/////////////////////////////////////////////////////////////////////////////
// SamplingSetDefinitions and ScopedSamplingSetDefinitions //////////////////
/////////////////////////////////////////////////////////////////////////////


static SCOREP_SamplingSetHandle
scorep_sampling_set_definition_define(
    SCOREP_DefinitionManager*     definition_manager,
    uint8_t                       numberOfMetrics,
    const SCOREP_MetricHandle*    metrics,
    SCOREP_MetricOccurrence       occurrence,
    SCOREP_Allocator_PageManager* handlesPageManager );

static void
scorep_sampling_set_definition_initialize(
    SCOREP_SamplingSet_Definition* definition,
    SCOREP_DefinitionManager*      definition_manager,
    uint8_t                        numberOfMetrics,
    const SCOREP_MetricHandle*     metrics,
    SCOREP_MetricOccurrence        occurrence,
    SCOREP_Allocator_PageManager*  handlesPageManager );

static SCOREP_SamplingSetHandle
scorep_scoped_sampling_set_definition_define(
    SCOREP_DefinitionManager* definition_manager,
    SCOREP_SamplingSetHandle  samplingSet,
    SCOREP_LocationHandle     recorderHandle,
    SCOREP_MetricScope        scopeType,
    SCOREP_AnyHandle          scopeHandle );

static void
scorep_scoped_sampling_set_definition_initialize(
    SCOREP_ScopedSamplingSet_Definition* definition,
    SCOREP_DefinitionManager*            definition_manager,
    SCOREP_SamplingSetHandle             samplingSet,
    SCOREP_LocationHandle                recorderHandle,
    SCOREP_MetricScope                   scopeType,
    SCOREP_AnyHandle                     scopeHandle );


bool
scorep_sampling_set_definitions_equal( const SCOREP_SamplingSet_Definition* existingDefinition,
                                       const SCOREP_SamplingSet_Definition* newDefinition );


SCOREP_SamplingSetHandle
SCOREP_DefineSamplingSet( uint8_t                    numberOfMetrics,
                          const SCOREP_MetricHandle* metrics,
                          SCOREP_MetricOccurrence    occurrence )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new sampling set with %hhu metrics",
                         numberOfMetrics );

    SCOREP_Definitions_Lock();

    SCOREP_SamplingSetHandle new_handle =
        scorep_sampling_set_definition_define( &scorep_local_definition_manager,
                                               numberOfMetrics,
                                               metrics,
                                               occurrence,
                                               NULL /* take the metric handles as is */ );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


SCOREP_SamplingSetHandle
SCOREP_DefineScopedSamplingSet( SCOREP_SamplingSetHandle samplingSet,
                                SCOREP_LocationHandle    recorderHandle,
                                SCOREP_MetricScope       scopeType,
                                SCOREP_AnyHandle         scopeHandle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new scoped sampling set" );

    SCOREP_Definitions_Lock();

    SCOREP_SamplingSetHandle new_handle =
        scorep_scoped_sampling_set_definition_define( &scorep_local_definition_manager,
                                                      samplingSet,
                                                      recorderHandle,
                                                      scopeType,
                                                      scopeHandle );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopySamplingSetDefinitionToUnified( SCOREP_SamplingSet_Definition* definition,
                                           SCOREP_Allocator_PageManager*  handlesPageManager )
{
    assert( !SCOREP_Omp_InParallel() );
    assert( definition );
    assert( handlesPageManager );

    if ( definition->is_scoped )
    {
        SCOREP_ScopedSamplingSet_Definition* scoped_definition
            = ( SCOREP_ScopedSamplingSet_Definition* )definition;

        assert( scoped_definition->sampling_set_handle != SCOREP_INVALID_SAMPLING_SET );
        assert( scoped_definition->recorder_handle != SCOREP_INVALID_LOCATION );
        assert( scoped_definition->scope_handle != SCOREP_MOVABLE_NULL );

        scoped_definition->unified = scorep_scoped_sampling_set_definition_define(
            scorep_unified_definition_manager,
            SCOREP_HANDLE_GET_UNIFIED( scoped_definition->sampling_set_handle,
                                       SamplingSet,
                                       handlesPageManager ),
            SCOREP_HANDLE_GET_UNIFIED( scoped_definition->recorder_handle,
                                       Location,
                                       handlesPageManager ),
            scoped_definition->scope_type,
            SCOREP_HANDLE_GET_UNIFIED( scoped_definition->scope_handle,
                                       Any,
                                       handlesPageManager ) );
    }
    else
    {
        definition->unified = scorep_sampling_set_definition_define(
            scorep_unified_definition_manager,
            definition->number_of_metrics,
            definition->metric_handles,
            definition->occurrence,
            handlesPageManager /* take the unified handles from the metrics */ );
    }
}


SCOREP_SamplingSetHandle
scorep_sampling_set_definition_define(
    SCOREP_DefinitionManager*     definition_manager,
    uint8_t                       numberOfMetrics,
    const SCOREP_MetricHandle*    metrics,
    SCOREP_MetricOccurrence       occurrence,
    SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( definition_manager );

    SCOREP_SamplingSet_Definition* new_definition = NULL;
    SCOREP_SamplingSetHandle       new_handle     = SCOREP_INVALID_SAMPLING_SET;

    SCOREP_DEFINITION_ALLOC_VARIABLE_ARRAY( SamplingSet,
                                            SCOREP_MetricHandle,
                                            numberOfMetrics );

    scorep_sampling_set_definition_initialize( new_definition,
                                               definition_manager,
                                               numberOfMetrics,
                                               metrics,
                                               occurrence,
                                               handlesPageManager );

    /* Does return */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( SamplingSet, sampling_set );
}


void
scorep_sampling_set_definition_initialize(
    SCOREP_SamplingSet_Definition* definition,
    SCOREP_DefinitionManager*      definition_manager,
    uint8_t                        numberOfMetrics,
    const SCOREP_MetricHandle*     metrics,
    SCOREP_MetricOccurrence        occurrence,
    SCOREP_Allocator_PageManager*  handlesPageManager )
{
    definition->is_scoped = false;
    HASH_ADD_POD( definition, is_scoped );

    definition->number_of_metrics = numberOfMetrics;
    HASH_ADD_POD( definition, number_of_metrics );

    /* Copy array of metrics */
    if ( handlesPageManager )
    {
        for ( uint8_t i = 0; i < numberOfMetrics; i++ )
        {
            definition->metric_handles[ i ] =
                SCOREP_HANDLE_GET_UNIFIED( metrics[ i ],
                                           Metric,
                                           handlesPageManager );
            assert( definition->metric_handles[ i ]
                    != SCOREP_INVALID_METRIC );
            HASH_ADD_HANDLE( definition, metric_handles[ i ], Metric );
        }
    }
    else
    {
        for ( uint8_t i = 0; i < numberOfMetrics; i++ )
        {
            definition->metric_handles[ i ] = metrics[ i ];
            assert( definition->metric_handles[ i ]
                    != SCOREP_INVALID_METRIC );
            HASH_ADD_HANDLE( definition, metric_handles[ i ], Metric );
        }
    }

    definition->occurrence = occurrence;
    HASH_ADD_POD( definition, occurrence );
}


SCOREP_SamplingSetHandle
scorep_scoped_sampling_set_definition_define(
    SCOREP_DefinitionManager* definition_manager,
    SCOREP_SamplingSetHandle  samplingSet,
    SCOREP_LocationHandle     recorderHandle,
    SCOREP_MetricScope        scopeType,
    SCOREP_AnyHandle          scopeHandle )
{
    assert( definition_manager );

    SCOREP_ScopedSamplingSet_Definition* new_definition = NULL;
    SCOREP_SamplingSetHandle             new_handle     = SCOREP_INVALID_SAMPLING_SET;

    SCOREP_DEFINITION_ALLOC( ScopedSamplingSet );
    scorep_scoped_sampling_set_definition_initialize( new_definition,
                                                      definition_manager,
                                                      samplingSet,
                                                      recorderHandle,
                                                      scopeType,
                                                      scopeHandle );

    SCOREP_ScopedSamplingSet_Definition* scoped_definition = new_definition;
    SCOREP_SamplingSetHandle             scoped_handle     = new_handle;

    /* ScopedSamplingSet overloads SamplingSet */
    {
        SCOREP_SamplingSet_Definition* new_definition
            = ( SCOREP_SamplingSet_Definition* )scoped_definition;
        SCOREP_SamplingSetHandle new_handle = scoped_handle;

        /* Does return */
        SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( SamplingSet,
                                                  sampling_set );
    }
}


void
scorep_scoped_sampling_set_definition_initialize(
    SCOREP_ScopedSamplingSet_Definition* definition,
    SCOREP_DefinitionManager*            definition_manager,
    SCOREP_SamplingSetHandle             samplingSet,
    SCOREP_LocationHandle                recorderHandle,
    SCOREP_MetricScope                   scopeType,
    SCOREP_AnyHandle                     scopeHandle )
{
    definition->is_scoped = true;
    HASH_ADD_POD( definition, is_scoped );

    definition->sampling_set_handle = samplingSet;
    HASH_ADD_HANDLE( definition, sampling_set_handle, SamplingSet );

    definition->recorder_handle = recorderHandle;
    HASH_ADD_HANDLE( definition, recorder_handle, Location );

    definition->scope_type = scopeType;
    HASH_ADD_POD( definition, scope_type );

    definition->scope_handle = scopeHandle;
    HASH_ADD_HANDLE( definition, scope_handle, Any );
}


bool
scorep_sampling_set_definitions_equal( const SCOREP_SamplingSet_Definition* existingDefinition,
                                       const SCOREP_SamplingSet_Definition* newDefinition )
{
    if ( existingDefinition->hash_value != newDefinition->hash_value
         || existingDefinition->is_scoped != newDefinition->is_scoped )
    {
        return false;
    }

    if ( existingDefinition->is_scoped )
    {
        SCOREP_ScopedSamplingSet_Definition* existing_scoped_definition
            = ( SCOREP_ScopedSamplingSet_Definition* )existingDefinition;
        SCOREP_ScopedSamplingSet_Definition* new_scoped_definition
            = ( SCOREP_ScopedSamplingSet_Definition* )newDefinition;
        return existing_scoped_definition->sampling_set_handle
               == new_scoped_definition->sampling_set_handle
               && existing_scoped_definition->recorder_handle
               == new_scoped_definition->recorder_handle
               && existing_scoped_definition->scope_type
               == new_scoped_definition->scope_type
               && existing_scoped_definition->scope_handle
               == new_scoped_definition->scope_handle;
    }
    else
    {
        return existingDefinition->number_of_metrics
               == newDefinition->number_of_metrics
               && 0 == memcmp( existingDefinition->metric_handles,
                               newDefinition->metric_handles,
                               sizeof( existingDefinition->metric_handles[ 0 ]
                                       * existingDefinition->number_of_metrics ) )
               && existingDefinition->occurrence
               == newDefinition->occurrence;
    }
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

    // Init new_definition
    // see ticket:423
    //SCOREP_DEBUG_NOT_YET_IMPLEMENTED();

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

    // Init new_definition
    // see ticket:423
    //SCOREP_DEBUG_NOT_YET_IMPLEMENTED();

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

    // Init new_definition
    // see ticket:423
    //SCOREP_DEBUG_NOT_YET_IMPLEMENTED();

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

    // Init new_definition
    // see ticket:423
    //SCOREP_DEBUG_NOT_YET_IMPLEMENTED();

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
                                    const SCOREP_Parameter_Definition* newDefinition );


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
    assert( !SCOREP_Omp_InParallel() );
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

    /* Does return */
    SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Parameter, parameter );
}


bool
scorep_parameter_definitions_equal( const SCOREP_Parameter_Definition* existingDefinition,
                                    const SCOREP_Parameter_Definition* newDefinition )
{
    return existingDefinition->hash_value == newDefinition->hash_value
           && existingDefinition->name_handle == newDefinition->name_handle
           && existingDefinition->parameter_type == newDefinition->parameter_type;
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
                                   const SCOREP_Callpath_Definition* newDefinition );


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
    assert( !SCOREP_Omp_InParallel() );
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
                SCOREP_BUG( "Not a valid parameter type." );
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

    /* Does return */
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
                SCOREP_BUG( "Not a valid parameter type." );
            }
        }
    }
}


bool
scorep_callpath_definitions_equal( const SCOREP_Callpath_Definition* existingDefinition,
                                   const SCOREP_Callpath_Definition* newDefinition )
{
    if ( existingDefinition->hash_value != newDefinition->hash_value
         || existingDefinition->parent_callpath_handle != newDefinition->parent_callpath_handle
         || existingDefinition->with_parameter != newDefinition->with_parameter )
    {
        return false;
    }

    if ( !existingDefinition->with_parameter )
    {
        return existingDefinition->callpath_argument.region_handle == newDefinition->callpath_argument.region_handle;
    }

    if ( existingDefinition->callpath_argument.parameter_handle
         != newDefinition->callpath_argument.parameter_handle )
    {
        return false;
    }

    /** @note: we need to deref parameter_handle to know the type,
     *         but we don't have the associated page manager
     *         thus, we use memcmp for comparison
     *         luckily, the union members have equal size, currently
     */
    return 0 == memcmp( &existingDefinition->parameter_value,
                        &newDefinition->parameter_value,
                        sizeof( existingDefinition->parameter_value ) );
}


/////////////////////////////////////////////////////////////////////////////
// ClockOffset //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Add a clock sync point into the local definitions
 */
void
SCOREP_AddClockOffset( uint64_t time,
                       int64_t  offset,
                       double   stddev )
{
    extern SCOREP_ClockOffset** scorep_clock_offset_tail;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "Add clock offset" );

    SCOREP_Definitions_Lock();

    SCOREP_AnyHandle new_handle =
        SCOREP_Memory_AllocForDefinitions( sizeof( SCOREP_ClockOffset ) );
    SCOREP_ClockOffset* new_clock_offset =
        SCOREP_MEMORY_DEREF_LOCAL( new_handle, SCOREP_ClockOffset* );
    new_clock_offset->next = NULL;

    new_clock_offset->time   = time;
    new_clock_offset->offset = offset;
    new_clock_offset->stddev = stddev;

    *scorep_clock_offset_tail = new_clock_offset;
    scorep_clock_offset_tail  = &new_clock_offset->next;

    SCOREP_Definitions_Unlock();
}


void
SCOREP_GetFirstClockSyncPair( int64_t*  offset1,
                              uint64_t* timestamp1,
                              int64_t*  offset2,
                              uint64_t* timestamp2 )
{
    extern SCOREP_ClockOffset* scorep_clock_offset_head;
    assert( scorep_clock_offset_head );
    assert( scorep_clock_offset_head->next );
    *offset1    = scorep_clock_offset_head->offset;
    *timestamp1 = scorep_clock_offset_head->time;
    *offset2    = scorep_clock_offset_head->next->offset;
    *timestamp2 = scorep_clock_offset_head->next->time;
    assert( *timestamp2 > *timestamp1 );
}


void
SCOREP_GetLastClockSyncPair( int64_t*  offset1,
                             uint64_t* timestamp1,
                             int64_t*  offset2,
                             uint64_t* timestamp2 )
{
    extern SCOREP_ClockOffset* scorep_clock_offset_head;
    assert( scorep_clock_offset_head );
    assert( scorep_clock_offset_head->next );
    SCOREP_ClockOffset* previous = scorep_clock_offset_head;
    SCOREP_ClockOffset* current  = previous->next;

    while ( current->next )
    {
        previous = current;
        current  = current->next;
    }

    *offset1    = previous->offset;
    *timestamp1 = previous->time;
    *offset2    = current->offset;
    *timestamp2 = current->time;
    assert( *timestamp2 > *timestamp1 );
}

/////////////////////////////////////////////////////////////////////////////
// convenient definition accessor functions /////////////////////////////////
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
 * Gets the adapter type of the region.
 *
 * @param handle A handle to the region.
 *
 * @return region\s adapter type.
 */
SCOREP_AdapterType
SCOREP_Region_GetAdapterType( SCOREP_RegionHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Region )->adapter_type;
}

/**
 * Gets the region first line of the region
 *
 * @param handle A handle to the region.
 *
 * @return region\s first line.
 */
SCOREP_LineNo
SCOREP_Region_GetBeginLine( SCOREP_RegionHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Region )->begin_line;
}

/**
 * Gets the region end line of the region.
 *
 * @param handle A handle to the region.
 *
 * @return region\s end line.
 */
SCOREP_LineNo
SCOREP_Region_GetEndLine( SCOREP_RegionHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Region )->end_line;
}
/**
 * @}
 */


/**
 * Parameter accessor functions for user adapters.
 * @{
 */

const char*
SCOREP_String_Get( SCOREP_StringHandle handle )
{
    SCOREP_String_Definition* str = SCOREP_LOCAL_HANDLE_DEREF( handle, String );

    return str->string_data;
}

/**
 * Gets read-only access to the name of the parameter.
 *
 * @param handle A handle to the parameter.
 *
 * @return parameter name.
 */
const char*
SCOREP_Parameter_GetName( SCOREP_ParameterHandle handle )
{
    SCOREP_Parameter_Definition* param = SCOREP_LOCAL_HANDLE_DEREF( handle, Parameter );

    return SCOREP_LOCAL_HANDLE_DEREF( param->name_handle, String )->string_data;
}

/**
 * Returns the type of the parameter.
 *
 * @param handle A handle to the parameter.
 *
 * @return parameter type.
 */
SCOREP_ParameterType
SCOREP_Parameter_GetType( SCOREP_ParameterHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Parameter )->parameter_type;
}

/**
 * Gets read-only access to the name of the source file.
 *
 * @param handle A handle to the source file definition.
 *
 * @return source file name.
 */
const char*
SCOREP_SourceFile_GetName( SCOREP_SourceFileHandle handle )
{
    SCOREP_SourceFile_Definition* source_file = SCOREP_LOCAL_HANDLE_DEREF( handle, SourceFile );

    return SCOREP_LOCAL_HANDLE_DEREF( source_file->name_handle, String )->string_data;
}

/**
 * Returns the sequence number of the unified definitions for a local callpath handle from
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

uint32_t
SCOREP_GetNumberOfRegionDefinitions()
{
    assert( !SCOREP_Omp_InParallel() );
    return scorep_local_definition_manager.region_definition_counter;
}

uint32_t
SCOREP_GetRegionHandleToID( SCOREP_RegionHandle handle )
{
    assert( !SCOREP_Omp_InParallel() );
    return SCOREP_LOCAL_HANDLE_TO_ID( handle, Region );
}

uint32_t
SCOREP_CallPathHandleToRegionID( SCOREP_CallpathHandle handle )
{
    SCOREP_Callpath_Definition* callpath = SCOREP_LOCAL_HANDLE_DEREF( handle, Callpath );

    return SCOREP_GetRegionHandleToID( callpath->callpath_argument.region_handle );
}

/**
 * Returns the sequence number of the unified definitions for a local metric handle from
 * the mappings.
 * @param handle handle to local metric handle.
 */
uint32_t
SCOREP_Metric_GetUnifiedSequenceNumber( SCOREP_CallpathHandle handle )
{
    uint32_t local_id = SCOREP_LOCAL_HANDLE_TO_ID( handle, Metric );
    return scorep_local_definition_manager.mappings->metric_mappings[ local_id ];
}

/**
 * Returns the unified handle from a local handle.
 */
SCOREP_MetricHandle
SCOREP_Metric_GetUnifiedHandle( SCOREP_MetricHandle handle )
{
    return SCOREP_HANDLE_GET_UNIFIED( handle, Metric,
                                      SCOREP_Memory_GetLocalDefinitionPageManager() );
}

/**
 * Returns the number of unified metric definitions.
 */
uint32_t
SCOREP_Metric_GetNumberOfUnifiedDefinitions()
{
    return scorep_unified_definition_manager->metric_definition_counter;
}

/**
 * Returns the value type of a metric.
 * @param handle to local meric definition.
 */
SCOREP_MetricValueType
SCOREP_Metric_GetValueType( SCOREP_MetricHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Metric )->value_type;
}

/**
 * Returns the name of a metric.
 * @param handle to local meric definition.
 */
const char*
SCOREP_Metric_GetName( SCOREP_MetricHandle handle )
{
    SCOREP_Metric_Definition* metric = SCOREP_LOCAL_HANDLE_DEREF( handle, Metric );

    return SCOREP_LOCAL_HANDLE_DEREF( metric->name_handle, String )->string_data;
}

SCOREP_MetricProfilingType
SCOREP_Metric_GetProfilingType( SCOREP_MetricHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Metric )->profiling_type;
}

/**
 * @}
 */
