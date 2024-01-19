/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022, 2024,
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

typedef enum tool_event_t
{
    TOOL_EVENT_UNKNOWN = 0,

    TOOL_EVENT_PARALLEL,
    TOOL_EVENT_IMPLICIT_BARRIER,
    TOOL_EVENT_SINGLE,
    TOOL_EVENT_SINGLE_SBLOCK,
    TOOL_EVENT_TASK,
    TOOL_EVENT_TASK_UNTIED,
    TOOL_EVENT_TASK_CREATE,
    TOOL_EVENT_LOOP,
    TOOL_EVENT_SECTIONS,
    TOOL_EVENT_WORKSHARE,
    TOOL_EVENT_TASKWAIT,
    TOOL_EVENT_TASKGROUP,
    TOOL_EVENT_BARRIER,
    TOOL_EVENT_MASKED,
    TOOL_EVENT_CRITICAL,
    TOOL_EVENT_CRITICAL_SBLOCK,
    TOOL_EVENT_ORDERED,
    TOOL_EVENT_ORDERED_SBLOCK,
    TOOL_EVENT_SECTION,
    TOOL_EVENT_FLUSH,
#if 0
    TOOL_EVENT_TASKLOOP,
#endif
    TOOL_EVENTS,

    TOOL_EVENT_INVALID
} tool_event_t;


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
#define REGION_OMP_SECTION "!$omp section"
#define REGION_OMP_FLUSH "!$omp flush"

static region_fallback_t region_fallback[ TOOL_EVENTS ] =
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
    { REGION_OMP_SECTION,         sizeof( REGION_OMP_SECTION ) - 1,         SCOREP_REGION_SECTION,          SCOREP_INVALID_REGION },
    { REGION_OMP_FLUSH,           sizeof( REGION_OMP_FLUSH ) - 1,           SCOREP_REGION_FLUSH,            SCOREP_INVALID_REGION },

#if 0
    { "!$omp taskloop",        SCOREP_REGION_TASKLOOP,                SCOREP_INVALID_REGION },
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
#undef REGION_OMP_SECTION
#undef REGION_OMP_FLUSH

/* To match opari2's behavior, define lock regions once but use for all lock
   events. With codeptr_ra available, we could provide a link to the source,
   though. */
typedef enum tool_lock_event_t
{
    TOOL_LOCK_EVENT_INIT,
    TOOL_LOCK_EVENT_INIT_WITH_HINT,
    TOOL_LOCK_EVENT_DESTROY,
    TOOL_LOCK_EVENT_SET,
    TOOL_LOCK_EVENT_UNSET,
    TOOL_LOCK_EVENT_TEST,
    TOOL_LOCK_EVENT_INIT_NEST,
    TOOL_LOCK_EVENT_INIT_NEST_WITH_HINT,
    TOOL_LOCK_EVENT_DESTROY_NEST,
    TOOL_LOCK_EVENT_SET_NEST,
    TOOL_LOCK_EVENT_UNSET_NEST,
    TOOL_LOCK_EVENT_TEST_NEST,

    TOOL_LOCK_EVENTS,

    TOOL_LOCK_EVENT_INVALID
} tool_lock_event_t;


static SCOREP_RegionHandle     lock_regions[ TOOL_LOCK_EVENTS ];
static SCOREP_SourceFileHandle omp_file = SCOREP_INVALID_SOURCE_FILE;

void
init_region_fallbacks( void )
{
    /* Create regions up front without the need for synchronization, even if
       fallback- and lock-regions don't get used. */
    static bool initialized = false;
    if ( !initialized )
    {
        initialized = true;
        omp_file    = SCOREP_Definitions_NewSourceFile( "OMP" );
        for ( int i = 0; i < TOOL_EVENTS; i++ )
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

        char* lock_region_names[ TOOL_LOCK_EVENTS ] =
        {
            /* Like opari2, i.e., no leading !$ */
            "omp_init_lock",
            "omp_init_lock_with_hint",
            "omp_destroy_lock",
            "omp_set_lock",
            "omp_unset_lock",
            "omp_test_lock",
            "omp_init_nest_lock",
            "omp_init_nest_lock_with_hint",
            "omp_destroy_nest_lock",
            "omp_set_nest_lock",
            "omp_unset_nest_lock",
            "omp_test_nest_lock"
        };
        for ( int i = 0; i < TOOL_LOCK_EVENTS; i++ )
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
    uintptr_t    codeptr_ra;
    tool_event_t type;
} codeptr_key;

typedef codeptr_key         codeptr_hash_key_t;
typedef SCOREP_RegionHandle codeptr_hash_value_t;


/* *INDENT-OFF* */
static inline bool codeptr_hash_get_and_insert( codeptr_hash_key_t key, void* ctorData, codeptr_hash_value_t* value );
static inline bool codeptr_hash_get( codeptr_hash_key_t key, codeptr_hash_value_t* value );
//static inline bool codeptr_hash_remove( codeptr_hash_key_t key );
/* *INDENT-ON* */


static inline SCOREP_RegionHandle
get_region( const void*  codeptrRa,
            tool_event_t regionType )
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
    codeptr_hash_value_t region   = SCOREP_INVALID_REGION;
    bool                 inserted = codeptr_hash_get_and_insert( key, NULL, &region );

    UTILS_DEBUG( "[%s] codeptrRa %p | region_id %u | region_name %s | "
                 "canonical_name %s | file_name %s | type %d | line_no %d | inserted %s ",
                 UTILS_FUNCTION_NAME,
                 codeptrRa,
                 SCOREP_RegionHandle_GetId( region ),
                 SCOREP_RegionHandle_GetName( region ),
                 SCOREP_RegionHandle_GetCanonicalName( region ),
                 SCOREP_RegionHandle_GetFileName( region ),
                 SCOREP_RegionHandle_GetType( region ),
                 SCOREP_RegionHandle_GetEndLine( region ),
                 inserted == true ? "yes" : "no" );

    return region;
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
    uint16_t    so_token_unused;

    bool        scl_found_unused;
    const char* path                 = NULL;
    const char* function_name_unused = NULL;
    unsigned    line_no              = SCOREP_INVALID_LINE_NO;

    /* we are interested in file name and line number only. */
    SCOREP_Addr2line_LookupAddr( SCOREP_Addr2line_ConvertRetAddr2PrgCntAddr( key->codeptr_ra ),
                                 &so_handle_unused,
                                 &so_file_name_unused,
                                 &so_base_addr_unused,
                                 &so_token_unused,
                                 &scl_found_unused,
                                 &path,
                                 &function_name_unused,
                                 &line_no );

    SCOREP_RegionHandle     region    = SCOREP_INVALID_REGION;
    SCOREP_SourceFileHandle file      = omp_file;
    const char*             file_name = NULL;
    if ( path )
    {
        file      = SCOREP_Definitions_NewSourceFile( path );
        file_name = strrchr( path, '/' );
    }

    size_t type_strlen = region_fallback[ key->type ].name_strlen;
    if ( file_name )
    {
        file_name++;
        /* Create unique name '<type> @<file>:<lineno>' to distinguish regions.
           Filename and line number are only available if compiled with -g. */
        size_t file_strlen    = strlen( file_name );
        size_t line_no_strlen = 10;   /* UINT_MAX fits in 10 characters */

        const size_t append = 2 + file_strlen + 1 + line_no_strlen + 1;
        char         unique_name[ type_strlen + append ];
        memcpy( &unique_name[ 0 ], region_fallback[ key->type ].name, type_strlen );
        snprintf( &unique_name[ type_strlen ], append, " @%s:%u", file_name, line_no );

        region = SCOREP_Definitions_NewRegion( unique_name,
                                               unique_name,
                                               file,
                                               line_no,
                                               SCOREP_INVALID_LINE_NO,
                                               SCOREP_PARADIGM_OPENMP,
                                               region_fallback[ key->type ].type );
    }
    else
    {
        /* Create unique name '<type> @0x<addr>' to distinguish regions. */
        const size_t append = 21;
        char         unique_name[ type_strlen + append ];
        memcpy( &unique_name[ 0 ], region_fallback[ key->type ].name, type_strlen );
        snprintf( &unique_name[ type_strlen ], append, " @0x%08" PRIxPTR "", ( uintptr_t )key->codeptr_ra );

        region = SCOREP_Definitions_NewRegion( unique_name,
                                               unique_name,
                                               file,
                                               line_no,
                                               SCOREP_INVALID_LINE_NO,
                                               SCOREP_PARADIGM_OPENMP,
                                               region_fallback[ key->type ].type );
    }

    return region;
}


static inline uint32_t
codeptr_hash_bucket_idx( codeptr_hash_key_t key )
{
    uint32_t hash_value = jenkins_hash( &key.codeptr_ra, sizeof( key.codeptr_ra ), 0 );
    hash_value = jenkins_hash( &key.type, sizeof( key.type ), hash_value );
    return hash_value & hashmask( CODEPTR_HASH_EXPONENT );
}


SCOREP_HASH_TABLE_MONOTONIC( codeptr_hash, 6, hashsize( CODEPTR_HASH_EXPONENT ) );


void
scorep_ompt_codeptr_hash_dlclose_cb( void*       soHandle,
                                     const char* soFileName,
                                     uintptr_t   soBaseAddr,
                                     uint16_t    soToken )
{
    UTILS_WARNING( "Shared object %s was dlclosed. It's addresses are not "
                   "removed from the ompt address hash table. If another "
                   "shared object is dlopened and addressees are reused, the "
                   "ompt adapter might enter/exit regions referring to "
                   "wrong source code locations", soFileName );
}
