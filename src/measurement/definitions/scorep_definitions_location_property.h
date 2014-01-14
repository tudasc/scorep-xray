/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_LOCATION_PROPERTY_H
#define SCOREP_PRIVATE_DEFINITIONS_LOCATION_PROPERTY_H


/**
 * @file
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>

SCOREP_DEFINE_DEFINITION_TYPE( LocationProperty )
{
    SCOREP_DEFINE_DEFINITION_HEADER( LocationProperty );

    /* don't use the sequence number for the id, this is generated */
    SCOREP_StringHandle   name_handle;
    SCOREP_StringHandle   value_handle;
    SCOREP_LocationHandle location_handle;
};

SCOREP_LocationHandle
SCOREP_Definitions_NewLocationProperty( SCOREP_Location* location,
                                        const char*      name,
                                        const char*      vale );

void
scorep_definitions_unify_location_property( SCOREP_LocationPropertyDef*   definition,
                                            SCOREP_Allocator_PageManager* handlesPageManager );

#endif /* SCOREP_PRIVATE_DEFINITIONS_LOCATION_PROPERTY_H */