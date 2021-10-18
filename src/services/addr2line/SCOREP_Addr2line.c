/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2021,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 *
 * @brief Implements addr2line by matching given addresses with
 * loadtime- (lt_) and runtime-loaded (rt_) shared objects and
 * corresponding libbfd images. Shared objects are obtained via
 * dl_iterate_phdr; the loadtime ones at measurement initialization
 * and the runtime ones via rtdl-audit callbacks.
 */

#include <config.h>

#include <SCOREP_Addr2line.h>

#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <link.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Memory.h>

#include <UTILS_Error.h>
#include <UTILS_CStr.h>

#define SCOREP_DEBUG_MODULE_NAME ADDR2LINE
#include <UTILS_Debug.h>

#ifndef PACKAGE
/* Some bfd.h installations require a PACKAGE or PACKAGE_VERSION to be
   defined, see https://sourceware.org/bugzilla/show_bug.cgi?id=14243
   and https://sourceware.org/bugzilla/show_bug.cgi?id=15920 */
#define PACKAGE
#endif /* ! PACKAGE */
#include <bfd.h>
/* Deal with bfd API changes since binutils 2.34 */
#ifndef bfd_get_section_size
#define bfd_get_section_size( asection ) bfd_section_size( asection )
#endif /* ! bfd_get_section_size */
#ifndef bfd_get_section_vma
#define bfd_get_section_vma( abfd, asection ) bfd_section_vma( asection )
#endif /* ! bfd_get_section_vma */
#ifndef bfd_get_section_flags
#define bfd_get_section_flags( abfd, asection ) bfd_section_flags( asection )
#endif /* ! bfd_get_section_flags */


/* *INDENT-OFF* */
static int count_shared_objs( struct dl_phdr_info* info, size_t unused, void* cnt );
static bool is_obj_relevant( const char* name );
static bool iterate_segments( struct dl_phdr_info* info, const char** name, uintptr_t* baseAddr, bfd** abfd, asymbol*** symbols, uintptr_t* beginAddrMin, uintptr_t* endAddrMax, bool adjustName );
static int fill_lt_arrays_cb( struct dl_phdr_info* info, size_t unused, void* cnt );
static void init_abfd( const char* name, bfd** abfd, asymbol*** symbols, long* nSyms );
/* *INDENT-ON* */


/* Loadtime-loaded shared objects correspond to two sorted arrays for
   fast and concurrent lookup. The first one contains just the start
   addresses where the second ones contains the remaining
   metadata. The capacity of the array might be larger than the actual
   element count.
   We bfd-lookup (addr - base_addr) if addr in [begin_addr, end_addr]. */
static size_t lt_objs_capacity;    /* upper bound of loadtime objects */
static size_t     lt_object_count; /* number of loadtime objects used */
static uintptr_t* lt_begin_addrs;
#define SO_OBJECT_COMMON \
    uintptr_t end_addr; \
    uintptr_t   base_addr; \
    bfd*        abfd; \
    asymbol**   symbols; \
    const char* name; \
    uint16_t    token
typedef struct lt_object lt_object;
struct lt_object
{
    SO_OBJECT_COMMON;
};
static lt_object* lt_objects;

static bool addr2line_initialized;

void
SCOREP_Addr2line_Initialize( void )
{
    UTILS_DEBUG_ENTRY();
    if ( addr2line_initialized )
    {
        return;
    }
    addr2line_initialized = true;

    /* get upper bound of relevant loadtime shared objects */
    dl_iterate_phdr( count_shared_objs, ( void* )&lt_objs_capacity );
    UTILS_DEBUG( "lt_objs_capacity=%zu", lt_objs_capacity );

    lt_begin_addrs = SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE,
                                                        lt_objs_capacity * sizeof( uintptr_t ) );
    memset( lt_begin_addrs, 0, lt_objs_capacity * sizeof( uintptr_t ) );
    lt_objects = calloc( lt_objs_capacity, sizeof( lt_object ) );
    UTILS_BUG_ON( !lt_begin_addrs || !lt_objects );

    unsigned init_result = bfd_init();

    /* fill arrays but ignore shared objects that don't report any symbols */
    dl_iterate_phdr( fill_lt_arrays_cb, ( void* )&lt_object_count );
    UTILS_BUG_ON( lt_object_count > lt_objs_capacity,
                  "Actual count must not exceed capacity." );

    UTILS_DEBUG_EXIT( "lt_object_count=%zu; bfd_init=%d",
                      lt_object_count, init_result );
}


static int
count_shared_objs( struct dl_phdr_info* info, size_t unused, void* cnt )
{
    if ( !is_obj_relevant( info->dlpi_name ) )
    {
        return 0;
    }
    /* Consider objects that have at least one loadable (PT_LOAD) and
       readable (PF_R) program segment. */
    for ( int i = 0; i < info->dlpi_phnum; i++ )
    {
        if ( info->dlpi_phdr[ i ].p_type == PT_LOAD
             && info->dlpi_phdr[ i ].p_flags & PF_R )
        {
            ++*( size_t* )cnt;
            return 0;
        }
    }
    return 0;
}


static bool
is_obj_relevant( const char* name )
{
    if ( strstr( name, "linux-vdso" )
         || strstr( name, "libscorep" )
         || strstr( name, "libcube4w" )
         || strstr( name, "libotf2" )
         /* Don't be too aggressive about excluding things
            1. Initial parsing time not relevant
            2. Space only relevant if library with line info is never queried
            3. Additional addr-range lookup time negligible
            4. "You think you want source for this address, but you
               don't." might have no use case */
         )
    {
        return false;
        UTILS_DEBUG( "'%s' not relevant", name );
    }
    return true;
}


#define SCOREP_ADDR2LINE_LT_OBJECT_TOKEN UINT16_MAX


static int
fill_lt_arrays_cb( struct dl_phdr_info* info, size_t unused, void* cnt )
{
    if ( !is_obj_relevant( info->dlpi_name ) )
    {
        return 0;
    }

    /* a dl_phdr_info object has one corresponding abfd, symbols, and
       baseAddr, and potentially several beginAddr/endAddr pairs.
       Later on, we will look up every address that is in the range
       [begin_addr_min, end_addr_max] */
    const char* name           = NULL;
    uintptr_t   base_addr      = 0;
    bfd*        abfd           = NULL;
    asymbol**   symbols        = NULL;
    uintptr_t   begin_addr_min = UINTPTR_MAX;
    uintptr_t   end_addr_max   = 0;
    bool        has_symbols    = iterate_segments( info,
                                                   &name,
                                                   &base_addr,
                                                   &abfd,
                                                   &symbols,
                                                   &begin_addr_min,
                                                   &end_addr_max,
                                                   true /* adjustName */ );
    if ( !has_symbols )
    {
        return 0;
    }

    /* add shared object representation to array. keep arrays sorted by
       begin-address */
    size_t insert = ( *( size_t* )cnt )++;
    for ( size_t j = insert; j-- > 0; )
    {
        if ( lt_begin_addrs[ j ] > begin_addr_min )
        {
            lt_begin_addrs[ j + 1 ] = lt_begin_addrs[ j ];
            lt_objects[ j + 1 ]     = lt_objects[ j ];
            insert                  = j;
        }
        else
        {
            break;
        }
    }

    lt_begin_addrs[ insert ]       = begin_addr_min;
    lt_objects[ insert ].end_addr  = end_addr_max;
    lt_objects[ insert ].base_addr = base_addr;
    lt_objects[ insert ].abfd      = abfd;
    lt_objects[ insert ].symbols   = symbols;
    lt_objects[ insert ].name      = name;
    lt_objects[ insert ].token     = SCOREP_ADDR2LINE_LT_OBJECT_TOKEN;

    UTILS_DEBUG( "Use %s: base=%" PRIuPTR "; begin=%" PRIuPTR "; end=%" PRIuPTR "",
                 name, base_addr, begin_addr_min, end_addr_max );
    return 0;
}


static bool
iterate_segments( struct dl_phdr_info* info,
                  const char**         name,
                  uintptr_t*           baseAddr,
                  bfd**                abfd,
                  asymbol***           symbols,
                  uintptr_t*           beginAddrMin,
                  uintptr_t*           endAddrMax,
                  bool                 adjustName )
{
    *name     = info->dlpi_name;
    *baseAddr = ( uintptr_t )info->dlpi_addr;
    /* iterate over segments to find all loadable program segments (PT_LOAD)
       ones that are readable (PF_R) */
    for ( int i = 0; i < info->dlpi_phnum; i++ )
    {
        if ( info->dlpi_phdr[ i ].p_type == PT_LOAD
             && info->dlpi_phdr[ i ].p_flags & PF_R )
        {
            if ( !*abfd )
            {
                /* A pathname is needed to open a bfd. */
                UTILS_BUG_ON( !*name, "Valid name form dl_phdr_info expected." );

                bool is_executable = false;
                if ( adjustName && *name[ 0 ] == '\0' )
                {
                    /* As dl_iterate_phdr returns "" for the executable itself,
                       we need to acquire the executable name. */
                    bool unused;
                    *name         = SCOREP_GetExecutableName( &unused );
                    is_executable = true;
                }

                long n_syms = 0; /* bfd_canonicalize_symtab returns long */
                init_abfd( *name, abfd, symbols, &n_syms );
                /* objects that don't report symbols are ignored */
                if ( n_syms < 1 )
                {
                    return false;
                }

                if ( adjustName && !is_executable )
                {
                    *name = UTILS_CStr_dup( info->dlpi_name );
                }
            }
            uintptr_t begin_addr = *baseAddr + ( uintptr_t )info->dlpi_phdr[ i ].p_vaddr;
            if ( begin_addr < *beginAddrMin )
            {
                *beginAddrMin = begin_addr;
            }
            uintptr_t end_addr = begin_addr + ( uintptr_t )info->dlpi_phdr[ i ].p_memsz;
            if ( end_addr > *endAddrMax )
            {
                *endAddrMax = end_addr;
            }
        }
    }
    if ( !*abfd )
    {
        UTILS_WARNING( "No readable PT_LOAD segment found for '%s'. "
                       "Is this supposed to happen?",
                       info->dlpi_name );
        return false;
    }
    UTILS_BUG_ON( *beginAddrMin > *endAddrMax );

    /* The memory range [ beginAddrMin, endAddrMax ] might contain segments
       that are not of type PT_LOAD and/or that are not readable (?). Would a
       bfd lookup into these parts of the memory range do any harm? If yes, we
       would need to track begin/end of the individual readable PT_LOAD
       segments. */

    return true;
}


static void
init_abfd( const char* name, bfd** abfd, asymbol*** symbols, long* nSyms )
{
    *abfd = bfd_openr( name, NULL );
    if ( !*abfd )
    {
        UTILS_DEBUG( "Could not bfd-open %s", name );
        return;
    }
    if ( !bfd_check_format( *abfd, bfd_object ) )
    {
        UTILS_DEBUG( "abfd of %s not of type bfd_object", name );
        bfd_close( *abfd ); /* returns bool */
        return;
    }
    if ( !( bfd_get_file_flags( *abfd ) & HAS_SYMS ) )
    {
        UTILS_DEBUG( "abfd of %s has no symbols", name );
        bfd_close( *abfd );
        return;
    }
    long upper_bound = bfd_get_symtab_upper_bound( *abfd );
    if ( upper_bound < 1 )
    {
        UTILS_DEBUG( "abfd of %s reports an symbol upper bound of %ld",
                     name, upper_bound );
        bfd_close( *abfd );
        return;
    }
    *symbols = malloc( upper_bound );
    if ( !*symbols )
    {
        UTILS_BUG( "Could not allocate symbols for abfd of %s", name );
        bfd_close( *abfd );
        return;
    }
    *nSyms = bfd_canonicalize_symtab( *abfd, *symbols );
    if ( *nSyms < 1 )
    {
        UTILS_DEBUG( "No symbols in abfd of %s (n_syms=%ld)", name, *nSyms );
        free( *symbols );
        bfd_close( *abfd );
        return;
    }
    else
    {
        UTILS_DEBUG( "Read %ld symbols for abfd of %s (upper_bound=%ld)",
                     *nSyms, name, upper_bound );
    }
}


/* data needed for iterating bfd sections, i.e. the actual source code
   location lookup. */
typedef struct lookup_bfd_t lookup_bfd_t;
struct lookup_bfd_t
{
    uintptr_t begin_addr;
    uintptr_t end_addr;
    asymbol** symbols;

    /* OUT members */
    bool*         scl_found_begin_addr;
    bool*         scl_found_end_addr;
    const char**  scl_file_name;
    const char**  scl_function_name;
    unsigned int* scl_begin_lno;
    unsigned int* scl_end_lno;
};


/* *INDENT-OFF* */
static lt_object* lookup_so( uintptr_t addr );
static void section_iterator( bfd* abfd, asection* section, void* payload );
/* *INDENT-ON* */


void
SCOREP_Addr2line_LookupSo( uintptr_t    programCounterAddr,
                           /* shared object OUT parameters */
                           void**       soHandle,
                           const char** soFileName,
                           uintptr_t*   soBaseAddr,
                           uint16_t*    soToken )
{
    UTILS_BUG_ON( soHandle == NULL ||
                  soFileName == NULL ||
                  soBaseAddr == NULL ||
                  soToken == NULL,
                  "Need valid OUT handles but NULL provided." );
    lt_object* handle = lookup_so( programCounterAddr );
    *soHandle = handle;
    *soToken  = SCOREP_ADDR2LINE_SO_OBJECT_INVALID_TOKEN;
    if ( handle )
    {
        *soFileName = handle->name;
        *soBaseAddr = handle->base_addr;
        *soToken    = handle->token;
    }
}


void
SCOREP_Addr2line_SoLookupAddr( uintptr_t    offset,
                               void*        soHandle,
                               /* shared object OUT parameters */
                               const char** soFileName,
                               uint16_t*    soToken,
                               /* Source code location OUT parameters */
                               bool*        sclFound,
                               const char** sclFileName,
                               const char** sclFunctionName,
                               unsigned*    sclLineNo )
{
    UTILS_BUG_ON( soHandle == NULL, "Need valid soHandle but NULL provided" );
    UTILS_BUG_ON( soFileName == NULL ||
                  soToken == NULL ||
                  sclFound == NULL ||
                  sclFileName == NULL ||
                  sclFunctionName == NULL ||
                  sclLineNo == NULL,
                  "Need valid OUT handles but NULL provided." );
    lt_object* so_handle = ( lt_object* )soHandle;

    *soFileName = so_handle->name;
    *soToken    = so_handle->token;

    bool         scl_found_unused;
    lookup_bfd_t data = {
        .begin_addr           = offset,
        .end_addr             = 0,
        .symbols              = so_handle->symbols,
        .scl_found_begin_addr = sclFound,
        .scl_found_end_addr   = &scl_found_unused,
        .scl_file_name        = sclFileName,
        .scl_function_name    = sclFunctionName,
        .scl_begin_lno        = sclLineNo,
        .scl_end_lno          = NULL
    };
    *( data.scl_found_begin_addr ) = false;
    *( data.scl_found_end_addr )   = false;

    bfd_map_over_sections( so_handle->abfd, section_iterator, &data );
}


void
SCOREP_Addr2line_SoLookupAddrRange( uintptr_t    beginOffset,
                                    uintptr_t    endOffset,
                                    void*        soHandle,
                                    /* shared object OUT parameters */
                                    const char** soFileName,
                                    uint16_t*    soToken,
                                    /* Source code location OUT parameters */
                                    bool*        sclFoundBegin,
                                    bool*        sclFoundEnd,
                                    const char** sclFileName,
                                    const char** sclFunctionName,
                                    unsigned*    sclBeginLineNo,
                                    unsigned*    sclEndLineNo )
{
    UTILS_BUG_ON( soHandle == NULL, "Need valid soHandle but NULL provided" );
    UTILS_BUG_ON( soFileName == NULL ||
                  soToken == NULL ||
                  sclFoundBegin == NULL ||
                  sclFoundEnd == NULL ||
                  sclFileName == NULL ||
                  sclFunctionName == NULL ||
                  sclBeginLineNo == NULL ||
                  sclEndLineNo == NULL,
                  "Need valid OUT handles but NULL provided." );
    lt_object* so_handle = ( lt_object* )soHandle;

    *soFileName = so_handle->name;
    *soToken    = so_handle->token;

    lookup_bfd_t data = {
        .begin_addr           = beginOffset,
        .end_addr             = endOffset,
        .symbols              = so_handle->symbols,
        .scl_found_begin_addr = sclFoundBegin,
        .scl_found_end_addr   = sclFoundEnd,
        .scl_file_name        = sclFileName,
        .scl_function_name    = sclFunctionName,
        .scl_begin_lno        = sclBeginLineNo,
        .scl_end_lno          = sclEndLineNo
    };
    *( data.scl_found_begin_addr ) = false;
    *( data.scl_found_end_addr )   = false;

    bfd_map_over_sections( so_handle->abfd, section_iterator, &data );
}


void
SCOREP_Addr2line_LookupAddr( uintptr_t    programCounterAddr,
                             /* shared object OUT parameters */
                             void**       soHandle,
                             const char** soFileName,
                             uintptr_t*   soBaseAddr,
                             uint16_t*    soToken,
                             /* Source code location OUT parameters */
                             bool*        sclFound,
                             const char** sclFileName,
                             const char** sclFunctionName,
                             unsigned*    sclLineNo )
{
    UTILS_BUG_ON( soHandle == NULL ||
                  soFileName == NULL ||
                  soBaseAddr == NULL ||
                  soToken == NULL ||
                  sclFound == NULL ||
                  sclFileName == NULL ||
                  sclFunctionName == NULL ||
                  sclLineNo == NULL,
                  "Need valid OUT handles but NULL provided." );
    lt_object* handle = lookup_so( programCounterAddr );
    *soHandle = handle;
    if ( handle )
    {
        *soFileName = handle->name;
        *soBaseAddr = handle->base_addr;
        *soToken    = handle->token;

        bool         found_end_addr_unused;
        lookup_bfd_t data = {
            .begin_addr           = programCounterAddr - handle->base_addr,
            .end_addr             = 0,
            .symbols              = handle->symbols,
            .scl_found_begin_addr = sclFound,
            .scl_found_end_addr   = &found_end_addr_unused,
            .scl_file_name        = sclFileName,
            .scl_function_name    = sclFunctionName,
            .scl_begin_lno        = sclLineNo,
            .scl_end_lno          = NULL
        };
        *( data.scl_found_begin_addr ) = false;
        *( data.scl_found_end_addr )   = false;

        bfd_map_over_sections( handle->abfd, section_iterator, &data );
    }
    else
    {
        *soToken  = SCOREP_ADDR2LINE_SO_OBJECT_INVALID_TOKEN;
        *sclFound = false;
    }

#if HAVE( SCOREP_DEBUG )
    if ( *sclFound )
    {
        UTILS_DEBUG( "addr %" PRIuPTR ": %s@%s:%d [%s]",
                     programCounterAddr, *sclFunctionName, *sclFileName,
                     *sclLineNo, *soFileName );
    }
    else if ( *soHandle )
    {
        UTILS_DEBUG( "addr %" PRIuPTR " found in %s, but bfd lookup not successful",
                     programCounterAddr, *soFileName );
    }
    else
    {
        UTILS_DEBUG( "addr %" PRIuPTR " not contained in shared objects",
                     programCounterAddr );
    }
#endif /* HAVE( SCOREP_DEBUG ) */
}


void
SCOREP_Addr2line_LookupAddrRange( uintptr_t    beginProgramCounterAddr,
                                  uintptr_t    endProgramCounterAddr,
                                  /* shared object OUT parameters */
                                  void**       soHandle,
                                  const char** soFileName,
                                  uintptr_t*   soBaseAddr,
                                  uint16_t*    soToken,
                                  /* Source code location OUT parameters */
                                  bool*        sclFoundBegin,
                                  bool*        sclFoundEnd,
                                  const char** sclFileName,
                                  const char** sclFunctionName,
                                  unsigned*    sclBeginLineNo,
                                  unsigned*    sclEndLineNo )
{
    UTILS_BUG_ON( soHandle == NULL ||
                  soFileName == NULL ||
                  soBaseAddr == NULL ||
                  soToken == NULL ||
                  sclFoundBegin == NULL ||
                  sclFoundEnd == NULL ||
                  sclFileName == NULL ||
                  sclFunctionName == NULL ||
                  sclBeginLineNo == NULL ||
                  sclEndLineNo == NULL,
                  "Need valid OUT handles but NULL provided." );
    lt_object* handle = lookup_so( beginProgramCounterAddr );
    *soHandle = handle;
    if ( handle )
    {
        *soFileName = handle->name;
        *soBaseAddr = handle->base_addr;
        *soToken    = handle->token;

        lookup_bfd_t data = {
            .begin_addr           = beginProgramCounterAddr - handle->base_addr,
            .end_addr             = endProgramCounterAddr - handle->base_addr,
            .symbols              = handle->symbols,
            .scl_found_begin_addr = sclFoundBegin,
            .scl_found_end_addr   = sclFoundEnd,
            .scl_file_name        = sclFileName,
            .scl_function_name    = sclFunctionName,
            .scl_begin_lno        = sclBeginLineNo,
            .scl_end_lno          = sclEndLineNo
        };
        *( data.scl_found_begin_addr ) = false;
        *( data.scl_found_end_addr )   = false;

        bfd_map_over_sections( handle->abfd, section_iterator, &data );
    }
    else
    {
        *soToken       = SCOREP_ADDR2LINE_SO_OBJECT_INVALID_TOKEN;
        *sclFoundBegin = false;
        *sclFoundEnd   = false;
    }

#if HAVE( SCOREP_DEBUG )
    if ( *sclFoundBegin )
    {
        if ( *sclFoundEnd )
        {
            UTILS_DEBUG( "addr range [%" PRIuPTR ", %" PRIuPTR "]: %s@%s:%d-%d [%s]",
                         beginProgramCounterAddr, endProgramCounterAddr,
                         *sclFunctionName, *sclFileName,
                         *sclBeginLineNo, *sclEndLineNo, *soFileName );
        }
        else
        {
            UTILS_DEBUG( "begin_addr of range [%" PRIuPTR ", %" PRIuPTR "]: %s@%s:%d [%s]",
                         beginProgramCounterAddr, endProgramCounterAddr,
                         *sclFunctionName, *sclFileName,
                         *sclBeginLineNo, *soFileName );
        }
    }
    else if ( *soHandle )
    {
        UTILS_DEBUG( "addr range [%" PRIuPTR ", %" PRIuPTR "] found in %s, "
                     "but bfd lookup not successful",
                     beginProgramCounterAddr, endProgramCounterAddr, *soFileName );
    }
    else
    {
        UTILS_DEBUG( "addr range [%" PRIuPTR ", %" PRIuPTR "] not contained "
                     "in shared objects",
                     beginProgramCounterAddr, endProgramCounterAddr );
    }
#endif /* HAVE( SCOREP_DEBUG ) */
}


static lt_object*
lookup_so( uintptr_t addr )
{
    /* addr to be in [begin_addr, end_addr] of shared object representation */

    /* search linearly in loadtime objects */
    if ( lt_object_count > 0
         && addr >= lt_begin_addrs[ 0 ]
         && addr <= lt_objects[ lt_object_count - 1 ].end_addr )
    {
        size_t i = 0;
        while ( i < lt_object_count - 1
                && lt_begin_addrs[ i + 1 ] < addr )
        {
            ++i;
        }
        if ( lt_objects[ i ].end_addr > addr )
        {
            /* found addr in loadtime objects */
            return &lt_objects[ i ];
        }
    }
    return NULL;
}


static void
section_iterator( bfd* abfd, asection* section, void* payload )
{
    lookup_bfd_t* data = payload;
    if ( *( data->scl_found_begin_addr ) )
    {
        return;
    }
    if ( !( bfd_get_section_flags( abfd, section ) & SEC_ALLOC ) )
    {
        return;
    }
    /* consider address if in [vma, vma+size] */
    bfd_vma vma = bfd_get_section_vma( abfd, section );
    if ( data->begin_addr < vma
         || ( data->end_addr != 0 && data->end_addr < vma ) )
    {
        return;
    }
    bfd_size_type size = bfd_get_section_size( section );
    if ( data->begin_addr >= vma + size
         || ( data->end_addr != 0
              && data->end_addr >= vma + size ) )
    {
        return;
    }
    *( data->scl_found_begin_addr ) = bfd_find_nearest_line( abfd,
                                                             section,
                                                             data->symbols,
                                                             data->begin_addr - vma,
                                                             data->scl_file_name,
                                                             data->scl_function_name,
                                                             data->scl_begin_lno );
    /* Calling bfd_find_inliner_info seems, after superficial testing,
       to not alter the result for function addressees provided by
       __cyg_profile_func_enter/exit. For callsite addressees, e.g.,
       provided via __builtin_return_address(0) or
       __cyg_profile_func_enter/exit's callsite parameter, walking the
       inline chain brings source code location information back from
       unexpected locations to user code. */
    while ( bfd_find_inliner_info( abfd,
                                   data->scl_file_name,
                                   data->scl_function_name,
                                   data->scl_begin_lno ) )
    {
    }

    if ( *( data->scl_found_begin_addr ) && data->end_addr != 0 )
    {
        const char* filename_unused;
        const char* funcname_unused;
        *( data->scl_found_end_addr ) = bfd_find_nearest_line( abfd,
                                                               section,
                                                               data->symbols,
                                                               data->end_addr - vma,
                                                               &filename_unused,
                                                               &funcname_unused,
                                                               data->scl_end_lno );
        while ( bfd_find_inliner_info( abfd,
                                       &filename_unused,
                                       &funcname_unused,
                                       data->scl_end_lno ) )
        {
        }
    }
    return;
}


void
SCOREP_Addr2line_Finalize( void )
{
    UTILS_DEBUG_ENTRY();

    if ( !addr2line_initialized )
    {
        return;
    }
    addr2line_initialized = false;

    bool        unused;
    const char* exe_name = SCOREP_GetExecutableName( &unused );
    for ( size_t i = 0; i < lt_object_count; i++ )
    {
        free( lt_objects[ i ].symbols );
        bfd_close( lt_objects[ i ].abfd );
        if ( strcmp( lt_objects[ i ].name, exe_name ) != 0 )
        {
            free( ( char* )lt_objects[ i ].name );
        }
    }
    free( lt_objects );
    lt_object_count = 0;

    UTILS_DEBUG_EXIT();
}
