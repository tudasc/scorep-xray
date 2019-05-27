/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017, 2019,
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


static SCOREP_GroupHandle
define_group( SCOREP_DefinitionManager* definition_manager,
              SCOREP_GroupType          groupType,
              uint64_t                  numberOfMembers,
              const uint64_t*           members,
              SCOREP_StringHandle       groupNameHandle,
              bool                      convertFromUint32 );

static bool
equal_group( const SCOREP_GroupDef* existingDefinition,
             const SCOREP_GroupDef* newDefinition );


/**
 * Associate a MPI group with a process unique group handle.
 */
SCOREP_GroupHandle
SCOREP_Definitions_NewGroup( SCOREP_GroupType type,
                             const char*      name,
                             uint32_t         numberOfMembers,
                             const uint64_t*  members )
{
    SCOREP_Definitions_Lock();

    SCOREP_GroupHandle new_handle = define_group(
        &scorep_local_definition_manager,
        type,
        numberOfMembers,
        members,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "", NULL ),
        false /* no need to converted from uint32_t */ );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/**
 * Associate a MPI group with a process unique group handle.
 */
SCOREP_GroupHandle
SCOREP_Definitions_NewGroupFrom32( SCOREP_GroupType type,
                                   const char*      name,
                                   uint32_t         numberOfMembers,
                                   const uint32_t*  members )
{
    SCOREP_Definitions_Lock();

    SCOREP_GroupHandle new_handle = define_group(
        &scorep_local_definition_manager,
        type,
        numberOfMembers,
        ( const uint64_t* )members,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "", NULL ),
        true /* need to be converted from uint32_t */ );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/**
 * Define a group in the unified definitions should only be called by the root
 * rank.
 */
SCOREP_GroupHandle
SCOREP_Definitions_NewUnifiedGroup( SCOREP_GroupType type,
                                    const char*      name,
                                    uint32_t         numberOfMembers,
                                    const uint64_t*  members )
{
    UTILS_ASSERT( scorep_unified_definition_manager );

    return define_group(
        scorep_unified_definition_manager,
        type,
        numberOfMembers,
        ( const uint64_t* )members,
        scorep_definitions_new_string(
            scorep_unified_definition_manager,
            name, NULL ),
        false /* no need to converted from uint32_t */ );
}

/**
 * Associate a MPI group with a process unique group handle.
 * Used to add groups from the communicator unifiaction after
 * group unification was done.
 */
SCOREP_GroupHandle
SCOREP_Definitions_NewUnifiedGroupFrom32( SCOREP_GroupType type,
                                          const char*      name,
                                          uint32_t         numberOfMembers,
                                          const uint32_t*  members )
{
    UTILS_ASSERT( scorep_unified_definition_manager );

    return define_group(
        scorep_unified_definition_manager,
        type,
        numberOfMembers,
        ( const uint64_t* )members,
        scorep_definitions_new_string(
            scorep_unified_definition_manager,
            name ? name : "", NULL ),
        true /* need to be converted from uint32_t */ );
}


void
scorep_definitions_unify_group( SCOREP_GroupDef*              definition,
                                SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    definition->unified = define_group(
        scorep_unified_definition_manager,
        definition->group_type,
        definition->number_of_members,
        definition->members,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        false );
}


SCOREP_GroupHandle
define_group( SCOREP_DefinitionManager* definition_manager,
              SCOREP_GroupType          groupType,
              uint64_t                  numberOfMembers,
              const uint64_t*           members,
              SCOREP_StringHandle       groupNameHandle,
              bool                      convertFromUint32 )
{
    SCOREP_GroupDef*   new_definition = NULL;
    SCOREP_GroupHandle new_handle     = SCOREP_INVALID_GROUP;
    SCOREP_DEFINITION_ALLOC_VARIABLE_ARRAY( Group,
                                            uint64_t,
                                            numberOfMembers );

    // Init new_definition
    new_definition->group_type = groupType;
    HASH_ADD_POD( new_definition, group_type );

    new_definition->name_handle = groupNameHandle;
    HASH_ADD_HANDLE( new_definition, name_handle, String );

    new_definition->number_of_members = numberOfMembers;
    HASH_ADD_POD( new_definition, number_of_members );

    if ( convertFromUint32 )
    {
        const int32_t* members32 = ( const int32_t* )members;
        for ( uint64_t i = 0; i < numberOfMembers; i++ )
        {
            new_definition->members[ i ] = ( uint64_t )members32[ i ];
        }
    }
    else
    {
        /* Just copy array */
        memcpy( new_definition->members,
                members,
                sizeof( new_definition->members[ 0 ] ) * numberOfMembers );
    }
    HASH_ADD_ARRAY( new_definition, members, number_of_members );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( Group, group );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_GROUP ) );
    }

    return new_handle;
}


bool
equal_group( const SCOREP_GroupDef* existingDefinition,
             const SCOREP_GroupDef* newDefinition )
{
    return existingDefinition->group_type == newDefinition->group_type
           && existingDefinition->name_handle == newDefinition->name_handle
           && existingDefinition->number_of_members == newDefinition->number_of_members
           && 0 == memcmp( existingDefinition->members,
                           newDefinition->members,
                           sizeof( existingDefinition->members[ 0 ] )
                           * existingDefinition->number_of_members );
}
