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

#ifndef SCOREP_DEFINITIONLOCKING_H
#define SCOREP_DEFINITIONLOCKING_H



/**
 * @file       SCOREP_DefinitionLocking.h
 *             Declarations of lock and unlock functions to be used when
 *             calling corresponding SCOREP_DefineFoo functions. The locking and
 *             unlocking is the responsibility of the adapter, as it anyhow
 *             needs to synchronize the access to it's internal data
 *             structures when defining new entities.
 *
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


/**
 * Lock the SCOREP_DefineSourceFile() and related operations (in the adapters).
 */
void
SCOREP_LockSourceFileDefinition();

/**
 * Unlock the SCOREP_DefineSourceFile() and related operations (in the adapters).
 */
void
SCOREP_UnlockSourceFileDefinition();


/**
 * Lock the SCOREP_DefineRegion() and related operations (in the adapters).
 */
void
SCOREP_LockRegionDefinition();

/**
 * Unlock the SCOREP_DefineRegion() and related operations (in the adapters).
 */
void
SCOREP_UnlockRegionDefinition();


/**
 * Lock the SCOREP_DefineMPICommunicator() and related operations (in the adapters).
 */
void
SCOREP_LockMPICommunicatorDefinition();

/**
 * Unlock the SCOREP_DefineMPICommunicator() and related operations (in the adapters).
 */
void
SCOREP_UnlockMPICommunicatorDefinition();


/**
 * Lock the SCOREP_DefineMPIWindow() and related operations (in the adapters).
 */
void
SCOREP_LockMPIWindowDefinition();

/**
 * Unlock the SCOREP_DefineMPIWindow() and related operations (in the adapters).
 */
void
SCOREP_UnlockMPIWindowDefinition();


/**
 * Lock the SCOREP_DefineMPICartesianTopology() and related operations (in the adapters).
 */
void
SCOREP_LockMPICartesianTopologyDefinition();

/**
 * Unlock the SCOREP_DefineMPICartesianTopology() and related operations (in the adapters).
 */
void
SCOREP_UnlockMPICartesianTopologyDefinition();


/**
 * Lock the SCOREP_DefineMPICartesianCoords() and related operations (in the adapters).
 */
void
SCOREP_LockMPICartesianCoordsDefinition();

/**
 * Unlock the SCOREP_DefineMPICartesianCoords() and related operations (in the adapters).
 */
void
SCOREP_UnlockMPICartesianCoordsDefinition();


/**
 * Lock the SCOREP_DefineCounterGroup() and related operations (in the adapters).
 */
void
SCOREP_LockCounterGroupDefinition();

/**
 * Unlock the SCOREP_DefineCounterGroup() and related operations (in the adapters).
 */
void
SCOREP_UnlockCounterGroupDefinition();


/**
 * Lock the SCOREP_DefineCounter() and related operations (in the adapters).
 */
void
SCOREP_LockCounterDefinition();

/**
 * Unlock the SCOREP_DefineCounter() and related operations (in the adapters).
 */
void
SCOREP_UnlockCounterDefinition();


/**
 * Lock the SCOREP_DefineIOFileGroup() and related operations (in the adapters).
 */
void
SCOREP_LockIOFileGroupDefinition();

/**
 * Unlock the SCOREP_DefineIOFileGroup() and related operations (in the adapters).
 */
void
SCOREP_UnlockIOFileGroupDefinition();


/**
 * Lock the SCOREP_DefineIOFile() and related operations (in the adapters).
 */
void
SCOREP_LockIOFileDefinition();

/**
 * Unlock the SCOREP_DefineIOFile() and related operations (in the adapters).
 */
void
SCOREP_UnlockIOFileDefinition();


/**
 * Lock the SCOREP_DefineMarkerGroup() and related operations (in the adapters).
 */
void
SCOREP_LockMarkerGroupDefinition();

/**
 * Unlock the SCOREP_DefineMarkerGroup() and related operations (in the adapters).
 */
void
SCOREP_UnlockMarkerGroupDefinition();


/**
 * Lock the SCOREP_DefineMarker() and related operations (in the adapters).
 */
void
SCOREP_LockMarkerDefinition();

/**
 * Unlock the SCOREP_DefineMarker() and related operations (in the adapters).
 */
void
SCOREP_UnlockMarkerDefinition();


/**
 * Lock the SCOREP_DefineParameter() and related operations (in the adapters).
 */
void
SCOREP_LockParameterDefinition();

/**
 * Unlock the SCOREP_DefineParameter() and related operations (in the adapters).
 */
void
SCOREP_UnlockParameterDefinition();


#endif /* SCOREP_DEFINITIONLOCKING_H */
