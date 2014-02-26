/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
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
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 */


#ifndef SCOREP_INTERNAL_DEFINITIONS_H
#error "Do not include this header directly, use SCOREP_Definitions.h instead."
#endif

#ifndef SCOREP_PRIVATE_DEFINITIONS_H
#define SCOREP_PRIVATE_DEFINITIONS_H

#include <SCOREP_Memory.h>
#include <stdint.h>

#include <UTILS_Error.h>

#include <jenkins_hash.h>

/**
 * Just a convenience macro to access the @e real memory a
 * SCOREP_Allocator_MovableMemory object is referring to.
 *
 * @param definition_memory_ptr Pointer to some SCOREP_Memory_DefinitionMemory
 * object.
 * @param targetType The type @a definition_memory_ptr should be converted to.
 *
 * @return A pointer to an object of type @a target_type.
 */
#define SCOREP_MEMORY_DEREF_MOVABLE( movableMemory, movablePageManager, targetType ) \
    ( ( targetType )SCOREP_Memory_GetAddressFromMovableMemory( movableMemory, \
                                                               movablePageManager ) )

#define SCOREP_MEMORY_DEREF_LOCAL( localMemory, targetType ) \
    SCOREP_MEMORY_DEREF_MOVABLE( localMemory, \
                                 SCOREP_Memory_GetLocalDefinitionPageManager(), \
                                 targetType )

/**
 *  Dereferences a moveable memory pointer to the definition struct.
 *
 */
#define SCOREP_HANDLE_DEREF( handle, Type, movablePageManager ) \
    SCOREP_MEMORY_DEREF_MOVABLE( handle, \
                                 movablePageManager, \
                                 SCOREP_ ## Type ## Def* )

#define SCOREP_LOCAL_HANDLE_DEREF( handle, Type ) \
    SCOREP_HANDLE_DEREF( handle, \
                         Type, \
                         SCOREP_Memory_GetLocalDefinitionPageManager() )

#define SCOREP_UNIFIED_HANDLE_DEREF( handle, Type ) \
    SCOREP_HANDLE_DEREF( handle, \
                         Type, \
                         SCOREP_Memory_GetLocalDefinitionPageManager() )


/**
 * Use this macro to define a definition struct.
 *
 * Usage:
 * @code
 *     SCOREP_DEFINE_DEFINITION_TYPE( Type )
 *     {
 *         SCOREP_DEFINE_DEFINITION_HEADER( Type ); // must be first
 *         // definition specfic members
 *         :
 *     };
 * @endcode
 *
 * @see SCOREP_DEFINE_DEFINITION_HEADER
 */
#define SCOREP_DEFINE_DEFINITION_TYPE( Type ) \
    typedef struct SCOREP_ ## Type ## Def SCOREP_ ## Type ## Def; \
    struct SCOREP_ ## Type ## Def

/**
 * Provides a stub for the definition struct header.
 *
 * The sequence_number member is mostly used as the id for the local definitions
 *
 * @see SCOREP_DEFINE_DEFINITION_TYPE
 *
 * @note No ';' after
 */
#define SCOREP_DEFINE_DEFINITION_HEADER( Type ) \
    SCOREP_ ## Type ## Handle next;             \
    SCOREP_ ## Type ## Handle unified;          \
    SCOREP_ ## Type ## Handle hash_next;        \
    uint32_t hash_value;                        \
    uint32_t sequence_number

/**
 * Initializes common members of the definition @a definition.
 *
 * @param definition pointer to definition.
 *
 * @note This needs to be in-sync with @a SCOREP_DEFINE_DEFINITION_HEADER
 * @see SCOREP_DEFINE_DEFINITION_HEADER
 */
#define SCOREP_INIT_DEFINITION_HEADER( definition )             \
    do \
    {                                                        \
        ( definition )->next            = SCOREP_MOVABLE_NULL;  \
        ( definition )->unified         = SCOREP_MOVABLE_NULL;  \
        ( definition )->hash_next       = SCOREP_MOVABLE_NULL;  \
        ( definition )->hash_value      = 0;                    \
        ( definition )->sequence_number = UINT32_MAX;           \
    } \
    while ( 0 )


/**
 *  Extracts the ID out of an handle pointer.
 *
 *  @note This is only the process local sequence number, which
 *        may happen to be the OTF2 definition id.
 *
 *  @note You can take also the address of this member with this macro
 */
#define SCOREP_HANDLE_TO_ID( handle, Type, movablePageManager ) \
    ( SCOREP_HANDLE_DEREF( handle, Type, movablePageManager )->sequence_number )

#define SCOREP_LOCAL_HANDLE_TO_ID( handle, Type ) \
    ( SCOREP_LOCAL_HANDLE_DEREF( handle, Type )->sequence_number )

#define SCOREP_UNIFIED_HANDLE_TO_ID( handle, Type ) \
    ( SCOREP_UNIFIED_HANDLE_DEREF( handle, Type )->sequence_number )


/**
 *  Extracts the hash value out of an handle pointer.
 *
 *  @note You can take also the address of this member with this macro
 */
#define SCOREP_GET_HASH_OF_HANDLE( handle, Type, movablePageManager ) \
    ( SCOREP_HANDLE_DEREF( handle, Type, movablePageManager )->hash_value )

#define SCOREP_GET_HASH_OF_LOCAL_HANDLE( handle, Type ) \
    ( SCOREP_LOCAL_HANDLE_DEREF( handle, Type )->hash_value )

#define SCOREP_GET_HASH_OF_UNIFIED_HANDLE( handle, Type ) \
    ( SCOREP_UNIFIED_HANDLE_DEREF( handle, Type )->hash_value )


/**
 *  Extracts the unified handle out of an handle pointer.
 *
 */
#define SCOREP_HANDLE_GET_UNIFIED( handle, Type, movablePageManager ) \
    ( SCOREP_HANDLE_DEREF( handle, Type, movablePageManager )->unified )


/**
 * Allocate a new definition of definition type @a Type.
 *
 * @needs Variable with name @a new_handle of handle type @a Type.
 * @needs Variable with name @a new_definition of definition type @a Type*.
 *
 * @note Only usable for local objects (ie. uses the local page manager).
 */
/* *INDENT-OFF* */
#define SCOREP_DEFINITION_ALLOC( Type ) \
    do \
    { \
        new_handle = SCOREP_Memory_AllocForDefinitions( NULL, \
            sizeof( SCOREP_ ## Type ## Def ) ); \
        new_definition = SCOREP_LOCAL_HANDLE_DEREF( new_handle, Type ); \
        SCOREP_INIT_DEFINITION_HEADER( new_definition ); \
    } \
    while ( 0 )
/* *INDENT-ON* */


/**
 * Allocate a new definition of type @a Type with a variable array member of
 * type @a array_type and a total number of members of @number_of_members.
 *
 * @needs Variable with name @a new_handle of handle type @a Type.
 * @needs Variable with name @a new_definition of definition type @a Type*.
 *
 * @note Only usable for local objects (ie. uses the local page manager).
 */
/* *INDENT-OFF* */
#define SCOREP_DEFINITION_ALLOC_VARIABLE_ARRAY( Type, \
                                                array_type, \
                                                number_of_members ) \
    do \
    { \
        new_handle = SCOREP_Memory_AllocForDefinitions( NULL, \
            sizeof( SCOREP_ ## Type ## Def ) + \
            ( ( number_of_members ) * sizeof( array_type ) ) ); \
        new_definition = SCOREP_LOCAL_HANDLE_DEREF( new_handle, Type ); \
        SCOREP_INIT_DEFINITION_HEADER( new_definition ); \
    } \
    while ( 0 )
/* *INDENT-ON* */

/**
 * Allocate a new definition of definition type @a Type with the given size.
 *
 * @needs Variable with name @a new_handle of handle type @a Type.
 * @needs Variable with name @a new_definition of definition type @a Type*.
 *
 * @note Only usable for local objects (ie. uses the local page manager).
 */
/* *INDENT-OFF* */
#define SCOREP_DEFINITION_ALLOC_SIZE( Type, size ) \
    do \
    { \
        new_handle = SCOREP_Memory_AllocForDefinitions( NULL, size ); \
        new_definition = SCOREP_LOCAL_HANDLE_DEREF( new_handle, Type ); \
        SCOREP_INIT_DEFINITION_HEADER( new_definition ); \
    } \
    while ( 0 )
/* *INDENT-ON* */


#define SCOREP_DEFINITIONS_DEFAULT_HASH_TABLE_POWER ( 8 )


/**
 * Defines a new type of definition for use in @a SCOREP_DefinitionManager.
 */
typedef struct scorep_definitions_manager_entry
{
    SCOREP_AnyHandle  head;
    SCOREP_AnyHandle* tail;
    SCOREP_AnyHandle* hash_table;
    uint32_t          hash_table_mask;
    uint32_t          counter;
    uint32_t*         mapping;
} scorep_definitions_manager_entry;


/**
 * Declares a definition manager entry of type @a type.
 */
#define SCOREP_DEFINITIONS_MANAGER_DECLARE_MEMBER( type ) \
    scorep_definitions_manager_entry type


/**
 * Initialize a definition type in a @a SCOREP_DefinitionManager @a
 * definition_manager.
 *
 */
static inline void
scorep_definitions_manager_init_entry( scorep_definitions_manager_entry* entry )
{
    entry->head            = SCOREP_MOVABLE_NULL;
    entry->tail            = &entry->head;
    entry->hash_table      = 0;
    entry->hash_table_mask = 0;
    entry->counter         = 0;
    entry->mapping         = 0;
}


/**
 * Initializes a manager entry named @a type in the definition manager @a
 * definition_manager.
 */
#define SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( definition_manager, type ) \
    scorep_definitions_manager_init_entry( &( definition_manager )->type )


/**
 * Allocate memory for a definition type's hash_table in a @a
 * SCOREP_DefinitionManager @a definition_manager.
 *
 * @note Will be called at init-time.
 */
static inline void
scorep_definitions_manager_entry_alloc_hash_table( scorep_definitions_manager_entry* entry,
                                                   uint32_t                          hash_table_power )
{
    UTILS_BUG_ON( hash_table_power > 15,
                  "Hash table too big: %u", hash_table_power );
    entry->hash_table_mask = hashmask( hash_table_power );
    entry->hash_table      = calloc( hashsize( hash_table_power ), sizeof( *entry->hash_table ) );
    UTILS_BUG_ON( entry->hash_table == 0,
                  "Can't allocate hash table of size %u",
                  hash_table_power );
}


/**
 * Allocates the hash table for type @a type in the given definition manager
 * with the default hash table size of @a SCOREP_DEFINITIONS_DEFAULT_HASH_TABLE_POWER.
 */
#define SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( definition_manager, type ) \
    scorep_definitions_manager_entry_alloc_hash_table( &( definition_manager )->type, \
                                                       SCOREP_DEFINITIONS_DEFAULT_HASH_TABLE_POWER )


/**
 * Allocates the array member @a type_mappings of struct SCOREP_DefinitionMappings that lives
 * in @a definition_manager. The size of the array equals @a type_definition_counter.
 *
 */
static inline void
scorep_definitions_manager_entry_alloc_mapping( scorep_definitions_manager_entry* entry )
{
    entry->mapping = NULL;
    if ( entry->counter > 0 )
    {
        entry->mapping = malloc( entry->counter * sizeof( *entry->mapping ) );
        UTILS_BUG_ON( entry->mapping == 0,
                      "Can't allocate mapping table of size %u",
                      entry->counter );
    }
}


/**
 * Frees the array member @a type_mappings of struct SCOREP_DefinitionMappings that lives
 * in @a definition_manager. The size of the array equals @a type_definition_counter.
 *
 */
static inline void
scorep_definitions_manager_entry_free_mapping( scorep_definitions_manager_entry* entry )
{
    free( entry->mapping );
    entry->mapping = NULL;
}


/**
 * Search for the definition @a new_definition in the definition manager @a
 * definition_manager, if the manager has a hash table allocated.
 *
 * If its found, discard the definition allocation done for @a new_allocation.
 *
 * If not, chain @a new_definition into the hash table and the definition
 * manager definitions list and assign the sequence number.
 *
 * @return Let return the calling function with the found definition's handle
 *         or the new definition as return value.
 *
 * @note This returns the calling function!
 *
 */
/* *INDENT-OFF* */
#define SCOREP_DEFINITIONS_MANAGER_ENTRY_ADD_DEFINITION( entry, \
                                                         Type, \
                                                         type, \
                                                         page_manager, \
                                                         new_definition, \
                                                         new_handle ) \
    do \
    { \
        if ( ( entry )->hash_table ) \
        { \
            SCOREP_AnyHandle* hash_table_bucket = \
                &( entry )->hash_table[ \
                    new_definition->hash_value & ( entry )->hash_table_mask ]; \
            SCOREP_AnyHandle hash_list_iterator = *hash_table_bucket; \
            while ( hash_list_iterator != SCOREP_MOVABLE_NULL ) \
            { \
                SCOREP_ ## Type ## Def * existing_definition = \
                    SCOREP_Allocator_GetAddressFromMovableMemory( \
                        page_manager, \
                        hash_list_iterator ); \
                if ( existing_definition->hash_value == new_definition->hash_value \
                     && equal_ ## type( existing_definition, new_definition ) ) \
                { \
                    SCOREP_Allocator_RollbackAllocMovable( \
                        page_manager, \
                        new_handle ); \
                    return hash_list_iterator; \
                } \
                hash_list_iterator = existing_definition->hash_next; \
            } \
            new_definition->hash_next = *hash_table_bucket; \
            *hash_table_bucket        = new_handle; \
        } \
        *( entry )->tail = new_handle; \
        ( entry )->tail  = &new_definition->next; \
        new_definition->sequence_number = ( entry )->counter++; \
    } \
    while ( 0 )
/* *INDENT-ON* */


/**
 * Search for the definition @a new_definition in the definition manager @a
 * definition_manager, if the manager has a hash table allocated.
 *
 * If its found, discard the definition allocation done for @a new_allocation.
 * If not, chain @a new_definition into the hash table and the definition
 * manager definitions list and assign the sequence number.
 *
 * @return Let return the calling function with the found definition's handle
 *         or the new definition as return value.
 *
 * @note This returns the calling function!
 *
 * @needs Variable named @a new_definition of definition type @a Type*.
 * @needs Variable with name @a new_handle of handle type @a Type.
 *        @see SCOREP_ALLOC_NEW_DEFINITION
 *        @see SCOREP_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY
 * @needs Variable named @a definition_manager of type @a SCOREP_DefinitionManager*.
 *
 * @note Only usable for local objects (ie. uses the local page manager).
 */
/* *INDENT-OFF* */
#define SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( Type, type ) \
    SCOREP_DEFINITIONS_MANAGER_ENTRY_ADD_DEFINITION( &( definition_manager )->type, \
                                                     Type, \
                                                     type, \
                                                     definition_manager->page_manager, \
                                                     new_definition, \
                                                     new_handle )
/* *INDENT-ON* */


/**
 * Iterator functions for definition. The iterator variable is named
 * @definition.
 * @{
 *
 * Iterates over all definitions of definition type @a Type from the
 * definition manager @a definition_manager.
 *
 * Example:
 * @code
 *  SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_definition_manager, String, string )
 *  {
 *      :
 *      definition->member = ...
 *      :
 *  }
 *  SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
 * @endcode
 *
 * You can still use break and continue statements.
 *
 * @declares Variable with name @a definition of definition type @a Type*.
 */
/* *INDENT-OFF* */
#define SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_BEGIN( entry, Type, page_manager ) \
    do \
    { \
        SCOREP_ ## Type ## Def*   definition; \
        SCOREP_ ## Type ## Handle handle; \
        for ( handle = ( entry )->head; \
              handle != SCOREP_MOVABLE_NULL; \
              handle = definition->next ) \
        { \
            definition = SCOREP_HANDLE_DEREF( handle, Type, page_manager ); \

#define SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_END() \
        } \
    } \
    while ( 0 )

#define SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definition_manager, Type, type ) \
    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_BEGIN( &( definition_manager )->type, \
                                                               Type, \
                                                               ( definition_manager )->page_manager )

#define SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END() \
        SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_END()
/* *INDENT-ON* */
/** @} */


/**
 * Some convenient macros to add members or sub-hashes to the hash value
 * @{
 */

/**
 * Calculates and add the hash value from the member @a pod_member to the hash
 * value of the definition @a definition.
 *
 * @a pod_member needs to be of any scalar type (i.e. int, double, ...).
 *
 * @note Only usable for local objects (ie. uses the local page manager).
 */
#define HASH_ADD_POD( definition, pod_member ) \
    do \
    { \
        ( definition )->hash_value = jenkins_hash( \
            &( definition )->pod_member, \
            sizeof( ( definition )->pod_member ), \
            ( definition )->hash_value ); \
    } \
    while ( 0 )

/**
 * Calculates and add the hash value from the array member @a array_member to
 * the hash value of the definition @a definition.
 *
 * @a array_member needs to be an array of any scalar type (i.e. int, double, ...).
 *
 * @note Only usable for local objects (ie. uses the local page manager).
 */
#define HASH_ADD_ARRAY( definition, array_member, number_member ) \
    do \
    { \
        ( definition )->hash_value = jenkins_hash( \
            ( definition )->array_member, \
            sizeof( ( definition )->array_member[ 0 ] ) \
            * ( definition )->number_member, \
            ( definition )->hash_value ); \
    } \
    while ( 0 )

/**
 * Add the hash value from the handle member @a handle_member with definition
 * type @a Type to the hash value of the definition @a definition.
 *
 * @note Only usable for local objects (ie. uses the local page manager).
 */
#define HASH_ADD_HANDLE( definition, handle_member, Type ) \
    do \
    { \
        ( definition )->hash_value = jenkins_hashword( \
            &SCOREP_GET_HASH_OF_LOCAL_HANDLE( ( definition )->handle_member, \
                                              Type ), \
            1, \
            ( definition )->hash_value ); \
    } \
    while ( 0 )

/**
 * @}
 */


#endif /* SCOREP_PRIVATE_DEFINITIONS_H */
