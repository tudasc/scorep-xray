/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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



/**
 * @file       /scorep/src/adapters/tau/SCOREP_Tau.c
 * @maintainer Sameer Shende <sameer@cs.uoregon.edu>
 * @status alpha
 * @ingroup    TAU
 *
 * @brief Implementation of the TAU adapter functions.
 */


#include <config.h>
#include <SCOREP_Tau.h>

/*
 #include <SCOREP_Types.h>
 #include <SCOREP_Events.h>
 #include <SCOREP_Definitions.h>
 */

typedef uint32_t SCOREP_LineNo;

extern void
SCOREP_InitMeasurement
(
);

extern SCOREP_RegionHandle
SCOREP_DefineRegion(
    const char*             regionName,
    SCOREP_SourceFileHandle fileHandle,
    SCOREP_LineNo           beginLine,
    SCOREP_LineNo           endLine,
    SCOREP_AdapterType      adapter,
    SCOREP_RegionType       regionType
    );

extern void
SCOREP_EnterRegion(
    SCOREP_RegionHandle regionHandle
    );

extern void
SCOREP_ExitRegion(
    SCOREP_Tau_RegionHandle regionHandle
    );





/** @ingroup TAU
    @{
 */
/* **************************************************************************************
 *                                                                   TAU event functions
 ***************************************************************************************/

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
SCOREP_Tau_InitMeasurement()
{
    SCOREP_InitMeasurement();
}

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
 * @todo Change SCOREP_RegionType from enum to bitstring? When using phases,
 * parametr-based profiling and dynamic regions, combinations of these
 * features may be useful. These combinations can be encoded in a
 * bitstring. The adapter group was queried for detailed input.
 *
 * @return A process unique region handle to be used in calls to
 * SCOREP_EnterRegion() and SCOREP_ExitRegion().
 *
 */

SCOREP_Tau_RegionHandle
SCOREP_Tau_DefineRegion(
    const char*                 regionName,
    SCOREP_Tau_SourceFileHandle fileHandle,
    SCOREP_Tau_LineNo           beginLine,
    SCOREP_Tau_LineNo           endLine,
    SCOREP_Tau_AdapterType      adapter,
    SCOREP_Tau_RegionType       regionType
    )
{
    return ( SCOREP_Tau_RegionHandle )SCOREP_DefineRegion( regionName, fileHandle,
                                                           beginLine, endLine, adapter, regionType );
}


/**
 * Generate a region enter event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SCOREP_Tau_EnterRegion( SCOREP_Tau_RegionHandle regionHandle )
{
    SCOREP_EnterRegion( ( SCOREP_RegionHandle )regionHandle );
}

/**
 * Generate a region exit event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SCOREP_Tau_ExitRegion( SCOREP_Tau_RegionHandle regionHandle )
{
    SCOREP_ExitRegion( ( SCOREP_RegionHandle )regionHandle );
}



/* *INDENT-OFF* */
/* *INDENT-ON*  */
/** @} */
