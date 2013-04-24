/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_PROPERTY_H
#define SCOREP_PRIVATE_DEFINITIONS_PROPERTY_H


/**
 * @file       src/measurement/definitions/scorep_definitions_property.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( Property )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Property );

    SCOREP_Property          property;
    SCOREP_PropertyCondition condition;
    bool                     initialValue;
    bool                     invalidated;
};


/**
 * Define a property with its initial value.
 */
SCOREP_PropertyHandle
SCOREP_Definitions_NewProperty( SCOREP_Property          property,
                                SCOREP_PropertyCondition condition,
                                bool                     initialValue );


void
SCOREP_CopyPropertyDefinitionToUnified( SCOREP_PropertyDef*           definition,
                                        SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_PROPERTY_H */
