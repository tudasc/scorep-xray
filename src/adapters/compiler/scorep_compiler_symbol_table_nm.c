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
 * @brief Uses nm to iterate over the symbol table of an exectubale.
 */

#include <config.h>

#include "scorep_compiler_symbol_table.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>
#include <UTILS_IO.h>

#include <SCOREP_Timing.h>
#include <SCOREP_Types.h>

/* ***************************************************************************************
   nm based symbol table analysis
*****************************************************************************************/

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

#ifdef GNU_DEMANGLE
    sprintf( command, SCOREP_BACKEND_NM " -Aol %s 2> %s > %s", exefile, errfile, nmfile );
#else /* GNU_DEMANGLE */
    sprintf( command, SCOREP_BACKEND_NM " -ol %s 2> %s > %s", exefile, errfile, nmfile );
#endif /* GNU_DEMANGLE */
    if ( system( command ) != EXIT_SUCCESS )
    {
        UTILS_ERROR( SCOREP_ERROR_ON_SYSTEM_CALL,
                     "Failed to get symbol table output using following command: %s",
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
    FILE*  nmfile;
    size_t line_size = 0;
    char*  line      = NULL;
    char   nmfilename[ 64 ];

    UTILS_DEBUG( "Read symbol table using nm" );

    /* open nm-file */
    snprintf( nmfilename, 64, "scorep_nm_file.%" PRIu64, SCOREP_GetClockTicks() );
    if ( !create_nm_file( nmfilename, executable ) )
    {
        return;
    }
    if ( !( nmfile = fopen( nmfilename, "r" ) ) )
    {
        UTILS_ERROR_POSIX();
    }

    /* read lines */
    while ( UTILS_IO_GetLine( &line, &line_size, nmfile ) == SCOREP_SUCCESS )
    {
        char* col;
        char  delim[ 2 ] = " ";
        int   col_num    = 0;
        int   length     = 0;

        long         addr     = -1;
        char*        filename = NULL;
        char*        funcname = NULL;
        unsigned int line_no  = SCOREP_INVALID_LINE_NO;

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
                    line_no = SCOREP_INVALID_LINE_NO;
                }
                break;
            }

            col_num++;
        }
        while ( ( col = strtok( 0, delim ) ) );

        if ( col_num >= 3 )
        {
            processSymbol( addr, funcname, filename, line_no );
        }
    }

    /* clean up */
    free( line );
    fclose( nmfile );
    remove( nmfilename );
}
