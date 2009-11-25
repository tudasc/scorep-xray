/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @ file SILC_compiler_gnu.c
 *
 * @brief Support for GNU-Compiler
 * Will be triggered by the '-finstrument-functions' flag of the GNU
 * compiler.
 */

#include "stdio.h"
#include "bfd.h"

#include <SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_RuntimeManagement.h>


/**
 * static variable to control initialize status of GNU
 */
static int gnu_init = 1;


/**
 * @brief Hash table to map function addresses to region identifier
 * identifier is called region handle
 *
 * @param id          hash key (address of function)
 * @param name        associated function name
 * @param fname       file name
 * @param lnobegin    line number of begin of function
 * @param lnoend      line number of end of function
 * @param reghandle   region identifier
 * @param HN          pointer to next element
 */
typedef struct HashNode
{
    long              id;
    const char*       name;
    const char*       fname;
    SILC_LineNo       lnobegin;
    SILC_LineNo       lnoend;
    SILC_RegionHandle reghandle;
    struct HashNode*  next;
} HashNode;

#define HASH_MAX 1021

static HashNode* htab[ HASH_MAX ];


/**
 * @brief Get hash table entry for given ID.
 *
 * @param h   Hash node key ID
 *
 * @return Returns pointer to hash table entry according to key
 */
static HashNode*
hash_get( long h )
{
    long id = h % HASH_MAX;

    printf( " hash id %i: ", id );

    HashNode* curr = htab[ id ];
    while ( curr )
    {
        if ( curr->id == h )
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}


/**
 * Stores function name under hash code
 *
 * @param h    Hash node key
 * @param n    file name
 * @param fn   function name
 * @param lno  line number
 */
static void
hash_put
(
    long        h,
    const char* n,
    const char* fn,
    int         lno
)
{
    long      id  = h % HASH_MAX;
    HashNode* add = ( HashNode* )malloc( sizeof( HashNode ) );
    add->id        = h;
    add->name      = n;
    add->fname     = fn ? ( const char* )strdup( fn ) : fn;
    add->lnobegin  = lno;
    add->lnoend    = SILC_INVALID_LINE_NO;
    add->reghandle = SILC_INVALID_REGION;
    add->next      = htab[ id ];
    htab[ id ]     = add;
}


/**
 * @brief Get symbol table either by using BFD or by parsing nm-file
 */
static void
get_symTab( void )
{
    bfd*      BfdImage = 0;
    int       nr_all_syms;
    int       i;
    size_t    size;
    char*     exe_env;
    asymbol** canonicSymbols;


    /* get gnu src from environment variable "VT_GNU_GETSRC" */
    /*  int do_getsrc = vt_env_gnu_getsrc(); */

    /* initialize BFD */
    bfd_init();
    /* get executable path from environment var. VT_APPPATH */

    /* get the path from the environment */
    char* temp = getenv( "SILC_APPPATH" );

    printf( " apppath set to: %s \n", temp );

    if ( !temp )
    {
        printf( "Could not determine path of executable.\n" );
        /* we should abort here */
    }
    else
    {
        BfdImage = bfd_openr( temp, 0 );
        if ( !BfdImage )
        {
            printf( "BFD: bfd_openr(): failed\n" );
        }
    }


    /* check image format   */
    if ( !bfd_check_format( BfdImage, bfd_object ) )
    {
        printf( "BFD: bfd_check_format(): failed\n" );
    }


    /* return if file has no symbols at all */
    if ( !( bfd_get_file_flags( BfdImage ) & HAS_SYMS ) )
    {
        printf( "BFD: bfd_get_file_flags(): failed\n" );
    }

    /* get the upper bound number of symbols */
    size = bfd_get_symtab_upper_bound( BfdImage );

    printf( " size of symtab: %i \n", size );


    /* HAS_SYMS can be set even with no symbols in the file! */
    if ( size < 1 )
    {
        printf( "BFD: bfd_get_symtab_upper_bound(): < 1 \n" );
    }

    /* read canonicalized symbols  */
    canonicSymbols = ( asymbol** )malloc( size );

    nr_all_syms = bfd_canonicalize_symtab( BfdImage, canonicSymbols );
    if ( nr_all_syms < 1 )
    {
        printf( "BFD: bfd_canonicalize_symtab(): < 1\n" );
    }
    for ( i = 0; i < nr_all_syms; ++i )
    {
        char*        dem_name = 0;
        long         addr;
        const char*  filename;
        const char*  funcname;
        unsigned int lno;

        /* ignore system functions */
        if ( strncmp( canonicSymbols[ i ]->name, "__", 2 ) == 0 ||
             strncmp( canonicSymbols[ i ]->name, "bfd_", 4 ) == 0 ||
             strstr( canonicSymbols[ i ]->name, "@@" ) != NULL )
        {
            continue;
        }

        /* get filename and linenumber from debug info */
        /* needs -g */
        filename = NULL;
        lno      = SILC_INVALID_LINE_NO;



        /* get the source info for every funciont in case of gnu by default */
        /* calls BFD_SEND */
        bfd_find_nearest_line( BfdImage,
                               bfd_get_section( canonicSymbols[ i ] ),
                               canonicSymbols,
                               canonicSymbols[ i ]->value,
                               &filename,
                               &funcname,
                               &lno
                               );



        /* calculate function address */
        addr = canonicSymbols[ i ]->section->vma + canonicSymbols[ i ]->value;
        /* printf(" address: %i, name %s, filename %s, line number %i \n", addr, canonicSymbols[i]->name, filename, lno); */

        hash_put( addr, canonicSymbols[ i ]->name, filename, lno );
    }

    bfd_close( BfdImage );
    return;
}

/**
 * @brief Register a new region to the measuremnt system
 *
 * @param hn   Hash node which stores the registered regions
 */
static void
register_region
(
    HashNode* hn
)
{
    hn->reghandle = SILC_DefineRegion( hn->name,
                                       SILC_INVALID_SOURCE_FILE,
                                       SILC_INVALID_LINE_NO,
                                       SILC_INVALID_LINE_NO,
                                       SILC_ADAPTER_COMPILER,
                                       SILC_REGION_FUNCTION
                                       );

    printf( " register a region: \n" );
}

/**
 * @brief finalize GNU interface
 */
void
silc_gnu_finalize
(
    void
)
{
    printf( "finalize the gnu compiler instrumentation. \n" );

    int i;
    for ( i = 0; i < HASH_MAX; i++ )
    {
        if ( htab[ i ] )
        {
            free( htab[ i ] );
            htab[ i ] = NULL;
        }
    }
    /* set gnu init status to one - means not initialized */
    gnu_init = 1;
}


/**
 * @brief This function is called just after the entry of a function
 * generated by the GNU compiler.
 * @param func      The address of the start of the current function.
 * @param callsice  The call site of the current function.
 */
void
__cyg_profile_func_enter
(
    void* func,
    void* callsite
)
{
    HashNode* hn;
    printf( "call at function enter!!!\n" );


    /*
     * init measurement just for dummy purpose
     *
     * put hash table entries via mechanism for bfd symbol table
     * to calculate function addresses
     */

    if ( gnu_init )
    {
        /* not initialized so far */
        SILC_InitMeasurement();

        /**
         * the finalize is not supported, probably not needed
         */
        /*	  silc_finalize = gnu_finalize; */
        gnu_init = 0;   /* is initialized */

        /* call function to calculate symbol table */
        get_symTab();
    }


    printf( " function pointer: %i \n", ( long )func );

    if ( ( hn = hash_get( ( long )func ) ) )
    {
        if ( hn->reghandle == SILC_INVALID_REGION )
        {
            /* -- region entered the first time, register region -- */
            register_region( hn );
            printf( " register region with handle %i \n", hn->reghandle );
        }
        printf( "enter the region with handle %i \n", hn->reghandle );
        SILC_EnterRegion( hn->reghandle );
    }
    else
    {
        SILC_RegionHandle dummy = 1;
        SILC_EnterRegion( dummy );
    }
}

/**
 * @brief This function is called just before the exit of a function
 * generated by the GNU compiler.
 * @param func      The address of the end of the current function.
 * @param callsice  The call site of the current function.
 */
void
__cyg_profile_func_exit
(
    void* func,
    void* callsite
)
{
    HashNode* hn;
    printf( "call function exit!!!\n" );
    if ( hn = hash_get( ( long )func ) )
    {
        SILC_ExitRegion( hn->reghandle );
    }
    else
    {
        SILC_RegionHandle dummy = 1;
        SILC_ExitRegion( dummy );
    }
}
