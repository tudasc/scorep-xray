#ifndef SCOREP_DEFINITION_MACROS_H_
#define SCOREP_DEFINITION_MACROS_H_

/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       scorep_definition_macros.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 */


#include <scorep_utility/SCOREP_Allocator.h>
#include <SCOREP_Memory.h>
#include <stdint.h>


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

#define SCOREP_MEMORY_DEREF_REMOTE( remoteMemory, targetType ) \
    SCOREP_MEMORY_DEREF_MOVABLE( localMemory, \
                                 SCOREP_Memory_GetRemoteDefinitionPageManager(), \
                                 targetType )


/**
 *  Dereferences a moveable memory pointer to the definition struct.
 *
 */
#define SCOREP_HANDLE_DEREF( handle, Type, movablePageManager ) \
    SCOREP_MEMORY_DEREF_MOVABLE( handle, \
                                 movablePageManager, \
                                 SCOREP_ ## Type ## _Definition* )

#define SCOREP_LOCAL_HANDLE_DEREF( handle, Type ) \
    SCOREP_HANDLE_DEREF( handle, \
                         Type, \
                         SCOREP_Memory_GetLocalDefinitionPageManager() )

#define SCOREP_UNIFIED_HANDLE_DEREF( handle, Type ) \
    SCOREP_HANDLE_DEREF( handle, \
                         Type, \
                         SCOREP_Memory_GetLocalDefinitionPageManager() )

#define SCOREP_REMOTE_HANDLE_DEREF( handle, Type ) \
    SCOREP_HANDLE_DEREF( handle, \
                         Type, \
                         SCOREP_Memory_GetRemoteDefinitionPageManager() )


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
    typedef struct SCOREP_ ## Type ## _Definition SCOREP_ ## Type ## _Definition; \
    struct SCOREP_ ## Type ## _Definition

/**
 * Provides a stub for the definition struct header.
 *
 * The sequence_number member is mostly used as the id for the local definitions
 *
 * @see SCOREP_DEFINE_DEFINITION_TYPE
 *
 * @note: This should be insync with the definition of @a scorep_any_definition
 * @note: No ';' after
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
 * @note: This needs to be in-sync with @a SCOREP_DEFINE_DEFINITION_HEADER
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
 *  @note: This is only the process local sequence number, which
 *         may happen to be the OTF2 definition id.
 *
 *  @note You can take also the address of this member with this macro
 */
#define SCOREP_HANDLE_TO_ID( handle, Type, movablePageManager ) \
    ( SCOREP_HANDLE_DEREF( handle, Type, movablePageManager )->sequence_number )

#define SCOREP_LOCAL_HANDLE_TO_ID( handle, Type ) \
    ( SCOREP_LOCAL_HANDLE_DEREF( handle, Type )->sequence_number )

#define SCOREP_UNIFIED_HANDLE_TO_ID( handle, Type ) \
    ( SCOREP_UNIFIED_HANDLE_DEREF( handle, Type )->sequence_number )

#define SCOREP_REMOTE_HANDLE_TO_ID( handle, Type ) \
    ( SCOREP_REMOTE_HANDLE_DEREF( handle, Type )->sequence_number )


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

#define SCOREP_GET_HASH_OF_REMOTE_HANDLE( handle, Type ) \
    ( SCOREP_REMOTE_HANDLE_DEREF( handle, Type )->hash_value )


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
        new_handle = SCOREP_Memory_AllocForDefinitions( \
            sizeof( SCOREP_ ## Type ## _Definition ) ); \
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
        new_handle = SCOREP_Memory_AllocForDefinitions( \
            sizeof( SCOREP_ ## Type ## _Definition ) + \
            ( ( number_of_members ) * sizeof( array_type ) ) ); \
        new_definition = SCOREP_LOCAL_HANDLE_DEREF( new_handle, Type ); \
        SCOREP_INIT_DEFINITION_HEADER( new_definition ); \
    } \
    while ( 0 )
/* *INDENT-ON* */

/**
 * Search for the definition @a new_definition in the definition manager @a
 * definition_manager, if the manager has a hash table allocated.
 *
 * If its found, discard the definition allocation done for @a new_allocation.
 *
 * If not, chain @a new_definition into the hash table and the definition
 * manager definitions list and assign the sequence number.
 *
 * @return Let return the calling function with the found defintion's handle
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
#define SCOREP_DEFINITION_MANAGER_ADD_DEFINITION( Type, type ) \
    do \
    { \
        SCOREP_ ## Type ## Handle* hash_table_bucket = 0; \
        if ( definition_manager->type ## _definition_hash_table ) \
        { \
            hash_table_bucket = &definition_manager->type ## _definition_hash_table[ \
                new_definition->hash_value & SCOREP_DEFINITION_HASH_TABLE_MASK ]; \
            SCOREP_ ## Type ## Handle hash_list_iterator = *hash_table_bucket; \
            while ( hash_list_iterator != SCOREP_MOVABLE_NULL ) \
            { \
                SCOREP_ ## Type ## _Definition * existing_definition = SCOREP_LOCAL_HANDLE_DEREF( \
                    hash_list_iterator, Type ); \
                if ( scorep_ ## type ## _definitions_equal( existing_definition, new_definition ) ) \
                { \
                    SCOREP_Allocator_RollbackAllocMovable( \
                        SCOREP_Memory_GetLocalDefinitionPageManager(), \
                        new_handle ); \
                    return hash_list_iterator; \
                } \
                hash_list_iterator = existing_definition->hash_next; \
            } \
            new_definition->hash_next = *hash_table_bucket; \
            *hash_table_bucket        = new_handle; \
        } \
        *( definition_manager->type ## _definition_tail_pointer ) = \
            new_handle; \
        definition_manager->type ## _definition_tail_pointer = \
            &new_definition->next; \
        new_definition->sequence_number = \
            definition_manager->type ## _definition_counter++; \
        return new_handle; \
    } \
    while ( 0 )
/* *INDENT-ON* */


/* this size is temporary */
#define SCOREP_DEFINITION_HASH_TABLE_SIZE hashsize( 8 )
#define SCOREP_DEFINITION_HASH_TABLE_MASK hashmask( 8 )


/**
 * Defines a new type of definition for use in @a SCOREP_DefinitionManager.
 *
 * @note: No ';'
 */
#define SCOREP_DEFINE_DEFINITION_MANAGER_MEMBERS( Type, type ) \
    SCOREP_ ## Type ## Handle type ## _definition_head; \
    SCOREP_ ## Type ## Handle * type ## _definition_tail_pointer; \
    SCOREP_ ## Type ## Handle * type ## _definition_hash_table; \
    uint32_t type ## _definition_counter;


/**
 * Initialize a definition type in a @a SCOREP_DefinitionManager @a
 * definition_manager.
 *
 * @note Only lower-case type needed
 */
#define SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( type, definition_manager ) \
    do \
    { \
        ( definition_manager )->type ## _definition_head = \
            SCOREP_MOVABLE_NULL; \
        ( definition_manager )->type ## _definition_tail_pointer = \
            &( ( definition_manager )->type ## _definition_head ); \
        ( definition_manager )->type ## _definition_hash_table = 0; \
        ( definition_manager )->type ## _definition_counter    = 0; \
    } \
    while ( 0 )


/**
 * Allocate memory for a definition type's hash_table in a @a
 * SCOREP_DefinitionManager @a definition_manager.
 *
 * @note Will be called at init-time.
 */
#define SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( type, definition_manager ) \
    do \
    { \
        ( definition_manager )->type ## _definition_hash_table = \
            calloc( SCOREP_DEFINITION_HASH_TABLE_SIZE, \
                    sizeof( *( ( definition_manager )->type ## _definition_hash_table ) ) ); \
        assert( ( definition_manager )->type ## _definition_hash_table ); \
    } \
    while ( 0 )


/**
 * Iterator functions for definition. The iterator variable is named
 * @definition.
 * @{
 *
 * Iterates over all definitions of definition type @a Type from the
 * definition mnager @a definition_manager.
 *
 * Example:
 * @code
 *  SCOREP_DEFINITION_FOREACH_DO( &scorep_definition_manager, String, string )
 *  {
 *      :
 *      definition->member = ...
 *      :
 *  }
 *  SCOREP_DEFINITION_FOREACH_WHILE();
 * @endcode
 *
 * You can still use break and continue statements.
 *
 * @declares Variable with name @a definition of definition type @a Type*.
 */
/* *INDENT-OFF* */
#define SCOREP_DEFINITION_FOREACH_DO( definition_manager, Type, type ) \
    do \
    { \
        SCOREP_ ## Type ## _Definition* definition; \
        SCOREP_ ## Type ## Handle handle; \
        for ( handle = ( definition_manager )->type ## _definition_head; \
              handle != SCOREP_MOVABLE_NULL; \
              handle = definition->next ) \
        { \
            definition = SCOREP_HANDLE_DEREF( handle, Type, ( definition_manager )->page_manager ); \

#define SCOREP_DEFINITION_FOREACH_WHILE() \
        } \
    } \
    while ( 0 )
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
        ( definition )->hash_value = hash( &( definition )->pod_member, \
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
        ( definition )->hash_value = hash( \
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
        ( definition )->hash_value = hashword( \
            &SCOREP_GET_HASH_OF_LOCAL_HANDLE( ( definition )->handle_member, \
                                              Type ), \
            1, \
            ( definition )->hash_value ); \
    } \
    while ( 0 )

/**
 * @}
 */


/**
 * Copies all definitions of type @a type to the unified definition manager.
 *
 * @needs Variable named @a definition_manager of type @a SCOREP_DefinitionManager*.
 *        The definitions to be copied live here.
 */
#define SCOREP_COPY_DEFINITIONS_TO_UNIFIED_DEFINITION_MANAGER( definition_manager, Type, type ) \
    do \
    { \
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "Copying %s to unified", #type ); \
        SCOREP_DEFINITION_FOREACH_DO( definition_manager, Type, type ) \
        { \
            SCOREP_Copy ## Type ## DefinitionToUnified( definition, ( definition_manager )->page_manager ); \
        } \
        SCOREP_DEFINITION_FOREACH_WHILE(); \
    } \
    while ( 0 )


/**
 * Allocates the array member @a type_mappings of struct SCOREP_DefinitionMappings that lives
 * in @a definition_manager. The size of the array equals @a type_definition_counter.
 *
 */
#define SCOREP_ALLOC_MAPPINGS_ARRAY( type, definition_manager ) \
    do \
    { \
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "Alloc mappings for %s", #type ); \
        ( definition_manager )->mappings->type ## _mappings = NULL; \
        if ( ( definition_manager )->type ## _definition_counter > 0 ) \
        { \
            ( definition_manager )->mappings->type ## _mappings = \
                calloc( sizeof( uint32_t ), ( definition_manager )->type ## _definition_counter ); \
            assert( ( definition_manager )->mappings->type ## _mappings ); \
        } \
    } \
    while ( 0 )


/**
 * Frees the array member @a type_mappings of struct SCOREP_DefinitionMappings that lives
 * in @a definition_manager. The size of the array equals @a type_definition_counter.
 *
 */
#define SCOREP_FREE_MAPPINGS_ARRAY( type, definition_manager ) \
    do \
    { \
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "Alloc mappings for %s", #type ); \
        free( ( definition_manager )->mappings->type ## _mappings ); \
        ( definition_manager )->mappings->type ## _mappings = NULL; \
    } \
    while ( 0 )


/**
 * Fill the mapping array member @a type_mappings @a SCOREP_DefinitionMappings with the
 * corresponding sequence numbers of the unified definition.
 *
 * @see SCOREP_ALLOC_MAPPINGS_ARRAY()
 */
#define SCOREP_ASSIGN_MAPPINGS( definition_manager, Type, type ) \
    do \
    { \
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "Assign mappings for %s", #Type ); \
        if ( ( definition_manager )->type ## _definition_counter > 0 ) \
        { \
            uint32_t type ## _sequence_number = 0; \
            SCOREP_DEFINITION_FOREACH_DO( definition_manager, Type, type ) \
            { \
                assert( type ## _sequence_number == definition->sequence_number ); \
                ( definition_manager )->mappings->type ## _mappings[ type ## _sequence_number ] = \
                    SCOREP_UNIFIED_HANDLE_DEREF( definition->unified, Type )->sequence_number; \
                ++type ## _sequence_number; \
            } \
            SCOREP_DEFINITION_FOREACH_WHILE(); \
        } \
    } \
    while ( 0 )


/**
 * Generate and write the id mapping for definition type @a type into the
 * OTF2 local definition writer @a definition_writer.
 *
 * @note @a TYPE denotes the all-caps OTF2 name of the definition type.
 */
#define SCOREP_WRITE_DEFINITION_MAPPING_TO_OTF2( type, TYPE, definition_writer ) \
    do \
    { \
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_DEFINITIONS, "Write mappings to OTF2 for %s", #type ); \
        if ( scorep_local_definition_manager.mappings && \
             scorep_local_definition_manager.mappings->type ## _mappings && \
             scorep_local_definition_manager.type ## _definition_counter > 0 ) \
        { \
            SCOREP_IdMap* map = SCOREP_IdMap_Create( \
                SCOREP_ID_MAP_DENSE, \
                scorep_local_definition_manager.type ## _definition_counter ); \
            for ( uint32_t index = 0; \
                  index < scorep_local_definition_manager.type ## _definition_counter; \
                  ++index ) \
            { \
                SCOREP_IdMap_AddIdPair( \
                    map, \
                    index, \
                    scorep_local_definition_manager.mappings->type ## _mappings[ index ] ); \
            } \
            SCOREP_Error_Code status = OTF2_DefWriter_WriteMappingTable( \
                definition_writer, \
                map, \
                OTF2_MAPPING_ ## TYPE ); \
            assert( status == SCOREP_SUCCESS ); \
            SCOREP_IdMap_Free( map ); \
        } \
    } \
    while ( 0 )

#endif /* SCOREP_DEFINITION_MACROS_H_ */
