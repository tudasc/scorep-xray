/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2019-2023,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#include <SCOREP_ReaderWriterLock.h>

#include <UTILS_Atomic.h>
#include <UTILS_Error.h>
#include <UTILS_Mutex.h>

#include <stdint.h>
#include <stdbool.h>
#if HAVE( STDALIGN_H )
#include <stdalign.h>
#endif


/*
   SCOREP_HASH_TABLE_MONOTONIC( prefix, nPairsPerChunk, hashTableSize ) and
   SCOREP_HASH_TABLE_NON_MONOTONIC( prefix, nPairsPerChunk, hashTableSize )
   are thread-safe hash table templates that lock per bucket, if necessary.
   Locks for distinct buckets reside in different cachelines. A bucket
   uses chained chunks of nPairsPerChunk keys and values for conflict
   resolution.

   SCOREP_HASH_TABLE_MONOTONIC and SCOREP_HASH_TABLE_NON_MONOTONIC are
   intended to be used in implementation files only. In case you need
   to access the table from several implementation files, use the
   SCOREP_HASH_TABLE[_NON]_MONOTONIC_HEADER and
   SCOREP_HASH_TABLE[_NON]_MONOTONIC_DEFINITION variants instead,
   where you put the _HEADER into a header file and the _DEFINITION
   into an implementation file. The only difference between the two
   variants is, that in the former case, hash-table variables are
   declared static.

   For the remainder of the documentation, we just talk about
   SCOREP_HASH_TABLE[_NON]_MONOTONIC but also mean the _HEADER plus
   _DEFINITION variants.

   An instantiation requires the user to provide:

   @a prefix, a compilation-unit unique name that is used to prefix
   functions, variables and types.
   @a nPairsPerChunk, an positive integer specifying the size of the
   key-array and the value-array a chunk is composed of. In addition a
   chunk, holds a next pointer.
   @a hashTableSize, the number of buckets the table should hold.

   In addition the user needs to provide helper functions and
   typedefs. Care was taken to minimize locking: an arbitrary number
   of 'getters' can run concurrently, even if an insert operation
   takes place. Remove operations block until pending getters and
   inserters have finished.

   The monotonic version has no remove operation.

   Before instantiating a template, key and value types need to be provided:
   typedef <existing_type> <prefix>_key_t;
   typedef <existing_type> <prefix>_value_t;
   where <existing_type> is either a pointer, an integral type or a
   struct; if you use a struct, beware that objects get copied.

   For performance reasons, insert operations are modeled using
   a get_and_insert function that is provided by an
   instantiation: <prefix>_get_and_insert().

   The bucket is determined by calling the user-provided
   <prefix>_bucket_idx(). Which must return a value in the interval
   [0, hashTableSize). For searching, the user-provided function
   <prefix>_equals() is called. If an item wasn't found, it will be inserted
   utilizing the the user-provided function <prefix>_value_ctor() and
   potentially the user-provided <prefix>_allocate_chunk(). The provided
   remove operations <prefix>_remove() and <prefix>_remove_if() call the
   user-provided <prefix>_value_dtor() if a key-value pair is to be removed.
   The provided <prefix>_get_and_remove() will provide the value if
   key is found, transferring the responsibility to call
   <prefix>_value_dtor() or other means to manage allocated memory to
   the caller.

   A template instantiation provides the functions
   <prefix>_iterate_key_value_pairs() and <prefix>_free_chunks(). The former
   expects a callback function of type

   void (*)( <prefix>_key_t, <prefix>_value_t, void* );

   The latter require a user-provided functions <prefix>_free_chunk().
   These functions are supposed to be used in a serial context only.

   All types and functions names are prefixed with <prefix>, so you
   might instantiate more than one table per compilation unit. All
   locking is done using atomic operations.

   With nPairsPerChunk you determine the size of a chunk which is
   nPairsPerChunk * ( sizeof(<prefix>_key_t) +
   sizeof(<prefix>_value_t) ) + sizeof( void* ). You might consider
   aligned storage and filling at least one cacheline.


   SCOREP_HASH_TABLE_MONOTONIC( prefix, nPairsPerChunk, hashTableSize )
   alternatively
   SCOREP_HASH_TABLE_MONOTONIC_HEADER( prefix, nPairsPerChunk, hashTableSize )
   SCOREP_HASH_TABLE_MONOTONIC_DEFINITION( prefix, nPairsPerChunk, hashTableSize )
   --------------------------------------------------------------------

   A hash table without remove operation.

   An instantiation of SCOREP_HASH_TABLE_MONOTONIC provides the following
   public API (in addition, there are *_impl() functions which are not to
   be used directly):

   // <prefix>_get() will return true if @a key was found in the hash
   // table, which is checked using the function <prefix>_equals().
   // In this case @a *value will be assigned the hash table's value
   // corresponding to key. The function <prefix>_bucket_idx() determines
   // the table-bucket used in searching @a key. If @a key is not in
   // the table, false is returned and @a *value stays untouched. An
   // arbitrary number of get operations can run concurrently, even
   // together with get_and_insert operations.
   static inline bool
   <prefix>_get( <prefix>_key_t key,
                 <prefix>_value_t* value );

   // <prefix>_get_and_insert() will return the <prefix>_value_t that
   // corresponds to @a key. The function <prefix>_bucket_idx() determines
   // the table-bucket used in searching key. If @a key is not already
   // in the table, which is checked using the function
   // <prefix>_equals(), a new key-value pair will be added where the
   // value is provided by the function <prefix>_value_ctor(), passing
   // a reference to the final key storage and using @a ctorData.
   // Note that the new key value needs to be identicial to @a key in
   // terms of <prefix>_equals( key, internal-key-storage ).
   // If the table needs to allocate new internal data during insert,
   // it calls <prefix>_allocate_chunk(). Returns true if a new key-value
   // pair was inserted.
   static inline bool
   <prefix>_get_and_insert( <prefix>_key_t    key,
                            void*             ctorData,
                            <prefix>_value_t* value );

   // <prefix>_iterate_key_value_pairs() will iterate over the entire
   // table and call cb( key, value, cbData ) for each key-value pair.
   // This function is supposed to be used in a serial context only.
   static inline void
   <prefix>_iterate_key_value_pairs( void ( *cb )( <prefix>_key_t   key,
                                                   <prefix>_value_t value,
                                                   void*            cbData ),
                                     void* cbData );

   // <prefix>_free_chunks() iterates over all chunks of the table and
   // calls <prefix>_free_chunk() for each chunk. Afterwards, the
   // table will be empty. This function is supposed to be used in a
   // serial context only.
   static inline void
   <prefix>_free_chunks( void );

   The functions mentioned in the documentation above need to be
   provided by the user, in addition to the key and value type
   typedefs (see complete list below). Note that the dynamic memory
   allocation and deallocation of chunks and values is in the
   responsibility of the user. Deallocate values, if necessary, using
   <prefix>_iterate_key_value_pairs().  Deallocate chunks, if
   necessary, after deallocating values by calling
   <prefix>_free_chunks().

   Here the list of typedefs and functions that need to be
   declared/defined before instantiating SCOREP_HASH_TABLE_MONOTONIC(_HEADER):

   typedef <existing_type> <prefix>_key_t;
   typedef <existing_type> <prefix>_value_t;
   uint32_t <prefix>_bucket_idx( <prefix>_key_t key ); // consider inlining
   bool <prefix>_equals( <prefix>_key_t key1, <prefix>_key_t key2 ); // consider inlining
   void* <prefix>_allocate_chunk( size_t chunkSize ); // consider cacheline-size alignment
   <prefix>_value_t <prefix>_value_ctor( <prefix>_key_t* key, void* ctorData );
   void <prefix>_iterate_key_value_pair( <prefix>_key_t key, <prefix>_value_t value );
   void <prefix>_free_chunk( void* chunk );



   SCOREP_HASH_TABLE_NON_MONOTONIC( prefix, nPairsPerChunk, hashTableSize )
   alternatively
   SCOREP_HASH_TABLE_NON_MONOTONIC_HEADER( prefix, nPairsPerChunk, hashTableSize )
   SCOREP_HASH_TABLE_NON_MONOTONIC_DEFINITION( prefix, nPairsPerChunk, hashTableSize )
   ------------------------------------------------------------------------

   Similar to SCOREP_HASH_TABLE_MONOTONIC, but in addition, provides
   remove operations.  The remove operations will wait for
   get_and_insert operations to finish. When removal takes place, new
   get_and_insert operations will wait until removal is complete. When
   a remove operation leads to an empty chunk, store this chunk in a
   hash table specific free-list for later reuse.

   An instantiation of SCOREP_HASH_TABLE_NON_MONOTONIC provides
   everything from SCOREP_HASH_TABLE_MONOTONIC and in addition:

   // <prefix>_remove() will remove the key-value pair (if any)
   // corresponding to @a key from the table after calling
   // <prefix>_value_dtor(). If key is found, return true.
   static inline bool
   <prefix>_remove( <prefix>_key_t key );

   // <prefix>_get_and_remove() will remove the key-value pair (if
   // any) corresponding to @a key from the table and provide @value
   // to the caller. The caller is responsible for managing allocated
   // memory as <prefix>_value_dtor() isn't called. If key is found,
   // return true.
   static inline bool
   <prefix>_get_and_remove( <prefix>_key_t key,
                            <prefix>_value_t* value );

   // A function of following type needs to be provided to
   // <prefix>_remove_if().
   typedef bool ( *<prefix>_condition_t )( <prefix>_key_t,
                                           <prefix>_value_t,
                                           void* data );

   // <prefix>_remove_if() will iterate over the entire hash table and
   // call @a condition() for every key-value pair, providing @a data
   // as third parameter. If condition() returns true, the
   // corresponding key-value pair is removed and
   // <prefix>_value_dtor() is called.
   static inline void
   <prefix>_remove_if( <prefix>_condition_t condition, void* data );

   In addition to the functions and types required for
   SCOREP_HASH_TABLE_MONOTONIC, the following needs to be declared/defined
   before instantiating SCOREP_HASH_TABLE_NON_MONOTONIC(_HEADER):

   void <prefix>_value_dtor( <prefix>_key_t key, <prefix>_value_t value );

 */

/* *INDENT-OFF* */


/*
  Begin of implementation details. Don't use directly until 'End of
  implementation details'.
*/

#define SCOREP_HASH_TABLE_MONOTONIC_BUCKET( prefix ) \
    typedef struct prefix ## _chunk_t prefix ##  _chunk_t; \
    typedef struct prefix ## _bucket_t prefix ## _bucket_t; \
    struct prefix ## _bucket_t \
    { \
        SCOREP_ALIGNAS( SCOREP_CACHELINESIZE ) uint32_t size; \
        UTILS_Mutex                                     insert_lock; \
        prefix ## _chunk_t*                             chunk; \
    };


#define SCOREP_HASH_TABLE_NON_MONOTONIC_BUCKET( prefix ) \
    typedef struct prefix ## _chunk_t prefix ##  _chunk_t; \
    typedef struct prefix ## _bucket_t prefix ## _bucket_t; \
    struct prefix ## _bucket_t \
    { \
        SCOREP_ALIGNAS( SCOREP_CACHELINESIZE ) uint32_t size; \
        UTILS_Mutex                                     insert_lock; \
        prefix ## _chunk_t*                             chunk; \
        /* RWLock data on its own cache line improves performance. */ \
        SCOREP_ALIGNAS( SCOREP_CACHELINESIZE ) int16_t  pending; \
        int16_t                                         departing; \
        int16_t                                         release_n_readers; \
        int16_t                                         release_writer; \
        UTILS_Mutex                                     remove_lock; \
    };


/* Used in get and get_and_insert. Directly returns from get_and_insert, */
/* thus return false, i.e., not inserted, if key is found. */
#define SCOREP_HASH_TABLE_GET( prefix, nPairsPerChunk ) \
    uint32_t i                 = 0; \
    uint32_t j                 = 0; \
    uint32_t current_size      = UTILS_Atomic_LoadN_uint32( &( bucket->size ), UTILS_ATOMIC_SEQUENTIAL_CONSISTENT ); \
    prefix ## _chunk_t** chunk = &( bucket->chunk ); \
    uint32_t old_size; \
    /* search until end of chunks */ \
    do \
    { \
        for (; i < current_size; ++i, ++j ) \
        { \
            if ( j == ( nPairsPerChunk ) ) \
            { \
                chunk = &( ( *chunk )->next ); \
                j     = 0; \
            } \
            if ( prefix ## _equals( key, ( *chunk )->keys[ j ] ) ) \
            { \
                *value = ( *chunk )->values[ j ]; \
                return false; \
            } \
        } \
        old_size     = current_size; \
        current_size = UTILS_Atomic_LoadN_uint32( &( bucket->size ), UTILS_ATOMIC_SEQUENTIAL_CONSISTENT ); \
    } \
    while ( current_size > old_size );


#define SCOREP_HASH_TABLE_INSERT_1( prefix, nPairsPerChunk ) \
    /* not found, search again while waiting for 'insert_lock' */ \
    while ( true ) \
    { \
        if ( UTILS_MutexTrylock( &( bucket->insert_lock ) ) == true ) \
        { \
            break; \
        } \
        else \
        { \
            current_size = UTILS_Atomic_LoadN_uint32( &( bucket->size ), UTILS_ATOMIC_SEQUENTIAL_CONSISTENT ); \
            if ( current_size > old_size ) \
            { \
                for (; i < current_size; ++i, ++j ) \
                { \
                    if ( j == ( nPairsPerChunk ) ) \
                    { \
                        chunk = &( ( *chunk )->next ); \
                        j     = 0; \
                    } \
                    if ( prefix ## _equals( key, ( *chunk )->keys[ j ] ) ) \
                    { \
                        *value = ( *chunk )->values[ j ]; \
                        return false; \
                    } \
                } \
                old_size = current_size; \
            } \
        } \
    } \
    /* 'insert_lock' acquired: search again, inserts might have taken place in between */ \
    current_size = UTILS_Atomic_LoadN_uint32( &( bucket->size ), UTILS_ATOMIC_SEQUENTIAL_CONSISTENT ); \
    for (; i < current_size; ++i, ++j ) \
    { \
        if ( j == ( nPairsPerChunk ) ) \
        { \
            chunk = &( ( *chunk )->next ); \
            j     = 0; \
        } \
        if ( prefix ## _equals( key, ( *chunk )->keys[ j ] ) ) \
        { \
            UTILS_MutexUnlock( &( bucket->insert_lock ) ); \
            *value = ( *chunk )->values[ j ]; \
            return false; \
        } \
    }


#define SCOREP_HASH_TABLE_NEW_CHUNK_MONOTONIC( prefix, nPairsPerChunk ) \
    if ( current_size == 0 ) \
    { \
        bucket->chunk       = prefix ## _allocate_chunk( sizeof( prefix ## _chunk_t ) ); \
        bucket->chunk->next = NULL; \
        chunk               = &( bucket->chunk ); \
    } \
    else if ( j == ( nPairsPerChunk ) ) \
    { \
        ( *chunk )->next       = prefix ## _allocate_chunk( sizeof( prefix ## _chunk_t ) ); \
        ( *chunk )->next->next = NULL; \
        chunk                  = &( ( *chunk )->next ); \
        j                      = 0; \
    }


#define SCOREP_HASH_TABLE_ASSIGN_NEW_CHUNK_NON_MONOTONIC( prefix, assignTo ) \
    UTILS_MutexLock( &( prefix ## _chunk_free_list_lock ) ); \
    if ( prefix ## _chunk_free_list != NULL ) \
    { \
        ( assignTo )               = prefix ## _chunk_free_list; \
        prefix ## _chunk_free_list = prefix ## _chunk_free_list->next; \
        UTILS_MutexUnlock( &( prefix ## _chunk_free_list_lock ) ); \
    } \
    else \
    { \
        UTILS_MutexUnlock( &( prefix ## _chunk_free_list_lock ) ); \
        ( assignTo ) = prefix ## _allocate_chunk( sizeof( prefix ## _chunk_t ) ); \
    } \
    ( assignTo )->next = NULL; \
    chunk              = &( assignTo );


#define SCOREP_HASH_TABLE_NEW_CHUNK_NON_MONOTONIC( prefix, nPairsPerChunk ) \
    if ( current_size == 0 ) \
    { \
        SCOREP_HASH_TABLE_ASSIGN_NEW_CHUNK_NON_MONOTONIC( prefix, bucket->chunk ) \
    } \
    else if ( j == ( nPairsPerChunk ) ) \
    { \
        SCOREP_HASH_TABLE_ASSIGN_NEW_CHUNK_NON_MONOTONIC( prefix, ( *chunk )->next ) \
        j = 0; \
    }


#define SCOREP_HASH_TABLE_INSERT_2( prefix ) \
    /* create new value_t and insert */ \
    ( *chunk )->keys[ j ]   = key; \
    ( *chunk )->values[ j ] = prefix ## _value_ctor( &( *chunk )->keys[ j ], ctorData ); \
    UTILS_BUG_ON( !prefix ## _equals( key, ( *chunk )->keys[ j ] ), "Key values are not equal" ); \
    UTILS_Atomic_StoreN_uint32( &( bucket->size ), current_size + 1, UTILS_ATOMIC_SEQUENTIAL_CONSISTENT ); \
    UTILS_MutexUnlock( &( bucket->insert_lock ) ); \
    *value = ( *chunk )->values[ j ]; \
    return true;


#define SCOREP_HASH_TABLE_COMMON( prefix, nPairsPerChunk, hashTableSize ) \
\
    struct prefix ## _chunk_t \
    { \
        prefix ## _key_t    keys[ ( nPairsPerChunk ) ]; \
        prefix ## _value_t  values[ ( nPairsPerChunk ) ]; \
        prefix ## _chunk_t* next; \
    }; \
\
    /* Do not call concurrently */ \
    static inline void \
    prefix ## _iterate_key_value_pairs( void ( *cb )( prefix ## _key_t   key, \
                                                      prefix ## _value_t value, \
                                                      void*              cbData ), \
                                        void* cbData ) \
    { \
        for ( uint32_t b = 0; b < ( hashTableSize ); ++b ) \
        { \
            prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ b ] ); \
            prefix ## _chunk_t* chunk   = bucket->chunk; \
            uint32_t i                  = 0; \
            uint32_t current_size       = bucket->size; \
            while ( chunk != NULL ) \
            { \
                for ( int j = 0; i < current_size && j < ( nPairsPerChunk ); ++i, ++j ) \
                { \
                    cb( chunk->keys[ j ], chunk->values[ j ], cbData ); \
                } \
                chunk = chunk->next; \
            } \
        } \
    } \
\
    static inline bool \
    prefix ## _get_impl( prefix ## _key_t     key, \
                         prefix ## _value_t*  value, \
                         prefix ## _bucket_t* bucket ) \
    { \
        SCOREP_HASH_TABLE_GET( prefix, nPairsPerChunk ) /* might return */ \
        return true; /* i.e. not found */ \
    }


#define SCOREP_HASH_TABLE_FREE_CHUNKS( prefix, hashTableSize )  \
    for ( uint32_t b = 0; b < ( hashTableSize ); ++b ) \
    { \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ b ] ); \
        prefix ## _chunk_t* chunk   = bucket->chunk; \
        while ( chunk != NULL ) \
        { \
            prefix ## _chunk_t* next = chunk->next; \
            prefix ## _free_chunk( chunk ); \
            chunk = next; \
        } \
        bucket->chunk = NULL; \
        bucket->size  = 0; \
    }


#define SCOREP_HASH_TABLE_MONOTONIC_FUNCTIONS( prefix, nPairsPerChunk, hashTableSize ) \
    static inline bool \
    prefix ## _get_and_insert_impl( prefix ## _key_t     key, \
                                    void*                ctorData, \
                                    prefix ## _value_t*  value, \
                                    prefix ## _bucket_t* bucket ) \
    { \
        SCOREP_HASH_TABLE_GET( prefix, nPairsPerChunk ) /* might return */ \
        SCOREP_HASH_TABLE_INSERT_1( prefix, nPairsPerChunk ) /* might return */ \
        SCOREP_HASH_TABLE_NEW_CHUNK_MONOTONIC( prefix, nPairsPerChunk ) \
        SCOREP_HASH_TABLE_INSERT_2( prefix ) /* returns */ \
    } \
\
    static inline bool /* found */ \
    prefix ## _get( prefix ## _key_t    key, \
                    prefix ## _value_t* value ) \
    { \
        UTILS_ASSERT( value ); \
        uint32_t bucket_idx = prefix ## _bucket_idx( key ); \
        UTILS_BUG_ON( bucket_idx >= hashTableSize, "Out-of-bounds bucket index %u", bucket_idx ); \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ bucket_idx ] ); \
        return !prefix ## _get_impl( key, value, bucket ); \
    } \
\
    static inline bool \
    prefix ## _get_and_insert( prefix ## _key_t    key, \
                               void*               ctorData, \
                               prefix ## _value_t* value ) \
    { \
        UTILS_ASSERT( value ); \
        uint32_t bucket_idx = prefix ## _bucket_idx( key ); \
        UTILS_BUG_ON( bucket_idx >= hashTableSize, "Out-of-bounds bucket index %u", bucket_idx ); \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ bucket_idx ] ); \
        return prefix ## _get_and_insert_impl( key, ctorData, value, bucket ); \
    } \
\
    /* Do not call concurrently */ \
    static inline void \
    prefix ## _free_chunks( void ) \
    { \
        SCOREP_HASH_TABLE_FREE_CHUNKS( prefix, hashTableSize ) \
    }


#define SCOREP_HASH_TABLE_MOVE_LAST_TO_REMOVED( prefix, nPairsPerChunk ) \
    /* move last key-value pair to removed key-value pair's slot, decrement size: */ \
    /*   go to first element of current chunk */ \
    i -= j; \
    /*   traverse to last chunk; by then, (i,j=0) refers to the first element of the last chunk. */ \
    while ( chunk->next != NULL ) \
    { \
        previous_chunk = chunk; \
        chunk          = chunk->next; \
        i             += ( nPairsPerChunk ); \
    } \
    /*   go to last used element of last chunk */ \
    j = current_size - i - 1; \
    /*   move key-value pair, decrement size, release empty chunk to free list */ \
    *free_key   = chunk->keys[ j ]; \
    *free_value = chunk->values[ j ]; \
    if ( j == 0 ) /* sole pair of last chunk was moved, chunk is now empty */ \
    { \
        if ( previous_chunk == NULL ) \
        { \
            bucket->chunk = NULL; \
        } \
        else \
        { \
            previous_chunk->next = NULL; \
        } \
        /* lock chunk_free_list as it is per hash table, not per bucket */ \
        UTILS_MutexLock( &( prefix ## _chunk_free_list_lock ) ); \
        chunk->next = prefix ## _chunk_free_list; \
        prefix ## _chunk_free_list = chunk; \
        UTILS_MutexUnlock( &( prefix ## _chunk_free_list_lock ) ); \
    } \
    UTILS_Atomic_StoreN_uint32( &( bucket->size ), --current_size, UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );


#define SCOREP_HASH_TABLE_NON_MONOTONIC_FUNCTIONS( prefix, nPairsPerChunk, hashTableSize ) \
    static inline bool \
    prefix ## _get_and_insert_impl( prefix ## _key_t     key, \
                                    void*                ctorData, \
                                    prefix ## _value_t*  value, \
                                    prefix ## _bucket_t* bucket ) \
    { \
        SCOREP_HASH_TABLE_GET( prefix, nPairsPerChunk ) /* might return */ \
        SCOREP_HASH_TABLE_INSERT_1( prefix, nPairsPerChunk ) /* might return */ \
        SCOREP_HASH_TABLE_NEW_CHUNK_NON_MONOTONIC( prefix, nPairsPerChunk ) \
        SCOREP_HASH_TABLE_INSERT_2( prefix ) /* returns */ \
    } \
\
    static inline bool /* found */ \
    prefix ## _get( prefix ## _key_t    key, \
                    prefix ## _value_t* value ) \
    { \
        UTILS_ASSERT( value ); \
        uint32_t bucket_idx = prefix ## _bucket_idx( key ); \
        UTILS_BUG_ON( bucket_idx >= hashTableSize, "Out-of-bounds bucket index %u", bucket_idx ); \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ bucket_idx ] ); \
        SCOREP_RWLock_ReaderLock( &( bucket->pending ), &( bucket->release_n_readers ) ); \
        bool inserted = prefix ## _get_impl( key, value, bucket ); \
        SCOREP_RWLock_ReaderUnlock( &( bucket->pending ), &( bucket->departing ), &( bucket->release_writer ) ); \
        return !inserted; \
    } \
\
    static inline bool \
    prefix ## _get_and_insert( prefix ## _key_t    key, \
                               void*               ctorData, \
                               prefix ## _value_t* value ) \
    { \
        UTILS_ASSERT( value ); \
        uint32_t bucket_idx = prefix ## _bucket_idx( key ); \
        UTILS_BUG_ON( bucket_idx >= hashTableSize, "Out-of-bounds bucket index %u", bucket_idx ); \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ bucket_idx ] ); \
        SCOREP_RWLock_ReaderLock( &( bucket->pending ), &( bucket->release_n_readers ) ); \
        bool inserted = prefix ## _get_and_insert_impl( key, ctorData, value, bucket ); \
        SCOREP_RWLock_ReaderUnlock( &( bucket->pending ), &( bucket->departing ), &( bucket->release_writer ) ); \
        return inserted; \
    } \
\
    static inline bool \
    prefix ## _get_and_remove_impl( prefix ## _key_t key, \
                                    prefix ## _value_t* value ) \
    { \
        uint32_t bucket_idx = prefix ## _bucket_idx( key ); \
        UTILS_BUG_ON( bucket_idx >= hashTableSize, "Out-of-bounds bucket index %u", bucket_idx ); \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ bucket_idx ] ); \
        /* search and remove, if found, reduce bucket->size */ \
        uint32_t i                         = 0; \
        uint32_t j                         = 0; \
        prefix ## _chunk_t* previous_chunk = NULL; \
        bool found                         = false; \
        prefix ## _key_t*   free_key; \
        prefix ## _value_t* free_value; \
        SCOREP_RWLock_WriterLock( &( bucket->remove_lock ), &( bucket->pending ), \
                                  &( bucket->departing ), &( bucket->release_writer ) ); \
        uint32_t current_size     = UTILS_Atomic_LoadN_uint32( &( bucket->size ), UTILS_ATOMIC_SEQUENTIAL_CONSISTENT ); \
        prefix ## _chunk_t* chunk = bucket->chunk; \
        /* search until end of chunks */ \
        for (; i < current_size; ++i, ++j ) \
        { \
            if ( j == ( nPairsPerChunk ) ) \
            { \
                previous_chunk = chunk; \
                chunk          = chunk->next; \
                j              = 0; \
            } \
            if ( prefix ## _equals( key, chunk->keys[ j ] ) ) \
            { \
                if ( value ) \
                { \
                    /* Provide element to the caller, tranferring memory-management responsibility. */ \
                    *value = chunk->values[ j ]; \
                } \
                else \
                { \
                    /* Release element by calling _value_dtor() */ \
                    prefix ## _value_dtor( chunk->keys[ j ], chunk->values[ j ] ); \
                } \
                free_key   = &( chunk->keys[ j ] ); \
                free_value = &( chunk->values[ j ] ); \
                found      = true; \
                break; \
            } \
        } \
        if ( !found ) \
        { \
            SCOREP_RWLock_WriterUnlock( &( bucket->remove_lock ), &( bucket->pending ), \
                                        &( bucket->release_n_readers ) );  \
            return false; \
        } \
        SCOREP_HASH_TABLE_MOVE_LAST_TO_REMOVED( prefix, nPairsPerChunk ) \
        SCOREP_RWLock_WriterUnlock( &( bucket->remove_lock ), &( bucket->pending ), \
                                    &( bucket->release_n_readers ) ); \
        return true; \
    } \
\
    static inline bool \
    prefix ## _remove( prefix ## _key_t key ) \
    { \
        return prefix ## _get_and_remove_impl( key, ( prefix ## _value_t* ) 0 ); \
    } \
\
    static inline bool \
    prefix ## _get_and_remove( prefix ## _key_t key, \
                               prefix ## _value_t* value ) \
    { \
        UTILS_ASSERT( value ); \
        return prefix ## _get_and_remove_impl( key, value ); \
    } \
\
    typedef bool ( *prefix ## _condition_t )( prefix ## _key_t, prefix ## _value_t, void* data ); \
\
    static inline void \
    prefix ## _remove_if( prefix ## _condition_t condition, \
                          void*                  data ) \
    { \
        for ( uint32_t b = 0; b < ( hashTableSize ); ++b ) \
        { \
            prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ b ] ); \
            SCOREP_RWLock_WriterLock( &( bucket->remove_lock ), &( bucket->pending ), \
                                      &( bucket->departing ), &( bucket->release_writer ) ); \
            prefix ## _chunk_t* outer_chunk = bucket->chunk; \
            int32_t outer_i                 = 0; \
            uint32_t current_size           = UTILS_Atomic_LoadN_uint32( &( bucket->size ), UTILS_ATOMIC_SEQUENTIAL_CONSISTENT ); \
            while ( outer_chunk != NULL ) \
            { \
                for ( int32_t outer_j = 0; outer_i < current_size && outer_j < ( nPairsPerChunk ); ++outer_i, ++outer_j ) \
                { \
                    if ( condition( outer_chunk->keys[ outer_j ], outer_chunk->values[ outer_j ], data ) ) \
                    { \
                        uint32_t i = outer_i; \
                        uint32_t j = outer_j; \
                        prefix ## _chunk_t* chunk = outer_chunk; \
                        prefix ## _key_t*   free_key; \
                        prefix ## _value_t* free_value; \
                        prefix ## _chunk_t* previous_chunk = NULL; \
                        /* release element */ \
                        prefix ## _value_dtor( chunk->keys[ j ], chunk->values[ j ] ); \
                        free_key   = &( chunk->keys[ j ] ); \
                        free_value = &( chunk->values[ j ] ); \
                        SCOREP_HASH_TABLE_MOVE_LAST_TO_REMOVED( prefix, nPairsPerChunk ) \
                        --outer_i; \
                        --outer_j; \
                    } \
                } \
                outer_chunk = outer_chunk->next; \
            } \
            SCOREP_RWLock_WriterUnlock( &( bucket->remove_lock ), &( bucket->pending ), \
                                        &( bucket->release_n_readers ) ); \
        } \
    } \
\
    /* Do not call concurrently */ \
    static inline void \
    prefix ## _free_chunks( void ) \
    { \
        SCOREP_HASH_TABLE_FREE_CHUNKS( prefix, hashTableSize ) \
        /* in addition, deallocate free_list elements */ \
        while ( prefix ## _chunk_free_list ) \
        { \
            prefix ## _chunk_t* next = prefix ## _chunk_free_list->next; \
            prefix ## _free_chunk( prefix ## _chunk_free_list ); \
            prefix ## _chunk_free_list = next; \
        } \
    }

/* End of implementation details */


/*
   SCOREP_HASH_TABLE_MONOTONIC_(HEADER|DEFINITION)( prefix, nPairsPerChunk, hashTableSize )

   Use either SCOREP_HASH_TABLE_MONOTONIC in implementation files only,
   or the HEADER and DEFINITION variant if the table is to be accessed
   by several implementation files.

   See documentation above
 */
#define SCOREP_HASH_TABLE_MONOTONIC( prefix, nPairsPerChunk, hashTableSize ) \
    SCOREP_HASH_TABLE_MONOTONIC_BUCKET( prefix ) \
    static prefix ## _bucket_t prefix ## _hash_table[ ( hashTableSize ) ];  \
    SCOREP_HASH_TABLE_COMMON( prefix, nPairsPerChunk, hashTableSize ) \
    SCOREP_HASH_TABLE_MONOTONIC_FUNCTIONS( prefix, nPairsPerChunk, hashTableSize )

#define SCOREP_HASH_TABLE_MONOTONIC_HEADER( prefix, nPairsPerChunk, hashTableSize ) \
    SCOREP_HASH_TABLE_MONOTONIC_BUCKET( prefix ) \
    extern prefix ## _bucket_t prefix ## _hash_table[ ( hashTableSize ) ];  \
    SCOREP_HASH_TABLE_COMMON( prefix, nPairsPerChunk, hashTableSize ) \
    SCOREP_HASH_TABLE_MONOTONIC_FUNCTIONS( prefix, nPairsPerChunk, hashTableSize )

#define SCOREP_HASH_TABLE_MONOTONIC_DEFINITION( prefix, nPairsPerChunk, hashTableSize ) \
    prefix ## _bucket_t prefix ## _hash_table[ ( hashTableSize ) ];


/*
   SCOREP_HASH_TABLE_NON_MONOTONIC_(HEADER|DEFINITION)( prefix, nPairsPerChunk, hashTableSize )

   Use either SCOREP_HASH_TABLE_NON_MONOTONIC in implementation files only,
   or the HEADER and DEFINITION variant if the table is to be accessed
   by several implementation files.

   See documentation above.
 */

#define SCOREP_HASH_TABLE_NON_MONOTONIC( prefix, nPairsPerChunk, hashTableSize ) \
    struct prefix ## _chunk_t; \
    SCOREP_HASH_TABLE_NON_MONOTONIC_BUCKET( prefix )    \
    static struct prefix ## _chunk_t* prefix ## _chunk_free_list; \
    static UTILS_Mutex                prefix ## _chunk_free_list_lock; \
    static prefix ## _bucket_t        prefix ## _hash_table[ ( hashTableSize ) ]; \
    SCOREP_HASH_TABLE_COMMON( prefix, nPairsPerChunk, hashTableSize ) \
    SCOREP_HASH_TABLE_NON_MONOTONIC_FUNCTIONS( prefix, nPairsPerChunk, hashTableSize )


#define SCOREP_HASH_TABLE_NON_MONOTONIC_HEADER( prefix, nPairsPerChunk, hashTableSize ) \
    struct prefix ## _chunk_t; \
    SCOREP_HASH_TABLE_NON_MONOTONIC_BUCKET( prefix )    \
    extern struct prefix ## _chunk_t* prefix ## _chunk_free_list; \
    extern UTILS_Mutex                prefix ## _chunk_free_list_lock; \
    extern prefix ## _bucket_t        prefix ## _hash_table[ ( hashTableSize ) ]; \
    SCOREP_HASH_TABLE_COMMON( prefix, nPairsPerChunk, hashTableSize ) \
    SCOREP_HASH_TABLE_NON_MONOTONIC_FUNCTIONS( prefix, nPairsPerChunk, hashTableSize )


#define SCOREP_HASH_TABLE_NON_MONOTONIC_DEFINITION( prefix, nPairsPerChunk, hashTableSize ) \
    /* _chunk_free_list handling could be done with a lock-free stack implementation. */ \
    /* This implementation needs to deal with the ABA problem and a potential counter */ \
    /* overflow. As an implementation doesn't exist and there is no evidence that it */ \
    /* performs better than a UTILS_Mutex in real world scenarios, use a mutex for now. */ \
    struct prefix ## _chunk_t* prefix ## _chunk_free_list; \
    UTILS_Mutex                prefix ## _chunk_free_list_lock; \
    prefix ## _bucket_t        prefix ## _hash_table[ ( hashTableSize ) ];

/* *INDENT-ON*  */
