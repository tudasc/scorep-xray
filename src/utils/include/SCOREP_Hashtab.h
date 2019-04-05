/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_HASHTAB_H
#define SCOREP_HASHTAB_H

/*--- Header file documentation -------------------------------------------*/
/**
 * @file
 * @ingroup         SCOREP_Hashtab_module
 *
 * @brief           This file provides the function header and type definitions for
 *                  a STL-like hash table.
 */

#include <stdint.h>
#include <stddef.h>
#include <SCOREP_Types.h>

UTILS_BEGIN_C_DECLS

/*--- Module documentation for the hash table -----------------------------*/
/**
 * @defgroup  SCOREP_Hashtab_module SCOREP Hash Table
 *
 * This module provides type definitions and function prototypes for a generic
 * hash table data structure. The elements of such a hash table are stored in
 * no particular order as a key/value pair, where both items are represented
 * by @c void pointers (see SCOREP_Hashtab_Entry). Both pointers must point to
 * a valid memory location as long as the hash table entry is not deleted. If the
 * content of that memory location is changed, the content of the key/value will
 * change, too.
 *
 * In addition, this module defines an associated iterator data type
 * SCOREP_Hashtab_Iterator, which allows convenient traversal of all entries stored in
 * the hash table.
 *
 * The scorep_hashtab module follows an object-oriented style. That is, each
 * function (except the two @a create functions) takes a pointer to an
 * instance of either type @ref SCOREP_Hashtab or @ref SCOREP_Hashtab_Iterator as their first
 * argument, which is the object (i.e., the data structure) they operate on.
 *
 * @note This module uses the @c UTILS_ASSERT() macro to check various conditions
 *       (especially the values of given parameters) at runtime, which can
 *       cause a performance penalty.
 *
 * @{
 */

/*--- Type definitions ----------------------------------------------------*/

/** Opaque data structure representing a hash table. */
typedef struct scorep_hashtab_struct SCOREP_Hashtab;

/** Opaque data structure representing a hash table iterator. */
typedef struct scorep_hashtab_iter_struct SCOREP_Hashtab_Iterator;

/**
 * Supported values of the hash table
 * Adding a new type:
 *  - Only add a new value name and type to HASHTAB_VALUES
 */
#define HASHTAB_VALUES( FUNC )                  \
    FUNC( Ptr,    ptr,      void* )             \
    FUNC( Uint32, uint32,   uint32_t )          \
    FUNC( Uint64, uint64,   uint64_t )          \
    FUNC( Handle, handle,   SCOREP_AnyHandle )

/**
 * Generates hash table entry with the HASHTAB_VALUES
 */
#define GEN_UNION_ENTRY( Name, name, type ) \
    type name;

typedef struct
{
    void* key; /**< Unique key */
    union
    {
        HASHTAB_VALUES( GEN_UNION_ENTRY )
    } value;
} SCOREP_Hashtab_Entry;

/* *INDENT-OFF* */
/**
 * Inserting elements
 *
 * Generates declaration of insertation functions of Hashtab
 * e.g. SCOREP_Hashtab_InsertPtr
 *
 * Inserts the given (@a key,@a value) pair into the SCOREP_Hashtab @a instance.
 * In addition, the hash value (e.g., returned by a preceeding call of
 * SCOREP_Hashtab_Find()) can be specified. If the hash value should be
 * (re-)calculated instead, @c NULL should be passed.
 *
 * This function also has to allocate memory for internal data structures. If
 * this memory allocation request cannot be fulfilled, an error message is
 * printed and the program is aborted.
 *
 * @param instance   Object in which the key/value pair should be inserted
 * @param key        Unique key. The memory location to which it points must be
 *                   valid as long as the hash entry exists.
 * @param value      Associated value. The memory location to which it points must be
 *                   valid as long as the hash entry exists.
 * @param hashValPtr Optional storage where an hash value was previously stored
 *                   by a call to @a SCOREP_Hashtab_Find() with the same key @a
 *                   key. (ignored if @c NULL)
 *
 * @return Pointer to hash table entry of the newly inserted key;
 *         @c NULL otherwise
 */
#define GEN_HASHTAB_INSERT_FUNC_DECL( Name, name, type )            \
    SCOREP_Hashtab_Entry*                                           \
    SCOREP_Hashtab_Insert##Name( SCOREP_Hashtab* instance,          \
                                 void*           key,               \
                                 type            name,              \
                                 size_t*         hashValPtr );
/* *INDENT-ON* */


HASHTAB_VALUES( GEN_HASHTAB_INSERT_FUNC_DECL )

/**
 * Pointer-to-function type describing hashing functions. The function has to
 * compute an integral index value for the given @a key. If the computed index
 * is larger than the size of the hash table, it will be automatically adjusted
 * using modulo arithmetics.
 *
 * @param key Key value
 *
 * @return entry Computed hash table index
 */
typedef size_t ( * SCOREP_Hashtab_HashFunction )( const void* key );

/**
 * Pointer-to-function type describing key comparison functions. It has to
 * return 0 if the given @a key equals the key of the current item (@a item_key)
 * or a non-zero value otherwise.
 *
 * @param key      Key value to compare
 * @param item_key Key value of the current item
 * @return It has to
 * return 0 if the given @a key equals the key of the current item (@a item_key)
 * or a non-zero value otherwise.
 */
typedef int32_t ( * SCOREP_Hashtab_CompareFunction )( const void* key,
                                                      const void* item_key );

/**
 * Pointer-to-function type describing unary processing functions that can
 * be used with SCOREP_Hashtab_Foreach(). Here, the current key/value pair will
 * be passed as parameter @a entry.
 *
 * @param Hash table entry
 */
typedef void ( * SCOREP_Hashtab_ProcessFunction )( SCOREP_Hashtab_Entry* entry );

/**
 * Pointer-to-functions type which frees the memory for data item. It is
 * used in SCOREP_Hashtab_FreeAll() to free the memory of key and value items.
 *
 * @param item The data item which should be deleted.
 */
typedef void ( * SCOREP_Hashtab_DeleteFunction )( void* item );

/*
 * --------------------------------------------------------------------------
 * SCOREP_Hashtab
 * --------------------------------------------------------------------------
 */

/**
 * Creates and returns an instance of SCOREP_Hashtab. Besides the @a size of the
 * hash table, pointers to a hashing function as well as a key comparison
 * function have to be provided. If the memory allocation request cannot be
 * fulfilled, an error message is printed and the program is aborted.
 *
 * @param size     Size of the hash table
 * @param hashfunc Hashing function
 * @param kcmpfunc Key comparison function
 *
 * @return Pointer to new instance
 */
SCOREP_Hashtab*
SCOREP_Hashtab_CreateSize( size_t                         size,
                           SCOREP_Hashtab_HashFunction    hashfunc,
                           SCOREP_Hashtab_CompareFunction kcmpfunc );

/**
 * Destroys the given @a instance of SCOREP_Hashtab and releases the allocated
 * memory.
 *
 * @note Similar to the @ref SCOREP_Vector data structure, this call does not
 *       free the memory occupied by the elements, i.e., keys and values,
 *       since only pointers are stored. This has to be done separately.
 *
 * @param instance Object to be freed
 */
void
SCOREP_Hashtab_Free( SCOREP_Hashtab* instance );

/* Size operations */

/**
 * Returns the actual number of elements stored in the given SCOREP_Hashtab
 * @a instance.
 *
 * @param instance Queried object
 *
 * @return Number of elements stored
 */
size_t
SCOREP_Hashtab_Size( const SCOREP_Hashtab* instance );

/**
 * Returns whether the given SCOREP_Hashtab @a instance is empty.
 *
 * @param instance Queried object
 *
 * @return Non-zero value if instance if empty; zero otherwise
 */
int32_t
SCOREP_Hashtab_Empty( const SCOREP_Hashtab* instance );

/* Algorithms */

/**
 * Searches for an hash table entry with the specified @a key in the
 * given SCOREP_Hashtab @a instance, using the binary key comparison function
 * @a cmpfunc (see @ref SCOREP_Hashtab_CompareFunction for implementation details). If a
 * matching item could be found, a pointer to the key/value pair is returned.
 * In addition, if @a hashValPtr is non-@c NULL, the hash value is stored in
 * the memory location pointed to by @a hashValPtr, which can be used in an
 * subsequent call to SCOREP_Hashtab_Insert(). Otherwise, i.e., if no
 * matching item could be found, this function returns @c NULL.
 *
 * @param instance   Object in which the item is searched
 * @param key        Unique key to search for
 * @param hashValPtr Storage where hash value of the key will be is stored. For
 *                   use to a later call to @a SCOREP_Hashtab_Insert() with the
 *                   same key @a key. (ignored if @c NULL)
 *
 * @return Pointer to hash table entry if matching item could be found;
 *         @c NULL otherwise
 */
SCOREP_Hashtab_Entry*
SCOREP_Hashtab_Find( const SCOREP_Hashtab* instance,
                     const void*           key,
                     size_t*               hashValPtr );

/**
 * Calls the unary processing function @a procfunc for each element of
 * the given SCOREP_Hashtab @a instance.
 *
 * @param instance Object whose entries should be processed
 * @param procfunc Unary processing function
 */
void
SCOREP_Hashtab_Foreach( const SCOREP_Hashtab*          instance,
                        SCOREP_Hashtab_ProcessFunction procfunc );

/**
 * Removes the entry specified by @a key from the hash table @a instance.
 * The user must provide appropriate deletion function to free the memory allocated
 * for @a key and value.
 *
 * @param instance    Object in which the item is searched
 * @param key         Unique key to search for
 * @param hashValPtr  Storage where hash value of the key will be is stored. For
 *                    use to a later call to @a SCOREP_Hashtab_Insert() with the
 *                    same key @a key. (ignored if @c NULL)
 * @param deleteKey   Function pointer to a function which deletes the key
 *                    objects.
 * @param deleteValue Function pointer to a function which deletes the value
 *                    objects.
 */
void
SCOREP_Hashtab_Remove( const SCOREP_Hashtab*         instance,
                       const void*                   key,
                       SCOREP_Hashtab_DeleteFunction deleteKey,
                       SCOREP_Hashtab_DeleteFunction deleteValue,
                       size_t*                       hashValPtr );


/*
 * --------------------------------------------------------------------------
 * SCOREP_Hashtab_Iterator
 * --------------------------------------------------------------------------
 */

/* Construction & destruction */

/**
 * Creates and returns an iterator for the given SCOREP_Hashtab @a instance. If
 * the memory allocation request cannot be fulfilled, an error message is
 * printed and the program is aborted.
 *
 * @return Pointer to new instance
 */
SCOREP_Hashtab_Iterator*
SCOREP_Hashtab_IteratorCreate( const SCOREP_Hashtab* hashtab );

/**
 * Destroys the given @a instance of SCOREP_Hashtab_Iterator and releases the allocated
 * memory.
 *
 * @param instance Object to be freed
 */
void
SCOREP_Hashtab_IteratorFree( SCOREP_Hashtab_Iterator* instance );

/* Element access */

/**
 * Returns a pointer to the first entry of the hash table which is
 * associated to the given iterator @a instance. If the hash table is
 * empty, @c NULL is returned.
 *
 * @param instance Iterator object
 *
 * @return Pointer to first entry of the hash table or @c NULL if empty
 */
SCOREP_Hashtab_Entry*
SCOREP_Hashtab_IteratorFirst( SCOREP_Hashtab_Iterator* instance );

/**
 * Returns a pointer to the next entry of the hash table which is
 * associated to the given iterator @a instance. If no next entry is
 * available, @c NULL is returned.
 *
 * @param instance Iterator object
 *
 * @return Pointer to next entry of the hash table or @c NULL if unavailable
 */
SCOREP_Hashtab_Entry*
SCOREP_Hashtab_IteratorNext( SCOREP_Hashtab_Iterator* instance );

/*
 * --------------------------------------------------------------------------
 * Extensions
 * --------------------------------------------------------------------------
 */

/**
 * Destroys the given @a instance of SCOREP_Hashtab and releases the allocated
 * memory.
 *
 * @note In difference to SCOREP_Hashtab_Free, this call does
 *       free the memory occupied by the elements, i.e., keys and values.
 *       For freeing keys and values the function pointers @a deleteKey and
 *       @a deleteValue are used
 *
 * @param instance    Object to be freed
 * @param deleteKey   Function pointer to a function which deletes the key
 *                    objects.
 * @param deleteValue Function pointer to a function which deletes the value
 *                    objects.
 */

void
SCOREP_Hashtab_FreeAll( SCOREP_Hashtab*               instance,
                        SCOREP_Hashtab_DeleteFunction deleteKey,
                        SCOREP_Hashtab_DeleteFunction deleteValue );

/*
 * --------------------------------------------------------------------------
 * Default Comparison functions
 * --------------------------------------------------------------------------
 */

/**
 * A comparison function for the hash table which interprets the keys as pointers
 * to NULL-terminated strings. It uses strcmp() for comparison.
 *
 * @param key      Pointer to a NULL-terminated string.
 * @param item_key Pointer to a NULL-terminated string that is compared to @a key.
 * @return 0 if the given @a key equals the key of the current item (@a item_key)
 * or a non-zero value otherwise.
 */
int32_t
SCOREP_Hashtab_CompareStrings( const void* key,
                               const void* item_key );

/**
 * A comparison function for the hash table which interprets the keys as pointers
 * to 8-bit integers.
 *
 * @param key      Pointer to a 8-bit integer.
 * @param item_key Pointer to a 8-bit integer that is compared to @a key.
 * @return 0 if the given @a key equals the key of the current item (@a item_key)
 * or a non-zero value otherwise.
 */
int32_t
SCOREP_Hashtab_CompareInt8( const void* key,
                            const void* item_key );

/**
 * A comparison function for the hash table which interprets the keys as pointers
 * to 16-bit integers.
 *
 * @param key      Pointer to a 16-bit integer.
 * @param item_key Pointer to a 16-bit integer that is compared to @a key.
 * @return 0 if the given @a key equals the key of the current item (@a item_key)
 * or a non-zero value otherwise.
 */
int32_t
SCOREP_Hashtab_CompareInt16( const void* key,
                             const void* item_key );

/**
 * A comparison function for the hash table which interprets the keys as pointers
 * to 32-bit integers.
 *
 * @param key      Pointer to a 32-bit integer.
 * @param item_key Pointer to a 32-bit integer that is compared to @a key.
 * @return 0 if the given @a key equals the key of the current item (@a item_key)
 * or a non-zero value otherwise.
 */
int32_t
SCOREP_Hashtab_CompareInt32( const void* key,
                             const void* item_key );

/**
 * A comparison function for the hash table which interprets the keys as pointers
 * to 64-bit integers.
 *
 * @param key      Pointer to a 64-bit integer.
 * @param item_key Pointer to a 64-bit integer that is compared to @a key.
 * @return 0 if the given @a key equals the key of the current item (@a item_key)
 * or a non-zero value otherwise.
 */
int32_t
SCOREP_Hashtab_CompareInt64( const void* key,
                             const void* item_key );

/**
 * A comparison function for the hash table which interprets the keys as pointers
 * to unsigned 32-bit integers.
 *
 * @param key      Pointer to a unsigned 32-bit integer.
 * @param item_key Pointer to a unsigned 32-bit integer that is compared to @a key.
 * @return 0 if the given @a key equals the key of the current item (@a item_key)
 * or a non-zero value otherwise.
 */
int32_t
SCOREP_Hashtab_CompareUint32( const void* key,
                              const void* item_key );

/**
 * A comparison function for the hash table which interprets the keys as pointers
 * to unsigned 64-bit integers.
 *
 * @param key      Pointer to a unsigned 64-bit integer.
 * @param item_key Pointer to a unsigned 64-bit integer that is compared to @a key.
 * @return 0 if the given @a key equals the key of the current item (@a item_key)
 * or a non-zero value otherwise.
 */
int32_t
SCOREP_Hashtab_CompareUint64( const void* key,
                              const void* item_key );

/**
 * A comparison function for the hash table which compares the pointers.
 *
 * @param key      Comparison pointer.
 * @param item_key Pointer of the key.
 * @return 0 if the given @a key equals the key of the current item (@a item_key)
 * or a non-zero value otherwise.
 */
int32_t
SCOREP_Hashtab_ComparePointer( const void* key,
                               const void* item_key );

/*
 * --------------------------------------------------------------------------
 * Default hash functions
 * --------------------------------------------------------------------------
 */

/**
 * Computes a hash value from a NULL-terminated string. It interprets the key as a
 * pointer to a NULL-terminated string and sums up all letters of the string.
 *
 * @param key A pointer to a NULL-terminated string which serves as key.
 * @return A hashvalue which is computed as sum of the letters of the string.
 */
size_t
SCOREP_Hashtab_HashString( const void* key );

/**
 * Default hash function for 64-integer values. It just returns the integer value.
 *
 * @param key A pointer to a 64-bit integer.
 * @return The value of the key.
 */
size_t
SCOREP_Hashtab_HashInt64( const void* key );

/**
 * Default hash function for 32-integer values. It just returns the integer value.
 *
 * @param key A pointer to a 32-bit integer.
 * @return The value of the key.
 */
size_t
SCOREP_Hashtab_HashInt32( const void* key );

/**
 * Default hash function. It uses the address/8 as hash value. The division by
 * 8 is done to avoid gaps because of wordsize alignments.
 *
 * @param key A pointer.
 * @return The value of the key.
 */
size_t
SCOREP_Hashtab_HashPointer( const void* key );

/*
 * --------------------------------------------------------------------------
 * Default deletion functions
 * --------------------------------------------------------------------------
 */

/**
 * Default deletion function as requested SCOREP_Hashtab_FreeAll() which frees
 * the item.
 *
 * @param item Pointer to the item which is freed.
 */
void
SCOREP_Hashtab_DeleteFree( void* item );

/**
 * Default deletion function as requested SCOREP_Hashtab_FreeAll() which
 * Does nothing. It can be used in case that only one item from the key/value
 * pair must be freed. The one which should not be freed is handled with this
 * function.
 *
 * @param item Pointer to the item which should not be freed.
 */
void
SCOREP_Hashtab_DeleteNone( void* item );

/**
 * Default deletion function for pointers to pointers as requested for
 * SCOREP_Hashtab_FreeAll().
 *
 * @param item Pointer to the item which is freed.
 */
void
SCOREP_Hashtab_DeletePointer( void* item );

/** @} */

UTILS_END_C_DECLS

#endif /* SCOREP_HASHTAB_H */
