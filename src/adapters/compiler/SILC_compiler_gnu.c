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
 * @ file      SILC_compiler_gnu.c
 * @maintainer Rene Jaekel <rene.jaekel@tu-dresden.de>
 *
 * @brief Support for GNU-Compiler
 * Will be triggered by the '-finstrument-functions' flag of the GNU
 * compiler.
 */

#include <stdio.h>
#include <limits.h>
#include <bfd.h>
#include <unistd.h>

#include <SILC_Types.h>
#include <SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_RuntimeManagement.h>

#include <SILC_Compiler_Init.h>
#include <SILC_Compiler_Data.h>




/**
 * static variable to control initialize status of GNU
 */
static int gnu_init = 1;

/**
 * data structure to map function name and region identifier
 */
typedef struct HashNode HN;


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
    char      path[ 512 ] = { 0 };

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
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "***********************************************************************" );
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "Could not determine path of executable." );
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "Meanwhile, you have to set 'SILC_APPPATH' to your local executable." );
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "***********************************************************************\n" );

            SILC_ERROR( SILC_ERROR_ENOENT, "" );
            return;
            /* we should abort here */
        }
        else
        {
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "  exepath = %s ", exepath );
            BfdImage = bfd_openr( exepath, 0 );
            if ( !BfdImage )
            {
                SILC_ERROR( SILC_ERROR_ENOENT, "" );
            }
        }
    }
    else
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " got the  path to binary = %sn", path );
        BfdImage = bfd_openr( ( const char* )&path, 0 );
        if ( !BfdImage )
        {
            SILC_ERROR( SILC_ERROR_ENOENT, "BFD image not present to given path: %s \n", path );
        }
    }


    /* check image format   */
    if ( !bfd_check_format( BfdImage, bfd_object ) )
    {
        SILC_ERROR( SILC_ERROR_EIO, "BFD: bfd_check_format(): failed\n" );
    }


    /* return if file has no symbols at all */
    if ( !( bfd_get_file_flags( BfdImage ) & HAS_SYMS ) )
    {
        SILC_ERROR( SILC_ERROR_FILE_INTERACTION, "BFD: bfd_get_file_flags(): failed \n" );
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

        hash_put( addr, canonicSymbols[ i ]->name, filename, lno );
    }
    free( canonicSymbols );
    bfd_close( BfdImage );
    return;
}


/**
 * @brief finalize GNU interface
 */
void
silc_gnu_free
(
    void
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "finalize the gnu compiler instrumentation." );

    hash_free();

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

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "call at function enter!!!" );

    /*
     * put hash table entries via mechanism for bfd symbol table
     * to calculate function addresses if measurement was not initialized
     */

    if ( gnu_init )
    {
        /* not initialized so far */
        SILC_InitMeasurement();

        silc_gnu_free();
        gnu_init = 0;   /* is initialized */

        /* call function to calculate symbol table */
        get_symTab();
    }


    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " function pointer: %ld ", ( long )func );

    if ( ( hn = hash_get( ( long )func ) ) )
    {
        if ( hn->reghandle == SILC_INVALID_REGION )
        {
            /* -- region entered the first time, register region -- */
            silc_compiler_register_region( hn );
        }
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "enter the region with handle %i ", hn->reghandle );
        SILC_EnterRegion( hn->reghandle );
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
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "call function exit!!!" );
    if ( hn = hash_get( ( long )func ) )
    {
        SILC_ExitRegion( hn->reghandle );
    }
}
