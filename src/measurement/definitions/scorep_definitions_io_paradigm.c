/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 */


#include <config.h>


#include <SCOREP_Definitions.h>
#include "scorep_definitions_private.h"


#include <string.h>


#define SCOREP_DEBUG_MODULE_NAME DEFINITIONS
#include <UTILS_Debug.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>
#include <SCOREP_Memory.h>


static SCOREP_IoParadigm*  head;
static SCOREP_IoParadigm** tail = &head;
static uint32_t            counter;


SCOREP_IoParadigm*
SCOREP_Definitions_NewIoParadigm( SCOREP_IoParadigmType  paradigm,
                                  const char*            identification,
                                  const char*            name,
                                  SCOREP_IoParadigmClass paradigmClass,
                                  SCOREP_IoParadigmFlag  paradigmFlags )
{
    UTILS_ASSERT( name );
    UTILS_ASSERT( identification );

    SCOREP_Definitions_Lock();

    SCOREP_AnyHandle new_handle =
        SCOREP_Memory_AllocForDefinitions( NULL, sizeof( SCOREP_IoParadigm ) );
    SCOREP_IoParadigm* new_paradigm =
        SCOREP_MEMORY_DEREF_LOCAL( new_handle, SCOREP_IoParadigm* );
    memset( new_paradigm, 0, sizeof( *new_paradigm ) );
    new_paradigm->sequence_number = counter++;
    new_paradigm->next            = NULL;

    new_paradigm->paradigm_type         = paradigm;
    new_paradigm->identification_handle = scorep_definitions_new_string(
        &scorep_local_definition_manager, identification, NULL );
    new_paradigm->name_handle = scorep_definitions_new_string(
        &scorep_local_definition_manager, name, NULL );
    new_paradigm->paradigm_class = paradigmClass;
    new_paradigm->paradigm_flags = paradigmFlags;

    /* Cache paradigm name */
    new_paradigm->name = SCOREP_StringHandle_Get( new_paradigm->name_handle );

    *tail = new_paradigm;
    tail  = &new_paradigm->next;

    SCOREP_Definitions_Unlock();

    SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                ( new_handle, SCOREP_HANDLE_TYPE_IO_PARADIGM ) );

    return new_paradigm;
}


void
SCOREP_Definitions_IoParadigmSetProperty( SCOREP_IoParadigm*        paradigm,
                                          SCOREP_IoParadigmProperty paradigmProperty,
                                          SCOREP_AnyHandle          propertyValue )
{
    UTILS_ASSERT( paradigm &&
                  paradigmProperty < SCOREP_INVALID_IO_PARADIGM_PROPERTY &&
                  propertyValue != SCOREP_MOVABLE_NULL );

    UTILS_BUG_ON( paradigm->property_handles[ paradigmProperty ] != SCOREP_MOVABLE_NULL,
                  "Redeclaration of property %s for I/O paradigm %s",
                  scorep_io_paradigm_property_to_string( paradigmProperty ),
                  paradigm->name );

    paradigm->property_handles[ paradigmProperty ] = propertyValue;
}


void
SCOREP_ForAllIoParadigms( void ( * cb )( SCOREP_IoParadigm*,
                                         void* ),
                          void*    userData )
{
    for ( SCOREP_IoParadigm* paradigm = head; paradigm; paradigm = paradigm->next )
    {
        cb( paradigm, userData );
    }
}
