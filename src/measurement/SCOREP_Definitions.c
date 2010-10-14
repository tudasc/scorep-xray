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


#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/**
 * @status     alpha
 * @file       SCOREP_Definitions.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */

#include <SCOREP_Definitions.h>

#include <scorep_utility/SCOREP_Debug.h>
#include <SCOREP_DefinitionHandles.h>
#include <jenkins_hash.h>

#include "scorep_definition_structs.h"
#include "scorep_definitions.h"
#include "scorep_types.h"

extern SCOREP_DefinitionManager scorep_definition_manager;

/**
 * Associate a file name with a process unique file handle.
 */
SCOREP_SourceFileHandle
SCOREP_DefineSourceFile( const char* fileName )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new source file \"%s\":",
                         fileName );

    SCOREP_SourceFile_Definition* new_definition = NULL;
    SCOREP_SourceFileHandle       new_handle     = SCOREP_INVALID_SOURCE_FILE;
    SCOREP_ALLOC_NEW_DEFINITION( SourceFile, source_file );

    new_definition->name_handle = SCOREP_DefineString( fileName );
    /* just take the hash value from the string */
    new_definition->hash_value =
        SCOREP_HANDLE_GET_HASH( new_definition->name_handle, String );

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    Handle ID: %x", new_definition->sequence_number );

    return new_handle;
}


/**
 * Associate a code region with a process unique file handle.
 */
SCOREP_RegionHandle
SCOREP_DefineRegion( const char*             regionName,
                     SCOREP_SourceFileHandle fileHandle,
                     SCOREP_LineNo           beginLine,
                     SCOREP_LineNo           endLine,
                     SCOREP_AdapterType      adapter,
                     SCOREP_RegionType       regionType )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new region \"%s\":", regionName );

    SCOREP_Region_Definition* new_definition = NULL;
    SCOREP_RegionHandle       new_handle     = SCOREP_INVALID_REGION;
    SCOREP_ALLOC_NEW_DEFINITION( Region, region );

    // Init new_definition
    new_definition->name_handle = SCOREP_DefineString( regionName );
    HASH_ADD_HANDLE( name_handle, String );
    /* currently not used */
    new_definition->description_handle = SCOREP_DefineString( "" );
    HASH_ADD_HANDLE( description_handle, String );

    new_definition->region_type = regionType;                      // maps to OTF2_RegionType
    HASH_ADD_POD( region_type );

    if ( fileHandle == SCOREP_INVALID_SOURCE_FILE )
    {
        new_definition->file_handle = SCOREP_MOVABLE_NULL;
        /* should we add a 0 value to the hash? */
    }
    else
    {
        new_definition->file_handle =
            SCOREP_HANDLE_DEREF( fileHandle, SourceFile )->name_handle;
        HASH_ADD_HANDLE( file_handle, SourceFile );
    }

    new_definition->begin_line = beginLine;
    HASH_ADD_POD( begin_line );
    new_definition->end_line = endLine;
    HASH_ADD_POD( end_line );
    new_definition->adapter_type = adapter;       // currently not used
    HASH_ADD_POD( adapter_type );

// TODO: make this to a scorep_debug_dump_*_definition
#ifdef SCOREP_DEBUG
    char stringBuffer[ 16 ];

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    Handle ID:   %x", new_definition->sequence_number );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    Source file: %s",
                         scorep_source_file_to_string( stringBuffer,
                                                       sizeof( stringBuffer ),
                                                       "%x", fileHandle ) );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    Start line:  %s",
                         scorep_line_number_to_string( stringBuffer,
                                                       sizeof( stringBuffer ),
                                                       "%u", beginLine ) );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    End line:    %s",
                         scorep_line_number_to_string( stringBuffer,
                                                       sizeof( stringBuffer ),
                                                       "%u", endLine ) );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    Adpater:     %s",
                         scorep_adapter_type_to_string( adapter ) );
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    Region type: %s",
                         scorep_region_type_to_string( regionType ) );
#endif

    return new_handle;
}

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
    SCOREP_Region_Definition* region = SCOREP_HANDLE_DEREF( handle, Region );

    return SCOREP_HANDLE_DEREF( region->name_handle, String )->string_data;
}

const char*
SCOREP_Parameter_GetName( SCOREP_ParameterHandle handle )
{
    SCOREP_Parameter_Definition* param = SCOREP_HANDLE_DEREF( handle, Parameter );

    return SCOREP_HANDLE_DEREF( param->name_handle, String )->string_data;
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
    SCOREP_Region_Definition* region = SCOREP_HANDLE_DEREF( handle, Region );

    return SCOREP_HANDLE_DEREF( region->file_handle, String )->string_data;
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
    return SCOREP_HANDLE_DEREF( handle, Region )->region_type;
}


/**
 * @}
 */


/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
SCOREP_MPICommunicatorHandle
SCOREP_DefineMPICommunicator( int32_t     numberOfRanks,
                              int32_t*    ranks,
                              const char* name )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "Define new MPI Communicator:" );

    SCOREP_Group_Definition* new_definition = NULL;
    SCOREP_GroupHandle       new_handle     = SCOREP_INVALID_MPI_COMMUNICATOR;
    SCOREP_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY( Group,
                                                group,
                                                uint64_t,
                                                numberOfRanks );

    // Init new_definition
    new_definition->group_type = SCOREP_GROUP_COMMUNICATOR;

    new_definition->name_handle = SCOREP_DefineString( name );
    HASH_ADD_HANDLE( name_handle, String );

    HASH_ADD_POD( group_type );
    new_definition->number_of_members = numberOfRanks;
    HASH_ADD_POD( number_of_members );
    for ( int32_t i = 0; i < numberOfRanks; i++ )
    {
        /* convert ranks to global location ids */
        new_definition->members[ i ] = ( uint64_t )ranks[ i ];
    }
    HASH_ADD_ARRAY( members, number_of_members );

// TODO: make this into a scorep_debug_dump_*_definition function
#ifdef SCOREP_DEBUG
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

    return new_handle;
}


/**
 * Associate a MPI window with a process unique window handle.
 */
SCOREP_MPIWindowHandle
SCOREP_DefineMPIWindow( SCOREP_MPICommunicatorHandle communicatorHandle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "Define new MPI Window:" );

    SCOREP_MPIWindow_Definition* new_definition = NULL;
    SCOREP_MPIWindowHandle       new_handle     = SCOREP_INVALID_MPI_WINDOW;
    SCOREP_ALLOC_NEW_DEFINITION( MPIWindow, mpi_window );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                         "    Handle ID: %x", new_definition->sequence_number );

    return new_handle;
}


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
                         "Define new MPI cartesian topology \"%s\":",
                         topologyName );

    SCOREP_MPICartesianTopology_Definition* new_definition = NULL;
    SCOREP_MPICartesianTopologyHandle       new_handle     = SCOREP_INVALID_CART_TOPOLOGY;
    SCOREP_ALLOC_NEW_DEFINITION( MPICartesianTopology, mpi_cartesian_topology );

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

    return new_handle;
}


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

    SCOREP_MPICartesianCoords_Definition* new_definition = NULL;
    SCOREP_MPICartesianCoordsHandle       new_handle     = SCOREP_INVALID_CART_COORDS;
    SCOREP_ALLOC_NEW_DEFINITION( MPICartesianCoords, mpi_cartesian_coords );

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


/**
 * Associate a name with a process unique counter group handle.
 */
SCOREP_CounterGroupHandle
SCOREP_DefineCounterGroup( const char* name )
{
    SCOREP_CounterGroup_Definition* new_definition = NULL;
    SCOREP_CounterGroupHandle       new_handle     = SCOREP_INVALID_COUNTER_GROUP;
    SCOREP_ALLOC_NEW_DEFINITION( CounterGroup, counter_group );

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "" );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    return new_handle;
}


/**
 * Associate the parameter tuple with a process unique counter handle.
 */
SCOREP_CounterHandle
SCOREP_DefineCounter( const char*               name,
                      SCOREP_CounterType        counterType,
                      SCOREP_CounterGroupHandle counterGroupHandle,
                      const char*               unit )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "" );

    SCOREP_Counter_Definition* new_definition = NULL;
    SCOREP_CounterHandle       new_handle     = SCOREP_INVALID_COUNTER;
    SCOREP_ALLOC_NEW_DEFINITION( Counter, counter );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    return new_handle;
}


/**
 * Associate a name with a process unique I/O file group handle.
 */
SCOREP_IOFileGroupHandle
SCOREP_DefineIOFileGroup( const char* name )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "" );

    SCOREP_IOFileGroup_Definition* new_definition = NULL;
    SCOREP_IOFileGroupHandle       new_handle     = SCOREP_INVALID_IOFILE_GROUP;
    SCOREP_ALLOC_NEW_DEFINITION( IOFileGroup, io_file_group );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    return new_handle;
}


/**
 * Associate a name and a group handle with a process unique I/O file handle.
 */
SCOREP_IOFileHandle
SCOREP_DefineIOFile( const char*              name,
                     SCOREP_IOFileGroupHandle ioFileGroup )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "" );

    SCOREP_IOFile_Definition* new_definition = NULL;
    SCOREP_IOFileHandle       new_handle     = SCOREP_INVALID_IOFILE;
    SCOREP_ALLOC_NEW_DEFINITION( IOFile, io_file );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    return new_handle;
}


/**
 * Associate a name with a process unique marker group handle.
 */
SCOREP_MarkerGroupHandle
SCOREP_DefineMarkerGroup( const char* name )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "" );

    SCOREP_MarkerGroup_Definition* new_definition = NULL;
    SCOREP_MarkerGroupHandle       new_handle     = SCOREP_INVALID_MARKER_GROUP;
    SCOREP_ALLOC_NEW_DEFINITION( MarkerGroup, marker_group );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    return new_handle;
}


/**
 * Associate a name and a group handle with a process unique marker handle.
 */
SCOREP_MarkerHandle
SCOREP_DefineMarker
(
    const char*              name,
    SCOREP_MarkerGroupHandle markerGroup
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "" );

    SCOREP_Marker_Definition* new_definition = NULL;
    SCOREP_MarkerHandle       new_handle     = SCOREP_INVALID_MARKER;
    SCOREP_ALLOC_NEW_DEFINITION( Marker, marker );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Not yet implemented." );

    return new_handle;
}


/**
 * Associate a name and a type with a process unique parameter handle.
 */
SCOREP_ParameterHandle
SCOREP_DefineParameter
(
    const char*          name,
    SCOREP_ParameterType type
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "" );

    SCOREP_Parameter_Definition* new_definition = NULL;
    SCOREP_ParameterHandle       new_handle     = SCOREP_INVALID_PARAMETER;
    SCOREP_ALLOC_NEW_DEFINITION( Parameter, parameter );

    // Init new_definition
    SCOREP_DEBUG_PRINTF( 0, "Only partially implemented." );

    new_definition->name_handle = SCOREP_DefineString( name );
    HASH_ADD_HANDLE( name_handle, String );
    /* currently not used */
    new_definition->description_handle = SCOREP_DefineString( "" );
    HASH_ADD_HANDLE( description_handle, String );

    new_definition->parameter_type = type;
    HASH_ADD_POD( parameter_type );

    return new_handle;
}
