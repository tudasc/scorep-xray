#ifndef SILC_INTERNAL_DEFINITIONS_H
#define SILC_INTERNAL_DEFINITIONS_H

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
 * @file       silc_definitions.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <SILC_DefinitionHandles.h>
#include <SILC_PublicTypes.h>

#include <SILC_Memory.h>


#define SILC_ALLOC_NEW_DEFINITION_OLD( DefinitionType )                                 \
    new_definition = SILC_MEMORY_DEREF_MOVABLE(                                     \
        SILC_Memory_AllocForDefinitions( sizeof( DefinitionType ) ),                \
        DefinitionType* );


#define SILC_DEFINITIONS_LIST_PUSH_FRONT_OLD( ListHeadDummy ) \
    new_definition->next = ListHeadDummy.next;            \
    ListHeadDummy.next   = new_definition;                \
    new_definition->id   = counter;


void
SILC_Definitions_Initialize();

void
SILC_Definitions_Finalize();

SILC_StringHandle
SILC_DefineString( const char* str );

SILC_CallpathHandle
SILC_DefineCallpath( SILC_CallpathHandle parent,
                     SILC_RegionHandle   region );

SILC_CallpathParameterIntegerHandle
SILC_DefineCallpathParameterInteger( SILC_CallpathHandle  parent,
                                     SILC_ParameterHandle param,
                                     int64_t              value );

SILC_CallpathParameterStringHandle
SILC_DefineCallpathParameterString( SILC_CallpathHandle  parent,
                                    SILC_ParameterHandle param,
                                    SILC_StringHandle    value );



#endif /* SILC_INTERNAL_DEFINITIONS_H */
