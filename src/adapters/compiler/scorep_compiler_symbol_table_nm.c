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
 * Copyright (c) 2009-2015,
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
 * @brief Uses nm to iterate over the symbol table of an executable.
 */

#include <config.h>

#include "scorep_compiler_symbol_table.h"

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>
#include <UTILS_IO.h>

#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Types.h>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

/* *INDENT-OFF* */
static bool create_nm_file( char* nmfile, const char* exefile );
/* *INDENT-ON* */

/* ***************************************************************************************
   nm based symbol table analysis
*****************************************************************************************/

/**
 * Get symbol table by parsing the output from nm. Stores all functions obtained
 * from the symbol table
 * in a hashtable. The key of the hashtable is the function pointer. This must be done
 * during initialization of the GNU compiler adapter, because enter and exit events
 * provide only a file pointer.
 * It also collects information about source file and line number.
 */
void
scorep_compiler_process_symbol_table( const char*                       executable,
                                      scorep_compiler_process_symbol_cb processSymbol )
{
    UTILS_DEBUG_ENTRY();
    /* create nm-file */
    char*  file_prefix = "scorep_nm_file.";
    size_t length      = strlen( file_prefix ) + 20 + 1;     /* UINT64_MAX consumes 20 char */
    char   nm_filename[ length ];
    snprintf( nm_filename, length, "%s%" PRIu64, file_prefix, SCOREP_Timer_GetClockTicks() );
    UTILS_DEBUG( "Reading symbol table from generated \"%s\" using nm.",
                 nm_filename );
    if ( !create_nm_file( nm_filename, executable ) )
    {
        return;
    }
    scorep_compiler_parse_nm_file( nm_filename, processSymbol );
    remove( nm_filename );
}


/**
 * Write output from nm for @a exefile to @a nmfile.
 * @param exefile Filename of the executable which is analyzed.
 * @param nmfile  Filename of the file to which the output is written.
 * @returns true if the nm output was created successfully, else it returns false.
 */
static bool
create_nm_file( char*       nmfile,
                const char* exefile )
{
    char* errfile = malloc( strlen( nmfile ) + 5 );
    UTILS_ASSERT( errfile );
    sprintf( errfile, "%s_err", nmfile );

    char* command = malloc( strlen( nmfile )  +
                            strlen( errfile ) +
                            strlen( exefile ) +
                            strlen( SCOREP_BACKEND_NM ) + 15 );
    UTILS_ASSERT( exefile );

#if HAVE( PLATFORM_MAC )
    sprintf( command, SCOREP_BACKEND_NM " -A %s 2> %s > %s", exefile, errfile, nmfile );
#else
    sprintf( command, SCOREP_BACKEND_NM " -l %s 2> %s > %s", exefile, errfile, nmfile );
#endif /* ! HAVE( PLATFORM_MAC ) */

    if ( system( command ) != EXIT_SUCCESS )
    {
        UTILS_ERROR( SCOREP_ERROR_ON_SYSTEM_CALL,
                     "Failed to get symbol table output using following command: "
                     "\'%s\'. Function enter/exit will not be recorded. Consider "
                     "providing the symbol table output in a file by exporting "
                     "SCOREP_NM_SYMBOLS",
                     command );
        free( errfile );
        free( command );
        return false;
    }
    remove( errfile );
    free( errfile );
    free( command );
    return true;
}
