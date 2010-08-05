/*
 * This file is part of the SILC project (http://www.silc.de)
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
 * @file       SILC_Definitions.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */

#include <SILC_Definitions.h>

#include <SILC_Debug.h>
#include <SILC_DefinitionHandles.h>
#include <jenkins_hash.h>

#include "silc_definition_structs.h"
#include "silc_definitions.h"
#include "silc_types.h"

extern SILC_DefinitionManager silc_definition_manager;

/**
 * Associate a file name with a process unique file handle.
 */
SILC_SourceFileHandle
SILC_DefineSourceFile( const char* fileName )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new source file \"%s\":",
                       fileName );

    SILC_SourceFile_Definition* new_definition = NULL;
    SILC_SourceFileHandle       new_handle     = SILC_INVALID_SOURCE_FILE;
    SILC_ALLOC_NEW_DEFINITION( SourceFile, source_file );

    new_definition->name_handle = SILC_DefineString( fileName );
    /* just take the hash value from the string */
    new_definition->hash_value =
        SILC_HANDLE_GET_HASH( new_definition->name_handle, String );

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID: %x", new_definition->sequence_number );

    return new_handle;
}


/**
 * Associate a code region with a process unique file handle.
 */
SILC_RegionHandle
SILC_DefineRegion( const char*           regionName,
                   SILC_SourceFileHandle fileHandle,
                   SILC_LineNo           beginLine,
                   SILC_LineNo           endLine,
                   SILC_AdapterType      adapter,
                   SILC_RegionType       regionType )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new region \"%s\":", regionName );

    SILC_Region_Definition* new_definition = NULL;
    SILC_RegionHandle       new_handle     = SILC_INVALID_REGION;
    SILC_ALLOC_NEW_DEFINITION( Region, region );

    // Init new_definition
    new_definition->name_handle = SILC_DefineString( regionName );
    HASH_ADD_HANDLE( name_handle, String );
    /* currently not used */
    new_definition->description_handle = SILC_DefineString( "" );
    HASH_ADD_HANDLE( description_handle, String );

    new_definition->region_type = regionType;                      // maps to OTF2_RegionType
    HASH_ADD_POD( region_type );

    if ( fileHandle == SILC_INVALID_SOURCE_FILE )
    {
        new_definition->file_handle = SILC_MOVABLE_NULL;
        /* should we add a 0 value to the hash? */
    }
    else
    {
        new_definition->file_handle =
            SILC_HANDLE_DEREF( fileHandle, SourceFile )->name_handle;
        HASH_ADD_HANDLE( file_handle, SourceFile );
    }

    new_definition->begin_line = beginLine;
    HASH_ADD_POD( begin_line );
    new_definition->end_line = endLine;
    HASH_ADD_POD( end_line );
    new_definition->adapter_type = adapter;       // currently not used
    HASH_ADD_POD( adapter_type );

// TODO: make this to a silc_debug_dump_*_definition
#ifdef SILC_DEBUG
    char stringBuffer[ 16 ];

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID:   %x", new_definition->sequence_number );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Source file: %s",
                       silc_source_file_to_string( stringBuffer,
                                                   sizeof( stringBuffer ),
                                                   "%x", fileHandle ) );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Start line:  %s",
                       silc_line_number_to_string( stringBuffer,
                                                   sizeof( stringBuffer ),
                                                   "%u", beginLine ) );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    End line:    %s",
                       silc_line_number_to_string( stringBuffer,
                                                   sizeof( stringBuffer ),
                                                   "%u", endLine ) );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Adpater:     %s",
                       silc_adapter_type_to_string( adapter ) );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Region type: %s",
                       silc_region_type_to_string( regionType ) );
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
SILC_Region_GetName( SILC_RegionHandle handle )
{
    SILC_Region_Definition* region = SILC_HANDLE_DEREF( handle, Region );

    return SILC_HANDLE_DEREF( region->name_handle, String )->string_data;
}


/**
 * Gets the type of the region.
 *
 * @param handle A handle to the region.
 *
 * @return region type.
 */
SILC_RegionType
SILC_Region_GetType( SILC_RegionHandle handle )
{
    return SILC_HANDLE_DEREF( handle, Region )->region_type;
}


/**
 * @}
 */


/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
SILC_MPICommunicatorHandle
SILC_DefineMPICommunicator( int32_t  numberOfRanks,
                            int32_t* ranks )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new MPI Communicator:" );

    SILC_Group_Definition* new_definition = NULL;
    SILC_GroupHandle       new_handle     = SILC_INVALID_MPI_COMMUNICATOR;
    SILC_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY( Group,
                                              group,
                                              uint64_t,
                                              numberOfRanks );

    // Init new_definition
    new_definition->group_type = SILC_GROUP_COMMUNICATOR;
    HASH_ADD_POD( group_type );
    new_definition->number_of_members = numberOfRanks;
    HASH_ADD_POD( number_of_members );
    for ( int32_t i = 0; i < numberOfRanks; i++ )
    {
        /* convert ranks to global location ids */
        new_definition->members[ i ] = ( uint64_t )ranks[ i ];
    }
    HASH_ADD_ARRAY( members, number_of_members );

// TODO: make this into a silc_debug_dump_*_definition function
#ifdef SILC_DEBUG
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID:   %x", new_definition->sequence_number );
    SILC_DEBUG_PREFIX( SILC_DEBUG_DEFINITIONS );
    SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS,
                           "    World ranks:" );

    uint32_t ranks_in_line = 0;
    for ( int32_t i = 0; i < numberOfRanks; ++i )
    {
        if ( ranks_in_line && ranks_in_line % 16 == 0 )
        {
            SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, "\n" );
            SILC_DEBUG_PREFIX( SILC_DEBUG_DEFINITIONS );
            SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, "%*s",
                                   ( int )strlen( "    World ranks:" ),
                                   "" );
        }

        SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, " %u", ranks[ i ] );

        ranks_in_line++;
    }
    SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, "\n" );
#endif

    return new_handle;
}


/**
 * Associate a MPI window with a process unique window handle.
 */
SILC_MPIWindowHandle
SILC_DefineMPIWindow( SILC_MPICommunicatorHandle communicatorHandle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "Define new MPI Window:" );

    SILC_MPIWindow_Definition* new_definition = NULL;
    SILC_MPIWindowHandle       new_handle     = SILC_INVALID_MPI_WINDOW;
    SILC_ALLOC_NEW_DEFINITION( MPIWindow, mpi_window );

    // Init new_definition

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID: %x", new_definition->sequence_number );

    return new_handle;
}


/**
 * Associate a MPI cartesian topology with a process unique topology handle.
 */
SILC_MPICartesianTopologyHandle
SILC_DefineMPICartesianTopology( const char*                topologyName,
                                 SILC_MPICommunicatorHandle communicatorHandle,
                                 uint32_t                   nDimensions,
                                 const uint32_t             nProcessesPerDimension[],
                                 const uint8_t              periodicityPerDimension[] )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new MPI cartesian topology \"%s\":",
                       topologyName );

    SILC_MPICartesianTopology_Definition* new_definition = NULL;
    SILC_MPICartesianTopologyHandle       new_handle     = SILC_INVALID_CART_TOPOLOGY;
    SILC_ALLOC_NEW_DEFINITION( MPICartesianTopology, mpi_cartesian_topology );

    // Init new_definition

// TODO: make this to a silc_debug_dump_*_definition
#ifdef SILC_DEBUG
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID:  %x", new_definition->sequence_number );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Dimensions: %u", nDimensions );

    for ( uint32_t i = 0; i < nDimensions; ++i )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                           "    Dimension %u:", i );
        SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                           "        #processes  %u:",
                           nProcessesPerDimension[ i ] );
        SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
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
SILC_DefineMPICartesianCoords(
    SILC_MPICartesianTopologyHandle cartesianTopologyHandle,
    uint32_t                        nCoords,
    const uint32_t                  coordsOfCurrentRank[] )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new MPI cartesian coordinates:" );

    SILC_MPICartesianCoords_Definition* new_definition = NULL;
    SILC_MPICartesianCoordsHandle       new_handle     = SILC_INVALID_CART_COORDS;
    SILC_ALLOC_NEW_DEFINITION( MPICartesianCoords, mpi_cartesian_coords );

    // Init new_definition

// TODO: make this into a silc_debug_dump_*_definition function
#ifdef SILC_DEBUG
    char stringBuffer[ 16 ];

    SILC_DEBUG_PREFIX( SILC_DEBUG_DEFINITIONS );
    SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, "    Coordinates:" );
    for ( uint32_t i = 0; i < nCoords; ++i )
    {
        SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS,
                               " %u", coordsOfCurrentRank[ i ] );
    }
    SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, "\n" );
#endif
}


/**
 * Associate a name with a process unique counter group handle.
 */
SILC_CounterGroupHandle
SILC_DefineCounterGroup( const char* name )
{
    SILC_CounterGroup_Definition* new_definition = NULL;
    SILC_CounterGroupHandle       new_handle     = SILC_INVALID_COUNTER_GROUP;
    SILC_ALLOC_NEW_DEFINITION( CounterGroup, counter_group );

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    // Init new_definition

    return new_handle;
}


/**
 * Associate the parameter tuple with a process unique counter handle.
 */
SILC_CounterHandle
SILC_DefineCounter( const char*             name,
                    SILC_CounterType        counterType,
                    SILC_CounterGroupHandle counterGroupHandle,
                    const char*             unit )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    SILC_Counter_Definition* new_definition = NULL;
    SILC_CounterHandle       new_handle     = SILC_INVALID_COUNTER;
    SILC_ALLOC_NEW_DEFINITION( Counter, counter );

    // Init new_definition

    return new_handle;
}


/**
 * Associate a name with a process unique I/O file group handle.
 */
SILC_IOFileGroupHandle
SILC_DefineIOFileGroup( const char* name )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    SILC_IOFileGroup_Definition* new_definition = NULL;
    SILC_IOFileGroupHandle       new_handle     = SILC_INVALID_IOFILE_GROUP;
    SILC_ALLOC_NEW_DEFINITION( IOFileGroup, io_file_group );

    // Init new_definition

    return new_handle;
}


/**
 * Associate a name and a group handle with a process unique I/O file handle.
 */
SILC_IOFileHandle
SILC_DefineIOFile( const char*            name,
                   SILC_IOFileGroupHandle ioFileGroup )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    SILC_IOFile_Definition* new_definition = NULL;
    SILC_IOFileHandle       new_handle     = SILC_INVALID_IOFILE;
    SILC_ALLOC_NEW_DEFINITION( IOFile, io_file );

    // Init new_definition

    return new_handle;
}


/**
 * Associate a name with a process unique marker group handle.
 */
SILC_MarkerGroupHandle
SILC_DefineMarkerGroup( const char* name )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    SILC_MarkerGroup_Definition* new_definition = NULL;
    SILC_MarkerGroupHandle       new_handle     = SILC_INVALID_MARKER_GROUP;
    SILC_ALLOC_NEW_DEFINITION( MarkerGroup, marker_group );

    // Init new_definition

    return new_handle;
}


/**
 * Associate a name and a group handle with a process unique marker handle.
 */
SILC_MarkerHandle
SILC_DefineMarker
(
    const char*            name,
    SILC_MarkerGroupHandle markerGroup
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    SILC_Marker_Definition* new_definition = NULL;
    SILC_MarkerHandle       new_handle     = SILC_INVALID_MARKER;
    SILC_ALLOC_NEW_DEFINITION( Marker, marker );

    // Init new_definition

    return new_handle;
}


/**
 * Associate a name and a type with a process unique parameter handle.
 */
SILC_ParameterHandle
SILC_DefineParameter
(
    const char*        name,
    SILC_ParameterType type
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    SILC_Parameter_Definition* new_definition = NULL;
    SILC_ParameterHandle       new_handle     = SILC_INVALID_PARAMETER;
    SILC_ALLOC_NEW_DEFINITION( Parameter, parameter );

    // Init new_definition
    new_definition->parameter_type = type;
    HASH_ADD_POD( parameter_type );

    return new_handle;
}
