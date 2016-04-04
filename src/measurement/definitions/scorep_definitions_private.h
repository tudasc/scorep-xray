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

#ifndef SCOREP_DEFINITIONS_PRIVATE_H
#define SCOREP_DEFINITIONS_PRIVATE_H

/**
 * @file
 *
 */


#include <jenkins_hash.h>


#define SCOREP_MEMORY_DEREF_LOCAL( localMemory, targetType ) \
    SCOREP_MEMORY_DEREF_MOVABLE( localMemory, \
                                 SCOREP_Memory_GetLocalDefinitionPageManager(), \
                                 targetType )

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
 *  Extracts the hash value out of a handle pointer.
 *
 *  @note You can take also the address of this member with this macro
 */
#define SCOREP_GET_HASH_OF_LOCAL_HANDLE( handle, Type ) \
    ( SCOREP_LOCAL_HANDLE_DEREF( handle, Type )->hash_value )


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


#endif /* SCOREP_DEFINITIONS_PRIVATE_H */
