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

#ifndef SCOREP_INTERNAL_DEFINITION_LOCKING_H
#define SCOREP_INTERNAL_DEFINITION_LOCKING_H



/**
 * @file       scorep_definition_locking.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


void
SCOREP_DefinitionLocks_Initialize();


void
SCOREP_DefinitionLocks_Finalize();


/**
 * Lock the SCOREP_DefineLocation() and related operations (in the measurement core).
 */
void
SCOREP_LockLocationDefinition();


/**
 * Unlock the SCOREP_DefineLocation() and related operations (in the measurement core).
 */
void
SCOREP_UnlockLocationDefinition();


#endif /* SCOREP_INTERNAL_DEFINITION_LOCKING_H */
