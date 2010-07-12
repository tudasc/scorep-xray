/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       silc_unify.cpp
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#define __STDC_LIMIT_MACROS

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <tclap/CmdLine.h>
#include <cassert>
#include <OTF2_Reader.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <silc_definitions.h>
#include <silc_definition_locking.h>
#include <SILC_Memory.h>
#include <SILC_Definitions.h>
#ifdef __cplusplus
} /* "C" */
#endif


/**
 * Allocate, assign the sequence number, and store in manager list a new
 * definition of type @type
 */
/* *INDENT-OFF* */
#define SILC_ALLOC_NEW_REMOTE_DEFINITION( Type, type ) \
    do { \
        new_movable = ( SILC_ ## Type ## _Definition_Movable* ) \
            SILC_Memory_AllocForDefinitions( \
                sizeof( SILC_ ## Type ## _Definition ) ); \
        new_definition = \
            SILC_MEMORY_DEREF_MOVABLE( new_movable, \
                                       SILC_ ## Type ## _Definition* ); \
        SILC_ALLOCATOR_MOVABLE_INIT_NULL( ( new_definition )->next ); \
        *silc_remote_definition_manager->type ## _definition_tail_pointer = \
            *new_movable; \
        silc_remote_definition_manager->type ## _definition_tail_pointer = \
            &( new_definition )->next; \
        ( new_definition )->sequence_number = \
            silc_remote_definition_manager->type ## _definition_counter++; \
    } while ( 0 )
/* *INDENT-ON* */


/**
 * Allocate, assign the sequence number, and store in manager list a new
 * definition of type @type with a variable array member of type @array_type
 * and a total number of members of @number_of_members
 */
/* *INDENT-OFF* */
#define SILC_ALLOC_NEW_REMOTE_DEFINITION_VARIABLE_ARRAY( Type, \
                                                         type, \
                                                         array_type, \
                                                         number_of_members ) \
    do { \
        new_movable = ( SILC_ ## Type ## _Definition_Movable* ) \
            SILC_Memory_AllocForDefinitions( \
                sizeof( SILC_ ## Type ## _Definition ) + \
                ( ( number_of_members ) - 1 ) * sizeof( array_type ) ); \
        new_definition = \
            SILC_MEMORY_DEREF_MOVABLE( new_movable, \
                                       SILC_ ## Type ## _Definition* ); \
        SILC_ALLOCATOR_MOVABLE_INIT_NULL( ( new_definition )->next ); \
        *silc_remote_definition_manager->type ## _definition_tail_pointer = \
            *new_movable; \
        silc_remote_definition_manager->type ## _definition_tail_pointer = \
            &( new_definition )->next; \
        ( new_definition )->sequence_number = \
            silc_remote_definition_manager->type ## _definition_counter++; \
    } while ( 0 )
/* *INDENT-ON* */


extern SILC_DefinitionManager silc_definition_manager;
extern SILC_DefinitionManager* silc_remote_definition_manager;

typedef std::map<uint32_t /* stringId */, SILC_StringHandle> silc_unify_string_lookup;
static silc_unify_string_lookup silc_unify_string_handles;


//typedef std::map<uint32_t /* fileId */, SILC_SourceFileHandle> silc_unify_file_lookup;
//static silc_unify_file_lookup silc_unify_file_handles;


typedef std::map<uint32_t /* regionId */, SILC_RegionHandle> silc_unify_region_lookup;
static silc_unify_region_lookup silc_unify_region_handles;


typedef std::vector<uint64_t> silc_unify_locations;
static silc_unify_locations silc_unify_locations_with_definitions;


static SILC_RegionType silc_unify_region_type_map[ OTF2_REGION_DYNAMIC_LOOP_PHASE + 1 ] = {
    SILC_REGION_UNKNOWN,
    SILC_REGION_FUNCTION,
    SILC_REGION_LOOP,
    SILC_REGION_USER,

    SILC_REGION_OMP_PARALLEL,
    SILC_REGION_OMP_LOOP,
    SILC_REGION_OMP_SECTIONS,
    SILC_REGION_OMP_SECTION,
    SILC_REGION_OMP_WORKSHARE,
    SILC_REGION_OMP_SINGLE,
    SILC_REGION_OMP_MASTER,
    SILC_REGION_OMP_CRITICAL,
    SILC_REGION_OMP_ATOMIC,
    SILC_REGION_OMP_BARRIER,
    SILC_REGION_OMP_IMPLICIT_BARRIER,
    SILC_REGION_OMP_FLUSH,
    SILC_REGION_OMP_CRITICAL_SBLOCK,
    SILC_REGION_OMP_SINGLE_SBLOCK,
    SILC_REGION_OMP_WRAPPER,

    SILC_REGION_MPI_COLL_BARRIER,
    SILC_REGION_MPI_COLL_ONE2ALL,
    SILC_REGION_MPI_COLL_ALL2ONE,
    SILC_REGION_MPI_COLL_ALL2ALL,
    SILC_REGION_MPI_COLL_OTHER,

    SILC_REGION_PHASE,
    SILC_REGION_DYNAMIC,

    SILC_REGION_DYNAMIC_PHASE,
    SILC_REGION_DYNAMIC_LOOP,
    SILC_REGION_DYNAMIC_FUNCTION,
    SILC_REGION_DYNAMIC_LOOP_PHASE
};


/* *INDENT-OFF* */
std::string silc_unify_parse_cmd_line(int argc, char* argv[] );

SILC_Error_Code silc_unify_on_number_of_processes(void* userdata, uint32_t nProcsIdentifier, int nProcs);

SILC_Error_Code silc_unify_on_string_definition( void* userData, uint32_t stringId, char* string );
SILC_Error_Code silc_unify_on_location_definition( void* userdata, uint64_t locationId, char* name, OTF2_LocationType locationType, uint64_t numberOfDefinitions );
SILC_Error_Code silc_unify_on_location_group_definition( void* userdata, uint64_t locationId, char* name, OTF2_LocationType locationType, uint64_t numberOfDefinitions, uint64_t locationsNumber, uint64_t* locations );
SILC_Error_Code silc_unify_on_region_definition( void* userdata, uint32_t regionId, uint32_t regionNameId, uint32_t regionDescriptionId, OTF2_RegionType regionType, uint32_t sourceFileId, uint32_t beginLineNumber, uint32_t endLineNumber );
SILC_Error_Code silc_unify_on_callsite_definition( void* userdata, uint32_t callsiteId, uint32_t sourceFileId, uint32_t lineNumber, uint32_t regionEnterd, uint32_t regionLeft );
SILC_Error_Code silc_unify_on_callpath_definition( void* userdata, uint32_t callpathId, uint32_t parentCallpath, uint32_t regionId, uint8_t callPathOrder );
SILC_Error_Code silc_unify_on_group_definition( void* userdata, uint64_t groupId, OTF2_GroupType groupType, uint64_t numberOfMembers, uint64_t* members );
SILC_Error_Code silc_unify_on_topology_cartesian_definition( void* userdata, uint32_t cartesianTopologyId, char* name, uint32_t locationsInEachDimensionNumber,uint32_t* numberOfLocationsInEachDimension, uint32_t periodicityOfTheGridInEachDimensionNumber, uint8_t* PeriodicityOfTheGridInEachDimension );
SILC_Error_Code silc_unify_on_topology_cartesian_coords_definition( void* userdata, uint32_t cartesianTopologyId, uint64_t locationId, uint32_t numberOfDimensions, uint8_t* coordinatesOfTheLocation );
SILC_Error_Code silc_unify_on_topology_graph_definition( void* userdata, uint32_t topologyGraphId, char* name, uint8_t isDirected );
SILC_Error_Code silc_unify_on_topology_graph_edge_definition( void* userdata, uint32_t topologyGraphId, uint64_t from, uint64_t to );
SILC_Error_Code silc_unify_on_mpi_win_definition( void* userdata, uint32_t windowId, uint64_t communicatorId );

SILC_Error_Code silc_unify_on_global_location_definition( void* userdata, uint64_t locationId, char* name, OTF2_GlobLocationType locationType, uint64_t numberOfDefinitions );

void silc_unify_remote_definitions_initialize();
void silc_unify_remote_definitions_finalize();
/* *INDENT-ON*  */


int
main( int   argc,
      char* argv[] )
{
    std::string path_to_archive = silc_unify_parse_cmd_line( argc, argv );

    std::cout << "archive " << path_to_archive <<  std::endl;
    OTF2_Reader* reader = OTF2_Reader_New( path_to_archive.c_str() );
    assert( reader );
    std::cout << "OTF2_Reader_New ready" << std::endl;

    OTF2_GlobDefReader* global_definition_reader = OTF2_Reader_GetGlobDefReader( reader );
    assert( global_definition_reader );
    std::cout << "GetGlobDefReader ready" << std::endl;
    OTF2_GlobDefReaderCallbacks get_locations_with_definitions;
    memset( &get_locations_with_definitions, 0, sizeof( get_locations_with_definitions ) );
    get_locations_with_definitions.GlobDefLocation = silc_unify_on_global_location_definition;

    // currently broken
    uint64_t dummy;
//    OTF2_Reader_RegisterGlobDefCallbacks( reader, global_definition_reader, get_locations_with_definitions, 0 );
//    OTF2_Reader_ReadGlobalDefinitions( reader, global_definition_reader, OTF2_UNDEFINED_UINT64, &dummy );
//    int n_procs = silc_unify_locations_with_definitions.size();
//    //assert( n_procs > 0 );
//    std::cout << "ReadGlobalDefinitions ready" << std::endl;

    // read definition of rank 0 (called local) to memory via call back
    int                     rank                    = 0;
    OTF2_DefReader*         local_definition_reader = OTF2_Reader_GetDefReader( reader, rank );

    OTF2_DefReaderCallbacks read_definitions;
    memset( &read_definitions, 0, sizeof( read_definitions ) );
    read_definitions.DefString                  = silc_unify_on_string_definition;
    read_definitions.DefLocation                = silc_unify_on_location_definition;
    read_definitions.DefLocationGroup           = silc_unify_on_location_group_definition;
    read_definitions.DefRegion                  = silc_unify_on_region_definition;
    read_definitions.DefCallsite                = silc_unify_on_callsite_definition;
    read_definitions.DefCallpath                = silc_unify_on_callpath_definition;
    read_definitions.DefGroup                   = silc_unify_on_group_definition;
    read_definitions.DefTopologyCartesian       = silc_unify_on_topology_cartesian_definition;
    read_definitions.DefTopologyCartesianCoords = silc_unify_on_topology_cartesian_coords_definition;
    read_definitions.DefTopologyGraph           = silc_unify_on_topology_graph_definition;
    read_definitions.DefTopologyGraphEdge       = silc_unify_on_topology_graph_edge_definition;
    read_definitions.DefMpiWin                  = silc_unify_on_mpi_win_definition;

    SILC_Memory_Initialize( 150 * 8192, 8192 );
    SILC_DefinitionLocks_Initialize();
    SILC_Definitions_Initialize();

    bool read_remote_definitions = 0;
    OTF2_Reader_RegisterDefCallbacks( reader, local_definition_reader, read_definitions, ( void* )read_remote_definitions );
    OTF2_Reader_ReadLocalDefinitions( reader, local_definition_reader, OTF2_UNDEFINED_UINT64, &dummy );

    /*
       create mapping for local definitions (1:1 mapping)
       OTF2_Reader_ResetDefReader?( reader, rank, void* my_data, write_local_definitions_and_mappings );
       // this triggers write_local_definitions_and_mappings(OTF2_DefWriter* writer, my_data)
       // where my data points to local_movable_memory and the mapping_table
     */

    // workaround until global location reading works.
    silc_unify_locations_with_definitions.push_back( 0 );
    //silc_unify_locations_with_definitions.push_back(1);
    read_remote_definitions = true;
    for ( silc_unify_locations::iterator location = silc_unify_locations_with_definitions.begin();
          location != silc_unify_locations_with_definitions.end(); ++location )
    {
        std::cout << "unifying location 0 with location " << *location << std::endl;
        silc_unify_remote_definitions_initialize();
        OTF2_DefReader* remote_definition_reader = OTF2_Reader_GetDefReader( reader, *location );
        OTF2_Reader_RegisterDefCallbacks( reader, remote_definition_reader, read_definitions, ( void* )read_remote_definitions );
        OTF2_Reader_ReadLocalDefinitions( reader, remote_definition_reader, OTF2_UNDEFINED_UINT64, &dummy );

        // create hash
        // unify

        silc_unify_remote_definitions_finalize();
    }

    /*

       OTF2_GlobReader_ResetDefReader?( global_definition_reader, void* my_data, silc_unify_write_global_definitions );
       // this triggers silc_unify_write_global_definitions (OTF2_GlobDefWriter* writer, void* my_data) where
       // my_data is local_movable_memory

       clear local_movable_memory

     */

    SILC_Definitions_Finalize();
    SILC_DefinitionLocks_Finalize();
    SILC_Memory_Finalize();

    SILC_Error_Code status = OTF2_Reader_Delete( reader );
    assert( status == SILC_SUCCESS );

    return 0;
}


std::string
silc_unify_parse_cmd_line( int   argc,
                           char* argv[] )
{
    try {
        TCLAP::CmdLine
        cmd( "silc_unify will unify the local definitions from a provided archive.",
             ' ',
             "0.9" ); // add silc version here

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


SILC_Error_Code
silc_unify_on_number_of_processes( void*    userdata,
                                   uint32_t nProcsIdentifier,
                                   int      nProcs )
{
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_string_definition( void*    readingRemoteDefinitions,
                                 uint32_t stringId,
                                 char*    string )
{
    SILC_String_Definition*         new_definition = NULL;
    SILC_String_Definition_Movable* new_movable    = NULL;

    uint32_t                        string_length = strlen( string ) + 1;

    if ( static_cast<bool>( readingRemoteDefinitions ) )
    {
        SILC_ALLOC_NEW_REMOTE_DEFINITION_VARIABLE_ARRAY( String,
                                                         string,
                                                         char,
                                                         string_length );
    }
    else
    {
        SILC_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY( String,
                                                  string,
                                                  char,
                                                  string_length );
    }

    assert( new_definition->sequence_number == stringId );

    new_definition->string_length = string_length;
    strcpy( new_definition->string_data, string );

    assert( silc_unify_string_handles.find( stringId ) == silc_unify_string_handles.end() );
    silc_unify_string_handles[ stringId ] = new_movable;

    std::cout << "string " << stringId << " " << string << std::endl;

    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_location_definition( void*             userdata,
                                   uint64_t          locationId,
                                   char*             name,
                                   OTF2_LocationType locationType,
                                   uint64_t          numberOfDefinitions )
{
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_global_location_definition( void*                 userdata,
                                          uint64_t              locationId,
                                          char*                 name,
                                          OTF2_GlobLocationType locationType,
                                          uint64_t              numberOfDefinitions )
{
    std::cout << "locationId " << locationId << ", numberOfDefinitions " << numberOfDefinitions << std::endl;
    if ( numberOfDefinitions > 0 )
    {
        silc_unify_locations_with_definitions.push_back( locationId );
    }
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_location_group_definition( void*             userdata,
                                         uint64_t          locationId,
                                         char*             name,
                                         OTF2_LocationType locationType,
                                         uint64_t          numberOfDefinitions,
                                         uint64_t          locationsNumber,
                                         uint64_t*         locations )
{
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_region_definition( void*           readingRemoteDefinitions,
                                 uint32_t        regionId,
                                 uint32_t        regionNameId,
                                 uint32_t        regionDescriptionId,
                                 OTF2_RegionType regionType,
                                 uint32_t        sourceFileId, // is actually a reference to a string
                                 uint32_t        beginLineNumber,
                                 uint32_t        endLineNumber )
{
    std::cout << std::endl;
    SILC_Region_Definition*         new_definition = NULL;
    SILC_Region_Definition_Movable* new_movable    = NULL;

    if ( static_cast<bool>( readingRemoteDefinitions ) )
    {
        SILC_ALLOC_NEW_REMOTE_DEFINITION( Region, region );
    }
    else
    {
        SILC_ALLOC_NEW_DEFINITION( Region, region );
    }

    assert( new_definition->sequence_number == regionId );

    // Init new_definition
    silc_unify_string_lookup::iterator string_handle_it = silc_unify_string_handles.find( regionNameId );
    assert( string_handle_it != silc_unify_string_handles.end() );
    new_definition->name_handle = *( string_handle_it->second );

    if ( sourceFileId == OTF2_UNDEFINED_UINT32 )
    {
        SILC_ALLOCATOR_MOVABLE_INIT_NULL( new_definition->file_handle );
    }
    else
    {
        silc_unify_string_lookup::iterator file_handle_it = silc_unify_string_handles.find( sourceFileId );
        assert( file_handle_it != silc_unify_string_handles.end() );
        new_definition->file_handle = *( file_handle_it->second );
        std::cout << "file " << SILC_MEMORY_DEREF_MOVABLE( &( new_definition->file_handle ), SILC_String_Definition* )->string_data << std::endl;
    }

    string_handle_it = silc_unify_string_handles.find( regionDescriptionId );
    assert( string_handle_it != silc_unify_string_handles.end() );
    new_definition->description_handle = *( string_handle_it->second ); // currently not used
    new_definition->region_type        = silc_unify_region_type_map[ regionType ];
    new_definition->begin_line         = beginLineNumber;
    new_definition->end_line           = endLineNumber;
    new_definition->adapter_type       = SILC_INVALID_ADAPTER_TYPE; // currently not used

    assert( silc_unify_region_handles.find( regionId ) == silc_unify_region_handles.end() );
    silc_unify_region_handles[ regionId ] = new_movable;

    std::cout << "region " << regionId << std::endl;
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_callsite_definition( void*    userdata,
                                   uint32_t callsiteId,
                                   uint32_t sourceFileId,
                                   uint32_t lineNumber,
                                   uint32_t regionEnterd,
                                   uint32_t regionLeft )
{
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_callpath_definition( void*    userdata,
                                   uint32_t callpathId,
                                   uint32_t parentCallpath,
                                   uint32_t regionId,
                                   uint8_t  callPathOrder )
{
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_group_definition( void*          userdata,
                                uint64_t       groupId,
                                OTF2_GroupType groupType,
                                uint64_t       numberOfMembers,
                                uint64_t*      members )
{
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_topology_cartesian_definition( void*     userdata,
                                             uint32_t  cartesianTopologyId,
                                             char*     name,
                                             uint32_t  locationsInEachDimensionNumber,
                                             uint32_t* numberOfLocationsInEachDimension,
                                             uint32_t  periodicityOfTheGridInEachDimensionNumber,
                                             uint8_t*  PeriodicityOfTheGridInEachDimension )
{
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_topology_cartesian_coords_definition( void*    userdata,
                                                    uint32_t cartesianTopologyId,
                                                    uint64_t locationId,
                                                    uint32_t numberOfDimensions,
                                                    uint8_t* coordinatesOfTheLocation )
{
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_topology_graph_definition( void*    userdata,
                                         uint32_t topologyGraphId,
                                         char*    name,
                                         uint8_t  isDirected )
{
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_topology_graph_edge_definition( void*    userdata,
                                              uint32_t topologyGraphId,
                                              uint64_t from,
                                              uint64_t to )
{
    return SILC_SUCCESS;
}


SILC_Error_Code
silc_unify_on_mpi_win_definition( void*    userdata,
                                  uint32_t windowId,
                                  uint64_t communicatorId )
{
    return SILC_SUCCESS;
}


void
silc_unify_remote_definitions_initialize()
{
    assert( silc_remote_definition_manager == 0 );
    silc_remote_definition_manager = new SILC_DefinitionManager(); //*)calloc(1, sizeof(SILC_DefinitionManager));

    // note, only lower-case type needed
    #define SILC_INIT_DEFINITION_LIST( type ) \
    do { \
        SILC_ALLOCATOR_MOVABLE_INIT_NULL( \
            silc_remote_definition_manager->type ## _definition_head ); \
        silc_remote_definition_manager->type ## _definition_tail_pointer = \
            &silc_remote_definition_manager->type ## _definition_head; \
    } while ( 0 )

    SILC_INIT_DEFINITION_LIST( string );
    SILC_INIT_DEFINITION_LIST( location );
    SILC_INIT_DEFINITION_LIST( source_file );
    SILC_INIT_DEFINITION_LIST( region );
    SILC_INIT_DEFINITION_LIST( group );
    SILC_INIT_DEFINITION_LIST( mpi_window );
    SILC_INIT_DEFINITION_LIST( mpi_cartesian_topology );
    SILC_INIT_DEFINITION_LIST( mpi_cartesian_coords );
    SILC_INIT_DEFINITION_LIST( counter_group );
    SILC_INIT_DEFINITION_LIST( counter );
    SILC_INIT_DEFINITION_LIST( io_file_group );
    SILC_INIT_DEFINITION_LIST( io_file );
    SILC_INIT_DEFINITION_LIST( marker_group );
    SILC_INIT_DEFINITION_LIST( marker );
    SILC_INIT_DEFINITION_LIST( parameter );
    SILC_INIT_DEFINITION_LIST( callpath );

    #undef SILC_INIT_DEFINITION_LIST
}


void
silc_unify_remote_definitions_finalize()
{
    // free remote pages
    delete silc_remote_definition_manager;
    silc_remote_definition_manager = 0;
}
