/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SCOREP_TAU_H_
#define SCOREP_TAU_H_



/**
 * @file       /scorep/include/scorep/SCOREP_Tau.h
 * @maintainer Suzanne Millstein <smillst@cs.uoregon.edu>
 *
 * @status alpha
 * @ingroup    TAU
 *
 * @brief Implementation of the TAU adapter functions.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <scorep/SCOREP_PublicTypes.h>


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
 * SCOREP_InitMeasurementMPI().
 *
 * @see SCOREP_FinalizeMeasurement()
 */
void
SCOREP_Tau_InitMeasurement
(
);


/**
 * Type used in specifying line numbers.
 * @see SCOREP_Tau_DefineRegion()
 */
/* typedef uint32_t SCOREP_LineNo; */
/* typedef SCOREP_LineNo SCOREP_Tau_LineNo; */
/* #define SCOREP_Tau_LineNo SCOREP_LineNo */

typedef uint32_t SCOREP_Tau_LineNo;

typedef int ( *SCOREP_Tau_ExitCallback )( void );


/**
 * Symbolic constant representing an invalid or unknown line number.
 * @see SCOREP_DefineRegion()
 */
#define SCOREP_INVALID_LINE_NO                    0
#define SCOREP_TAU_INVALID_LINE_NO                SCOREP_INVALID_LINE_NO
#define SCOREP_TAU_INVALID_SOURCE_FILE            SCOREP_INVALID_SOURCE_FILE


/**
 * Types to be used in defining a region (SCOREP_DefineRegion()). In order to
 * track the origin of a region definition, the adapter needs to provide @e
 * his type.
 *
 */
#define SCOREP_Tau_AdapterType SCOREP_AdapterType

typedef enum SCOREP_AdapterType
{
    SCOREP_ADAPTER_USER,
    SCOREP_ADAPTER_COMPILER,
    SCOREP_ADAPTER_MPI,
    SCOREP_ADAPTER_POMP,
    SCOREP_ADAPTER_PTHREAD,

    SCOREP_INVALID_ADAPTER_TYPE /**< For internal use only. */
} SCOREP_AdapterType;

#define SCOREP_TAU_ADAPTER_USER           SCOREP_ADAPTER_USER
#define SCOREP_TAU_ADAPTER_COMPILER       SCOREP_ADAPTER_COMPILER
#define SCOREP_TAU_ADAPTER_MPI            SCOREP_ADAPTER_MPI
#define SCOREP_TAU_ADAPTER_POMP           SCOREP_ADAPTER_POMP
#define SCOREP_TAU_ADAPTER_PTHREAD        SCOREP_ADAPTER_PTHREAD
#define SCOREP_TAU_INVALID_ADAPTER_TYPE   SCOREP_INVALID_ADAPTER_TYPE

typedef enum SCOREP_RegionType
{
    SCOREP_REGION_UNKNOWN = 0,
    SCOREP_REGION_FUNCTION,
    SCOREP_REGION_LOOP,
    SCOREP_REGION_USER,
    SCOREP_REGION_PHASE,
    SCOREP_REGION_DYNAMIC,

    SCOREP_REGION_DYNAMIC_PHASE,
    SCOREP_REGION_DYNAMIC_LOOP,
    SCOREP_REGION_DYNAMIC_FUNCTION,
    SCOREP_REGION_DYNAMIC_LOOP_PHASE,

    SCOREP_REGION_MPI_COLL_BARRIER,
    SCOREP_REGION_MPI_COLL_ONE2ALL,
    SCOREP_REGION_MPI_COLL_ALL2ONE,
    SCOREP_REGION_MPI_COLL_ALL2ALL,
    SCOREP_REGION_MPI_COLL_OTHER,

    SCOREP_REGION_OMP_PARALLEL,
    SCOREP_REGION_OMP_LOOP,
    SCOREP_REGION_OMP_SECTIONS,
    SCOREP_REGION_OMP_SECTION,
    SCOREP_REGION_OMP_WORKSHARE,
    SCOREP_REGION_OMP_SINGLE,
    SCOREP_REGION_OMP_MASTER,
    SCOREP_REGION_OMP_CRITICAL,
    SCOREP_REGION_OMP_ATOMIC,
    SCOREP_REGION_OMP_BARRIER,
    SCOREP_REGION_OMP_IMPLICIT_BARRIER,
    SCOREP_REGION_OMP_FLUSH,
    SCOREP_REGION_OMP_CRITICAL_SBLOCK, /**< @todo what is SBLOCK? */
    SCOREP_REGION_OMP_SINGLE_SBLOCK,
    SCOREP_REGION_OMP_WRAPPER,

    SCOREP_INVALID_REGION_TYPE /**< For internal use only. */
} SCOREP_RegionType;

#define SCOREP_Tau_RegionType                     SCOREP_RegionType

#define SCOREP_TAU_REGION_UNKNOWN                 SCOREP_REGION_UNKNOWN
#define SCOREP_TAU_REGION_FUNCTION                SCOREP_REGION_FUNCTION
#define SCOREP_TAU_REGION_LOOP                    SCOREP_REGION_LOOP
#define SCOREP_TAU_REGION_USER                    SCOREP_REGION_USER
#define SCOREP_TAU_REGION_PHASE                   SCOREP_REGION_PHASE
#define SCOREP_TAU_REGION_DYNAMIC                 SCOREP_REGION_DYNAMIC
#define SCOREP_TAU_REGION_DYNAMIC_PHASE           SCOREP_REGION_DYNAMIC_PHASE
#define SCOREP_TAU_REGION_DYNAMIC_LOOP            SCOREP_REGION_DYNAMIC_LOOP
#define SCOREP_TAU_REGION_DYNAMIC_LOOP_PHASE      SCOREP_REGION_DYNAMIC_LOOP_PHASE
#define SCOREP_TAU_REGION_MPI_COLL_BARRIER        SCOREP_REGION_MPI_COLL_BARRIER
#define SCOREP_TAU_REGION_MPI_COLL_ONE2ALL        SCOREP_REGION_MPI_COLL_ONE2ALL
#define SCOREP_TAU_REGION_MPI_COLL_ALL2ONE        SCOREP_REGION_MPI_COLL_ALL2ONE
#define SCOREP_TAU_REGION_MPI_COLL_ALL2ALL        SCOREP_REGION_MPI_COLL_ALL2ALL
#define SCOREP_TAU_REGION_MPI_COLL_OTHER          SCOREP_REGION_MPI_COLL_OTHER
#define SCOREP_TAU_REGION_OMP_PARALLEL            SCOREP_REGION_OMP_PARALLEL
#define SCOREP_TAU_REGION_OMP_LOOP                SCOREP_REGION_OMP_LOOP
#define SCOREP_TAU_REGION_OMP_SECTIONS            SCOREP_REGION_OMP_SECTIONS
#define SCOREP_TAU_REGION_OMP_SECTION             SCOREP_REGION_OMP_SECTION
#define SCOREP_TAU_REGION_OMP_WORKSHARE           SCOREP_REGION_OMP_WORKSHARE
#define SCOREP_TAU_REGION_OMP_SINGLE              SCOREP_REGION_OMP_SINGLE
#define SCOREP_TAU_REGION_OMP_MASTER              SCOREP_REGION_OMP_MASTER
#define SCOREP_TAU_REGION_OMP_CRITICAL            SCOREP_REGION_OMP_CRITICAL
#define SCOREP_TAU_REGION_OMP_ATOMIC              SCOREP_REGION_OMP_ATOMIC
#define SCOREP_TAU_REGION_OMP_BARRIER             SCOREP_REGION_OMP_BARRIER
#define SCOREP_TAU_REGION_OMP_IMPLICIT_BARRIER    SCOREP_REGION_OMP_IMPLICIT_BARRIER
#define SCOREP_TAU_REGION_OMP_FLUSH               SCOREP_REGION_OMP_FLUSH
#define SCOREP_TAU_REGION_OMP_CRITICAL_SBLOCK     SCOREP_REGION_OMP_CRITICAL_SBLOCK
#define SCOREP_TAU_REGION_OMP_SINGLE_SBLOCK       SCOREP_REGION_OMP_SINGLE_SBLOCK
#define SCOREP_TAU_REGION_OMP_WRAPPER             SCOREP_REGION_OMP_WRAPPER
#define SCOREP_INVALID_REGION_TYPE                SCOREP_INVALID_REGION_TYPE



/**
 * Associate a code region with a process unique file handle.
 *
 * @param regionName A meaningful name for the region, e.g. a function
 * name. The string will be copied.
 *
 * @param fileHandle A previously defined SCOREP_SourceFileHandle or
 * SCOREP_INVALID_SOURCE_FILE.
 *
 * @param beginLine The file line number where the region starts or
 * SCOREP_INVALID_LINE_NO.
 *
 * @param endLine The file line number where the region ends or
 * SCOREP_INVALID_LINE_NO.
 *
 * @param adapter The type of adapter (SCOREP_AdapterType) that is calling.
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
 * @return A process unique region handle to be used in calls to
 * SCOREP_EnterRegion() and SCOREP_ExitRegion().
 *
 */

#define SCOREP_Tau_RegionHandle           uint64_t
#define SCOREP_Tau_SourceFileHandle       SCOREP_SourceFileHandle
#define SCOREP_Tau_AdapterType            SCOREP_AdapterType

SCOREP_Tau_RegionHandle
SCOREP_Tau_DefineRegion(
    const char*                 regionName,
    SCOREP_Tau_SourceFileHandle fileHandle,
    SCOREP_Tau_LineNo           beginLine,
    SCOREP_Tau_LineNo           endLine,
    SCOREP_Tau_AdapterType      adapter,
    SCOREP_Tau_RegionType       regionType
    );

/**
 * Generate a region enter event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SCOREP_Tau_EnterRegion(
    SCOREP_Tau_RegionHandle regionHandle
    );

/**
 * Generate a region exit event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SCOREP_Tau_ExitRegion(
    SCOREP_Tau_RegionHandle regionHandle
    );

/**
 * Register a function that can close the callstack. This is invoked by
 * the SCOREP routine that is called by atexit. Before flushing the data to
 * disk, all the open timers are closed by invoking the function callback.
 *
 * @param: callback, a function pointer. It points to the routine
 * Tau_profile_exit_all_threads.
 */
void
SCOREP_Tau_RegisterExitCallback(
    SCOREP_Tau_ExitCallback
    );


#ifdef __cplusplus
} /* extern C */
#endif

#endif /* SCOREP_TAU_H_ */
