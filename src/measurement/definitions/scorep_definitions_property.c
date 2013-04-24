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


/**
 * @status     alpha
 * @file       src/measurement/definitions/scorep_definitions_property.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <definitions/SCOREP_Definitions.h>


#include <assert.h>
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
#include <scorep_types.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_PropertyHandle
define_property( SCOREP_DefinitionManager* definition_manager,
                 SCOREP_Property           property,
                 SCOREP_PropertyCondition  condition,
                 bool                      initialValue,
                 bool                      invalidated );


static bool
equal_property( const SCOREP_PropertyDef* existingDefinition,
                const SCOREP_PropertyDef* newDefinition );


/**
 * Associate a name with a process unique property handle.
 */
SCOREP_PropertyHandle
SCOREP_Definitions_NewProperty( SCOREP_Property          property,
                                SCOREP_PropertyCondition condition,
                                bool                     initialValue )
{
    UTILS_DEBUG_ENTRY( "%d, %d, %s",
                       property,
                       condition,
                       initialValue ? "true" : "false" );

    UTILS_ASSERT( property < SCOREP_PROPERTY_MAX );

    SCOREP_Definitions_Lock();

    SCOREP_PropertyHandle new_handle = define_property(
        &scorep_local_definition_manager,
        property,
        condition,
        initialValue,
        false );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_CopyPropertyDefinitionToUnified( SCOREP_PropertyDef*           definition,
                                        SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    definition->unified = define_property(
        scorep_unified_definition_manager,
        definition->property,
        definition->condition,
        definition->initialValue,
        definition->invalidated );
}


SCOREP_PropertyHandle
define_property( SCOREP_DefinitionManager* definition_manager,
                 SCOREP_Property           property,
                 SCOREP_PropertyCondition  condition,
                 bool                      initialValue,
                 bool                      invalidated )
{
    assert( definition_manager );

    SCOREP_PropertyDef*   new_definition = NULL;
    SCOREP_PropertyHandle new_handle     = SCOREP_INVALID_PARAMETER;

    SCOREP_DEFINITION_ALLOC( Property );
    new_definition->property = property;
    HASH_ADD_POD( new_definition, property );
    new_definition->condition = condition;
    HASH_ADD_POD( new_definition, condition );
    new_definition->initialValue = initialValue;
    HASH_ADD_POD( new_definition, initialValue );
    new_definition->invalidated = invalidated;
    // no hashing, can be modified

    // modified SCOREP_DEFINITION_MANAGER_ADD_DEFINITION macro:

    SCOREP_PropertyHandle* hash_table_bucket = 0;

    if ( definition_manager->property_definition_hash_table )
    {
        hash_table_bucket = &definition_manager->property_definition_hash_table[
            new_definition->hash_value & SCOREP_DEFINITION_HASH_TABLE_MASK ];
        SCOREP_PropertyHandle hash_list_iterator = *hash_table_bucket;

        while ( hash_list_iterator != SCOREP_MOVABLE_NULL )
        {
            SCOREP_PropertyDef* existing_definition = SCOREP_LOCAL_HANDLE_DEREF(
                hash_list_iterator, Property );
            if ( equal_property( existing_definition, new_definition ) )
            {
                /* the hash guarantees that both properties have the same condition */
                switch ( existing_definition->condition )
                {
                    case SCOREP_PROPERTY_CONDITION_ALL:
                        existing_definition->invalidated =
                            existing_definition->invalidated && new_definition->invalidated;
                        break;

                    case SCOREP_PROPERTY_CONDITION_ANY:
                        existing_definition->invalidated =
                            existing_definition->invalidated || new_definition->invalidated;
                        break;
                    default:
                        UTILS_BUG( "Invalid condition for property" );
                }

                SCOREP_Allocator_RollbackAllocMovable(
                    SCOREP_Memory_GetLocalDefinitionPageManager(),
                    new_handle );
                return hash_list_iterator;
            }
            hash_list_iterator = existing_definition->hash_next;
        }
        new_definition->hash_next = *hash_table_bucket;
        *hash_table_bucket        = new_handle;
    }

    *definition_manager->property_definition_tail_pointer =
        new_handle;
    definition_manager->property_definition_tail_pointer =
        &new_definition->next;
    new_definition->sequence_number =
        definition_manager->property_definition_counter++;

    return new_handle;
}


bool
equal_property( const SCOREP_PropertyDef* existingDefinition,
                const SCOREP_PropertyDef* newDefinition )
{
    return existingDefinition->property == newDefinition->property;
}
