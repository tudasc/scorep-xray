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
 * Copyright (c) 2009-2011, 2016,
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

/**
 * @file
 * @ingroup         SCOREP_Hashtab_module
 *
 * @brief           Implementation of a STL-like hash table.
 */

#include <config.h>
#include <SCOREP_Hashtab.h>

#include <UTILS_Error.h>

#include <stdlib.h>


/*
 * --------------------------------------------------------------------------
 * SCOREP_Hashtab
 * --------------------------------------------------------------------------
 */

/*--- Type definitions ----------------------------------------------------*/

typedef struct scorep_hashtab_listitem_struct scorep_hashtab_listitem;

/* Collision list entry */
struct scorep_hashtab_listitem_struct
{
    SCOREP_Hashtab_Entry     entry;      /* Table entry (key, value) */
    size_t                   hash_value; /* hash value for entry.key, for fast comparison */
    scorep_hashtab_listitem* next;       /* Pointer to next entry */
};

/* Actual hash table data type */
struct scorep_hashtab_struct
{
    scorep_hashtab_listitem**      table;   /* Field elements */
    size_t                         tabsize; /* Number of field elements */
    size_t                         size;    /* Number of items stored */
    SCOREP_Hashtab_HashFunction    hash;    /* Hashing function */
    SCOREP_Hashtab_CompareFunction kcmp;    /* Comparison function */
};


/*--- Construction & destruction ------------------------------------------*/

SCOREP_Hashtab*
SCOREP_Hashtab_CreateSize( size_t                         size,
                           SCOREP_Hashtab_HashFunction    hashfunc,
                           SCOREP_Hashtab_CompareFunction kcmpfunc )
{
    SCOREP_Hashtab* instance;

    /* Validate arguments */
    UTILS_ASSERT( size > 0 && hashfunc && kcmpfunc );

    /* Create hash table data structure */
    instance = ( SCOREP_Hashtab* )malloc( sizeof( SCOREP_Hashtab ) );
    if ( !instance )
    {
        UTILS_ERROR_POSIX();
        return NULL;
    }

    instance->table = ( scorep_hashtab_listitem** )calloc( size, sizeof( scorep_hashtab_listitem* ) );
    if ( !instance->table )
    {
        UTILS_ERROR_POSIX();
        free( instance );
        return NULL;
    }

    /* Initialization */
    instance->tabsize = size;
    instance->size    = 0;
    instance->hash    = hashfunc;
    instance->kcmp    = kcmpfunc;

    return instance;
}


void
SCOREP_Hashtab_Free( SCOREP_Hashtab* instance )
{
    size_t index;

    /* Validate arguments */
    UTILS_ASSERT( instance );

    /* Release data structure */
    for ( index = 0; index < instance->tabsize; ++index )
    {
        scorep_hashtab_listitem* item = instance->table[ index ];
        while ( item )
        {
            scorep_hashtab_listitem* next = item->next;
            free( item );
            item = next;
        }
    }
    free( instance->table );
    free( instance );
}


/*--- Size operations -----------------------------------------------------*/

size_t
SCOREP_Hashtab_Size( const SCOREP_Hashtab* instance )
{
    /* Validate arguments */
    UTILS_ASSERT( instance );

    return instance->size;
}

int32_t
SCOREP_Hashtab_Empty( const SCOREP_Hashtab* instance )
{
    /* Validate arguments */
    UTILS_ASSERT( instance );

    return instance->size == 0;
}


/*--- Inserting & removing elements ---------------------------------------*/

/* *INDENT-OFF* */
#define GEN_HASHTAB_INSERT_FUNC_DEF( Name, name, type )                                 \
    SCOREP_Hashtab_Entry*                                                               \
    SCOREP_Hashtab_Insert##Name( SCOREP_Hashtab* instance,                              \
                                  void*           key,                                  \
                                  type            value,                                \
                                  size_t*         hashValPtr )                          \
    {                                                                                   \
        scorep_hashtab_listitem* item;                                                  \
        size_t                   hashval;                                               \
        size_t                   index;                                                 \
        /* Validate arguments */                                                        \
        UTILS_ASSERT( instance && key );                                                \
        /* Eventually calculate hash value */                                           \
        if ( hashValPtr )                                                               \
        {                                                                               \
            hashval = *hashValPtr;                                                      \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            hashval = instance->hash( key );                                            \
        }                                                                               \
        index = hashval % instance->tabsize;                                            \
        /* Create new item */                                                           \
        item = ( scorep_hashtab_listitem* )malloc( sizeof( scorep_hashtab_listitem ) ); \
        if ( !item )                                                                    \
        {                                                                               \
            UTILS_ERROR_POSIX();                                                        \
            return NULL;                                                                \
        }                                                                               \
        /* Initialize item */                                                           \
        item->entry.key              = key;                                             \
        item->entry.value.name       = value;                                           \
        item->hash_value             = hashval;                                         \
        item->next                   = instance->table[ index ];                        \
        /* Add item to hash table */                                                    \
        instance->table[ index ] = item;                                                \
        instance->size++;                                                               \
        return &item->entry;                                                            \
    }
/* *INDENT-ON* */
HASHTAB_VALUES( GEN_HASHTAB_INSERT_FUNC_DEF )

void
SCOREP_Hashtab_Remove( const SCOREP_Hashtab*         instance,
                       const void*                   key,
                       SCOREP_Hashtab_DeleteFunction deleteKey,
                       SCOREP_Hashtab_DeleteFunction deleteValue,
                       size_t*                       hashValPtr )
{
    scorep_hashtab_listitem* item;
    scorep_hashtab_listitem* last = NULL;
    size_t                   hashval;
    size_t                   index;

    /* Validate arguments */
    UTILS_ASSERT( instance && key );

    /* Eventually calculate hash value */
    if ( hashValPtr )
    {
        hashval = *hashValPtr;
    }
    else
    {
        hashval = instance->hash( key );
    }
    index = hashval % instance->tabsize;

    /* Search element */
    item = instance->table[ index ];
    while ( item )
    {
        if ( hashval == item->hash_value &&
             0 == instance->kcmp( key, item->entry.key ) )
        {
            if ( last == NULL )
            {
                instance->table[ index ] = item->next;
            }
            else
            {
                last->next = item->next;
            }

            deleteKey( item->entry.key );
            deleteValue( item->entry.value.ptr );
            free( item );
            return;
        }

        last = item;
        item = item->next;
    }
}



/*--- Algorithms ----------------------------------------------------------*/

SCOREP_Hashtab_Entry*
SCOREP_Hashtab_Find( const SCOREP_Hashtab* instance,
                     const void*           key,
                     size_t*               hashValPtr )
{
    scorep_hashtab_listitem* item;
    size_t                   hashval;
    size_t                   index;

    /* Validate arguments */
    UTILS_ASSERT( instance && key );

    /* Calculate hash value */
    hashval = instance->hash( key );
    if ( hashValPtr )
    {
        *hashValPtr = hashval;
    }
    index = hashval % instance->tabsize;

    /* Search element */
    item = instance->table[ index ];
    while ( item )
    {
        if ( hashval == item->hash_value &&
             0 == instance->kcmp( key, item->entry.key ) )
        {
            return &item->entry;
        }

        item = item->next;
    }

    return NULL;
}

void
SCOREP_Hashtab_Foreach( const SCOREP_Hashtab*          instance,
                        SCOREP_Hashtab_ProcessFunction procfunc )
{
    SCOREP_Hashtab_Iterator* iter;
    SCOREP_Hashtab_Entry*    entry;

    /* Validate arguments */
    UTILS_ASSERT( instance && procfunc );

    /* Execute function for each entry */
    iter  = SCOREP_Hashtab_IteratorCreate( instance );
    entry = SCOREP_Hashtab_IteratorFirst( iter );
    while ( entry )
    {
        procfunc( entry );
        entry = SCOREP_Hashtab_IteratorNext( iter );
    }
    SCOREP_Hashtab_IteratorFree( iter );
}


/*
 * --------------------------------------------------------------------------
 * SCOREP_Hashtab_Iterator
 * --------------------------------------------------------------------------
 */

/*--- Type definitions ----------------------------------------------------*/

/* Actual hash table iterator data structure */
struct scorep_hashtab_iter_struct
{
    const SCOREP_Hashtab*    hashtab;      /* corresponding hash table */
    size_t                   index;        /* current field index      */
    scorep_hashtab_listitem* item;         /* current item             */
};


/*--- Construction & destruction ------------------------------------------*/

SCOREP_Hashtab_Iterator*
SCOREP_Hashtab_IteratorCreate( const SCOREP_Hashtab* hashtab )
{
    SCOREP_Hashtab_Iterator* instance;

    /* Validate arguments */
    UTILS_ASSERT( hashtab );

    /* Create iterator */
    instance = ( SCOREP_Hashtab_Iterator* )malloc( sizeof( SCOREP_Hashtab_Iterator ) );
    if ( !instance )
    {
        UTILS_ERROR_POSIX();
        return NULL;
    }

    /* Initialization */
    instance->hashtab = hashtab;
    instance->index   = 0;
    instance->item    = NULL;

    return instance;
}

void
SCOREP_Hashtab_IteratorFree( SCOREP_Hashtab_Iterator* instance )
{
    /* Validate arguments */
    UTILS_ASSERT( instance );

    /* Release iterator */
    free( instance );
}


/*--- Element access ------------------------------------------------------*/

SCOREP_Hashtab_Entry*
SCOREP_Hashtab_IteratorFirst( SCOREP_Hashtab_Iterator* instance )
{
    /* Validate arguments */
    UTILS_ASSERT( instance );

    /* Hash table empty? */
    if ( 0 == instance->hashtab->size )
    {
        return NULL;
    }

    /* Reset iterator */
    instance->index = 0;
    instance->item  = NULL;

    /* Search first list entry */
    while ( instance->hashtab->table[ instance->index ] == NULL &&
            instance->index < instance->hashtab->tabsize )
    {
        instance->index++;
    }
    instance->item = instance->hashtab->table[ instance->index ];

    return &instance->item->entry;
}

SCOREP_Hashtab_Entry*
SCOREP_Hashtab_IteratorNext( SCOREP_Hashtab_Iterator* instance )
{
    /* Validate arguments */
    UTILS_ASSERT( instance );

    /* No more entries? */
    if ( instance->item == NULL )
    {
        return NULL;
    }

    /* Search next entry */
    instance->item = instance->item->next;
    while ( instance->item == NULL )
    {
        instance->index++;
        if ( instance->index == instance->hashtab->tabsize )
        {
            return NULL;
        }

        instance->item = instance->hashtab->table[ instance->index ];
    }

    return &instance->item->entry;
}
