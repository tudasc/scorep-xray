/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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
 * @file       scorep_unify.cpp
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>

#define __STDC_LIMIT_MACROS

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <tclap/CmdLine.h>
#include <cassert>
#include <otf2/OTF2_Reader.h>
#include <cstdio>
//#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <scorep_definitions.h>
#include <scorep_definition_macros.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Definitions.h>
#ifdef __cplusplus
} /* "C" */
#endif


extern SCOREP_DefinitionManager  scorep_local_definition_manager;
extern SCOREP_DefinitionManager* scorep_remote_definition_manager;

typedef std::map<uint32_t /* stringId */, SCOREP_StringHandle> scorep_unify_string_lookup;
static scorep_unify_string_lookup scorep_unify_string_handles;


//typedef std::map<uint32_t /* fileId */, SCOREP_SourceFileHandle> scorep_unify_file_lookup;
//static scorep_unify_file_lookup scorep_unify_file_handles;


typedef std::map<uint32_t /* regionId */, SCOREP_RegionHandle> scorep_unify_region_lookup;
static scorep_unify_region_lookup scorep_unify_region_handles;


typedef std::vector<uint64_t> scorep_unify_locations;
static scorep_unify_locations scorep_unify_locations_with_definitions;


//typedef struct scorep_unify_string_definition scorep_unify_string_definition;
//struct scorep_unify_string_definition
//{
//    SCOREP_String_Definition*       definition;
//    scorep_unify_string_definition* next;
//};
//
//scorep_unify_string_definition** string_definition_hash_table;


typedef struct scorep_unify_region_definition scorep_unify_region_definition;
struct scorep_unify_region_definition
{
    SCOREP_Region_Definition*       definition;
    scorep_unify_region_definition* next;
};

scorep_unify_region_definition** region_definition_hash_table;

static SCOREP_RegionType         scorep_unify_region_type_map[ OTF2_REGION_DYNAMIC_LOOP_PHASE + 1 ] = {
    SCOREP_REGION_UNKNOWN,
    SCOREP_REGION_FUNCTION,
    SCOREP_REGION_LOOP,
    SCOREP_REGION_USER,

    SCOREP_REGION_OMP_PARALLEL,
    SCOREP_REGION_OMP_LOOP,
    SCOREP_REGION_OMP_SECTIONS,
    SCOREP_REGION_OMP_SECTION,
    SCOREP_REGION_OMP_WORKSHARE,
    SCOREP_REGION_OMP_SINGLE,
    SCOREP_REGION_OMP_MASTER,
    SCOREP_REGION_OMP_CRITICAL,
    SCOREP_REGION_OMP_ATOMIC,
    SCOREP_REGION_OMP_BARRIER,
    SCOREP_REGION_OMP_IMPLICIT_BARRIER,
    SCOREP_REGION_OMP_FLUSH,
    SCOREP_REGION_OMP_CRITICAL_SBLOCK,
    SCOREP_REGION_OMP_SINGLE_SBLOCK,
    SCOREP_REGION_OMP_WRAPPER,

    SCOREP_REGION_MPI_COLL_BARRIER,
    SCOREP_REGION_MPI_COLL_ONE2ALL,
    SCOREP_REGION_MPI_COLL_ALL2ONE,
    SCOREP_REGION_MPI_COLL_ALL2ALL,
    SCOREP_REGION_MPI_COLL_OTHER,

    SCOREP_REGION_PHASE,
    SCOREP_REGION_DYNAMIC,

    SCOREP_REGION_DYNAMIC_PHASE,
    SCOREP_REGION_DYNAMIC_LOOP,
    SCOREP_REGION_DYNAMIC_FUNCTION,
    SCOREP_REGION_DYNAMIC_LOOP_PHASE
};


/* *INDENT-OFF* */
std::string scorep_unify_parse_cmd_line(int argc, char* argv[] );

SCOREP_Error_Code scorep_unify_on_read_string_definition( void* userData, uint32_t stringId, char* string );
SCOREP_Error_Code scorep_unify_on_read_location_definition( void* userdata, uint64_t locationId, uint32_t nameId, OTF2_LocationType locationType, uint64_t numberOfDefinitions );
SCOREP_Error_Code scorep_unify_on_read_location_group_definition( void* userdata, uint64_t locationId, uint32_t nameId, OTF2_LocationType locationType, uint64_t numberOfDefinitions, uint64_t locationsNumber, uint64_t* locations );
SCOREP_Error_Code scorep_unify_on_read_region_definition( void* userdata, uint32_t regionId, uint32_t regionNameId, uint32_t regionDescriptionId, OTF2_RegionType regionType, uint32_t sourceFileId, uint32_t beginLineNumber, uint32_t endLineNumber );
SCOREP_Error_Code scorep_unify_on_read_callsite_definition( void* userdata, uint32_t callsiteId, uint32_t sourceFileId, uint32_t lineNumber, uint32_t regionEnterd, uint32_t regionLeft );
SCOREP_Error_Code scorep_unify_on_read_callpath_definition( void* userdata, uint32_t callpathId, uint32_t parentCallpath, uint32_t regionId, uint8_t callPathOrder );
SCOREP_Error_Code scorep_unify_on_read_group_definition( void* userdata, uint64_t groupId, OTF2_GroupType groupType, uint32_t IDGS_name, uint64_t numberOfMembers, uint64_t* members );
SCOREP_Error_Code scorep_unify_on_read_topology_cartesian_definition( void* userdata, uint32_t cartesianTopologyId, uint32_t nameId, uint32_t locationsInEachDimensionNumber,uint32_t* numberOfLocationsInEachDimension, uint32_t periodicityOfTheGridInEachDimensionNumber, uint8_t* PeriodicityOfTheGridInEachDimension );
SCOREP_Error_Code scorep_unify_on_read_topology_cartesian_coords_definition( void* userdata, uint32_t cartesianTopologyId, uint64_t locationId, uint32_t numberOfDimensions, uint8_t* coordinatesOfTheLocation );
SCOREP_Error_Code scorep_unify_on_read_topology_graph_definition( void* userdata, uint32_t topologyGraphId, uint32_t nameId, uint8_t isDirected );
SCOREP_Error_Code scorep_unify_on_read_topology_graph_edge_definition( void* userdata, uint32_t topologyGraphId, uint64_t from, uint64_t to );
SCOREP_Error_Code scorep_unify_on_read_mpi_win_definition( void* userdata, uint32_t windowId, uint64_t communicatorId );

SCOREP_Error_Code scorep_unify_on_read_global_location_definition( void* userdata, uint64_t locationId, uint32_t name_id, OTF2_GlobLocationType locationType, uint64_t numberOfDefinitions );

void scorep_unify_build_local_hash_tables();
void scorep_unify_build_local_string_hash_table();
void scorep_unify_build_local_region_hash_table();

void scorep_unify_remote_definitions_initialize();
void scorep_unify_remote_definitions_finalize();
/* *INDENT-ON*  */


int
main( int   argc,
      char* argv[] )
{
    SCOREP_Error_Code status          = SCOREP_SUCCESS;
    std::string       path_to_archive = scorep_unify_parse_cmd_line( argc, argv );

    std::cout << "archive " << path_to_archive <<  std::endl;
    OTF2_Reader* reader = OTF2_Reader_New( path_to_archive.c_str() );
    assert( reader );
    std::cout << "OTF2_Reader_New ready" << std::endl;

    OTF2_GlobDefReader* global_definition_reader = OTF2_Reader_GetGlobDefReader( reader );
    assert( global_definition_reader );
    std::cout << "GetGlobDefReader ready" << std::endl;
    OTF2_GlobDefReaderCallbacks get_locations_with_definitions;
    memset( &get_locations_with_definitions, 0, sizeof( get_locations_with_definitions ) );
    get_locations_with_definitions.GlobDefLocation = scorep_unify_on_read_global_location_definition;

    // currently broken
    uint64_t dummy;
    status = OTF2_Reader_RegisterGlobDefCallbacks( reader, global_definition_reader, get_locations_with_definitions, 0 );
    assert( status == SCOREP_SUCCESS );
    status = OTF2_Reader_ReadGlobalDefinitions( reader, global_definition_reader, OTF2_UNDEFINED_UINT64, &dummy );
    assert( status == SCOREP_SUCCESS );
    int n_procs = scorep_unify_locations_with_definitions.size();
    assert( n_procs > 0 );
    std::cout << "ReadGlobalDefinitions ready" << std::endl;

    // read definition of rank 0 (called local) to memory via call back
    int                     rank                    = 0;
    OTF2_DefReader*         local_definition_reader = OTF2_Reader_GetDefReader( reader, rank );

    OTF2_DefReaderCallbacks read_definitions;
    memset( &read_definitions, 0, sizeof( read_definitions ) );
    read_definitions.DefString                  = scorep_unify_on_read_string_definition;
    read_definitions.DefLocation                = scorep_unify_on_read_location_definition;
    read_definitions.DefLocationGroup           = scorep_unify_on_read_location_group_definition;
    read_definitions.DefRegion                  = scorep_unify_on_read_region_definition;
    read_definitions.DefCallsite                = scorep_unify_on_read_callsite_definition;
    read_definitions.DefCallpath                = scorep_unify_on_read_callpath_definition;
    read_definitions.DefGroup                   = scorep_unify_on_read_group_definition;
    read_definitions.DefTopologyCartesian       = scorep_unify_on_read_topology_cartesian_definition;
    read_definitions.DefTopologyCartesianCoords = scorep_unify_on_read_topology_cartesian_coords_definition;
    read_definitions.DefTopologyGraph           = scorep_unify_on_read_topology_graph_definition;
    read_definitions.DefTopologyGraphEdge       = scorep_unify_on_read_topology_graph_edge_definition;
    read_definitions.DefMpiWin                  = scorep_unify_on_read_mpi_win_definition;

    SCOREP_Memory_Initialize( 150 * 8192, 8192 );
    SCOREP_Definitions_Initialize();

    std::cout << "ReadLocalDefinitions start" << std::endl;
    bool read_remote_definitions = false;
    status = OTF2_Reader_RegisterDefCallbacks( reader, local_definition_reader, read_definitions, ( void* )read_remote_definitions );
    assert( status == SCOREP_SUCCESS );
    status = OTF2_Reader_ReadLocalDefinitions( reader, local_definition_reader, OTF2_UNDEFINED_UINT64, &dummy );
    assert( status == SCOREP_SUCCESS );
    std::cout << "ReadLocalDefinitions ready" << std::endl;

    scorep_unify_build_local_hash_tables();
    std::cout << "Building Hash Table ready" << std::endl;


    /*
       create mapping for local definitions (1:1 mapping)
       OTF2_Reader_ResetDefReader?( reader, rank, void* my_data, write_local_definitions_and_mappings );
       // this triggers write_local_definitions_and_mappings(OTF2_DefWriter* writer, my_data)
       // where my data points to local_movable_memory and the mapping_table
     */

    read_remote_definitions = true;
    scorep_unify_locations::iterator location = scorep_unify_locations_with_definitions.begin();
    for ( ++location; // can't read first location again.
          location != scorep_unify_locations_with_definitions.end(); ++location )
    {
        scorep_unify_region_handles.clear();
        scorep_unify_string_handles.clear();
        std::cout << "unifying location 0 with location " << *location << std::endl;
        scorep_unify_remote_definitions_initialize();
        OTF2_DefReader* remote_definition_reader = OTF2_Reader_GetDefReader( reader, *location );
        assert( remote_definition_reader );
        status = OTF2_Reader_RegisterDefCallbacks( reader, remote_definition_reader, read_definitions, ( void* )read_remote_definitions );
        assert( status == SCOREP_SUCCESS );
        status = OTF2_Reader_ReadLocalDefinitions( reader, remote_definition_reader, OTF2_UNDEFINED_UINT64, &dummy );
        assert( status == SCOREP_SUCCESS );

        // for each def type
        // create id_map of appropriate size
        // SCOREP_IdMap* string_id_map = SCOREP_IdMap_Create( SCOREP_ID_MAP_DENSE, scorep_remote_definition_manager->string_definition_counter - 1 );
        // for each remote definition
        // matching_local_def = search_in_local_hash_map(remote_def)
        // if matching_local_def
        // SCOREP_IdMap_AddIdPair(remote->sequence_number, matching_local_def->sequence_number)
        // else
        // append remote to local_definition_manager, i.e. create new_local_def
        // update local_hash_table
        // SCOREP_IdMap_AddIdPair(remote->sequence_number, new_local_def->sequence_number)

        // write id_map and unchanged remote_defs back
        scorep_unify_remote_definitions_finalize();
    }

    /*

       OTF2_GlobReader_ResetDefReader?( global_definition_reader, void* my_data, scorep_unify_write_global_definitions );
       // this triggers scorep_unify_write_global_definitions (OTF2_GlobDefWriter* writer, void* my_data) where
       // my_data is local_movable_memory

       clear local_movable_memory

     */

    SCOREP_Definitions_Finalize();
    SCOREP_Memory_Finalize();

    status = OTF2_Reader_Delete( reader );
    assert( status == SCOREP_SUCCESS );

    return 0;
}


std::string
scorep_unify_parse_cmd_line( int   argc,
                             char* argv[] )
{
    try {
        TCLAP::CmdLine
        cmd( "scorep_unify will unify the local definitions from a provided archive.",
             ' ',
             "0.9" ); // add scorep version here

        TCLAP::ValueArg<std::string>
        archive_argument( "a",
                          "anchor_file",
                          "Path to the archive's anchor file.",
                          true,
                          "",
                          "string" );
        cmd.add( archive_argument );
        cmd.parse( argc, argv );

        return archive_argument.getValue();
    }
    catch ( TCLAP::ArgException &e )  // catch any exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
}


SCOREP_Error_Code
scorep_unify_on_read_string_definition( void*    readingRemoteDefinitions,
                                        uint32_t stringId,
                                        char*    string )
{
    SCOREP_String_Definition* new_definition = NULL;
    SCOREP_StringHandle       new_handle     = NULL;

    uint32_t                  string_length = strlen( string ) + 1;

    if ( static_cast<bool>( readingRemoteDefinitions ) )
    {
        SCOREP_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY_OLD( String,
                                                        string,
                                                        char,
                                                        string_length,
                                                        scorep_remote_definition_manager );
    }
    else
    {
        SCOREP_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY_OLD( String,
                                                        string,
                                                        char,
                                                        string_length,
                                                        &scorep_local_definition_manager );
    }

    assert( new_definition->sequence_number == stringId );

    new_definition->string_length = string_length;
    strcpy( new_definition->string_data, string );
    new_definition->hash_value = hash( new_definition->string_data, new_definition->string_length, 0 );

    assert( scorep_unify_string_handles.find( stringId ) == scorep_unify_string_handles.end() );
    scorep_unify_string_handles[ stringId ] = new_handle;

    if ( static_cast<bool>( readingRemoteDefinitions ) )
    {
        std::cout << "string (remote) " << stringId << " " << string << std::endl;
    }
    else
    {
        std::cout << "string (local)  " << stringId << " " << string << std::endl;
    }

    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_location_definition( void*             userdata,
                                          uint64_t          locationId,
                                          uint32_t          nameId,
                                          OTF2_LocationType locationType,
                                          uint64_t          numberOfDefinitions )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_global_location_definition( void*                 userdata,
                                                 uint64_t              locationId,
                                                 uint32_t              nameId,
                                                 OTF2_GlobLocationType locationType,
                                                 uint64_t              numberOfDefinitions )
{
    std::cout << "locationId " << locationId << ", numberOfDefinitions " << numberOfDefinitions << std::endl;
    if ( numberOfDefinitions > 0 )
    {
        scorep_unify_locations_with_definitions.push_back( locationId );
    }
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_location_group_definition( void*             userdata,
                                                uint64_t          locationId,
                                                uint32_t          nameId,
                                                OTF2_LocationType locationType,
                                                uint64_t          numberOfDefinitions,
                                                uint64_t          locationsNumber,
                                                uint64_t*         locations )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_region_definition( void*           readingRemoteDefinitions,
                                        uint32_t        regionId,
                                        uint32_t        regionNameId,
                                        uint32_t        regionDescriptionId,
                                        OTF2_RegionType regionType,
                                        uint32_t        sourceFileId, // is actually a reference to a string
                                        uint32_t        beginLineNumber,
                                        uint32_t        endLineNumber )
{
    SCOREP_Region_Definition* new_definition = NULL;
    SCOREP_RegionHandle       new_handle     = SCOREP_MOVABLE_NULL;

    if ( static_cast<bool>( readingRemoteDefinitions ) )
    {
        SCOREP_ALLOC_NEW_DEFINITION_OLD( Region, region, scorep_remote_definition_manager );
    }
    else
    {
        SCOREP_ALLOC_NEW_DEFINITION_OLD( Region, region, &scorep_local_definition_manager );
    }

    assert( new_definition->sequence_number == regionId );

    // Init new_definition
    scorep_unify_string_lookup::iterator string_handle_it = scorep_unify_string_handles.find( regionNameId );
    assert( string_handle_it != scorep_unify_string_handles.end() );
    new_definition->name_handle = string_handle_it->second;
    HASH_ADD_HANDLE( new_definition, name_handle, String );

    string_handle_it = scorep_unify_string_handles.find( regionDescriptionId );
    assert( string_handle_it != scorep_unify_string_handles.end() );
    new_definition->description_handle = string_handle_it->second; // currently not used
    HASH_ADD_HANDLE( new_definition, description_handle, String );
    new_definition->region_type = scorep_unify_region_type_map[ regionType ];
    HASH_ADD_POD( new_definition, region_type );

    if ( sourceFileId == OTF2_UNDEFINED_UINT32 )
    {
        new_definition->file_name_handle = SCOREP_MOVABLE_NULL;
        /* should we add a 0 value to the hash? */
    }
    else
    {
        scorep_unify_string_lookup::iterator file_name_handle_it = scorep_unify_string_handles.find( sourceFileId );
        assert( file_name_handle_it != scorep_unify_string_handles.end() );
        new_definition->file_name_handle = file_name_handle_it->second;
        HASH_ADD_HANDLE( new_definition, file_name_handle, SourceFile );
        //std::cout << "file " << SCOREP_MEMORY_DEREF_MOVABLE( &( new_definition->file_name_handle ), SCOREP_String_Definition* )->string_data << std::endl;
    }


    new_definition->begin_line = beginLineNumber;
    HASH_ADD_POD( new_definition, begin_line );
    new_definition->end_line = endLineNumber;
    HASH_ADD_POD( new_definition, end_line );
    new_definition->adapter_type = SCOREP_INVALID_ADAPTER_TYPE;       // currently not used
    HASH_ADD_POD( new_definition, adapter_type );

    assert( scorep_unify_region_handles.find( regionId ) == scorep_unify_region_handles.end() );
    scorep_unify_region_handles[ regionId ] = new_handle;

    std::cout << "region " << regionId << std::endl;
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_callsite_definition( void*    userdata,
                                          uint32_t callsiteId,
                                          uint32_t sourceFileId,
                                          uint32_t lineNumber,
                                          uint32_t regionEnterd,
                                          uint32_t regionLeft )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_callpath_definition( void*    userdata,
                                          uint32_t callpathId,
                                          uint32_t parentCallpath,
                                          uint32_t regionId,
                                          uint8_t  callPathOrder )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_group_definition( void*          userdata,
                                       uint64_t       groupId,
                                       OTF2_GroupType groupType,
                                       uint32_t       IDGS_name,
                                       uint64_t       numberOfMembers,
                                       uint64_t*      members )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_topology_cartesian_definition( void*     userdata,
                                                    uint32_t  cartesianTopologyId,
                                                    uint32_t  nameId,
                                                    uint32_t  locationsInEachDimensionNumber,
                                                    uint32_t* numberOfLocationsInEachDimension,
                                                    uint32_t  periodicityOfTheGridInEachDimensionNumber,
                                                    uint8_t*  PeriodicityOfTheGridInEachDimension )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_topology_cartesian_coords_definition( void*    userdata,
                                                           uint32_t cartesianTopologyId,
                                                           uint64_t locationId,
                                                           uint32_t numberOfDimensions,
                                                           uint8_t* coordinatesOfTheLocation )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_topology_graph_definition( void*    userdata,
                                                uint32_t topologyGraphId,
                                                uint32_t nameId,
                                                uint8_t  isDirected )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_topology_graph_edge_definition( void*    userdata,
                                                     uint32_t topologyGraphId,
                                                     uint64_t from,
                                                     uint64_t to )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_read_mpi_win_definition( void*    userdata,
                                         uint32_t windowId,
                                         uint64_t communicatorId )
{
    return SCOREP_SUCCESS;
}


void
scorep_unify_remote_definitions_initialize()
{
    assert( scorep_remote_definition_manager == 0 );
    scorep_remote_definition_manager = new SCOREP_DefinitionManager(); //*)calloc(1, sizeof(SCOREP_DefinitionManager));

    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( string, scorep_remote_definition_manager );
    // do we need this here?
    ( *scorep_remote_definition_manager ).string_definition_hash_table = static_cast<SCOREP_StringHandle*>(
        calloc( SCOREP_DEFINITION_HASH_TABLE_SIZE,
                sizeof( *( *scorep_remote_definition_manager ).string_definition_hash_table ) ) );
    assert( ( *scorep_remote_definition_manager ).string_definition_hash_table );

    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( location, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( source_file, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( region, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( group, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( mpi_window, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( mpi_cartesian_topology, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( mpi_cartesian_coords, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( counter_group, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( counter, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( io_file_group, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( io_file, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( marker_group, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( marker, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( parameter, scorep_remote_definition_manager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( callpath, scorep_remote_definition_manager );
}


void
scorep_unify_remote_definitions_finalize()
{
    // free remote pages
    delete scorep_remote_definition_manager;
    scorep_remote_definition_manager = 0;
}


void
scorep_unify_build_local_hash_tables()
{
    scorep_unify_build_local_string_hash_table();
    scorep_unify_build_local_region_hash_table();
}


bool
scorep_unify_string_definitions_equal( const SCOREP_String_Definition* local,
                                       const SCOREP_String_Definition* remote )
{
    return local->hash_value       == remote->hash_value
           && local->string_length == remote->string_length
           && 0 == strcmp( local->string_data, remote->string_data );
}


bool
scorep_unify_region_definitions_equal( const SCOREP_Region_Definition* local,
                                       const SCOREP_Region_Definition* remote,
                                       SCOREP_Allocator_PageManager*   movedPageManager )
{
    return local->hash_value      == remote->hash_value
           && local->region_type  == remote->region_type
           && local->begin_line   == remote->begin_line
           && local->end_line     == remote->end_line
           && local->adapter_type == remote->adapter_type
           && scorep_unify_string_definitions_equal( SCOREP_LOCAL_HANDLE_DEREF( local->name_handle, String ),
                                                     SCOREP_HANDLE_DEREF( remote->name_handle, String, movedPageManager ) )
           && scorep_unify_string_definitions_equal( SCOREP_LOCAL_HANDLE_DEREF( local->file_name_handle, String ),
                                                     SCOREP_HANDLE_DEREF( remote->file_name_handle, String, movedPageManager ) );
}


void
scorep_unify_build_local_string_hash_table()
{
//    assert( string_definition_hash_table == 0 );
//    string_definition_hash_table = ( scorep_unify_string_definition** )
//                                   calloc( SCOREP_DEFINITION_HASH_TABLE_SIZE, sizeof( scorep_unify_string_definition* ) );
//    assert( string_definition_hash_table );
//    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, String, string )
//    {
//        //std::cout << "definition->string_data " << definition->string_data << std::endl;
//        uint32_t hash_value = definition->hash_value;
//        //assert( hash_value != 0 );
//        //std::cout << "definition->hash_value  " << ( long )definition->hash_value << std::endl;
//        scorep_unify_string_definition** hash_table_bucket =
//            &string_definition_hash_table[ definition->hash_value & SCOREP_DEFINITION_HASH_TABLE_MASK ];
//        scorep_unify_string_definition*  hash_list_iterator = *hash_table_bucket;
//        while ( hash_list_iterator != 0 )
//        {
//            if ( scorep_unify_string_definitions_equal( hash_list_iterator->definition, definition ) )
//            {
//                assert( 0 ); // should already be unique
//            }
//            hash_list_iterator = hash_list_iterator->next;
//        }
//        // append new definition at front
//        scorep_unify_string_definition* append_me = ( scorep_unify_string_definition* )malloc( sizeof( scorep_unify_string_definition ) );
//        append_me->definition = definition;
//        append_me->next       = *hash_table_bucket;
//        *hash_table_bucket    = append_me;
//    }
//    SCOREP_DEFINITION_FOREACH_WHILE();
}


void
scorep_unify_build_local_region_hash_table()
{
    assert( region_definition_hash_table == 0 );
    region_definition_hash_table = ( scorep_unify_region_definition** )
                                   calloc( SCOREP_DEFINITION_HASH_TABLE_SIZE, sizeof( scorep_unify_region_definition* ) );
    assert( region_definition_hash_table );
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, Region, region )
    {
        uint32_t                         hash_value        = definition->hash_value;
        scorep_unify_region_definition** hash_table_bucket =
            &region_definition_hash_table[ definition->hash_value & SCOREP_DEFINITION_HASH_TABLE_MASK ];
        scorep_unify_region_definition*  hash_list_iterator = *hash_table_bucket;
        while ( hash_list_iterator != 0 )
        {
            if ( scorep_unify_region_definitions_equal( hash_list_iterator->definition, definition, SCOREP_Memory_GetLocalDefinitionPageManager() ) )
            {
                assert( 0 ); // should already be unique, hm, is this always true?
            }
            hash_list_iterator = hash_list_iterator->next;
        }
        // append new definition at front
        scorep_unify_region_definition* append_me = ( scorep_unify_region_definition* )malloc( sizeof( scorep_unify_region_definition ) );
        append_me->definition = definition;
        append_me->next       = *hash_table_bucket;
        *hash_table_bucket    = append_me;
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}
