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
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
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


#ifdef GNU_DEMANGLE
/* Declaration of external demangeling function */
/* It is contained in "demangle.h" */
extern char*
cplus_demangle( const char* mangled,
                int         options );

/* cplus_demangle options */
#define SILC_COMPILER_DEMANGLE_NO_OPTS   0
#define SILC_COMPILER_DEMANGLE_PARAMS    ( 1 << 0 )  /* include function arguments */
#define SILC_COMPILER_DEMANGLE_ANSI      ( 1 << 1 )  /* include const, volatile, etc. */
#define SILC_COMPILER_DEMANGLE_VERBOSE   ( 1 << 3 )  /* include implementation details */
#define SILC_COMPILER_DEMANGLE_TYPES     ( 1 << 4 )  /* include type encodings */

/* Demangeling style. */
static int silc_compiler_demangle_style = SILC_COMPILER_DEMANGLE_PARAMS  |
                                          SILC_COMPILER_DEMANGLE_ANSI    |
                                          SILC_COMPILER_DEMANGLE_VERBOSE |
                                          SILC_COMPILER_DEMANGLE_TYPES;
#endif /* GNU_DEMANGLE */


/**
 * static variable to control initialize status of GNU
 */
static int silc_compiler_initialize = 1;


/**
 * @brief Get symbol table either by using BFD or by parsing nm-file
 */
static void
silc_compiler_get_sym_tab( void )
{
    bfd*      bfd_image = 0;
    int       nr_all_syms;
    int       i;
    size_t    size;
    asymbol** canonic_symbols;
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
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                               "********************************************************"
                               "***************" );
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                               "Could not determine path of executable." );
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                               "Meanwhile, you have to set 'SILC_APPPATH' to your local "
                               "executable." );
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                               "********************************************************"
                               "***************\n" );

            SILC_ERROR( SILC_ERROR_ENOENT, "" );
            return;
            /* we should abort here */
        }
        else
        {
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "  exepath = %s ", exepath );
            bfd_image = bfd_openr( exepath, 0 );
            if ( !bfd_image )
            {
                SILC_ERROR( SILC_ERROR_ENOENT, "" );
            }
        }
    }
    else
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " got the  path to binary = %sn", path );
        bfd_image = bfd_openr( ( const char* )&path, 0 );
        if ( !bfd_image )
        {
            SILC_ERROR( SILC_ERROR_ENOENT,
                        "BFD image not present to given path: %s \n", path );
        }
    }


    /* check image format   */
    if ( !bfd_check_format( bfd_image, bfd_object ) )
    {
        SILC_ERROR( SILC_ERROR_EIO, "BFD: bfd_check_format(): failed\n" );
    }


    /* return if file has no symbols at all */
    if ( !( bfd_get_file_flags( bfd_image ) & HAS_SYMS ) )
    {
        SILC_ERROR( SILC_ERROR_FILE_INTERACTION,
                    "BFD: bfd_get_file_flags(): failed \n" );
    }

    /* get the upper bound number of symbols */
    size = bfd_get_symtab_upper_bound( bfd_image );

    /* HAS_SYMS can be set even with no symbols in the file! */
    if ( size < 1 )
    {
        SILC_ERROR( SILC_ERROR_INVALID_SIZE_GIVEN,
                    "BFD: bfd_get_symtab_upper_bound(): < 1 \n" );
    }

    /* read canonicalized symbols  */
    canonic_symbols = ( asymbol** )malloc( size );

    nr_all_syms = bfd_canonicalize_symtab( bfd_image, canonic_symbols );
    if ( nr_all_syms < 1 )
    {
        SILC_ERROR( SILC_ERROR_INVALID_SIZE_GIVEN,
                    "BFD: bfd_canonicalize_symtab(): < 1\n" );
    }
    for ( i = 0; i < nr_all_syms; ++i )
    {
        char*        dem_name = 0;
        long         addr;
        const char*  filename;
        const char*  funcname;
        unsigned int lno;

        /* ignore system functions */
        if ( strncmp( canonic_symbols[ i ]->name, "__", 2 ) == 0 ||
             strncmp( canonic_symbols[ i ]->name, "bfd_", 4 ) == 0 ||
             strstr( canonic_symbols[ i ]->name, "@@" ) != NULL )
        {
            continue;
        }

        /* get filename and linenumber from debug info */
        /* needs -g */
        filename = NULL;
        lno      = SILC_INVALID_LINE_NO;

        /* calculate function address */
        addr = canonic_symbols[ i ]->section->vma + canonic_symbols[ i ]->value;

        /* get the source info for every funciont in case of gnu by default */
        /* calls BFD_SEND */
        bfd_find_nearest_line( bfd_image,
                               bfd_get_section( canonic_symbols[ i ] ),
                               canonic_symbols,
                               canonic_symbols[ i ]->value,
                               &filename,
                               &funcname,
                               &lno );

        /* In case no debugging symbols are found, funcname is NULL. */
        /* Thus, set it from the alway present csnonic Symbols.      */
        funcname = canonic_symbols[ i ]->name;

#ifdef GNU_DEMANGLE
        /* use demangled name if possible */
        if ( silc_compiler_demangle_style >= 0 )
        {
            dem_name = cplus_demangle( funcname,
                                       silc_compiler_demangle_style );
            if ( dem_name != NULL )
            {
                funcname = dem_name;
            }
        }
#endif  /* GNU_DEMANGLE */

        silc_compiler_hash_put( addr, funcname, filename, lno );
    }
    free( canonic_symbols );
    bfd_close( bfd_image );
    return;
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
    silc_compiler_hash_node* hash_node;

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "call at function enter!!!" );

    /*
     * put hash table entries via mechanism for bfd symbol table
     * to calculate function addresses if measurement was not initialized
     */

    if ( silc_compiler_initialize )
    {
        /* not initialized so far */
        SILC_InitMeasurement();
    }


    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " function pointer: %ld ", ( long )func );

    if ( ( hash_node = silc_compiler_hash_get( ( long )func ) ) )
    {
        if ( hash_node->region_handle == SILC_INVALID_REGION )
        {
            /* -- region entered the first time, register region -- */
            silc_compiler_register_region( hash_node );
        }
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                           "enter the region with handle %i ",
                           hash_node->region_handle );
        SILC_EnterRegion( hash_node->region_handle );
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
    silc_compiler_hash_node* hash_node;
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "call function exit!!!" );
    if ( hash_node = silc_compiler_hash_get( ( long )func ) )
    {
        SILC_ExitRegion( hash_node->region_handle );
    }
}

/* Adapter initialization */
SILC_Error_Code
silc_compiler_init_adapter()
{
    if ( silc_compiler_initialize )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " inititialize GNU compiler adapter!" );

        /* Initialize hash tables */
        silc_compiler_hash_init();

        /* call function to calculate symbol table */
        silc_compiler_get_sym_tab();

        /* Sez flag */
        silc_compiler_initialize = 0;
    }

    return SILC_SUCCESS;
}

/* Adapter finalization */
void
silc_compiler_finalize()
{
    /* call only, if previously initialized */
    if ( !silc_compiler_initialize )
    {
        /* Delete hash table */
        silc_compiler_hash_free();

        /* Set initilaization flag */
        silc_compiler_initialize = 1;
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " finalize GNU compiler adapter!" );
    }
}
