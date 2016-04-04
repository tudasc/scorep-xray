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
 * Copyright (c) 2009-2013,
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

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_PROPERTY_H
#define SCOREP_PRIVATE_DEFINITIONS_PROPERTY_H


/**
 * @file
 *
 *
 */


#define SCOREP_INVALID_PROPERTY SCOREP_MOVABLE_NULL


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
scorep_definitions_unify_property( SCOREP_PropertyDef*                  definition,
                                   struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_PROPERTY_H */
