/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2019,
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


#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>
#include <SCOREP_Memory.h>


static SCOREP_Paradigm*  head;
static SCOREP_Paradigm** tail = &head;


SCOREP_Paradigm*
SCOREP_Definitions_NewParadigm( SCOREP_ParadigmType  paradigm,
                                SCOREP_ParadigmClass paradigmClass,
                                const char*          name,
                                SCOREP_ParadigmFlags paradigmFlags )
{
    UTILS_ASSERT( name );
    UTILS_DEBUG_ENTRY( "%s, %s, %s, %x",
                       scorep_paradigm_type_to_string( paradigm ),
                       scorep_paradigm_class_to_string( paradigmClass ),
                       name,
                       paradigmFlags );

    UTILS_BUG_ON( paradigmClass >= SCOREP_INVALID_PARADIGM_CLASS,
                  "Invalid paradigm class: %u", paradigmClass );

    SCOREP_Definitions_Lock();

    SCOREP_AnyHandle new_handle =
        SCOREP_Memory_AllocForDefinitions( NULL, sizeof( SCOREP_Paradigm ) );
    SCOREP_Paradigm* new_paradigm =
        SCOREP_MEMORY_DEREF_LOCAL( new_handle, SCOREP_Paradigm* );
    memset( new_paradigm, 0, sizeof( *new_paradigm ) );
    new_paradigm->next = NULL;

    new_paradigm->name_handle = scorep_definitions_new_string(
        &scorep_local_definition_manager, name, NULL );
    /* Cache paradigm name */
    new_paradigm->name = SCOREP_StringHandle_Get( new_paradigm->name_handle );

    new_paradigm->paradigm_type  = paradigm;
    new_paradigm->paradigm_class = paradigmClass;
    new_paradigm->paradigm_flags = paradigmFlags;

    memset( new_paradigm->property_handles,
            0,
            sizeof( new_paradigm->property_handles ) );

    *tail = new_paradigm;
    tail  = &new_paradigm->next;

    SCOREP_Definitions_Unlock();

    SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                ( new_handle, SCOREP_HANDLE_TYPE_PARADIGM ) );

    return new_paradigm;
}


void
SCOREP_Definitions_ParadigmSetProperty( SCOREP_Paradigm*        paradigm,
                                        SCOREP_ParadigmProperty paradigmProperty,
                                        SCOREP_AnyHandle        propertyValue )
{
    UTILS_ASSERT( paradigm &&
                  paradigmProperty < SCOREP_INVALID_PARADIGM_PROPERTY &&
                  propertyValue != SCOREP_MOVABLE_NULL );

    UTILS_BUG_ON( paradigm->property_handles[ paradigmProperty ] != SCOREP_MOVABLE_NULL,
                  "Redeclaration of property %s for paradigm %s",
                  scorep_paradigm_property_to_string( paradigmProperty ),
                  paradigm->name );

    paradigm->property_handles[ paradigmProperty ] = propertyValue;
}


void
SCOREP_ForAllParadigms( void ( * cb )( SCOREP_Paradigm*,
                                       void* ),
                        void*    userData )
{
    for ( SCOREP_Paradigm* paradigm = head; paradigm; paradigm = paradigm->next )
    {
        cb( paradigm, userData );
    }
}
/**
 * Returns paradigm class (@see SCOREP_PublicTypes.h)
 * @param handle to paradigm
 * @return class (e.g. SCOREP_PARADIGM_CLASS_MPP for MPI regions)
 */
SCOREP_ParadigmClass
SCOREP_ParadigmHandle_GetClass( SCOREP_ParadigmHandle handle )
{
    SCOREP_Paradigm* paradigm = SCOREP_MEMORY_DEREF_LOCAL( handle, SCOREP_Paradigm* );
    return paradigm->paradigm_class;
}

/**
 * Returns paradigm name  (@see SCOREP_PublicTypes.h)
 * @param handle to paradigm
 * @return name (lowercase, e.g., mpi)
 */
const char*
SCOREP_ParadigmHandle_GetName( SCOREP_ParadigmHandle handle )
{
    SCOREP_Paradigm* paradigm = SCOREP_MEMORY_DEREF_LOCAL( handle, SCOREP_Paradigm* );
    return SCOREP_LOCAL_HANDLE_DEREF( paradigm->name_handle, String )->string_data;
}

/**
 * Returns paradigm type (@see SCOREP_PublicTypes.h)
 * @param handle to paradigm
 * @return type (e.g. SCOREP_PARADIGM_MPI for MPI regions)
 */
SCOREP_ParadigmType
SCOREP_ParadigmHandle_GetType( SCOREP_ParadigmHandle handle )
{
    SCOREP_Paradigm* paradigm = SCOREP_MEMORY_DEREF_LOCAL( handle, SCOREP_Paradigm* );
    return paradigm->paradigm_type;
}
