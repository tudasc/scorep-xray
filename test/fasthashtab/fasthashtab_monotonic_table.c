/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2023,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#include <config.h>

#include "fasthashtab_monotonic_table.h"

#include <assert.h>
#include <stdlib.h>

SCOREP_HASH_TABLE_MONOTONIC_DEFINITION( table,
                                        8,
                                        hashsize( TABLE_HASH_EXPONENT ) );


/* Define required functions that are not intended to be inlined. */

void*
table_allocate_chunk( size_t chunkSize )
{
    void* ptr;
    assert( posix_memalign( &ptr, SCOREP_CACHELINESIZE, chunkSize ) == 0 );
    return ptr;
}

void
table_free_chunk( void* chunk )
{
}

table_value_t
table_value_ctor( table_key_t* key,
                  void*        ctorData )
{
    return *( table_value_t* )ctorData;
}
