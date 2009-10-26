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

/*
 * forward declarations of static functions
 */

static inline const char*
source_file_handle_to_string
(
    SILC_SourceFileHandle handle,
    char*                 stringBuffer
    size_t                stringBufferSize
);

static inline const char*
line_number_to_string
(
    SILC_LineNo lineNo,
    char*       stringBuffer
    size_t      stringBufferSize
);

static inline const char*
adapter_type_to_string
(
    SILC_AdapterType adapterType
);

static inline const char*
region_type_to_string
(
    SILC_RegionType regionType
);

/**
 * Associate a file name with a process unique file handle.
 */
SILC_SourceFileHandle
SILC_DefineSourceFile
(
    const char* fileName
)
{
    fprintf( stderr, "%s: Define new source file \"%s\"\n", fileName );

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

    fprintf( stderr, "%s: Define new region \"%s\":\n", regionName );
    fprintf( stderr, "    Source file: %s\n",
             source_file_handle_to_string( fileHandle,
                                           stringBuffer,
                                           sizeof( stringBuffer ) ) );
    fprintf( stderr, "    Start line:  %s\n",
             line_number_to_string( beginLine,
                                    stringBuffer,
                                    sizeof( stringBuffer ) ) );
    fprintf( stderr, "    End line:    %s\n",
             line_number_to_string( endLine,
                                    stringBuffer,
                                    sizeof( stringBuffer ) ) );
    fprintf( stderr, "    Adpater:     %s\n",
             adapter_type_to_string( adapter ) );
    fprintf( stderr, "    Region type: %s\n",
             region_type_to_string( regionType ) );

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

/*
 * functions for pretty printing types
 *
 * @todo move this to silc_types.c
 */

static inline const char*
source_file_handle_to_string
(
    SILC_SourceFileHandle handle,
    char*                 stringBuffer
    size_t                stringBufferSize
)
{
    if ( SILC_INVALID_SOURCE_FILE == handle )
    {
        return "invalid";
    }

    snprintf( stringBuffer, stringBufferSize, "%08x", handle );
    return stringBuffer;
}

static inline const char*
line_number_to_string
(
    SILC_LineNo lineNo,
    char*       stringBuffer
    size_t      stringBufferSize
)
{
    if ( SILC_INVALID_LINE_NO == lineNo )
    {
        return "invalid";
    }

    snprintf( stringBuffer, stringBufferSize, "%u", lineNo );
    return stringBuffer;
}

static inline const char*
adapter_type_to_string
(
    SILC_AdapterType adapterType
)
{
    switch ( adapterType )
    {
        case SILC_ADAPTER_USER:
            return "user";
        case SILC_ADAPTER_COMPILER:
            return "compiler";
        case SILC_ADAPTER_MPI:
            return "mpi";
        case SILC_ADAPTER_POMP:
            return "pomp";
        case SILC_ADAPTER_PTHREAD:
            return "pthread";
        default:
            return "unknown";
    }
}

static inline const char*
region_type_to_string
(
    SILC_RegionType regionType
)
{
    switch ( regionType )
    {
        case SILC_REGION_FUNCTION:
            return "fnction";
        case SILC_REGION_LOOP:
            return "loop";
        case SILC_REGION_USER:
            return "user";
        case SILC_REGION_PHASE:
            return "phase";
        case SILC_REGION_DYNAMIC:
            return "dynamic";
        case SILC_REGION_DYNAMIC_PHASE:
            return "dynamic phase";
        case SILC_REGION_DYNAMIC_LOOP:
            return "dynamic loop";
        case SILC_REGION_DYNAMIC_FUNCTION:
            return "dynamix function";
        case SILC_REGION_DYNAMIC_LOOP_PHASE:
            return "dynamix loop phase";
        case SILC_REGION_MPI_COLL_BARRIER:
            return "mpi barrier";
        case SILC_REGION_MPI_COLL_ONE2ALL:
            return "mpi one2all";
        case SILC_REGION_MPI_COLL_ALL2ONE:
            return "mpi all2one";
        case SILC_REGION_MPI_COLL_ALL2ALL:
            return "mpi all2all";
        case SILC_REGION_MPI_COLL_OTHER:
            return "mpi other";
        case SILC_REGION_OMP_PARALLEL:
            return "omp parallel";
        case SILC_REGION_OMP_LOOP:
            return "omp loop";
        case SILC_REGION_OMP_SECTIONS:
            return "omp secions";
        case SILC_REGION_OMP_SECTION:
            return "omp section";
        case SILC_REGION_OMP_WORKSHARE:
            return "omp workshare";
        case SILC_REGION_OMP_SINGLE:
            return "omp single";
        case SILC_REGION_OMP_MASTER:
            return "omp master";
        case SILC_REGION_OMP_CRITICAL:
            return "omp critical";
        case SILC_REGION_OMP_ATOMIC:
            return "omp atomic";
        case SILC_REGION_OMP_BARRIER:
            return "omp barrier";
        case SILC_REGION_OMP_IMPLICIT_BARRIER:
            return "omp implicit barrier";
        case SILC_REGION_OMP_FLUSH:
            return "omp flush";
        case SILC_REGION_OMP_CRITICAL_SBLOCK:
            return "omp critical sblock";
        case SILC_REGION_OMP_SINGLE_SBLOCK:
            return "omp single sblock";
        default:
            return "unknown";
    }
}
