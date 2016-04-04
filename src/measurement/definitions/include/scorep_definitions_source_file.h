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

#ifndef SCOREP_PRIVATE_DEFINITIONS_SOURCE_FILE_H
#define SCOREP_PRIVATE_DEFINITIONS_SOURCE_FILE_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( SourceFile )
{
    SCOREP_DEFINE_DEFINITION_HEADER( SourceFile );

    SCOREP_StringHandle name_handle;
};


/**
 * Associate a file name with a process unique file handle.
 *
 * @param fileName A meaningful name for the source file.
 *
 * @return A process unique file handle to be used in calls to
 * SCOREP_Definitions_NewRegion().
 *
 */
SCOREP_SourceFileHandle
SCOREP_Definitions_NewSourceFile( const char* fileName );


const char*
SCOREP_SourceFileHandle_GetName( SCOREP_SourceFileHandle handle );


void
scorep_definitions_unify_source_file( SCOREP_SourceFileDef*                definition,
                                      struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_SOURCE_FILE_H */
