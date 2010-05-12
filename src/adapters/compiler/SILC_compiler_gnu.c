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
#include <unistd.h>

#ifdef HAVE_LIBBFD
#include <bfd.h>
#elif defined HAVE_NM
#include <SILC_Timing.h>
#endif

#include <SILC_Types.h>
#include <SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_RuntimeManagement.h>

#include <SILC_Compiler_Init.h>
#include <SILC_Compiler_Data.h>


#define SILC_COMPILER_BUFFER_LEN 512

extern char* silc_compiler_executable;

#if defined( GNU_DEMANGLE ) && defined( HAVE_LIBBFD )
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
#endif /* GNU_DEMANGLE && HAVE_LIBBFD */

/**
 * static variable to control initialize status of GNU
 */
static int silc_compiler_initialize = 1;

/* ***************************************************************************************
   helper functions for symbaol table analysis
*****************************************************************************************/

/**
   Writes the path/filename of the executable into @a path. If the path is longer
   than the @a path, the last @a length characters of the path are written.
   @param path    A buffer into which the pat is written. The memoty for the buffer
                  must be allocated already.
   @param length  The size of the buffer @a path. If the retrieved path/filename of
                  the executable is longer than @a length, the path's head is truncated.
   @retruns true if the path of the executable was obtained successfully. Else false is
                 returned.
 */
static bool
silc_compiler_get_exe( char   path[],
                       size_t length )
{
    /**
     * by default, use /proc mechanism to obtain path to executable
     * in other cases, do it by examining SILC_APPPATH variable
     */

    /* get the path from system */
    if ( readlink( "/proc/self/exe", path, length ) == -1 )
    {
        /* try to get the path via environment variable */
        if ( silc_compiler_executable == NULL )
        {
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                               "Meanwhile, you have to set the configuration variable"
                               "'executable' to your local executable." );
            SILC_ERROR( SILC_ERROR_ENOENT, "Could not determine path of executable." );
            return false;
            /* we should abort here */
        }
        else
        {
            char*  exepath   = silc_compiler_executable;
            size_t exelength = strlen( exepath );
            if ( exelength > length )
            {
                /* If path is too long for buffer, truncate the head.
                   add one charakter for the terminating 0.           */
                exepath   = silc_compiler_executable + exelength - length + 1;
                exelength = length;
            }
            SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "  exepath = %s ", exepath );

            strncpy( path, exepath, exelength );
            return true;
        }
    }
    else
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " got the  path to binary = %sn", path );

        return true;
    }
}

/* ***************************************************************************************
   BFD based symbol table analysis
*****************************************************************************************/
#ifdef HAVE_LIBBFD
/**
 * Get symbol table using BFD. Stores all functions obtained from the symbol table
 * in a hashtable. The key of the hashtable is the function pointer. This must be done
 * during initialization of the GNU compiler adapter, becuase enter and exit events
 * provide only a file pointer.
 * It also collects information about source file and line number.
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

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "Read symbol table using BFD" );

    /* get the path from system */
    if ( !silc_compiler_get_exe( path, SILC_COMPILER_BUFFER_LEN ) )
    {
        return;
    }
    bfd_image = bfd_openr( ( const char* )&path, 0 );
    if ( !bfd_image )
    {
        SILC_ERROR( SILC_ERROR_ENOENT, "BFD image not present at path: %s \n", path );
        return;
    }

    /* check image format   */
    if ( !bfd_check_format( bfd_image, bfd_object ) )
    {
        SILC_ERROR( SILC_ERROR_EIO, "BFD: bfd_check_format(): failed\n" );
        return;
    }

    /* return if file has no symbols at all */
    if ( !( bfd_get_file_flags( bfd_image ) & HAS_SYMS ) )
    {
        SILC_ERROR( SILC_ERROR_FILE_INTERACTION,
                    "BFD: bfd_get_file_flags(): failed \n" );
        return;
    }

    /* get the upper bound number of symbols */
    size = bfd_get_symtab_upper_bound( bfd_image );

    /* HAS_SYMS can be set even with no symbols in the file! */
    if ( size < 1 )
    {
        SILC_ERROR( SILC_ERROR_INVALID_SIZE_GIVEN,
                    "BFD: bfd_get_symtab_upper_bound(): < 1 \n" );
        return;
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

#elif HAVE_NM

/* ***************************************************************************************
   nm based symbol table analysis
*****************************************************************************************/

/**
 * Write output from nm for @a exefile to @a nmfile.
 * @param exefile Filename of the executable which is analysed.
 * @param nmfile  Filename of the file to which the output is written.
 */
static void
silc_compiler_create_nm_file( char* nmfile,
                              char* exefile )
{
    char command[ 1024 ];
#ifdef GNU_DEMANGLE
    sprintf( command, "nm -Aol %s > %s", exefile, nmfile );
#else /* GNU_DEMANGLE */
    sprintf( command, "nm -ol %s > %s", exefile, nmfile );
#endif /* GNU_DEMANGLE */
    system( command );
}


/**
 * Get symbol table by parsing the oputput from nm. Stores all functions obtained
 * from the symbol table
 * in a hashtable. The key of the hashtable is the function pointer. This must be done
 * during initialization of the GNU compiler adapter, becuase enter and exit events
 * provide only a file pointer.
 * It also collects information about source file and line number.

 */
static void
silc_compiler_get_sym_tab( void )
{
    FILE* nmfile;
    char  line[ 1024 ];
    char  path[ SILC_COMPILER_BUFFER_LEN ] = { 0 };
    char  nmfilename[ 64 ];

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "Read symbol table using nm" );

    /* get the path from system */
    if ( !silc_compiler_get_exe( path, SILC_COMPILER_BUFFER_LEN ) )
    {
        return;
    }

    /* open nm-file */
    sprintf( nmfilename, "silc_nm_file.%ld", SILC_GetClockTicks() );
    silc_compiler_create_nm_file( nmfilename, path );
    if ( !( nmfile = fopen( nmfilename, "r" ) ) )
    {
        SILC_ERROR_POSIX();
    }

    /* read lines */
    while ( fgets( line, sizeof( line ) - 1, nmfile ) )
    {
        char*        col;
        char         delim[ 2 ] = " ";
        int          col_num    = 0;
        int          length     = 0;

        long         addr     = -1;
        char*        filename = NULL;
        char*        funcname = NULL;
        unsigned int line_no  = SILC_INVALID_LINE_NO;

        if ( strlen( line ) == 0 || line[ 0 ] == ' ' )
        {
            continue;
        }

        if ( line[ strlen( line ) - 1 ] == '\n' )
        {
            line[ strlen( line ) - 1 ] = '\0';
        }

        /* split line to columns */
        col = strtok( line, delim );
        do
        {
            if ( col_num == 0 ) /* column 1 (address) */
            {
                length = strlen( col );
                addr   = strtol( col + length - sizeof( void* ) * 2, NULL, 16 );
                if ( addr == 0 )
                {
                    break;
                }
            }
            else if ( col_num == 1 ) /* column 2 (type) */
            {
                strcpy( delim, "\t" );
            }
            else if ( col_num == 2 ) /* column 3 (symbol) */
            {
                funcname = col;
                strcpy( delim, ":" );
            }
            else if ( col_num == 3 ) /* column 4 (filename) */
            {
                filename = col;
            }
            else /* column 5 (line number) */
            {
                line_no = atoi( col );
                if ( line_no == 0 )
                {
                    line_no = SILC_INVALID_LINE_NO;
                }
                break;
            }

            col_num++;
        }
        while ( ( col = strtok( 0, delim ) ) );

        /* add symbol to hash table */
        if ( col_num >= 3 )
        {
            char* region_name = strdup( funcname );
            silc_compiler_hash_put( addr, region_name, filename, line_no );
        }
    }

    /* close nm-file */
    fclose( nmfile );
    remove( nmfilename );
}

#else /* HAVE_LIBBFD / HAVE_NM */

/* ***************************************************************************************
   dummy implememtation of symbol table analysis
*****************************************************************************************/

#warning Neither BFD nor nm are available. Thus, the symbol table can not be analyzed.
#warning The GNU compiler adapter will be disabled.

/**
   Dummy implementation of symbol table analysis for the case that neither BFD
   nor nm are available. It allows to compile without error, but the compiler adapter
   is will not generate events, though the compiler instrumented the code, because
   it can not map the function pointers to names.
 */
static void
silc_compiler_get_sym_tab( void )
{
}

#endif /* HAVE_LIBBFD / HAVE_NM */


/* ***************************************************************************************
   Implementation of functions called by compiler instrumentation
*****************************************************************************************/

/**
 * @brief This function is called just after the entry of a function
 * generated by the GNU compiler.
 * @param func      The address of the start of the current function.
 * @param callsice  The call site of the current function.
 */
void
__cyg_profile_func_enter( void* func,
                          void* callsite )
{
    silc_compiler_hash_node* hash_node;

    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "call at function enter." );

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
__cyg_profile_func_exit( void* func,
                         void* callsite )
{
    silc_compiler_hash_node* hash_node;
    SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, "call function exit." );
    if ( hash_node = silc_compiler_hash_get( ( long )func ) )
    {
        SILC_ExitRegion( hash_node->region_handle );
    }
}

/* ***************************************************************************************
   Adapter management
*****************************************************************************************/

SILC_Error_Code
silc_compiler_init_adapter()
{
    if ( silc_compiler_initialize )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " inititialize GNU compiler adapter." );

        /* Initialize hash tables */
        silc_compiler_hash_init();

        /* call function to calculate symbol table */
        silc_compiler_get_sym_tab();

        /* Sez flag */
        silc_compiler_initialize = 0;

        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER,
                           " inititialization of GNU compiler adapter done." );
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
        SILC_DEBUG_PRINTF( SILC_DEBUG_COMPILER, " finalize GNU compiler adapter." );
    }
}
