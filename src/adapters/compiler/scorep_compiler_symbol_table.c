/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
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

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>
#include <UTILS_IO.h>

#include <SCOREP_Filter.h>

#include "scorep_compiler_data.h"

#define SCOREP_COMPILER_BUFFER_LEN 512

extern char* scorep_compiler_executable;


/* ***************************************************************************************
   Demangling declarations
*****************************************************************************************/

#if defined( GNU_DEMANGLE )
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

/* Demangeling style. */
static int scorep_compiler_demangle_style = SCOREP_COMPILER_DEMANGLE_PARAMS  |
                                            SCOREP_COMPILER_DEMANGLE_ANSI    |
                                            SCOREP_COMPILER_DEMANGLE_VERBOSE |
                                            SCOREP_COMPILER_DEMANGLE_TYPES;
#endif /* GNU_DEMANGLE */

/* ***************************************************************************************
   helper functions for symbaol table analysis
*****************************************************************************************/

/**
   Writes the path/filename of the executable into @a path. If the path is longer
   than the @a path, the last @a length characters of the path are written.
   @param path    A buffer into which the pat is written. The memory for the buffer
                  must be allocated already.
   @param length  The size of the buffer @a path. If the retrieved path/filename of
                  the executable is longer than @a length, the path's head is truncated.
   @retruns true if the path of the executable was obtained successfully. Else false is
                 returned.
 */
static bool
get_executable( char*  path,
                size_t length )
{
    int         pid;
    int         err;
    struct stat status;

    /**
     * If a user specified an executable, it overrides every default.
     * By default, use /proc mechanism to obtain path to executable.
     */

    /* First trial */
    /* try to get the path via environment variable */
    if ( strlen( scorep_compiler_executable ) > 0 )
    {
        char*  exepath   = scorep_compiler_executable;
        size_t exelength = strlen( exepath );
        if ( exelength > length )
        {
            /* If path is too long for buffer, truncate the head.
               add one charakter for the terminating 0.           */
            exepath   = scorep_compiler_executable + exelength - length + 1;
            exelength = length;
        }
        UTILS_DEBUG( "exepath = %s", exepath );

        strncpy( path, exepath, exelength );
        return true;
    }

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
        UTILS_DEBUG( "Meanwhile, you have to set the configuration variable"
                     "'executable' to your local executable." );
        UTILS_ERROR( SCOREP_ERROR_ENOENT, "Could not determine path of executable." );
        return false;
    }
}

static void
process_symbol( long         addr,
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

    const char* funcname_demangled = funcname;
#ifdef GNU_DEMANGLE
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
#endif  /* GNU_DEMANGLE */

    if ( ( strncmp( funcname_demangled, "POMP", 4 ) != 0 ) &&
         ( strncmp( funcname_demangled, "Pomp", 4 ) != 0 ) &&
         ( strncmp( funcname_demangled, "pomp", 4 ) != 0 ) &&
         ( strncmp( funcname_demangled, "SCOREP_", 7 ) != 0 ) &&
         ( strncmp( funcname_demangled, "scorep_", 7 ) != 0 ) &&
         ( strncmp( funcname_demangled, "OTF2_", 5 ) != 0 ) &&
         ( strncmp( funcname_demangled, "otf2_", 5 ) != 0 ) &&
         ( strncmp( funcname_demangled, "cube_", 5 ) != 0 ) &&
         ( !SCOREP_Filter_Match( path, funcname_demangled, funcname ) ) )
    {
        scorep_compiler_hash_put( addr, funcname, funcname_demangled, path, lno );
    }

    free( path );
}

void
scorep_compiler_load_symbols( void )
{
    char executable[ SCOREP_COMPILER_BUFFER_LEN ];

    if ( !get_executable( executable, SCOREP_COMPILER_BUFFER_LEN ) )
    {
        return;
    }

    scorep_compiler_process_symbol_table( executable,
                                          process_symbol );
}
