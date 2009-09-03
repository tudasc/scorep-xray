#ifndef SILC_API_DEFINITIONS_H
#define SILC_API_DEFINITIONS_H


/**
 * @file    SILC_API_Definitions.h
 * @author  Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @date    Started Thu Sep  3 11:13:44 2009
 *
 * @brief Declaration of define functions to be used by the adapter layer.
 *
 */


#include "SILC_API_Types.h"
#include <mpi.h>
#include <stdint.h>


/**
 * Associate a file name with a process unique file handle.
 *
 * @note This function is not thread safe. If you are calling it from multiple
 * threads simultaneously, you are responsible for sufficient synchronization.
 *
 * @return A process unique file handle to be used in calls to
 * SILC_API_DefineRegion().
 *
 */
SILC_API_FileHandle SILC_API_DefineFile( const char* fileName );


/**
 * Associate a code region with a process unique file handle.
 *
 * @param regionName
 * @param fileHandle
 * @param beginLine
 * @param endLine
 * @param adapter
 * @param regionType
 *
 * @note This function is not thread safe. If you are calling it from multiple
 * threads simultaneously, you are responsible for sufficient synchronization.
 *
 * @return A process unique region handle to be used in calls to
 * SILC_API_EnterRegion() and SILC_API_ExitRegion().
 *
 */
SILC_API_RegionHandle SILC_API_DefineRegion( const char*           regionName,
                                             SILC_API_FileHandle   fileHandle,
                                             SILC_API_LineNo       beginLine,
                                             SILC_API_LineNo       endLine,
                                             SILC_API_AdapterTypes adapter,
                                             SILC_API_RegionType   regionType );


/**
 * @name MPI adapter specific definitions
 */
/*@{*/

/**
 *
 *
 * @param mpiCommunicatorId
 * @param sizeOfBitVectorReprOfCommGroup
 * @param bitVectorReprOfCommGroup
 *
 * @todo specify format of bit vector representation
 *
 * @note This function is not thread safe. If you are calling it from multiple
 * threads simultaneously, you are responsible for sufficient synchronization.
 *
 * @return A process unique communicator handle to be used in calls to other
 * SILC_API_DefineMPI* functions.
 *
 */
SILC_API_MPICommunicatorHandle SILC_API_DefineMPICommunicator(
    MPI_Comm mpiCommunicatorId,
    unsigned
             sizeOfBitVectorReprOfCommGroup,
    unsigned char
             bitVectorReprOfCommGroup
    [] );


/**
 *
 * @param mpiWindowId
 * @param communicatorHandle
 *
 * @todo do we need a return type here?
 *
 * @note This function is not thread safe. If you are calling it from multiple
 * threads simultaneously, you are responsible for sufficient synchronization.
 *
 */
SILC_API_MPIWindowHandle SILC_API_DefineMPIWindow(
    MPI_Win mpiWindowId,
    SILC_API_MPICommunicatorHandle
            communicatorHandle );


/**
 *
 *
 * @param topologyName
 * @param communicatorHandle
 * @param nDimensions
 * @param processesPerDimension
 * @param periodicityPerDimension
 *
 * @note This function is not thread safe. If you are calling it from multiple
 * threads simultaneously, you are responsible for sufficient synchronization.
 *
 * @return A process unique topology handle to be used in calls to
 * SILC_API_DefineMPICartesianCoords().
 *
 */
SILC_API_MPICartTopolHandle SILC_API_DefineMPICartesianTopology(
    char* topologyName,
    SILC_API_MPICommunicatorHandle
          communicatorHandle,
    uint32_t
          nDimensions,
    uint32_t
          processesPerDimension
    [],
    uint32_t
          periodicityPerDimension
    [] );



/**
 *
 *
 * @param topologyHandle
 * @param nCoords
 * @param coordsOfCurrentRank
 *
 * @note This function is not thread safe. If you are calling it from multiple
 * threads simultaneously, you are responsible for sufficient synchronization.
 *
 */
void SILC_API_DefineMPICartesianCoords(
    SILC_API_MPICartTopolHandle topologyHandle,
    uint32_t                    nCoords,
    uint32_t
                                coordsOfCurrentRank
    [] );

/*@}*/


/**
 * @todo bert
   SILC_API_DefineComment
   SILC_API_DefineFileGroup
   SILC_API_DefineFileWithGroup
   SILC_API_DefineCounterGroup
   SILC_API_DefineCounterWithGroup
   SILC_API_DefineMarker
 */

#endif /* SILC_API_DEFINITIONS_H */
