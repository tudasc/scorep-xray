/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 * @ingroup scorep_skiplist_module
 *
 * @brief This file implements a randomized skiplist for a predefined type.
 *
 * This file contains a header-only implementation of a ramdomized skiplist.
 * The value type of the list can be configured using a preprocessor macro.
 * The user needs to define `SCOREP_SKIPLIST_TYPE` prior to including this
 * file to a type used as payload for the skiplist. Furthermore, the
 * implementation will use this type to define `SCOREP_SKIPLIST_PREFIX` used
 * as <prefix> for internal skiplist function and variable names. The symbol
 * names are create by concatenating the define typename with a suffix,
 * separated by an underscore. The user needs to define three functions for the
 * skiplist to work properly:
 *   - <prefix>_cmp: a function that provides a comparison of two skiplist
 *   payloads.
 *   - <prefix>_reset_payload: a function that resets the payload to a defined
 *   state.
 *   - <prefix>_print_payload: a function that prints the payload information
 *   using `UTILS_DEBUG`.
 *
 * The user can then use the following skiplist functions to interact with the
 * skiplist, all of which are defined local scope (i.e., static):
 *   - <prefix>_compute_random_height:   compute the random height of the next
 *                                     element
 *   - <prefix>_create_list:             initialize skiplist internals.
 *   - <prefix>_destroy_list:            destroy list and return elements to
 *                                     free list
 *   - <prefix>_allocate_node_of_height: allocate a new list node of a certain
 *                                     height
 *   - <prefix>_get_node_of_height:      returns a new list node of a certain
 *                                     height
 *   - <prefix>_create_node:             returns a new list node
 *   - <prefix>_insert_node:             insert node into list
 *   - <prefix>_lower_bound:             return the last list node smaller or
 *                                     equal to the given search criteria
 *   - <prefix>_remove_node:             remove node from list
 *   - <prefix>_print_list_node:         calls *_print_payload to print node
 *                                     information
 *   - <prefix>_reset_node_links:        reset node links to other list nodes.
 *
 * The user can enable skiplist-specific debug output by defining
 * `SCOREP_SKIPLIST_DEBUG_ENABLE` prior to including the this file.
 */

#if !defined( SCOREP_SKIPLIST_TYPE )
    #error SCOREP_SKIPLIST_TYPE undefined. Please define it before including this source file.
#endif

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#if !defined( SCOREP_SKIPLIST_PREFIX )
/**
 * @def SCOREP_SKIPLIST_PREFIX defines the function and variable prefixes used
 *      in the definition of skiplist functions with internal and external
 *      linkage.
 */
#define SCOREP_SKIPLIST_PREFIX SCOREP_SKIPLIST_TYPE
#endif

/**
 * @def SCOREP_SKIPLIST_ Helper macro to define type specific names
 */
#define SCOREP_SKIPLIST_( suffix ) \
    UTILS_JOIN_SYMS( SCOREP_SKIPLIST_PREFIX, _ ## suffix )

/**
 * @def SCOREP_SKIPLIST_NODE Skiplist node type for a given base type
 */
#define SCOREP_SKIPLIST_NODE \
    SCOREP_SKIPLIST_( node )

/**
 * @def SCOREP_SKIPLIST_HANDLE Skiplist handle type for a given base type
 */
#define SCOREP_SKIPLIST_HANDLE \
    SCOREP_SKIPLIST_( skiplist_t )

/**
 * @def SCOREP_SKIPLIST_FUNC Wrapper macro for skiplist function names
 */
#define SCOREP_SKIPLIST_FUNC( suffix ) \
    SCOREP_SKIPLIST_( suffix )

// --- Generic definition for all skiplists

#ifndef SCOREP_SKIPLIST_H
#define SCOREP_SKIPLIST_H

/// @brief Pointer to memory used by the guards to ensure mutual exclusion
///
typedef void* SCOREP_Skiplist_GuardObject;

/// @brief Function pointer for guard callbacks used for locking and unlocking
///
typedef void (* SCOREP_Skiplist_Guard)( SCOREP_Skiplist_GuardObject );

/// @brief Default implementation of a no-op guard
///
static void
null_guard( SCOREP_Skiplist_GuardObject guard_object )
{
}

#endif /* ! SCOREP_SKIPLIST_H */

// --- end of generic definitions


/**
 * @internal
 * @brief Skiplist node
 */
typedef struct SCOREP_SKIPLIST_NODE SCOREP_SKIPLIST_NODE;
struct SCOREP_SKIPLIST_NODE
{
    SCOREP_SKIPLIST_TYPE   payload; ///< Payload block of skiplist node
    unsigned int           height;  ///< Number of elements in prev/next arrays
    SCOREP_SKIPLIST_NODE** prev;    ///< Array of pointers to previous nodes in each level
    SCOREP_SKIPLIST_NODE** next;    ///< Array of pointers to next nodes in each level
};

/**
 * @internal
 * @brief Skiplist handle
 */
typedef struct SCOREP_SKIPLIST_HANDLE SCOREP_SKIPLIST_HANDLE;
struct SCOREP_SKIPLIST_HANDLE
{
    SCOREP_SKIPLIST_NODE*       headnode;         ///< Dummy head node
    SCOREP_SKIPLIST_NODE**      freelist;         ///< Freelist pointers for managing released nodes
    unsigned int                random_seed;      ///< Random seed for PRNG
    unsigned int                ceiling;          ///< Largest height inserted so far
    unsigned int                max_height;       ///< Largest height allowed to be inserted
    unsigned long int           random_bits;      ///< Random source for coin flips to determine height
    unsigned int                bits_available;   ///< Number of unused random bits in the stream

    SCOREP_Skiplist_Guard       lock;             ///< Callback function to lock skiplist
    SCOREP_Skiplist_Guard       unlock;           ///< Callback function to unlock skiplist
    SCOREP_Skiplist_GuardObject guard_object;     ///< Memory to hold lock information
};

//------------------------------------------------------------
// Prototype declarations of user-defined functions.
//------------------------------------------------------------

static void
SCOREP_SKIPLIST_FUNC( reset_payload )( SCOREP_SKIPLIST_TYPE* element );

static void
SCOREP_SKIPLIST_FUNC( print_payload )( const char*                 prefix,
                                       const SCOREP_SKIPLIST_TYPE* request );

static int
SCOREP_SKIPLIST_FUNC( cmp )( const SCOREP_SKIPLIST_TYPE* listElement,
                             const SCOREP_SKIPLIST_TYPE* searchElement );

//------------------------------------------------------------
// Internal random number generator
//------------------------------------------------------------
/* Implementation of pseudo-random number generator as used
 * in glibc and described here:
 * https://stackoverflow.com/questions/11946622/implementation-of-random-number-generator
 */

/**
 * @internal
 * @brief Set random seed associated with the given @a list.
 *
 * @param[in,out] list Skiplist handle
 * @param[in] newseed New seed for skiplist
 *
 * @note This call assumes that the provided @a list is already guarded from
 *       concurrent accesses.
 */
static void
SCOREP_SKIPLIST_FUNC( set_random_seed )( SCOREP_SKIPLIST_HANDLE* list, int newseed )
{
    list->random_seed = ( unsigned )newseed & 0x7fffffffU;
}

/**
 * @internal
 * @brief Generate a new random number for list
 *
 * @param[in,out] list Skiplist handle
 *
 * @return Pseudo-random number
 *
 * @note This call assumes that the provided @a list is already guarded from
 *       concurrent accesses.
 */
static int
SCOREP_SKIPLIST_FUNC( get_random_number )( SCOREP_SKIPLIST_HANDLE* list )
{
    list->random_seed = ( list->random_seed * 1103515245U + 12345U ) & 0x7fffffffU;
    return ( int )list->random_seed;
}

//------------------------------------------------------------

/**
 * @internal
 * @brief Return a random height between 1 and @a max_height.
 *
 * @param max_height Maximum height to be reported
 *
 * @return Randomly chosen height
 *
 * @note The distribution of heights needs to ensure that the number
 *       of nodes with height i+1 is expected to be half the number
 *       of nodes with height i.
 */
static unsigned int
SCOREP_SKIPLIST_FUNC( compute_random_height )( SCOREP_SKIPLIST_HANDLE* list )
{
    int height           = 1;
    int coin_flip_result = 1;

#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
    UTILS_DEBUG_ENTRY();
#endif

    // Lock list to ensure random source is only used by a single thread */
    list->lock( list->guard_object );

    while ( coin_flip_result && height < list->max_height - 1 )
    {
        // Re-fill the bitstream with random 0/1 pattern if needed
        if ( list->bits_available == 0 )
        {
            // seed is part of list, which is already locked at this point
            list->random_bits = SCOREP_SKIPLIST_FUNC( get_random_number )( list );
            // get_random_number() provides 31 bits of randomized bits
            list->bits_available = 31;

#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
            UTILS_DEBUG( "Refreshed random bitstream" );
#endif
        }

        // ------------------------------------------------------------
        // Coin flip
        // ------------------------------------------------------------
        // check least-significant bit
        coin_flip_result = list->random_bits & 1;
        // Discard least-significant bit
        list->random_bits >>= 1;
        // Decrease the counter for available bits
        --( list->bits_available );

        // Increase height if coin toss was true
        if ( coin_flip_result == 1 )
        {
            ++height;
        }
    }

    list->unlock( list->guard_object );

#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
    UTILS_DEBUG_EXIT();
#endif

    return height;
}

/**
 * @internal
 * @brief Print the contents of a skiplist node
 *
 * @param prefix String prefix of the output
 * @param element Skiplist node to print
 */
static void
SCOREP_SKIPLIST_FUNC( print_list_node )( const char* prefix, const SCOREP_SKIPLIST_NODE*  element )
{
#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
    if ( element )
    {
        SCOREP_SKIPLIST_FUNC( print_payload )( prefix, &( element->payload ) );
    }
    else
    {
        UTILS_DEBUG( "%s: NULL", prefix );
    }
#endif
}

/**
 * @internal
 * @brief Reset links of element node
 *
 * @param element Skiplist node to reset
 */
static void
SCOREP_SKIPLIST_FUNC( reset_node_links )( SCOREP_SKIPLIST_NODE* element )
{
    for ( unsigned int level = 0; level < element->height; ++level )
    {
        element->next[ level ] = NULL;
        element->prev[ level ] = NULL;
    }
}

/**
 * @internal
 * @brief Allocate memory for a node of a given height
 *
 * @param height Height of the element to allocate
 *
 * @return A skiplist node of the given height
 */
static SCOREP_SKIPLIST_NODE*
SCOREP_SKIPLIST_FUNC( allocate_node_of_height )( unsigned int height )
{
    size_t element_size = sizeof( SCOREP_SKIPLIST_NODE )
                          + ( 2 * height * sizeof( SCOREP_SKIPLIST_NODE* ) );

    SCOREP_SKIPLIST_NODE* new_element =
        ( SCOREP_SKIPLIST_NODE*  )SCOREP_Memory_AllocForMisc( element_size );
    memset( new_element, 0, element_size );

    SCOREP_SKIPLIST_FUNC( reset_payload )( &( new_element->payload ) );

    new_element->height = height;
    new_element->next   = ( SCOREP_SKIPLIST_NODE** )( ( char* )new_element + sizeof( SCOREP_SKIPLIST_NODE ) );
    new_element->prev   = new_element->next + height;

    SCOREP_SKIPLIST_FUNC( reset_node_links )( new_element );

    return new_element;
}

/**
 * @internal
 * @brief Return a fresh skiplist element of a given height
 *
 * This routine will pick an element of a given height from the corresponding
 * freelist. If the freelist of the given height is empty, it will
 * automatically allocate a new element of the given height.
 *
 * @param height Height of the element to return
 *
 * @return A skiplist node of the given height
 */
static SCOREP_SKIPLIST_NODE*
SCOREP_SKIPLIST_FUNC( get_node_of_height )( SCOREP_SKIPLIST_HANDLE* list, unsigned int height )
{
    list->lock( list->guard_object );

    if ( list->freelist[ height ] == NULL )
    {
        SCOREP_SKIPLIST_NODE* new_node =
            SCOREP_SKIPLIST_FUNC( allocate_node_of_height )( height );
        list->unlock( list->guard_object );
        return new_node;
    }
    else
    {
        SCOREP_SKIPLIST_NODE* new_node = list->freelist[ height ];
        list->freelist[ height ] = list->freelist[ height ]->next[ 0 ];
        new_node->next[ 0 ]      = NULL;
        list->unlock( list->guard_object );

        return new_node;
    }
}

/**
 * @internal
 * @brief Return largest element that lower or equal to @a search_element.
 *
 * @param search_element Payload structure identifying the search criteria
 *
 * @return Largest list element lower or equal to the @a search_element.
 */
static SCOREP_SKIPLIST_NODE*
SCOREP_SKIPLIST_FUNC( lower_bound )( SCOREP_SKIPLIST_HANDLE* list, SCOREP_SKIPLIST_TYPE* searchElement )
{
#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
    UTILS_DEBUG_ENTRY();
#endif

    SCOREP_SKIPLIST_NODE* list_element = list->headnode;

    for ( unsigned int i = 1; i <= list->ceiling; ++i )
    {
        unsigned int level = list->ceiling - i;

#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
        UTILS_DEBUG( "Checking on level: %i", level );
#endif

        // advance to the largest element smaller or equal to the search element
        while ( list_element != NULL
                && list_element->next[ level ] != NULL
                && SCOREP_SKIPLIST_FUNC( cmp )( &( list_element->next[ level ]->payload ),
                                                searchElement ) <= 0 )
        {
            list_element = list_element->next[ level ];
        }

        // if this is already the searched element, return immediately
        if ( SCOREP_SKIPLIST_FUNC( cmp )( &( list_element->payload ), searchElement ) == 0 )
        {
#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
            UTILS_DEBUG_EXIT( "returned element" );
#endif
            return list_element;
        }
    }

    // return the largest element smaller than the search element
#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
    UTILS_DEBUG_EXIT( "returned lower bound element" );
#endif
    return list_element;
}

/**
 * @internal
 * @brief Create a new element for the skiplist
 */
SCOREP_SKIPLIST_NODE*
SCOREP_SKIPLIST_FUNC( create_node )( SCOREP_SKIPLIST_HANDLE* list )
{
    // determine height of new node
    unsigned int height = SCOREP_SKIPLIST_FUNC( compute_random_height )( list );

    // get a node of determined height
    return SCOREP_SKIPLIST_FUNC( get_node_of_height )( list, height );
}

/**
 * @internal
 * @brief Insert a new element into the skiplist
 */
static void
SCOREP_SKIPLIST_FUNC( insert_node )( SCOREP_SKIPLIST_HANDLE* list, SCOREP_SKIPLIST_NODE* newNode )
{
    list->lock( list->guard_object );

    // Insert element into list
    SCOREP_SKIPLIST_NODE* predecessor_element =
        SCOREP_SKIPLIST_FUNC( lower_bound )( list, &( newNode->payload ) );

#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
    SCOREP_SKIPLIST_FUNC( print_list_node )( "Inserting after", predecessor_element );
#endif

    for ( unsigned int level = 0; level < newNode->height; ++level )
    {
#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
        UTILS_DEBUG( "Linking on level: %lu", level );
#endif
        // insert into
        newNode->prev[ level ] = predecessor_element;
#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
        SCOREP_SKIPLIST_FUNC( print_list_node )( "Prev", newNode->prev[ level ] );
#endif
        newNode->next[ level ] = predecessor_element->next[ level ];
#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
        SCOREP_SKIPLIST_FUNC( print_list_node )( "Next", newNode->next[ level ] );
#endif

        if ( predecessor_element->next[ level ] )
        {
            predecessor_element->next[ level ]->prev[ level ] = newNode;
#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
            SCOREP_SKIPLIST_FUNC( print_list_node )( "Next->Prev", newNode->prev[ level ]->next[ level ] );
#endif
        }

        predecessor_element->next[ level ] = newNode;
#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
        SCOREP_SKIPLIST_FUNC( print_list_node )( "Prev->Next", newNode->prev[ level ]->next[ level ] );
#endif

        // search for predecessor on next level
        while ( predecessor_element->height <= level + 1 )
        {
            predecessor_element = predecessor_element->prev[ level ];
        }
    }

    if ( newNode->height > list->ceiling )
    {
        list->ceiling = newNode->height;
    }

    list->unlock( list->guard_object );
}

/**
 * @internal
 * @brief Remove an element from the skiplist
 *
 * @param element Element to remove
 */
static void
SCOREP_SKIPLIST_FUNC( remove_node )( SCOREP_SKIPLIST_HANDLE* list, SCOREP_SKIPLIST_NODE* element )
{
    list->lock( list->guard_object );

    for ( unsigned int level = 0; level < element->height; ++level )
    {
        if ( element->prev[ level ] )
        {
            element->prev[ level ]->next[ level ] = element->next[ level ];
            if ( element->next[ level ] )
            {
                element->next[ level ]->prev[ level ] = element->prev[ level ];
            }
        }
    }

    SCOREP_SKIPLIST_FUNC( reset_payload )( &( element->payload ) );
    SCOREP_SKIPLIST_FUNC( reset_node_links )( element );

    element->next[ 0 ]                = list->freelist[ element->height ];
    list->freelist[ element->height ] = element;

    list->unlock( list->guard_object );
}


/**
 * @internal
 * @brief Create a skiplist with expected size 2^height.
 *
 * @param[in] max_height  Max height of the nodes.
 *
 * @return Pointer to an initialized, empty skiplist.
 */
static SCOREP_SKIPLIST_HANDLE*
SCOREP_SKIPLIST_FUNC( create_list )( unsigned int                maxHeight,
                                     SCOREP_Skiplist_Guard       lock,
                                     SCOREP_Skiplist_Guard       unlock,
                                     SCOREP_Skiplist_GuardObject guardObject )
{
    size_t handle_size = sizeof( SCOREP_SKIPLIST_HANDLE )
                         + maxHeight * sizeof( SCOREP_SKIPLIST_NODE* );

    SCOREP_SKIPLIST_HANDLE* new_list =
        ( SCOREP_SKIPLIST_HANDLE* )SCOREP_Memory_AllocForMisc( handle_size );
    memset( new_list, 0, handle_size );

    new_list->max_height  = maxHeight;
    new_list->random_seed = 1;
    new_list->ceiling     = 1;
    new_list->freelist    = ( SCOREP_SKIPLIST_NODE** )( ( char* )( new_list ) + sizeof( SCOREP_SKIPLIST_HANDLE ) );

    new_list->headnode = SCOREP_SKIPLIST_FUNC( allocate_node_of_height )( maxHeight );

    if ( lock && unlock && guardObject )
    {
        new_list->lock         = lock;
        new_list->unlock       = unlock;
        new_list->guard_object = guardObject;
    }
    else
    {
        new_list->lock         = null_guard;
        new_list->unlock       = null_guard;
        new_list->guard_object = 0;
    }

    return new_list;
}


/**
 * @internal
 * @brief Check whether a given list is empty.
 *
 * @param list Point to skiplist
 *
 * @return true, if list contains no elements, false otherwise.
 */
static bool
SCOREP_SKIPLIST_FUNC( is_empty )( SCOREP_SKIPLIST_HANDLE* list )
{
    UTILS_BUG_ON( list == NULL, "Invalid skiplist handle" );

    return list->headnode->next[ 0 ] == NULL;
}

/**
 * @internal
 * @brief Destroy skiplist
 *
 * @param[in,out] list Pointer to skiplist to destroy
 *
 * @note As the memory for the skiplist elements are allocated through the
 * SCOREP_Memory_AllocForMisc, the memory for the elements is not released
 * until the finalization of the Score-P measurement system.
 */
static void
SCOREP_SKIPLIST_FUNC( destroy_list )( SCOREP_SKIPLIST_HANDLE* list )
{
    UTILS_BUG_ON( list == NULL, "Invalid skiplist handle" );

    while ( list->headnode->next[ 0 ] != NULL )
    {
        SCOREP_SKIPLIST_FUNC( remove_node )( list, *list->headnode->next );
    }
}

#ifdef SCOREP_SKIPLIST_DEBUG_ENABLE
#undef SCOREP_SKIPLIST_DEBUG_ENABLE
#endif /* SCOREP_SKIPTLIST_DEBUG_ENABLE */

#undef SCOREP_DEBUG_SKIPLIST_SCOPE
#undef SCOREP_SKIPLIST_
#undef SCOREP_SKIPLIST_FUNC
#undef SCOREP_SKIPLIST_HANDLE
#undef SCOREP_SKIPLIST_NODE
#undef SCOREP_SKIPLIST_PREFIX
#undef SCOREP_SKIPLIST_TYPE
