/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2023,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/* Minimal header for the use case SCOREP_HASH_TABLE_MONOTONIC_HEADER +
 * SCOREP_HASH_TABLE_MONOTONIC_DEFINITION. */

/************************** table *********************************************/

#include <SCOREP_FastHashtab.h>
#include <jenkins_hash.h>

typedef uint32_t table_key_t;
typedef uint32_t table_value_t;

#define TABLE_HASH_EXPONENT 8

static inline uint32_t
table_bucket_idx( table_key_t key )
{
#if defined( USE_JENKINS )
    return jenkins_hash( &key, sizeof( key ), 0 ) & hashmask( TABLE_HASH_EXPONENT );
#else
    /* use key directly to allow to trigger collisions */
    return key & hashmask( TABLE_HASH_EXPONENT );
#endif
}

static inline bool
table_equals( table_key_t key1,
              table_key_t key2 )
{
    return key1 == key2;
}


/* You need declare required functions that are not intended to be
   inlined before using SCOREP_HASH_TABLE_MONOTONIC_HEADER. */

void*
table_allocate_chunk( size_t chunkSize );

void
table_free_chunk( void* chunk );

table_value_t
table_value_ctor( table_key_t* key,
                  void*        ctorData );


SCOREP_HASH_TABLE_MONOTONIC_HEADER( table,
                                    8,
                                    hashsize( TABLE_HASH_EXPONENT ) );
