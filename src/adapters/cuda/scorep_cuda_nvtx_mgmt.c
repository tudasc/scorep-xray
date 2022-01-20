/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 */

#include <config.h>

#include "scorep_cuda_nvtx_mgmt.h"

#include <SCOREP_Definitions.h>
#include <SCOREP_FastHashtab.h>

#include <UTILS_Error.h>

#include <jenkins_hash.h>

#include <wchar.h>

/*************** Widestring table *********************************************/

typedef struct
{
    uint32_t       hash_value;
    const wchar_t* wide_string;
} widestring_table_key_t;
typedef SCOREP_StringHandle widestring_table_value_t;

#define WIDESTRING_TABLE_HASH_EXPONENT 8

static inline uint32_t
widestring_table_bucket_idx( widestring_table_key_t key )
{
    return key.hash_value & hashmask( WIDESTRING_TABLE_HASH_EXPONENT );
}

static inline bool
widestring_table_equals( widestring_table_key_t key1,
                         widestring_table_key_t key2 )
{
    return key1.hash_value == key2.hash_value && wcscmp( key1.wide_string, key2.wide_string ) == 0;
}

static inline void*
widestring_table_allocate_chunk( size_t chunkSize )
{
    return SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, chunkSize );
}

static inline void
widestring_table_free_chunk( void* chunk )
{
}

static void
widestring_to_string( size_t stringLength,
                      char*  string,
                      void*  arg )
{
    const wchar_t* wide            = arg;
    size_t         bytes_converted = wcstombs( string, wide, stringLength + 1 );
    // returns -1 iff an invalid wide char was encountered
    UTILS_BUG_ON( bytes_converted == ( size_t )( -1 ), "Invalid wide char conversion." );
}

static inline widestring_table_value_t
widestring_table_value_ctor( widestring_table_key_t* key,
                             void*                   ctorData )
{
    size_t              bytes_needed = wcstombs( NULL, key->wide_string, 0 );
    SCOREP_StringHandle new_string   = SCOREP_Definitions_NewStringGenerator( bytes_needed,
                                                                              widestring_to_string,
                                                                              ( void* )key->wide_string );

    size_t wide_size       = *( size_t* )ctorData;
    void*  copy_widestring = SCOREP_Memory_AllocForMisc( wide_size );
    memcpy( copy_widestring, key->wide_string, wide_size );
    key->wide_string = copy_widestring;

    return new_string;
}

/* nPairsPerChunk: 16+4 bytes per pair, 0 wasted bytes on x86-64 in 128 bytes */
SCOREP_HASH_TABLE_MONOTONIC( widestring_table,
                             6,
                             hashsize( WIDESTRING_TABLE_HASH_EXPONENT ) );

#undef WIDESTRING_TABLE_HASH_EXPONENT

/*************** Functions ****************************************************/

void
scorep_cuda_nvtx_init( void )
{
}

const char*
scorep_cuda_nvtx_unicode_to_ascii( const wchar_t* wide )
{
    size_t                 bytes = ( wcslen( wide ) + 1 ) * sizeof( wchar_t );
    widestring_table_key_t key   = {
        .hash_value  = jenkins_hash( wide, bytes, 0 ),
        .wide_string = wide
    };

    bool                ignored;
    SCOREP_StringHandle string = widestring_table_get_and_insert( key, &bytes, &ignored );

    return SCOREP_StringHandle_Get( string );
}
