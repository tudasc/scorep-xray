/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
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

#ifndef SCOREP_PRIVATE_DEFINITIONS_IO_FILE_H
#define SCOREP_PRIVATE_DEFINITIONS_IO_FILE_H


/**
 * @file
 *
 *
 */



SCOREP_DEFINE_DEFINITION_TYPE( IoFile )
{
    SCOREP_DEFINE_DEFINITION_HEADER( IoFile );

    // Add SCOREP_IoFile stuff from here on.
    SCOREP_StringHandle         file_name_handle;   /**< String handle of the file name. */
    SCOREP_SystemTreeNodeHandle scope;              /**< Handle marking the scope of this file. */

    /* Chain of all properties for this file. */
    SCOREP_IoFilePropertyHandle  properties;
    SCOREP_IoFilePropertyHandle* properties_tail;
};

const char*
SCOREP_Definitions_GetIoFileName( SCOREP_IoFileHandle handle );

/**
 * Associate a name/path, file system, and a paradigm with a process unique handle of an I/O file.
 *
 * @param name          A meaningful name for the I/O file.
 * @param scope         Scope of the file (e.g., shared network file system vs node local storage).
 *
 * @return A process unique handle of an I/O file to be used in I/O recording events.
 *
 */
SCOREP_IoFileHandle
SCOREP_Definitions_NewIoFile( const char*                 name,
                              SCOREP_SystemTreeNodeHandle scope );

void
scorep_definitions_unify_io_file( SCOREP_IoFileDef*                    definition,
                                  struct SCOREP_Allocator_PageManager* handlesPageManager );


#endif /* SCOREP_PRIVATE_DEFINITIONS_IO_FILE_H */
