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
SILC_API_SourceFileHandle SILC_API_DefineSourceFile( const char* fileName );


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
                                             SILC_API_SourceFileHandle
                                                                   fileHandle,
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
 * Write an arbitrary string into the definition stream of the current location
 *
 * @param format a printf(3) style format specifier string
 * @param ...    additional arguments to format
 *
 * @note This function is not thread safe. If you are calling it from multiple
 * threads simultaneously, you are responsible for sufficient synchronization.
 *
 */
void SILC_API_DefineComment( const char* format,
                             ... );

/**
 *
 * @param name name of the counter group
 *
 * @note This function is not thread safe. If you are calling it from multiple
 * threads simultaneously, you are responsible for sufficient synchronization.
 *
 */
SILC_API_CounterGroupHandle SILC_API_DefineCounterGroup( const char* name );

/**
 *
 * @param name          name of the counter
 * @param properties    properties of the counter, as defined by OTF2
 * @param counter_group the group in which this counter is
 * @param unit          a descriptive name of the unit
 *
 * @todo: add base parameter (i.e. 1000 or 1024)
 *
 * @note This function is not thread safe. If you are calling it from multiple
 * threads simultaneously, you are responsible for sufficient synchronization.
 *
 */
SILC_API_CounterHandle SILC_API_DefineCounter( const char* name,
                                               uint32_t    properties,
                                               SILC_API_CounterGroupHandle
                                                           counter_group,
                                               const char* unit );


/**
 * @todo bert
   SILC_API_DefineFileGroup
   SILC_API_DefineFile
   SILC_API_DefineMarkerGroup
   SILC_API_DefineMarker
 */

#endif /* SILC_API_DEFINITIONS_H */
