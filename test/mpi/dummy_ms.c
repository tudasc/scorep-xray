#include "SILC_Definitions.h"
#include "SILC_Events.h"
#include "SILC_RuntimeManagement.h"
#include "SILC_Adapter.h"
#include "SILC_Mpi_Init.h"
#include <stdio.h>

bool ini = false;

SILC_SourceFileHandle
SILC_DefineSourceFile
(
    const char* fileName
)
{
    static SILC_SourceFileHandle fh = 1;
    printf( "Defined source file: %s\n", fileName );
    return fh++;
}

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
    static SILC_RegionHandle rh = 1;
    printf( "Defined region: %s, File: %d\n", regionName, fileHandle );
    return rh++;
}

void
SILC_EnterRegion
(
    SILC_RegionHandle regionHandle
)
{
    printf( "Enter region %d\n", regionHandle );
}

void
SILC_ExitRegion
(
    SILC_RegionHandle regionHandle
)
{
    printf( "Exit region %d\n", regionHandle );
}

void
SILC_InitMeasurement
(
)
{
    ini = true;
    ( *SILC_Mpi_Adapter.adapter_register )();
    ( *SILC_Mpi_Adapter.adapter_init )();
    ( *SILC_Mpi_Adapter.adapter_init_location )();
}

SILC_MPICommunicatorHandle
SILC_DefineMPICommunicator
(
    const unsigned char bitVectorReprOfCommGroup[],
    uint32_t            sizeOfBitVectorReprOfCommGroup
)
{
    static SILC_MPICommunicatorHandle ch = 1;
    printf( "Defined communicator\n" );
    return ch++;
}

SILC_MPIWindowHandle
SILC_DefineMPIWindow
(
    SILC_MPICommunicatorHandle communicatorHandle
)
{
    static SILC_MPIWindowHandle wh = 1;
    return wh++;
}

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
    static SILC_MPICartTopolHandle th = 1;
    return th++;
}

void
SILC_DefineMPICartesianCoords
(
    SILC_MPICartTopolHandle cartesianTopologyHandle,
    uint32_t                nCoords,
    const uint32_t          coordsOfCurrentRank[]
)
{
}

void
SILC_MpiSend
(
    int32_t                    globalDestinationRank,
    SILC_MPICommunicatorHandle communicatorHandle,
    int32_t                    tag,
    int32_t                    bytesSent
)
{
    printf( "SILC_MpiSend: Bytes sent: %d\n", bytesSent );
}

void
SILC_MpiRecv
(
    int32_t                    globalSourceRank,
    SILC_MPICommunicatorHandle communicatorHandle,
    int32_t                    tag,
    int32_t                    bytesReceived
)
{
    printf( "SILC_MpiRecv: Bytes received: %d\n", bytesReceived );
}

void
SILC_MpiCollective
(
    SILC_RegionHandle          regionHandle,
    SILC_MPICommunicatorHandle communicatorHandle,
    int32_t                    globalRootRank,
    int32_t                    bytesSent,
    int32_t                    bytesReceived
)
{
    printf( "SILC_MpiCollective: root %d, bytes sent %d, bytes received %d\n", globalRootRank, bytesSent, bytesReceived );
}

bool
SILC_IsInitialized
(
)
{
    return ini;
}

void
SILC_InitMeasurementMPI
(
)
{
    printf( "SILC_InitMeasurementMPI\n" );
}

char* groups[] = { "CG",  "COLL",  "ERR",     "EXT",     "IO",      "MISC",
                   "P2P", "RMA",   "SPAWN",   "TOPO",    "TYPE",    NULL };

SILC_Error_Code
SILC_ConfigRegister
(
    SILC_ConfigVariable* variables,
    uint32_t             numberOfVariables
)
{
    *( char*** )variables->variableReference = &groups;
}
