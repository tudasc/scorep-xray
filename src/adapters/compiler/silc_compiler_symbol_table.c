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
 * @status     alpha
 * @file       silc_compiler_symbol_table.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief Symbol table analysis functions.
 * Contains functions that read the symbol table of a executable and add all functions
 * found to a hashtable. For this it offers 2 possibilities:
 * @li A bfd based implementation is the best choice.
 * @li If no bfd is available, nm can be used instead. However, it involves system calls
 *     and scales worse.
 * @li If none of both is enabled, a dummy is compiled, which disables the adapter.
 *
 * It may be compiled with different defines:
 * @li If HAVE( READLINK ) is true, it tries to resolve the symbolic link in
 *     /proc/self/exe to find the executable. Else other methods are used.
 * @li If HAVE_LIBBFD is defined, it uses the bfd library to read the symbols.
 * @li If HAVE_NM is defined, it uses a system call to nm to read the symbols. This
 *     option is only compiler if HAVE_LIBBFD is undefined, because of worse scaling.
 * @li If GNU_DEMANGLE is defined it uses cplus_demangle() to demangle function names.
 * @li If INTEL_COMPILER is defined, regions do not use the address as key, but get a
 *     32 bit integer id as key.
 */

#include <config.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <config.h>

#ifdef HAVE_LIBBFD
#include <bfd.h>
#elif defined HAVE_NM
#include <SILC_Timing.h>
#endif /* HAVE_LIBBFD / HAVE_NM */

#include <SILC_Types.h>
#include <silc_utility/SILC_Utils.h>
#include <SILC_Events.h>
#include <SILC_Definitions.h>
#include <SILC_RuntimeManagement.h>

#include <SILC_Compiler_Init.h>
#include <silc_compiler_data.h>

#define SILC_COMPILER_BUFFER_LEN 512

extern char* silc_compiler_executable;

#ifdef INTEL_COMPILER
extern void
silc_compiler_name_add( const char* name,
                        int32_t     id );

#endif /* INTEL_COMPILER */

/* ***************************************************************************************
   Demangling declarations
*****************************************************************************************/

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
    int         pid;
    int         err;
    struct stat status;

    /**
     * by default, use /proc mechanism to obtain path to executable
     * in other cases, do it by examining SILC_APPPATH variable
     */

    /* First trial */
    /* Does not work this way. Must either use readlink (creates lots of
       problems with intel compilers) or leave it out.
       sprintf( path, "/proc/self/exe" );
       err = stat( path, &status );
       if ( err == 0 )
       {
        return true;
       }
     */

    /* Second trial */
    pid = getpid();
    sprintf( path, "/proc/%d/exe", pid );
    err = stat( path, &status );
    if ( err == 0 )
    {
        return true;
    }

    /* Third trial */
    sprintf( path, "/proc/%d/object/a.out", pid );
    err = stat( path, &status );
    if ( err == 0 )
    {
        return true;
    }
    else
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
void
silc_compiler_get_sym_tab( void )
{
#ifdef INTEL_COMPILER
    /* Counter for the last assigned region id.
       When using the Intel VT_ instrumentation, the functions provide a 32 bit storage.
       Thus, addresses may not fit, thus, we provide an other id as key.
     */
    int32_t   region_counter = 1;
#endif /* INTEL_COMPILER */
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

#ifdef INTEL_COMPILER
        silc_compiler_hash_put( region_counter, funcname, filename, lno );
        silc_compiler_name_add( funcname, region_counter );
        region_counter++;
#else
        silc_compiler_hash_put( addr, funcname, filename, lno );
#endif  /* INTEL_COMPILER */
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
void
silc_compiler_get_sym_tab( void )
{
#ifdef INTEL_COMPILER
    /* Counter for the last assigned region id.
       When using the Intel VT_ instrumentation, the functions provide a 32 bit storage.
       Thus, addresses may not fit, thus, we provide an other id as key.
     */
    int32_t region_counter = 1;
#endif /* INTEL_COMPILER */
    FILE*   nmfile;
    char    line[ 1024 ];
    char    path[ SILC_COMPILER_BUFFER_LEN ] = { 0 };
    char    nmfilename[ 64 ];

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
#ifdef INTEL_COMPILER
            silc_compiler_hash_put( region_counter, funcname, filename, line_no );
            silc_compiler_name_add( funcname, region_counter );
            region_counter++;
#else
            silc_compiler_hash_put( addr, funcname, filename, line_no );
#endif      /* INTEL_COMPILER */
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
#warning The compiler adapter will be disabled.

/**
   Dummy implementation of symbol table analysis for the case that neither BFD
   nor nm are available. It allows to compile without error, but the compiler adapter
   is will not generate events, though the compiler instrumented the code, because
   it can not map the function pointers to names.
 */
void
silc_compiler_get_sym_tab( void )
{
}

#endif /* HAVE_LIBBFD / HAVE_NM */
