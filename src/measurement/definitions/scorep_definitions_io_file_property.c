/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015, 2017, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <SCOREP_Definitions.h>
#include "scorep_definitions_private.h"


#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>


#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME DEFINITIONS
#include <UTILS_Debug.h>


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_IoFilePropertyHandle
add_io_file_property( SCOREP_DefinitionManager* definition_manager,
                      SCOREP_IoFileDef*         ioFile,
                      SCOREP_IoFileHandle       ioFileHandle,
                      SCOREP_StringHandle       propertyNameHandle,
                      SCOREP_StringHandle       propertyValueHandle );


static bool
equal_io_file_property( const SCOREP_IoFilePropertyDef* existingDefinition,
                        const SCOREP_IoFilePropertyDef* newDefinition );


void
SCOREP_IoFileHandle_AddProperty( SCOREP_IoFileHandle ioFileHandle,
                                 const char*         propertyName,
                                 const char*         propertyValue )
{
    UTILS_DEBUG_ENTRY( "%s=%s", propertyName, propertyValue );

    UTILS_ASSERT( ioFileHandle != SCOREP_INVALID_IO_FILE );
    UTILS_ASSERT( propertyName );
    UTILS_ASSERT( propertyValue );

    SCOREP_IoFileDef* io_file = SCOREP_LOCAL_HANDLE_DEREF(
        ioFileHandle,
        IoFile );

    SCOREP_Definitions_Lock();

    SCOREP_IoFilePropertyHandle new_handle =
        add_io_file_property(
            &scorep_local_definition_manager,
            io_file,
            ioFileHandle,
            scorep_definitions_new_string(
                &scorep_local_definition_manager,
                propertyName, NULL ),
            scorep_definitions_new_string(
                &scorep_local_definition_manager,
                propertyValue, NULL ) );

    SCOREP_Definitions_Unlock();

    SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                ( new_handle, SCOREP_HANDLE_TYPE_IO_FILE_PROPERTY ) );
}


void
scorep_definitions_unify_io_file_property( SCOREP_IoFilePropertyDef*     definition,
                                           SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_IoFileHandle unified_io_file_handle = SCOREP_HANDLE_GET_UNIFIED(
        definition->io_file_handle,
        IoFile,
        handlesPageManager );

    SCOREP_IoFileDef* unified_io_file = SCOREP_HANDLE_DEREF(
        unified_io_file_handle,
        IoFile,
        scorep_unified_definition_manager->page_manager );

    SCOREP_IoFilePropertyHandle unified =
        add_io_file_property(
            scorep_unified_definition_manager,
            unified_io_file,
            unified_io_file_handle,
            SCOREP_HANDLE_GET_UNIFIED(
                definition->property_name_handle,
                String,
                handlesPageManager ),
            SCOREP_HANDLE_GET_UNIFIED(
                definition->property_value_handle,
                String,
                handlesPageManager ) );
}


bool
equal_io_file_property( const SCOREP_IoFilePropertyDef* existingDefinition,
                        const SCOREP_IoFilePropertyDef* newDefinition )
{
    return existingDefinition->io_file_handle == newDefinition->io_file_handle &&
           existingDefinition->property_name_handle    == newDefinition->property_name_handle    &&
           existingDefinition->property_value_handle   == newDefinition->property_value_handle;
}


SCOREP_IoFilePropertyHandle
add_io_file_property( SCOREP_DefinitionManager* definition_manager,
                      SCOREP_IoFileDef*         ioFile,
                      SCOREP_IoFileHandle       ioFileHandle,
                      SCOREP_StringHandle       propertyNameHandle,
                      SCOREP_StringHandle       propertyValueHandle )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_IoFilePropertyDef*   new_definition = NULL;
    SCOREP_IoFilePropertyHandle new_handle     = SCOREP_INVALID_IO_FILE_PROPERTY;

    SCOREP_DEFINITION_ALLOC( IoFileProperty );

    new_definition->io_file_handle = ioFileHandle;
    HASH_ADD_HANDLE( new_definition, io_file_handle, IoFile );

    new_definition->property_name_handle = propertyNameHandle;
    HASH_ADD_HANDLE( new_definition, property_name_handle, String );

    new_definition->property_value_handle = propertyValueHandle;
    HASH_ADD_HANDLE( new_definition, property_value_handle, String );

    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( IoFileProperty,
                                               io_file_property );

    /* Chain this the properties of the I/O file. */
    new_definition->properties_next = SCOREP_INVALID_IO_FILE_PROPERTY;
    *ioFile->properties_tail        = new_handle;
    ioFile->properties_tail         = &new_definition->properties_next;

    return new_handle;
}
