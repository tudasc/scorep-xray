/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
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

#ifndef SCOREP_PRIVATE_DEFINITIONS_STRING_H
#define SCOREP_PRIVATE_DEFINITIONS_STRING_H


/**
 * @file
 *
 *
 */


SCOREP_DEFINE_DEFINITION_TYPE( String )
{
    SCOREP_DEFINE_DEFINITION_HEADER( String );

    uint32_t string_length;
    // variable array member
    char string_data[];
};


SCOREP_StringHandle
SCOREP_Definitions_NewString( const char* str );

const char*
SCOREP_StringHandle_Get( SCOREP_StringHandle handle );

const char*
SCOREP_StringHandle_GetById( uint32_t id );

void
scorep_definitions_unify_string( SCOREP_StringDef*                    definition,
                                 struct SCOREP_Allocator_PageManager* handlesPageManager );


typedef void ( * scorep_string_definition_modifier )( char* str );

SCOREP_StringHandle
scorep_definitions_new_string( SCOREP_DefinitionManager*         definition_manager,
                               const char*                       str,
                               scorep_string_definition_modifier modifier );


#endif /* SCOREP_PRIVATE_DEFINITIONS_STRING_H */
