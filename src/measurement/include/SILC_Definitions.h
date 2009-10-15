#ifndef SILC_DEFINITIONS_H
#define SILC_DEFINITIONS_H


/**
 * @file    SILC_Definitions.h
 * @author  Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 * @date    Started Thu Sep  3 11:13:44 2009
 *
 * @brief Declaration of define functions to be used by the adapter layer.
 *
 */


#include <SILC_Types.h>
#include <mpi.h>
#include <stdint.h>


/**
 * Associate a file name with a process unique file handle.
 *
 * @note This function is not thread safe. If you are calling it from multiple
 * threads simultaneously, you are responsible for sufficient synchronization.
 *
 * @return A process unique file handle to be used in calls to
 * SILC_DefineRegion().
 *
 */
SILC_SourceFileHandle SILC_DefineSourceFile( const char* fileName );


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
 * SILC_EnterRegion() and SILC_ExitRegion().
 *
 */
SILC_RegionHandle SILC_DefineRegion( const char*           regionName,
                                     SILC_SourceFileHandle fileHandle,
                                     SILC_LineNo           beginLine,
                                     SILC_LineNo           endLine,
                                     SILC_AdapterTypes     adapter,
                                     SILC_RegionType       regionType );


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
 * SILC_DefineMPI* functions.
 *
 */
SILC_MPICommunicatorHandle SILC_DefineMPICommunicator(
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
SILC_MPIWindowHandle SILC_DefineMPIWindow( MPI_Win mpiWindowId,
                                           SILC_MPICommunicatorHandle
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
 * SILC_DefineMPICartesianCoords().
 *
 */
SILC_MPICartTopolHandle SILC_DefineMPICartesianTopology(
    char* topologyName,
    SILC_MPICommunicatorHandle
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
void SILC_DefineMPICartesianCoords( SILC_MPICartTopolHandle topologyHandle,
                                    uint32_t                nCoords,
                                    uint32_t                coordsOfCurrentRank
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
void SILC_DefineComment( const char* format,
                         ... );

/**
 *
 * @param name name of the counter group
 *
 * @note This function is not thread safe. If you are calling it from multiple
 * threads simultaneously, you are responsible for sufficient synchronization.
 *
 */
SILC_CounterGroupHandle SILC_DefineCounterGroup( const char* name );

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
SILC_CounterHandle SILC_DefineCounter( const char*             name,
                                       uint32_t                properties,
                                       SILC_CounterGroupHandle counter_group,
                                       const char*             unit );


/**
 * define a parameter for parameter based profiling
 *
 * @return a location unique id handle to be passed to the parameter trigger
 * functions
 *
 */
SILC_ParameterHandle SILC_DefineParameter( const char*        name,
                                           SILC_ParameterType type );

/**
 * @todo bert
   SILC_DefineFileGroup
   SILC_DefineFile
   SILC_DefineMarkerGroup
   SILC_DefineMarker
 */

#endif /* SILC_DEFINITIONS_H */
