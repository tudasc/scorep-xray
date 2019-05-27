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


static SCOREP_RegionHandle
define_region( SCOREP_DefinitionManager* definition_manager,
               SCOREP_StringHandle       regionNameHandle,
               SCOREP_StringHandle       regionCanonicalNameHandle,
               SCOREP_StringHandle       descriptionNameHandle,
               SCOREP_StringHandle       fileNameHandle,
               SCOREP_LineNo             beginLine,
               SCOREP_LineNo             endLine,
               SCOREP_ParadigmType       paradigm,
               SCOREP_RegionType         regionType,
               SCOREP_StringHandle       groupNameHandle );


static void
initialize_region( SCOREP_RegionDef*         definition,
                   SCOREP_DefinitionManager* definition_manager,
                   SCOREP_StringHandle       regionNameHandle,
                   SCOREP_StringHandle       regionCanonicalNameHandle,
                   SCOREP_StringHandle       descriptionNameHandle,
                   SCOREP_StringHandle       fileNameHandle,
                   SCOREP_LineNo             beginLine,
                   SCOREP_LineNo             endLine,
                   SCOREP_ParadigmType       paradigm,
                   SCOREP_RegionType         regionType,
                   SCOREP_StringHandle       groupNameHandle );


static bool
equal_region( const SCOREP_RegionDef* existingDefinition,
              const SCOREP_RegionDef* newDefinition );


SCOREP_RegionHandle
SCOREP_Definitions_NewRegion( const char*             regionName,
                              const char*             regionCanonicalName,
                              SCOREP_SourceFileHandle fileHandle,
                              SCOREP_LineNo           beginLine,
                              SCOREP_LineNo           endLine,
                              SCOREP_ParadigmType     paradigm,
                              SCOREP_RegionType       regionType )
{
    UTILS_DEBUG_ENTRY( "%s", regionName );

    /* resolve the file name early */
    SCOREP_StringHandle file_name_handle = SCOREP_INVALID_STRING;
    if ( fileHandle != SCOREP_INVALID_SOURCE_FILE )
    {
        file_name_handle = SCOREP_LOCAL_HANDLE_DEREF( fileHandle, SourceFile )->name_handle;
    }

    SCOREP_Definitions_Lock();

    SCOREP_RegionHandle new_handle = define_region(
        &scorep_local_definition_manager,
        /* region name (use it for demangled name) */
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            regionName ? regionName : "<unknown region>", NULL ),
        /* canonical region name (use it for mangled name) */
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            regionCanonicalName ? regionCanonicalName :
            regionName ? regionName : "<unknown region>", NULL ),
        /* description currently not used */
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            "", NULL ),
        file_name_handle,
        beginLine,
        endLine,
        paradigm,
        regionType,
        SCOREP_INVALID_STRING );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_RegionHandle_SetGroup( SCOREP_RegionHandle handle,
                              const char*         groupName )
{
    SCOREP_Definitions_Lock();

    SCOREP_RegionDef* region = SCOREP_LOCAL_HANDLE_DEREF( handle, Region );

    region->group_name_handle = scorep_definitions_new_string(
        &scorep_local_definition_manager,
        groupName,
        NULL );

    SCOREP_Definitions_Unlock();
}

void
scorep_definitions_unify_region( SCOREP_RegionDef*             definition,
                                 SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_StringHandle unified_file_name_handle = SCOREP_INVALID_STRING;
    if ( definition->file_name_handle != SCOREP_INVALID_STRING )
    {
        unified_file_name_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->file_name_handle,
            String,
            handlesPageManager );
        UTILS_BUG_ON( unified_file_name_handle == SCOREP_INVALID_STRING,
                      "Invalid unification order of region definition: file name not yet unified" );
    }

    SCOREP_StringHandle unified_group_name_handle = SCOREP_INVALID_STRING;
    if ( definition->group_name_handle != SCOREP_INVALID_STRING )
    {
        unified_group_name_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->group_name_handle,
            String,
            handlesPageManager );
        UTILS_BUG_ON( unified_group_name_handle == SCOREP_INVALID_STRING,
                      "Invalid unification order of region definition: group name not yet unified" );
    }

    definition->unified = define_region(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED(
            definition->canonical_name_handle,
            String,
            handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED(
            definition->description_handle,
            String,
            handlesPageManager ),
        unified_file_name_handle,
        definition->begin_line,
        definition->end_line,
        definition->paradigm_type,
        definition->region_type,
        unified_group_name_handle );
}


SCOREP_RegionHandle
define_region( SCOREP_DefinitionManager* definition_manager,
               SCOREP_StringHandle       regionNameHandle,
               SCOREP_StringHandle       regionCanonicalNameHandle,
               SCOREP_StringHandle       descriptionNameHandle,
               SCOREP_StringHandle       fileNameHandle,
               SCOREP_LineNo             beginLine,
               SCOREP_LineNo             endLine,
               SCOREP_ParadigmType       paradigm,
               SCOREP_RegionType         regionType,
               SCOREP_StringHandle       groupNameHandle )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_RegionDef*   new_definition = NULL;
    SCOREP_RegionHandle new_handle     = SCOREP_INVALID_REGION;

    SCOREP_DEFINITION_ALLOC( Region );
    initialize_region( new_definition,
                       definition_manager,
                       regionNameHandle,
                       regionCanonicalNameHandle,
                       descriptionNameHandle,
                       fileNameHandle,
                       beginLine,
                       endLine,
                       paradigm,
                       regionType,
                       groupNameHandle );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( Region, region );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_REGION ) );
    }

    return new_handle;
}


void
initialize_region( SCOREP_RegionDef*         definition,
                   SCOREP_DefinitionManager* definition_manager,
                   SCOREP_StringHandle       regionNameHandle,
                   SCOREP_StringHandle       regionCanonicalNameHandle,
                   SCOREP_StringHandle       descriptionNameHandle,
                   SCOREP_StringHandle       fileNameHandle,
                   SCOREP_LineNo             beginLine,
                   SCOREP_LineNo             endLine,
                   SCOREP_ParadigmType       paradigm,
                   SCOREP_RegionType         regionType,
                   SCOREP_StringHandle       groupNameHandle )
{
    definition->name_handle = regionNameHandle;
    HASH_ADD_HANDLE( definition, name_handle, String );

    definition->canonical_name_handle = regionCanonicalNameHandle;
    HASH_ADD_HANDLE( definition, canonical_name_handle, String );

    definition->description_handle = descriptionNameHandle;
    HASH_ADD_HANDLE( definition, description_handle, String );

    definition->region_type = regionType;
    HASH_ADD_POD( definition, region_type );

    definition->file_name_handle = fileNameHandle;
    if ( fileNameHandle != SCOREP_INVALID_STRING )
    {
        HASH_ADD_HANDLE( definition, file_name_handle, String );
    }

    definition->begin_line = beginLine;
    HASH_ADD_POD( definition, begin_line );
    definition->end_line = endLine;
    HASH_ADD_POD( definition, end_line );
    definition->paradigm_type = paradigm;
    HASH_ADD_POD( definition, paradigm_type );

    /* Not a primary key, might be changed later */
    definition->group_name_handle = groupNameHandle;
}


bool
equal_region( const SCOREP_RegionDef* existingDefinition,
              const SCOREP_RegionDef* newDefinition )
{
    return existingDefinition->name_handle           == newDefinition->name_handle &&
           existingDefinition->canonical_name_handle == newDefinition->canonical_name_handle &&
           existingDefinition->description_handle    == newDefinition->description_handle &&
           existingDefinition->region_type           == newDefinition->region_type &&
           existingDefinition->file_name_handle      == newDefinition->file_name_handle &&
           existingDefinition->begin_line            == newDefinition->begin_line &&
           existingDefinition->end_line              == newDefinition->end_line &&
           existingDefinition->paradigm_type          == newDefinition->paradigm_type;
}


uint32_t
SCOREP_RegionHandle_GetId( SCOREP_RegionHandle handle )
{
    return SCOREP_LOCAL_HANDLE_TO_ID( handle, Region );
}


/**
 * Gets read-only access to the name of the region.
 *
 * @param handle A handle to the region.
 *
 * @return region name.
 */
const char*
SCOREP_RegionHandle_GetName( SCOREP_RegionHandle handle )
{
    SCOREP_RegionDef* region = SCOREP_LOCAL_HANDLE_DEREF( handle, Region );

    return SCOREP_LOCAL_HANDLE_DEREF( region->name_handle, String )->string_data;
}


/**
 * Gets read-only access to the canonical name of the region.
 *
 * @param handle A handle to the region.
 *
 * @return region name.
 */
const char*
SCOREP_RegionHandle_GetCanonicalName( SCOREP_RegionHandle handle )
{
    SCOREP_RegionDef* region = SCOREP_LOCAL_HANDLE_DEREF( handle, Region );

    return SCOREP_LOCAL_HANDLE_DEREF( region->canonical_name_handle, String )->string_data;
}


/**
 * Gets read-only access to the file name of the region.
 *
 * @param handle A handle to the region.
 *
 * @return region file name.
 */
const char*
SCOREP_RegionHandle_GetFileName( SCOREP_RegionHandle handle )
{
    SCOREP_RegionDef* region = SCOREP_LOCAL_HANDLE_DEREF( handle, Region );
    if ( region->file_name_handle == SCOREP_INVALID_STRING )
    {
        return NULL;
    }
    return SCOREP_LOCAL_HANDLE_DEREF( region->file_name_handle, String )->string_data;
}


/**
 * Gets the type of the region.
 *
 * @param handle A handle to the region.
 *
 * @return region type.
 */
SCOREP_RegionType
SCOREP_RegionHandle_GetType( SCOREP_RegionHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Region )->region_type;
}


/**
 * Gets the paradigm type of the region.
 *
 * @param handle A handle to the region.
 *
 * @return regions paradigm type.
 */
SCOREP_ParadigmType
SCOREP_RegionHandle_GetParadigmType( SCOREP_RegionHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Region )->paradigm_type;
}


/**
 * Gets the region first line of the region
 *
 * @param handle A handle to the region.
 *
 * @return regions first line.
 */
SCOREP_LineNo
SCOREP_RegionHandle_GetBeginLine( SCOREP_RegionHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Region )->begin_line;
}


/**
 * Gets the region end line of the region.
 *
 * @param handle A handle to the region.
 *
 * @return regions end line.
 */
SCOREP_LineNo
SCOREP_RegionHandle_GetEndLine( SCOREP_RegionHandle handle )
{
    return SCOREP_LOCAL_HANDLE_DEREF( handle, Region )->end_line;
}
