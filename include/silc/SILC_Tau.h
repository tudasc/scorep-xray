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

#ifndef SILC_TAU_H_
#define SILC_TAU_H_



/**
 * @file       /silc/include/silc/SILC_Tau.h
 * @maintainer Sameer Shende <sameer@cs.uoregon.edu>
 *
 * @status alpha
 * @ingroup    TAU
 *
 * @brief Implementation of the TAU adapter functions.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <SILC_PublicTypes.h>


/**
 * Initialize the measurement system from the adapter layer. This function
 * needs to be called at least once by an (arbitrary) adapter before any other
 * measurement API function is called. Calling other API functions before is
 * seen as undefined behaviour. The first call to this function triggers the
 * initialization of all adapters in use.
 *
 * For performance reasons the adapter should keep track of it's
 * initialization status and call this function only once.
 *
 * Calling this function several times does no harm to the measurement system.
 *
 * Each arising error leads to a fatal abortion of the program.
 *
 * @note The MPI adapter needs special treatment, see
 * SILC_InitMeasurementMPI().
 *
 * @see SILC_FinalizeMeasurement()
 */
void
SILC_Tau_InitMeasurement
(
);


/**
 * Type used in specifying line numbers.
 * @see SILC_Tau_DefineRegion()
 */
/* typedef uint32_t SILC_LineNo; */
/* typedef SILC_LineNo SILC_Tau_LineNo; */
/* #define SILC_Tau_LineNo SILC_LineNo */

typedef uint32_t SILC_Tau_LineNo;

/**
 * Symbolic constant representing an invalid or unknown line number.
 * @see SILC_DefineRegion()
 */
#define SILC_INVALID_LINE_NO                    0
#define SILC_TAU_INVALID_LINE_NO                SILC_INVALID_LINE_NO
#define SILC_TAU_INVALID_SOURCE_FILE            SILC_INVALID_SOURCE_FILE


/**
 * Types to be used in defining a region (SILC_DefineRegion()). In order to
 * track the origin of a region definition, the adapter needs to provide @e
 * his type.
 *
 */
#define SILC_Tau_AdapterType SILC_AdapterType

typedef enum SILC_AdapterType
{
    SILC_ADAPTER_USER,
    SILC_ADAPTER_COMPILER,
    SILC_ADAPTER_MPI,
    SILC_ADAPTER_POMP,
    SILC_ADAPTER_PTHREAD,

    SILC_INVALID_ADAPTER_TYPE /**< For internal use only. */
} SILC_AdapterType;

#define SILC_TAU_ADAPTER_USER           SILC_ADAPTER_USER
#define SILC_TAU_ADAPTER_COMPILER       SILC_ADAPTER_COMPILER
#define SILC_TAU_ADAPTER_MPI            SILC_ADAPTER_MPI
#define SILC_TAU_ADAPTER_POMP           SILC_ADAPTER_POMP
#define SILC_TAU_ADAPTER_PTHREAD        SILC_ADAPTER_PTHREAD
#define SILC_TAU_INVALID_ADAPTER_TYPE   SILC_INVALID_ADAPTER_TYPE

typedef enum SILC_RegionType
{
    SILC_REGION_UNKNOWN = 0,
    SILC_REGION_FUNCTION,
    SILC_REGION_LOOP,
    SILC_REGION_USER,
    SILC_REGION_PHASE,
    SILC_REGION_DYNAMIC,

    SILC_REGION_DYNAMIC_PHASE,
    SILC_REGION_DYNAMIC_LOOP,
    SILC_REGION_DYNAMIC_FUNCTION,
    SILC_REGION_DYNAMIC_LOOP_PHASE,

    SILC_REGION_MPI_COLL_BARRIER,
    SILC_REGION_MPI_COLL_ONE2ALL,
    SILC_REGION_MPI_COLL_ALL2ONE,
    SILC_REGION_MPI_COLL_ALL2ALL,
    SILC_REGION_MPI_COLL_OTHER,

    SILC_REGION_OMP_PARALLEL,
    SILC_REGION_OMP_LOOP,
    SILC_REGION_OMP_SECTIONS,
    SILC_REGION_OMP_SECTION,
    SILC_REGION_OMP_WORKSHARE,
    SILC_REGION_OMP_SINGLE,
    SILC_REGION_OMP_MASTER,
    SILC_REGION_OMP_CRITICAL,
    SILC_REGION_OMP_ATOMIC,
    SILC_REGION_OMP_BARRIER,
    SILC_REGION_OMP_IMPLICIT_BARRIER,
    SILC_REGION_OMP_FLUSH,
    SILC_REGION_OMP_CRITICAL_SBLOCK, /**< @todo what is SBLOCK? */
    SILC_REGION_OMP_SINGLE_SBLOCK,
    SILC_REGION_OMP_WRAPPER,

    SILC_INVALID_REGION_TYPE /**< For internal use only. */
} SILC_RegionType;

#define SILC_Tau_RegionType                     SILC_RegionType

#define SILC_TAU_REGION_UNKNOWN                 SILC_REGION_UNKNOWN
#define SILC_TAU_REGION_FUNCTION                SILC_REGION_FUNCTION
#define SILC_TAU_REGION_LOOP                    SILC_REGION_LOOP
#define SILC_TAU_REGION_USER                    SILC_REGION_USER
#define SILC_TAU_REGION_PHASE                   SILC_REGION_PHASE
#define SILC_TAU_REGION_DYNAMIC                 SILC_REGION_DYNAMIC
#define SILC_TAU_REGION_DYNAMIC_PHASE           SILC_REGION_DYNAMIC_PHASE
#define SILC_TAU_REGION_DYNAMIC_LOOP            SILC_REGION_DYNAMIC_LOOP
#define SILC_TAU_REGION_DYNAMIC_LOOP_PHASE      SILC_REGION_DYNAMIC_LOOP_PHASE
#define SILC_TAU_REGION_MPI_COLL_BARRIER        SILC_REGION_MPI_COLL_BARRIER
#define SILC_TAU_REGION_MPI_COLL_ONE2ALL        SILC_REGION_MPI_COLL_ONE2ALL
#define SILC_TAU_REGION_MPI_COLL_ALL2ONE        SILC_REGION_MPI_COLL_ALL2ONE
#define SILC_TAU_REGION_MPI_COLL_ALL2ALL        SILC_REGION_MPI_COLL_ALL2ALL
#define SILC_TAU_REGION_MPI_COLL_OTHER          SILC_REGION_MPI_COLL_OTHER
#define SILC_TAU_REGION_OMP_PARALLEL            SILC_REGION_OMP_PARALLEL
#define SILC_TAU_REGION_OMP_LOOP                SILC_REGION_OMP_LOOP
#define SILC_TAU_REGION_OMP_SECTIONS            SILC_REGION_OMP_SECTIONS
#define SILC_TAU_REGION_OMP_SECTION             SILC_REGION_OMP_SECTION
#define SILC_TAU_REGION_OMP_WORKSHARE           SILC_REGION_OMP_WORKSHARE
#define SILC_TAU_REGION_OMP_SINGLE              SILC_REGION_OMP_SINGLE
#define SILC_TAU_REGION_OMP_MASTER              SILC_REGION_OMP_MASTER
#define SILC_TAU_REGION_OMP_CRITICAL            SILC_REGION_OMP_CRITICAL
#define SILC_TAU_REGION_OMP_ATOMIC              SILC_REGION_OMP_ATOMIC
#define SILC_TAU_REGION_OMP_BARRIER             SILC_REGION_OMP_BARRIER
#define SILC_TAU_REGION_OMP_IMPLICIT_BARRIER    SILC_REGION_OMP_IMPLICIT_BARRIER
#define SILC_TAU_REGION_OMP_FLUSH               SILC_REGION_OMP_FLUSH
#define SILC_TAU_REGION_OMP_CRITICAL_SBLOCK     SILC_REGION_OMP_CRITICAL_SBLOCK
#define SILC_TAU_REGION_OMP_SINGLE_SBLOCK       SILC_REGION_OMP_SINGLE_SBLOCK
#define SILC_TAU_REGION_OMP_WRAPPER             SILC_REGION_OMP_WRAPPER
#define SILC_INVALID_REGION_TYPE                SILC_INVALID_REGION_TYPE



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

#define SILC_Tau_RegionHandle           uint64_t
#define SILC_Tau_SourceFileHandle       SILC_SourceFileHandle
#define SILC_Tau_AdapterType            SILC_AdapterType

SILC_Tau_RegionHandle
SILC_Tau_DefineRegion(
    const char*               regionName,
    SILC_Tau_SourceFileHandle fileHandle,
    SILC_Tau_LineNo           beginLine,
    SILC_Tau_LineNo           endLine,
    SILC_Tau_AdapterType      adapter,
    SILC_Tau_RegionType       regionType
    );

/**
 * Generate a region enter event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SILC_Tau_EnterRegion(
    SILC_Tau_RegionHandle regionHandle
    );

/**
 * Generate a region exit event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SILC_Tau_ExitRegion(
    SILC_Tau_RegionHandle regionHandle
    );


#ifdef __cplusplus
} /* extern C */
#endif

#endif /* SILC_TAU_H_ */
