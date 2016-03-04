/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include "scorep_unwinding_mgmt.h"

#include <SCOREP_Definitions.h>
#include <SCOREP_Location.h>

#define SCOREP_DEBUG_MODULE_NAME UNWINDING
#include <UTILS_Debug.h>
#include <UTILS_IO.h>

#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/mman.h>

#if HAVE( LIBBFD )
#include <bfd.h>
#endif

#include "scorep_unwinding_region.h"

static const char*
parse_hex( const char* pos,
           uint64_t*   value )
{
    char base;
    *value = 0;
again:
    base = '0';
    switch ( *pos )
    {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            base = 'a' - 10;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            *value = *value * 16 + ( *pos - base );
            pos++;
            goto again;
    }

    return pos;
}

static const char*
skip_blanks( const char* pos )
{
    while ( isspace( *pos ) )
    {
        pos++;
    }
    return pos;
}

static const char*
skip_field( const char* pos )
{
    while ( !isspace( *pos ) )
    {
        pos++;
    }
    return skip_blanks( pos );
}

static const char*
parse_line( const char* line,
            uint64_t*   start,
            uint64_t*   end,
            int*        prot )
{
    line = parse_hex( line, start );
    /* skip '-' */
    line++;
    line = parse_hex( line, end );

    line  = skip_blanks( line );
    *prot = PROT_NONE;
    if ( *line++ == 'r' )
    {
        *prot |= PROT_READ;
    }
    if ( *line++ == 'w' )
    {
        *prot |= PROT_WRITE;
    }
    if ( *line++ == 'x' )
    {
        *prot |= PROT_EXEC;
    }
    /* skip shared/private */
    line++;
    line = skip_blanks( line );

    /* ignore offset, its 0 for EXEC */
    line = skip_field( line );

    /* ignore device */
    line = skip_field( line );

    /* ignore inode, reminder is pathname */
    return skip_field( line );
}

struct addr_range
{
    struct addr_range* left;
    struct addr_range* right;
    uint64_t           start;
    uint64_t           end;
#if HAVE( LIBBFD )
    bfd*               image;
    asymbol**          image_symbols;
#endif
    char               pathname[ 1 ];
};

typedef int ( * addr_range_compare )( uint64_t           start,
                                      struct addr_range* node );

typedef void ( * addr_range_cleanup )( struct addr_range* node );

static int
addr_range_compare_start( uint64_t           start,
                          struct addr_range* node )
{
    return ( start > node->start ) - ( start < node->start );
}

static int
addr_range_compare_incl( uint64_t addr, struct addr_range* node )
{
    if ( addr < node->start )
    {
        return -1;
    }
    if ( addr >= node->end )
    {
        return 1;
    }
    return 0;
}

/* Splay tree based on: */
/*
                An implementation of top-down splaying
                    D. Sleator <sleator@cs.cmu.edu>
                            March 1992

   "Splay trees", or "self-adjusting search trees" are a simple and
   efficient data structure for storing an ordered set.  The data
   structure consists of a binary tree, without parent pointers, and no
   additional fields.  It allows searching, insertion, deletion,
   deletemin, deletemax, splitting, joining, and many other operations,
   all with amortized logarithmic performance.  Since the trees adapt to
   the sequence of requests, their performance on real access patterns is
   typically even better.  Splay trees are described in a number of texts
   and papers [1,2,3,4,5].

   The code here is adapted from simple top-down splay, at the bottom of
   page 669 of [3].  It can be obtained via anonymous ftp from
   spade.pc.cs.cmu.edu in directory /usr/sleator/public.

   The chief modification here is that the splay operation works even if the
   item being splayed is not in the tree, and even if the tree root of the
   tree is NULL.  So the line:

                              t = splay(i, t);

   causes it to search for item with key i in the tree rooted at t.  If it's
   there, it is splayed to the root.  If it isn't there, then the node put
   at the root is the last one before NULL that would have been reached in a
   normal binary search for i.  (It's a neighbor of i in the tree.)  This
   allows many other operations to be easily implemented, as shown below.

   [1] "Fundamentals of data structures in C", Horowitz, Sahni,
       and Anderson-Freed, Computer Science Press, pp 542-547.
   [2] "Data Structures and Their Algorithms", Lewis and Denenberg,
       Harper Collins, 1991, pp 243-251.
   [3] "Self-adjusting Binary Search Trees" Sleator and Tarjan,
       JACM Volume 32, No 3, July 1985, pp 652-686.
   [4] "Data Structure and Algorithm Analysis", Mark Weiss,
       Benjamin Cummins, 1992, pp 119-130.
   [5] "Data Structures, Algorithms, and Performance", Derick Wood,
       Addison-Wesley, 1993, pp 367-375.
 */

static struct addr_range*
addr_range_splay( struct addr_range* root,
                  uint64_t           key,
                  addr_range_compare compare )
{
    struct addr_range  sentinel;
    struct addr_range* left;
    struct addr_range* right;
    struct addr_range* node;

    if ( root == NULL )
    {
        return root;
    }

    sentinel.left = sentinel.right = NULL;
    left          = right = &sentinel;

    for (;; )
    {
        if ( compare( key, root ) < 0 )
        {
            if ( root->left == NULL )
            {
                break;
            }
            if ( compare( key, root->left ) < 0 )
            {
                node        = root->left;                    /* rotate right */
                root->left  = node->right;
                node->right = root;
                root        = node;
                if ( root->left == NULL )
                {
                    break;
                }
            }
            right->left = root;                               /* link right */
            right       = root;
            root        = root->left;
        }
        else if ( compare( key, root ) > 0 )
        {
            if ( root->right == NULL )
            {
                break;
            }
            if ( compare( key, root->right ) > 0 )
            {
                node        = root->right;                   /* rotate left */
                root->right = node->left;
                node->left  = root;
                root        = node;
                if ( root->right == NULL )
                {
                    break;
                }
            }
            left->right = root;                              /* link left */
            left        = root;
            root        = root->right;
        }
        else
        {
            break;
        }
    }

    left->right = root->left;                                /* assemble */
    right->left = root->right;
    root->left  = sentinel.right;
    root->right = sentinel.left;

    return root;
}

static void
addr_range_insert( struct addr_range** root,
                   uint64_t            start,
                   uint64_t            end,
                   const char*         pathname )
{
    size_t             len = strlen( pathname );
    struct addr_range* new = calloc( 1, sizeof( *new ) + len );
    new->start = start;
    new->end   = end;
    memcpy( new->pathname, pathname, len + 1 );
    new->left = new->right = NULL;

    if ( *root == NULL )
    {
        *root = new;
        return;
    }

    *root = addr_range_splay( *root, start, addr_range_compare_start );
    if ( start < ( *root )->start )
    {
        new->left       = ( *root )->left;
        new->right      = *root;
        ( *root )->left = NULL;
        *root           = new;
    }
    else if ( start > ( *root )->start )
    {
        new->right       = ( *root )->right;
        new->left        = *root;
        ( *root )->right = NULL;
        *root            = new;
    }
    else
    {
        free( new );
    }
}

static struct addr_range*
addr_range_find( struct addr_range** root,
                 uint64_t            addr )
{
    *root = addr_range_splay( *root, addr, addr_range_compare_incl );
    if ( addr_range_compare_incl( addr, *root ) == 0 )
    {
        return *root;
    }
    return NULL;
}

static void
addr_range_clear( struct addr_range** root,
                  addr_range_cleanup  cleanup )
{
    while ( *root )
    {
        struct addr_range* node = NULL;
        if ( ( *root )->left == NULL )
        {
            node = ( *root )->right;
        }
        else
        {
            node = addr_range_splay( ( *root )->left,
                                     ( *root )->start,
                                     addr_range_compare_start );
            node->right = ( *root )->right;
        }
        if ( cleanup )
        {
            cleanup( *root );
        }
        free( *root );
        *root = node;
    }
}

#if HAVE( DEMANGLE )
/* Declaration of external demangling function */
/* It is contained in "demangle.h" */
extern char*
cplus_demangle( const char* mangled,
                int         options );

#endif /* HAVE( DEMANGLE ) */

struct scl_data
{
    bool          found;
#if HAVE( LIBBFD )
    asymbol**     image_symbols;
    long          start_addr;
    long          end_addr;
    char**        filename;
    unsigned int* start_lno;
    unsigned int* end_lno;
#endif
};

#if HAVE( LIBBFD )

static void
section_iterator( bfd*      abfd,
                  asection* section,
                  void*     vdata )
{
    struct scl_data* data = vdata;

    if ( data->found )
    {
        return;
    }

    if ( !bfd_get_section_flags( abfd, section ) & SEC_ALLOC )
    {
        return;
    }

    bfd_vma vma = bfd_get_section_vma( abfd, section );
    if ( data->start_addr < vma || ( data->end_addr != 0 && data->end_addr < vma ) )
    {
        return;
    }

    bfd_size_type size = bfd_get_section_size( section );
    if ( data->start_addr >= vma + size  || ( data->end_addr != 0 && data->end_addr >= vma + size ) )
    {
        return;
    }

    const char* functionname;
    data->found = bfd_find_nearest_line(
        abfd,
        section,
        data->image_symbols,
        data->start_addr - vma,
        ( const char** )data->filename,
        &functionname,
        data->start_lno );

    if ( data->found && data->end_addr != 0 )
    {
        const char* filename;
        bfd_find_nearest_line(
            abfd,
            section,
            data->image_symbols,
            data->end_addr - vma - 1,
            &filename,
            &functionname,
            data->end_lno );
    }
}

#endif

static bool
get_function_scl( struct addr_range* range,
                  long               startAddr,
                  long               endAddr,
                  char**             filename,
                  unsigned int*      startLno,
                  unsigned int*      endLno )
{
    struct scl_data data;
    data.found = false;

#if HAVE( LIBBFD )
    if ( range->image )
    {
        data.image_symbols = range->image_symbols;
        data.start_addr    = startAddr;
        data.end_addr      = endAddr;
        data.filename      = filename;
        data.start_lno     = startLno;
        data.end_lno       = endLno;

        bfd_map_over_sections( range->image, section_iterator, &data );
    }
#endif

    return data.found;
}


static bool
get_instruction_scl( struct addr_range* range,
                     long               addr,
                     char**             filename,
                     unsigned int*      lno )
{
    struct scl_data data;
    data.found = false;

#if HAVE( LIBBFD )
    if ( range->image )
    {
        data.image_symbols = range->image_symbols;
        data.start_addr    = addr;
        data.end_addr      = 0;
        data.filename      = filename;
        data.start_lno     = lno;
        data.end_lno       = NULL;

        bfd_map_over_sections( range->image, section_iterator, &data );
    }
#endif

    return data.found;
}

static void
finalize_region( scorep_unwinding_region* region,
                 void*                    data )
{
    struct addr_range** addr_ranges = data;

    if ( region->handle == SCOREP_INVALID_REGION )
    {
        return;
    }

    SCOREP_RegionDef* definition =
        SCOREP_LOCAL_HANDLE_DEREF( region->handle, Region );

#if HAVE( DEMANGLE )
    const char* region_name    = SCOREP_RegionHandle_GetName( region->handle );
    char*       demangled_name = cplus_demangle( region_name, 15 );
    if ( demangled_name )
    {
        definition->name_handle = SCOREP_Definitions_NewString( demangled_name );
        free( demangled_name );
    }
#endif

    struct addr_range* range = addr_range_find( addr_ranges, region->start );
    if ( !range )
    {
        return;
    }

    unsigned int start_lno = 0;
    unsigned int end_lno   = 0;
    char*        filename  = NULL;
    bool         found     = get_function_scl( range, region->start, region->end,
                                               &filename, &start_lno, &end_lno );
    if ( found && filename && *filename && start_lno )
    {
        SCOREP_SourceFileHandle source_file_handle =
            SCOREP_Definitions_NewSourceFile( filename );
        definition->file_name_handle =
            SCOREP_LOCAL_HANDLE_DEREF( source_file_handle, SourceFile )->name_handle;
        definition->begin_line = start_lno;
        if ( region->end )
        {
            definition->end_line = end_lno;
        }
    }
}

static bool
finalize_region_definitions( SCOREP_Location* location,
                             void*            data )
{
    struct addr_range** addr_ranges = data;

    SCOREP_LocationType location_type = SCOREP_Location_GetType( location );

    if ( location_type != SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        return false;
    }

    SCOREP_Unwinding_CpuLocationData* unwind_data =
        SCOREP_Location_GetSubsystemData( location, scorep_unwinding_subsystem_id );

    /* we clear the known regions and apply the post processing while at it */
    scorep_unwinding_region_clear( unwind_data, finalize_region, addr_ranges );

    return false;
}

static void
open_image( struct addr_range* range )
{
#if HAVE( LIBBFD )
    if ( range->image )
    {
        return;
    }

    range->image = bfd_openr( range->pathname, 0 );
    if ( !range->image )
    {
        return;
    }

    if ( !bfd_check_format( range->image, bfd_object )
         || !( bfd_get_file_flags( range->image ) & HAS_SYMS ) )
    {
        goto out;
    }

    /* get the upper bound number of symbols */
    size_t size = bfd_get_symtab_upper_bound( range->image );
    if ( size == 0 )
    {
        goto out;
    }

    /* read canonicalized symbols  */
    range->image_symbols = malloc( size );
    if ( !range->image_symbols )
    {
        goto out;
    }

    size = bfd_canonicalize_symtab( range->image, range->image_symbols );
    if ( size < 1 )
    {
        free( range->image_symbols );
        range->image_symbols = NULL;
        goto out;
    }

    return;

out:
    bfd_close( range->image );
    range->image = NULL;
#endif
}

static void
close_image( struct addr_range* range )
{
#if HAVE( LIBBFD )
    free( range->image_symbols );
    range->image_symbols = NULL;
    if ( range->image )
    {
        bfd_close( range->image );
        range->image = NULL;
    }
#endif
}

SCOREP_ErrorCode
scorep_unwinding_unify( void )
{
#if HAVE( LIBBFD )
    /* initialize BFD */
    bfd_init();
#endif

    /* Load /proc/self/maps */
    struct addr_range* addr_ranges = NULL;
    FILE*              maps        = fopen( "/proc/self/maps", "rb" );
    size_t             line_size   = 0;
    char*              line        = NULL;
    while ( maps && UTILS_IO_GetLine( &line, &line_size, maps ) == SCOREP_SUCCESS )
    {
        size_t len = strlen( line );
        if ( line[ len - 1 ] == '\n' )
        {
            line[ len - 1 ] = 0;
        }
        const char* pathname;
        uint64_t    start;
        uint64_t    end;
        int         prot;

        pathname = parse_line( line, &start, &end, &prot );

        if ( ~prot & PROT_EXEC || !*pathname || *pathname == '[' )
        {
            continue;
        }

        addr_range_insert( &addr_ranges, start, end, pathname );

        open_image( addr_ranges );
    }
    free( line );
    if ( maps )
    {
        fclose( maps );
    }

    /*
     * Itereate over all calling context definitions and resolve instruction
     * address to SCL
     */
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN(
        &scorep_local_definition_manager,
        CallingContext,
        calling_context )
    {
        if ( !definition->ip )
        {
            continue;
        }

        definition->ip_offset = definition->ip;
        struct addr_range* range = addr_range_find( &addr_ranges, definition->ip );
        if ( !range )
        {
            continue;
        }

        definition->ip_offset   = definition->ip - range->start;
        definition->file_handle = SCOREP_Definitions_NewString( range->pathname );

        char*        filename = NULL;
        unsigned int lno      = 0;
        bool         found    = get_instruction_scl( range, definition->ip, &filename, &lno );
        if ( found && filename && *filename && lno )
        {
            definition->scl_handle = SCOREP_Definitions_NewSourceCodeLocation( filename, lno );
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    SCOREP_Location_ForAll( finalize_region_definitions, &addr_ranges );

    addr_range_clear( &addr_ranges, close_image );

    return SCOREP_SUCCESS;
}
