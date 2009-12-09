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

#include <stdio.h>
#include <limits.h>
#include <bfd.h>

#include <SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_RuntimeManagement.h>

#include <SILC_Compiler_Init.h>
#include <SILC_Compiler_Data.h>




#define HASH_MAX 1021

/**
 * static variable to control initialize status of GNU
 */
static int gnu_init = 1;


/**
 * data structure to map function name and region identifier
 */
typedef struct HashNode HN;
static HN* htab[ HASH_MAX ];


/**
 * @brief Get hash table entry for given ID.
 *
 * @param h   Hash node key ID
 *
 * @return Returns pointer to hash table entry according to key
 */
static HN*
hash_get( long h )
{
    long id = h % HASH_MAX;

    printf( " hash id %ld: \n", id );

    HN* curr = htab[ id ];
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
    long id  = h % HASH_MAX;
    HN*  add = ( HN* )malloc( sizeof( HN ) );
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
    asymbol** canonicSymbols;
    char*     exepath;
    char      path[ PATH_MAX ] = { 0 };



    /* initialize BFD */
    bfd_init();

    /**
     * by default, use /proc mechanism to obtain path to executable
     * in other cases, do it by examining SILC_APPPATH variable
     */

    /* get the path from system */
    if ( readlink( "/proc/self/exe", path, sizeof( path ) ) == -1 )
    {
        /* try to get the path via environment variable */
        exepath = getenv( "SILC_APPPATH" );

        if ( exepath == NULL )
        {
            printf( " \n***********************************************************************\n" );
            printf( "Could not determine path of executable.\n" );
            printf( "Meanwhile, you have to set 'SILC_APPPATH' to your local executable.\n" );
            printf( "***********************************************************************\n" );

            SILC_ERROR( SILC_ERROR_ENOENT, "" );
            return;
            /* we should abort here */
        }
        else
        {
            printf( "  exepath = %s \n", exepath );
            BfdImage = bfd_openr( exepath, 0 );
            if ( !BfdImage )
            {
                SILC_ERROR( ENOENT, "" );
            }
        }
    }
    else
    {
        printf( "  path = %s \n", path );
        BfdImage = bfd_openr( ( const char* )&path, 0 );
        if ( !BfdImage )
        {
            SILC_ERROR( ENOENT, "" );
        }
    }


    /* check image format   */
    if ( !bfd_check_format( BfdImage, bfd_object ) )
    {
        SILC_ERROR( EIO, "BFD: bfd_check_format(): failed\n" );
    }


    /* return if file has no symbols at all */
    if ( !( bfd_get_file_flags( BfdImage ) & HAS_SYMS ) )
    {
        SILC_ERROR( SILC_ERROR_FILE_INTERACTION, "BFD: bfd_get_file_flags(): failed" );
    }

    /* get the upper bound number of symbols */
    size = bfd_get_symtab_upper_bound( BfdImage );

    /* HAS_SYMS can be set even with no symbols in the file! */
    if ( size < 1 )
    {
        SILC_ERROR( SILC_ERROR_INVALID_SIZE_GIVEN, "BFD: bfd_get_symtab_upper_bound(): < 1 \n" );
    }

    /* read canonicalized symbols  */
    canonicSymbols = ( asymbol** )malloc( size );

    nr_all_syms = bfd_canonicalize_symtab( BfdImage, canonicSymbols );
    if ( nr_all_syms < 1 )
    {
        SILC_ERROR( SILC_ERROR_INVALID_SIZE_GIVEN, "BFD: bfd_canonicalize_symtab(): < 1\n" );
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
void
silc_compiler_register_region
(
    struct  HashNode* hn
)
{
    printf( " register a region: \n" );
    hn->reghandle = SILC_DefineRegion( hn->name,
                                       SILC_INVALID_SOURCE_FILE,
                                       SILC_INVALID_LINE_NO,
                                       SILC_INVALID_LINE_NO,
                                       SILC_ADAPTER_COMPILER,
                                       SILC_REGION_FUNCTION
                                       );
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
    HN* hn;
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

        silc_gnu_finalize;
        gnu_init = 0;   /* is initialized */

        /* call function to calculate symbol table */
        get_symTab();
    }


    printf( " function pointer: %ld \n", ( long )func );

    if ( ( hn = hash_get( ( long )func ) ) )
    {
        if ( hn->reghandle == SILC_INVALID_REGION )
        {
            /* -- region entered the first time, register region -- */
            silc_compiler_register_region( hn );
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
    HN* hn;
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
