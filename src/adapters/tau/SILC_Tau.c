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



/**
 * @file       /silc/src/adapters/tau/SILC_Tau.c
 * @maintainer Sameer Shende <sameer@cs.uoregon.edu>
 * @status alpha
 * @ingroup    TAU
 *
 * @brief Implementation of the TAU adapter functions.
 */


#include <config.h>
#include <SILC_Tau.h>

/*
 #include <SILC_Types.h>
 #include <SILC_Events.h>
 #include <SILC_Definitions.h>
 */

typedef uint32_t SILC_LineNo;

extern void
SILC_InitMeasurement
(
);

extern SILC_RegionHandle
SILC_DefineRegion(
    const char*           regionName,
    SILC_SourceFileHandle fileHandle,
    SILC_LineNo           beginLine,
    SILC_LineNo           endLine,
    SILC_AdapterType      adapter,
    SILC_RegionType       regionType
    );

extern void
SILC_EnterRegion(
    SILC_RegionHandle regionHandle
    );

extern void
SILC_ExitRegion(
    SILC_Tau_RegionHandle regionHandle
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
 * SILC_InitMeasurementMPI().
 *
 * @see SILC_FinalizeMeasurement()
 */

void
SILC_Tau_InitMeasurement()
{
    SILC_InitMeasurement();
}

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

SILC_Tau_RegionHandle
SILC_Tau_DefineRegion(
    const char*               regionName,
    SILC_Tau_SourceFileHandle fileHandle,
    SILC_Tau_LineNo           beginLine,
    SILC_Tau_LineNo           endLine,
    SILC_Tau_AdapterType      adapter,
    SILC_Tau_RegionType       regionType
    )
{
    return ( SILC_Tau_RegionHandle )SILC_DefineRegion( regionName, fileHandle,
                                                       beginLine, endLine, adapter, regionType );
}


/**
 * Generate a region enter event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SILC_Tau_EnterRegion( SILC_Tau_RegionHandle regionHandle )
{
    SILC_EnterRegion( ( SILC_RegionHandle )regionHandle );
}

/**
 * Generate a region exit event in the measurement system.
 *
 * @param regionHandle The corresponding region for the enter event.
 */
void
SILC_Tau_ExitRegion( SILC_Tau_RegionHandle regionHandle )
{
    SILC_ExitRegion( ( SILC_RegionHandle )regionHandle );
}



/* *INDENT-OFF* */
/* *INDENT-ON*  */
/** @} */
