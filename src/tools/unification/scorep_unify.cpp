/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
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

#define __STDC_LIMIT_MACROS

#include <config.h>

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
#include <scorep_definitions.h>
#include <scorep_definition_locking.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Definitions.h>
#ifdef __cplusplus
} /* "C" */
#endif


/**
 * Allocate, assign the sequence number, and store in manager list a new
 * definition of type @type
 */
/* *INDENT-OFF* */
#define SCOREP_ALLOC_NEW_REMOTE_DEFINITION( Type, type ) \
    do { \
        new_movable = ( SCOREP_ ## Type ## _Definition_Movable* ) \
            SCOREP_Memory_AllocForDefinitions( \
                sizeof( SCOREP_ ## Type ## _Definition ) ); \
        new_definition = \
            SCOREP_MEMORY_DEREF_MOVABLE( new_movable, \
                                       SCOREP_ ## Type ## _Definition* ); \
        SCOREP_ALLOCATOR_MOVABLE_INIT_NULL( ( new_definition )->next ); \
        *scorep_remote_definition_manager->type ## _definition_tail_pointer = \
            *new_movable; \
        scorep_remote_definition_manager->type ## _definition_tail_pointer = \
            &( new_definition )->next; \
        ( new_definition )->sequence_number = \
            scorep_remote_definition_manager->type ## _definition_counter++; \
    } while ( 0 )
/* *INDENT-ON* */


/**
 * Allocate, assign the sequence number, and store in manager list a new
 * definition of type @type with a variable array member of type @array_type
 * and a total number of members of @number_of_members
 */
/* *INDENT-OFF* */
#define SCOREP_ALLOC_NEW_REMOTE_DEFINITION_VARIABLE_ARRAY( Type, \
                                                         type, \
                                                         array_type, \
                                                         number_of_members ) \
    do { \
        new_movable = ( SCOREP_ ## Type ## _Definition_Movable* ) \
            SCOREP_Memory_AllocForDefinitions( \
                sizeof( SCOREP_ ## Type ## _Definition ) + \
                ( ( number_of_members ) - 1 ) * sizeof( array_type ) ); \
        new_definition = \
            SCOREP_MEMORY_DEREF_MOVABLE( new_movable, \
                                       SCOREP_ ## Type ## _Definition* ); \
        SCOREP_ALLOCATOR_MOVABLE_INIT_NULL( ( new_definition )->next ); \
        *scorep_remote_definition_manager->type ## _definition_tail_pointer = \
            *new_movable; \
        scorep_remote_definition_manager->type ## _definition_tail_pointer = \
            &( new_definition )->next; \
        ( new_definition )->sequence_number = \
            scorep_remote_definition_manager->type ## _definition_counter++; \
    } while ( 0 )
/* *INDENT-ON* */


extern SCOREP_DefinitionManager scorep_definition_manager;
extern SCOREP_DefinitionManager* scorep_remote_definition_manager;

typedef std::map<uint32_t /* stringId */, SCOREP_StringHandle> scorep_unify_string_lookup;
static scorep_unify_string_lookup scorep_unify_string_handles;


//typedef std::map<uint32_t /* fileId */, SCOREP_SourceFileHandle> scorep_unify_file_lookup;
//static scorep_unify_file_lookup scorep_unify_file_handles;


typedef std::map<uint32_t /* regionId */, SCOREP_RegionHandle> scorep_unify_region_lookup;
static scorep_unify_region_lookup scorep_unify_region_handles;


typedef std::vector<uint64_t> scorep_unify_locations;
static scorep_unify_locations scorep_unify_locations_with_definitions;


static SCOREP_RegionType scorep_unify_region_type_map[ OTF2_REGION_DYNAMIC_LOOP_PHASE + 1 ] = {
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

SCOREP_Error_Code scorep_unify_on_number_of_processes(void* userdata, uint32_t nProcsIdentifier, int nProcs);

SCOREP_Error_Code scorep_unify_on_string_definition( void* userData, uint32_t stringId, char* string );
SCOREP_Error_Code scorep_unify_on_location_definition( void* userdata, uint64_t locationId, char* name, OTF2_LocationType locationType, uint64_t numberOfDefinitions );
SCOREP_Error_Code scorep_unify_on_location_group_definition( void* userdata, uint64_t locationId, char* name, OTF2_LocationType locationType, uint64_t numberOfDefinitions, uint64_t locationsNumber, uint64_t* locations );
SCOREP_Error_Code scorep_unify_on_region_definition( void* userdata, uint32_t regionId, uint32_t regionNameId, uint32_t regionDescriptionId, OTF2_RegionType regionType, uint32_t sourceFileId, uint32_t beginLineNumber, uint32_t endLineNumber );
SCOREP_Error_Code scorep_unify_on_callsite_definition( void* userdata, uint32_t callsiteId, uint32_t sourceFileId, uint32_t lineNumber, uint32_t regionEnterd, uint32_t regionLeft );
SCOREP_Error_Code scorep_unify_on_callpath_definition( void* userdata, uint32_t callpathId, uint32_t parentCallpath, uint32_t regionId, uint8_t callPathOrder );
SCOREP_Error_Code scorep_unify_on_group_definition( void* userdata, uint64_t groupId, OTF2_GroupType groupType, uint64_t numberOfMembers, uint64_t* members );
SCOREP_Error_Code scorep_unify_on_topology_cartesian_definition( void* userdata, uint32_t cartesianTopologyId, char* name, uint32_t locationsInEachDimensionNumber,uint32_t* numberOfLocationsInEachDimension, uint32_t periodicityOfTheGridInEachDimensionNumber, uint8_t* PeriodicityOfTheGridInEachDimension );
SCOREP_Error_Code scorep_unify_on_topology_cartesian_coords_definition( void* userdata, uint32_t cartesianTopologyId, uint64_t locationId, uint32_t numberOfDimensions, uint8_t* coordinatesOfTheLocation );
SCOREP_Error_Code scorep_unify_on_topology_graph_definition( void* userdata, uint32_t topologyGraphId, char* name, uint8_t isDirected );
SCOREP_Error_Code scorep_unify_on_topology_graph_edge_definition( void* userdata, uint32_t topologyGraphId, uint64_t from, uint64_t to );
SCOREP_Error_Code scorep_unify_on_mpi_win_definition( void* userdata, uint32_t windowId, uint64_t communicatorId );

SCOREP_Error_Code scorep_unify_on_global_location_definition( void* userdata, uint64_t locationId, char* name, OTF2_GlobLocationType locationType, uint64_t numberOfDefinitions );

void scorep_unify_remote_definitions_initialize();
void scorep_unify_remote_definitions_finalize();
/* *INDENT-ON*  */


int
main( int   argc,
      char* argv[] )
{
    std::string path_to_archive = scorep_unify_parse_cmd_line( argc, argv );

    std::cout << "archive " << path_to_archive <<  std::endl;
    OTF2_Reader* reader = OTF2_Reader_New( path_to_archive.c_str() );
    assert( reader );
    std::cout << "OTF2_Reader_New ready" << std::endl;

    OTF2_GlobDefReader* global_definition_reader = OTF2_Reader_GetGlobDefReader( reader );
    assert( global_definition_reader );
    std::cout << "GetGlobDefReader ready" << std::endl;
    OTF2_GlobDefReaderCallbacks get_locations_with_definitions;
    memset( &get_locations_with_definitions, 0, sizeof( get_locations_with_definitions ) );
    get_locations_with_definitions.GlobDefLocation = scorep_unify_on_global_location_definition;

    // currently broken
    uint64_t dummy;
//    OTF2_Reader_RegisterGlobDefCallbacks( reader, global_definition_reader, get_locations_with_definitions, 0 );
//    OTF2_Reader_ReadGlobalDefinitions( reader, global_definition_reader, OTF2_UNDEFINED_UINT64, &dummy );
//    int n_procs = scorep_unify_locations_with_definitions.size();
//    //assert( n_procs > 0 );
//    std::cout << "ReadGlobalDefinitions ready" << std::endl;

    // read definition of rank 0 (called local) to memory via call back
    int                     rank                    = 0;
    OTF2_DefReader*         local_definition_reader = OTF2_Reader_GetDefReader( reader, rank );

    OTF2_DefReaderCallbacks read_definitions;
    memset( &read_definitions, 0, sizeof( read_definitions ) );
    read_definitions.DefString                  = scorep_unify_on_string_definition;
    read_definitions.DefLocation                = scorep_unify_on_location_definition;
    read_definitions.DefLocationGroup           = scorep_unify_on_location_group_definition;
    read_definitions.DefRegion                  = scorep_unify_on_region_definition;
    read_definitions.DefCallsite                = scorep_unify_on_callsite_definition;
    read_definitions.DefCallpath                = scorep_unify_on_callpath_definition;
    read_definitions.DefGroup                   = scorep_unify_on_group_definition;
    read_definitions.DefTopologyCartesian       = scorep_unify_on_topology_cartesian_definition;
    read_definitions.DefTopologyCartesianCoords = scorep_unify_on_topology_cartesian_coords_definition;
    read_definitions.DefTopologyGraph           = scorep_unify_on_topology_graph_definition;
    read_definitions.DefTopologyGraphEdge       = scorep_unify_on_topology_graph_edge_definition;
    read_definitions.DefMpiWin                  = scorep_unify_on_mpi_win_definition;

    SCOREP_Memory_Initialize( 150 * 8192, 8192 );
    SCOREP_DefinitionLocks_Initialize();
    SCOREP_Definitions_Initialize();

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
    scorep_unify_locations_with_definitions.push_back( 0 );
    //scorep_unify_locations_with_definitions.push_back(1);
    read_remote_definitions = true;
    for ( scorep_unify_locations::iterator location = scorep_unify_locations_with_definitions.begin();
          location != scorep_unify_locations_with_definitions.end(); ++location )
    {
        std::cout << "unifying location 0 with location " << *location << std::endl;
        scorep_unify_remote_definitions_initialize();
        OTF2_DefReader* remote_definition_reader = OTF2_Reader_GetDefReader( reader, *location );
        OTF2_Reader_RegisterDefCallbacks( reader, remote_definition_reader, read_definitions, ( void* )read_remote_definitions );
        OTF2_Reader_ReadLocalDefinitions( reader, remote_definition_reader, OTF2_UNDEFINED_UINT64, &dummy );

        // create hash
        // unify

        scorep_unify_remote_definitions_finalize();
    }

    /*

       OTF2_GlobReader_ResetDefReader?( global_definition_reader, void* my_data, scorep_unify_write_global_definitions );
       // this triggers scorep_unify_write_global_definitions (OTF2_GlobDefWriter* writer, void* my_data) where
       // my_data is local_movable_memory

       clear local_movable_memory

     */

    SCOREP_Definitions_Finalize();
    SCOREP_DefinitionLocks_Finalize();
    SCOREP_Memory_Finalize();

    SCOREP_Error_Code status = OTF2_Reader_Delete( reader );
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
scorep_unify_on_number_of_processes( void*    userdata,
                                     uint32_t nProcsIdentifier,
                                     int      nProcs )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_string_definition( void*    readingRemoteDefinitions,
                                   uint32_t stringId,
                                   char*    string )
{
    SCOREP_String_Definition*         new_definition = NULL;
    SCOREP_String_Definition_Movable* new_movable    = NULL;

    uint32_t                          string_length = strlen( string ) + 1;

    if ( static_cast<bool>( readingRemoteDefinitions ) )
    {
        SCOREP_ALLOC_NEW_REMOTE_DEFINITION_VARIABLE_ARRAY( String,
                                                           string,
                                                           char,
                                                           string_length );
    }
    else
    {
        SCOREP_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY( String,
                                                    string,
                                                    char,
                                                    string_length );
    }

    assert( new_definition->sequence_number == stringId );

    new_definition->string_length = string_length;
    strcpy( new_definition->string_data, string );

    assert( scorep_unify_string_handles.find( stringId ) == scorep_unify_string_handles.end() );
    scorep_unify_string_handles[ stringId ] = new_movable;

    std::cout << "string " << stringId << " " << string << std::endl;

    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_location_definition( void*             userdata,
                                     uint64_t          locationId,
                                     char*             name,
                                     OTF2_LocationType locationType,
                                     uint64_t          numberOfDefinitions )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_global_location_definition( void*                 userdata,
                                            uint64_t              locationId,
                                            char*                 name,
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
scorep_unify_on_location_group_definition( void*             userdata,
                                           uint64_t          locationId,
                                           char*             name,
                                           OTF2_LocationType locationType,
                                           uint64_t          numberOfDefinitions,
                                           uint64_t          locationsNumber,
                                           uint64_t*         locations )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_region_definition( void*           readingRemoteDefinitions,
                                   uint32_t        regionId,
                                   uint32_t        regionNameId,
                                   uint32_t        regionDescriptionId,
                                   OTF2_RegionType regionType,
                                   uint32_t        sourceFileId, // is actually a reference to a string
                                   uint32_t        beginLineNumber,
                                   uint32_t        endLineNumber )
{
    std::cout << std::endl;
    SCOREP_Region_Definition*         new_definition = NULL;
    SCOREP_Region_Definition_Movable* new_movable    = NULL;

    if ( static_cast<bool>( readingRemoteDefinitions ) )
    {
        SCOREP_ALLOC_NEW_REMOTE_DEFINITION( Region, region );
    }
    else
    {
        SCOREP_ALLOC_NEW_DEFINITION( Region, region );
    }

    assert( new_definition->sequence_number == regionId );

    // Init new_definition
    scorep_unify_string_lookup::iterator string_handle_it = scorep_unify_string_handles.find( regionNameId );
    assert( string_handle_it != scorep_unify_string_handles.end() );
    new_definition->name_handle = *( string_handle_it->second );

    if ( sourceFileId == OTF2_UNDEFINED_UINT32 )
    {
        SCOREP_ALLOCATOR_MOVABLE_INIT_NULL( new_definition->file_handle );
    }
    else
    {
        scorep_unify_string_lookup::iterator file_handle_it = scorep_unify_string_handles.find( sourceFileId );
        assert( file_handle_it != scorep_unify_string_handles.end() );
        new_definition->file_handle = *( file_handle_it->second );
        std::cout << "file " << SCOREP_MEMORY_DEREF_MOVABLE( &( new_definition->file_handle ), SCOREP_String_Definition* )->string_data << std::endl;
    }

    string_handle_it = scorep_unify_string_handles.find( regionDescriptionId );
    assert( string_handle_it != scorep_unify_string_handles.end() );
    new_definition->description_handle = *( string_handle_it->second ); // currently not used
    new_definition->region_type        = scorep_unify_region_type_map[ regionType ];
    new_definition->begin_line         = beginLineNumber;
    new_definition->end_line           = endLineNumber;
    new_definition->adapter_type       = SCOREP_INVALID_ADAPTER_TYPE; // currently not used

    assert( scorep_unify_region_handles.find( regionId ) == scorep_unify_region_handles.end() );
    scorep_unify_region_handles[ regionId ] = new_movable;

    std::cout << "region " << regionId << std::endl;
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_callsite_definition( void*    userdata,
                                     uint32_t callsiteId,
                                     uint32_t sourceFileId,
                                     uint32_t lineNumber,
                                     uint32_t regionEnterd,
                                     uint32_t regionLeft )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_callpath_definition( void*    userdata,
                                     uint32_t callpathId,
                                     uint32_t parentCallpath,
                                     uint32_t regionId,
                                     uint8_t  callPathOrder )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_group_definition( void*          userdata,
                                  uint64_t       groupId,
                                  OTF2_GroupType groupType,
                                  uint64_t       numberOfMembers,
                                  uint64_t*      members )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_topology_cartesian_definition( void*     userdata,
                                               uint32_t  cartesianTopologyId,
                                               char*     name,
                                               uint32_t  locationsInEachDimensionNumber,
                                               uint32_t* numberOfLocationsInEachDimension,
                                               uint32_t  periodicityOfTheGridInEachDimensionNumber,
                                               uint8_t*  PeriodicityOfTheGridInEachDimension )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_topology_cartesian_coords_definition( void*    userdata,
                                                      uint32_t cartesianTopologyId,
                                                      uint64_t locationId,
                                                      uint32_t numberOfDimensions,
                                                      uint8_t* coordinatesOfTheLocation )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_topology_graph_definition( void*    userdata,
                                           uint32_t topologyGraphId,
                                           char*    name,
                                           uint8_t  isDirected )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_topology_graph_edge_definition( void*    userdata,
                                                uint32_t topologyGraphId,
                                                uint64_t from,
                                                uint64_t to )
{
    return SCOREP_SUCCESS;
}


SCOREP_Error_Code
scorep_unify_on_mpi_win_definition( void*    userdata,
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

    // note, only lower-case type needed
    #define SCOREP_INIT_DEFINITION_LIST( type ) \
    do { \
        SCOREP_ALLOCATOR_MOVABLE_INIT_NULL( \
            scorep_remote_definition_manager->type ## _definition_head ); \
        scorep_remote_definition_manager->type ## _definition_tail_pointer = \
            &scorep_remote_definition_manager->type ## _definition_head; \
    } while ( 0 )

    SCOREP_INIT_DEFINITION_LIST( string );
    SCOREP_INIT_DEFINITION_LIST( location );
    SCOREP_INIT_DEFINITION_LIST( source_file );
    SCOREP_INIT_DEFINITION_LIST( region );
    SCOREP_INIT_DEFINITION_LIST( group );
    SCOREP_INIT_DEFINITION_LIST( mpi_window );
    SCOREP_INIT_DEFINITION_LIST( mpi_cartesian_topology );
    SCOREP_INIT_DEFINITION_LIST( mpi_cartesian_coords );
    SCOREP_INIT_DEFINITION_LIST( counter_group );
    SCOREP_INIT_DEFINITION_LIST( counter );
    SCOREP_INIT_DEFINITION_LIST( io_file_group );
    SCOREP_INIT_DEFINITION_LIST( io_file );
    SCOREP_INIT_DEFINITION_LIST( marker_group );
    SCOREP_INIT_DEFINITION_LIST( marker );
    SCOREP_INIT_DEFINITION_LIST( parameter );
    SCOREP_INIT_DEFINITION_LIST( callpath );

    #undef SCOREP_INIT_DEFINITION_LIST
}


void
scorep_unify_remote_definitions_finalize()
{
    // free remote pages
    delete scorep_remote_definition_manager;
    scorep_remote_definition_manager = 0;
}
