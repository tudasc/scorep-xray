#include <stdlib.h>
#include <stdio.h>

/**
 * @file       SILC_Definitions.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <SILC_Definitions.h>

/**
 * Associate a file name with a process unique file handle.
 */
SILC_SourceFileHandle
SILC_DefineSourceFile
(
    const char* fileName
)
{
    fprintf( stderr, "%s: Define new source file \"%s\"\n",
             __func__, fileName );

    return SILC_INVALID_SOURCE_FILE;
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
    char stringBuffer[ 16 ];

    fprintf( stderr, "%s: Define new region \"%s\":\n",
             __func__, regionName );
    fprintf( stderr, "    Source file: %s\n",
             silc_source_file_handle_to_string( stringBuffer,
                                                sizeof( stringBuffer ),
                                                "%08x", fileHandle ) );
    fprintf( stderr, "    Start line:  %s\n",
             silc_line_number_to_string( stringBuffer, sizeof( stringBuffer ),
                                         "%u", beginLine ) );
    fprintf( stderr, "    End line:    %s\n",
             silc_line_number_to_string( stringBuffer, sizeof( stringBuffer ),
                                         "%u", endLine ) );
    fprintf( stderr, "    Adpater:     %s\n",
             silc_adapter_type_to_string( adapter ) );
    fprintf( stderr, "    Region type: %s\n",
             silc_region_type_to_string( regionType ) );

    return SILC_INVALID_REGION;
}


/**
 * Associate a MPI communicator with a process unique communicator handle.
 */
SILC_MPICommunicatorHandle
SILC_DefineMPICommunicator
(
    uint32_t            mpiCommunicatorId,
    const unsigned char bitVectorReprOfCommGroup[],
    uint32_t            sizeOfBitVectorReprOfCommGroup
)
{
    fprintf( stderr, "%s: Define new MPI Communicator %x\n",
             __func__, mpiCommunicatorId );
    fprintf( stderr, "    World ranks:" );

    uint32_t ranks_in_line = 0;
    for ( uint32_t i = 0; i < sizeOfBitVectorReprOfCommGroup; ++i )
    {
        /* test bit i in bit vector */
        if ( bitVectorReprOfCommGroup[ i / 8 ] & ( 1u << ( i % 8 ) ) )
        {
            if ( ranks_in_line && ranks_in_line % 16 == 0 )
            {
                fprintf( stderr, "\n%*s",
                         ( int )strlen( "    World ranks:" ), "" );
            }

            fprintf( stderr, " %u", i );

            ranks_in_line++;
        }
    }

    return SILC_INVALID_MPI_COMMUNICATOR;
}

/**
 * Associate a MPI window with a process unique window handle.
 */
SILC_MPIWindowHandle
SILC_DefineMPIWindow
(
    uint32_t                   mpiWindowId,
    SILC_MPICommunicatorHandle communicatorHandle
)
{
    fprintf( stderr, "%s: Define new MPI Window %x\n", __func__, mpiWindowId );

    return SILC_INVALID_MPI_WINDOW;
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
    fprintf( stderr, "%s: Define new MPI cartesian topology \"%s\"\n",
             __func__, topologyName );
    fprintf( stderr, "    Dimensions: %u\n", nDimensions );

    for ( uint32_t i = 0; i < nDimensions; ++i )
    {
        fprintf( stderr, "    Dimension %u:\n", i );
        fprintf( stderr, "        #processes  %u:\n",
                 nProcessesPerDimension[ i ] );
        fprintf( stderr, "        periodicity %hhu:\n",
                 periodicityPerDimension[ i ] );
    }

    return SILC_INVALID_CART_TOPOLOGY;
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

    fprintf( stderr,
             "%s: Define new MPI cartesian coordinates in topology %s\n",
             __func__,
             silc_mpi_cart_topol_to_string( stringBuffer,
                                            sizeof(  stringBuffer ),
                                            "%08x",
                                            cartesianTopologyHandle ) );

    fprintf( stderr, "    Coordinates:" );
    for ( uint32_t i; i < nCoords; ++i )
    {
        fprintf( stderr, " %u", coordsOfCurrentRank[ i ] );
    }

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
    return SILC_INVALID_PARAMETER;
}
