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


#include <SILC_Debug.h>
#include <SILC_Definitions.h>


#include "silc_types.h"


/**
 * Associate a file name with a process unique file handle.
 */
SILC_SourceFileHandle
SILC_DefineSourceFile
(
    const char* fileName
)
{
    static SILC_SourceFileHandle next_source_file_handle;

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new source file \"%s\":",
                       fileName );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID: %x", next_source_file_handle );

    return next_source_file_handle++;
}


/**
 * Associate a code region with a process unique file handle.
 */
SILC_RegionHandle
SILC_DefineRegion
(
    const char*           regionName,
    SILC_SourceFileHandle fileHandle,
    SILC_LineNo           beginLine,
    SILC_LineNo           endLine,
    SILC_AdapterType      adapter,
    SILC_RegionType       regionType
)
{
    static SILC_RegionHandle next_region_handle;


    SILC_DEBUG_ONLY( char stringBuffer[ 16 ];
                     )

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new region \"%s\":", regionName );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID:   %x", next_region_handle );
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

    return next_region_handle++;
}


/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
SILC_MPICommunicatorHandle
SILC_DefineMPICommunicator
(
    const unsigned char bitVectorReprOfCommGroup[],
    uint32_t            sizeOfBitVectorReprOfCommGroup
)
{
    static SILC_MPICommunicatorHandle next_mpi_communicator_handle;

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new MPI Communicator:" );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID:   %x", next_mpi_communicator_handle );
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

    return next_mpi_communicator_handle++;
}


/**
 * Associate a MPI window with a process unique window handle.
 */
SILC_MPIWindowHandle
SILC_DefineMPIWindow
(
    SILC_MPICommunicatorHandle communicatorHandle
)
{
    static SILC_MPIWindowHandle next_mpi_window_handle;

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "Define new MPI Window:" );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID: %x", next_mpi_window_handle );

    return next_mpi_window_handle++;
}


/**
 * Associate a MPI cartesian topology with a process unique topology handle.
 */
SILC_MPICartTopolHandle
SILC_DefineMPICartesianTopology
(
    const char*                topologyName,
    SILC_MPICommunicatorHandle communicatorHandle,
    uint32_t                   nDimensions,
    const uint32_t             nProcessesPerDimension[],
    const uint8_t              periodicityPerDimension[]
)
{
    static SILC_MPICartTopolHandle next_mpi_cart_topol_handle;

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new MPI cartesian topology \"%s\":",
                       topologyName );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Handle ID:  %x", next_mpi_cart_topol_handle );
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "    Dimensions: %u", nDimensions );

    for ( uint32_t i = 0; i < nDimensions; ++i )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                           "    Dimension %u:\n", i );
        SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                           "        #processes  %u:\n",
                           nProcessesPerDimension[ i ] );
        SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                           "        periodicity %hhu:\n",
                           periodicityPerDimension[ i ] );
    }

    return next_mpi_cart_topol_handle++;
}


/**
 * Define the coordinates of the current rank in the cartesian topology
 * referenced by @a cartesianTopologyHandle.
 */
void
SILC_DefineMPICartesianCoords
(
    SILC_MPICartTopolHandle cartesianTopologyHandle,
    uint32_t                nCoords,
    const uint32_t          coordsOfCurrentRank[]
)
{
    char stringBuffer[ 16 ];

    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS,
                       "Define new MPI cartesian coordinates in topology %s",
                       silc_mpi_cart_topol_to_string( stringBuffer,
                                                      sizeof(  stringBuffer ),
                                                      "%x",
                                                      cartesianTopologyHandle ) );

    SILC_DEBUG_PREFIX( SILC_DEBUG_DEFINITIONS );
    SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, "    Coordinates:" );
    for ( uint32_t i; i < nCoords; ++i )
    {
        SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS,
                               " %u", coordsOfCurrentRank[ i ] );
    }
    SILC_DEBUG_RAW_PRINTF( SILC_DEBUG_DEFINITIONS, "\n" );

    return;
}


/**
 * Associate a name with a process unique counter group handle.
 */
SILC_CounterGroupHandle
SILC_DefineCounterGroup
(
    const char* name
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    return SILC_INVALID_COUNTER_GROUP;
}


/**
 * Associate the parammeter tuple with a process unique counter handle.
 */
SILC_CounterHandle
SILC_DefineCounter
(
    const char*             name,
    SILC_CounterType        counterType,
    SILC_CounterGroupHandle counterGroupHandle,
    const char*             unit
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    return SILC_INVALID_COUNTER;
}


/**
 * Associate a name with a process unique I/O file group handle.
 */
SILC_IOFileGroupHandle
SILC_DefineIOFileGroup
(
    const char* name
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    return SILC_INVALID_IOFILE_GROUP;
}


/**
 * Associate a name and a group handle with a process unique I/O file handle.
 */
SILC_IOFileHandle
SILC_DefineIOFile
(
    const char*            name,
    SILC_IOFileGroupHandle ioFileGroup
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    return SILC_INVALID_IOFILE;
}


/**
 * Associate a name with a process unique marker group handle.
 */
SILC_MarkerGroupHandle
SILC_DefineMarkerGroup
(
    const char* name
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_DEFINITIONS, "" );

    return SILC_INVALID_MARKER_GROUP;
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

    return SILC_INVALID_MARKER;
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

    return SILC_INVALID_PARAMETER;
}
