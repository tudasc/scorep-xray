/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 *    Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 *    University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 *    Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_ATTRIBUTE_H
#define SCOREP_PRIVATE_DEFINITIONS_ATTRIBUTE_H


/**
 * @file
 *
 *
 */

#define SCOREP_INVALID_ATTRIBUTE SCOREP_MOVABLE_NULL

SCOREP_DEFINE_DEFINITION_TYPE( Attribute )
{
    SCOREP_DEFINE_DEFINITION_HEADER( Attribute );

    SCOREP_StringHandle  name_handle;               // Name for this attribute.
    SCOREP_StringHandle  description_handle;        // Description for this attribute.
    SCOREP_AttributeType type;                      // Type for all values of this attribute.
};


/**
 * Add a new attribute definition.
 *
 * @param name The string naming the attribute.
 *
 * @param name The string describing the attribute.
 *
 * @param type The type of this attribute.
 *
 * @return A process unique handle to a new attribute.
 */
SCOREP_AttributeHandle
SCOREP_Definitions_NewAttribute( const char*          name,
                                 const char*          description,
                                 SCOREP_AttributeType type );

uint32_t
SCOREP_AttributeHandle_GetId( SCOREP_AttributeHandle handle );


SCOREP_AttributeType
SCOREP_AttributeHandle_GetType( SCOREP_AttributeHandle handle );

void
scorep_definitions_unify_attribute( SCOREP_AttributeDef*                 definition,
                                    struct SCOREP_Allocator_PageManager* handlesPageManager );

#endif /* SCOREP_PRIVATE_DEFINITIONS_ATTRIBUTE_H */
