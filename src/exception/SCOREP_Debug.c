/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @status          alpha
 * @file            SCOREP_Debug.c
 * @maintainer      Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @ingroup         MANGLE_NAME( Exception_module )
 *
 * @brief           Implementation of debug output handling in SCOREP.
 *
 * @author          Dominic Eschweiler <d.eschweiler@fz-juelich.de>
 * @created         2009-05-08
 */


#include <config.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>


#include <SCOREP_Debug.h>
#include <SCOREP_Error.h>


#include "scorep_normalize_file.h"


static uint32_t initFlag   = 0;
static uint64_t debugLevel = 0;


/**
 * This is the init function for SCOREP_DebugPrintf(), which is mainly used to
 * collect all needed information from the system and warn that the debug mode
 * was set on compile time.
 *
 * @returns MANGLE_NAME( SUCCESS ) on success.
 *
 */
static inline uint64_t
debug_init()
{
    if ( initFlag == 0 )
    {
        initFlag = 1;

        const char* debug_level_string;
        debug_level_string = getenv( "MANGLE_NAME( DEBUG )" );

        if ( debug_level_string )
        {
            debugLevel = atoi( debug_level_string );
        }
        else
        {
            debugLevel = 0;
        }

        if ( debugLevel == 0 )
        {
            fprintf(
                stderr,
                "WARNING : you have compiled the binary with debugging " \
                "support (you can set the level with the environment " \
                "variable MANGLE_NAME( DEBUG ))\n" );
        }
        else
        {
            fprintf(
                stderr,
                "WARNING : you have compiled the binary with debugging " \
                "support (current debug level is %lu)\n",
                ( unsigned long )debugLevel );
        }
    }

    return MANGLE_NAME( SUCCESS );
}


void
MANGLE_NAME( Debug_Printf )
(
    uint64_t bitMask,
    const char* package,
    const char* srcdir,
    const char* builddir,
    const char* file,
    uint64_t line,
    const char* function,
    const char* msgFormatString,
    ...
)
{
    debug_init();

    uint64_t kind = bitMask & ( MANGLE_NAME( DEBUG_FUNCTION_ENTRY ) | MANGLE_NAME( DEBUG_FUNCTION_EXIT ) );
    bitMask &= ~( MANGLE_NAME( DEBUG_FUNCTION_ENTRY ) | MANGLE_NAME( DEBUG_FUNCTION_EXIT ) );

    if ( ( debugLevel & bitMask ) != bitMask )
    {
        return;
    }

    assert( kind != ( MANGLE_NAME( DEBUG_FUNCTION_ENTRY ) | MANGLE_NAME( DEBUG_FUNCTION_EXIT ) ) );

    size_t msg_format_string_length = msgFormatString ?
                                      strlen( msgFormatString ) : 0;

    char* normalized_file = scorep_normalize_file( srcdir, builddir, file );

    if ( kind )
    {
        const char* kind_str = "Entering";
        if ( bitMask & MANGLE_NAME( DEBUG_FUNCTION_EXIT ) )
        {
            kind_str = "Leaving";
        }

        fprintf( stdout,
                 "[%s] %s:%" PRIu64 ": %s function '%s'%s",
                 package,
                 normalized_file,
                 line,
                 kind_str,
                 function,
                 msg_format_string_length ? ": " : "\n" );
    }
    else
    {
        fprintf( stdout,
                 "[%s] %s:%" PRIu64 "%s",
                 package,
                 normalized_file,
                 line,
                 msg_format_string_length ? ": " : "\n" );
    }

    free( normalized_file );

    if ( msg_format_string_length )
    {
        va_list va;
        va_start( va, msgFormatString );
        vfprintf( stdout, msgFormatString, va );
        fprintf( stdout, "\n" );
        va_end( va );
    }
}


void
MANGLE_NAME( Debug_RawPrintf )
(
    const uint64_t bitMask,
    const char*    msgFormatString,
    ...
)
{
    debug_init();

    if ( ( debugLevel & bitMask ) != bitMask )
    {
        return;
    }

    va_list va;
    va_start( va, msgFormatString );
    vfprintf( stdout, msgFormatString, va );
    va_end( va );
}


void
MANGLE_NAME( Debug_Prefix )
(
    const uint64_t bitMask,
    const char*    package,
    const char*    srcdir,
    const char*    builddir,
    const char*    file,
    const uint64_t line,
    const char*    function
)
{
    debug_init();

    if ( ( debugLevel & bitMask ) != bitMask )
    {
        return;
    }

    char* normalized_file = scorep_normalize_file( srcdir, builddir, file );

    fprintf( stdout,
             "[%s] %s:%" PRIu64 ": In function '%s': ",
             package,
             normalized_file,
             line,
             function );

    free( normalized_file );
}


void
MANGLE_NAME( Abort )
(
    const bool truthValue,
    const char*    message,
    const char*    package,
    const char*    srcdir,
    const char*    builddir,
    const char*    file,
    const uint64_t line,
    const char*    func
)
{
    if ( truthValue )
    {
        return;
    }

    MANGLE_NAME( Error_Handler ) ( package, srcdir, builddir, file, line, func,
                                   MANGLE_NAME( ERROR_ASSERTION_FAILED ),
                                   "%s. Aborting.", message );
    abort();
}
