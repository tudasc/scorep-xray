/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_SOURCE_CODE_LOCATION_H
#define SCOREP_PRIVATE_DEFINITIONS_SOURCE_CODE_LOCATION_H


/**
 * @file
 *
 *
 */

#include <stdarg.h>

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_DefinitionHandles.h>
#include <SCOREP_Memory.h>

#define SCOREP_INVALID_SOURCE_CODE_LOCATION SCOREP_MOVABLE_NULL

SCOREP_DEFINE_DEFINITION_TYPE( SourceCodeLocation )
{
    SCOREP_DEFINE_DEFINITION_HEADER( SourceCodeLocation );

    SCOREP_StringHandle file_handle;                // File of the source code location.
    SCOREP_LineNo       line_number;                // Line number of the source code location.
};

SCOREP_SourceCodeLocationHandle
SCOREP_Definitions_NewSourceCodeLocation( const char*   file,
                                          SCOREP_LineNo lineNumber );

void
scorep_definitions_unify_source_code_location( SCOREP_SourceCodeLocationDef* definition,
                                               SCOREP_Allocator_PageManager* handlesPageManager );

#endif /* SCOREP_PRIVATE_DEFINITIONS_SOURCE_CODE_LOCATION_H */
