/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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

#ifndef SCOREP_UNIFY_H_
#define SCOREP_UNIFY_H_



/**
 * @file       scorep_unify.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include "scorep_definitions.h"

void
SCOREP_Unify( void );


void
SCOREP_Unify_Locally( void );


void
SCOREP_Mpi_Unify( void );


void
SCOREP_CopyDefinitionsToUnified( SCOREP_DefinitionManager* sourceDefinitionManager );


void
SCOREP_CreateDefinitionMappings( SCOREP_DefinitionManager* definitionManager );


void
SCOREP_AssignDefinitionMappingsFromUnified( SCOREP_DefinitionManager* definitionManager );


void
SCOREP_DestroyDefinitionMappings( SCOREP_DefinitionManager* definitionManager );


#endif /* SCOREP_UNIFY_H_ */
