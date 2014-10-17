/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_UNIFY_H
#define SCOREP_INTERNAL_UNIFY_H



/**
 * @file
 *
 *
 */


#include <SCOREP_Definitions.h>

void
SCOREP_Unify( void );


void
SCOREP_Unify_CreateUnifiedDefinitionManager( void );


void
SCOREP_Unify_Locally( void );


void
SCOREP_Unify_Mpp( void );


void
SCOREP_CopyDefinitionsToUnified( SCOREP_DefinitionManager* sourceDefinitionManager );


void
SCOREP_CreateDefinitionMappings( SCOREP_DefinitionManager* definitionManager );


void
SCOREP_AssignDefinitionMappingsFromUnified( SCOREP_DefinitionManager* definitionManager );


void
SCOREP_DestroyDefinitionMappings( SCOREP_DefinitionManager* definitionManager );


#endif /* SCOREP_INTERNAL_UNIFY_H */
