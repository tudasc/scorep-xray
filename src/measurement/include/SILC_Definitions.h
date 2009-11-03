#ifndef SILC_DEFINITIONS_H
#define SILC_DEFINITIONS_H


/**
 * @file       SILC_Definitions.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include "SILC_Types.h"
#include <stdint.h>


/**
 * @defgroup SILC_Definitions SILC Definitions
 *
 * - Before calling one of the @ref SILC_Events event functions the adapter
     needs to define (i.e. register) entities like regions to be used in
     subsequent definition calls or event function calls.
 *
 * - The definition function calls return opaque handles. The adapter must not
     make any assumptions on the type or the operations that are allowed on
     this type.
 *
 * - A call to a definition function creates internally a process local
     definition. All these process local definitions are unified at the end of
     the measurement.
 *
 * - Definitions are stored per process (as opposed to per loccation) in the
     measurement system. This renders storing of redundant information
     unnecessary and reduces unification expense.
 *
 * - Note that calls to the definition functions are thread
     safe. Synchronization is done internally if appropriate. Changes to this
     approach will be possible if scalability issues arise.
 *
 * - Note that the MPI definition functions also return handles now if
     appropriate (they previously returned void what caused some troubles
     during unification).
 *
 * - Note that the definition functions don't check for uniqueness of their
     arguments but create a new handle for each call. Checking for uniqueness
     is the responsibility of the adapter. Uniqueness of argument tuples is
     required for unification.
 *
 */
/*@{*/


/**
 * Associate a file name with a process unique file handle.
 *
 * @param fileName A meaningful name for the source file.
 *
 * @return A process unique file handle to be used in calls to
 * SILC_DefineRegion().
 *
 */
SILC_SourceFileHandle
SILC_DefineSourceFile
(
    const char* fileName
);


/**
 * Associate a code region with a process unique file handle.
 *
 * @param regionName A meaningful name for the region, e.g. a function
 * name. The string will be copied.
 *
 * @param fileHandle A previously defined SILC_SourceFileHandle or
 * SILC_INVALID_SOURCE_FILE.
 *
 * @param beginLine The file line number where the region starts or
 * SILC_INVALID_LINE_NO.
 *
 * @param endLine The file line number where the region ends or
 * SILC_INVALID_LINE_NO.
 *
 * @param adapter The type of adapter (SILC_AdapterType) that is calling.
 *
 * @param regionType The type of the region. Until now, the @a regionType was
 * not used during the measurement but during analysis. This @e may change in
 * future with e.g. dynamic regions or parameter based profiling. In the first
 * run, we can implement at least dynamic regions in the adapter.
 *
 * @note The name of e.g. Java classes, previously provided as a string to the
 * region description, should now be encoded in the region name. The region
 * description field is replaced by the adapter type as that was it's primary
 * use.
 *
 * @note During unification, we compare @a regionName, @a fileHandle, @a
 * beginLine, @a endLine and @a adapter of regions from different
 * processes. If all values are equal, we consider the regions to be equal. We
 * don't check for uniqueness of this tuple in this function, i.e. during
 * measurement, this is the adapters responsibility, but we require that every
 * call defines a unique and distinguishable region.
 *
 * @todo Change SILC_RegionType from enum to bitstring? When using phases,
 * parametr-based profiling and dynamic regions, combinations of these
 * features may be useful. These combinations can be encoded in a
 * bitstring. The adapter group was queried for detailed input.
 *
 * @return A process unique region handle to be used in calls to
 * SILC_EnterRegion() and SILC_ExitRegion().
 *
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
);


/**
 * Associate a MPI communicator with a process unique communicator handle.
 *
 * @param bitVectorReprOfCommGroup A bitvector representation of the MPI group
 * of the new MPI communicator. Will be copied.
 *
 * @param sizeOfBitVectorReprOfCommGroup The size of the bitvector (in bits).
 *
 * @note See the Scalasca MPI wrapper for the format of the bit vector.
 *
 * @return A process unique communicator handle to be used in calls to other
 * SILC_DefineMPI* functions.
 *
 */
SILC_MPICommunicatorHandle
SILC_DefineMPICommunicator
(
    const unsigned char bitVectorReprOfCommGroup[],
    uint32_t            sizeOfBitVectorReprOfCommGroup
);

/**
 * Associate a MPI window with a process unique window handle.
 *
 * @param communicatorHandle A handle to the associated communicator,
 * previously defined by DefineMPICommunicator().
 *
 * @todo Do we need a return type here, i.e. is the
 * returned handle used somewhere?
 *
 * @planned To be implemented in milestone 2 (belongs to RMA)
 *
 */
SILC_MPIWindowHandle
SILC_DefineMPIWindow
(
    SILC_MPICommunicatorHandle communicatorHandle
);


/**
 * Associate a MPI cartesian topology with a process unique topology handle.
 *
 * @param topologyName A meaningful name for the topology. The string will be
 * copied.
 *
 * @param communicatorHandle A handle to the associated communicator,
 * previously defined by DefineMPICommunicator().
 *
 * @param nDimensions Number of dimensions of the cartesian topology.
 *
 * @param nProcessesPerDimension Number of processes in each dimension.
 *
 * @param periodicityPerDimension Periodicity in each dimension, true (1) or
 * false (0).
 *
 * @note The @a topologyName and the @a communicatorHandle will be used to
 * determine uniqueness during unification (only). It's the user's
 * responsibility to define unique topologies.
 *
 * @return A process unique topology handle to be used in calls to
 * SILC_DefineMPICartesianCoords().
 *
 */
SILC_MPICartTopolHandle
SILC_DefineMPICartesianTopology
(
    const char*                topologyName,
    SILC_MPICommunicatorHandle communicatorHandle,
    uint32_t                   nDimensions,
    const uint32_t             nProcessesPerDimension[],
    const uint8_t              periodicityPerDimension[]
);



/**
 * Define the coordinates of the current rank in the cartesian topology
 * referenced by @a cartesianTopologyHandle.
 *
 * @param cartesianTopologyHandle Handle to the cartesian topology for which
 * the coordinates are defines.
 *
 * @param nCoords Number of dimensions of the cartesian topology.
 *
 * @param coordsOfCurrentRank Coordinates of current rank.
 *
 */
void
SILC_DefineMPICartesianCoords
(
    SILC_MPICartTopolHandle cartesianTopologyHandle,
    uint32_t                nCoords,
    const uint32_t          coordsOfCurrentRank[]
);



/**
 * Associate a name with a process unique counter group handle.
 *
 * @param name A meaningfule name of the counter group.
 *
 * @planned To be implemented in milestone 2
 *
 * @return A process unique counter group handle to be used in calls to
 * SILC_DefineCounter().
 *
 * @planned To be implemented in milestone 2
 *
 */
SILC_CounterGroupHandle
SILC_DefineCounterGroup
(
    const char* name
);


/**
 *
 * Associate the parammeter tuple with a process unique counter handle.
 *
 * @param name A descriptive name for the counter.
 *
 * @param counterType The counters value type.
 *
 * @param counterGroupHandle Handle to the counter group this counter belongs
 * to.
 *
 * @param unit A descriptive name of the unit in which counter values are
 * measured.
 *
 * @return A process unique counter handle to be used in calls to
 * SILC_TriggerCounter().
 *
 * @todo add base parameter (i.e. 1000 or 1024)
 *
 * @planned To be implemented in milestone 2
 *
 */
SILC_CounterHandle
SILC_DefineCounter
(
    const char*             name,
    SILC_CounterType        counterType,
    SILC_CounterGroupHandle counterGroupHandle,
    const char*             unit
);


/**
 * Associate a name with a process unique I/O file group handle.
 *
 * @param name A meaningfule name of the I/O file group.
 *
 * @return A process unique file I/O file group handle to be used in calls to
 * SILC_DefineIOFile().
 *
 * @planned To be implemented in milestone 2
 */
SILC_IOFileGroupHandle
SILC_DefineIOFileGroup
(
    const char* name
);


/**
 * Associate a name and a group handle with a process unique I/O file handle.
 *
 * @param name A meaningful name for the I/O file.
 *
 * @param ioFileGroup Handle to the group the I/O file is associated to.
 *
 * @return A process unique I/O file handle to be used in calls to
 * SILC_TriggerIOFile().
 *
 * @planned To be implemented in milestone 2
 */
SILC_IOFileHandle
SILC_DefineIOFile
(
    const char*            name,
    SILC_IOFileGroupHandle ioFileGroup
);

/**
 * Associate a name with a process unique marker group handle.
 *
 * @param name A meaningfule name of the marker group.
 *
 * @return A process unique marker group handle to be used in calls to
 * SILC_DefineMarker().
 *
 * @planned To be implemented in milestone 2
 */
SILC_MarkerGroupHandle
SILC_DefineMarkerGroup
(
    const char* name
);


/**
 * Associate a name and a group handle with a process unique marker handle.
 *
 * @param name A meaningful name for the marker
 *
 * @param markerGroup Handle to the group the marker is associated to.
 *
 * @return A process unique marker handle to be used in calls to
 * SILC_TriggerMarker().
 *
 * @planned To be implemented in milestone 2
 */
SILC_MarkerHandle
SILC_DefineMarker
(
    const char*            name,
    SILC_MarkerGroupHandle markerGroup
);


/**
 * Associate a name and a type with a process unique parameter handle.
 *
 * @param name A meaningful name for the parameter.
 *
 * @param type The type of the parameter.
 *
 * @return A process unique parameter handle to be used in calls to
 * SILC_TriggerParameter().
 *
 * @planned To be implemented in milestone 3
 *
 */
SILC_ParameterHandle
SILC_DefineParameter
(
    const char*        name,
    SILC_ParameterType type
);


/*@}*/

#endif /* SILC_DEFINITIONS_H */
