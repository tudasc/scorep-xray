/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */


#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <SCOREP_Addr2line.h>
#include <SCOREP_FastHashtab.h>
#include <SCOREP_ThreadForkJoin_Event.h>
#include <jenkins_hash.h>

typedef enum ompt_region_type
{
    OMPT_UNKNOWN = 0,

    OMPT_PARALLEL,
    OMPT_IMPLICIT_BARRIER,
    OMPT_SINGLE,
    OMPT_SINGLE_SBLOCK,
    OMPT_TASK,
    OMPT_TASK_UNTIED,
    OMPT_TASK_CREATE,
    OMPT_LOOP,
    OMPT_SECTIONS,
    OMPT_WORKSHARE,
    OMPT_TASKWAIT,
    OMPT_TASKGROUP,
    OMPT_BARRIER,
    OMPT_MASKED,
    OMPT_CRITICAL,
    OMPT_CRITICAL_SBLOCK,
    OMPT_ORDERED,
    OMPT_ORDERED_SBLOCK,
#if 0
    OMPT_SECTION,
    OMPT_TASKLOOP,
    OMPT_FLUSH,
#endif
    OMPT_REGIONS,

    OMPT_INVALID
} ompt_region_type;


typedef struct region_fallback_t
{
    const char*         name;
    size_t              name_strlen;
    SCOREP_RegionType   type;
    SCOREP_RegionHandle handle;
} region_fallback_t;


#define REGION_OMP_UNKNOWN "!$omp unknown"
#define REGION_OMP_PARALLEL "!$omp parallel"
/* Note: OpenMP 5.1+ allows us to distinguish between several barrier types */
#define REGION_OMP_IBARRIER "!$omp implicit barrier"
#define REGION_OMP_SINGLE "!$omp single"
#define REGION_OMP_SINGLE_SBLOCK "!$omp single sblock"
#define REGION_OMP_TASK "!$omp task"
#define REGION_OMP_TASK_UNTIED "!$omp task untied"
#define REGION_OMP_CREATE_TASK "!$omp create task"
#define REGION_OMP_LOOP "!$omp for/do"
#define REGION_OMP_SECTIONS "!$omp sections"
#define REGION_OMP_WORKSHARE "!$omp workshare"
#define REGION_OMP_TASKWAIT "!$omp taskwait"
#define REGION_OMP_TASKGROUP "!$omp taskgroup"
#define REGION_OMP_BARRIER "!$omp barrier"
#define REGION_OMP_MASKED "!$omp masked"
#define REGION_OMP_CRITICAL "!$omp critical"
#define REGION_OMP_CRITICAL_SBLOCK "!$omp critical sblock"
#define REGION_OMP_ORDERED "!$omp ordered"
#define REGION_OMP_ORDERED_SBLOCK "!$omp ordered sblock"

static region_fallback_t region_fallback[ OMPT_REGIONS ] =
{
    /* uncrustify adds unnecessary whitespace at every invocation */
    /* *INDENT-OFF* */
    { REGION_OMP_UNKNOWN,         sizeof( REGION_OMP_UNKNOWN ) - 1,         SCOREP_REGION_UNKNOWN,          SCOREP_INVALID_REGION },
    { REGION_OMP_PARALLEL,        sizeof( REGION_OMP_PARALLEL ) - 1,        SCOREP_REGION_PARALLEL,         SCOREP_INVALID_REGION },
    { REGION_OMP_IBARRIER,        sizeof( REGION_OMP_IBARRIER ) - 1,        SCOREP_REGION_IMPLICIT_BARRIER, SCOREP_INVALID_REGION },
    { REGION_OMP_SINGLE,          sizeof( REGION_OMP_SINGLE ) - 1,          SCOREP_REGION_SINGLE,           SCOREP_INVALID_REGION },
    { REGION_OMP_SINGLE_SBLOCK,   sizeof( REGION_OMP_SINGLE_SBLOCK ) - 1,   SCOREP_REGION_SINGLE_SBLOCK,    SCOREP_INVALID_REGION },
    { REGION_OMP_TASK,            sizeof( REGION_OMP_TASK ) - 1,            SCOREP_REGION_TASK,             SCOREP_INVALID_REGION },
    { REGION_OMP_TASK_UNTIED,     sizeof( REGION_OMP_TASK_UNTIED ) - 1,     SCOREP_REGION_TASK_UNTIED,      SCOREP_INVALID_REGION },
    { REGION_OMP_CREATE_TASK,     sizeof( REGION_OMP_CREATE_TASK ) - 1,     SCOREP_REGION_TASK_CREATE,      SCOREP_INVALID_REGION },
    { REGION_OMP_LOOP,            sizeof( REGION_OMP_LOOP ) - 1,            SCOREP_REGION_LOOP,             SCOREP_INVALID_REGION },
    { REGION_OMP_SECTIONS,        sizeof( REGION_OMP_SECTIONS ) - 1,        SCOREP_REGION_SECTIONS,         SCOREP_INVALID_REGION },
    { REGION_OMP_WORKSHARE,       sizeof( REGION_OMP_WORKSHARE ) - 1,       SCOREP_REGION_WORKSHARE,        SCOREP_INVALID_REGION },
    { REGION_OMP_TASKWAIT,        sizeof( REGION_OMP_TASKWAIT ) - 1,        SCOREP_REGION_BARRIER,          SCOREP_INVALID_REGION },
    { REGION_OMP_TASKGROUP,       sizeof( REGION_OMP_TASKGROUP ) - 1,       SCOREP_REGION_BARRIER,          SCOREP_INVALID_REGION },
    { REGION_OMP_BARRIER,         sizeof( REGION_OMP_BARRIER ) - 1,         SCOREP_REGION_BARRIER,          SCOREP_INVALID_REGION },
    { REGION_OMP_MASKED,          sizeof( REGION_OMP_MASKED ) - 1,          SCOREP_REGION_MASTER,           SCOREP_INVALID_REGION },
    { REGION_OMP_CRITICAL,        sizeof( REGION_OMP_CRITICAL ) - 1,        SCOREP_REGION_CRITICAL,         SCOREP_INVALID_REGION },
    { REGION_OMP_CRITICAL_SBLOCK, sizeof( REGION_OMP_CRITICAL_SBLOCK ) - 1, SCOREP_REGION_CRITICAL_SBLOCK,  SCOREP_INVALID_REGION },
    { REGION_OMP_ORDERED,         sizeof( REGION_OMP_ORDERED ) - 1,         SCOREP_REGION_ORDERED,          SCOREP_INVALID_REGION },
    { REGION_OMP_ORDERED_SBLOCK,  sizeof( REGION_OMP_ORDERED_SBLOCK ) - 1,  SCOREP_REGION_ORDERED_SBLOCK,   SCOREP_INVALID_REGION },

#if 0
    { "!$omp section",         SCOREP_REGION_SECTION,                 SCOREP_INVALID_REGION },
    { "!$omp taskloop",        SCOREP_REGION_TASKLOOP,                SCOREP_INVALID_REGION },
    { "!$omp flush",           SCOREP_REGION_FLUSH,                   SCOREP_INVALID_REGION },
    /* TODO barrier does not really fit for taskgroup (not handled by opari2). Invent something new? */
#endif
    /* *INDENT-ON* */
};

#undef REGION_OMP_UNKNOWN
#undef REGION_OMP_PARALLEL
#undef REGION_OMP_IBARRIER
#undef REGION_OMP_SINGLE
#undef REGION_OMP_SINGLE_SBLOCK
#undef REGION_OMP_TASK
#undef REGION_OMP_TASK_UNTIED
#undef REGION_OMP_CREATE_TASK
#undef REGION_OMP_LOOP
#undef REGION_OMP_SECTIONS
#undef REGION_OMP_WORKSHARE
#undef REGION_OMP_TASKWAIT
#undef REGION_OMP_TASKGROUP
#undef REGION_OMP_BARRIER
#undef REGION_OMP_MASKED
#undef REGION_OMP_CRITICAL
#undef REGION_OMP_CRITICAL_SBLOCK
#undef REGION_OMP_ORDERED
#undef REGION_OMP_ORDERED_SBLOCK


/* To match opari2's behavior, define lock regions once but use for all lock
   events. With codeptr_ra available, we could provide a link to the source,
   though. */
typedef enum ompt_lock_region_type
{
    OMPT_LOCK_INIT,
    OMPT_LOCK_INIT_WITH_HINT,
    OMPT_LOCK_DESTROY,
    OMPT_LOCK_SET,
    OMPT_LOCK_UNSET,
    OMPT_LOCK_INIT_NEST,
    OMPT_LOCK_INIT_NEST_WITH_HINT,
    OMPT_LOCK_DESTROY_NEST,
    OMPT_LOCK_SET_NEST,
    OMPT_LOCK_UNSET_NEST,

    /* omp_test_lock and omp_test_nest_lock are missing. For them, we get
       acquire/acquired/released if we obtain the lock or just acquire if the
       lock was already held. In the latter case, I don't see a way to trigger
       the exit(test_[nest_]lock) that corresponds to the acquire's
       enter(test_[nest_lock). */

    OMPT_LOCK_REGIONS,

    OMPT_LOCK_INVALID
} ompt_lock_region_type;


static SCOREP_RegionHandle lock_regions[ OMPT_LOCK_REGIONS ];


void
init_region_fallbacks( void )
{
    /* Create regions up front without the need for synchronization, even if
       fallback- and lock-regions don't get used. */
    static bool initialized = false;
    if ( !initialized )
    {
        initialized = true;
        SCOREP_SourceFileHandle omp_file = SCOREP_Definitions_NewSourceFile( "OMP" );
        for ( int i = 0; i < OMPT_REGIONS; i++ )
        {
            region_fallback[ i ].handle =
                SCOREP_Definitions_NewRegion( region_fallback[ i ].name,
                                              NULL,
                                              omp_file,
                                              SCOREP_INVALID_LINE_NO,
                                              SCOREP_INVALID_LINE_NO,
                                              SCOREP_PARADIGM_OPENMP,
                                              region_fallback[ i ].type );
        }

        char* lock_region_names[ OMPT_LOCK_REGIONS ] =
        {
            /* Like opari2, i.e., no leading !$ */
            "omp_init_lock",
            "omp_init_lock_with_hint",
            "omp_destroy_lock",
            "omp_set_lock",
            "omp_unset_lock",
            "omp_init_nest_lock",
            "omp_init_nest_lock_with_hint",
            "omp_destroy_nest_lock",
            "omp_set_nest_lock",
            "omp_unset_nest_lock"
        };
        for ( int i = 0; i < OMPT_LOCK_REGIONS; i++ )
        {
            lock_regions[ i ] =
                SCOREP_Definitions_NewRegion( lock_region_names[ i ],
                                              NULL,
                                              omp_file,
                                              SCOREP_INVALID_LINE_NO,
                                              SCOREP_INVALID_LINE_NO,
                                              SCOREP_PARADIGM_OPENMP,
                                              SCOREP_REGION_WRAPPER );
        }
    }
}

typedef struct codeptr_key
{
    uintptr_t        codeptr_ra;
    ompt_region_type type;
} codeptr_key;

typedef struct codeptr_value
{
    SCOREP_RegionHandle region;
    uint16_t            so_token;
} codeptr_value;

typedef codeptr_key   codeptr_hash_key_t;
typedef codeptr_value codeptr_hash_value_t;


/* *INDENT-OFF* */
static inline bool codeptr_hash_get_and_insert( codeptr_hash_key_t key, void* ctorData, codeptr_hash_value_t* value );
static inline bool codeptr_hash_get( codeptr_hash_key_t key, codeptr_hash_value_t* value );
//static inline bool codeptr_hash_remove( codeptr_hash_key_t key );
/* *INDENT-ON* */


static inline SCOREP_RegionHandle
get_region( const void*      codeptrRa,
            ompt_region_type regionType )
{
    if ( !codeptrRa )
    {
        SCOREP_RegionHandle region = region_fallback[ regionType ].handle;
        UTILS_DEBUG( "[%s] (fallback) codeptrRa == NULL | region_id %u | region_name %s | type %d",
                     UTILS_FUNCTION_NAME, SCOREP_RegionHandle_GetId( region ),
                     SCOREP_RegionHandle_GetName( region ), SCOREP_RegionHandle_GetType( region ) );
        return region;
    }

    codeptr_hash_key_t   key      = { .codeptr_ra = ( uintptr_t )codeptrRa, .type = regionType };
    codeptr_hash_value_t value    = { .region = SCOREP_INVALID_REGION, .so_token = 0 };
    bool                 inserted = codeptr_hash_get_and_insert( key, NULL, &value );

    UTILS_DEBUG( "[%s] codeptrRa %p | region_id %u | region_name %s | "
                 "canonical_name %s | file_name %s | type %d | line_no %d | inserted %s ",
                 UTILS_FUNCTION_NAME,
                 codeptrRa,
                 SCOREP_RegionHandle_GetId( value.region ),
                 SCOREP_RegionHandle_GetName( value.region ),
                 SCOREP_RegionHandle_GetCanonicalName( value.region ),
                 SCOREP_RegionHandle_GetFileName( value.region ),
                 SCOREP_RegionHandle_GetType( value.region ),
                 SCOREP_RegionHandle_GetEndLine( value.region ),
                 inserted == true ? "yes" : "no" );

    return value.region;
}


/* Hash table for (codeptr_ra, type)->(region_handle, so_token) lookup.
   The hashtable has 256 buckets, each chunk contains up to 2 key-value
   pairs. */

#define CODEPTR_HASH_EXPONENT 8


static inline bool
codeptr_hash_equals( codeptr_hash_key_t key1,
                     codeptr_hash_key_t key2 )
{
    return ( key1.type == key2.type ) && ( key1.codeptr_ra == key2.codeptr_ra );
}

static void*
codeptr_hash_allocate_chunk( size_t chunkSize )
{
    /* We might enter this function from a (device) callback on a thread that
       has (intentionally) no location associated. */
    void* chunk = SCOREP_Memory_AlignedMalloc( SCOREP_CACHELINESIZE, chunkSize );
    UTILS_BUG_ON( chunk == 0 );
    return chunk;
}

static void
codeptr_hash_free_chunk( void* chunk )
{
    SCOREP_Memory_AlignedFree( chunk );
}

static codeptr_hash_value_t
codeptr_hash_value_ctor( codeptr_hash_key_t* key,
                         const void*         ctorData )
{
    void*       so_handle_unused;
    const char* so_file_name_unused;
    uintptr_t   so_base_addr_unused;
    uint16_t    so_token;

    bool        scl_found_unused;
    const char* file_name            = NULL;
    const char* function_name_unused = NULL;
    unsigned    line_no              = SCOREP_INVALID_LINE_NO;

    /* we are interested in file name and line number only. */
    SCOREP_Addr2line_LookupAddr( SCOREP_Addr2line_ConvertRetAddr2PrgCntAddr( key->codeptr_ra ),
                                 &so_handle_unused,
                                 &so_file_name_unused,
                                 &so_base_addr_unused,
                                 &so_token,
                                 &scl_found_unused,
                                 &file_name,
                                 &function_name_unused,
                                 &line_no );

    SCOREP_SourceFileHandle omp_file = SCOREP_INVALID_SOURCE_FILE;
    if ( file_name )
    {
        omp_file = SCOREP_Definitions_NewSourceFile( file_name );
    }

    /* Create unique name '<type>@0x<addr>' to distinguish regions.
       Additional distinguishable elements such as line number and filename are
       only available if compiled with -g. */
    size_t       type_strlen = region_fallback[ key->type ].name_strlen;
    const size_t append      = 21;
    char         unique_name[ type_strlen + append ];
    memcpy( &unique_name[ 0 ], region_fallback[ key->type ].name, type_strlen );
    snprintf( &unique_name[ type_strlen ], append, " @0x%016" PRIxPTR "", ( uintptr_t )key->codeptr_ra );
    unique_name[ type_strlen + append - 1 ] = '\0';

    SCOREP_RegionHandle region =
        SCOREP_Definitions_NewRegion( unique_name, /* Used in CubeGUI for matching between
                                                      calltree tabs. Needs to have a
                                                      'unique' feature like <addr> or
                                                      <file>:<lineno>. */
                                      unique_name,
                                      omp_file,
                                      SCOREP_INVALID_LINE_NO,
                                      line_no,
                                      SCOREP_PARADIGM_OPENMP,
                                      region_fallback[ key->type ].type );

    codeptr_hash_value_t value = { .region = region, .so_token = so_token };
    return value;
}

static inline void
codeptr_hash_value_dtor( codeptr_hash_key_t key, codeptr_hash_value_t value )
{
}

static inline uint32_t
codeptr_hash_bucket_idx( codeptr_hash_key_t key )
{
    uint32_t hash_value = jenkins_hash( &key.codeptr_ra, sizeof( key.codeptr_ra ), 0 );
    hash_value = jenkins_hash( &key.type, sizeof( key.type ), hash_value );
    return hash_value & hashmask( CODEPTR_HASH_EXPONENT );
}


SCOREP_HASH_TABLE_NON_MONOTONIC( codeptr_hash, 2, hashsize( CODEPTR_HASH_EXPONENT ) );


static inline bool
token_matches( codeptr_hash_key_t   key,
               codeptr_hash_value_t value,
               void*                data )
{
    uint16_t dlclose_token = *( ( uint16_t* )data );
    if ( value.so_token == dlclose_token )
    {
        UTILS_DEBUG( "Remove %" PRIuPTR ":%d %s@%s:%d",
                     key.codeptr_ra, key.type,
                     SCOREP_RegionHandle_GetName( value.region ),
                     SCOREP_RegionHandle_GetFileName( value.region ),
                     SCOREP_RegionHandle_GetEndLine( value.region ) );
        return true;
    }
    return false;
}


void
scorep_ompt_codeptr_hash_dlclose_cb( void*       soHandle,
                                     const char* soFileName,
                                     uintptr_t   soBaseAddr,
                                     uint16_t    soToken )
{
    UTILS_DEBUG( "Remove key-value pairs of dlclosed %s; token=%d", soFileName, soToken );
    codeptr_hash_remove_if( token_matches, ( void* )&soToken );
}
