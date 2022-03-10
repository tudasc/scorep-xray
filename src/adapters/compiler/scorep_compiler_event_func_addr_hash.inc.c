/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2021-2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#ifndef SCOREP_COMPILER_FUNC_ADDR_HASH_INC_C
#define SCOREP_COMPILER_FUNC_ADDR_HASH_INC_C

#include "scorep_compiler_func_addr_hash.h"
#include "scorep_compiler_demangle.h"

#include <SCOREP_Addr2line.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_FastHashtab.h>
#include <SCOREP_Filtering.h>

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include <jenkins_hash.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>


/* Hash table for compiler instrumentation address lookup. The
   hashtable has 512 buckets, each chunk contains up to 3 key-value
   pairs. The hash table starts empty and gets filled at
   enter-function events. */

typedef uintptr_t                     func_addr_hash_key_t;
typedef struct func_addr_hash_value_t func_addr_hash_value_t;
struct func_addr_hash_value_t
{
    SCOREP_RegionHandle region;
    uint16_t            so_token;
};

#define FUNC_ADDR_HASH_EXPONENT 9

static inline bool
func_addr_hash_equals( func_addr_hash_key_t key1, func_addr_hash_key_t key2 )
{
    return key1 == key2;
}

static void*
func_addr_hash_allocate_chunk( size_t chunkSize )
{
    /* We might enter this function from an outlined OpenMP function
       in between fork and team-begin. Several threads compete to
       enter first. If a non-master thread is first, there is no valid
       location object to be used in SCOREP_Memory_AlignedAllocForMisc
       and we abort with a TPD == 0 error. Thus, use plain malloc here
       and manually align to 64. */
    #define roundupto( x, to ) ( ( ( intptr_t )( x ) + ( ( intptr_t )( to ) - 1 ) ) & ~( ( intptr_t )( to ) - 1 ) )
    void* raw   = malloc( chunkSize + 64 );
    void* chunk = ( void* )roundupto( raw, 64 );
    #undef roundupto
    /* To free the allocated memory, we would need to track the 'raw'
       pointers. This would be possible, though as we need the chunks
       until the end of the program, this additional effort doesn't
       seem to be justified. */
    return chunk;
}

static void
func_addr_hash_free_chunk( void* chunk )
{
}

static func_addr_hash_value_t
func_addr_hash_value_ctor( func_addr_hash_key_t* addr,
                           const void*           ctorDataUnused )
{
    void*       so_handle_unused;
    const char* so_file_name_unused;
    uintptr_t   so_base_addr_unused;
    uint16_t    so_token;

    bool        scl_found;
    const char* file_name     = NULL;
    const char* function_name = NULL;
    unsigned    line_no       = SCOREP_INVALID_LINE_NO;

    SCOREP_Addr2line_LookupAddr( *addr,
                                 &so_handle_unused,
                                 &so_file_name_unused,
                                 &so_base_addr_unused,
                                 &so_token,
                                 &scl_found,
                                 &file_name,
                                 &function_name,
                                 &line_no );

    func_addr_hash_value_t value = { .region = SCOREP_FILTERED_REGION, .so_token = so_token };

    if ( scl_found )
    {
#if HAVE( PLATFORM_MAC )
        /* Skip these leading underscores on macOS */
        if ( '_' == scl_function_name[ 0 ] )
        {
            scl_function_name++;
        }
#endif  /*HAVE( PLATFORM_MAC )*/

        char* function_name_demangled;
        scorep_compiler_demangle( function_name, function_name_demangled );

        bool use_address = ( *addr != 0 );
        use_address &= ( strncmp( function_name_demangled, "POMP", 4 ) != 0 )
                       && ( strncmp( function_name_demangled, "Pomp", 4 ) != 0 )
                       && ( strncmp( function_name_demangled, "pomp", 4 ) != 0 )
                       && ( strncmp( function_name_demangled, "SCOREP_", 7 ) != 0 )
                       && ( strncmp( function_name_demangled, "scorep_", 7 ) != 0 )
                       && ( strncmp( function_name_demangled, "OTF2_", 5 ) != 0 )
                       && ( strncmp( function_name_demangled, "otf2_", 5 ) != 0 )
                       && ( strncmp( function_name_demangled, "cube_", 5 ) != 0 )
                       && ( strncmp( function_name_demangled, "cubew_", 6 ) != 0 )
                       && ( strncmp( function_name_demangled, ".omp.", 5 ) != 0 )
                       && ( strncmp( function_name_demangled, ".omp_outlined.", 14 ) != 0 )
                       && ( strncmp( function_name_demangled, ".nondebug_wrapper.", 18 ) != 0 )
                       && ( !strstr( function_name_demangled, "Kokkos::Tools" ) )
                       && ( !strstr( function_name_demangled, "Kokkos::Profiling" ) )
                       && ( !strstr( function_name_demangled, "6Kokkos5Tools" ) )
                       && ( !strstr( function_name_demangled, "6Kokkos9Profiling" )  );
        /* Usage of UTILS_IO_SimplifyPath on a copy of file_name not needed as libbfd lookup provides absolute paths. */
        use_address &= ( !SCOREP_Filtering_Match( file_name, function_name_demangled, function_name ) );
        if ( use_address )
        {
            SCOREP_SourceFileHandle file_handle = SCOREP_Definitions_NewSourceFile( file_name );
            value.region = SCOREP_Definitions_NewRegion( function_name_demangled,
                                                         function_name,
                                                         file_handle,
                                                         line_no,
                                                         SCOREP_INVALID_LINE_NO,
                                                         SCOREP_PARADIGM_COMPILER,
                                                         SCOREP_REGION_FUNCTION );
#if HAVE( UTILS_DEBUG )
            UTILS_DEBUG( "[table-insert] %ld used: %s(%s)@%s:%d",
                         addr, function_name_demangled, function_name, file_name, line_no );
        }
        else
        {
            UTILS_DEBUG( "[table-insert] %ld filtered: %s(%s)@%s:%d",
                         addr, function_name_demangled, function_name, file_name, line_no );
#endif      /* HAVE( SCOREP_DEBUG ) */
        }
        scorep_compiler_demangle_free( function_name, function_name_demangled );
    }
    return value;
}


static void
func_addr_hash_value_dtor( func_addr_hash_key_t   key,
                           func_addr_hash_value_t value )
{
}


static inline uint32_t
func_addr_hash_bucket_idx( func_addr_hash_key_t key )
{
    uint32_t hash = jenkins_hash( &key, sizeof( key ), 0 ) & hashmask( FUNC_ADDR_HASH_EXPONENT );
    return hash;
}

static void
func_addr_hash_iterate_key_value_pair( func_addr_hash_key_t   key,
                                       func_addr_hash_value_t value )
{
}

SCOREP_HASH_TABLE_NON_MONOTONIC( func_addr_hash, 3, hashsize( FUNC_ADDR_HASH_EXPONENT ) );

#undef FUNC_ADDR_HASH_EXPONENT


static inline bool
token_matches( func_addr_hash_key_t   key,
               func_addr_hash_value_t value,
               void*                  data )
{
    uint16_t dlclose_token = *( ( uint16_t* )data );
    if ( value.so_token == dlclose_token )
    {
        UTILS_DEBUG( "Remove %" PRIuPTR ": %s@%s:%d",
                     key,
                     SCOREP_RegionHandle_GetName( value.region ),
                     SCOREP_RegionHandle_GetFileName( value.region ),
                     SCOREP_RegionHandle_GetBeginLine( value.region ) );
        return true;
    }
    return false;
}


void
scorep_compiler_func_addr_hash_dlclose_cb( void*       soHandle,
                                           const char* soFileName,
                                           uintptr_t   soBaseAddr,
                                           uint16_t    soToken )
{
    UTILS_DEBUG( "Remove key-value pairs of dlclosed %s; token=%d", soFileName, soToken );
    func_addr_hash_remove_if( token_matches, ( void* )&soToken );
}


#endif /* SCOREP_COMPILER_FUNC_ADDR_HASH_INC_C */
