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

    SILC_SourceFile_Definition*         new_definition = NULL;
    SILC_SourceFile_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( SourceFile );

    new_definition->name_handle = *SILC_DefineString( fileName );

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID: %x", new_definition->id );

    return new_movable;
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

    SILC_Region_Definition*         new_definition = NULL;
    SILC_Region_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( Region );

    // Init new_definition
    new_definition->name_handle = *SILC_DefineString( regionName );
    new_definition->file_handle = *fileHandle;
    SILC_ALLOCATOR_MOVABLE_INIT_NULL( new_definition->description_handle ); // currently not used
    new_definition->region_type  = regionType;                              // maps to OTF2_RegionType
    new_definition->begin_line   = beginLine;
    new_definition->end_line     = endLine;
    new_definition->adapter_type = adapter;       // currently not used

// TODO: make this to a silc_debug_dump_*_definition
#ifdef SILC_DEBUG
    char stringBuffer[ 16 ];

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID:   %x", new_definition->id );
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

    return new_movable;
}


/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
SILC_MPICommunicatorHandle
SILC_DefineMPICommunicator( const unsigned char bitVectorReprOfCommGroup[],
                            uint32_t            sizeOfBitVectorReprOfCommGroup )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new MPI Communicator:" );

    SILC_MPICommunicator_Definition*         new_definition = NULL;
    SILC_MPICommunicator_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( MPICommunicator );

    // Init new_definition

// TODO: make this into a silc_debug_dump_*_definition function
#ifdef SILC_DEBUG
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID:   %x", new_definition->id );
    SILC_DEBUG_PREFIX( SILC_DEBUG_DEFINITIONS );
    SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS,
                           "    World ranks:" );

    uint32_t ranks_in_line = 0;
    for ( uint32_t i = 0; i < sizeOfBitVectorReprOfCommGroup; ++i )
    {
        /* test bit i in bit vector */
        if ( bitVectorReprOfCommGroup[ i / 8 ] & ( 1u << ( i % 8 ) ) )
        {
            if ( ranks_in_line && ranks_in_line % 16 == 0 )
            {
                SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, "\n" );
                SILC_DEBUG_PREFIX( SILC_DEBUG_DEFINITIONS );
                SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, "%*s",
                                       ( int )strlen( "    World ranks:" ),
                                       "" );
            }

            SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, " %u", i );

            ranks_in_line++;
        }
    }
    SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, "\n" );
#endif

    return new_movable;
}


/**
 * Associate a MPI window with a process unique window handle.
 */
SILC_MPIWindowHandle
SILC_DefineMPIWindow( SILC_MPICommunicatorHandle communicatorHandle )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "Define new MPI Window:" );

    SILC_MPIWindow_Definition*         new_definition = NULL;
    SILC_MPIWindow_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( MPIWindow );

    // Init new_definition

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID: %x", new_definition->id );

    return new_movable;
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

    SILC_MPICartesianTopology_Definition*         new_definition = NULL;
    SILC_MPICartesianTopology_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( MPICartesianTopology );

    // Init new_definition

// TODO: make this to a silc_debug_dump_*_definition
#ifdef SILC_DEBUG
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID:  %x", new_definition->id );
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

    return new_movable;
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

    SILC_MPICartesianCoords_Definition*         new_definition = NULL;
    SILC_MPICartesianCoords_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( MPICartesianCoords );

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
    SILC_CounterGroup_Definition*         new_definition = NULL;
    SILC_CounterGroup_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( CounterGroup );

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    // Init new_definition

    return new_movable;
}


/**
 * Associate the parammeter tuple with a process unique counter handle.
 */
SILC_CounterHandle
SILC_DefineCounter( const char*             name,
                    SILC_CounterType        counterType,
                    SILC_CounterGroupHandle counterGroupHandle,
                    const char*             unit )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    SILC_Counter_Definition*         new_definition = NULL;
    SILC_Counter_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( Counter );

    // Init new_definition

    return new_movable;
}


/**
 * Associate a name with a process unique I/O file group handle.
 */
SILC_IOFileGroupHandle
SILC_DefineIOFileGroup( const char* name )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    SILC_IOFileGroup_Definition*         new_definition = NULL;
    SILC_IOFileGroup_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( IOFileGroup );

    // Init new_definition

    return new_movable;
}


/**
 * Associate a name and a group handle with a process unique I/O file handle.
 */
SILC_IOFileHandle
SILC_DefineIOFile( const char*            name,
                   SILC_IOFileGroupHandle ioFileGroup )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    SILC_IOFile_Definition*         new_definition = NULL;
    SILC_IOFile_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( IOFile );

    // Init new_definition

    return new_movable;
}


/**
 * Associate a name with a process unique marker group handle.
 */
SILC_MarkerGroupHandle
SILC_DefineMarkerGroup( const char* name )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    SILC_MarkerGroup_Definition*         new_definition = NULL;
    SILC_MarkerGroup_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( MarkerGroup );

    // Init new_definition

    return new_movable;
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

    SILC_Marker_Definition*         new_definition = NULL;
    SILC_Marker_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( Marker );

    // Init new_definition

    return new_movable;
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

    SILC_Parameter_Definition*         new_definition = NULL;
    SILC_Parameter_Definition_Movable* new_movable    = NULL;
    SILC_ALLOC_NEW_DEFINITION( Parameter );

    // Init new_definition

    return new_movable;
}
