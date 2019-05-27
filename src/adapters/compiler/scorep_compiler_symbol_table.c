/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief Symbol table analysis functions.
 * Contains functions that read the symbol table of a executable and add all functions
 * found to a hashtable.
 */

#include <config.h>

#include "scorep_compiler_symbol_table.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>
#include <UTILS_IO.h>

#include <SCOREP_Filtering.h>
#include <scorep_environment.h>
#include <SCOREP_RuntimeManagement.h>

#include "scorep_compiler_data.h"

/* ***************************************************************************************
   Demangling declarations
*****************************************************************************************/

#if HAVE( DEMANGLE )
/* Declaration of external demangling function */
/* It is contained in "demangle.h" */
extern char*
cplus_demangle( const char* mangled,
                int         options );

/* cplus_demangle options */
#define SCOREP_COMPILER_DEMANGLE_NO_OPTS   0
#define SCOREP_COMPILER_DEMANGLE_PARAMS    ( 1 << 0 )  /* include function arguments */
#define SCOREP_COMPILER_DEMANGLE_ANSI      ( 1 << 1 )  /* include const, volatile, etc. */
#define SCOREP_COMPILER_DEMANGLE_VERBOSE   ( 1 << 3 )  /* include implementation details */
#define SCOREP_COMPILER_DEMANGLE_TYPES     ( 1 << 4 )  /* include type encodings */

/* Demangling style. */
static int scorep_compiler_demangle_style = SCOREP_COMPILER_DEMANGLE_PARAMS  |
                                            SCOREP_COMPILER_DEMANGLE_ANSI    |
                                            SCOREP_COMPILER_DEMANGLE_VERBOSE |
                                            SCOREP_COMPILER_DEMANGLE_TYPES;
#endif /* HAVE( DEMANGLE ) */

/* ***************************************************************************************
   helper functions for symbol table analysis
*****************************************************************************************/

static void
process_symbol( long         address,
                const char*  funcname,
                const char*  filename,
                unsigned int lno )
{
    char* path = NULL;
    if ( filename != NULL )
    {
        path = UTILS_CStr_dup( filename );
        UTILS_IO_SimplifyPath( path );
    }

#if HAVE( PLATFORM_MAC )
    /* Skip these leading underscores on macOS */
    if ( '_' == funcname[ 0 ] )
    {
        funcname++;
    }
#endif /*HAVE( PLATFORM_MAC )*/

    const char* funcname_demangled = funcname;
#if HAVE( DEMANGLE )
    /* use demangled name if possible */
    if ( scorep_compiler_demangle_style >= 0 )
    {
        funcname_demangled = cplus_demangle( funcname,
                                             scorep_compiler_demangle_style );

        if ( funcname_demangled == NULL )
        {
            funcname_demangled = funcname;
        }
    }
#endif /* HAVE( DEMANGLE ) */

    bool use_address = ( address != 0 );

    use_address &= ( strncmp( funcname_demangled, "POMP", 4 ) != 0 ) &&
                   ( strncmp( funcname_demangled, "Pomp", 4 ) != 0 ) &&
                   ( strncmp( funcname_demangled, "pomp", 4 ) != 0 ) &&
                   ( strncmp( funcname_demangled, "SCOREP_", 7 ) != 0 ) &&
                   ( strncmp( funcname_demangled, "scorep_", 7 ) != 0 ) &&
                   ( strncmp( funcname_demangled, "OTF2_", 5 ) != 0 ) &&
                   ( strncmp( funcname_demangled, "otf2_", 5 ) != 0 ) &&
                   ( strncmp( funcname_demangled, "cube_", 5 ) != 0 ) &&
                   ( strncmp( funcname_demangled, "cubew_", 6 ) != 0 ) &&
                   ( strncmp( funcname_demangled, ".omp.", 5 ) != 0 ) &&
                   ( strncmp( funcname_demangled, ".omp_outlined.", 14 ) != 0 ) &&
                   ( strncmp( funcname_demangled, ".nondebug_wrapper.", 18 ) != 0 );

    use_address &= ( !SCOREP_Filtering_Match( path, funcname_demangled, funcname ) );

    if ( path != NULL )
    {
        static int length = sizeof( SCOREP_ABS_TOPLEVEL_SRCDIR ) - 1;
        use_address &= ( strncmp( path, SCOREP_ABS_TOPLEVEL_SRCDIR, length ) != 0 );
    }

    if ( use_address )
    {
        scorep_compiler_hash_put( address, funcname, funcname_demangled, path, lno );
        UTILS_DEBUG( "hash table: added %p:%s:%s:%d", address, funcname_demangled, path, lno );
    }
    else
    {
        UTILS_DEBUG( "hash table: ignored %p:%s:%s:%d", address, funcname_demangled, path, lno );
    }

    free( path );
}


/**
 * Returns true if symbols (functions names) are provided, e.g., via file
 * using the SCOREP_NM_SYMBOLS environment variable.
 */
static bool
symbols_provided( void )
{
    extern char* scorep_compiler_nm_symbols;
    return strlen( scorep_compiler_nm_symbols ) > 0;
}


/**
 * @param nmFilename Filename of file that contains nm -l output.
 * @param processSymbol Callback for processing individual symbols.
 */
void
scorep_compiler_parse_nm_file( const char*                       nmFilename,
                               scorep_compiler_process_symbol_cb processSymbol )
{
    FILE* nm_file = fopen( nmFilename, "r" );
    if ( !nm_file )
    {
        UTILS_ERROR_POSIX( "Could not open file \"%s\". Function enter/exit will "
                           "not be recorded.", nmFilename );
        return;
    }

    /* read lines */
    size_t   line_size  = 0;
    char*    line       = NULL;
    unsigned nm_line_no = 0;
    while ( UTILS_IO_GetLine( &line, &line_size, nm_file ) == SCOREP_SUCCESS )
    {
        nm_line_no++;
        /* The '\n' in 'empty' lines counts as 1. */
        if ( strlen( line ) <= 1 || line[ 0 ] == ' ' )
        {
            UTILS_DEBUG( "ignore line %d (empty/leading whitespace) \'%s\'", nm_line_no, line );
            continue;
        }

        if ( line[ strlen( line ) - 1 ] == '\n' )
        {
            line[ strlen( line ) - 1 ] = '\0';
        }

        /* Split line to columns.
         * Expect line to be of type 'addr type funcname filename:line_no',
         * where the fields are whitespace-separated.
         *
         * On macOS lines are of type 'binary: addr type funcname'
         */
        char delim[ 2 ] = " ";
#if HAVE( PLATFORM_MAC )
        char delim_mac_binary_name[ 2 ] = ":";
#endif
        int   col_num  = 0;
        long  addr     = -1;
        char* filename = NULL;
        char* funcname = NULL;

#if HAVE( SCOREP_DEBUG )
        char orig_line[ 256 ];
        strncpy( orig_line, line, 256 );
        orig_line[ 255 ] = '\0';
#endif  /* HAVE( SCOREP_DEBUG ) */

#if HAVE( PLATFORM_MAC )
        /* Skip column containing binary name */
        char* col = strtok( line, delim_mac_binary_name );
        col = strtok( NULL, delim );
#else
        char* col = strtok( line, delim );
#endif  /* HAVE( PLATFORM_MAC ) */
        do
        {
            col_num++;
            if ( col_num == 1 ) /* column 1 (address, e.g. 00000000004026ff) */
            {
                //length = strlen( col );
                addr = strtol( col, NULL, 16 );
                if ( addr == 0 )
                {
                    UTILS_DEBUG( "ignore line %d (address) \'%s\'", nm_line_no, orig_line );
                    break;
                }
            }
            else if ( col_num == 2 ) /* column 2 (symbol type, e.g. T) */
            {
                bool ignore_symbol = false;
                switch ( col[ 0 ] /* symbol type */ )
                {
                    // Ignore non-function types
                    case 'B':
                    case 'G':
                    case 'R':
                    case 'S':
                    case 'U':
                    case 'b':
                    case 'g':
                    case 'r':
                    case 's':
                    case 'u':
                    {
                        ignore_symbol = true;
                        break;
                    }
                }
                if ( ignore_symbol )
                {
                    UTILS_DEBUG( "ignore line %d (symbol type \'%s\') \'%s\'", nm_line_no, col, orig_line );
                    break;
                }
                strcpy( delim, "\t" );
            }
            else if ( col_num == 3 ) /* column 3 (symbol, e.g. main) */
            {
                funcname = col;
                strcpy( delim,  "" ); /* get entire remainder at next strtok */
            }
            else /* column 4 (filename:line_no) */
            {
                filename = col;
                break;
            }
        }
        while ( ( col = strtok( 0, delim ) ) );

        if ( col_num == 4 )
        {
            /* extract filename and line_no from <filename>:<line_no> */
            unsigned line_no    = SCOREP_INVALID_LINE_NO;
            char*    last_colon = strrchr( filename, ':' );
            if ( last_colon != NULL )
            {
                line_no = atoi( last_colon + 1 );
                if ( line_no == 0 )
                {
                    line_no = SCOREP_INVALID_LINE_NO;
                }
                filename[ strlen( filename ) - strlen( last_colon ) ] = '\0';
            }
            processSymbol( addr, funcname, filename, line_no );
        }
        else if ( col_num == 3 )
        {
            /* no <filename>:<line_no> provided */
            /* C++ ctors/dtors addresses appear twice in nm -l output, the second
             * time without <filename>:<line_no>. But these will be the first
             * match in the hash table. */
            processSymbol( addr, funcname, "", SCOREP_INVALID_LINE_NO );
            UTILS_DEBUG( "line %d lacks filename \'%s\'", nm_line_no, orig_line );
        }
    }

    /* clean up */
    free( line );
    fclose( nm_file );
}


void
scorep_compiler_load_symbols( void )
{
    bool        executable_name_is_file;
    const char* executable = SCOREP_GetExecutableName( &executable_name_is_file );

    if ( symbols_provided() )
    {
        extern char* scorep_compiler_nm_symbols;
        UTILS_DEBUG( "Reading symbol table from provided file \"%s\".",
                     scorep_compiler_nm_symbols );
        scorep_compiler_parse_nm_file( scorep_compiler_nm_symbols,
                                       process_symbol );
    }
    else if ( executable_name_is_file )
    {
        scorep_compiler_process_symbol_table( executable, process_symbol );
    }
    else
    {
        UTILS_FATAL( "Cannot process symbol table. Please provide the executable, "
                     "preferably with full path, via SCOREP_EXECUTABLE." );
    }
}
