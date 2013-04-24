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

#ifndef SCOREP_PRIVATE_DEFINITIONS_STRING_H
#define SCOREP_PRIVATE_DEFINITIONS_STRING_H


/**
 * @file       src/measurement/definitions/scorep_definitions_string.h
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>


SCOREP_DEFINE_DEFINITION_TYPE( String )
{
    SCOREP_DEFINE_DEFINITION_HEADER( String );

    uint32_t string_length;
    // variable array member
    char string_data[];
};


SCOREP_StringHandle
SCOREP_Definitions_NewString( const char* str );


void
SCOREP_CopyStringDefinitionToUnified( SCOREP_StringDef*             definition,
                                      SCOREP_Allocator_PageManager* handlesPageManager );


SCOREP_StringHandle
scorep_definitions_new_string( SCOREP_DefinitionManager* definition_manager,
                               const char*               str );


#endif /* SCOREP_PRIVATE_DEFINITIONS_STRING_H */
