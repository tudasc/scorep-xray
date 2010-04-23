#ifndef SILC_DEFINITIONLOCKING_H
#define SILC_DEFINITIONLOCKING_H

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
 * @file       SILC_DefinitionLocking.h
 *             Declarations of lock and unlock functions to be used when
 *             calling corresponding SILC_DefineFoo functions. The locking and
 *             unlocking is the responsibility of the adapter, as it anyhow
 *             needs to synchronize the access to it's internal data
 *             structures when defining new entities.
 *
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 */

/**
   @def SILC_LOCK( x ) A macro which inserts a lock if OpenMP is enabled, else it
   disappears. It can be used to avoid ifdef cluttering in code which uses locks.
   Nevertheless, the code needs to be compiled twice, one with version with locks and
   one version without locks.
   Usage example:
   @code
   SILC_LOCK(Region);
   // do critical stuff here
   SILC_UNLOCK(Region);
   @endcode
 */
#ifdef _OPENMP
#define SILC_LOCK( lockname ) SILC_Lock##lockname##Definition()
#else
#define SILC_LOCK( lockname )
#endif /* _OPENMP */

/**
   @def SILC_UNLOCK( x ) A macro which inserts a unlock if OpenMP is enabled, else it
   disappears. It can be used to avoid ifdef cluttering in code which uses locks.
   Nevertheless, the code needs to be compiled twice, one with version with locks and
   one version without locks.
   Usage example:
   @code
   SILC_LOCK(Region);
   // do critical stuff here
   SILC_UNLOCK(Region);
   @endcode
 */
#ifdef _OPENMP
#define SILC_UNLOCK( lockname )  SILC_Unlock##lockname##Definition()
#else
#define SILC_UNLOCK( lockname )
#endif /* _OPENMP */

/**
 * Lock the SILC_DefineSourceFile() and related operations (in the adapters).
 */
void
SILC_LockSourceFileDefinition();

/**
 * Unlock the SILC_DefineSourceFile() and related operations (in the adapters).
 */
void
SILC_UnlockSourceFileDefinition();


/**
 * Lock the SILC_DefineRegion() and related operations (in the adapters).
 */
void
SILC_LockRegionDefinition();

/**
 * Unlock the SILC_DefineRegion() and related operations (in the adapters).
 */
void
SILC_UnlockRegionDefinition();


/**
 * Lock the SILC_DefineMPICommunicator() and related operations (in the adapters).
 */
void
SILC_LockMPICommunicatorDefinition();

/**
 * Unlock the SILC_DefineMPICommunicator() and related operations (in the adapters).
 */
void
SILC_UnlockMPICommunicatorDefinition();


/**
 * Lock the SILC_DefineMPIWindow() and related operations (in the adapters).
 */
void
SILC_LockMPIWindowDefinition();

/**
 * Unlock the SILC_DefineMPIWindow() and related operations (in the adapters).
 */
void
SILC_UnlockMPIWindowDefinition();


/**
 * Lock the SILC_DefineMPICartesianTopology() and related operations (in the adapters).
 */
void
SILC_LockMPICartesianTopologyDefinition();

/**
 * Unlock the SILC_DefineMPICartesianTopology() and related operations (in the adapters).
 */
void
SILC_UnlockMPICartesianTopologyDefinition();


/**
 * Lock the SILC_DefineMPICartesianCoords() and related operations (in the adapters).
 */
void
SILC_LockMPICartesianCoordsDefinition();

/**
 * Unlock the SILC_DefineMPICartesianCoords() and related operations (in the adapters).
 */
void
SILC_UnlockMPICartesianCoordsDefinition();


/**
 * Lock the SILC_DefineCounterGroup() and related operations (in the adapters).
 */
void
SILC_LockCounterGroupDefinition();

/**
 * Unlock the SILC_DefineCounterGroup() and related operations (in the adapters).
 */
void
SILC_UnlockCounterGroupDefinition();


/**
 * Lock the SILC_DefineCounter() and related operations (in the adapters).
 */
void
SILC_LockCounterDefinition();

/**
 * Unlock the SILC_DefineCounter() and related operations (in the adapters).
 */
void
SILC_UnlockCounterDefinition();


/**
 * Lock the SILC_DefineIOFileGroup() and related operations (in the adapters).
 */
void
SILC_LockIOFileGroupDefinition();

/**
 * Unlock the SILC_DefineIOFileGroup() and related operations (in the adapters).
 */
void
SILC_UnlockIOFileGroupDefinition();


/**
 * Lock the SILC_DefineIOFile() and related operations (in the adapters).
 */
void
SILC_LockIOFileDefinition();

/**
 * Unlock the SILC_DefineIOFile() and related operations (in the adapters).
 */
void
SILC_UnlockIOFileDefinition();


/**
 * Lock the SILC_DefineMarkerGroup() and related operations (in the adapters).
 */
void
SILC_LockMarkerGroupDefinition();

/**
 * Unlock the SILC_DefineMarkerGroup() and related operations (in the adapters).
 */
void
SILC_UnlockMarkerGroupDefinition();


/**
 * Lock the SILC_DefineMarker() and related operations (in the adapters).
 */
void
SILC_LockMarkerDefinition();

/**
 * Unlock the SILC_DefineMarker() and related operations (in the adapters).
 */
void
SILC_UnlockMarkerDefinition();


/**
 * Lock the SILC_DefineParameter() and related operations (in the adapters).
 */
void
SILC_LockParameterDefinition();

/**
 * Unlock the SILC_DefineParameter() and related operations (in the adapters).
 */
void
SILC_UnlockParameterDefinition();


#endif /* SILC_DEFINITIONLOCKING_H */
